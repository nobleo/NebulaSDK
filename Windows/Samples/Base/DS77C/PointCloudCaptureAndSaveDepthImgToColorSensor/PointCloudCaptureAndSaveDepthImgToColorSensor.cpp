﻿#include <thread>
#include <iostream>
#include "VzenseNebula_api.h"
#include <fstream>
#define frameSpace 20

using namespace std;

int main() {
	cout << "---PointCloudCaptureAndSaveDepthImgToColorSensor---"<< endl;

	//about dev
	uint32_t deviceCount;
	VzDeviceInfo* pDeviceListInfo = NULL;
	VzDeviceHandle deviceHandle = 0;
	VzReturnStatus status = VzRetOthers;
	
	//about frame
	
	VzFrameReady FrameReady = {0};
	VzFrame frame = { 0 };

	//SDK Initialize
	status = VZ_Initialize();
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VzInitialize failed status:" <<status << endl;
		system("pause");
		return -1;
	}

	//1.Search and notice the count of devices.
	//2.get infomation of the devices. 
	//3.open devices accroding to the info.
GET:
	status = VZ_GetDeviceCount(&deviceCount);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VzGetDeviceCount failed! make sure the DCAM is connected" << endl;
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}
	cout << "Get device count: " << deviceCount << endl;
	if (0 == deviceCount)
	{
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}

	pDeviceListInfo = new VzDeviceInfo[deviceCount];
	status = VZ_GetDeviceInfoList(deviceCount, pDeviceListInfo);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "GetDeviceListInfo failed status:" << status << endl;
		return -1;
	}
	else
	{
		if (VzConnected != pDeviceListInfo[0].status)
		{
			cout << "connect statu" << pDeviceListInfo[0].status << endl;
			cout << "Call VZ_OpenDevice with connect status :" << VzConnected << endl;
			return -1;
		}
	}

	cout << "uri:" << pDeviceListInfo[0].uri << endl
	<< "alias:" << pDeviceListInfo[0].alias << endl
	<< "ip:" << pDeviceListInfo[0].ip << endl
	<< "connectStatus:" << pDeviceListInfo[0].status << endl;

	status = VZ_OpenDeviceByUri(pDeviceListInfo[0].uri, &deviceHandle);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "OpenDevice failed status:" <<status << endl;
		return false;
	}

    cout << "open device successful,status :" << status << endl;

	//Starts capturing the image stream
	status = VZ_StartStream(deviceHandle);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VZ_StartStream failed status:" <<status<< endl;
		return -1;
	}

	//switch RGBResolution
	int resolution_w = 800;
	int resolution_h = 600;
 	status = VZ_SetColorResolution(deviceHandle, resolution_w, resolution_h);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VZ_SetColorResolution failed status:" << status << endl;
		return -1;
	}
	else
	{
		cout << "set to 800_600" << endl;
	}
	//Wait for the device to upload image data
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//set Mapper
	status = VZ_SetTransformDepthImgToColorSensorEnabled(deviceHandle, true);
	if (status == VzRetOK)
	{
		cout << "VZ_SetTransformDepthImgToColorSensorEnabled Enabled" << endl;
	}

	//1.ReadNextFrame.
	//2.GetFrame acoording to Ready flag and Frametype.
	for (int i = 0; i < frameSpace; i++)
	{
		status = VZ_GetFrameReady(deviceHandle, 1200, &FrameReady);
		if (status != VzReturnStatus::VzRetOK)
		{
			cout << "VZ_GetFrameReady failed status:" <<status<< endl;
			continue;
		}

		if (1 == FrameReady.transformedDepth)
		{
			status = VZ_GetFrame(deviceHandle, VzTransformDepthImgToColorSensorFrame, &frame);
			if (status == VzReturnStatus::VzRetOK&&frame.pFrameData != NULL)
			{
				cout << "get Frame successful,status:" << status << "  "
					<< "frameTpye:" << frame.frameType << "  "
					<< "frameIndex:" << frame.frameIndex << endl;
				// once save
				ofstream PointCloudWriter;

				PointCloudWriter.open("PointCloud.txt");
				VzFrame &srcFrame = frame;
				const int len = srcFrame.width * srcFrame.height;
				VzVector3f* worldV = new VzVector3f[len];
				cout << srcFrame.width << "," << srcFrame.height << endl;

				VZ_ConvertDepthFrameToPointCloudVector(deviceHandle, &srcFrame, worldV); //Convert Depth frame to World vectors.

				for (int i = 0; i < len; i++)
				{
					if (0 < worldV[i].z && worldV[i].z < 0xFFFF)
					{
						PointCloudWriter << worldV[i].x << "\t" << worldV[i].y << "\t" << worldV[i].z << std::endl;
					}
				}
				delete[] worldV;
				worldV = NULL;
				std::cout << "Save point cloud successful in PointCloud.txt" << std::endl;
				PointCloudWriter.close();
				break;
			}
		}

	}
	//Stop capturing the image stream
	status = VZ_StopStream(deviceHandle);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VZ_StopStream failed status:" <<status<< endl;
		return -1;
	}

	//1.close device
	//2.SDK shutdown
	status = VZ_CloseDevice(&deviceHandle);
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VZ_CloseDevice failed status:" <<status<< endl;
		return -1;
	}
	status = VZ_Shutdown();
	if (status != VzReturnStatus::VzRetOK)
	{
		cout << "VZ_Shutdown failed status:" <<status<< endl;
		return -1;
	}
	cout << "---end---"<< endl;

	return 0;
}
