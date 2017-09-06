/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1QtInventorUtils             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1Msg.h"


//#include "Inventor/nodes/SoNode.h"
#include "Inventor/nodes/SoMaterial.h"
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoCoordinate4.h>

#include <Inventor/SoPath.h>
#include <Inventor/SoOffscreenRenderer.h>
#include "Inventor/Qt/SoQtRenderArea.h"
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoWriteAction.h>

#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>


#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/actions/SoToVRML2Action.h>

#include <QDir>
#include <QTime>
#include <QBuffer>
#include <QByteArray>
#include <QTextStream>
#include <QSlider>
#include <QGLFormat>

#include <iostream>

//____________________________________________________________________
class VP1QtInventorUtils::Imp {
public:

	static QImage constructImageWithTransparentBackground(const QImage& im_black_bgd, const QImage& image_white_bgd);

	static void bwtorgba(unsigned char *b,unsigned char *l,int n)
	{
		while (n--) {
			l[0] = *b;
			l[1] = *b;
			l[2] = *b;
			l[3] = 0xff;
			l += 4; ++b;
		}
	}

	static void latorgba(unsigned char *b, unsigned char *a,unsigned char *l,int n)
	{
		while (n--) {
			l[0] = *b;
			l[1] = *b;
			l[2] = *b;
			l[3] = *a;
			l += 4; ++b; ++a;
		}
	}

	static void rgbtorgba(unsigned char *r,unsigned char *g,
			unsigned char *b,unsigned char *l,int n)
	{
		while (n--) {
			l[0] = r[0];
			l[1] = g[0];
			l[2] = b[0];
			l[3] = 0xff;
			l += 4; ++r; ++g; ++b;
		}
	}

	static void rgbatorgba(unsigned char *r,unsigned char *g,
			unsigned char *b,unsigned char *a,unsigned char *l,int n)
	{
		while (n--) {
			l[0] = r[0];
			l[1] = g[0];
			l[2] = b[0];
			l[3] = a[0];
			l += 4; ++r; ++g; ++b; ++a;
		}
	}

	typedef struct _ImageRec {
		unsigned short imagic;
		unsigned short type;
		unsigned short dim;
		unsigned short xsize, ysize, zsize;
		unsigned int min, max;
		unsigned int wasteBytes;
		char name[80];
		unsigned long colorMap;
		FILE *file;
		unsigned char *tmp, *tmpR, *tmpG, *tmpB;
		unsigned long rleEnd;
		unsigned int *rowStart;
		int *rowSize;
	} ImageRec;

	static void ConvertShort(unsigned short *array, long length)
	{
		unsigned b1, b2;
		unsigned char *ptr;

		ptr = (unsigned char *)array;
		while (length--) {
			b1 = *ptr++;
			b2 = *ptr++;
			*array++ = (b1 << 8) | (b2);
		}
	}

	static void ConvertLong(unsigned *array, long length)
	{
		unsigned b1, b2, b3, b4;
		unsigned char *ptr;

		ptr = (unsigned char *)array;
		while (length--) {
			b1 = *ptr++;
			b2 = *ptr++;
			b3 = *ptr++;
			b4 = *ptr++;
			*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
		}
	}

	static ImageRec *ImageOpen(const char *fileName)
	{
		union {
			int testWord;
			char testByte[4];
		} endianTest;
		ImageRec *image;
		int swapFlag;

		endianTest.testWord = 1;
		if (endianTest.testByte[0] == 1) {
			swapFlag = 1;
		} else {
			swapFlag = 0;
		}

		image = (ImageRec *)malloc(sizeof(ImageRec));
		if (image == NULL) {
			fprintf(stderr, "Out of memory!\n");
			exit(1);
		}
		if ((image->file = fopen(fileName, "rb")) == NULL) {
			perror(fileName);
			exit(1);
		}

		int bytesRead = fread(image, 1, 12, image->file);
        
    if (!bytesRead) {
      fprintf(stderr, "fread failed!\n");
    }
        /**if (image == NULL) { //image cannot be null here, it has been used!
            fprintf(stderr, "image == NULL!\n");
            return (ImageRec *)malloc(sizeof(ImageRec));
        } **/

		if (swapFlag) {
			ConvertShort(&image->imagic, 6);
		}

        
        const unsigned int colourBuffSize=image->xsize*256u;
		    image->tmp = (unsigned char *)malloc(colourBuffSize);
		    image->tmpR = (unsigned char *)malloc(colourBuffSize);
		    image->tmpG = (unsigned char *)malloc(colourBuffSize);
		    image->tmpB = (unsigned char *)malloc(colourBuffSize);
		    if (image->tmp == NULL || image->tmpR == NULL || image->tmpG == NULL ||
			    	image->tmpB == NULL) {
			    fprintf(stderr, "Out of memory!\n");
			    exit(1);
		    }
        
    //should test upper limits on x here...but what is sensible? 1Mb? 100Mb?
		if ((image->type & 0xFF00) == 0x0100) {
			size_t x = ((size_t)image->ysize * (size_t)image->zsize) * sizeof(unsigned);
			image->rowStart = (unsigned *)malloc(x);
			image->rowSize = (int *)malloc(x);
			if (image->rowStart == NULL || image->rowSize == NULL) {
				fprintf(stderr, "Out of memory!\n"); 
				exit(1);
			}
			image->rleEnd = 512 + (2 * x);
			const int fseekRetVal= fseek(image->file, 512, SEEK_SET);
      if (fseekRetVal !=0){
        fprintf(stderr, "Something very wrong with fseek near line 205 of VP1QtInventorUtils.cxx");
      }
			size_t bytesRead = 0;
			bytesRead = fread(image->rowStart, 1, x, image->file);
			VP1Msg::messageDebug("bytesRead(rowStart): " + QString::number(bytesRead));
			bytesRead = fread(image->rowSize, 1, x, image->file);
			VP1Msg::messageDebug("bytesRead(rowSize): " + QString::number(bytesRead));

			if (swapFlag) {
				ConvertLong(image->rowStart, x/(int)sizeof(unsigned));
				ConvertLong((unsigned *)image->rowSize, x/(int)sizeof(int));
			}
		} else {
			image->rowStart = NULL;
			image->rowSize = NULL;
		}
		return image;
	}

	static void ImageClose(ImageRec *image)
	{
		fclose(image->file);
		free(image->tmp);
		free(image->tmpR);
		free(image->tmpG);
		free(image->tmpB);
		free(image->rowSize);
		free(image->rowStart);
		free(image);
	}

