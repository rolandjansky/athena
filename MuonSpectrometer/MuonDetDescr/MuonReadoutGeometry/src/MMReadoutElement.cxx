/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The MM detector = an assembly module = STGC in amdb 
 ----------------------------------------------------
***************************************************************************/

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "MuonAlignmentData/BLinePar.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

namespace MuonGM {


  MMReadoutElement::MMReadoutElement(GeoVFullPhysVol* pv, std::string stName,
				     int zi, int fi, int mL, bool is_mirrored,
				     MuonDetectorManager* mgr)
    : MuonClusterReadoutElement(pv, stName, zi, fi, is_mirrored, mgr),
      m_BLinePar(0)
  {
    m_rots = 0.;
    m_rotz = 0.;
    m_rott = 0.;

    m_hasALines = false;
    m_hasBLines = false;
    m_delta = Amg::Transform3D::Identity();
    m_ml = mL;
    
    // get the setting of the caching flag from the manager
    setCachingFlag(mgr->cachingFlag());

    std::string vName = pv->getLogVol()->getName();
    std::string sName = vName.substr(vName.find("-")+1);
    std::string fixName = (sName[2]=='L') ? "MML" : "MMS";

    setStationName(fixName);
    setStationEta(zi);
    setStationPhi(fi);    
    setChamberLayer(mL);
    Identifier id = mgr->mmIdHelper()->channelID(fixName,zi,fi,mL,1,1);
    setIdentifier(id);

    m_nlayers=0;
    m_halfX =  100.;
    m_minHalfY = 1900.;
    m_maxHalfY = 2000.;
    bool foundShape = false;

    if (mgr->MinimalGeoFlag() == 0) {
      if (GeoFullPhysVol* pvc = dynamic_cast<GeoFullPhysVol*> (pv)) {
	unsigned int nchildvol = pvc->getNChildVols();
	int llay = 0;
	std::string::size_type npos;
	for (unsigned ich=0; ich<nchildvol; ++ich) {
	  PVConstLink pc = pvc->getChildVol(ich);
	  std::string childname = (pc->getLogVol())->getName();
	  if ((npos = childname.find("Sensitive")) != std::string::npos ) {
	    llay ++;
            if (llay > 4) {
              MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
              log << MSG::WARNING<<"number of MM layers > 4: increase transform array size"<< endmsg;
              continue;
	    }
           m_Xlg[llay-1] = pvc->getXToChildVol(ich);
            // save layer dimensions
            if (llay==1) {
              if (pc->getLogVol()->getShape()->type()=="Trd") {
		const GeoTrd* trd = dynamic_cast<const GeoTrd*> (pc->getLogVol()->getShape());
                m_halfX =  trd->getZHalfLength();
                // adjust phi dimensions according to the active area  
                m_minHalfY = trd->getYHalfLength1();
                m_maxHalfY = trd->getYHalfLength2();
		foundShape = true;
 	      }	else if (pc->getLogVol()->getShape()->type()=="Subtraction") {
		const GeoShapeSubtraction* sh = dynamic_cast<const GeoShapeSubtraction*> (pc->getLogVol()->getShape());
                const GeoShape* sub = sh->getOpA();  

                while (sub->type()=="Subtraction") {
                   sh = dynamic_cast<const GeoShapeSubtraction*> (sub);
                   sub = sh->getOpA();
		}
		const GeoTrd* trd = dynamic_cast<const GeoTrd*> (sub);
                
                if (!trd) {
                  MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
                  log << MSG::WARNING<<"MM layer base shape not trapezoid? " << sub->type()<<endmsg;
		} else {
		  m_halfX =  trd->getZHalfLength();
		  m_minHalfY = trd->getYHalfLength1();
		  m_maxHalfY = trd->getYHalfLength2();
		  foundShape = true;
                }
              } else {
                MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
                log << MSG::WARNING<<"MM layer shape not recognized:" <<pc->getLogVol()->getShape()->type()<<endmsg;
	      }
	    }
	  }
	}
        m_nlayers=llay;
      } else {
        MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
        log << MSG::WARNING<<"Cannot perform a dynamic cast ! "<<endmsg;
      }
    }
    if( !foundShape ){
      MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
      log << MSG::WARNING << " failed to initialize dimensions of this chamber " << endmsg;
    }
  }

