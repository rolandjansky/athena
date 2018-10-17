/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class VP12DGeometrySystem                        //
//                                                                     //
//  Author: Brock Greman <bgreman@gmail.com>                           //
//                                                                     //
//  Initial version: August 2007                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP12DGeometrySystems/VP12DGeometrySystem.h"
#include "VP12DGeometrySystems/StoreGateGeometryReader.h"
#include "VP1Base/VP1GraphicsItemCollection.h"
//TK: include when needed #include "ui_2dgeometrycontroller.h"

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <cmath>

// #include "StoreGate/StoreGate.h"

//TK: This scale factor needs to go. CLHEP units is what both geomodel and VP1 uses.
//TK: Worst of all - if I set it to 1., I seem to get different (better?) results...
#define SCALE_FACTOR 1.

#define DEG_2_RAD M_PI/180

static int items_drawn = 0;
static int ABox_drawn = 0;
static int ATrd_drawn = 0;
static int ATBx_drawn = 0;
static int AGeometryConstant_read = 0;
static int AGeneralPath_drawn = 0;
static int ADisc_drawn = 0;
static int ARectangle_drawn = 0;
static int ABarrelCalorimeter_drawn = 0;
static int AEndcapCalorimeter_drawn = 0;
static int AGapCalorimeter_drawn = 0;
static int ABarrelSiliconDetector_drawn = 0;
static int AEndcapSiliconDetector_drawn = 0;
static int ABarrelTRTDetector_drawn = 0;
static int AEndcapTRTDetector_drawn = 0;
static int ERROR_TYPE_drawn = 0;

//implementation class for the system
class VP12DGeometrySystem::Imp
{
 public:
  VP12DGeometrySystem * theclass;

  //instance of the object which parses the geometry from the detector store
  StoreGateGeometryReader mGeoReader;

  //utility function needed for certain muon geometry elements
  double calculatePhi(std::string name);

  //Tk: uncomment when using:
  //   QRadioButton *xy_RadioBtn;
  //   QRadioButton *rz_RadioBtn;

  //These functions perform the bulk of the drawing work
  void addYXProjectionToCollection(VP1GraphicsItemCollection*);
  void addRZProjectionToCollection(VP1GraphicsItemCollection*);
  void addFRProjectionToCollection(VP1GraphicsItemCollection*);
  void addFZProjectionToCollection(VP1GraphicsItemCollection*);

  //Due to the complexity of the muon detector element drawing routines,
  //these processes have been spun off into a separate function.
  void processMuonDetector(const Detector&, VP1GraphicsItemCollection*, std::string);


};

double VP12DGeometrySystem::Imp::calculatePhi(std::string name)
{
  double phiRet;
  std::string technology = name.substr(0,3);
  if (technology == "TGC")
    {
      if (name.at(6)=='E' && name.at(5) !='4')
	{
	  phiRet = 24.;
	  return phiRet;
	}
      else
	{
	  phiRet = 12.;
	  return phiRet;
	}
    }
  else
    {
      phiRet = 4.;
      return phiRet;
    }
}

//_____________________________________________________________________________________
VP12DGeometrySystem::VP12DGeometrySystem()
  : IVP12DDetViewsSystem("2DGeo",
		       "This is an illustration of an extremely basic 2D system.\n "
		       "It is a placeholder for the future 2D geometry system.",
			 "Thomas.Kittelmann@cern.ch"), d(new Imp)
{
  d->theclass=this;
}


VP12DGeometrySystem::~VP12DGeometrySystem()
{
  delete d; d = 0;
}

//Initializes system members, loads geometry
void VP12DGeometrySystem::systemcreate(StoreGateSvc *detStore)
{
  if (detStore)
    {
      //set detector store pointer of geometry reader
      d->mGeoReader.setDetStore(detStore);

      //read geometry and test for success
      if (!d->mGeoReader.readGeometry())
	{
	  //here we had an error somewhere while reading geometry
	  message("Error parsing DetStore geometry!");
	}
      else
	{
	  message("Geometry successfully read!");
	}
    }
  else
    {
      message("Received null detector store pointer!");
    }

  //print number of detector elements found for sanity check
  std::stringstream outputdata;
  outputdata << "2D Geometry System finds: " << d->mGeoReader.mDetectors.size() << " elements." << std::endl;
  QString qstrout = outputdata.str().c_str();
  message(qstrout);
}

//_____________________________________________________________________________________
void VP12DGeometrySystem::buildPermanentItemCollections(StoreGateSvc* /*detstore*/,VP1GraphicsItemCollection *ic_xy, VP1GraphicsItemCollection *ic_rz)
{
  //add appropriate projection data to the visible collection.
  //todo: Add all four at once, but only make one visible, depending
  //on a UI radio selector

  d->addYXProjectionToCollection(ic_xy);
  message("Just wrote X-Y Projection");

  d->addRZProjectionToCollection(ic_rz);
  message("Just wrote Rho-Z Projection");

//   if (PROJECTION == "FR")
//     {
//       addFRProjectionToCollection(ic);
//       message("Just wrote Phi-Rho Projection");
//     }
//   if (PROJECTION == "FZ")
//     {
//       addFZProjectionToCollection(ic);
//       message("Just wrote Phi-Z Projection");
//     }

}