	static void ImageGetRow(ImageRec *image,
			unsigned char *buf, int y, int z)
	{
		unsigned char *iPtr, *oPtr, pixel;
		int count;

		if (image) {
			if ((image->type & 0xFF00) == 0x0100) {

                int okseek = fseek(image->file, (long)image->rowStart[y+z*image->ysize], SEEK_SET);
				int okread = fread(image->tmp, 1, (unsigned int)image->rowSize[y+z*image->ysize],
						image->file);

                if( !okseek || !okread ) VP1Msg::messageDebug("fseek or fread failed!!");

				iPtr = image->tmp;
				oPtr = buf;
				for (;;) {
					pixel = *iPtr++;
					count = (int)(pixel & 0x7F);
					if (!count) {
						return;
					}
					if (pixel & 0x80) {
						while (count--) {
							*oPtr++ = *iPtr++;
						}
					} else {
						pixel = *iPtr++;
						while (count--) {
							*oPtr++ = pixel;
						}
					}
				}
			} else {
			      const unsigned int yDim(y*image->xsize), zDim(z*image->xsize*image->ysize);
            int okstatus = fseek(image->file, 512u+yDim+zDim, SEEK_SET);
            if (okstatus) { VP1Msg::messageDebug("fseek failed!!"); }

			size_t bytesRead = 0;
			bytesRead = fread(buf, 1, image->xsize, image->file);
			VP1Msg::messageDebug("bytesRead(buf): " + QString::number(bytesRead));

		}
		}
		else {
			std::cout << "Warning! ImageGetRow() - no 'image'..." << std::endl;
		}
	}

	static unsigned *read_texture(const char *name, int *width, int *height, int *components)
	{
		unsigned *base, *lptr;
		unsigned char *rbuf, *gbuf, *bbuf, *abuf;
		ImageRec *image;
		int y;

		image = ImageOpen(name);

		if(!image)
			return NULL;
        
		(*width)=image->xsize;
		(*height)=image->ysize;
		(*components)=image->zsize;
		const unsigned int imageWidth =  image->xsize;
		const unsigned int imageHeight = image->ysize;
		const unsigned int uintSize(sizeof(unsigned)), ucharSize(sizeof(unsigned char));
		const unsigned int colourBufSize=imageWidth*ucharSize;
		base = (unsigned *)malloc(imageWidth*imageHeight*uintSize);
		rbuf = (unsigned char *)malloc(colourBufSize);
		gbuf = (unsigned char *)malloc(colourBufSize);
		bbuf = (unsigned char *)malloc(colourBufSize);
		abuf = (unsigned char *)malloc(colourBufSize);
		if(!base || !rbuf || !gbuf || !bbuf) {
			ImageClose(image);
			if (base) free(base);
			if (rbuf) free(rbuf);
			if (gbuf) free(gbuf);
			if (bbuf) free(bbuf);
			if (abuf) free(abuf);
			return NULL;
		}
		lptr = base;
		for (y=0; y<image->ysize; ++y) {
			if (image->zsize>=4) {
				ImageGetRow(image,rbuf,y,0);
				ImageGetRow(image,gbuf,y,1);
				ImageGetRow(image,bbuf,y,2);
				ImageGetRow(image,abuf,y,3);
				rgbatorgba(rbuf,gbuf,bbuf,abuf,(unsigned char *)lptr,image->xsize);
				lptr += image->xsize;
			} else if(image->zsize==3) {
				ImageGetRow(image,rbuf,y,0);
				ImageGetRow(image,gbuf,y,1);
				ImageGetRow(image,bbuf,y,2);
				rgbtorgba(rbuf,gbuf,bbuf,(unsigned char *)lptr,image->xsize);
				lptr += image->xsize;
			} else if(image->zsize==2) {
				ImageGetRow(image,rbuf,y,0);
				ImageGetRow(image,abuf,y,1);
				latorgba(rbuf,abuf,(unsigned char *)lptr,image->xsize);
				lptr += image->xsize;
			} else {
				ImageGetRow(image,rbuf,y,0);
				bwtorgba(rbuf,(unsigned char *)lptr,image->xsize);
				lptr += image->xsize;
			}
		}
		ImageClose(image);
		free(rbuf);
		free(gbuf);
		free(bbuf);
		free(abuf);

		return (unsigned *) base;
	}

	//read/write scenegraphs:
	static char * buffer;
	static size_t buffer_size;
	static void * buffer_realloc(void * bufptr, size_t size);
	static QString buffer_writeaction(SoNode * root);
	static void buffer_vrmlwriteaction(SoNode * root, const QString& filename);

	static bool lineWidthAndPointSizeNeedsInit;
	static double allowedLineWidthMin;
	static double allowedLineWidthMax;
	static double allowedLineWidthGranularity;
	static double allowedPointSizeMin;
	static double allowedPointSizeMax;
	static double allowedPointSizeGranularity;

	//Prerender callback:
	//  static void prerendercallback_rendertoimage( void * userdata, class SoGLRenderAction * action );

};

bool VP1QtInventorUtils::Imp::lineWidthAndPointSizeNeedsInit = true;
double VP1QtInventorUtils::Imp::allowedLineWidthMin = -1.0;
double VP1QtInventorUtils::Imp::allowedLineWidthMax = -1.0;
double VP1QtInventorUtils::Imp::allowedLineWidthGranularity = -1.0;
double VP1QtInventorUtils::Imp::allowedPointSizeMin = -1.0;
double VP1QtInventorUtils::Imp::allowedPointSizeMax = -1.0;
double VP1QtInventorUtils::Imp::allowedPointSizeGranularity = -1.0;

//____________________________________________________________________
VP1QtInventorUtils::VP1QtInventorUtils()
{
}

//____________________________________________________________________
VP1QtInventorUtils::~VP1QtInventorUtils()
{
}

//____________________________________________________________________
QPixmap VP1QtInventorUtils::pixmapFromRGBFile(const QString& filename)
{
	return QPixmap::fromImage(imageFromRGBFile(filename));

}

//____________________________________________________________________
QImage VP1QtInventorUtils::imageFromRGBFile(const QString& filename)
{
	int width = 0;
    int height = 0;
    int components = 0;

	unsigned * imagedata = Imp::read_texture(filename.toStdString().c_str(), &width, &height, &components);
    if( width == 0 || height == 0 ) std::cout << "VP1QtInventorUtils::imageFromRGBFile - read_texture failed?" << std::endl;

	unsigned char * data = reinterpret_cast<unsigned char*>(imagedata);


	QImage im(width,height, ( components <= 3 ? QImage::Format_RGB32 : QImage::Format_ARGB32 ) );

	int x, y, index = 0;
	for (y=0; y<height; ++y) {
		for (x=0; x<width; ++x) {
			//Fixme: Does this also work for components=1,2 4??
			im.setPixel ( x, height-y-1, QColor( static_cast<int>(data[index]),static_cast<int>(data[index+1]),static_cast<int>(data[index+2]),static_cast<int>(data[index+3]) ).rgb() );
			index+=4;
		}
	}
	free(imagedata);
	return im;
}

//____________________________________________________________________
//void VP1QtInventorUtils::Imp::prerendercallback_rendertoimage( void * userdata, class SoGLRenderAction * action )
//{
//  std::cout<<"TKTEST LALA VP1QtInventorUtils::Imp::prerendercallback_rendertoimage"<<std::endl;
//  glEnable(GL_MULTISAMPLE);
//  std::cout<<"isSmoothing: "<<action->isSmoothing()<<std::endl;
//}


