#include <AEEngine.h> // f32, unsigned int, etc...
#include "FrameBuffer.h"
#include "Color.h"

// file io
#include <fstream>
#include <sstream>

#define COLOR_COMP 4

namespace Rasterizer

{
	// ------------------------------------------------------------------------
	// Our framebuffer
	unsigned char *		FrameBuffer::frameBuffer = NULL;
	unsigned int		FrameBuffer::frameBufferWidth = 0;
	unsigned int		FrameBuffer::frameBufferHeight = 0;

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		Delete
	// \brief	Free the memory allocated in the function above.
	void FrameBuffer::Delete()
	{
		if (frameBuffer)
		{
			delete[] frameBuffer;
			frameBuffer = nullptr;
		}
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		Allocate
	// \brief	Allocate memory for the frame buffer given by the width and height. 
	bool FrameBuffer::Allocate(unsigned int width, unsigned int height)
	{
		//Clear anything we could have
		Delete();
		
		//Set values
		frameBuffer = new unsigned char[width * height * COLOR_COMP];
		frameBufferHeight = height;
		frameBufferWidth = width;

		return true;
	}

	// ---------------------------------------------------------------------------
	// \fn		Present
	// \brief	Draws the contents of the frame buffer to the screen using Alpha
	//			Engine.
	void FrameBuffer::Present()
	{
		if (frameBuffer != nullptr) {
			// create a quad to draw -- counter clock wise
			AEGfxTriStart();
			
			AEGfxTriAdd(
				-0.5f, 0.5f, AE_COLORS_BLACK, 0.0, 1.0f,  // Top left
				-0.5f, -0.5f, AE_COLORS_BLACK, 0.0, 0.0f,  // Bottom Left 
				0.5f, -0.5f, AE_COLORS_BLACK, 1.0f, 0.0f); // Bottom right

			AEGfxTriAdd(
				-0.5f, 0.5f, AE_COLORS_BLACK, 0.0, 1.0f, // TR
				0.5f, -0.5f, AE_COLORS_BLACK, 1.0f, 0.0f, // BR  
				0.5f, 0.5f, AE_COLORS_BLACK, 1.0f, 1.0f); //BL				
					
			// create a texture from the framebuffer
			AEGfxTriList* quad = AEGfxTriEnd();
			AEGfxTexture *texture = AEGfxTextureLoad(frameBufferWidth, frameBufferHeight, frameBuffer);

			AEMtx33 mtx = AEMtx33::Scale(AEGetSysWindowWidth(), AEGetSysWindowHeight());
			AEGfxSetTransform(&mtx);
			AEGfxTextureSet(texture);

			//draw the quad with the size of our windows
			AEGfxTriDraw(quad);

			// free the quad
			AEGfxTriFree(quad);
			AEGfxTextureUnload(texture);
		}
	}

	// ---------------------------------------------------------------------------
	// TODO	
	// \fn		GetBufferData
	// \brief	Returns the pointer to the frame buffer variable
	unsigned char *	FrameBuffer::GetBufferData()
	{
		return frameBuffer;
	}

	// ---------------------------------------------------------------------------
	// TODO	
	// \fn		GetWidth
	// \brief	Returns the width of the frame buffer.
	unsigned int		FrameBuffer::GetWidth()
	{
		return frameBufferWidth;
	}

	// ---------------------------------------------------------------------------
	// TODO	
	// \fn		GetHeight
	// \brief	Returns the height of the frame buffer.
	unsigned int		FrameBuffer::GetHeight()
	{
		return frameBufferHeight;
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		Clear
	// \brief	Sets the entire frame buffer to the provided color.
	void FrameBuffer::Clear(const Color & c)
	{
		Clear(c.r * 255, c.g * 255, c.b * 255, c.a * 255);
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		Clear
	// \brief	Sets the entire frame buffer to the provided color in rgb format
	void FrameBuffer::Clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		int max = frameBufferHeight * frameBufferWidth * COLOR_COMP;
		for (int i = 0; i < max; i += COLOR_COMP)
		{
			frameBuffer[i + 0] = r;
			frameBuffer[i + 1] = g;
			frameBuffer[i + 2] = b;
			frameBuffer[i + 3] = a;
		}
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		SetPixel
	// \brief	Sets the pixel at position x, y to the provided color. 
	void FrameBuffer::SetPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		//Sanity check
		if (x >= frameBufferWidth)
			return;
		if (y >= frameBufferHeight)
			return;

		//Set that pixel
		int idx = (y * frameBufferWidth + x) * COLOR_COMP;
		frameBuffer[idx + 0] = r;
		frameBuffer[idx + 1] = g;
		frameBuffer[idx + 2] = b;
		frameBuffer[idx + 3] = a;
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		SetPixel
	// \brief	Sets the pixel at position x, y to the provided color. 
	void FrameBuffer::SetPixel(unsigned int x, unsigned int y, const Color& c)
	{
		SetPixel(x, y, c.r * 255, c.g * 255, c.b * 255, c.a * 255);
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		GetPixel
	// \brief	Returns the color of the pixel at position x, y.
	Color FrameBuffer::GetPixel(unsigned int x, unsigned int y)
	{		
		if (x >= frameBufferWidth || y >= frameBufferHeight)
			return Color();

		//Get idx and then normalizing color
		int idx = (x + y * frameBufferWidth) * COLOR_COMP;

		Color c;
		c.r = frameBuffer[idx + 0] / 255.0f;
		c.g = frameBuffer[idx + 1] / 255.0f;
		c.b = frameBuffer[idx + 2] / 255.0f;
		c.a = frameBuffer[idx + 3] / 255.0f;

		return c;
	}

	// ---------------------------------------------------------------------------
	// \fn		SaveToFile
	// \brief	Saves the frame buffer to a binary file. 
	void FrameBuffer::SaveToFile(const char *filename)
	{
		// Sanity Check
		if (!filename)
			return;

		// try to open the file
		std::fstream fp(filename, std::ios::out | std::ios::binary);

		if (fp.is_open() && fp.good())
		{
			// write header information - just width and height
			fp.write(reinterpret_cast<const char*>(&frameBufferWidth), sizeof(unsigned int));
			fp.write(reinterpret_cast<const char*>(&frameBufferHeight), sizeof(unsigned int));

			// write pixel data
			fp.write(reinterpret_cast<const char*>(frameBuffer), frameBufferWidth * frameBufferHeight * COLOR_COMP);

			// close the file
			fp.close();
		}
	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		LoadFromFile
	// \brief	Load the frame buffer from a binary file. 
	void FrameBuffer::LoadFromFile(const char * filename)
	{
		// Sanity check
		if (!filename)
			return;

		// try to open the file
		std::fstream fp(filename, std::ios::in | std::ios::binary);

		if (fp.is_open() && fp.good())
		{
			unsigned int fbWidth, fbHeight;
			fp.read(reinterpret_cast<char *>(&fbWidth), sizeof(unsigned int));
			fp.read(reinterpret_cast<char *>(&fbHeight), sizeof(unsigned int));

			// re-allocate the data if necessary
			if (NULL != frameBuffer && (fbWidth * fbHeight) != (frameBufferWidth * frameBufferHeight))
			{
				delete[] frameBuffer;
				frameBuffer = new unsigned char[fbWidth * fbHeight * COLOR_COMP];
			}
			// no frame buffer, allocate one
			else if (NULL == frameBuffer)
			{
				frameBuffer = new unsigned char[fbWidth * fbHeight * COLOR_COMP];
			}

			// store new width and new height
			frameBufferWidth = fbWidth;
			frameBufferHeight = fbHeight;

			// now read the framebuffer data
			fp.read(reinterpret_cast<char *>(frameBuffer), frameBufferWidth * frameBufferHeight * COLOR_COMP);

			// close the file
			fp.close();
		}
	}

	// ---------------------------------------------------------------------------
	// TODO	
	// \fn		SaveToImageFile
	// \brief	Save the frame buffer to image file. 
	void FrameBuffer::SaveToImageFile(const char * filename)
	{
		AEGfxSaveImagePNG(filename, frameBuffer, frameBufferWidth, frameBufferHeight);
	}

#pragma region// Extra Challenges

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		ClearCheckerboard
	// \brief	Clear frame buffer to a checkerboard pattern.
	void FrameBuffer::ClearCheckerboard(Color colors[2], unsigned int size)
	{

	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		LoadFromImageFile
	// \brief	Loads an image from file and sets it to the framebuffer
	void FrameBuffer::LoadFromImageFile(const char* filename)
	{

	}

	// ---------------------------------------------------------------------------
	// TODO
	// \fn		CheckerboardImage
	// \brief	Loads an image from file and sets it to the framebuffer in a checker
	//			board pattern. One square should be the original image, the other 
	//			should be the inverted color of the image (see docs for details).
	void FrameBuffer::CheckerboardImage(const char* filename, unsigned int size)
	{

	}

#pragma endregion
}