  MMReadoutElement::~MMReadoutElement()
  {
    clearCache();
  }

  void  MMReadoutElement::setIdentifier(Identifier id)
  {
    m_id = id;
    IdentifierHash collIdhash = 0;
    IdentifierHash detIdhash = 0;
    // set parent data collection hash id
    if (manager()->mmIdHelper()->get_module_hash(id, collIdhash) != 0) {
      MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
      log << MSG::WARNING << "MMReadoutElement --  collection hash Id NOT computed for id = " << manager()->mmIdHelper()->show_to_string(id) << endmsg;
    }
    m_idhash = collIdhash;
    // // set RE hash id 
    if (manager()->mmIdHelper()->get_detectorElement_hash(id, detIdhash) != 0) {
      MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
      log << MSG::WARNING << "MMReadoutElement --  detectorElement hash Id NOT computed for id = " <<manager()->mmIdHelper()->show_to_string(id) << endmsg;
    }
    m_detectorElIdhash = detIdhash;
  }

  void MMReadoutElement::initDesign( double /*maxY*/, double /*minY*/, double /*xS*/, double /*pitch*/, double /*thickness*/) {
        
    m_etaDesign= std::vector<MuonChannelDesign>(m_nlayers);
     
    for (int il=0; il<m_nlayers; il++) {

      // identifier of the first channel to retrieve max number of strips
      Identifier id = manager()->mmIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, il+1, 1);
      int chMax =  manager()->mmIdHelper()->channelMax(id);
      if ( chMax < 0 ) {
        chMax = 2500;
        MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
        log << MSG::WARNING<<"MMReadoutElement -- Max number of strips not a real value"<<endmsg;
      }
      char side = getStationEta() < 0 ? 'C' : 'A';
      char sector_l = getStationName().substr(2,1)=="L" ? 'L' : 'S';
      MMDetectorHelper aHelper;
      MMDetectorDescription* mm = aHelper.Get_MMDetector(sector_l, std::abs(getStationEta()), getStationPhi(), m_ml, side);
      MMReadoutParameters roParam = mm->GetReadoutParameters();


      m_halfX = roParam.activeH/2;    //0.5*radial length (active area)
      m_minHalfY = roParam.activeBottomLength/2;   //0.5*bottom length (active area)
      m_maxHalfY = roParam.activeTopLength/2;       //0.5*top length (active area)

      m_etaDesign[il].type=MuonChannelDesign::Type::etaStrip;
      m_etaDesign[il].detType=MuonChannelDesign::DetType::MM;

      m_etaDesign[il].xSize=2*m_halfX;          // radial length (active area)
      m_etaDesign[il].minYSize=2*m_minHalfY;   //bottom length (active area)
      m_etaDesign[il].maxYSize=2*m_maxHalfY;  //top length (active area)

      double pitch =  roParam.stripPitch;
      m_etaDesign[il].inputPitch = pitch;
      m_etaDesign[il].inputLength = m_etaDesign[il].minYSize;
      m_etaDesign[il].inputWidth = pitch; //inputwidth is defined as the pitch
      m_etaDesign[il].thickness = roParam.gasThickness;
  
      m_etaDesign[il].nMissedTopEta = roParam.nMissedTopEta;   // #of eta strips that are not connected to any FE board
      m_etaDesign[il].nMissedBottomEta = roParam.nMissedBottomEta;
      m_etaDesign[il].nMissedTopStereo = roParam.nMissedTopStereo; // #of stereo strips that are not connected to any FE board
      m_etaDesign[il].nMissedBottomStereo = roParam.nMissedBottomStereo;
      m_etaDesign[il].nRoutedTop = roParam.nRoutedTop;  // #of stereo strips that are shorter in length (low efficient regions)
      m_etaDesign[il].nRoutedBottom = roParam.nRoutedBottom;
      m_etaDesign[il].dlStereoTop = roParam.dlStereoTop; // the length kept between the intersection point of the first/last active strips (eta,stereo) till the very edge of the first/last active stereo strip
      m_etaDesign[il].dlStereoBottom = roParam.dlStereoBottom;
      m_etaDesign[il].minYPhiL = roParam.minYPhiL; // radial distance kept between the first active strips (eta,stereo) [for the bottom parts two distances are defined at left and right corners as LM1 has not any routed strips]
      m_etaDesign[il].minYPhiR = roParam.minYPhiR;
      m_etaDesign[il].maxYPhi = roParam.maxYPhi;
      m_etaDesign[il].totalStrips = roParam.tStrips;
      m_etaDesign[il].sAngle = (roParam.stereoAngle).at(il);
      if (m_ml < 1 || m_ml > 2) {
        MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
        log << MSG::WARNING <<"MMReadoutElement -- Unexpected Multilayer: m_ml= " << m_ml <<endmsg;
      }
      
      if (m_etaDesign[il].sAngle == 0.) {    // eta layers

        m_etaDesign[il].firstPos = -0.5*m_etaDesign[il].xSize + pitch;
        m_etaDesign[il].signY  = 1 ;
        m_etaDesign[il].nch = ((int) std::round( (m_etaDesign[il].xSize/pitch))) + 1; // Total number of active strips
	
      } else { // stereo layers
          
        m_etaDesign[il].signY  = il==2? 1 : -1 ;
        
        // define the distance from the frame till the point that the first and last active stereo strips cross the center of the chamber (low_swift & up_swift)
        // in order to derive the total number of active strips for the stereo layer

        double low_swift=( m_minHalfY -m_etaDesign[il].dlStereoBottom)*std::abs(std::tan(m_etaDesign[il].sAngle));
        double up_swift = (m_maxHalfY - m_etaDesign[il].dlStereoTop)*std::abs(std::tan(m_etaDesign[il].sAngle));

        double lm1_swift =0;
        if(sector_l=='L' && (std::abs(getStationEta()))==1){
          lm1_swift = (m_etaDesign[il].minYPhiR - m_etaDesign[il].minYPhiL)/2 + m_etaDesign[il].minYPhiL;
          low_swift = 0;
        }

        double fPos = -0.5*m_etaDesign[il].xSize - low_swift + lm1_swift;
        double lPos = 0.5*m_etaDesign[il].xSize + up_swift;

        m_etaDesign[il].nch = ((int)std::round( (lPos - fPos)/pitch )) + 1;
        m_etaDesign[il].firstPos = ( -0.5*m_etaDesign[il].xSize + (m_etaDesign[il].nMissedBottomStereo - m_etaDesign[il].nMissedBottomEta)*pitch) + pitch;

      }
      m_nStrips.push_back(m_etaDesign[il].totalStrips);

      MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
      if (log.level()<=MSG::DEBUG) log << MSG::DEBUG <<"initDesign:" << getStationName()<< " layer " << il << ", strip pitch " << m_etaDesign[il].inputPitch << ", nstrips " << m_etaDesign[il].nch << " stereo " <<  m_etaDesign[il].sAngle << endmsg;

    }

  }
    
    void MMReadoutElement::fillCache()
  {
    if( !m_surfaceData ) m_surfaceData = new SurfaceData();
    else{
      MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
      log << MSG::WARNING<<"calling fillCache on an already filled cache" << endmsg;
      return;
    }

    m_surfaceData->m_surfBounds.push_back( new Trk::RotatedTrapezoidBounds( m_halfX, m_minHalfY, m_maxHalfY));

#ifndef NDEBUG
    MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
#endif
    for( int layer = 0; layer < m_nlayers; ++layer ){
      // identifier of the first channel
      Identifier id = manager()->mmIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, layer+1, 1);

      // keep the tracking surface at the center of the gap
      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*m_Xlg[layer]*
						 Amg::AngleAxis3D(-90.*CLHEP::deg,Amg::Vector3D(0.,1.,0.)) );

      // surface info (center, normal) 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

