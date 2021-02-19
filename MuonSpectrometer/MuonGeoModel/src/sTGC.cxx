/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/sTGC.h"

#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <GeoModelKernel/GeoDefinitions.h>
#include <GeoModelKernel/GeoShape.h>
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoTrd.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"
#include "MuonAGDDDescription/sTGC_Technology.h"
#include "MuonGeoModel/Component.h"
#include "MuonGeoModel/sTGCComponent.h"

#include <cmath>
#include <string>

class GeoMaterial;

namespace MuonGM {

//Get sTGC components for the trapezoidal shape of the quadruplet
sTGC::sTGC(Component* ss): DetectorElement(ss->name)
{
  sTGCComponent* s = (sTGCComponent*)ss;
  m_component = s;
  width = s->dx1;
  longWidth = s->dx2;
  yCutout= s->yCutout;
  yCutoutCathode= s->yCutoutCathode;
  length = s->dy;
  name=s->name;
  index = s->index;
}


GeoFullPhysVol* sTGC::build(int minimalgeo)
{
  std::vector<Cutout*> vcutdef;
  int cutoutson = 0;
  return build(minimalgeo, cutoutson, vcutdef);
}


//Start building the physical volume of the quadruplet
GeoFullPhysVol* sTGC::build(int minimalgeo, int , std::vector<Cutout*> )
{
  AGDDDetectorStore* ds = AGDDDetectorStore::GetDetectorStore();
  sTGCDetectorHelper stgcHelper;
  sTGCDetectorDescription* stgc_descr = stgcHelper.Get_sTGCDetectorSubType(m_component->subType);

  sTGC_Technology* t = (sTGC_Technology*) ds->GetTechnology(name);
  thickness = t->Thickness();
  double gasTck=t->gasThickness;
  double pcbTck=t->pcbThickness;
  double f4=stgc_descr->ylFrame();
  double f5=stgc_descr->ysFrame();
  double f6=stgc_descr->xFrame();

  //Evaluate honeycomb thickness
  double chamberTck = gasTck+pcbTck; //Note: pcbTck is the xml value and is the combined thickness of 2 pcbs.
  double honeycombTck = (thickness - 4*chamberTck)/5;
  double pcbActualTck = pcbTck/2;

  minimalgeo=t->geoLevel;

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
  GeoTrf::Transform3D rot = GeoTrf::RotateZ3D(M_PI/2.)*GeoTrf::RotateX3D(M_PI/2.);
  const GeoShape *strd=new GeoShapeShift(solid,rot);

  logVolName=name;
  if (!(m_component->subType).empty()) logVolName+=("-"+m_component->subType);
  const GeoMaterial* mtrd = getMaterialManager()->getMaterial("muo::Honeycomb");
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

    double newXPos;
    double pcbpos;
    if (i==0) newXPos=newpos+ chamberTck/2 + honeycombTck ;
    else newXPos=newpos+ chamberTck + honeycombTck ;

    //Build chamber volume (gas + pcb) out of gas
    GeoSimplePolygonBrep *sGasVolume = new GeoSimplePolygonBrep(chamberTck/2.);
    sGasVolume->addVertex(longWidthActive/2.,lengthActive/2.);
    sGasVolume->addVertex(-longWidthActive/2.,lengthActive/2.);
    if (yCutout) sGasVolume->addVertex(-longWidthActive/2.,lengthActive/2.-yCutout);
    sGasVolume->addVertex(-widthActive/2.,-lengthActive/2.);
    sGasVolume->addVertex(widthActive/2.,-lengthActive/2.);
    if (yCutout) sGasVolume->addVertex(longWidthActive/2.,lengthActive/2.-yCutout);

    //Transform gas volume
    GeoTrf::Transform3D rot = GeoTrf::RotateZ3D(M_PI/2.)*GeoTrf::RotateX3D(M_PI/2.);
    const GeoShape *sGasVolume1=new GeoShapeShift(sGasVolume,rot);
    GeoLogVol* ltrdgas = new GeoLogVol("sTGC_Sensitive", sGasVolume1, getMaterialManager()->getMaterial("muo::TGCGas"));
    GeoPhysVol* ptrdgas = new GeoPhysVol(ltrdgas);
    GeoNameTag* gastag = new GeoNameTag(name+"muo::TGCGas");
    GeoTransform* chamberpos = new GeoTransform(GeoTrf::TranslateX3D(newXPos));

    //Build two pcb volumes and add them to the gas at -chamberTck/2 and at +chamberTck/2
    for(int i = 0; i < 2; i++){
      if (i==0) {
         //This becomes the zero reference point for the pcb at -chamberTck/2
        pcbpos = -chamberTck/2 + pcbActualTck/2;
      } else {
        //This becomes the zero reference point for the pcb at +chamberTck/2. Alternatively,
        // we can say pcbpos = +chamberTck/2 - pcbActualTck/2 ???
        pcbpos = -chamberTck/2 + pcbActualTck + gasTck + pcbActualTck/2;
      }

      //Build pcb volume out of G10 material
      GeoSimplePolygonBrep *sPcbVolume = new GeoSimplePolygonBrep(pcbActualTck/2.);
      sPcbVolume->addVertex(longWidthActive/2.,lengthActive/2.);
      sPcbVolume->addVertex(-longWidthActive/2.,lengthActive/2.);
      if (yCutout) sPcbVolume->addVertex(-longWidthActive/2.,lengthActive/2.-yCutout);
      sPcbVolume->addVertex(-widthActive/2.,-lengthActive/2.);
      sPcbVolume->addVertex(widthActive/2.,-lengthActive/2.);
      if (yCutout) sPcbVolume->addVertex(longWidthActive/2.,lengthActive/2.-yCutout);

      //Transform PCB volume
      GeoTrf::Transform3D rott = GeoTrf::RotateZ3D(M_PI/2.)*GeoTrf::RotateX3D(M_PI/2.);
      const GeoShape *sPcbVolume1=new GeoShapeShift(sPcbVolume,rott);

      const GeoMaterial* mtrdC = getMaterialManager()->getMaterial("std::G10");
      GeoLogVol* ltrdC = new GeoLogVol(logVolName, sPcbVolume1, mtrdC);
      GeoPhysVol* ptrdPcb = new GeoPhysVol(ltrdC);
      GeoNameTag* ntrdtmpC = new GeoNameTag(name+"std::G10");

      GeoTransform* ttrdtmpC = new GeoTransform(GeoTrf::TranslateX3D(pcbpos));


      // Place pcb volume inside chamber volume
      ptrdgas->add(ntrdtmpC);//nametag
      ptrdgas->add(ttrdtmpC);//shift
      ptrdgas->add(ptrdPcb);//volume
    } //Close loop on pcb volumes

    //Place chamber volume inside the mother volume (honeycomb volume)
    ptrd->add(gastag);
    ptrd->add(chamberpos);
    ptrd->add(ptrdgas);

    //Cutouts
    if (!yCutout) {
      double lW=longWidth/2.-((longWidth-width)/2.)*f4/length;
      double W=width/2.+((longWidth-width)/2.)*f5/length;
      const GeoShape* trd1 = new GeoTrd(gasTck/2,gasTck/2, width/2,
                                longWidth/2, length/2);
      const GeoShape* trd2 = new GeoTrd(gasTck,gasTck, W-f6,
                                lW-f6, length/2-(f4+f5)/2.);
      GeoTrf::Translate3D c(0,0,(f5-f4)/2.);
      trd1= &(trd1->subtract( (*trd2) << c ));
      GeoLogVol* ltrdframe = new GeoLogVol("sTGC_Frame", trd1,
                                     getMaterialManager()->getMaterial("std::Aluminium"));
      GeoPhysVol* ptrdframe = new GeoPhysVol(ltrdframe);

      ptrdgas->add(ptrdframe);
    } else {
      double W=width/2.+((longWidth-width)/2.)*f5/(length);
      // This describes the active area
    	GeoSimplePolygonBrep *sGasV=new GeoSimplePolygonBrep(gasTck/2.);
      sGasV->addVertex(longWidthActive/2.-f6,lengthActive/2.-f4);
		  sGasV->addVertex(-longWidthActive/2.+f6,lengthActive/2.-f4);
		  sGasV->addVertex(-longWidthActive/2.+f6,lengthActive/2.-f4-yCutoutCathode);
		  sGasV->addVertex(-W+f6,-lengthActive/2.+f5);
		  sGasV->addVertex(W-f6,-lengthActive/2.+f5);
		  sGasV->addVertex(longWidthActive/2.-f6,lengthActive/2.-f4-yCutoutCathode);
      
      // This describes the enveloppe (active area + frames) 
    	GeoSimplePolygonBrep *sGasV2=new GeoSimplePolygonBrep(gasTck/2.);
      sGasV2->addVertex(longWidth/2.,length/2.);
		  sGasV2->addVertex(-longWidth/2.,length/2.);
		  sGasV2->addVertex(-longWidth/2.,length/2.-yCutout);
		  sGasV2->addVertex(-width/2.,-length/2.);
		  sGasV2->addVertex(width/2.,-length/2.);
		  sGasV2->addVertex(longWidth/2.,length/2.-yCutout);
	
      // define the final geo shapes
	    const GeoShape *sGasV1=new GeoShapeShift(sGasV,rot);
	    const GeoShape *sGasV3=new GeoShapeShift(sGasV2,rot);
      // Remove active from active+frames to only get frames
      sGasV3= &(sGasV3->subtract( (*sGasV1)));
	
	    GeoLogVol* ltrdframe = new GeoLogVol("sTGC_Frame", sGasV3,
                                       getMaterialManager()->getMaterial("std::Aluminium"));
      GeoPhysVol* ptrdframe = new GeoPhysVol(ltrdframe);

      // Add frame volume to QL3
      ptrdgas->add(ptrdframe);
    }

    iSenLyr++;
    newpos = newXPos;
  } // Loop over stgc layers

  return ptrd;
}


void sTGC::print()
{
  MsgStream log(Athena::getMessageSvc(), "MuGM::sTGC");
  log << MSG::INFO << " sTGC " << name << " :" << endmsg;
}

} // namespace MuonGM
