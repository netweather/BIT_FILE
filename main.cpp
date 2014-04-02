#include <windows.h>
#include <stdio.h>
#include "main.h"
//这是一个标准的WIN32程序, 用SDK编写
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{    //这个是主函数, 是标准的创建窗口的过程
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

     RegisterClass (&wndclass); //注册窗口
     
	 //创建窗口
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
     
	 //显示窗口
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
	 //消息循环
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

//这个是消息处理函数
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
	BITMAPINFO* lpBitmapInfo; //我们来看看这个BITMAPINFO究竟是什么东西

	static BOOL bFlag = FALSE;

	switch (message)
	{
	case WM_PAINT: //我们主要来看一下WM_PAINT消息处理的过程, 在处理这个消息的时候我们将会演示大多数常用的位图函数
		hDC = BeginPaint (hwnd, &stPS); //首先我们获取设备上下文

		//DIB转DDB //然后我们演示一下DIB转化为DDB的过程。。。这里有一个0.bmp的文件。。我们就是要把这个DIB显示到屏幕上
		hFile = fopen("0.LBT", "rb"); //首先打开这个文件
		if (hFile)
		{
			fread(&bmfh, 1, sizeof(bmfh), hFile); //读取文件头
		//	fread(&stBMPInfoHeader, 1, sizeof(BITMAPINFOHEADER), hFile); //读取位图信息头
			if (bmfh.bfType == 0x54424C) //这个是位图BMP的文件标识, 在这里判断是否为BMP文件
			{
				//我们未这么一个结构的指针分配内存, 注意后面的那个256
				lpBitmapInfo = (BITMAPINFO*)new CHAR[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256];
				if (bmfh.biBitCount == 8) //这里判断是否为8位位图, 如果是，则要把调色板读出来
				{
					fread(MainPalette, 1, sizeof(MainPalette), hFile);
				}	
				//下面就是读取位图的像素数据了。。还记得位图的像素数据的总字节数怎么计算吗?我们回顾一下啊
				//RowLength = ((bmch.bcWidth * bmch.bcBitCount + 31) / 32) * 4
				//计算好后, 我们分配好内存, 然后把位图像素数据读出来
				pByte = new BYTE[((((stBMPInfoHeader.biWidth * stBMPInfoHeader.biBitCount) + 31) / 32) * 4) * stBMPInfoHeader.biHeight];
				fread(pByte, 1, ((((bmfh.biWidth * bmfh.biBitCount) + 31) / 32) * 4) * bmfh.biHeight, hFile);

				//下面来设置BITMAPINFO这个玩意了。。。
				//先清0
				memset(lpBitmapInfo, 0, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256);
				lpBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //结构大小
				lpBitmapInfo->bmiHeader.biWidth = bmfh.biWidth; //位图宽度
				lpBitmapInfo->bmiHeader.biHeight = bmfh.biHeight; //位图高度
				lpBitmapInfo->bmiHeader.biPlanes = 1; //一般为1
				lpBitmapInfo->bmiHeader.biBitCount = bmfh.biBitCount; //位图位数
				if (bmfh.biBitCount == 8) //如果是8位的位图还得设置调色板, 把刚刚读出来的调色板复制过去
				{
					//看到了没, 你可以把bmiColors这个字段当一个数组用。。。只要分配了足够的内存就可以了。。
					memcpy(lpBitmapInfo->bmiColors, MainPalette, sizeof(RGBQUAD)*256);
				}
				
				//下面演示怎么把这个0.bmp显示到屏幕上了。。
				//方法一:
				/*
				//我们传入获取到的DC， 还有我们刚刚设置的位图信息头, 然后设置我们读取的位图数据, 还有这个特殊的结构。。
				//传入了正确的参数。。我们由0.bmp创建了一个DDB了。。。
				//不过此时我们还不能直接将DDB显示到设备上。。我们还得将它选入到一个内存DC中, 再将这个内存DC显示到真实的DC上
				hBitmap = CreateDIBitmap(hDC, &lpBitmapInfo->bmiHeader, CBM_INIT, pByte, lpBitmapInfo, DIB_RGB_COLORS);
				hMemDC = CreateCompatibleDC(hDC); //创建一个兼容的内存DC
				hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap); //将我们刚刚创建的DDB选入这个内存DC中
				                                                     //这样现在我们这个内存DC的画面就是这个位图了。。
				                                                     //还要记得保存旧的位图

				//然后把这个内存DC BitBlt 到真实的DC中。。这样显示就大功告成
				//我们就在目标DC的0,0坐标画0.bmp这个位图好了
				//目标DC是跟刚刚白色的客户区相关的。。。坐标以左上角为原点。。。坐标轴为右为正向X。。下为正向Y
				//这样我们就将整个hMemDC BitBlt 到目标hDC上了
				//由于这2个DC是相兼容的我们才可以这样做哦。。。
//				BitBlt(hDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, hMemDC, 0, 0, SRCCOPY);

				//上面这个函数没有放大缩小的功能。。下面我们介绍一个可以伸缩的函数
				//看到没。。缩小了。。。
//				SetStretchBltMode(hDC, COLORONCOLOR); //这个是什么东西?
				//假如没了这个东西那么刚刚我们缩小的就变为失真了。。这个东西是用来设置缩放的处理效果的。。一般像刚刚上面这样设置就可以了。。
//				StretchBlt(hDC, 0, 0, 50, 50, hMemDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, SRCCOPY);

				//假如我们要把刚刚图片黑色的东西去掉。。怎么做? 呵呵, 黑色不见了哦。。
				TransparentBlt(hDC, 0, 0, 50, 50, hMemDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, RGB(0, 0, 0));

				SelectObject(hMemDC, hOldBitmap);
				DeleteDC(hMemDC);
				DeleteObject(hBitmap);
				*/

				//方法二:
				//*
				//方法二简短多了。。就一个函数就搞定了。。
// 				SetDIBitsToDevice(hDC, 0, 0, stBMPInfoHeader.biWidth, stBMPInfoHeader.biHeight, 0, 0, 
// 					0, stBMPInfoHeader.biHeight, pByte, lpBitmapInfo, DIB_RGB_COLORS);
	
				//当然了。。上面这个函数也是不提供缩放的功能了。。下面的这个函数就提供了缩放的功能了
				SetStretchBltMode(hDC, COLORONCOLOR); //同样需要设置伸缩处理的模式
				StretchDIBits(hDC, 0, 0, bmfh.biWidth, bmfh.biHeight, 0, 0, bmfh.biWidth, bmfh.biHeight, pByte, lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
				
				//不过这个简单的方法不提供透明显示的功能了。。要想透明显示只能用第一种方法了。。
				//其实这2种方法都要将DIB转化为DDB才能显示。。只不过第二种方法在内部转换了。。。
				//第二种方法调用简单。。不过就是每次显示都得转换一次。。所以效率不高
				//而第一种方法则只要把DIB转化为DDB了。。只要把DDB保存了。。就可以反复地显示了。。呵呵。。不用
				//每次都做DIB到DDB的转换了。。。
				//*/

				delete[] lpBitmapInfo;
			}

			fclose(hFile);
		}

		//讲完DIB到DDB的转换。。我们讲点DDB到DIB的转换吧。。这个作用就相当于屏幕截图功能。。这个相信大家都用过了吧。。
		//DDB转DIB
		/*
		if (!bFlag) //这个标志用来设置我们只运行一次这个代码。。就是WM_PAINT第一次响应的时候运行。。
		{
			RECT rect;

			bFlag = TRUE;

			GetClientRect(hwnd, &rect); //首先我们获取客户区大小
			hMemDC = CreateCompatibleDC(hDC); //创建内存DC。。这个大家都不陌生了吧。。
			lpBitmapInfo = new BITMAPINFO; //这次我们不分配调色板的内存了。。嘿嘿。。因为我们不用调色板了。。
			memset(lpBitmapInfo, 0, sizeof(BITMAPINFO)); //初始化
			lpBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
			lpBitmapInfo->bmiHeader.biWidth = rect.right; //客户区大小。。
			lpBitmapInfo->bmiHeader.biHeight = rect.bottom; //我们打算把客户区相关的DDB保存为DIB
			lpBitmapInfo->bmiHeader.biPlanes = 1; 
			lpBitmapInfo->bmiHeader.biBitCount = 16; //我们保存为16位的位图哦。。所以上面我们没有分配调色板的内存

			//好我们创建了一个DIB了。。。这个DIB跟内存DC相关联了哦。。注意了。。表示我们一会要保存的是内存DC的图像哦。。
			hBitmap = CreateDIBSection(hMemDC, lpBitmapInfo, DIB_RGB_COLORS, (VOID**)&pByte, NULL, 0);
			//将我们创建的DIB选入内存DC
			hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
			//将实际DC的数据BitBlt到内存DC。。这样内存DC的画面就是实际DC的画面了啊。。这样我们其实是保存了实际DC的画面了。。
			BitBlt(hMemDC, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);

			//我们这样就获得了DIB的数据了。。怎样获得了？就从pByte这个指针。。刚刚BitBlt的时候已经将DDB转化为DIB了
			//这都是GDI内部操作了。。。
			//好。。我们下面来保存BMP了。。我们只需要按BMP文件的格式组织好文件就可以了
			memset(&stBMPHeader, 0, sizeof(BITMAPFILEHEADER));
			stBMPHeader.bfType = 0x4d42;
			stBMPHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			stBMPHeader.bfSize = stBMPHeader.bfOffBits + ((((rect.right * 16) + 31) / 32) * 4)*rect.bottom;
			//这些代码都设置好了BMP的文件头了。。
			hFile = fopen("1.bmp", "wb"); //我们写到的是1.bmp文件中哦。。
			if (hFile)
			{
				fwrite(&stBMPHeader, 1, sizeof(BITMAPFILEHEADER), hFile); //写文件头。。
				fwrite(lpBitmapInfo, 1, sizeof(BITMAPINFOHEADER), hFile); //写位图信息头。。这个我们创建DIB位图的时候已经设置好了。。。
				//写入数据。。还是要记得那个4个字节的规则哦。。。
				fwrite(pByte, 1, ((((rect.right * 16) + 31) / 32) * 4)*rect.bottom, hFile);
				fclose(hFile);	//关闭文件。。
			}
			SelectObject(hMemDC, hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hMemDC);  //最后还原和清理资源。。
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