//____________________________________________________________________
//QImage VP1QtInventorUtils::renderToImage(SoQtRenderArea *ra, int pixels_x, int pixels_y,
QImage VP1QtInventorUtils::renderToImage(VP1ExaminerViewer *ra, int pixels_x, int pixels_y,
		bool transparent_background, double actualRenderedSizeFact )
{
	VP1Msg::messageVerbose("VP1QtInventorUtils::renderToImage()");

	if (!ra)
		return QImage();


	//  transp,anti: Render two large, figure out transp, then resize (gives best result)
	//  transp     : Render two normal, then figure out transp.
	//             : Render one normal.
	//  anti       : Render one large, resize.

	if (actualRenderedSizeFact!=1.0&&!transparent_background) {
		return renderToImage(ra,
				static_cast<int>(pixels_x*actualRenderedSizeFact+0.5),
				static_cast<int>(pixels_y*actualRenderedSizeFact+0.5),
				false,
				1.0)
				.scaled(pixels_x,pixels_y,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	}

	if (transparent_background) {
		//Lets make it transparent. We do this by rendering with both
		//white and black background, and using the two results to figure
		//out the final result.

		SbColor save_bgd = ra->getBackgroundColor();
		SbBool save_redraw = ra->isAutoRedraw();

		ra->setAutoRedraw(false);


		QImage im_black_bgd, im_white_bgd;
		if (actualRenderedSizeFact==1.0) {
			ra->setBackgroundColor(SbColor(0.0,0.0,0.0));
			im_black_bgd = renderToImage(ra, pixels_x, pixels_y,false,1.0);
			ra->setBackgroundColor(SbColor(1.0,1.0,1.0));
			im_white_bgd = renderToImage(ra, pixels_x, pixels_y,false,1.0);
		} else {
			ra->setBackgroundColor(SbColor(0.0,0.0,0.0));
			im_black_bgd = renderToImage(ra, static_cast<int>(pixels_x*actualRenderedSizeFact+0.5), static_cast<int>(pixels_y*actualRenderedSizeFact+0.5),false,1.0);
			ra->setBackgroundColor(SbColor(1.0,1.0,1.0));
			im_white_bgd = renderToImage(ra, static_cast<int>(pixels_x*actualRenderedSizeFact+0.5), static_cast<int>(pixels_y*actualRenderedSizeFact+0.5),false,1.0);
		}

		ra->setBackgroundColor(save_bgd);
		ra->setAutoRedraw(save_redraw);

		if (actualRenderedSizeFact==1.0)
			return Imp::constructImageWithTransparentBackground(im_black_bgd, im_white_bgd);
		else
			return Imp::constructImageWithTransparentBackground(im_black_bgd, im_white_bgd)
		.scaled(pixels_x,pixels_y,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	}

	// DEFAULT CALL

	//debug
	int off = ra->getStereoOffsetSlot();
	int typeSt = ra->getStereoTypeSlot();
	VP1Msg::messageVerbose("off: " + QString::number( off ) + " - type: " + QString::number( typeSt ) );

	// get the scenegraph
	SoNode *root = ra->getSceneManager()->getSceneGraph();
	VP1Msg::messageVerbose("got the scenegraph");
	//std::cout << "root: " << root << std::endl;

	// get the overlay scenegraph
//	SoNode *rootOverlay = ra->getOverlaySceneManager()->getSceneGraph();
	SoNode *rootOverlay = ra->getOverlaySceneGraph();
	VP1Msg::messageVerbose("got the overlay scenegraph");
	//std::cout << "overlay root: " << rootOverlay << std::endl;

	// set a new viewport to the preferred size
	SbViewportRegion myViewport;
	myViewport.setWindowSize(SbVec2s(pixels_x,pixels_y));

	// init the random number generator a get a random filename for the temp file
	qsrand(QTime::currentTime().msecsTo(QTime(0,0)));
	QString tmppath(QDir::tempPath());
	if (!tmppath.endsWith(QDir::separator()))
		tmppath+=QDir::separator();
	QString tmpfile = tmppath+"vp1tmpfile" +QString::number(qrand())+QString::number(qrand())+".rgb";

	// declare a new renderer with the viewport created above
	SoOffscreenRenderer *myRenderer = new SoOffscreenRenderer(myViewport);

	//Copy settings from the render area:
	myRenderer->setBackgroundColor(ra->getBackgroundColor());


	myRenderer->setComponents(SoOffscreenRenderer::RGB_TRANSPARENCY);
	myRenderer->getGLRenderAction()->setTransparencyType(ra->getTransparencyType());
	//  myRenderer->getGLRenderAction()->addPreRenderCallback( VP1QtInventorUtils::Imp::prerendercallback_rendertoimage, 0/*userdata*/ );

	// Anti-Aliasing
	SbBool smoothing; int numPasses;
	ra->getAntialiasing (smoothing, numPasses);
	myRenderer->getGLRenderAction()->setSmoothing (smoothing);
	myRenderer->getGLRenderAction()->setNumPasses(numPasses);

	//Other things we could set:
	//  Overlay scenegraph.

	// render the scenegraph
	// if fails, delete the renderer and return an empty image
	if (!myRenderer->render(root)) {
		delete myRenderer;
		return QImage();
	}
	VP1Msg::messageVerbose("rendered the scenegraph");

	// render the overlay scenegraph
	// if fails, delete the renderer and return an empty image
	if (rootOverlay) {
		bool okOver = myRenderer->render(rootOverlay);
		if ( !okOver) {
			delete myRenderer;
			return QImage();
		}
		else {
			VP1Msg::messageVerbose("rendered the overlay scenegraph");
		}
	}

	// write the rendered image to the temp file
	// if fails, remove the temp file and return an empty image
	if (!myRenderer->writeToRGB(tmpfile.toStdString().c_str())) {
		if (QFile::exists(tmpfile))
			QFile(tmpfile).remove();
		delete myRenderer;
		return QImage();
	}

	// delete the renderer
	delete myRenderer;

	// get the rendered image from the temp file as a Qt QImage instance
	QImage im(imageFromRGBFile(tmpfile));

	// delete the temp file
	if (QFile::exists(tmpfile))
		QFile(tmpfile).remove();

	// return the rendered image
	return im;
}

//____________________________________________________________________
//QPixmap VP1QtInventorUtils::renderToPixmap(SoQtRenderArea *ra, int pixels_x, int pixels_y,
QPixmap VP1QtInventorUtils::renderToPixmap(VP1ExaminerViewer *ra, int pixels_x, int pixels_y,
		bool transparent_background, double actualRenderedSizeFact )
{
	return QPixmap::fromImage(renderToImage(ra, pixels_x, pixels_y, transparent_background, actualRenderedSizeFact));
}

//____________________________________________________________________
QImage VP1QtInventorUtils::Imp::constructImageWithTransparentBackground(const QImage& im_black_bgd, const QImage& im_white_bgd)
{
	if (im_black_bgd.isNull()||im_white_bgd.isNull()||im_black_bgd.size()!=im_white_bgd.size())
		return QImage();

	QImage im(im_black_bgd.size(),QImage::Format_ARGB32);

	int width = im.width();
	int height = im.height();
	QRgb white = qRgba(255,255,255,255);
	QRgb black = qRgba(0,0,0,255);

	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y) {
			if (im_black_bgd.pixel(x,y)==im_white_bgd.pixel(x,y)) {
				im.setPixel(x,y,im_white_bgd.pixel(x,y));
			} else if (im_black_bgd.pixel(x,y)==black&&im_white_bgd.pixel(x,y)==white) {
				im.setPixel(x,y,Qt::transparent);
			} else {
				//Calculate ...
				QColor pix_b = QColor(im_black_bgd.pixel(x,y));
				QColor pix_w = QColor(im_white_bgd.pixel(x,y));
				qreal alpha = 1.0 - pix_w.redF() + pix_b.redF();
				if (alpha==0) {
					im.setPixel(x,y,Qt::transparent);
				} else {
					im.setPixel(x,y,qRgba(static_cast<int>(pix_b.redF()/alpha*255+0.5),
							static_cast<int>(pix_b.greenF()/alpha*255+0.5),
							static_cast<int>(pix_b.blueF()/alpha*255+0.5),
							static_cast<int>(alpha*255+0.5)));
				}
			}
		}

	return im;
}


//____________________________________________________________________
SoGLRenderAction::TransparencyType VP1QtInventorUtils::getDefaultVP1TransparencyType()
{
	return SoGLRenderAction::DELAYED_BLEND;
}

//____________________________________________________________________
QList<SoGLRenderAction::TransparencyType> VP1QtInventorUtils::getAllTransparencyTypes()
{
	QList<SoGLRenderAction::TransparencyType> l;
	l << SoGLRenderAction::NONE
			<< SoGLRenderAction::SCREEN_DOOR
			<< SoGLRenderAction::ADD
			<< SoGLRenderAction::DELAYED_ADD
			<< SoGLRenderAction::SORTED_OBJECT_ADD
			<< SoGLRenderAction::BLEND
			<< SoGLRenderAction::DELAYED_BLEND
			<< SoGLRenderAction::SORTED_OBJECT_BLEND
			<< SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_ADD
			<< SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND
			<< SoGLRenderAction::SORTED_LAYERS_BLEND;
	return l;
}

//____________________________________________________________________
int VP1QtInventorUtils::transparencyTypeToInt( SoGLRenderAction::TransparencyType tt )
{
	switch (tt) {
	case SoGLRenderAction::SCREEN_DOOR: return 0;
	case SoGLRenderAction::ADD: return 1;
	case SoGLRenderAction::DELAYED_ADD: return 2;
	case SoGLRenderAction::SORTED_OBJECT_ADD: return 3;
	case SoGLRenderAction::BLEND: return 4;
	case SoGLRenderAction::DELAYED_BLEND: return 5;
	case SoGLRenderAction::SORTED_OBJECT_BLEND: return 6;
	case SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_ADD: return 7;
	case SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND: return 8;
	case SoGLRenderAction::NONE: return 9;
	case SoGLRenderAction::SORTED_LAYERS_BLEND: return 10;
	default:
		VP1Msg::messageDebug("VP1QtInventorUtils::transparencyTypeToInt ERROR: Unknown transparency type");
		return -1;
	}
}
//____________________________________________________________________
SoGLRenderAction::TransparencyType VP1QtInventorUtils::intToTransparencyType( int i )
{
	switch (i) {
	case 0: return SoGLRenderAction::SCREEN_DOOR;
	case 1: return SoGLRenderAction::ADD;
	case 2: return SoGLRenderAction::DELAYED_ADD;
	case 3: return SoGLRenderAction::SORTED_OBJECT_ADD;
	case 4: return SoGLRenderAction::BLEND;
	case 5: return SoGLRenderAction::DELAYED_BLEND;
	case 6: return SoGLRenderAction::SORTED_OBJECT_BLEND;
	case 7: return SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_ADD;
	case 8: return SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND;
	case 9: return SoGLRenderAction::NONE;
	case 10: return SoGLRenderAction::SORTED_LAYERS_BLEND;
	default:
		VP1Msg::messageDebug("VP1QtInventorUtils::intToTransparencyType ERROR: int out of range "+VP1Msg::str(i));
		return SoGLRenderAction::DELAYED_BLEND;
	}
}


//____________________________________________________________________
QString VP1QtInventorUtils::transparencyType2PrettyString( SoGLRenderAction::TransparencyType tt )
{
	switch (tt) {
	case SoGLRenderAction::DELAYED_BLEND: return "Delayed blend"; break;
	case SoGLRenderAction::SCREEN_DOOR: return "Screen door"; break;
	case SoGLRenderAction::ADD: return "Add"; break;
	case SoGLRenderAction::DELAYED_ADD: return "Delayed add"; break;
	case SoGLRenderAction::SORTED_OBJECT_ADD: return "Sorted object add"; break;
	case SoGLRenderAction::BLEND: return "Blend (Best for Geo volumes)"; break;
	case SoGLRenderAction::SORTED_OBJECT_BLEND: return "Sorted object blend (Best for physics objects: jets, tracks, ...)"; break;
	case SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_ADD: return "Sorted object sorted triangle add"; break;
	case SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND: return "Sorted object sorted triangle blend"; break;
	case SoGLRenderAction::NONE: return "None"; break;
	case SoGLRenderAction::SORTED_LAYERS_BLEND: return "Sorted layers blend"; break;
	default: return "";
	}

}

//____________________________________________________________________
QByteArray VP1QtInventorUtils::serialize( const SbRotation& rot )
{

	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	//Write data:

	float q0,q1,q2,q3;//quarternion components
	rot.getValue (q0,q1,q2,q3);

	out<<(double)q0;
	out<<(double)q1;
	out<<(double)q2;
	out<<(double)q3;

	if (VP1Msg::verbose()) {
		//Fixme: check for nan's.
		VP1Msg::messageVerbose("VP1QtInventorUtils::serialize SbRotation(q0,q1,q2,q3) = ("
				+QString::number(q0)+", "+QString::number(q1)+", "
				+QString::number(q2)+", "+QString::number(q3)+")");
	}

	// ===> Finish up:
	buffer.close();
	return byteArray;
}

//____________________________________________________________________
bool VP1QtInventorUtils::deserialize( QByteArray& ba, SbRotation& rot )
{
	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	///////////////////////////////////////////
	if(ba.size()==16) {
		// Single precision
		state.setFloatingPointPrecision(QDataStream::SinglePrecision);
		float q0,q1,q2,q3;//quarternion components

		state >> q0;
		state >> q1;
		state >> q2;
		state >> q3;

		rot.setValue (q0,q1,q2,q3);

		if (VP1Msg::verbose()) {
			//Fixme: check for nan's.
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserialize SbRotation(q0,q1,q2,q3) = ("
					+QString::number(q0)+", "+QString::number(q1)+", "
					+QString::number(q2)+", "+QString::number(q3)+")");
		}
	}
	else {
		// Double precision
		double q0,q1,q2,q3;//quarternion components

		state >> q0;
		state >> q1;
		state >> q2;
		state >> q3;

		rot.setValue (q0,q1,q2,q3);

		if (VP1Msg::verbose()) {
			//Fixme: check for nan's.
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserialize SbRotation(q0,q1,q2,q3) = ("
					+QString::number(q0)+", "+QString::number(q1)+", "
					+QString::number(q2)+", "+QString::number(q3)+")");
		}
	}

	// ===> Finish up:
	buffer.close();

	return true;//Fixme: How to check for errors? - at least check for nan's and determinant?
}

//____________________________________________________________________
QByteArray VP1QtInventorUtils::serialize( const SbVec3f& vec )
{
	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	//Write data:
	float x,y,z;
	vec.getValue(x,y,z);
	out << (double)x;
	out << (double)y;
	out << (double)z;

	if (VP1Msg::verbose()) {
		//Fixme: check for nan's.
		VP1Msg::messageVerbose("VP1QtInventorUtils::serialize SbVec3f(x,y,z) = ("
				+QString::number(x)+", "+QString::number(y)+", "+QString::number(z)+")");
	}

	// ===> Finish up:
	buffer.close();
	return byteArray;
}

//____________________________________________________________________
bool VP1QtInventorUtils::deserialize( QByteArray& ba, SbVec3f& vec )
{
	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	///////////////////////////////////////////
	if(ba.size()==12) {
		// Single precision
		state.setFloatingPointPrecision(QDataStream::SinglePrecision);
		float x,y,z;

		state >> x;
		state >> y;
		state >> z;

		vec.setValue (x,y,z);

		if (VP1Msg::verbose()) {
			//Fixme: check for nan's.
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserialize SbVec3f(x,y,z) = ("
					+QString::number(x)+", "+QString::number(y)+", "+QString::number(z)+")");
		}
	}
	else {
		double x,y,z;

		state >> x;
		state >> y;
		state >> z;

		vec.setValue (x,y,z);

		if (VP1Msg::verbose()) {
			//Fixme: check for nan's.
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserialize SbVec3f(x,y,z) = ("
					+QString::number(x)+", "+QString::number(y)+", "+QString::number(z)+")");
		}
	}

	// ===> Finish up:
	buffer.close();

	return true;//Fixme: How to check for errors?  - at least check for nan's

}

//____________________________________________________________________
QByteArray VP1QtInventorUtils::serializeSoCameraParameters( const SoCamera& cam ) {

	VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters start");

	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	cam.ref();
	//Write data:
	SbRotation camrot = cam.orientation.getValue();
	out << serialize(camrot);
	SbVec3f campos = cam.position.getValue();
	out << serialize(campos);
	float f_aspectRatio(cam.aspectRatio.getValue());
	float f_nearDistance(cam.nearDistance.getValue());
	float f_farDistance(cam.farDistance.getValue());
	float f_focalDistance(cam.focalDistance.getValue());
	out << (double)f_aspectRatio;
	out << (double)f_nearDistance;
	out << (double)f_farDistance;
	out << (double)f_focalDistance;

	int viewportmap(-1);
	switch (cam.viewportMapping.getValue()) {
	case SoCamera::CROP_VIEWPORT_FILL_FRAME: viewportmap = 0;break;
	case SoCamera::CROP_VIEWPORT_LINE_FRAME: viewportmap = 1;break;
	case SoCamera::CROP_VIEWPORT_NO_FRAME: viewportmap = 2;break;
	case SoCamera::ADJUST_CAMERA: viewportmap = 3;break;
	case SoCamera::LEAVE_ALONE: viewportmap = 4;break;
	}
	out << viewportmap;

	//Camera type and specialised info:
	int camtype (-1);
	if (cam.getTypeId().isDerivedFrom(SoPerspectiveCamera::getClassTypeId()))
		camtype = 0;
	else if (cam.getTypeId().isDerivedFrom(SoOrthographicCamera::getClassTypeId()))
		camtype = 1;

	out <<camtype;
	if (camtype==0) {
		out << (double)static_cast<const SoPerspectiveCamera*>(&cam)->heightAngle.getValue();
	} else if (camtype==1) {
		out << (double)static_cast<const SoOrthographicCamera*>(&cam)->height.getValue();
	}

	cam.unrefNoDelete();

	// ===> Finish up:
	buffer.close();

	if (VP1Msg::verbose()) {
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters aspectRatio = "+QString::number(f_aspectRatio));
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters nearDistance = "+QString::number(f_nearDistance));
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters farDistance = "+QString::number(f_farDistance));
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters focalDistance = "+QString::number(f_focalDistance));
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters viewportmap = "+QString::number(viewportmap));
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters camtype = "
				+QString(camtype==0?"perspective":(camtype==1?"orthographic":"unknown")));
		if (camtype==0)
			VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters (persp) heightAngle = "
					+QString::number(static_cast<const SoPerspectiveCamera*>(&cam)->heightAngle.getValue()));
		if (camtype==1)
			VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters (ortho) height = "
					+QString::number(static_cast<const SoOrthographicCamera*>(&cam)->height.getValue()));
		VP1Msg::messageVerbose("VP1QtInventorUtils::serializeSoCameraParameters end");
	}

	return byteArray;
}

