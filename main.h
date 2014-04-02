
struct LBTHeader
{	  
   DWORD    bfType;
    LONG    biWidth;
    LONG    biHeight;
    DWORD    biBitCount;
};
struct LBTINFO{
        //DWORD      biSize;

        //WORD       biPlanes;
        DWORD       biBitCount;
		PBYTE pBits;
        //DWORD      biCompression;
        //DWORD      biSizeImage;
       // LONG       biXPelsPerMeter;
        //LONG       biYPelsPerMeter;
        //DWORD      biClrUsed;
        //DWORD      biClrImportant;
};
struct BilHeader
{	
	   CHAR szTitle[4];
	   INT iColorCount;
	   INT iImageCount;
};

struct BilImage
{
	   SHORT sWidth;
	   SHORT sHeight;
	   SHORT sOffsetX;
	   SHORT sOffsetY;
	   PBYTE pBits; //程序中保存的数据指针
};

/**********
struct LBTMAPINFO{
LBTINFO bmiHeader;
RGBQUAD bmiColors[1];
};
*************/