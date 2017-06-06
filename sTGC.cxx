
#include "MuonGeoModel/sTGC.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/sTGCComponent.h"
#include "MuonGeoModel/Cutout.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
// for cutouts:
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"

using namespace std;

namespace MuonGM {

//Get sTGC components for the trapezoidal shape of the quadruplet
sTGC::sTGC(Component* ss): DetectorElement(ss->name)
{
  sTGCComponent* s = (sTGCComponent*)ss;
  m_component = s;
  width = s->dx1;
  longWidth = s->dx2;
  yCutout= s->yCutout;
  length = s->dy;
  name=s->name;
  index = s->index;
}


GeoFullPhysVol* sTGC::build(int minimalgeo)
{
//  std::cout<<"this is sTGC::build(int minimalgeo) "<<std::endl;
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(minimalgeo, cutoutson, vcutdef);
}


//Start building the physical volume of the quadruplet

GeoFullPhysVol* sTGC::build(int minimalgeo, int , std::vector<Cutout*> )
{
//  std::cout<<"this is sTGC::build "<<std::endl;
  AGDDDetectorStore* ds = AGDDDetectorStore::GetDetectorStore();	
//  std::cout<<"fetching technology "<<std::endl;
  sTGC_Technology* t = (sTGC_Technology*) ds->GetTechnology(name);
  thickness = t->Thickness();
  double gasTck=t->gasThickness;
  double pcbTck=t->pcbThickness;
  double f4=t->f4Thickness;
  double f5=t->f5Thickness;
  double f6=t->f6Thickness;
  //Evaluate honeycomb thickness
  double chamberTck = gasTck+pcbTck;
  double honeycombTck = (thickness - 4*chamberTck)/5;
 

  minimalgeo=t->geoLevel;

// std::cout<<"===================> FRAME in sTGC.cxx ====> f4= "<<f4<<" f5= "<<f5<<" f6= "<<f6<<std::endl;

  //Build sTGC mother volume out of honeycomb material
  GeoSimplePolygonBrep *solid;
	solid=new GeoSimplePolygonBrep(thickness/2.);
	solid->addVertex(longWidth/2.,length/2.);
	solid->addVertex(-longWidth/2.,length/2.);
	if (yCutout) solid->addVertex(-longWidth/2.,length/2.-yCutout);
	solid->addVertex(-width/2.,-length/2.);
	solid->addVertex(width/2.,-length/2.);
	if (yCutout) solid->addVertex(longWidth/2.,length/2.-yCutout);

  //Transform the mother volume to the correct position
  CLHEP::Hep3Vector v(0,0,0);
  CLHEP::HepRotation rot;
  rot.rotateX(M_PI/2.);
  rot.rotateZ(M_PI/2.);
  HepGeom::Transform3D transf(rot,v);
  const GeoShape *strd=new GeoShapeShift(solid,transf);
									
  logVolName=name;
  if (!(m_component->subType).empty()) logVolName+=("-"+m_component->subType);
  const GeoMaterial* mtrd = matManager->getMaterial("muo::Honeycomb");
  GeoLogVol* ltrd = new GeoLogVol(logVolName, strd, mtrd);
  GeoFullPhysVol* ptrd = new GeoFullPhysVol(ltrd); 

  if (!minimalgeo) return ptrd;

  double newpos = -thickness/2.;
  ptrd->add(new GeoSerialIdentifier(0));

  int igl = 0;
  int iSenLyr = 0;

  // Loop over sTGC layers
  for (int i = 0; i < t->nlayers; i++) {
    double widthActive;
    double longWidthActive;
    double lengthActive;

    igl++;
    ptrd->add(new GeoIdentifierTag(igl));

    widthActive = width;
    longWidthActive = longWidth;
    lengthActive = length;

    double newXPos=newpos+ chamberTck/2 + honeycombTck ;
    if (i==0) newXPos=newpos+ chamberTck/2 + honeycombTck ;
    else newXPos=newpos+ chamberTck + honeycombTck ;

 
    //Build gas volume
    GeoSimplePolygonBrep *sGasVolume
        =new GeoSimplePolygonBrep(gasTck/2.);
        sGasVolume->addVertex(longWidthActive/2.,lengthActive/2.);
		sGasVolume->addVertex(-longWidthActive/2.,lengthActive/2.); 
		if (yCutout) sGasVolume->addVertex(-longWidthActive/2.,lengthActive/2.-yCutout);
		sGasVolume->addVertex(-widthActive/2.,-lengthActive/2.);
		sGasVolume->addVertex(widthActive/2.,-lengthActive/2.);
		if (yCutout) sGasVolume->addVertex(longWidthActive/2.,lengthActive/2.-yCutout);
             
    //Transform gas volume
    CLHEP::Hep3Vector v(0,0,0);
    CLHEP::HepRotation rot;
    rot.rotateX(M_PI/2.);
    rot.rotateZ(M_PI/2.);
    HepGeom::Transform3D transf(rot,v);
    const GeoShape *sGasVolume1=new GeoShapeShift(sGasVolume,transf);

    GeoLogVol* ltrdgas = new GeoLogVol("sTGC_Sensitive", sGasVolume1,
                                         matManager->getMaterial("muo::TGCGas"));
    GeoPhysVol* ptrdgas = new GeoPhysVol(ltrdgas);

    
    //Build chamber volume out of G10 material (gas + pcb)
    GeoSimplePolygonBrep *sChamberVolume
        =new GeoSimplePolygonBrep(chamberTck/2.);
        sChamberVolume->addVertex(longWidthActive/2.,lengthActive/2.);
                sChamberVolume->addVertex(-longWidthActive/2.,lengthActive/2.);
                if (yCutout) sChamberVolume->addVertex(-longWidthActive/2.,lengthActive/2.-yCutout);
                sChamberVolume->addVertex(-widthActive/2.,-lengthActive/2.);
                sChamberVolume->addVertex(widthActive/2.,-lengthActive/2.);
                if (yCutout) sChamberVolume->addVertex(longWidthActive/2.,lengthActive/2.-yCutout);

    //Transform chamber volume
    CLHEP::Hep3Vector vv(0,0,0);
    CLHEP::HepRotation rott;
    rott.rotateX(M_PI/2.);
    rott.rotateZ(M_PI/2.);
    HepGeom::Transform3D transff(rott,vv);
    const GeoShape *sChamberVolume1=new GeoShapeShift(sChamberVolume,transff);

    
    const GeoMaterial* mtrdC = matManager->getMaterial("std::G10");    
    GeoLogVol* ltrdC = new GeoLogVol(logVolName, sChamberVolume1, mtrdC);
    GeoPhysVol* ptrdChamber = new GeoPhysVol(ltrdC); 
    GeoNameTag* ntrdtmpC = new GeoNameTag(name+"std::G10");                                                                                                                    
    GeoTransform* ttrdtmpC = new GeoTransform(HepGeom::TranslateX3D(newXPos)); 

    // Place gas volume inside chamber volume
    ptrdChamber->add(ptrdgas);

    //Place chamber volume inside the mother volume (honeycomb volume)
    ptrd->add(ntrdtmpC);                                                                                                                                   
    ptrd->add(ttrdtmpC);
    ptrd->add(ptrdChamber);

    //Cutouts
      if (!yCutout)
      {
          double lW=longWidth/2.-((longWidth-width)/2.)*f4/length;
          double W=width/2.+((longWidth-width)/2.)*f5/length;
          const GeoShape* trd1 = new GeoTrd(gasTck/2,gasTck/2, width/2,
                                    longWidth/2, length/2);
          const GeoShape* trd2 = new GeoTrd(gasTck,gasTck, W-f6,
                                    lW-f6, length/2-(f4+f5)/2.);
          HepGeom::Transform3D c;
          c=HepGeom::Translate3D(0,0,(f5-f4)/2.);
          trd1= &(trd1->subtract( (*trd2) << c ));
          GeoLogVol* ltrdframe = new GeoLogVol("sTGC_Frame", trd1,
                                         matManager->getMaterial("std::Aluminium"));
          GeoPhysVol* ptrdframe = new GeoPhysVol(ltrdframe);

          ptrdgas->add(ptrdframe);
      }
      else
      {
        double W=width/2.+((longWidth-width)/2.)*f5/(length);
      	  GeoSimplePolygonBrep *sGasV
        	=new GeoSimplePolygonBrep(gasTck);
        sGasV->addVertex(longWidthActive/2.-f6,lengthActive/2.-f4);
		sGasV->addVertex(-longWidthActive/2.+f6,lengthActive/2.-f4);
		sGasV->addVertex(-longWidthActive/2.+f6,lengthActive/2.-yCutout);
		sGasV->addVertex(-W+f6,-lengthActive/2.+f5);
		sGasV->addVertex(W-f6,-lengthActive/2.+f5);
		sGasV->addVertex(longWidthActive/2.-f6,lengthActive/2.-yCutout);
	
	const GeoShape *sGasV1=new GeoShapeShift(sGasV,transf);
	
	const GeoShape* sGasV2=&(sGasVolume1->subtract(*sGasV1));
	
	GeoLogVol* ltrdframe = new GeoLogVol("sTGC_Frame", sGasV2,
                                         matManager->getMaterial("std::Aluminium"));
          GeoPhysVol* ptrdframe = new GeoPhysVol(ltrdframe);

          ptrdgas->add(ptrdframe);
	
      }


      iSenLyr++;

      newpos = newXPos;
  } // Loop over stgc layers
        
  return ptrd;	
}


void sTGC::print()
{
  std::cout << " sTGC " << name << " :" << std::endl;
}

} // namespace MuonGM
