/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//  Update: Riccardo Maria BIANCHI rbianchi@cern.ch Feb 2014                   //

#include "VP1Utils/HitToSoNode.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Base/VP1Msg.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>

#include "TrkSurfaces/Surface.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include <sstream>
#include <cmath>


#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

class HitToSoNode::Imp {
public:
  Imp() : theclass(0), driftdischeight(0.15) {}
  HitToSoNode * theclass;
  const double driftdischeight;
  HitsSoNodeManager nodeManager;
};

HitToSoNode::HitToSoNode(IVP1System * sys)
    : VP1HelperClassBase(sys,"HitToSoNode"), m_d(new Imp)
{
    m_d->theclass = this;
}

//____________________________________________________________________
HitToSoNode::~HitToSoNode()
{
    messageVerbose("destructor begin");
    delete m_d;
    messageVerbose("destructor end");
}

double HitToSoNode::getTGCAngle(Identifier id) const {
  const TgcIdHelper * idhelper = VP1DetInfo::tgcIDHelper();
  if (!idhelper) {
    VP1Msg::message("HitToSoNode::getTGCAngle WARNING: could not get TGC idhelper!");
    return 0.0;
  }
  
  if ( !idhelper->isStrip(id)) return 0.0; // Not phi strip
  const MuonGM::TgcReadoutElement* detEl = VP1DetInfo::muonDetMgr ()->getTgcReadoutElement(id);
  
  // int stationeta=idHelper->stationEta(id);
  // int stripNo = idHelper->channel(id);
  // int gasGap = idHelper->gasGap(id);
  // 
  // const MuonGM::TgcReadoutElement *ele=  VP1DetInfo::muonDetMgr ()->getTgcReadoutElement(id);
  // 
  // double stripLength = ele->stripLength(gasGap,stripNo);
  // double stripWidth=fabs(ele->stripMaxX(gasGap, stripNo, (*lp)[Trk::locZ])-ele->stripMinX(gasGap, stripNo,  (*lp)[Trk::locZ]));
  // 
  // double localX1=ele->stripCtrX(gasGap, stripNo,  stripLength/2.);
  // double localX2=ele->stripCtrX(gasGap, stripNo,  -stripLength/2.);
  // if (stationeta>0) {
  //   localX1=-localX1;
  //   localX2=-localX2;
  // }
  // 
  // return atan2(localX2-localX1,stripLength);
  
    int stripNo = idhelper->channel(id); 
    int gasGap  = idhelper->gasGap(id); 


// calculate two points along the tgc phi strip in the local tgc reference frame 
//    HepGeom::Point3D<double> lposTGC = detEl->localChannelPos(id);
    Amg::Vector3D lposTGC = detEl->localChannelPos(id);
    double z_shift = lposTGC.z()+10; 
    double locy_shift = detEl->stripCtrX(gasGap, stripNo, z_shift ); 
    if (0 < detEl->getStationEta()) { 
        locy_shift *= -1.; // Needed because the coord system for  stripCtrX is flipped compared to localChannelPos (!)
    } 
    Amg::Vector3D lpos_shift(lposTGC.x(),locy_shift,z_shift);

    const Amg::Transform3D tgcTrans = Amg::CLHEPTransformToEigen(detEl->getMaterialGeom()->getAbsoluteTransform());
    Amg::Vector3D gpos = tgcTrans * lposTGC;
    Amg::Vector3D gpos_shift = tgcTrans*lpos_shift;

    const Trk::Surface& surf =  detEl->surface(id);
    const Amg::Vector2D* locPos1 = surf.globalToLocal(gpos,100);
    const Amg::Vector2D* locPos2 = surf.globalToLocal(gpos_shift,100);

    if (!locPos1 || !locPos2) {
        VP1Msg::message("HitToSoNode::getTGCangle() Warning: global to local failed - cannot make transform!");
        delete locPos1; delete locPos2;
        return 0;
    }

    Amg::Vector2D difPos = (*locPos2) - (*locPos1);
// std::cout << " Strip pos " << *locPos1 << " shifted " << *locPos2 << " dif " << difPos << std::endl;
    double tmp= difPos[Trk::locY] / sqrt(pow(difPos[Trk::locX],2)+pow(difPos[Trk::locY],2));
    delete locPos1; delete locPos2;

    tmp = (tmp>1.0) ? 1.0 : tmp;
    tmp = (tmp<-1.0) ? -1.0 : tmp;
    return atan2( difPos[Trk::locX],  difPos[Trk::locY]);
    
}