//Each of the following four functions has the same general format, so I'll comment this one and
//you can use it as a template for the others.  The differences are only in the details of drawing
//the geometry for each shape.  All geometry drawing code adapted from the Atlantis Event Display
void VP12DGeometrySystem::Imp::addYXProjectionToCollection(VP1GraphicsItemCollection *theCollection)
{

  //Iterate through the detector shape elements
  for (unsigned int i=0;i<mGeoReader.mDetectors.size();i++)
    {
      //Create a brush containing the color used to paint the detector shape
      QBrush elementBrush(mGeoReader.mDetectors[i].color);

      //Create a pen to outline the elements in their own color
      QPen elementPen(mGeoReader.mDetectors[i].color,0);


      //ABox and ATrd are used to draw certain Muon detector elements
      if (mGeoReader.mDetectors[i].type==Detector::ABox || mGeoReader.mDetectors[i].type==Detector::ATrd)
	{
	  processMuonDetector(mGeoReader.mDetectors[i], theCollection, "YX");
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGeneralPath)
	{
	  //will add later
	  items_drawn++;
	  AGeneralPath_drawn++;
	}
      //ADisc is used to represent various abstract circular shapes from various detector systems
      else if (mGeoReader.mDetectors[i].type==Detector::ADisc && mGeoReader.mDetectors[i].projection=="YX" )
	{
	  double rInner = mGeoReader.mDetectors[i].rInner;
	  double rOuter = mGeoReader.mDetectors[i].rOuter;
	  int nInner    = mGeoReader.mDetectors[i].nInner;
	  int nOuter    = mGeoReader.mDetectors[i].nOuter;

	  rInner/=SCALE_FACTOR;
	  rOuter/=SCALE_FACTOR;

	  QPolygonF disc;

	  for (int j=0; j<nInner+1; j++)
	    {
	      double cosTerm = cos((2*M_PI*j)/nInner);
	      double sinTerm = sin((2*M_PI*j)/nInner);
	      disc << QPointF(rInner*cosTerm, rInner*sinTerm);
	    }

	  for (int j=0; j<nOuter+1; j++)
	    {
	      double cosTerm = cos(2*M_PI*(nOuter-j)/nOuter);
	      double sinTerm = sin(2*M_PI*(nOuter-j)/nOuter);
	      disc << QPointF(rOuter*cosTerm, rInner*sinTerm);
	    }

	  QGraphicsPolygonItem *theDisc = new QGraphicsPolygonItem(disc);
	  theDisc->setBrush(elementBrush);
	  theDisc->setPen(elementPen);

	  theCollection->addItem(theDisc);
	  items_drawn++;
	  ADisc_drawn++;
	}

      //ARectangles are used to represent various abstract rectangular shapes from various detector systems
      else if (mGeoReader.mDetectors[i].type==Detector::ARectangle && mGeoReader.mDetectors[i].projection=="YX" )
	{
	  double xMin = mGeoReader.mDetectors[i].xMin;
	  double xMax = mGeoReader.mDetectors[i].xMax;
	  double yMin = mGeoReader.mDetectors[i].yMin;
	  double yMax = mGeoReader.mDetectors[i].yMax;

	  xMin/=SCALE_FACTOR;
	  xMax/=SCALE_FACTOR;
	  yMin/=SCALE_FACTOR;
	  yMax/=SCALE_FACTOR;

	  QPolygonF theBox;
	  theBox << QPointF(xMin,yMin);
	  theBox << QPointF(xMin,yMax);
	  theBox << QPointF(xMax,yMax);
	  theBox << QPointF(xMax,yMin);

	  QGraphicsPolygonItem *theItem = new QGraphicsPolygonItem(theBox);
	  theItem->setPen(elementPen);
	  theItem->setBrush(elementBrush);
	  theCollection->addItem(theItem);

	  if (mGeoReader.mDetectors[i].xReflect)
	    {
	      QPolygonF theBoxXR;
	      double xMinr = -xMin;
	      double xMaxr = -xMax;
	      theBoxXR << QPointF(xMinr,yMin);
	      theBoxXR << QPointF(xMinr,yMax);
	      theBoxXR << QPointF(xMaxr,yMax);
	      theBoxXR << QPointF(xMaxr,yMin);

	      theItem = new QGraphicsPolygonItem(theBoxXR);
	      theItem->setPen(elementPen);
	      theItem->setBrush(elementBrush);
	      theCollection->addItem(theItem);
	    }

	  if (mGeoReader.mDetectors[i].yReflect)
	    {
	      QPolygonF theBoxYR;
	      double yMinr = -yMin;
	      double yMaxr = -yMax;
	      theBoxYR << QPointF(xMin,yMinr);
	      theBoxYR << QPointF(xMin,yMaxr);
	      theBoxYR << QPointF(xMax,yMaxr);
	      theBoxYR << QPointF(xMax,yMinr);

	      theItem = new QGraphicsPolygonItem(theBoxYR);
	      theItem->setPen(elementPen);
	      theItem->setBrush(elementBrush);
	      theCollection->addItem(theItem);
	    }

	  if (mGeoReader.mDetectors[i].xReflect && mGeoReader.mDetectors[i].yReflect)
	    {
	      QPolygonF theBoxXYR;
	      double xMinr = -xMin;
	      double xMaxr = -xMax;
	      double yMinr = -yMin;
	      double yMaxr = -yMax;
	      theBoxXYR << QPointF(xMinr,yMinr);
	      theBoxXYR << QPointF(xMinr,yMaxr);
	      theBoxXYR << QPointF(xMaxr,yMaxr);
	      theBoxXYR << QPointF(xMaxr,yMinr);

	      theItem = new QGraphicsPolygonItem(theBoxXYR);
	      theItem->setPen(elementPen);
	      theItem->setBrush(elementBrush);
	      theCollection->addItem(theItem);
	    }

	  items_drawn++;
	  ARectangle_drawn++;
	}

      //ABarrelCalorimeter represents barrel calorimeters detector elements
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelCalorimeter )
	{
	  double rMin = mGeoReader.mDetectors[i].rMin;
	  double rMax = mGeoReader.mDetectors[i].rMax;
	  int numPhi = mGeoReader.mDetectors[i].nphi;
	  double delPhi = 2*M_PI/numPhi;
	  double phi0 = DEG_2_RAD * mGeoReader.mDetectors[i].phi0;

	  rMin/=SCALE_FACTOR;
	  rMax/=SCALE_FACTOR;

	  QPolygonF barCal;

	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMin = cos(phi0 + i*delPhi);
	      double sinTermMin = sin(phi0 + i*delPhi);

	      barCal << QPointF(rMin*cosTermMin, rMin*sinTermMin);
	    }
	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMax = cos(phi0 - i*delPhi);
	      double sinTermMax = sin(phi0 - i*delPhi);

	      barCal << QPointF(rMax*cosTermMax, rMax*sinTermMax);
	    }

	  QGraphicsPolygonItem *theBarCal = new QGraphicsPolygonItem(barCal);
	  theBarCal->setPen(elementPen);
	  theBarCal->setBrush(elementBrush);

	  theCollection->addItem(theBarCal);
	  items_drawn++;
	  ABarrelCalorimeter_drawn++;
	}

      //AEndcapCalorimeters represent endcap calorimeter elements
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCalorimeter )
	{
	  double etaMax = mGeoReader.mDetectors[i].eta0 + mGeoReader.mDetectors[i].neta * mGeoReader.mDetectors[i].deta;
	  double etaMin = mGeoReader.mDetectors[i].eta0;
	  int numPhi = mGeoReader.mDetectors[i].nphi;
	  double delPhi = 2*M_PI/numPhi;
	  double phi0 = DEG_2_RAD * mGeoReader.mDetectors[i].phi0;
	  double rMin, rMax;

	  if (mGeoReader.mDetectors[i].name == "HEC")
	    {
	      rMin = mGeoReader.mDetectors[i].rMin;
	      rMax = mGeoReader.mDetectors[i].rMax;
	    }
	  else
	    {
	      if (mGeoReader.mDetectors[i].name.find("Inner") != std::string::npos || mGeoReader.mDetectors[i].name.find("Presampler") != std::string::npos)
		{
		  rMax = mGeoReader.mDetectors[i].zMin/sinh(etaMin);
		  rMin = 0.0;//TK - added this line since rMin is otherwise unitialized. Is 0 the right value?????? Fixme.
		  //BG: In the constructor for StoreGateGeometryReader::Detector, every member is initialized to 0 to take care of situations like this.
		  //I believe that Atlantis uses the same technique, so when I came across this line in the Atlantis code, rMax was the only variable init'd.
		}
	      else
		{
		  rMax = mGeoReader.mDetectors[i].rMax;
		  rMin = mGeoReader.mDetectors[i].zMin/sinh(etaMax);
		}
	    }

	  rMax/=SCALE_FACTOR;
	  rMin/=SCALE_FACTOR;

	  QPolygonF ecCal;

	  for (int i=0; i < numPhi + 1; i++)
	    {
	      double cosTermMin = cos(phi0 + i*delPhi);
	      double sinTermMin = sin(phi0 + i*delPhi);

	      ecCal << QPointF(rMin*cosTermMin, rMin*sinTermMin);
	    }
	  for (int i=0; i < numPhi + 1; i++)
	    {
	      double cosTermMax = cos(phi0 - i*delPhi);
	      double sinTermMax = sin(phi0 - i*delPhi);

	      ecCal << QPointF(rMax*cosTermMax, rMax*sinTermMax);
	    }

	  QGraphicsPolygonItem *theEcCal = new QGraphicsPolygonItem(ecCal);
	  theEcCal->setPen(elementPen);
	  theEcCal->setBrush(elementBrush);

	  theCollection->addItem(theEcCal);
	  items_drawn++;
	  AEndcapCalorimeter_drawn++;
	}

      //AGapCalorimeters represent gap calorimeter elements
      else if (mGeoReader.mDetectors[i].type==Detector::AGapCalorimeter)
	{
	  //only has an RZ and VP projection
	}

      //ABarrelSiliconDetectors represent barrel silicon (pixel and silicon) detectors
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelSiliconDetector )
	{
	  double r0 = mGeoReader.mDetectors[i].r0;
	  double width = mGeoReader.mDetectors[i].width;
	  double thickness = mGeoReader.mDetectors[i].thickness;
	  double tilt = DEG_2_RAD * mGeoReader.mDetectors[i].tilt;
	  int numPhi = mGeoReader.mDetectors[i].nphi;
	  double delPhi = 2*M_PI/numPhi;
	  double phi0 = DEG_2_RAD * mGeoReader.mDetectors[i].phi0;

	  double rMin = r0 - fabs(thickness*cos(tilt)) - fabs(width*sin(tilt));
	  double rMax = r0 + fabs(thickness*cos(tilt)) + fabs(width*sin(tilt));

	  rMin/=SCALE_FACTOR;
	  rMax/=SCALE_FACTOR;

	  QPolygonF barSil;

	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMin = cos(phi0 + (0.5+i)*delPhi);
	      double sinTermMin = sin(phi0 + (0.5+i)*delPhi);

	      barSil << QPointF(rMin*cosTermMin, rMin*sinTermMin);
	    }
	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMax = cos(phi0 - (0.5+i)*delPhi);
	      double sinTermMax = sin(phi0 - (0.5+i)*delPhi);

	      barSil << QPointF(rMax*cosTermMax, rMax*sinTermMax);
	    }

	  QGraphicsPolygonItem *theBarSil = new QGraphicsPolygonItem(barSil);
	  theBarSil->setPen(elementPen);
	  theBarSil->setBrush(elementBrush);

	  theCollection->addItem(theBarSil);
	  items_drawn++;
	  ABarrelSiliconDetector_drawn++;
	}

      //AEndcapSiliconDetectors represent endcap silicon (pixel and silicon) detector elements
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapSiliconDetector )
	{
	  int numPhi = mGeoReader.mDetectors[i].nphi;
	  double delPhi = 2*M_PI/numPhi;
	  double phi0 = DEG_2_RAD * mGeoReader.mDetectors[i].phi0;
	  double rMin = mGeoReader.mDetectors[i].rMin;
	  double rMax = mGeoReader.mDetectors[i].rMax;

	  rMin/=SCALE_FACTOR;
	  rMax/=SCALE_FACTOR;

	  QPolygonF ecSil;

	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMin = cos(phi0 + (0.5+i)*delPhi);
	      double sinTermMin = sin(phi0 + (0.5+i)*delPhi);

	      ecSil << QPointF(rMin*cosTermMin, rMin*sinTermMin);
	    }
	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMax = cos(phi0 - (0.5+i)*delPhi);
	      double sinTermMax = sin(phi0 - (0.5+i)*delPhi);
	      ecSil << QPointF(rMax*cosTermMax, rMax*sinTermMax);
	    }

	  QGraphicsPolygonItem *theEcSil = new QGraphicsPolygonItem(ecSil);
	  theEcSil->setPen(elementPen);
	  theEcSil->setBrush(elementBrush);

	  theCollection->addItem(theEcSil);
	  items_drawn++;
	  AEndcapSiliconDetector_drawn++;
	}

      //ABarrelTRTDetectors represent barrel TRT detector elements
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelTRTDetector )
	{
	  int numPhi = mGeoReader.mDetectors[i].nphi;
	  double delPhi = 2*M_PI/numPhi;
	  double phiIn = DEG_2_RAD * mGeoReader.mDetectors[i].phiIn;
	  double phiOut= DEG_2_RAD * mGeoReader.mDetectors[i].phiOut;
	  double rMin = mGeoReader.mDetectors[i].rMin;
	  double rMax = mGeoReader.mDetectors[i].rMax;

	  rMin/=SCALE_FACTOR;
	  rMax/=SCALE_FACTOR;

	  QPolygonF barTRT;

	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMin = cos(phiIn + i*delPhi);
	      double sinTermMin = sin(phiIn + i*delPhi);

	      barTRT << QPointF(rMin*cosTermMin, rMin*sinTermMin);
	    }
	  for (int i = 0; i < numPhi + 1; i++)
	    {
	      double cosTermMax = cos(phiOut - i*delPhi);
	      double sinTermMax = sin(phiOut - i*delPhi);
	      barTRT << QPointF(rMax*cosTermMax, rMax*sinTermMax);
	    }

	  QGraphicsPolygonItem *theBarTRT = new QGraphicsPolygonItem(barTRT);
	  theBarTRT->setPen(elementPen);
	  theBarTRT->setBrush(elementBrush);

	  theCollection->addItem(theBarTRT);
	  items_drawn++;
	  ABarrelTRTDetector_drawn++;
	}

      //AEndcapTRTDetectors represent endcap TRT detector elements
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapTRTDetector)
	{
	  // has no YX projection
	}

      //I'm not quite sure what this is.  JiveXML apparently creates these when it loads MBTS geometry,
      //but I've never seen one in any of the Atlantis xml files
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCryostat)
	{
	  // does this even exist?"
	}

      //these aren't drawn, and are tracked merely for internal debuggin purposes
      else if (mGeoReader.mDetectors[i].type==Detector::ERROR_TYPE)
	{
	  ERROR_TYPE_drawn++;
	}
    }
}