//____________________________________________________________________
bool VP1QtInventorUtils::deserializeSoCameraParameters( QByteArray& ba, SoCamera& cam )
{
	VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters start");
	if (ba==QByteArray())
		return false;

	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	///////////////////////////////////////////
	if(ba.size()==64) {
		// Single precision
		state.setFloatingPointPrecision(QDataStream::SinglePrecision);

		//Orientation:
		SbRotation rot;  QByteArray ba_rot; state >> ba_rot;
		if (!deserialize(ba_rot,rot)) return false;
		//position:
		SbVec3f pos;  QByteArray ba_pos; state >> ba_pos;
		if (!deserialize(ba_pos,pos)) return false;

		bool save = cam.enableNotify(false);
		cam.ref();
		cam.orientation.setValue(rot);
		cam.position.setValue(pos);
		//Misc:
		float f_aspectRatio, f_nearDistance, f_farDistance, f_focalDistance;

		state >> f_aspectRatio; cam.aspectRatio.setValue(f_aspectRatio);
		state >> f_nearDistance; cam.nearDistance.setValue(f_nearDistance);
		state >> f_farDistance; cam.farDistance.setValue(f_farDistance);
		state >> f_focalDistance; cam.focalDistance.setValue(f_focalDistance);
		//viewport mapping:
		int viewportmap;
		state>>viewportmap;
		switch (viewportmap) {
		case 0: cam.viewportMapping.setValue(SoCamera::CROP_VIEWPORT_FILL_FRAME); break;
		case 1: cam.viewportMapping.setValue(SoCamera::CROP_VIEWPORT_LINE_FRAME);break;
		case 2: cam.viewportMapping.setValue(SoCamera::CROP_VIEWPORT_NO_FRAME);break;
		case 3: cam.viewportMapping.setValue(SoCamera::ADJUST_CAMERA);break;
		case 4: cam.viewportMapping.setValue(SoCamera::LEAVE_ALONE);break;
		//ERROR
		}

		bool passedcameraisperspective = cam.getTypeId().isDerivedFrom(SoPerspectiveCamera::getClassTypeId());

		//Camera type and specialised info:
		int camtype;
		state>>camtype;
		float f_orthopersp_heightpar(-999);
		if (camtype==0) {
			//perspective
			if (!passedcameraisperspective)
				return false;
			state >> f_orthopersp_heightpar;
			static_cast<SoPerspectiveCamera*>(&cam)->heightAngle.setValue(f_orthopersp_heightpar);
		} else if (camtype==1) {
			//ortho
			if (passedcameraisperspective)
				return false;
			state >> f_orthopersp_heightpar;
			static_cast<SoOrthographicCamera*>(&cam)->height.setValue(f_orthopersp_heightpar);
		}

		if (save) {
			cam.enableNotify(true);
			cam.touch();
		}

		// ===> Finish up:
		buffer.close();

		if (VP1Msg::verbose()) {
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters aspectRatio = "+QString::number(f_aspectRatio));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters nearDistance = "+QString::number(f_nearDistance));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters farDistance = "+QString::number(f_farDistance));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters focalDistance = "+QString::number(f_focalDistance));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters viewportmap = "+QString::number(viewportmap));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters camtype = "
					+QString(camtype==0?"perspective":(camtype==1?"orthographic":"unknown")));
			if (camtype==0)
				VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters (persp) heightAngle = "
						+QString::number(f_orthopersp_heightpar));
			if (camtype==1)
				VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters (ortho) height = "
						+QString::number(f_orthopersp_heightpar));

		}
	}
	else {
		// Double precision

		//Orientation:
		SbRotation rot;  QByteArray ba_rot; state >> ba_rot;
		if (!deserialize(ba_rot,rot)) return false;
		//position:
		SbVec3f pos;  QByteArray ba_pos; state >> ba_pos;
		if (!deserialize(ba_pos,pos)) return false;

		bool save = cam.enableNotify(false);
		cam.ref();
		cam.orientation.setValue(rot);
		cam.position.setValue(pos);
		//Misc:
		double f_aspectRatio, f_nearDistance, f_farDistance, f_focalDistance;

		state >> f_aspectRatio; cam.aspectRatio.setValue(f_aspectRatio);
		state >> f_nearDistance; cam.nearDistance.setValue(f_nearDistance);
		state >> f_farDistance; cam.farDistance.setValue(f_farDistance);
		state >> f_focalDistance; cam.focalDistance.setValue(f_focalDistance);
		//viewport mapping:
		int viewportmap;
		state>>viewportmap;
		switch (viewportmap) {
		case 0: cam.viewportMapping.setValue(SoCamera::CROP_VIEWPORT_FILL_FRAME); break;
		case 1: cam.viewportMapping.setValue(SoCamera::CROP_VIEWPORT_LINE_FRAME);break;
		case 2: cam.viewportMapping.setValue(SoCamera::CROP_VIEWPORT_NO_FRAME);break;
		case 3: cam.viewportMapping.setValue(SoCamera::ADJUST_CAMERA);break;
		case 4: cam.viewportMapping.setValue(SoCamera::LEAVE_ALONE);break;
		//ERROR
		}

		bool passedcameraisperspective = cam.getTypeId().isDerivedFrom(SoPerspectiveCamera::getClassTypeId());

		//Camera type and specialised info:
		int camtype;
		state>>camtype;
		double f_orthopersp_heightpar(-999);
		if (camtype==0) {
			//perspective
			if (!passedcameraisperspective)
				return false;
			state >> f_orthopersp_heightpar;
			static_cast<SoPerspectiveCamera*>(&cam)->heightAngle.setValue(f_orthopersp_heightpar);
		} else if (camtype==1) {
			//ortho
			if (passedcameraisperspective)
				return false;
			state >> f_orthopersp_heightpar;
			static_cast<SoOrthographicCamera*>(&cam)->height.setValue(f_orthopersp_heightpar);
		}

		if (save) {
			cam.enableNotify(true);
			cam.touch();
		}

		// ===> Finish up:
		buffer.close();

		if (VP1Msg::verbose()) {
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters aspectRatio = "+QString::number(f_aspectRatio));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters nearDistance = "+QString::number(f_nearDistance));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters farDistance = "+QString::number(f_farDistance));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters focalDistance = "+QString::number(f_focalDistance));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters viewportmap = "+QString::number(viewportmap));
			VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters camtype = "
					+QString(camtype==0?"perspective":(camtype==1?"orthographic":"unknown")));
			if (camtype==0)
				VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters (persp) heightAngle = "
						+QString::number(f_orthopersp_heightpar));
			if (camtype==1)
				VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters (ortho) height = "
						+QString::number(f_orthopersp_heightpar));

		}
	}

	cam.unrefNoDelete();
	VP1Msg::messageVerbose("VP1QtInventorUtils::deserializeSoCameraParameters end");
	return true;
}

