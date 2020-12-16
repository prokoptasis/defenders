/////////////////////////////////////////////////////////////////////////////
// Name : CDBitmap.cpp
// Desc : Bitmap Class
// Date : 2002.11.18
// Mail : jhook@hanmail.net
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// INCLUDE
/////////////////////////////////////////////////////////////////////////////
#include "CDBitmap.h"
/////////////////////////////////////////////////////////////////////////////
// GLOBAL
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	TYPE
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//	Name : CDBitmap()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
CDBitmap::CDBitmap()
{

}

/////////////////////////////////////////////////////////////////////////////
//	Name : ~CDBitmap()
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
CDBitmap::~CDBitmap()
{

}

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
//  Desc : 
///////////////////////////////////////////////////////////////////////////// 
HRESULT CDBitmap::Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
{
	int			file_handle,index;			// looping index
	OFSTRUCT	file_data;				// the file data information

	//if ((file_handle = OpenFile(filename,&file_data,OF_READ))==-1)
	//	return E_FAIL;

	HANDLE hFile;
	hFile = CreateFile ( filename,
						 GENERIC_READ,
						 0,
						 NULL,
						 OPEN_EXISTING,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL );
	//file_handle = OpenFile(filename,&file_data,OF_READ);

	DWORD dwRead;
	ReadFile(hFile,&bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER),&dwRead,NULL);	
	//_lread(file_handle, &bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER));

	if (bitmap->bitmapfileheader.bfType!=BITMAP_ID)
	{
		CloseHandle(hFile);
		//_lclose(file_handle);
		return E_FAIL;
	} // end if

	ReadFile(hFile,&bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER),&dwRead,NULL);
	//_lread(file_handle, &bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER));

	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		ReadFile(hFile,&bitmap->palette,PALETTE_MAX_COLORS*sizeof(PALETTEENTRY),&dwRead,NULL);
		//_lread(file_handle, &bitmap->palette,PALETTE_MAX_COLORS*sizeof(PALETTEENTRY));
		
		for (index=0; index < PALETTE_MAX_COLORS; index++)
		{
			int temp_color                = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = temp_color;
			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		} // end for index
    } // end if

	int a=sizeof(BITMAPFILEHEADER);
	int b=sizeof(BITMAPINFOHEADER);
	int c= PALETTE_MAX_COLORS*sizeof(PALETTEENTRY);
	int xx = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);// + PALETTE_MAX_COLORS*sizeof(PALETTEENTRY);

	SetFilePointer( hFile,-xx,NULL,FILE_END );    // how to move );
	//int xxhFile = SetFilePointer( hFile,-(int)(bitmap->bitmapinfoheader.biSizeImage),NULL,FILE_END );    // how to move );
 	//_lseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

	if (bitmap->bitmapinfoheader.biBitCount==8 || bitmap->bitmapinfoheader.biBitCount==16 || bitmap->bitmapinfoheader.biBitCount==24)
	{
   		if (bitmap->buffer)
		{
			free(bitmap->buffer);
		}

		if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapfileheader.bfSize -bitmap->bitmapfileheader.bfOffBits)))
		{
			CloseHandle(hFile);
			//_lclose(file_handle);
			return E_FAIL;
		} // end if

		ReadFile(hFile,&bitmap->buffer,bitmap->bitmapfileheader.bfSize -bitmap->bitmapfileheader.bfOffBits,&dwRead,NULL);
		//_lread(file_handle,bitmap->buffer,bitmap->bitmapfileheader.bfSize -bitmap->bitmapfileheader.bfOffBits);

	} // end if
	else
	{
   		return E_FAIL;
	} // end else

	#if 0
	printf("\nfilename:%s \nsize=%d \nwidth=%d \nheight=%d \nbitsperpixel=%d \ncolors=%d \nimpcolors=%d",
		    filename,
			bitmap->bitmapinfoheader.biSizeImage,
			bitmap->bitmapinfoheader.biWidth,
			bitmap->bitmapinfoheader.biHeight,
			bitmap->bitmapinfoheader.biBitCount,
			bitmap->bitmapinfoheader.biClrUsed,
			bitmap->bitmapinfoheader.biClrImportant);
	#endif

	CloseHandle(hFile);
	//_lclose(file_handle);

	int etc = bitmap->bitmapinfoheader.biWidth%4;