//____________________________________________________________________
SoTransform * HitToSoNode::createTransform(const Trk::RIO_OnTrack& rio, bool useSurfacePositionOnly) const
{  
    const Trk::Surface& theSurface = rio.detectorElement()->surface(rio.identify());
    SoTransform * theHitTransform = VP1LinAlgUtils::toSoTransform(theSurface.transform());

  // const Amg::Vector3D* theHitGPos;
    if ( useSurfacePositionOnly ) {
    // if (verbose) VP1Msg::message("useSurfacePositionOnly");
        theHitTransform->translation.setValue(theSurface.center()[0], theSurface.center()[1], theSurface.center()[2]);

    } else {

    // for strips, clusters or short tubes, use position of hit.
    // FIXME - for DCs, shouldn't this be the d0=0 position, i.e. on the wire?
        theHitTransform->translation.setValue(rio.globalPosition()[0], rio.globalPosition()[1], rio.globalPosition()[2]);
    }
    return theHitTransform;
}

SoTransform * HitToSoNode::createTGC_Transform(const Trk::RIO_OnTrack& rio, bool useSurfacePositionOnly) const {
  // std::cout<<"createTGC_Transform for:\n"<<rio<<std::endl;

  const TgcIdHelper * idhelper = VP1DetInfo::tgcIDHelper();
  const MuonGM::TgcReadoutElement* detEl = dynamic_cast<const MuonGM::TgcReadoutElement*>(rio.detectorElement()); 

  if (!idhelper||!detEl) {
    return new SoTransform;
  }

  const Trk::Surface& theSurface = rio.detectorElement()->surface(rio.identify());
  SoTransform * theHitTransform =0;
  if (idhelper->isStrip( rio.identify() )){
    // std::cout<<"Strip"<<std::endl;
    
      // for phi strips, use sinstereo to get correct orientation

    double angle = getTGCAngle(rio.identify());

    Amg::RotationMatrix3D localRot;
    localRot.setIdentity();
    
    if (0 < detEl->getStationEta()) { 
//      localRot.setPhi (angle);
    	Amg::setPhi(localRot, angle);
    } else {
//      localRot.setPhi (-angle);
      Amg::setPhi(localRot, -angle);
    }
    // std::cout<<"Local ROT = "<<localRot<<std::endl;

    Amg::toString(localRot);
    localRot *= theSurface.transform().rotation();

    // std::cout<<"translation = "<<Amg::toString(theSurface.transform().translation())<<std::endl;

//    HepGeom::Transform3D trans (localRot, theSurface.transform().translation());
    Amg::Transform3D trans = Amg::getTransformFromRotTransl(localRot, theSurface.transform().translation() );
    
    // std::cout<<"transform = "<<Amg::toString(trans)<<std::endl;

    theHitTransform = VP1LinAlgUtils::toSoTransform(trans);
  } else {
   // eta strips just use surface orientiation
    theHitTransform = VP1LinAlgUtils::toSoTransform(theSurface.transform());
  }

  if (!useSurfacePositionOnly){ 
    const Amg::Vector3D& theHitGPos= rio.globalPosition();
    theHitTransform->translation.setValue((theHitGPos)[0], (theHitGPos)[1], (theHitGPos)[2]);
  }
  return theHitTransform;
}