//____________________________________________________________________
SbColor VP1QtInventorUtils::qcol2sbcol(const QColor& col)
{
	return SbColor( col.red()/255.0, col.green()/255.0, col.blue()/255.0 );
}

//____________________________________________________________________
QColor VP1QtInventorUtils::sbcol2qcol(const SbColor& col)
{
	float r,g,b;
	col.getValue(r,g,b);
	return QColor::fromRgbF( r,g,b );
}

//____________________________________________________________________
QByteArray VP1QtInventorUtils::serialiseSoMaterial(SoMaterial *m)
{
	if (!m||m->ambientColor.getNum()!=1
			||m->diffuseColor.getNum()!=1
			||m->specularColor.getNum()!=1
			||m->emissiveColor.getNum()!=1
			||m->transparency.getNum()!=1
			||m->shininess.getNum()!=1) {
		VP1Msg::message("VP1QtInventorUtils::serialiseSoMaterial Error: "
				"Passed material must have exactly one value in each of the 6 fields!!");
		return QByteArray();
	}


	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	//Write data:
	out << QString("somat_v1_begin");
	out << sbcol2qcol(m->ambientColor[0]);
	out << sbcol2qcol(m->diffuseColor[0]);
	out << sbcol2qcol(m->specularColor[0]);
	out << sbcol2qcol(m->emissiveColor[0]);
	out << (double)m->shininess[0];
	out << (double)m->transparency[0];
	out << QString("somat_end");

	// ===> Finish up:
	buffer.close();

	return byteArray;

}

