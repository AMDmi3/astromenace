/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../graphics/graphics.h"
#include "../math/math.h"
#include "../vfs/vfs.h"
#include "model3d.h"


//-----------------------------------------------------------------------------
// загрузка "родного" формата
//-----------------------------------------------------------------------------
bool cModel3D::ReadVW3D(const char *FileName)
{
	std::unique_ptr<sFILE> file = vw_fopen(FileName);
	if (!file)
		return false;

	size_t SizeB = strlen(FileName) + 1;
	Name = new char[SizeB];
	strcpy(Name, FileName);

	// "пропускаем" заголовок "VW3D"
	file->fread(&DrawObjectCount, 4, 1);

	// читаем, сколько объектов
	file->fread(&DrawObjectCount, sizeof(DrawObjectCount), 1);

	DrawObjectList = new sObjectBlock[DrawObjectCount];

	GlobalIndexCount = 0;

	// для каждого объекта
	for (int i = 0; i < DrawObjectCount; i++) {
		DrawObjectList[i].RangeStart = GlobalIndexCount;

		// VertexFormat
		file->fread(&(DrawObjectList[i].VertexFormat), sizeof(DrawObjectList[0].VertexFormat), 1);
		// VertexStride
		file->fread(&(DrawObjectList[i].VertexStride), sizeof(DrawObjectList[0].VertexStride), 1);
		// VertexCount на самом деле, это кол-во индексов на объект
		file->fread(&(DrawObjectList[i].VertexCount), sizeof(DrawObjectList[0].VertexCount), 1);
		GlobalIndexCount += DrawObjectList[i].VertexCount;

		// Location
		file->fread(&(DrawObjectList[i].Location), sizeof(DrawObjectList[0].Location.x) * 3, 1);
		// Rotation
		file->fread(&(DrawObjectList[i].Rotation), sizeof(DrawObjectList[0].Rotation.x) * 3, 1);

		// рисуем нормально, не прозрачным
		DrawObjectList[i].DrawType = 0;

		// вертексный буфер
		DrawObjectList[i].NeedDestroyDataInObjectBlock = false;
		DrawObjectList[i].VertexBuffer = nullptr;
		DrawObjectList[i].VBO = nullptr;
		// индексный буфер
		DrawObjectList[i].IndexBuffer = nullptr;
		DrawObjectList[i].IBO = nullptr;
		// vao
		DrawObjectList[i].VAO = nullptr;
	}

	// получаем сколько всего вертексов
	file->fread(&GlobalVertexCount, sizeof(GlobalVertexCount), 1);

	// собственно данные (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	GlobalVertexBuffer = new float[GlobalVertexCount * DrawObjectList[0].VertexStride];
	file->fread(GlobalVertexBuffer,	GlobalVertexCount * DrawObjectList[0].VertexStride * sizeof(GlobalVertexBuffer[0]), 1);

	// индекс буфер
	GlobalIndexBuffer = new unsigned int[GlobalIndexCount];
	file->fread(GlobalIndexBuffer, GlobalIndexCount * sizeof(GlobalIndexBuffer[0]), 1);

	// т.к. наши объекты используют глобальные буферы, надо поставить указатели
	for (int i = 0; i < DrawObjectCount; i++) {
		DrawObjectList[i].VertexBuffer = GlobalVertexBuffer;
		DrawObjectList[i].IndexBuffer = GlobalIndexBuffer;
	}

	vw_fclose(file);

	return true;
}

//-----------------------------------------------------------------------------
// запись "родного" формата на диск
//-----------------------------------------------------------------------------
bool cModel3D::WriteVW3D(const char *FileName)
{
	// небольшие проверки
	if (!GlobalVertexBuffer || !GlobalIndexBuffer || !DrawObjectList) {
		std::cerr << __func__ << "(): " << "Can't create " << FileName << " file for empty Model3D.\n";
		return false;
	}

	SDL_RWops *FileVW3D;
	FileVW3D = SDL_RWFromFile(FileName, "wb");
	// если не можем создать файл на запись - уходим
	if (!FileVW3D) {
		std::cerr << __func__ << "(): " << "Can't create " << FileName << " file on disk.\n";
		return false;
	}

	// маркер файла 4 байта
	char tmp1[5] = "VW3D";
	SDL_RWwrite(FileVW3D, tmp1, 4, 1);

	// общее кол-во объектов в моделе - 4 байта (int)
	SDL_RWwrite(FileVW3D, &DrawObjectCount, sizeof(DrawObjectCount), 1);

	// для каждого объекта в моделе
	for (int i = 0; i < DrawObjectCount; i++) {
		// VertexFormat
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexFormat, sizeof(DrawObjectList[0].VertexFormat), 1);
		// VertexStride
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexStride, sizeof(DrawObjectList[0].VertexStride), 1);
		// VertexCount
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].VertexCount, sizeof(DrawObjectList[0].VertexCount), 1);

		// Location
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Location, sizeof(DrawObjectList[0].Location.x)*3, 1);
		// Rotation
		SDL_RWwrite(FileVW3D, &DrawObjectList[i].Rotation, sizeof(DrawObjectList[0].Rotation.x)*3, 1);
	}

	// записываем реальное кол-во вертексов в общем вертекс буфере, мы их посчитали заранее
	SDL_RWwrite(FileVW3D, &GlobalVertexCount, sizeof(GlobalVertexCount), 1);

	// данные, вертексы (берем смещение нулевого объекта, т.к. смещение одинаковое на весь объект)
	SDL_RWwrite(FileVW3D, GlobalVertexBuffer,
		    DrawObjectList[0].VertexStride * GlobalVertexCount * sizeof(GlobalVertexBuffer[0]), 1);

	// данные, индексный буфер
	SDL_RWwrite(FileVW3D, GlobalIndexBuffer, GlobalIndexCount * sizeof(GlobalIndexBuffer[0]), 1);

	// закрываем файл
	SDL_RWclose(FileVW3D);

	std::cout << "VW3D Write: " << FileName << "\n";
	return true;
}