void HitToSoNode::buildTubeShapes(const Trk::RIO_OnTrack& rio, SoSeparator*&shape_simple, SoSeparator*&shape_detailed, bool /**blockGP*/, double length, bool doProjection)
{
    const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(rio.detectorElement()->surface(rio.identify()).bounds()));
    assert(ccbo!=0);
    if (!ccbo) {
      VP1Msg::message("HitToSoNode::buildTubeShapes ERROR: Trk::CylinderBounds conversion failed");
      return;
    }

    double radius = fabs(rio.localParameters().get(Trk::locR));
    if (radius<0.15)
        radius = 0.0;//radius is so small it is better to collapse to line
        
    double halflength(ccbo->halflengthZ());
    
    if (doProjection && dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(&rio)){

      double strawLength=2.0*halflength;
      if (strawLength<160.0)
        strawLength = 2*349.3150-strawLength;//Yeah, we hardcode a lot here... but dimensions of TRT barrel straws are not likely to change.

      SoPointSet       * scatPointSet    = new SoPointSet;
      SoVertexProperty * scatVtxProperty = new SoVertexProperty;
      scatVtxProperty->vertex.set1Value(0,0.0f,0.0f,strawLength);
      scatPointSet->numPoints=1;
      scatPointSet->vertexProperty.setValue(scatVtxProperty);

      shape_detailed->addChild(scatPointSet);
      shape_simple->addChild(scatPointSet);

    } else {
      if (length!=0.0) {
        halflength = length/2.0;
        // FIXME! translate to z position here instead of in AscObj_TSOS, for symmetry with buildStripShapes
      }
      SoNode * simpleShape = m_d->nodeManager.getShapeNode_DriftTube( halflength, 0.0 );
      shape_simple->addChild(simpleShape);
         //Detailed shape uses tube (unless negligible radius):
    if (radius==0.0)
        shape_detailed->addChild(simpleShape);
    else
        shape_detailed->addChild(m_d->nodeManager.getShapeNode_DriftTube( halflength, radius ));
    }
}

void HitToSoNode::buildStripShapes(const Trk::RIO_OnTrack& rio, SoSeparator*&shape_simple, SoSeparator*&shape_detailed, bool blockGP)
{
    // assuming starting with position being that of centre of surface i.e. always working in local coord system


    double stripLength =100.0, stripWidth = 10.0, stripThickness=1.0;
    const Amg::Vector2D* localposROT=0;
    Amg::Vector2D* localposStrip=0;
    Identifier id=rio.identify();
    fillValues(id, rio.detectorElement(), stripLength, stripWidth, stripThickness, localposStrip);

  //std::cout<<"Got back: "<<stripLength<<","<<stripWidth<<","<<stripThickness<<std::endl;
  //std::cout<<"LocalposStrip="<<*localposStrip<<std::endl;

  // if (static_cast<PRDCollHandle_TGC*>(collHandle())->project())
  //   striplength += 300.0;//Fixme: Rough extension for now
    double tolerance = 3.;
    localposROT = rio.detectorElement()->surface( rio.identify() ).globalToLocal(rio.globalPosition(), tolerance);

    if( !localposROT )
    {
        localposROT = new Amg::Vector2D;
        VP1Msg::message("Warning: Local hit position was NULL");
    }

    if (!blockGP){
      SoSeparator * gpSep = new SoSeparator;
        SoTranslation * localtransGP = new SoTranslation;
        localtransGP->translation.setValue(static_cast<float>( (*localposROT)[Trk::locX]),
            static_cast<float>( (*localposROT)[Trk::locY]),
            0.0f);
            
        gpSep->addChild(localtransGP);
        gpSep->addChild(m_d->nodeManager.getShapeNode_Cross(10));
        shape_detailed->addChild(gpSep);
    }


    SoTranslation * localtrans0 = new SoTranslation;
    localtrans0->translation.setValue(static_cast<float>( (*localposStrip)[Trk::locX]),
        static_cast<float>( (*localposStrip)[Trk::locY]),
        0.0f);


    shape_simple->addChild(localtrans0);
    shape_simple->addChild( m_d->nodeManager.getShapeNode_Strip(stripLength));

    shape_detailed->addChild(localtrans0);
    shape_detailed->addChild(m_d->nodeManager.getShapeNode_Strip(stripLength,stripWidth,stripThickness));
    // Transform back to centre of Surface
    SoTranslation * localtrans1 = new SoTranslation;
    localtrans1->translation.setValue(static_cast<float>( -(*localposStrip)[Trk::locX]),
        static_cast<float>( -(*localposStrip)[Trk::locY]),
        0.0f);
    shape_simple->addChild(localtrans1);
    shape_detailed->addChild(localtrans1);

    delete localposStrip;
    delete localposROT;    
}

void HitToSoNode::fillValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness, Amg::Vector2D*& localposStrip){

    const AtlasDetectorID * idhelper = VP1DetInfo::atlasIDHelper();
    if (!idhelper) return;
    if (idhelper->is_muon(id)){
        if (idhelper->is_rpc(id)) { fillRPCValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;}
        if (idhelper->is_tgc(id)) { fillTGCValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;}
        if (idhelper->is_csc(id)) { fillCSCValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;}
        if (idhelper->is_stgc(id)) { fillSTGCValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;}
        if (idhelper->is_mm(id)) { fillMMValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;}
    } else {
        if (idhelper->is_indet(id) && (idhelper->is_pixel(id)||idhelper->is_sct(id))) { 
            fillSiValues(id, baseDetEl, striplength, stripWidth, stripThickness, localposStrip); return;
        }
    }
    VP1Msg::message("Warning: HitToSoNode::fillValues(...) unknown technology.");
    return;
}