//This function adds the Rho-Z projection.  See the XY projection for detailed comments
void VP12DGeometrySystem::Imp::addRZProjectionToCollection(VP1GraphicsItemCollection* theCollection)
{
  //TK  QPen elementPen(Qt::black,0);
  QPen elementPen(Qt::transparent);//TK

  //needs heavy fixing
  for (unsigned int i = 0; i < mGeoReader.mDetectors.size(); i++)
  {
    QBrush elementBrush(mGeoReader.mDetectors[i].color);

    if (mGeoReader.mDetectors[i].type == Detector::ABox || mGeoReader.mDetectors[i].type == Detector::ATrd)
      {
	processMuonDetector(mGeoReader.mDetectors[i], theCollection, "RZ");
      }
    else if (mGeoReader.mDetectors[i].type == Detector::ATBx)
      {
	//only used in FZ projection
	items_drawn++;
	ATBx_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::AGeometryConstant)
      {
	//nothing to draw
	AGeometryConstant_read++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::AGeneralPath)
      {
	//to be added later
	items_drawn++;
	AGeneralPath_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::ARectangle && mGeoReader.mDetectors[i].projection == "RZ" )
      {
	double xMin = mGeoReader.mDetectors[i].xMin;
	double xMax = mGeoReader.mDetectors[i].xMax;
	double yMin = mGeoReader.mDetectors[i].yMin;
	double yMax = mGeoReader.mDetectors[i].yMax;

	xMin/=SCALE_FACTOR;
	xMax/=SCALE_FACTOR;
	yMin/=SCALE_FACTOR;
	yMax/=SCALE_FACTOR;

	QPolygonF theBox;
	theBox << QPointF(xMin,yMin);
	theBox << QPointF(xMin,yMax);
	theBox << QPointF(xMax,yMax);
	theBox << QPointF(xMax,yMin);
	;
	QGraphicsPolygonItem *theItem = new QGraphicsPolygonItem(theBox);
	theItem->setPen(elementPen);
	theItem->setBrush(elementBrush);
	theCollection->addItem(theItem);

	if (mGeoReader.mDetectors[i].xReflect)
	  {
	    QPolygonF theBoxXR;
	    double xMinr = -xMin;
	    double xMaxr = -xMax;
	    theBoxXR << QPointF(xMinr,yMin);
	    theBoxXR << QPointF(xMinr,yMax);
	    theBoxXR << QPointF(xMaxr,yMax);
	    theBoxXR << QPointF(xMaxr,yMin);

	    theItem = new QGraphicsPolygonItem(theBoxXR);
	    theItem->setPen(elementPen);
	    theItem->setBrush(elementBrush);
	    theCollection->addItem(theItem);
	  }

	if (mGeoReader.mDetectors[i].yReflect)
	  {
	    QPolygonF theBoxYR;
	    double yMinr = -yMin;
	    double yMaxr = -yMax;
	    theBoxYR << QPointF(xMin,yMinr);
	    theBoxYR << QPointF(xMin,yMaxr);
	    theBoxYR << QPointF(xMax,yMaxr);
	    theBoxYR << QPointF(xMax,yMinr);

	    theItem = new QGraphicsPolygonItem(theBoxYR);
	    theItem->setPen(elementPen);
	    theItem->setBrush(elementBrush);
	    theCollection->addItem(theItem);
	  }

	if (mGeoReader.mDetectors[i].xReflect && mGeoReader.mDetectors[i].yReflect)
	  {
	    QPolygonF theBoxXYR;
	    double xMinr = -xMin;
	    double xMaxr = -xMax;
	    double yMinr = -yMin;
	    double yMaxr = -yMax;
	    theBoxXYR << QPointF(xMinr,yMinr);
	    theBoxXYR << QPointF(xMinr,yMaxr);
	    theBoxXYR << QPointF(xMaxr,yMaxr);
	    theBoxXYR << QPointF(xMaxr,yMinr);

	    theItem = new QGraphicsPolygonItem(theBoxXYR);
	    theItem->setPen(elementPen);
	    theItem->setBrush(elementBrush);
	    theCollection->addItem(theItem);
	  }

    items_drawn++;
    ARectangle_drawn++;
  }
    else if (mGeoReader.mDetectors[i].type == Detector::ABarrelCalorimeter)
      {
	//need to implement clipper
	double rMin = mGeoReader.mDetectors[i].rMin;
	double rMax = mGeoReader.mDetectors[i].rMax;
	double zMin = mGeoReader.mDetectors[i].zMin;
	double zMax = mGeoReader.mDetectors[i].zMax;
	double eta0 = mGeoReader.mDetectors[i].eta0;
	double dEta = mGeoReader.mDetectors[i].deta;
	int    nEta = mGeoReader.mDetectors[i].neta;

	double etaMax = eta0 + nEta*dEta;
	double etaMin;
	if (eta0 <= 0.0)
	  {etaMin = -etaMax;}
	else
	  {etaMin = eta0;}

	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;
	QPolygonF barCal;

	double z0, z1, z2, z3;

	if (mGeoReader.mDetectors[i].name.find("TILE") != std::string::npos)
	  {
	    z0 = zMax; z1 = zMin; z2 = zMin; z3 = zMax;
	    theclass->message("Here thar be tiles!");
	  }
	else
	  {
	    z0 = rMax * sinh(etaMax);
	    z1 = rMax * sinh(etaMin);
	    z2 = rMin * sinh(etaMin);
	    z3 = rMin * sinh(etaMax);
	  }

	barCal << QPointF(z0, rMax);
	barCal << QPointF(z1, rMax);
	barCal << QPointF(z2, rMin);
	barCal << QPointF(z3, rMin);
	barCal << QPointF(z0, -rMin);
	barCal << QPointF(z1, -rMin);
	barCal << QPointF(z2, -rMax);
	barCal << QPointF(z3, -rMax);

	QGraphicsPolygonItem *theBarCal = new QGraphicsPolygonItem(barCal);
	theBarCal->setPen(elementPen);
	theBarCal->setBrush(elementBrush);
	theCollection->addItem(theBarCal);

	if (eta0 > 0.0)
	  {
	    QPolygonF barCal2;
	    barCal2 << QPointF(-z1, rMax);
	    barCal2 << QPointF(-z0, rMax);
	    barCal2 << QPointF(-z3, rMin);
	    barCal2 << QPointF(-z2, rMin);
	    barCal2 << QPointF(-z2, -rMin);
	    barCal2 << QPointF(-z3, -rMin);
	    barCal2 << QPointF(-z0, -rMax);
	    barCal2 << QPointF(-z1, -rMax);
	    QGraphicsPolygonItem *theBarCal2 = new QGraphicsPolygonItem(barCal2);
	    theBarCal2->setPen(elementPen);
	    theBarCal2->setBrush(elementBrush);
	    theCollection->addItem(theBarCal2);
	  }

	items_drawn++;
	ABarrelCalorimeter_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::AEndcapCalorimeter)
      {
	//need to implement clipper
	double rMin = mGeoReader.mDetectors[i].rMin;
	double rMax = mGeoReader.mDetectors[i].rMax;
	double zMin = mGeoReader.mDetectors[i].zMin;
	double zMax = mGeoReader.mDetectors[i].zMax;
	double eta0 = mGeoReader.mDetectors[i].eta0;
	double dEta = mGeoReader.mDetectors[i].deta;
	int    nEta = mGeoReader.mDetectors[i].neta;

	double etaMax = eta0 + nEta*dEta;
	double etaMin = eta0;

	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;
	QPolygonF ecCal;

	double r0, r1, r2, r3;

	if (mGeoReader.mDetectors[i].name.find("HEC") != std::string::npos)
	  {
	    r0 = rMax; r1 = zMax; r2 = rMin; r3 = rMin;
	    theclass->message("Here thar be HECs");
	  }
	else
	  {
	    r0 = zMax / sinh(etaMin);
	    r1 = zMin / sinh(etaMin);
	    r2 = zMin / sinh(etaMax);
	    r3 = zMax / sinh(etaMax);
	  }

	ecCal << QPointF(zMax, r0);
	ecCal << QPointF(zMin, r1);
	ecCal << QPointF(zMin, r2);
	ecCal << QPointF(zMax, r3);
	ecCal << QPointF(zMax, -r3);
	ecCal << QPointF(zMin, -r2);
	ecCal << QPointF(zMin, -r1);
	ecCal << QPointF(zMax, -r0);

	QGraphicsPolygonItem *theEcCal = new QGraphicsPolygonItem(ecCal);
	theEcCal->setPen(elementPen);
	theEcCal->setBrush(elementBrush);
	theCollection->addItem(theEcCal);

	if (eta0 > 0.0)
	  {
	    QPolygonF ecCal2;
	    ecCal2 << QPointF(-zMin, r1);
	    ecCal2 << QPointF(-zMax, r0);
	    ecCal2 << QPointF(-zMax, r3);
	    ecCal2 << QPointF(-zMin, r2);
	    ecCal2 << QPointF(-zMin, -r2);
	    ecCal2 << QPointF(-zMax, -r3);
	    ecCal2 << QPointF(-zMax, -r0);
	    ecCal2 << QPointF(-zMin, -r1);
	    QGraphicsPolygonItem *theEcCal2 = new QGraphicsPolygonItem(ecCal2);
	    theEcCal2->setPen(elementPen);
	    theEcCal2->setBrush(elementBrush);
	    theCollection->addItem(theEcCal2);
	  }

	items_drawn++;
	AEndcapCalorimeter_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::AGapCalorimeter )
      {
	double rMin = mGeoReader.mDetectors[i].rMin;
	double rMax = mGeoReader.mDetectors[i].rMax;
	double zMin = mGeoReader.mDetectors[i].zMin;
	double zMax = mGeoReader.mDetectors[i].zMax;

	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;

	//this double polygon system is hackish, and I don't like it.  Look into how atlantis does this separated geometry.
	//also used in the endcap detectors ohgod rargh I used it everywhere QPolygons suck it hard
	QPolygonF gapCal;
	QPolygonF gapCal2;

	if (zMin < 0)
	  {
	    gapCal << QPointF(zMax, rMax);
	    gapCal << QPointF(zMin, rMax);
	    gapCal << QPointF(zMin, rMin);
	    gapCal << QPointF(zMax, rMin);
	    gapCal << QPointF(zMax, -rMin);
	    gapCal << QPointF(zMin, -rMin);
	    gapCal << QPointF(zMin, -rMax);
	    gapCal << QPointF(zMax, -rMax);
	  }

	else
	  {
	    gapCal << QPointF(zMax, rMax);
	    gapCal << QPointF(zMin, rMax);
	    gapCal << QPointF(zMin, rMin);
	    gapCal << QPointF(zMax, rMin);
	    gapCal << QPointF(zMax, -rMin);
	    gapCal << QPointF(zMin, -rMin);
	    gapCal << QPointF(zMin, -rMax);
	    gapCal << QPointF(zMax, -rMax);

	    gapCal2 << QPointF(-zMin, rMax);
	    gapCal2 << QPointF(-zMax, rMax);
	    gapCal2 << QPointF(-zMax, rMin);
	    gapCal2 << QPointF(-zMin, rMin);
	    gapCal2 << QPointF(-zMin, -rMin);
	    gapCal2 << QPointF(-zMax, -rMin);
	    gapCal2 << QPointF(-zMax, -rMax);
	    gapCal2 << QPointF(-zMin, -rMax);
	  }

	QGraphicsPolygonItem *theGapCal = new QGraphicsPolygonItem(gapCal);
	QGraphicsPolygonItem *theGapCal2= new QGraphicsPolygonItem(gapCal2);
	theGapCal->setPen(elementPen);
	theGapCal->setBrush(elementBrush);
	theGapCal2->setPen(elementPen);
	theGapCal2->setBrush(elementBrush);
	theCollection->addItem(theGapCal);
	theCollection->addItem(theGapCal2);

	items_drawn++;
	AGapCalorimeter_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::ABarrelSiliconDetector)
      {
	double r0   = mGeoReader.mDetectors[i].r0;
	double width = mGeoReader.mDetectors[i].width;
	double thickness = mGeoReader.mDetectors[i].thickness;
	double tilt = DEG_2_RAD * mGeoReader.mDetectors[i].tilt;
	double zMin = mGeoReader.mDetectors[i].zMin;
	double zMax = mGeoReader.mDetectors[i].zMax;

	double rMin = r0 - fabs(thickness*cos(tilt)) - fabs(width*sin(tilt));
	double rMax = r0 + fabs(thickness*cos(tilt)) + fabs(width*sin(tilt));


	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;

	QPolygonF barSil;

	barSil << QPointF(zMax, rMax);
	barSil << QPointF(zMin, rMax);
	barSil << QPointF(zMin, rMin);
	barSil << QPointF(zMax, rMin);
	barSil << QPointF(zMax, -rMin);
	barSil << QPointF(zMin, -rMin);
	barSil << QPointF(zMin, -rMax);
	barSil << QPointF(zMax, -rMax);

	QBrush aBarSilBrush(Qt::blue);
	QGraphicsPolygonItem *theBarSil = new QGraphicsPolygonItem(barSil);
	theBarSil->setPen(elementPen);
	theBarSil->setBrush(aBarSilBrush);
	theCollection->addItem(theBarSil);

	items_drawn++;
	ABarrelSiliconDetector_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::AEndcapSiliconDetector)
      {
	double rMax = mGeoReader.mDetectors[i].rMax;
	double rMin = mGeoReader.mDetectors[i].rMin;
	double zMax = mGeoReader.mDetectors[i].zMax;
	double zMin = mGeoReader.mDetectors[i].zMin;

	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;

	QPolygonF ecSil;
	QPolygonF ecSil2;

	ecSil << QPointF(zMax, rMax);
	ecSil << QPointF(zMin, rMax);
	ecSil << QPointF(zMin, rMin);
	ecSil << QPointF(zMax, rMin);

	ecSil << QPointF(zMax, -rMin);
	ecSil << QPointF(zMin, -rMin);
	ecSil << QPointF(zMin, -rMax);
	ecSil << QPointF(zMax, -rMax);

	ecSil2 << QPointF(-zMax, rMax);
	ecSil2 << QPointF(-zMin, rMax);
	ecSil2 << QPointF(-zMin, rMin);
	ecSil2 << QPointF(-zMax, rMin);

	ecSil2 << QPointF(-zMax, -rMin);
	ecSil2 << QPointF(-zMin, -rMin);
	ecSil2 << QPointF(-zMin, -rMax);
	ecSil2 << QPointF(-zMax, -rMax);
;
	QGraphicsPolygonItem *theEcSil = new QGraphicsPolygonItem(ecSil);
	QGraphicsPolygonItem *theEcSil2 = new QGraphicsPolygonItem(ecSil2);
	theEcSil->setPen(elementPen);
	theEcSil->setBrush(elementBrush);
	theEcSil2->setPen(elementPen);
	theEcSil2->setBrush(elementBrush);
	theCollection->addItem(theEcSil);
	theCollection->addItem(theEcSil2);

	items_drawn++;
	AEndcapSiliconDetector_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::ABarrelTRTDetector)
      {
	double rMin = mGeoReader.mDetectors[i].rMin;
	double rMax = mGeoReader.mDetectors[i].rMax;
	double zMin = mGeoReader.mDetectors[i].zMin;
	double zMax = mGeoReader.mDetectors[i].zMax;

	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;

	QPolygonF barTRT;

	barTRT << QPointF(zMax, rMax);
	barTRT << QPointF(zMin, rMax);
	barTRT << QPointF(zMin, rMin);
	barTRT << QPointF(zMax, rMin);

	barTRT << QPointF(zMax, -rMin);
	barTRT << QPointF(zMin, -rMin);
	barTRT << QPointF(zMin, -rMax);
	barTRT << QPointF(zMax, -rMax);

	//TKQPen aBarTRTPen(Qt::black, 0);
	QPen aBarTRTPen(Qt::transparent);//TK
	QBrush aBarTRTBrush(Qt::yellow);
	QGraphicsPolygonItem *theBarTRT = new QGraphicsPolygonItem(barTRT);
	theBarTRT->setPen(aBarTRTPen);
	theBarTRT->setBrush(aBarTRTBrush);
	theCollection->addItem(theBarTRT);

	items_drawn++;
	ABarrelTRTDetector_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type == Detector::AEndcapTRTDetector)
      {
	double rMin = mGeoReader.mDetectors[i].rMin;
	double rMax = mGeoReader.mDetectors[i].rMax;
	double zMin = mGeoReader.mDetectors[i].zMin;
	double zMax = mGeoReader.mDetectors[i].zMax;

	rMin/=SCALE_FACTOR;
	rMax/=SCALE_FACTOR;
	zMin/=SCALE_FACTOR;
	zMax/=SCALE_FACTOR;

	QPolygonF ecTRT;
	QPolygonF ecTRT2;

	ecTRT << QPointF(zMax, rMax);
	ecTRT << QPointF(zMin, rMax);
	ecTRT << QPointF(zMin, rMin);
	ecTRT << QPointF(zMax, rMin);

	ecTRT << QPointF(zMax, -rMin);
	ecTRT << QPointF(zMin, -rMin);
	ecTRT << QPointF(zMin, -rMax);
	ecTRT << QPointF(zMax, -rMax);

	ecTRT2 << QPointF(-zMax, rMax);
	ecTRT2 << QPointF(-zMin, rMax);
	ecTRT2 << QPointF(-zMin, rMin);
	ecTRT2 << QPointF(-zMax, rMin);

	ecTRT2 << QPointF(-zMax, -rMin);
	ecTRT2 << QPointF(-zMin, -rMin);
	ecTRT2 << QPointF(-zMin, -rMax);
	ecTRT2 << QPointF(-zMax, -rMax);

	QGraphicsPolygonItem *theEcTRT = new QGraphicsPolygonItem(ecTRT);
	QGraphicsPolygonItem *theEcTRT2 = new QGraphicsPolygonItem(ecTRT2);
	theEcTRT->setPen(elementPen);
	theEcTRT->setBrush(elementBrush);
	theEcTRT2->setPen(elementPen);
	theEcTRT2->setBrush(elementBrush);
	theCollection->addItem(theEcTRT);
	theCollection->addItem(theEcTRT2);

	items_drawn++;
	AEndcapTRTDetector_drawn++;
      }
    else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCryostat)
      {
	// does this even exist?"
      }
    else if (mGeoReader.mDetectors[i].type == Detector::ERROR_TYPE)
      {
	ERROR_TYPE_drawn++;
      }
  }
}