//____________________________________________________________________
bool VP1QtInventorUtils::deserialiseSoMaterial(QByteArray&ba,SoMaterial *&m)
{
	if (!m||m->ambientColor.getNum()!=1
			||m->diffuseColor.getNum()!=1
			||m->specularColor.getNum()!=1
			||m->emissiveColor.getNum()!=1
			||m->transparency.getNum()!=1
			||m->shininess.getNum()!=1) {
		VP1Msg::message("VP1QtInventorUtils::deserialiseSoMaterial Error: "
				"Passed material must have exactly one value in each of the 6 fields!!");
		return false;
	}

	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba);
	buffer.open(QIODevice::ReadOnly);
	QDataStream stream(&buffer);
	if(ba.size()==106)
		stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

	//Read contents while checking for validity
	QString str; stream >> str;
	if (str!="somat_v1_begin")
		return false;

	QColor ambientcol;  stream >> ambientcol;
	if (!ambientcol.isValid())
		return false;

	QColor diffusecol;  stream >> diffusecol;
	if (!diffusecol.isValid())
		return false;

	QColor specularcol;  stream >> specularcol;
	if (!specularcol.isValid())
		return false;

	QColor emissivecol;  stream >> emissivecol;
	if (!emissivecol.isValid())
		return false;

	if(ba.size()==106) {
		// Single precision
		float shininess; stream >> shininess;
		if (shininess<0.0f||shininess>1.0f)
			return false;

		float transparency; stream >> transparency;
		if (transparency<0.0f||transparency>1.0f)
			return false;

		stream >> str;
		if (str!="somat_end")
			return false;

		buffer.close();

		//Apply values:
		m->ambientColor.setValue(qcol2sbcol(ambientcol));
		m->diffuseColor.setValue(qcol2sbcol(diffusecol));
		m->specularColor.setValue(qcol2sbcol(specularcol));
		m->emissiveColor.setValue(qcol2sbcol(emissivecol));
		m->shininess.setValue(shininess);
		m->transparency.setValue(transparency);
	}
	else {
		// Double precision
		double shininess; stream >> shininess;
		if (shininess<0.0||shininess>1.0)
			return false;

		double transparency; stream >> transparency;
		if (transparency<0.0||transparency>1.0)
			return false;

		stream >> str;
		if (str!="somat_end")
			return false;

		buffer.close();

		//Apply values:
		m->ambientColor.setValue(qcol2sbcol(ambientcol));
		m->diffuseColor.setValue(qcol2sbcol(diffusecol));
		m->specularColor.setValue(qcol2sbcol(specularcol));
		m->emissiveColor.setValue(qcol2sbcol(emissivecol));
		m->shininess.setValue(shininess);
		m->transparency.setValue(transparency);
	}

	return true;
}

