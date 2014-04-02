#include <windows.h>
#include <stdio.h>
#include "main.h"
//����һ����׼��WIN32����, ��SDK��д
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{    //�����������, �Ǳ�׼�Ĵ������ڵĹ���
     static TCHAR szAppName[] = TEXT ("Bitmap") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     RegisterClass (&wndclass); //ע�ᴰ��
     
	 //��������
     hwnd = CreateWindow (szAppName,                  // window class name
                          TEXT ("The Hello Program"), // window caption
                          WS_OVERLAPPEDWINDOW,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters
     
	 //��ʾ����
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
	 //��Ϣѭ��
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

//�������Ϣ������
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BITMAPFILEHEADER stBMPHeader;
	BITMAPINFOHEADER stBMPInfoHeader;
	RGBQUAD MainPalette[256];
	PBYTE pByte;
    FILE* hFile;

	/////////////////////////////////////////
	LBTHeader bmfh;
	RGBQUAD rgbt;
	PBYTE bytesPerLine;




	/////////////////////////////////////////
	PAINTSTRUCT stPS;
    HDC hDC;
	HBITMAP hOldBitmap, hBitmap;
	HDC hMemDC;
	BITMAPINFO* lpBitmapInfo; //�������������BITMAPINFO������ʲô����

	static BOOL bFlag = FALSE;

	switch (message)
	{
	case WM_PAINT: //������Ҫ����һ��WM_PAINT��Ϣ����Ĺ���, �ڴ��������Ϣ��ʱ�����ǽ�����ʾ��������õ�λͼ����
		hDC = BeginPaint (hwnd, &stPS); //�������ǻ�ȡ�豸������

		//DIBתDDB //Ȼ��������ʾһ��DIBת��ΪDDB�Ĺ��̡�����������һ��0.bmp���ļ��������Ǿ���Ҫ�����DIB��ʾ����Ļ��
		hFile = fopen("0.LBT", "rb"); //���ȴ�����ļ�
		if (hFile)
		{
			fread(&bmfh, 1, sizeof(bmfh), hFile); //��ȡ�ļ�ͷ
		//	fread(&stBMPInfoHeader, 1, sizeof(BITMAPINFOHEADER), hFile); //��ȡλͼ��Ϣͷ
			if (bmfh.bfType == 0x54424C) //�����λͼBMP���ļ���ʶ, �������ж��Ƿ�ΪBMP�ļ�
			{
				//����δ��ôһ���ṹ��ָ������ڴ�, ע�������Ǹ�256
				lpBitmapInfo = (BITMAPINFO*)new CHAR[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256];
				if (bmfh.biBitCount == 8) //�����ж��Ƿ�Ϊ8λλͼ, ����ǣ���Ҫ�ѵ�ɫ�������
				{
					fread(MainPalette, 1, sizeof(MainPalette), hFile);
				}	
				//������Ƕ�ȡλͼ�����������ˡ������ǵ�λͼ���������ݵ����ֽ�����ô������?���ǻع�һ�°�
				//RowLength = ((bmch.bcWidth * bmch.bcBitCount + 31) / 32) * 4
				//����ú�, ���Ƿ�����ڴ�, Ȼ���λͼ�������ݶ�����
				pByte = new BYTE[((((stBMPInfoHeader.biWidth * stBMPInfoHeader.biBitCount) + 31) / 32) * 4) * stBMPInfoHeader.biHeight];
				fread(pByte, 1, ((((bmfh.biWidth * bmfh.biBitCount) + 31) / 32) * 4) * bmfh.biHeight, hFile);

				//����������BITMAPINFO��������ˡ�����
				//����0
				memset(lpBitmapInfo, 0, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256);
				lpBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //�ṹ��С
				lpBitmapInfo->bmiHeader.biWidth = bmfh.biWidth; //λͼ���
				lpBitmapInfo->bmiHeader.biHeight = bmfh.biHeight; //λͼ�߶�
				lpBitmapInfo->bmiHeader.biPlanes = 1; //һ��Ϊ1
				lpBitmapInfo->bmiHeader.biBitCount = bmfh.biBitCount; //λͼλ��
				if (bmfh.biBitCount == 8) //�����8λ��λͼ�������õ�ɫ��, �Ѹոն������ĵ�ɫ�帴�ƹ�ȥ
				{
					//������û, ����԰�bmiColors����ֶε�һ�������á�����ֻҪ�������㹻���ڴ�Ϳ����ˡ���
					memcpy(lpBitmapInfo->bmiColors, MainPalette, sizeof(RGBQUAD)*256);
				}
				
				//������ʾ��ô�����0.bmp��ʾ����Ļ���ˡ���
				//����һ:
				/*
				//���Ǵ����ȡ����DC�� �������Ǹո����õ�λͼ��Ϣͷ, Ȼ���������Ƕ�ȡ��λͼ����, �����������Ľṹ����
				//��������ȷ�Ĳ�������������0.bmp������һ��DDB�ˡ�����
				//������ʱ���ǻ�����ֱ�ӽ�DDB��ʾ���豸�ϡ������ǻ��ý���ѡ�뵽һ���ڴ�DC��, �ٽ�����ڴ�DC��ʾ����ʵ��DC��
				hBitmap = CreateDIBitmap(hDC, &lpBitmapInfo->bmiHeader, CBM_INIT, pByte, lpBitmapInfo, DIB_RGB_COLORS);
				hMemDC = CreateCompatibleDC(hDC); //����һ�����ݵ��ڴ�DC
				hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap); //�����Ǹոմ�����DDBѡ������ڴ�DC��
				                                                     //����������������ڴ�DC�Ļ���������λͼ�ˡ���
				                                                     //��Ҫ�ǵñ���ɵ�λͼ

				//Ȼ�������ڴ�DC BitBlt ����ʵ��DC�С���������ʾ�ʹ󹦸��
				//���Ǿ���Ŀ��DC��0,0���껭0.bmp���λͼ����
				//Ŀ��DC�Ǹ��ոհ�ɫ�Ŀͻ�����صġ��������������Ͻ�Ϊԭ�㡣����������Ϊ��Ϊ����X������Ϊ����Y
				//�������Ǿͽ�����hMemDC BitBlt ��Ŀ��hDC����
				//������2��DC������ݵ����ǲſ���������Ŷ������
//				BitBlt(hDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, hMemDC, 0, 0, SRCCOPY);

				//�����������û�зŴ���С�Ĺ��ܡ����������ǽ���һ�����������ĺ���
				//����û������С�ˡ�����
//				SetStretchBltMode(hDC, COLORONCOLOR); //�����ʲô����?
				//����û�����������ô�ո�������С�ľͱ�Ϊʧ���ˡ�����������������������ŵĴ���Ч���ġ���һ����ո������������þͿ����ˡ���
//				StretchBlt(hDC, 0, 0, 50, 50, hMemDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, SRCCOPY);

				//��������Ҫ�Ѹո�ͼƬ��ɫ�Ķ���ȥ��������ô��? �Ǻ�, ��ɫ������Ŷ����
				TransparentBlt(hDC, 0, 0, 50, 50, hMemDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, RGB(0, 0, 0));

				SelectObject(hMemDC, hOldBitmap);
				DeleteDC(hMemDC);
				DeleteObject(hBitmap);
				*/

				//������:
				//*
				//��������̶��ˡ�����һ�������͸㶨�ˡ���
// 				SetDIBitsToDevice(hDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, 0, 0, 
// 					0, stBMPInfoHeader.biHeight, pByte, lpBitmapInfo, DIB_RGB_COLORS);
	
				//��Ȼ�ˡ��������������Ҳ�ǲ��ṩ���ŵĹ����ˡ������������������ṩ�����ŵĹ�����
				SetStretchBltMode(hDC, COLORONCOLOR); //ͬ����Ҫ�������������ģʽ
				StretchDIBits(hDC, 0, 0, bmfh.biWidth, bmfh.biHeight, 0, 0, bmfh.biWidth, bmfh.biHeight, pByte, lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
				
				//��������򵥵ķ������ṩ͸����ʾ�Ĺ����ˡ���Ҫ��͸����ʾֻ���õ�һ�ַ����ˡ���
				//��ʵ��2�ַ�����Ҫ��DIBת��ΪDDB������ʾ����ֻ�����ڶ��ַ������ڲ�ת���ˡ�����
				//�ڶ��ַ������ü򵥡�����������ÿ����ʾ����ת��һ�Ρ�������Ч�ʲ���
				//����һ�ַ�����ֻҪ��DIBת��ΪDDB�ˡ���ֻҪ��DDB�����ˡ����Ϳ��Է�������ʾ�ˡ����Ǻǡ�������
				//ÿ�ζ���DIB��DDB��ת���ˡ�����
				//*/

				delete[] lpBitmapInfo;
			}

			fclose(hFile);
		}

		//����DIB��DDB��ת���������ǽ���DDB��DIB��ת���ɡ���������þ��൱����Ļ��ͼ���ܡ���������Ŵ�Ҷ��ù��˰ɡ���
		//DDBתDIB
		/*
		if (!bFlag) //�����־������������ֻ����һ��������롣������WM_PAINT��һ����Ӧ��ʱ�����С���
		{
			RECT rect;

			bFlag = TRUE;

			GetClientRect(hwnd, &rect); //�������ǻ�ȡ�ͻ�����С
			hMemDC = CreateCompatibleDC(hDC); //�����ڴ�DC���������Ҷ���İ���˰ɡ���
			lpBitmapInfo = new BITMAPINFO; //������ǲ������ɫ����ڴ��ˡ����ٺ١�����Ϊ���ǲ��õ�ɫ���ˡ���
			memset(lpBitmapInfo, 0, sizeof(BITMAPINFO)); //��ʼ��
			lpBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
			lpBitmapInfo->bmiHeader.biWidth = rect.right; //�ͻ�����С����
			lpBitmapInfo->bmiHeader.biHeight = rect.bottom; //���Ǵ���ѿͻ�����ص�DDB����ΪDIB
			lpBitmapInfo->bmiHeader.biPlanes = 1; 
			lpBitmapInfo->bmiHeader.biBitCount = 16; //���Ǳ���Ϊ16λ��λͼŶ����������������û�з����ɫ����ڴ�

			//�����Ǵ�����һ��DIB�ˡ��������DIB���ڴ�DC�������Ŷ����ע���ˡ�����ʾ����һ��Ҫ��������ڴ�DC��ͼ��Ŷ����
			hBitmap = CreateDIBSection(hMemDC, lpBitmapInfo, DIB_RGB_COLORS, (VOID**)&pByte, NULL, 0);
			//�����Ǵ�����DIBѡ���ڴ�DC
			hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
			//��ʵ��DC������BitBlt���ڴ�DC���������ڴ�DC�Ļ������ʵ��DC�Ļ����˰���������������ʵ�Ǳ�����ʵ��DC�Ļ����ˡ���
			BitBlt(hMemDC, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);

			//���������ͻ����DIB�������ˡ�����������ˣ��ʹ�pByte���ָ�롣���ո�BitBlt��ʱ���Ѿ���DDBת��ΪDIB��
			//�ⶼ��GDI�ڲ������ˡ�����
			//�á�����������������BMP�ˡ�������ֻ��Ҫ��BMP�ļ��ĸ�ʽ��֯���ļ��Ϳ�����
			memset(&stBMPHeader, 0, sizeof(BITMAPFILEHEADER));
			stBMPHeader.bfType = 0x4d42;
			stBMPHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			stBMPHeader.bfSize = stBMPHeader.bfOffBits + ((((rect.right * 16) + 31) / 32) * 4)*rect.bottom;
			//��Щ���붼���ú���BMP���ļ�ͷ�ˡ���
			hFile = fopen("1.bmp", "wb"); //����д������1.bmp�ļ���Ŷ����
			if (hFile)
			{
				fwrite(&stBMPHeader, 1, sizeof(BITMAPFILEHEADER), hFile); //д�ļ�ͷ����
				fwrite(lpBitmapInfo, 1, sizeof(BITMAPINFOHEADER), hFile); //дλͼ��Ϣͷ����������Ǵ���DIBλͼ��ʱ���Ѿ����ú��ˡ�����
				//д�����ݡ�������Ҫ�ǵ��Ǹ�4���ֽڵĹ���Ŷ������
				fwrite(pByte, 1, ((((rect.right * 16) + 31) / 32) * 4)*rect.bottom, hFile);
				fclose(hFile);	//�ر��ļ�����
			}
			SelectObject(hMemDC, hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hMemDC);  //���ԭ��������Դ����
			delete lpBitmapInfo;
		}
		*/
		EndPaint (hwnd, &stPS);
		return 0;

	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}