#ifndef NDEBUG
      double sAngle = m_etaDesign[layer].sAngle;
      if (log.level()<=MSG::DEBUG) log << MSG::DEBUG <<"MMReadoutElement layer " << layer << " sAngle " << sAngle << " phi direction MM eta strip " << (m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,1.,0.)).phi() << endmsg; 
#endif
    }
  }

  bool MMReadoutElement::containsId(Identifier id) const
  {
 
    if (manager()->mmIdHelper()->stationEta(id)!= getStationEta() ) return false;
    if (manager()->mmIdHelper()->stationPhi(id)!= getStationPhi() ) return false;

    if (manager()->mmIdHelper()->multilayerID(id)!= m_ml ) return false;
    
    int gasgap     = manager()->mmIdHelper()->gasGap(id);
    if (gasgap  <1 || gasgap > m_nlayers) return false;

    int strip      = manager()->mmIdHelper()->channel(id);
    if (strip <1 || strip > m_nStrips[gasgap-1] ) return false;

    return true;
  }

  Amg::Vector3D MMReadoutElement::localToGlobalCoords(Amg::Vector3D locPos, Identifier id) const
  {
    int gg = manager()->mmIdHelper()->gasGap(id);
    
    //    Amg::Vector3D  locP = (m_Xlg[gg-1].inverse())*locPos;
    Amg::Vector3D  locP = (m_Xlg[gg-1])*locPos;
#ifndef NDEBUG
    MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
    if (log.level()<=MSG::DEBUG) {
      log << MSG::DEBUG<<"locPos in the gg      r.f. "<<locPos<<endmsg;
      log << MSG::DEBUG<<"locP in the multilayer r.f. "<<locP<<endmsg;
    }
#endif
    Amg::Vector3D gVec = absTransform()*locP;
    return m_delta*gVec;
  }

  void MMReadoutElement::setDelta(double tras, double traz, double trat,
                                  double rots, double rotz, double rott)
  {
    m_rots = rots;
    m_rotz = rotz;
    m_rott = rott;

    HepGeom::Transform3D delta = HepGeom::Transform3D::Identity;
     if (std::abs(tras)+std::abs(traz)+std::abs(trat)+(std::abs(rots)+std::abs(rotz)+std::abs(rott))*1000. > 0.01)
    {
       // compute the delta transform 
       delta = HepGeom::TranslateX3D(tras)*HepGeom::TranslateY3D(traz)*
                     HepGeom::TranslateZ3D(trat)*HepGeom::RotateX3D(rots)*
                    HepGeom::RotateY3D(rotz)*HepGeom::RotateZ3D(rott);
       m_hasALines = true;
    }
    Amg::Transform3D deltaToAmg = Amg::CLHEPTransformToEigen(delta);
    m_delta = deltaToAmg;
  }
  
  void MMReadoutElement::setDelta(MuonDetectorManager* mgr)
  {
    const ALineMapContainer* alineMap = mgr->ALineContainer();
    Identifier id = mgr->mmIdHelper()->elementID(getStationName(), getStationEta(), getStationPhi());
    Identifier idMult = mgr->mmIdHelper()->multilayerID(id, m_ml);
    if( alineMap->find(idMult) == alineMap->cend())
    {
      MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
      if(log.level()<=MSG::DEBUG)
      {
        log << MSG::DEBUG << "m_aLineMapContainer does not contain any ALine for MM" << endmsg;
      }
    }
    else
    {
      ALinePar aline = alineMap->find(idMult)->second;
      float s, z, t, rots, rotz, rott;
      aline.getParameters(s, z, t, rots, rotz, rott);
      setDelta(s, z, t, rots, rotz, rott);
    }
  }

  void MMReadoutElement::setBLinePar(BLinePar* bLine)
  {
#ifndef NDEBUG
    MsgStream log(Athena::getMessageSvc(),"MMReadoutElement");
    if (log.level()<=MSG::DEBUG) log << MSG::DEBUG<<"Setting B-line for "<<getStationName().substr(0,3)<<" at eta/phi "<<getStationEta()<<"/"<<getStationPhi()<<endmsg;
#endif
    m_BLinePar = bLine;
  }

} // namespace MuonGM
