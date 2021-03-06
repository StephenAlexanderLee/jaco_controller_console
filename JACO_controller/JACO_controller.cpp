// JACO_controller.cpp
// For the control and use of the JACO v2 robotic arm for PNS stimulation using FUS
// Written by: Stephen Alexander Lee 
// 8-21-2018
//
#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <Windows.h>
#include "CommunicationLayerWindows.h"
#include "CommandLayer.h"
#include "KinovaTypes.h"

using namespace std;

HINSTANCE commandLayer_handle;

//Function pointers to the functions we need
int(*MyInitAPI)();
int(*MyCloseAPI)();
int(*MyStartForceControl)();
int(*MyStopForceControl)();
int(*MyGetDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
int(*MySetActiveDevice)(KinovaDevice device);
int(*MyGetGlobalTrajectoryInfo)(TrajectoryFIFO &Response);
int(*MyRunGravityZEstimationSequence)(ROBOT_TYPE type, double OptimalzParam[OPTIMAL_Z_PARAM_SIZE]);
int(*MySendBasicTrajectory)(TrajectoryPoint command);
int(*MyMoveHome)();
int(*MyInitFingers)();
int(*MyGetCartesianCommand)(CartesianPosition &);

int main(int argc, char* argv[])
{
	int programResult = 0;
	int result;
	commandLayer_handle = LoadLibrary(L"CommandLayerWindows.dll");

	CartesianPosition currentCommand;
	CartesianPosition point1;
	CartesianPosition point2;

	MyInitAPI = (int(*)()) GetProcAddress(commandLayer_handle, "InitAPI");
	MyCloseAPI = (int(*)()) GetProcAddress(commandLayer_handle, "CloseAPI");
	MyStartForceControl = (int(*)()) GetProcAddress(commandLayer_handle, "StartForceControl");
	MyStopForceControl = (int(*)()) GetProcAddress(commandLayer_handle, "StopForceControl");
	MyGetDevices = (int(*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) GetProcAddress(commandLayer_handle, "GetDevices");
	MySetActiveDevice = (int(*)(KinovaDevice devices)) GetProcAddress(commandLayer_handle, "SetActiveDevice");
	MyGetGlobalTrajectoryInfo = (int(*)(TrajectoryFIFO&)) GetProcAddress(commandLayer_handle, "GetGlobalTrajectoryInfo");
	MyRunGravityZEstimationSequence = (int(*)(ROBOT_TYPE type, double OptimalzParam[OPTIMAL_Z_PARAM_SIZE])) GetProcAddress(commandLayer_handle, "RunGravityZEstimationSequence");
	MySendBasicTrajectory = (int(*)(TrajectoryPoint)) GetProcAddress(commandLayer_handle, "SendBasicTrajectory");
	MyGetCartesianCommand = (int(*)(CartesianPosition &)) GetProcAddress(commandLayer_handle, "GetCartesianCommand");
	MyMoveHome = (int(*)()) GetProcAddress(commandLayer_handle, "MoveHome");
	MyInitFingers = (int(*)()) GetProcAddress(commandLayer_handle, "InitFingers");

	if ((MyInitAPI == NULL) || (MyCloseAPI == NULL)
		|| (MySetActiveDevice == NULL) || (MyGetDevices == NULL) || 
		(MySendBasicTrajectory == NULL) || (MyGetCartesianCommand == NULL) || 
		(MyMoveHome == NULL) || (MyInitFingers = NULL))

	{
		cout << "* * *  E R R O R   D U R I N G   I N I T I A L I Z A T I O N  * * *" << endl;
		programResult = 0;
	}
	else
	{
		cout << "I N I T I A L I Z A T I O N   C O M P L E T E D" << endl << endl;

		result = (*MyInitAPI)();

		cout << "Initialization's result :" << result << endl;

		KinovaDevice list[MAX_KINOVA_DEVICE];

		TrajectoryFIFO info;
		TrajectoryPoint pointToSend;
		pointToSend.InitStruct();

		int devicesCount = MyGetDevices(list, result);

		for (int i = 0; i < devicesCount; i++)
		{
			cout << "Found a robot on the USB bus (" << list[i].SerialNumber << ")" << endl;

			//Setting the current device as the active device.
			MySetActiveDevice(list[i]);
			int initialized = 1;
			int c;
			MyGetCartesianCommand(point1);
			MyGetCartesianCommand(point2);
			while (initialized != 0)
			{
				cout << endl << "1: Freely jog the robot by hand\n" <<
					"2: Lock the robot in place\n" << 
					"3: Set Position 1\n" << 
					"4: Set Position 2\n" <<
					"5: Display Position 1 and 2\n" <<
					"6: Move to Point 1\n" <<
					"7: Move to Point 2\n" <<
					"8: Disconnect Robot\n" << endl;
				cin >> c;
				switch (c) 
				{
					case 1: cout << "Free Move\n";
						MyStartForceControl();
						break;
					case 2: cout << "Lock Robot\n";
						MyStopForceControl();
						break;
					case 3: MyGetCartesianCommand(currentCommand);
						MyGetCartesianCommand(point1);
						cout << currentCommand.Coordinates.X << endl <<
							currentCommand.Coordinates.Y << endl <<
							currentCommand.Coordinates.Z << endl;
						break;
					case 4: MyGetCartesianCommand(currentCommand);
						MyGetCartesianCommand(point2);
						cout << currentCommand.Coordinates.X << endl <<
							currentCommand.Coordinates.Y << endl <<
							currentCommand.Coordinates.Z << endl;
						break;
					case 5: cout << "Point 1 Coordinates: \n" << point1.Coordinates.X << endl <<
						point1.Coordinates.Y << endl <<
						point1.Coordinates.Z << endl;
						cout << "Point 2 Coordinates: \n" << point2.Coordinates.X << endl <<
							point2.Coordinates.Y << endl <<
							point2.Coordinates.Z << endl;
						break;
					case 6: cout << "Moving to Point 1..." << endl;
						pointToSend.Position.CartesianPosition.X = point1.Coordinates.X;
						pointToSend.Position.CartesianPosition.Y = point1.Coordinates.Y;
						pointToSend.Position.CartesianPosition.Z = point1.Coordinates.Z;
						pointToSend.Position.CartesianPosition.ThetaX = point1.Coordinates.ThetaX;
						pointToSend.Position.CartesianPosition.ThetaY = point1.Coordinates.ThetaY;
						pointToSend.Position.CartesianPosition.ThetaZ = point1.Coordinates.ThetaZ;
						MySendBasicTrajectory(pointToSend);
						break;
					case 7: cout << "Moving to Point 2..." << endl;
						pointToSend.Position.CartesianPosition.X = point2.Coordinates.X;
						pointToSend.Position.CartesianPosition.Y = point2.Coordinates.Y;
						pointToSend.Position.CartesianPosition.Z = point2.Coordinates.Z;
						pointToSend.Position.CartesianPosition.ThetaX = point2.Coordinates.ThetaX;
						pointToSend.Position.CartesianPosition.ThetaY = point2.Coordinates.ThetaY;
						pointToSend.Position.CartesianPosition.ThetaZ = point2.Coordinates.ThetaZ;
						MySendBasicTrajectory(pointToSend);
						break;
					case 8: initialized = 0;
				}
			}
		}


		cout << endl << "C L O S I N G   A P I" << endl;
		result = (*MyCloseAPI)();
		programResult = 1;
	}

	FreeLibrary(commandLayer_handle);
	Sleep(2000);
	return programResult;
}

