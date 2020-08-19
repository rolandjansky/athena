/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1QtInventorUtils               //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: June 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1QTINVENTORUTILS_H
#define VP1QTINVENTORUTILS_H

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <QString>
#include <QByteArray>
#include <QPixmap>
#include <QImage>

class SoQtRenderArea;
class VP1ExaminerViewer;
class SoNode;
class SoShape;
class SoSeparator;
class SoPath;
class SbRotation;
class SbColor;
class SbVec3f;
class QColor;
class SoCamera;
class SoMaterial;
class QByteArray;
class QSlider;

class VP1QtInventorUtils {
public:

  ////////////////////////////////////////////////////////////
  //             Conversion of colour classes               //
  ////////////////////////////////////////////////////////////

  static SbColor qcol2sbcol(const QColor&);//Ignores alpha channel of QColor
  static QColor sbcol2qcol(const SbColor&);

  //////////////////////////////////////////////////////
  //             How to create a circle               //
  //////////////////////////////////////////////////////

  static SoNode * createCircle( const double& radius );//Creates circle (with ref-count 0)  of given
                                                       //radius in xy plane, centered at the origo.

  //////////////////////////////////////////////////////
  //             How to create an ellipse             //
  //////////////////////////////////////////////////////

  static SoNode * createEllipse( const double& radiusX, const double& radiusY, const int& numnodes = 12 );
                                                    //Creates ellipse (with ref-count 0)  of given
                                                    //radii in xy plane, centered at the origo.

  ////////////////////////////////////////////////////////////
  //                 Off-screen rendering                   //
  ////////////////////////////////////////////////////////////

  //Check .isNull() of returned images to check success/failure.

  //Useful stuff:
//  static QImage renderToImage(SoQtRenderArea *ra, int pixels_x, int pixels_y,
  static QImage renderToImage(VP1ExaminerViewer *ra, int pixels_x, int pixels_y,
			      bool transparent_background = false, double actualRenderedSizeFact = 1.0 );

//  static QPixmap renderToPixmap(SoQtRenderArea *ra, int pixels_x, int pixels_y,
  static QPixmap renderToPixmap(VP1ExaminerViewer *ra, int pixels_x, int pixels_y,
				bool transparent_background = false, double actualRenderedSizeFact = 1.0  );

  //Esoteric usage:
  static QPixmap pixmapFromRGBFile(const QString& filename);
  static QImage  imageFromRGBFile(const QString& filename);

  ////////////////////////////////////////////////////////////
  //    For persistification, etc. of transparency types    //
  ////////////////////////////////////////////////////////////

  static SoGLRenderAction::TransparencyType getDefaultVP1TransparencyType();
  static QList<SoGLRenderAction::TransparencyType> getAllTransparencyTypes();
  static int transparencyTypeToInt( SoGLRenderAction::TransparencyType );//Returns -1 if unknown.
  static SoGLRenderAction::TransparencyType intToTransparencyType( int );//Return DELAYED_BLEND if unknown;
  static QString transparencyType2PrettyString( SoGLRenderAction::TransparencyType );//Returns "" if unknown.

  ////////////////////////////////////////////////////////////
  //    For persistification of various Inventor types      //
  ////////////////////////////////////////////////////////////

  static QByteArray serialize( const SbRotation& );
  static bool deserialize( QByteArray&, SbRotation& );

  static QByteArray serialize( const SbVec3f& );
  static bool deserialize( QByteArray&, SbVec3f& );

  static QByteArray serializeSoCameraParameters( const SoCamera& );
  static bool deserializeSoCameraParameters( QByteArray&, SoCamera& );//Does nothing and returns false in case of errors.

  static QByteArray serialiseSoMaterial(SoMaterial *);
  static bool deserialiseSoMaterial(QByteArray&,SoMaterial *&);//Does nothing and returns false in case of errors.

  ////////////////////////////////////////////////////////////
  //     Changing path trailing part (to fix selections)    //
  ////////////////////////////////////////////////////////////

  static bool changePathTail(SoPath*path,SoNode*commonBranchPoint,SoNode*newtail);

  ///////////////////////////////////////////
  //     Read/write scenegraphs to file    //
  ///////////////////////////////////////////

  static bool writeGraphToFile(SoNode*root, const QString& filename);//returns false in case of errors
  static SoSeparator* readGraphFromFile(const QString& filename);//returns 0 in case of errors

  static bool writeGraphToVRMLFile(SoNode*root, const QString& filename);//returns false in case of errors

  ///////////////////////////////////////////////////////////
  //    Simple materials (color/brightness/transparency)   //
  ///////////////////////////////////////////////////////////

  //THESE ARE OBSOLETE. USE STATIC METHODS IN VP1MaterialButton instead!!

  static void setMatColor( SoMaterial *, const double& r, const double& g, const double& b,
			   const double& brightness = 0.0, const double& transp = 0.0 );//OBSOLETE
  static void setMatColor( SoMaterial *, const QColor&,
			   const double& brightness = 0.0, const double& transp = 0.0 );//OBSOLETE

  ////////////////////////////////////////////////////////////////////////////
  //    Get allowed ranges for point size/line widths (depends on hardware) //
  ////////////////////////////////////////////////////////////////////////////

  static void getLineWidthRanges(double& min, double& max, double& granularity);
  static void getPointSizeRanges(double& min, double& max, double& granularity);
  static void setLimitsLineWidthSlider(QSlider *);
  static void setLimitsPointSizeSlider(QSlider *);
  static void setValueLineWidthSlider(QSlider *, const double& value);
  static void setValuePointSizeSlider(QSlider *, const double& value);
  static double getValueLineWidthSlider(const QSlider *);
  static double getValuePointSizeSlider(const QSlider *);

  //Call whenever making a renderarea (so we don't need to create extra instances just for this info):
  static void ensureInitLineWidthAndPointSize(SoQtRenderArea*);

private:

  VP1QtInventorUtils();
  ~VP1QtInventorUtils();

  class Imp;

};

#endif