//This function adds the phi-rho projection.  See the XY projection for detailed comments.
void VP12DGeometrySystem::Imp::addFRProjectionToCollection(VP1GraphicsItemCollection* theCollection)
{
  //TK  QPen elementPen(Qt::black,0);
  QPen elementPen(Qt::transparent);//TK

  for (unsigned int i=0; i<mGeoReader.mDetectors.size(); i++)
    {
      QBrush elementBrush(mGeoReader.mDetectors[i].color);
      if (mGeoReader.mDetectors[i].type==Detector::ABox)
	{

	  for (unsigned int p=0;p<mGeoReader.mDetectors[i].phi.size();p++)
	    {
	      int phiIndex=mGeoReader.mDetectors[i].phi[p];
	      double phi0 =mGeoReader.mDetectors[i].dphi;
	      double phiDiv = calculatePhi(mGeoReader.mDetectors[i].name);
	      double phi = phi0 * DEG_2_RAD + (phiIndex - 1) * M_PI / phiDiv;
	      double rMin=mGeoReader.mDetectors[i].rInner+mGeoReader.mDetectors[i].RPCi;
	      double rMax=mGeoReader.mDetectors[i].rInner-mGeoReader.mDetectors[i].RPCo;
	      double sinPhi=sin(phi), cosPhi=cos(phi);

	      rMin/=SCALE_FACTOR;
	      rMax/=SCALE_FACTOR;
	      double wd=M_PI*(mGeoReader.mDetectors[i].w)/64;

	      double x1 = rMin*cosPhi - wd*sinPhi; double y1 = rMin*sinPhi + wd*cosPhi;
	      double x2 = rMin*cosPhi + wd*sinPhi; double y2 = rMin*sinPhi - wd*cosPhi;
	      double x3 = rMax*cosPhi + wd*sinPhi; double y3 = rMax*sinPhi - wd*cosPhi;
	      double x4 = rMax*cosPhi - wd*sinPhi; double y4 = rMax*sinPhi + wd*cosPhi;

	      double r1 = sqrt(x1*x1 + y1*y1); double f1 = atan2(y1, x1);
	      double r2 = sqrt(x2*x2 + y2*y2); double f2 = atan2(y2, x2);
	      double r3 = sqrt(x3*x3 + y3*y3); double f3 = atan2(y3, x3);
	      double r4 = sqrt(x4*x4 + y4*y4); double f4 = atan2(y4, x4);

	      if (f1 < 0) {f1 += 360;}
	      if (f2 < 0) {f2 += 360;}
	      if (f3 < 0) {f3 += 360;}
	      if (f4 < 0) {f4 += 360;}

	      QPolygonF poly;
	      poly << QPointF(r1,f1);
	      if (f2 - f1 > 180.) {f2 -= 360;}
	      if (f2 - f1 < -180.) {f2 += 360;}
	      poly << QPointF(r2,f2);
	      if (f3 - f2 > 180.) {f2 -= 360;}
	      if (f3 - f2 < -180.) {f2 += 360;}
	      poly << QPointF(r3,f3);
	      if (f4 - f3 > 180.) {f2 -= 360;}
	      if (f4 - f3 < -180.) {f2 += 360;}
	      poly << QPointF(r4,f4);

	      QGraphicsPolygonItem * item = new QGraphicsPolygonItem(poly);
	      item->setBrush(elementBrush);
	      item->setPen(elementPen);
	      theCollection->addItem(item);
	      items_drawn++;
	      ABox_drawn++;
	    }
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ATrd)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ATBx)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGeometryConstant)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGeneralPath)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ADisc)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ARectangle)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelCalorimeter)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCalorimeter)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGapCalorimeter)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelSiliconDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapSiliconDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelTRTDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapTRTDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCryostat)
	{
	  // does this even exist?"
	}
      else if (mGeoReader.mDetectors[i].type == Detector::ERROR_TYPE)
	{
	  ERROR_TYPE_drawn++;
	}
    }
}

