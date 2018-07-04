/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "ground_object.h"
#include "../../assets/texture.h"
#include "../weapon/weapon.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

struct sWheeledData {
	float Strength;
	int WeaponQuantity;
	float SpeedToRotate;
	std::string Model3DFileName;
	std::string TextureFileName;
};

const std::vector<sWheeledData> PresetWheeledData{
	{120,	4,	50.0f,	"models/wheeled/jeep-01.vw3d", "models/gr-02.vw2d"},
	{70,	2,	50.0f,	"models/wheeled/jeep-02.vw3d", "models/gr-02.vw2d"},
	{100,	2,	50.0f,	"models/wheeled/jeep-03.vw3d", "models/gr-02.vw2d"},
	{100,	1,	50.0f,	"models/wheeled/jeep-04.vw3d", "models/gr-07.vw2d"},
	{150,	1,	50.0f,	"models/wheeled/jeep-05.vw3d", "models/gr-03.vw2d"},
	{250,	1,	30.0f,	"models/wheeled/apc-02.vw3d", "models/gr-03.vw2d"},
	{200,	2,	30.0f,	"models/wheeled/apc-04.vw3d", "models/gr-01.vw2d"},
	{400,	2,	34.0f,	"models/wheeled/r-launcher-01.vw3d", "models/gr-02.vw2d"}
};


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cWheeled::cWheeled(int WheeledNum)
{
	if ((WheeledNum <= 0) || (static_cast<unsigned>(WheeledNum) > PresetWheeledData.size())) {
		std::cerr << __func__ << "(): " << "Could not init cWheeled object with Number " << WheeledNum << "\n";
		return;
	}

	ObjectType = eObjectType::PirateVehicle;
	InternalType = WheeledNum;
	MaxSpeed = 20.0f;
	MaxAcceler = 5.0f;
	MaxSpeedRotate = 20.0f;

	LoadObjectData(PresetWheeledData[WheeledNum - 1].Model3DFileName, this);

	for (unsigned int i = 0; i < Model3DBlocks.size(); i++) {
		Texture[i] = GetPreloadedTextureAsset(PresetWheeledData[WheeledNum - 1].TextureFileName);
	}
	ResistanceHull = 1.0f;
	ResistanceSystems = 1.0f;
	SpeedToRotate = PresetWheeledData[WheeledNum - 1].SpeedToRotate;

	Strength = StrengthStart = PresetWheeledData[WheeledNum - 1].Strength/GameEnemyArmorPenalty;

	WeaponQuantity = PresetWheeledData[WheeledNum - 1].WeaponQuantity;
	// начальные установки для оружия
	WeaponSetFire = new bool[WeaponQuantity];
	WeaponLocation = new sVECTOR3D[WeaponQuantity];
	Weapon = new cWeapon*[WeaponQuantity];
	WeaponBound = new sVECTOR3D[WeaponQuantity];
	for (int i = 0; i < WeaponQuantity; i++) {
		WeaponSetFire[i] = false;
		Weapon[i] = nullptr;
	}

	// установка доп. текстуры и других настроек для каждой модели
	switch (WheeledNum) {
	case 1:
		WeaponLocation[0] = sVECTOR3D(0.3f, 4.5f, -1.5f);
		Weapon[0] = new cWeapon(206);
		WeaponLocation[1] = sVECTOR3D(-0.3f, 4.5f, -1.5f);
		Weapon[1] = new cWeapon(206);
		WeaponLocation[2] = sVECTOR3D(0.9f, 4.5f, -1.5f);
		Weapon[2] = new cWeapon(206);
		WeaponLocation[3] = sVECTOR3D(-0.9f, 4.5f, -1.5f);
		Weapon[3] = new cWeapon(206);
		WeaponFireType = 3;

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 0;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 3;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 30.0f;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 5;
		TargetVertBlocksMaxAngle = 30.0f;
		break;

	case 2:
		WeaponLocation[0] = sVECTOR3D(1.3f, 3.5f, -1.5f);
		Weapon[0] = new cWeapon(206);
		WeaponLocation[1] = sVECTOR3D(-1.3f, 3.5f, -1.5f);
		Weapon[1] = new cWeapon(206);
		WeaponFireType = 3;
		DoNotCalculateRotation = true;

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 20.0f;
		break;

	case 3:
		WeaponLocation[0] = sVECTOR3D(0.8f, 4.2f, -1.0f);
		Weapon[0] = new cWeapon(204);
		WeaponLocation[1] = sVECTOR3D(-0.8f, 4.2f, -1.0f);
		Weapon[1] = new cWeapon(204);
		WeaponFireType = 2;

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 0;
		WheelObjectsNum[1] = 1;
		WheelObjectsNum[2] = 2;
		WheelObjectsNum[3] = 5;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 0;
		WheelRotateObjectsNum[1] = 1;
		MaxWheelRotateAngle = 20.0f;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 3;
		TargetHorizBlocks[1] = 6;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 6;
		TargetVertBlocksMaxAngle = 60.0f;
		break;

	case 4:
		WeaponLocation[0] = sVECTOR3D(0.0f, 4.5f, -4.0f);
		Weapon[0] = new cWeapon(207);

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 0;
		WheelObjectsNum[1] = 1;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 0;
		WheelRotateObjectsNum[1] = 3;
		MaxWheelRotateAngle = 20.0f;
		break;

	case 5:
		WeaponLocation[0] = sVECTOR3D(0.0f, 4.5f, -4.0f);
		Weapon[0] = new cWeapon(208);

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 2;
		MaxWheelRotateAngle = 30.0f;
		break;

	case 6:
		WeaponLocation[0] = sVECTOR3D(0.0f, 6.0f, -3.0f);
		Weapon[0] = new cWeapon(204);

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 30.0f;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 5;
		TargetHorizBlocks[1] = 6;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 6;
		TargetVertBlocksMaxAngle = 40.0f;
		break;

	case 7:
		WeaponLocation[0] = sVECTOR3D(0.1f, 5.0f, -1.0f);
		Weapon[0] = new cWeapon(204);
		WeaponLocation[1] = sVECTOR3D(-0.1f, 5.0f, -1.0f);
		Weapon[1] = new cWeapon(204);
		WeaponFireType = 2;

		WheelQuantity = 4;
		WheelObjectsNum = new int[4];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 2;
		WheelObjectsNum[2] = 3;
		WheelObjectsNum[3] = 4;

		WheelRotateQuantity = 2;
		WheelRotateObjectsNum = new int[2];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 4;
		MaxWheelRotateAngle = 20.0f;

		TargetHorizBlocksQuantity = 2;
		TargetHorizBlocks = new int[TargetHorizBlocksQuantity];
		TargetHorizBlocks[0] = 5;
		TargetHorizBlocks[1] = 6;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 6;
		TargetVertBlocksMaxAngle = 40.0f;
		break;

	case 8:
		WeaponLocation[0] = sVECTOR3D(1.5f, 5.2f, 7.0f);
		Weapon[0] = new cWeapon(210);
		WeaponLocation[1] = sVECTOR3D(-1.5f, 5.2f, 7.0f);
		Weapon[1] = new cWeapon(210);
		WeaponFireType = 3;

		WheelQuantity = 8;
		WheelObjectsNum = new int[8];
		WheelObjectsNum[0] = 1;
		WheelObjectsNum[1] = 3;
		WheelObjectsNum[2] = 4;
		WheelObjectsNum[3] = 5;
		WheelObjectsNum[4] = 6;
		WheelObjectsNum[5] = 7;
		WheelObjectsNum[6] = 8;
		WheelObjectsNum[7] = 9;

		WheelRotateQuantity = 4;
		WheelRotateObjectsNum = new int[4];
		WheelRotateObjectsNum[0] = 1;
		WheelRotateObjectsNum[1] = 3;
		WheelRotateObjectsNum[2] = 6;
		WheelRotateObjectsNum[3] = 7;
		MaxWheelRotateAngle = 20.0f;

		TargetVertBlocksQuantity = 1;
		TargetVertBlocks = new int[TargetVertBlocksQuantity];
		TargetVertBlocks[0] = 2;
		TargetVertBlocksMaxAngle = 90.0f;
		break;
	}

	// установка остальных параметров девиации
	DeviationOn = false;
	DeviationObjQuantity = WheelQuantity;
	Deviation = new sVECTOR3D[DeviationObjQuantity];
	NeedDeviation = new float[DeviationObjQuantity];
	CurentDeviation = new float[DeviationObjQuantity];
	CurentDeviationSum = new float[DeviationObjQuantity];
	DeviationObjNum = new int[DeviationObjQuantity];

	for (int i = 0; i < DeviationObjQuantity; i++) {
		Deviation[i] = sVECTOR3D(0.0f, 1.0f, 0.0f);
		NeedDeviation[i] = vw_fRand0() * 0.1f;
		CurentDeviation[i] = CurentDeviationSum[i] = 0.0f;
		DeviationObjNum[i] = WheelObjectsNum[i];
	}

	// делаем сдвиг поворота колес, чтобы смотрелось естественнее
	for (int i = 0; i < WheelQuantity; i++)
		Model3DBlocks[WheelObjectsNum[i]].Rotation.x = vw_fRandNum(360.0f);

	// вычисляем данные для нахождения точки стрельбы
	if (TargetHorizBlocks != nullptr)
		BaseBound = Model3DBlocks[TargetHorizBlocks[0]].Location;

	if (TargetVertBlocks != nullptr) {
		if (TargetHorizBlocks != nullptr) {
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location - Model3DBlocks[TargetHorizBlocks[0]].Location;
		} else {
			MiddleBound = Model3DBlocks[TargetVertBlocks[0]].Location;
		}
	}

	if (WeaponBound != nullptr) {
		for (int i = 0; i < WeaponQuantity; i++) {
			if (TargetVertBlocks != nullptr)
				WeaponBound[i] = WeaponLocation[i] - Model3DBlocks[TargetVertBlocks[0]].Location;
			else if (TargetHorizBlocks != nullptr)
				WeaponBound[i] = WeaponLocation[i] - Model3DBlocks[TargetHorizBlocks[0]].Location;
			else
				WeaponBound[i] = WeaponLocation[i];
		}
	}
}

} // astromenace namespace
} // viewizard namespace