//	Flip_Bitmap(bitmap->buffer,bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8)+etc,bitmap->bitmapinfoheader.biHeight);

	return S_OK;
} // end Load_Bitmap_File

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
{
	int index;     
	UCHAR *buffer;
	
	if (!(buffer = (UCHAR*)malloc(bytes_per_line*height*3)))
		return E_FAIL;

	memcpy(buffer,image,bytes_per_line*height);
	
	for (index=0; index < height; index++)
	{
		memcpy(&image[((height-1)-index)*bytes_per_line],&buffer[index*bytes_per_line],bytes_per_line);
	}

	free(buffer);

	return S_OK;
} // end Flip_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Scan_Image_Bitmap(HWND m_hWnd, BOOL m_bWindowed, BITMAP_FILE_PTR bitmap,LPDIRECTDRAWSURFACE7 lpdds, int cx,int cy)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Scan_Image_Bitmap(HWND m_hWnd, BOOL m_bWindowed, BITMAP_FILE_PTR bitmap,LPDIRECTDRAWSURFACE7 lpdds, int cx,int cy)
{
	if (!bitmap)
		return E_FAIL;
	
	DDSURFACEDESC2 ddsd;
	CDBITMAP_INITSTRUCT(ddsd);

	if(FAILED(lpdds->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL)))
		return E_FAIL;

	DDPIXELFORMAT ddpixelformat;
	DWORD *primary_buffer = (DWORD *)ddsd.lpSurface;       

	CDBITMAP_INITSTRUCT(ddpixelformat);
	lpdds->GetPixelFormat(&ddpixelformat);

	int pixel_format = ddpixelformat.dwRGBBitCount;

	GetWindowRect(m_hWnd,&m_rcWindow);

    DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

	int windowx = m_rcWindow.left+dwFrameWidth;
	int windowy = m_rcWindow.top+dwCaptionHeight+dwFrameHeight;

	int etc = bitmap->bitmapinfoheader.biWidth%4;

	if (pixel_format == 32 || pixel_format == 24)
	{
		for (int index_y = 0 ; index_y < bitmap->bitmapinfoheader.biHeight; index_y++)
		{
			for (int index_x = 0; index_x < bitmap->bitmapinfoheader.biWidth; index_x++)
			{
				DWORD	blue  = (bitmap->buffer[index_y*(bitmap->bitmapinfoheader.biWidth*3+etc) + index_x*3 + 0]),
						green = (bitmap->buffer[index_y*(bitmap->bitmapinfoheader.biWidth*3+etc) + index_x*3 + 1]),
						red   = (bitmap->buffer[index_y*(bitmap->bitmapinfoheader.biWidth*3+etc) + index_x*3 + 2]);

				DWORD pixel = _RGB32BIT(0,red,green,blue);

				if ( index_y < SCREEN_HEIGHT && index_x < SCREEN_WIDTH)
				{
					if (m_bWindowed)
					{
						primary_buffer[index_x*4+(index_y*ddsd.lPitch)/4] = pixel;
					}
					else
					{
						primary_buffer[(index_x+windowx) + (((index_y+windowy)*ddsd.lPitch) >> 2)] = pixel;
					}
				}
			} 
		} 
	}
	else if (pixel_format == 16)		// 16biy
	{
	}
	else								// 8bit
	{
	}

	if (FAILED(lpdds->Unlock(NULL)))
		return E_FAIL;

	return S_OK;
} // end Scan_Image_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
//  Desc : 
///////////////////////////////////////////////////////////////////////////// 
HRESULT CDBitmap::Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
	if (bitmap->buffer)
	{
		free(bitmap->buffer);
		bitmap->buffer = NULL;
	} // end if

	return S_OK;
} // end Unload_Bitmap_File

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height)
//  Desc : Initialize a Bitmap
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Create_Bitmap(BITMAP_IMAGE_PTR image, int x, int y, int width, int height)
{
	int bpp = 1;

	if((SCREEN_BPP/8) > 3)
		bpp = 3;

	if (!(image->buffer = (UCHAR *)malloc(width*height*bpp)))
		return E_FAIL;

	image->state     = BITMAP_STATE_ALIVE;
	image->attr      = 0;
	image->width     = width;
	image->height    = height;
	image->x         = x;
	image->y         = y;
	image->num_bytes = width*height*bpp;

	memset(image->buffer,0,width*height*bpp);

	return S_OK;
} // end Create_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Destroy_Bitmap(BITMAP_IMAGE_PTR image)
//  Desc : Release the memory associated with a bitmap
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Destroy_Bitmap(BITMAP_IMAGE_PTR image)
{
	if (image && image->buffer)
	{
		free(image->buffer);
		memset(image,0,sizeof(BITMAP_IMAGE));
		return S_OK;
	} 
	else
	{
		return E_FAIL;
	}		
} // end Destroy_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : Load_Image_Bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode)
//  Desc : 
///////////////////////////////////////////////////////////////////////////// 
HRESULT CDBitmap::Load_Image_Bitmap(BITMAP_IMAGE_PTR image, BITMAP_FILE_PTR bitmap, int cx,int cy, int mode)
{
	UCHAR *source_ptr;
	UCHAR *dest_ptr;

	if (!image)
		return E_FAIL;

	source_ptr	= bitmap->buffer; // + cy*bitmap->bitmapinfoheader.biWidth+cx;
	dest_ptr	= image->buffer;

	if (mode==BITMAP_EXTRACT_MODE_CELL)
	{
		cx = cx*(image->width+1) + 1;
		cy = cy*(image->height+1) + 1;
	}

	for (int index_y=0; index_y<image->height; index_y++)
    {
		memcpy(dest_ptr, source_ptr,image->width*(bitmap->bitmapinfoheader.biBitCount/8));

		dest_ptr   += image->width*(bitmap->bitmapinfoheader.biBitCount/8);
		source_ptr += bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8);
	} // end for index_y

	image->attr |= BITMAP_ATTR_LOADED;
	return S_OK;
} // end Load_Image_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Scroll_Bitmap(void)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Scroll_Bitmap(void)
{
	// this function scrolls a bitmap
	// not implemented
	// return success
	return S_OK;
} // end Scroll_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Copy_Bitmap(void)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Copy_Bitmap(void)
{
	// this function copies a bitmap from one source to another
	// not implemented
	// return success
	return S_OK;
} // end Copy_Bitmap