void HitToSoNode::fillTGCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth,double& stripThickness, Amg::Vector2D*& localposStrip){
    const TgcIdHelper * idhelper = VP1DetInfo::tgcIDHelper();
    const MuonGM::TgcReadoutElement* detEl =
        dynamic_cast<const MuonGM::TgcReadoutElement*>(baseDetEl);
    if (!idhelper || !detEl){
        VP1Msg::message("Could not get either TGC IdHelper or det element");
        localposStrip = new Amg::Vector2D;
        return;
    }

    int plane = idhelper->gasGap( id );
    int strip = idhelper->channel( id );
    int isStrip = idhelper->isStrip( id );

    if (isStrip){
        striplength = detEl->stripLength(plane, strip);
        stripWidth = detEl->stripWidth(plane, strip);
        stripThickness = 3*0.8+0.1;
    } else {    
        striplength = detEl->gangShortWidth(plane, strip);
        stripWidth = detEl->gangLength(plane, strip);
        stripThickness = 3*0.8;
    }
    stripWidth=std::max(10.0,stripWidth);
 // Translation from objects local position to the strip position
  // get local position on wire plane, here we have to use a tolarance as the wire plane is located 2.5 CLHEP::mm
  // from the strip plane

    Amg::Vector2D localPos_a;
    Amg::Vector3D dummy; // this is not actually used
    Amg::Vector3D globalPos_a = detEl->channelPos( id );
    bool ok = detEl->surface( id ).globalToLocal(globalPos_a, dummy, localPos_a);
    if (ok) {
    		localposStrip = new Amg::Vector2D(localPos_a);
    }
    else{
    	VP1Msg::message("fillTGCValues - Could not get localposStrip!!");
    }


  // std::cout<<"HitToSoNode::fillTGCValues channelPos = "<<detEl->channelPos( id )<<std::endl;
    if( !localposStrip )
    {
        localposStrip = new Amg::Vector2D;
        VP1Msg::message("Warning: Local strip position was NULL");
    }
    return;
}

void HitToSoNode::fillRPCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness, Amg::Vector2D*& localposStrip){
    const RpcIdHelper * idhelper = VP1DetInfo::rpcIDHelper();
    const MuonGM::RpcReadoutElement* detEl =
        dynamic_cast<const MuonGM::RpcReadoutElement*>(baseDetEl);
    if (!idhelper || !detEl){
        VP1Msg::message("Could not get either RPC IdHelper or det element");
        localposStrip = new Amg::Vector2D;
        return;
    }  

    int measPhi = idhelper->measuresPhi( id );
    striplength = detEl->StripLength(measPhi);
    stripWidth = detEl->StripWidth(measPhi),//strip width
        stripThickness = (measPhi==1) ? 0.9: 0.8;//strip thickness - different from phi/eta to avoid hatching issues

  // Translation from objects local position to the strip position
    Amg::Vector2D localPos_a;
    Amg::Vector3D dummy; // this is not actually used
    Amg::Vector3D globalPos_a = detEl->stripPos( id );
    bool ok = detEl->surface( id ).globalToLocal(globalPos_a, dummy, localPos_a);
    if (ok) {
    	localposStrip = new Amg::Vector2D(localPos_a);
    }
    else{
    	VP1Msg::message("fillRPCValues - Could not get localposStrip!!");
    }

    //std::cout<<"HitToSoNode::fillRPCValues channelPos = "<<detEl->stripPos( id )<<std::endl;

    if( !localposStrip )
    {
        localposStrip = new Amg::Vector2D;
        VP1Msg::message("Warning: Local strip position is NULL");
    }
}