//____________________________________________________________________
SoNode * VP1QtInventorUtils::createCircle( const double& radius )
{
	SoGroup* grp = new SoGroup;
	grp->ref();

	SoCoordinate4 * coord = new SoCoordinate4;
	const double invsqrttwo=0.707106781186547;

	int icoord(0);
	coord->point.set1Value(icoord++,SbVec4f(1*radius,0,0,1));
	coord->point.set1Value(icoord++,SbVec4f(invsqrttwo*radius,invsqrttwo*radius,0,invsqrttwo));
	coord->point.set1Value(icoord++,SbVec4f(0,1*radius,0,1));
	coord->point.set1Value(icoord++,SbVec4f(-invsqrttwo*radius,invsqrttwo*radius,0,invsqrttwo));
	coord->point.set1Value(icoord++,SbVec4f(-1*radius,0,0,1));
	coord->point.set1Value(icoord++,SbVec4f(-invsqrttwo*radius,-invsqrttwo*radius,0,invsqrttwo));
	coord->point.set1Value(icoord++,SbVec4f(0,-1*radius,0,1));
	coord->point.set1Value(icoord++,SbVec4f(invsqrttwo*radius,-invsqrttwo*radius,0,invsqrttwo));
	coord->point.set1Value(icoord++,SbVec4f(1*radius,0,0,1));

	SoNurbsCurve * curve = new SoNurbsCurve;
	curve->numControlPoints = icoord;

	int iknot(0);

	curve->knotVector.set1Value(iknot++,0);
	curve->knotVector.set1Value(iknot++,0);
	curve->knotVector.set1Value(iknot++,0);
	curve->knotVector.set1Value(iknot++,1);
	curve->knotVector.set1Value(iknot++,1);
	curve->knotVector.set1Value(iknot++,2);
	curve->knotVector.set1Value(iknot++,2);
	curve->knotVector.set1Value(iknot++,3);
	curve->knotVector.set1Value(iknot++,3);
	curve->knotVector.set1Value(iknot++,4);
	curve->knotVector.set1Value(iknot++,4);
	curve->knotVector.set1Value(iknot++,4);
	grp->addChild(coord);
	grp->addChild(curve);

	grp->unrefNoDelete();
	return grp;
}

//____________________________________________________________________
SoNode * VP1QtInventorUtils::createEllipse( const double& radiusX, const double& radiusY, const int& numnodes )
{
	SoVertexProperty *vertices = new SoVertexProperty();

	int iver(0);
	vertices->vertex.set1Value(iver++,radiusX,0.0,0.0);
	for (int i = 1; i < numnodes; i++)
	{
		vertices->vertex.set1Value(iver++,
				cos(2.0*static_cast<double>(i)*M_PI/static_cast<double>(numnodes))*radiusX,
				sin(2.0*static_cast<double>(i)*M_PI/static_cast<double>(numnodes))*radiusY,0.0);
	}
	vertices->vertex.set1Value(iver++,radiusX,0.0,0.0);

	SoLineSet * ellipse = new SoLineSet();
	ellipse->numVertices = iver;
	ellipse->vertexProperty = vertices;

	return ellipse;
}

//_____________________________________________________________________________________
bool VP1QtInventorUtils::changePathTail(SoPath*path,SoNode*commonBranchPoint,SoNode*newtail)
{
	if (!path||!commonBranchPoint||!newtail)
		return false;

	SoSearchAction sa;
	sa.setInterest(SoSearchAction::FIRST);
	sa.setNode(newtail);
	sa.apply(commonBranchPoint);
	//First truncate pickedPath at d->sceneroot, then append
	//newpath to pickedPath:
	SoPath * newpath = sa.getPath();
	if (!newpath)
		return false;
	bool found(false);
	for (int i=0;i<path->getLength();++i) {
		if (path->getNode(i)==commonBranchPoint) {
			found = true;
			path->truncate(i+1);
			break;
		}
	}
	if (found)
		path->append(newpath);
	return found;
}


//_____________________________________________________________________________________
char * VP1QtInventorUtils::Imp::buffer = 0;
size_t VP1QtInventorUtils::Imp::buffer_size = 0;

//_____________________________________________________________________________________
void * VP1QtInventorUtils::Imp::buffer_realloc(void * bufptr, size_t size)
{
	buffer = (char *)realloc(bufptr, size);
	buffer_size = size;
	return buffer;
}