/////////////////////////////////////////////////////////////////////////////
//	Name : HRESULT CDBitmap::Draw_Bitmap(HWND m_hWnd,BOOL m_bWindowed, BITMAP_IMAGE_PTR source_bitmap,UCHAR *dest_buffer, int lpitch, int transparent)
//  Desc : 
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBitmap::Draw_Bitmap(HWND m_hWnd,BOOL m_bWindowed, BITMAP_IMAGE_PTR source_bitmap,UCHAR *dest_buffer, int lpitch, DWORD transparent)
{
	DWORD	*dest_addr = (DWORD *)dest_buffer;

	GetWindowRect(m_hWnd,&m_rcWindow);

    DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
    DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
    DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

	int windowx = 0;//m_rcWindow.left+dwFrameWidth;
	int windowy = 0;//m_rcWindow.top+dwCaptionHeight+dwFrameHeight;

	int etc = source_bitmap->width%4;

	if (source_bitmap->attr & BITMAP_ATTR_LOADED)
	{
		for (int index_y = 0; index_y < source_bitmap->height;index_y++)
		{
			for (int index_x = 0; index_x < source_bitmap->width;index_x++)
			{
				DWORD	blue  = (source_bitmap->buffer[index_y*(source_bitmap->width*3+etc) + index_x*3 + 0]),
						green = (source_bitmap->buffer[index_y*(source_bitmap->width*3+etc) + index_x*3 + 1]),
						red   = (source_bitmap->buffer[index_y*(source_bitmap->width*3+etc) + index_x*3 + 2]);

				DWORD pixel = _RGB32BIT(0,red,green,blue);

				if ( index_y < SCREEN_HEIGHT && index_x < SCREEN_WIDTH)
				{
					if (m_bWindowed)
					{
						if(pixel != transparent)
						{
							dest_addr[index_x*4+(index_y*lpitch)>>2] = pixel;
						}
					}
					else
					{
						if(pixel != transparent)
						{
							dest_addr[(index_x+windowx) + (((index_y+windowy)*lpitch) >> 2)] = pixel;
						}
					}
				}
			}
		}
		
		return S_OK;
	} 
	else
	{
		return E_FAIL;
	}
} 