void HitToSoNode::fillCSCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness, Amg::Vector2D*& localposStrip){
    const CscIdHelper * idhelper = VP1DetInfo::cscIDHelper();
    const MuonGM::CscReadoutElement* detEl =
        dynamic_cast<const MuonGM::CscReadoutElement*>(baseDetEl);
    if (!idhelper || !detEl){
        VP1Msg::message("Could not get either CSC IdHelper or det element");
        localposStrip = new Amg::Vector2D;
        return;
    }

    int measPhi = idhelper->measuresPhi( id );
    int chamberLayer = idhelper->chamberLayer( id );

    striplength = detEl->stripLength(id);
    stripWidth  = detEl->cathodeReadoutPitch(chamberLayer,measPhi);
    stripThickness = (measPhi==1) ? 0.9: 0.8;//strip thickness - different from phi/eta to avoid hatching issues

  // Translation from objects local position to the strip position
    Amg::Vector2D localPos_a;
    Amg::Vector3D dummy; // this is not actually used
    Amg::Vector3D globalPos_a = detEl->stripPos( id );
    bool ok = detEl->surface( id ).globalToLocal(globalPos_a, dummy, localPos_a);
    if (ok) {
    	localposStrip = new Amg::Vector2D(localPos_a);
    }
    else{
    	VP1Msg::message("fillCSCValues - Could not get localposStrip!!");
    }



    if( !localposStrip )
    {
        localposStrip = new Amg::Vector2D;
        VP1Msg::message("Warning: Local strip position is NULL");
    }
}

void HitToSoNode::fillSTGCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness, Amg::Vector2D*& localposStrip){
    const sTgcIdHelper * idhelper = VP1DetInfo::stgcIDHelper();
    MuonGM::sTgcReadoutElement* detEl =
        const_cast<MuonGM::sTgcReadoutElement*>(dynamic_cast<const MuonGM::sTgcReadoutElement*>(baseDetEl));
    if (!idhelper || !detEl){
        VP1Msg::message("Could not get either STGC IdHelper or det element");
        localposStrip = new Amg::Vector2D;
        return;
    }

    striplength=100; //FIXME!
    stripWidth=2.7;
    stripThickness=1;
    // int measPhi = idhelper->measuresPhi( id );
    // int chamberLayer = idhelper->chamberLayer( id );
    // 
    // striplength = detEl->stripLength(id);
    // stripWidth  = detEl->cathodeReadoutPitch(chamberLayer,measPhi);
    // stripThickness = (measPhi==1) ? 0.9: 0.8;//strip thickness - different from phi/eta to avoid hatching issues

    localposStrip = new Amg::Vector2D;
    if( !(detEl->stripPosition(id,*localposStrip)) )
    {
        VP1Msg::message("Warning: Local strip position is NULL");
    } 
}

void HitToSoNode::fillMMValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,  Amg::Vector2D*& localposStrip){
    const MmIdHelper * idhelper = VP1DetInfo::mmIDHelper();
    const MuonGM::CscReadoutElement* detEl =
        dynamic_cast<const MuonGM::CscReadoutElement*>(baseDetEl);
    if (!idhelper || !detEl){
        VP1Msg::message("Could not get either CSC IdHelper or det element");
        localposStrip = new Amg::Vector2D;
        return;
    }
  // 
  
  // FIXME - get dimensions!
  
    striplength=100; //FIXME!
    stripWidth=10;
    stripThickness=1;
  //   int measPhi = idhelper->measuresPhi( id );
  //   int chamberLayer = idhelper->chamberLayer( id );
  // 
  //   striplength = detEl->stripLength(id);
  //   stripWidth  = detEl->cathodeReadoutPitch(chamberLayer,measPhi);
  //   stripThickness = (measPhi==1) ? 0.9: 0.8;//strip thickness - different from phi/eta to avoid hatching issues
  // 
  // // Translation from objects local position to the strip position
    localposStrip = new Amg::Vector2D;
  
    if( !(detEl->stripPosition(id,*localposStrip)) )
    {
        VP1Msg::message("Warning: Local strip position is NULL");
    }
}

void HitToSoNode::fillSiValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,  Amg::Vector2D*& localposStrip){
    const InDetDD::SiDetectorElement* detEl =
        dynamic_cast<const InDetDD::SiDetectorElement*>(baseDetEl);
    if ( !detEl){
        VP1Msg::message("Could not get Si det element");
        localposStrip = new Amg::Vector2D;
        return;
    }

    localposStrip = new Amg::Vector2D(detEl->localPositionOfCell( id ));

    striplength    = detEl->etaPitch() ;
    stripWidth     = detEl->phiPitch( *localposStrip );
    stripThickness = detEl->thickness()/10.0;
    
}