//This function adds the phi-z projection.  See the XY projection function for detailed comments.
void VP12DGeometrySystem::Imp::addFZProjectionToCollection(VP1GraphicsItemCollection* /*theCollection*/)
{
  //TKQPen elementPen(Qt::black,0);
  QPen elementPen(Qt::transparent);//TK

  for (unsigned int i = 0; i < mGeoReader.mDetectors.size(); i++)
    {
      QBrush elementBrush(mGeoReader.mDetectors[i].color);
      if (mGeoReader.mDetectors[i].type==Detector::ABox)
	{

	}
      else if (mGeoReader.mDetectors[i].type==Detector::ATrd)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ATBx)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGeometryConstant)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGeneralPath)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ADisc)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ARectangle)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelCalorimeter)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCalorimeter)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AGapCalorimeter)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelSiliconDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapSiliconDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::ABarrelTRTDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapTRTDetector)
	{
	}
      else if (mGeoReader.mDetectors[i].type==Detector::AEndcapCryostat)
	{
	  // does this even exist?"
	}
      else if (mGeoReader.mDetectors[i].type == Detector::ERROR_TYPE)
	{
	  ERROR_TYPE_drawn++;
	}
    }
}

void VP12DGeometrySystem::Imp::processMuonDetector(const Detector& theDetector, VP1GraphicsItemCollection *theCollection, std::string projection)
{
  //Create a brush containing the color used to paint the detector shape
  QBrush elementBrush(theDetector.color);

  //Create a pen to outline the elements in their own color
  QPen elementPen(theDetector.color,0);

  switch (theDetector.type)
    {
    case Detector::ABox:
      if (projection == "YX")
	{
	  if (theDetector.eta>=-100)
	    {
	      for (unsigned int p=0;p<theDetector.phi.size();p++)
		for (int rpiter = 0; rpiter < 3; rpiter++)
		  {
		    {
		      //in general, acquire the parameters needed to draw the shape
		      int phiIndex=theDetector.phi[p];
		      double phi0 =theDetector.dphi;
		      double phiDiv = calculatePhi(theDetector.name);
		      double phi = phi0 * DEG_2_RAD + (phiIndex - 1) * M_PI / phiDiv;
		      double rMin(0), rMax(0);

		      //Add the RPC layers on either side of the main detector if necessary

		      //The main detector element
		      if (rpiter == 0)
			{
			  rMin=theDetector.rInner+theDetector.RPCi;
			  rMax=theDetector.rOuter-theDetector.RPCo;
			}

		      //Inner RPC layer
		      else if (rpiter == 1)
			{
			  if (theDetector.RPCi > 0)
			    {
			      rMin = theDetector.rInner;
			      rMax = theDetector.rInner + theDetector.RPCi;
			    }
			  else
			    break;
			}

		      //Outer RPC layer
		      else if (rpiter == 2)
			{
			  if (theDetector.RPCo > 0)
			    {
			      rMin = theDetector.rOuter - theDetector.RPCo;
			      rMax = theDetector.rOuter;
			    }
			  else
			    break;
			}
		      double sinPhi=sin(phi), cosPhi=cos(phi);
		      double s = theDetector.sh;
		      double d = theDetector.w/2.;

		      //all parameters are scaled by a global #define, which can be tuned as necessary (currently set to 1.)
		      rMin/=SCALE_FACTOR;
		      rMax/=SCALE_FACTOR;

		      double xMin = rMin*cosPhi-s*sinPhi; double xMax = rMax*cosPhi - s*sinPhi;
		      double yMin = rMin*sinPhi+s*cosPhi; double yMax = rMax*sinPhi + s*cosPhi;
		      double dx = d*sinPhi; double dy = d*cosPhi;

		      //Create and add points to QPolygonF representing the geometry
		      QPolygonF poly;
		      poly << QPointF(xMin+dx, yMin-dy);
		      poly << QPointF(xMin-dx, yMin+dy);
		      poly << QPointF(xMax-dx, yMax+dy);
		      poly << QPointF(xMax+dx, yMax-dy);

		      //Create a new QGraphicsPolygonItem from the poly,
		      //initialize its pen and brush, and add it to the collection
		      QGraphicsPolygonItem * item = new QGraphicsPolygonItem(poly);
		      item->setBrush(elementBrush);
		      item->setPen(elementPen);
		      theCollection->addItem(item);


		      //Internal debugging/information statics
		      items_drawn++;
		      ABox_drawn++;
		    }
		  }
	    }
	}
      else if (projection == "RZ")
	{
	  for (int zSide = -1; zSide <=1; zSide += 2)
	    {
	      for (int rpiter = 0; rpiter < 3; rpiter++)
		{
		  double rMin(0), rMax(0);
		  if (rpiter == 0)
		    {
		      rMin = theDetector.rInner - theDetector.RPCi;
		      rMax = theDetector.rOuter + theDetector.RPCo;
		    }
		  else if (rpiter == 1)
		    {
		      if (theDetector.RPCi > 0)
			{
			  rMin = theDetector.rInner;
			  rMax = theDetector.rInner + theDetector.RPCi;
			}
		      else
			break;
		    }
		  else if (rpiter == 2)
		    {
		      if (theDetector.RPCo > 0)
			{
			  rMin = theDetector.rOuter - theDetector.RPCo;
			  rMax = theDetector.rOuter;
			}
		      else
			break;
		    }

		  double zMin = theDetector.zInner*zSide;
		  double zMax = theDetector.zOuter*zSide;

		  rMin/=SCALE_FACTOR;
		  rMax/=SCALE_FACTOR;
		  zMin/=SCALE_FACTOR;
		  zMax/=SCALE_FACTOR;

		  QPolygonF aBox;
		  aBox << QPointF(zMin,rMin);
		  aBox << QPointF(zMin,rMax);
		  aBox << QPointF(zMax,rMax);
		  aBox << QPointF(zMax,rMin);

		  QPolygonF aBoxFlip;
		  aBoxFlip << QPointF(zMin,-rMax);
		  aBoxFlip << QPointF(zMin, -rMin);
		  aBoxFlip << QPointF(zMax,-rMin);
		  aBoxFlip << QPointF(zMax, -rMax);

		  QGraphicsPolygonItem *theBox = new QGraphicsPolygonItem(aBox);
		  QGraphicsPolygonItem *theBoxFlip = new QGraphicsPolygonItem(aBoxFlip);
		  theBox->setPen(elementPen);
		  theBox->setBrush(elementBrush);
		  theBoxFlip->setPen(elementPen);
		  theBoxFlip->setBrush(elementBrush);
		  theCollection->addItem(theBox);
		  theCollection->addItem(theBoxFlip);

		  items_drawn++;
		  ABox_drawn++;
		}
	    }
	}
      break;

    case Detector::ATrd:
      if (projection == "YX")
	{
	  std::string technology = theDetector.name.substr(0,3);
	  double phiDiv = calculatePhi(theDetector.name);
	  for (unsigned int p=0; p < theDetector.phi.size(); p++)
	    {
	      int phiIndex = theDetector.phi[p];
	      double phi0 = theDetector.dphi;
	      double phi = phi0 * DEG_2_RAD + (phiIndex - 1) + M_PI/phiDiv;
	      double rMin = theDetector.rInner;
	      double rMax = theDetector.rOuter;
	      double dS = theDetector.wInner/2.;
	      double dL = theDetector.wOuter/2.;
	      double sinPhi= sin(phi), cosPhi=cos(phi);

	      rMin/=SCALE_FACTOR;
	      rMax/=SCALE_FACTOR;

	      double xMin = rMin*cosPhi, yMin = rMin*sinPhi;
	      double xMax = rMax*cosPhi, yMax = rMax*sinPhi;
	      double dxMin = dS*sinPhi, dyMin = dS*cosPhi;
	      double dxMax = dL*sinPhi, dyMax = dL*cosPhi;

	      QPolygonF poly;
	      poly << QPointF(xMin+dxMin, yMin-dyMin);
	      poly << QPointF(xMin-dxMin, yMin+dyMin);
	      poly << QPointF(xMax-dxMax, yMax+dyMax);
	      poly << QPointF(xMax+dxMax, yMax-dyMax);


	      //Create a new QGraphicsPolygonItem from the poly,
	      //initialize its pen and brush, and add it to the collection
	      QGraphicsPolygonItem * item = new QGraphicsPolygonItem(poly);
	      item->setBrush(elementBrush);
	      item->setPen(elementPen);
	      theCollection->addItem(item);


	      //Internal debugging/information statics
	      items_drawn++;
	      ATrd_drawn++;
	    }
	}

      else if (projection == "RZ")
      	{
	  for (int zSide = -1; zSide <= 1; zSide+=2)
	    {
	      double rMin = theDetector.rInner;
	      double rMax = theDetector.rOuter;
	      double zMin = theDetector.zInner*zSide;
	      double zMax = theDetector.zOuter*zSide;
	      double alpha= theDetector.alpha*DEG_2_RAD;

	      rMin/=SCALE_FACTOR;
	      rMax/=SCALE_FACTOR;
	      zMin/=SCALE_FACTOR;
	      zMax/=SCALE_FACTOR;

	      QPolygonF aTrd, aTrdFlip;

	      if (alpha == 0.)
		{
		  aTrd << QPointF(zMin, rMin);
		  aTrd << QPointF(zMin, rMax);
		  aTrd << QPointF(zMax, rMax);
		  aTrd << QPointF(zMax, rMin);

		  aTrdFlip << QPointF(zMin, -rMin);
		  aTrdFlip << QPointF(zMin, -rMax);
		  aTrdFlip << QPointF(zMax, -rMax);
		  aTrdFlip << QPointF(zMax, -rMin);
		}

	      else
		{
		  double zMid = (zMin + zMax) / 2;
		  double rMid = (rMin + rMax) / 2;
		  double dRho = (rMax - rMin) / 2;
		  double dZ   = (zMax - zMin) / 2;
		  double sinAlpha = sin(alpha);
		  double cosAlpha = cos(alpha);

		  if (zMid * rMid > 0)
		    { sinAlpha *= -1.;}

		  double x1 = zMid - sinAlpha*dRho - cosAlpha*dZ;
		  double x2 = zMid - sinAlpha*dRho + cosAlpha*dZ;
		  double x3 = zMid + sinAlpha*dRho + cosAlpha*dZ;
		  double x4 = zMid + sinAlpha*dRho - cosAlpha*dZ;

		  std::stringstream debugger;
		  debugger << x1 << " " << x2 << " " << x3 << " " << x4 << std::endl;

		  double y1 = rMid + cosAlpha*dRho - sinAlpha*dZ;
		  double y2 = rMid + cosAlpha*dRho + sinAlpha*dZ;
		  double y3 = rMid - cosAlpha*dRho + sinAlpha*dZ;
		  double y4 = rMid - cosAlpha*dRho - sinAlpha*dZ;

		  debugger << y1 << " " << y2 << " " << y3 << " " << y4 << std::endl;

		  QString debugout = debugger.str().c_str();
		  theclass->message(debugout);

		  aTrd << QPointF(x1, y1);
		  aTrd << QPointF(x2, y2);
		  aTrd << QPointF(x3, y3);
		  aTrd << QPointF(x4, y4);

		  aTrdFlip << QPointF(x1, -y1);
		  aTrdFlip << QPointF(x2, -y2);
		  aTrdFlip << QPointF(x3, -y3);
		  aTrdFlip << QPointF(x4, -y4);
		}

	      QGraphicsPolygonItem *theTrd = new QGraphicsPolygonItem(aTrd);
	      QGraphicsPolygonItem *theTrdFlip = new QGraphicsPolygonItem(aTrdFlip);
	      theTrd->setPen(elementPen);
	      theTrd->setBrush(elementBrush);
	      theTrdFlip->setPen(elementPen);
	      theTrdFlip->setBrush(elementBrush);
	      theCollection->addItem(theTrd);
	      theCollection->addItem(theTrdFlip);
	      items_drawn++;
	      ATrd_drawn++;
	    }
	}

      break;

    default:
      break;
    }

}

//More specific controller implementation to come
// QWidget *VP12DGeometrySystem::buildController()
// {
//   QWidget *theWidget = new QWidget;
//   Ui::VP12DGeometryControllerForm ui;
//   ui.setupUi(theWidget);

//   d->xy_RadioBtn = ui.radio_xy;
//   d->rz_RadioBtn = ui.radio_rz;

//   return theWidget;
// }