//_____________________________________________________________________________________
QString VP1QtInventorUtils::Imp::buffer_writeaction(SoNode * root)
{
	SoOutput out;
	buffer = (char *)malloc(1024);
	buffer_size = 1024;
	out.setBuffer(buffer, buffer_size, buffer_realloc);

	SoWriteAction wa(&out);
	wa.apply(root);

	QString s(buffer);
	free(buffer);
	return s;
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::Imp::buffer_vrmlwriteaction(SoNode * root, const QString& filename)
{
	SoToVRML2Action vwa;
	
	vwa.apply(root);
	SoVRMLGroup * newroot = vwa.getVRML2SceneGraph();

    SoOutput out;
    out.openFile(qPrintable(filename));
    out.setHeaderString("#VRML V2.0 utf8");
    SoWriteAction wra(&out);
    wra.apply(newroot);
    out.closeFile();
	newroot->unref();
	return;
}

//_____________________________________________________________________________________
bool VP1QtInventorUtils::writeGraphToFile(SoNode*root, const QString& filename)
{
	if (!root)
		return false;

	root->ref();
	QString s = Imp::buffer_writeaction(root);
	root->unrefNoDelete();

	QFile data(filename);
	if (data.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream out(&data);
		out << s << endl;
		return true;
	} else {
		return false;
	}
}

//_____________________________________________________________________________________
SoSeparator* VP1QtInventorUtils::readGraphFromFile(const QString& filename)
{
	//  SoDB::init();
	SoInput in;
	if (!in.openFile(filename.toStdString().c_str()))
		return 0;
	return SoDB::readAll(&in);
}


//_____________________________________________________________________________________
bool VP1QtInventorUtils::writeGraphToVRMLFile(SoNode*root, const QString& filename)
{
	if (!root)
		return false;

	root->ref();
	Imp::buffer_vrmlwriteaction(root, filename);
	root->unrefNoDelete();

	// QFile data(filename);
	// if (data.open(QFile::WriteOnly | QFile::Truncate)) {
	// 	QTextStream out(&data);
	// 	out << s << endl;
	// 	return true;
	// } else {
	// 	return false;
	// }
	return true;
}


/////////////////// OBSOLETE /////////////////////
#include "VP1Base/VP1MaterialButton.h"
//_____________________________________________________________________________________
void VP1QtInventorUtils::setMatColor( SoMaterial * m, const double& r, const double& g, const double& b,
		const double& brightness, const double& transp )
{
	if (m)
		VP1MaterialButton::setMaterialParameters( m, r,g,b,brightness,transp );
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::setMatColor( SoMaterial * m, const QColor& col,
		const double& brightness, const double& transp )
{
	setMatColor( m, col.redF(), col.greenF(), col.blueF(), brightness, transp);
}
/////////////////// OBSOLETE /////////////////////

//_____________________________________________________________________________________
void VP1QtInventorUtils::getLineWidthRanges(double& min, double& max, double& granularity)
{
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	min = Imp::allowedLineWidthMin;
	max = Imp::allowedLineWidthMax;
	granularity = Imp::allowedLineWidthGranularity;
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::getPointSizeRanges(double& min, double& max, double& granularity)
{
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	min = Imp::allowedPointSizeMin;
	max = Imp::allowedPointSizeMax;
	granularity = Imp::allowedPointSizeGranularity;
}

#include "VP1Base/VP1ExaminerViewer.h"

//_____________________________________________________________________________________
void VP1QtInventorUtils::ensureInitLineWidthAndPointSize(  SoQtRenderArea * ra )
{
	if (!Imp::lineWidthAndPointSizeNeedsInit)
		return;
	Imp::lineWidthAndPointSizeNeedsInit = false;
	QWidget * w(0);
	if (!ra) {
		VP1Msg::messageVerbose("VP1QtInventorUtils WARNING: Have to create temporary renderarea for the sole "
				"purpose of getting supported line widths and point sizes!");
		w = new QWidget(0);
		ra = new VP1ExaminerViewer(w);
	}
	SbVec2f range; float granularity;
	ra->getLineWidthLimits(range, granularity);
	float a,b;
	range.getValue(a,b);
	Imp::allowedLineWidthMin = a;
	Imp::allowedLineWidthMax = b;
	Imp::allowedLineWidthGranularity = granularity;
	VP1Msg::messageVerbose("VP1QtInventorUtils Determined line widths supported by hardware (min,max,granularity) = ("
			+VP1Msg::str(a)+", "+VP1Msg::str(b)+", "+VP1Msg::str(granularity)+")");
	ra->getPointSizeLimits(range, granularity);
	range.getValue(a,b);
	Imp::allowedPointSizeMin = a;
	Imp::allowedPointSizeMax = b;
	Imp::allowedPointSizeGranularity = granularity;
	VP1Msg::messageVerbose("VP1QtInventorUtils Determined point sizes supported by hardware (min,max,granularity) = ("
			+VP1Msg::str(a)+", "+VP1Msg::str(b)+", "+VP1Msg::str(granularity)+")");
	if (w) {
		delete ra;
		delete w;
	}
	//We clip to get a more consistent behaviour across hardware (and to limit ourselves to reasonable values:

	if (Imp::allowedLineWidthMin<0.5)
		Imp::allowedLineWidthMin = 0.5;
	if (Imp::allowedLineWidthMax>7.0)
		Imp::allowedLineWidthMax = 7.0;
	if (Imp::allowedPointSizeMin<0.5)
		Imp::allowedPointSizeMin = 0.5;
	if (Imp::allowedPointSizeMax>12.0)
		Imp::allowedPointSizeMax = 12.0;
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::setLimitsLineWidthSlider(QSlider * slider)
{
	if (!slider)
		return;
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	int nsteps = std::min(1000,std::max<int>(0,static_cast<int>((Imp::allowedLineWidthMax-Imp::allowedLineWidthMin)/Imp::allowedLineWidthGranularity)));
	int stepsPerUnit = std::min(nsteps,std::max<int>(1,static_cast<int>(1.0/Imp::allowedLineWidthGranularity)));
	slider->setRange(0,nsteps);
	slider->setSingleStep(1);
	slider->setPageStep(stepsPerUnit);
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::setLimitsPointSizeSlider(QSlider * slider)
{
	if (!slider)
		return;
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	int nsteps = std::min(1000,std::max<int>(0,
			static_cast<int>(0.5+(Imp::allowedPointSizeMax-Imp::allowedPointSizeMin)/Imp::allowedPointSizeGranularity)));
	int stepsPerUnit = std::min(nsteps,std::max<int>(1,
			static_cast<int>(0.5+1.0/Imp::allowedPointSizeGranularity)));
	slider->setRange(0,nsteps);
	slider->setSingleStep(1);
	slider->setPageStep(stepsPerUnit);
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::setValueLineWidthSlider(QSlider * slider, const double& value)
{
	if (!slider)
		return;
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	int itarget = std::min(slider->maximum(),std::max<int>(slider->minimum(),
			static_cast<int>(0.5+(value-Imp::allowedLineWidthMin)/Imp::allowedLineWidthGranularity)));
	if (slider->value()!=itarget)
		slider->setValue(itarget);
}

//_____________________________________________________________________________________
void VP1QtInventorUtils::setValuePointSizeSlider(QSlider * slider, const double& value)
{
	if (!slider)
		return;
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	int itarget = std::min(slider->maximum(),std::max<int>(slider->minimum(),
			static_cast<int>(0.5+(value-Imp::allowedPointSizeMin)/Imp::allowedPointSizeGranularity)));
	if (slider->value()!=itarget)
		slider->setValue(itarget);
}

//_____________________________________________________________________________________
double VP1QtInventorUtils::getValueLineWidthSlider(const QSlider * slider)
{
	if (!slider)
		return 1.0;
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	return std::max(Imp::allowedLineWidthMin,std::min(Imp::allowedLineWidthMax,
			Imp::allowedLineWidthMin+Imp::allowedLineWidthGranularity * slider->value()));
}

//_____________________________________________________________________________________
double VP1QtInventorUtils::getValuePointSizeSlider(const QSlider * slider)
{
	if (!slider)
		return 1.0;
	if (Imp::lineWidthAndPointSizeNeedsInit)
		ensureInitLineWidthAndPointSize(0);
	return std::max(Imp::allowedPointSizeMin,std::min(Imp::allowedPointSizeMax,
			Imp::allowedPointSizeMin+Imp::allowedPointSizeGranularity * slider->value()));
}
