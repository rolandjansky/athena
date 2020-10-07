/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The MM detector = an assembly module = STGC in amdb 
 ----------------------------------------------------
***************************************************************************/


//<doc><file>	$Id: MMReadoutElement.cxx,v 1.5 2009-05-13 15:03:47 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
//#include "StoreGate/DataHandle.h"
#include "MuonAGDDDescription/MMDetectorDescription.h"
#include "MuonAGDDDescription/MMDetectorHelper.h"
#include "MuonAlignmentData/BLinePar.h"

#define MMReadout_verbose false

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
    m_offset = 0.;

    m_hasALines = false;
    m_hasBLines = false;
    m_delta = NULL;
    m_ml = mL;
    m_MsgStream = new MsgStream(mgr->msgSvc(),"MuGM:MMReadoutElement");
    //MsgStream log(Athena::getMessageSvc(), "MuGM:MMReadoutElement");

    // get the setting of the caching flag from the manager
    setCachingFlag(mgr->cachingFlag());

    std::string vName = pv->getLogVol()->getName();
    std::string sName = vName.substr(vName.find("-")+1);
    std::string fixName = (sName[2]=='L') ? "MML" : "MMS";



    //setStationName(stName);
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
	                          reLog()<<MSG::ERROR<<"number of MM layers > 4: increase transform array size"<< endmsg;
              continue;
	    }
	   
	    m_Xlg[llay-1] = Amg::CLHEPTransformToEigen(pvc->getXToChildVol(ich));
            // save layer dimensions
            if (llay==1) {
              if (pc->getLogVol()->getShape()->type()=="Trd") {
		const GeoTrd* trd = dynamic_cast<const GeoTrd*> (pc->getLogVol()->getShape());
                m_halfX =  trd->getZHalfLength();
                // adjust phi dimensions according to the active area  
                m_minHalfY = trd->getYHalfLength1();
                m_maxHalfY = trd->getYHalfLength2();
                //m_minHalfY = (sName[2]=='L') ? 0.5*MM_A_ACTIVE[1][abs(zi)-1] : 0.5*MM_A_ACTIVE[0][abs(zi)-1];
                //m_maxHalfY = (sName[2]=='L') ? 0.5*MM_B_ACTIVE[1][abs(zi)-1] : 0.5*MM_B_ACTIVE[0][abs(zi)-1];       
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
		                        reLog()<<MSG::ERROR<<"MM layer base shape not trapezoid? " << sub->type()<<endmsg;
		} else {
		  m_halfX =  trd->getZHalfLength();
		  m_minHalfY = trd->getYHalfLength1();
		  m_maxHalfY = trd->getYHalfLength2();
		  //m_minHalfY = (sName[2]=='L') ? 0.5*MM_A_ACTIVE[1][abs(zi)-1] : 0.5*MM_A_ACTIVE[0][abs(zi)-1];
		  //m_maxHalfY = (sName[2]=='L') ? 0.5*MM_B_ACTIVE[1][abs(zi)-1] : 0.5*MM_B_ACTIVE[0][abs(zi)-1];       
		  foundShape = true;
                }
              } else {
	                     reLog()<<MSG::ERROR<<"MM layer shape not recognized:" <<pc->getLogVol()->getShape()->type()<<endmsg;
	      }
	    }
	  }
	}
        m_nlayers=llay;
      } else {
	             reLog()<<MSG::ERROR<<"Cannot perform a dynamic cast ! "<<endmsg;
      }
    }
    if( !foundShape ){
      *m_MsgStream << MSG::WARNING << " failed to initialize dimensions of this chamber " << endmsg;
    }
    //fillCache();

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
    int gethash_code = manager()->mmIdHelper()->get_module_hash(id, collIdhash);
    if (gethash_code != 0) 
      reLog()<<MSG::WARNING
     	     <<"MMReadoutElement --  collection hash Id NOT computed for id = "
     	     <<manager()->mmIdHelper()->show_to_string(id)<<std::endl;
    m_idhash = collIdhash;
    // // set RE hash id 
    gethash_code = manager()->mmIdHelper()->get_detectorElement_hash(id, detIdhash);
    if (gethash_code != 0) 
       reLog()<<MSG::WARNING
	      <<"MMReadoutElement --  detectorElement hash Id NOT computed for id = "
	      <<manager()->mmIdHelper()->show_to_string(id)<<endmsg;
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
        reLog()<<MSG::WARNING<<"MMReadoutElement -- Max number of strips not a real value"<<endmsg;
      }
      char side = getStationEta() < 0 ? 'C' : 'A';
      char sector_l = getStationName().substr(2,1)=="L" ? 'L' : 'S';
      MMDetectorHelper aHelper;
      MMDetectorDescription* mm = aHelper.Get_MMDetector(sector_l, abs(getStationEta()), getStationPhi(), m_ml, side);
      MMReadoutParameters roParam = mm->GetReadoutParameters();

      double ylFrame = mm->ylFrame();
      double ysFrame = mm->ysFrame();
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
      m_etaDesign[il].ylFrame = ylFrame;
      m_etaDesign[il].ysFrame = ysFrame;
      m_offset = -0.5*(m_etaDesign[il].ylFrame - m_etaDesign[il].ysFrame);
      if (m_ml < 1 || m_ml > 2)
        reLog()<<MSG::WARNING <<"MMReadoutElement -- Unexpected Multilayer: m_ml= " << m_ml <<endmsg;
      
      if (m_etaDesign[il].sAngle == 0.) {    // eta layers

        m_etaDesign[il].firstPos = -0.5*m_etaDesign[il].xSize + pitch;
        m_etaDesign[il].signY  = 1 ;
        m_etaDesign[il].nch = ((int) std::round( (m_etaDesign[il].xSize/pitch))); // Total number of active strips
	
      } else { // stereo layers
          
        m_etaDesign[il].signY  = il==2? 1 : -1 ;
        
        // define the distance from the frame till the point that the first and last active stereo strips cross the center of the chamber (low_swift & up_swift)
        // in order to derive the total number of active strips for the stereo layer

        double low_swift=( m_minHalfY -m_etaDesign[il].dlStereoBottom)*fabs(tan(m_etaDesign[il].sAngle));
        double up_swift = (m_maxHalfY - m_etaDesign[il].dlStereoTop)*fabs(tan(m_etaDesign[il].sAngle));

        double lm1_swift =0;
        if(sector_l=='L' && (abs(getStationEta()))==1){
          lm1_swift = (m_etaDesign[il].minYPhiR - m_etaDesign[il].minYPhiL)/2 + m_etaDesign[il].minYPhiL;
          low_swift = 0;
        }

        double fPos = -0.5*m_etaDesign[il].xSize - low_swift + lm1_swift;
        double lPos = 0.5*m_etaDesign[il].xSize + up_swift;

        m_etaDesign[il].nch = ((int)std::round( (lPos - fPos)/pitch ));

        m_etaDesign[il].firstPos = ( -0.5*m_etaDesign[il].xSize + (m_etaDesign[il].nMissedBottomStereo - m_etaDesign[il].nMissedBottomEta)*pitch) + pitch;

      }
      m_nStrips.push_back(m_etaDesign[il].totalStrips);

      reLog()<<MSG::DEBUG
	     <<"initDesign:" << getStationName()<< " layer " << il << ", strip pitch " << m_etaDesign[il].inputPitch << ", nstrips " << m_etaDesign[il].nch << " stereo " <<  m_etaDesign[il].sAngle << endmsg;

    }

  }
    
    void MMReadoutElement::fillCache() const
  {
    if( !m_surfaceData ) m_surfaceData = new SurfaceData();
    else{
      reLog()<<MSG::WARNING<<"calling fillCache on an already filled cache" << endmsg;
      return;
    }

    m_surfaceData->m_surfBounds.push_back( new Trk::RotatedTrapezoidBounds( m_halfX, m_minHalfY, m_maxHalfY));

    for( int layer = 0; layer < m_nlayers; ++layer ){
      // identifier of the first channel
      Identifier id = manager()->mmIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, layer+1, 1);

      /// keep the tracking surface at the center of the gap
      double sAngle = m_etaDesign[layer].sAngle;
      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*m_Xlg[layer]*
             Amg::Translation3D(0.,0.,m_offset)*
						 Amg::AngleAxis3D(-90.*CLHEP::deg,Amg::Vector3D(0.,1.,0.)) );

      // surface info (center, normal) 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));
// get phi direction of MM eta strip 
      *m_MsgStream << MSG::DEBUG << " MMReadoutElement layer " << layer << " sAngle " << sAngle << " phi direction MM eta strip " << (m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,1.,0.)).phi() << endmsg; 
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
    Amg::Vector3D  locP = (m_Xlg[gg-1])*Amg::Translation3D(0.,0.,m_offset)*locPos;
    reLog()<<MSG::DEBUG<<"locPos in the gg      r.f. "<<locPos<<endmsg;
    reLog()<<MSG::DEBUG<<"locP in the multilayer r.f. "<<locP<<endmsg;

    return absTransform()*locP;
  }

  void MMReadoutElement::setDelta(double tras, double traz, double trat,
                                  double rots, double rotz, double rott)
  {
    m_rots = rots;
    m_rotz = rotz;
    m_rott = rott;

    HepGeom::Transform3D delta = HepGeom::Transform3D::Identity;
     if (fabs(tras)+fabs(traz)+fabs(trat)+(fabs(rots)+fabs(rotz)+fabs(rott))*1000. > 0.01)
    {
       // compute the delta transform 
       delta = HepGeom::TranslateX3D(tras)*HepGeom::TranslateY3D(traz)*
                     HepGeom::TranslateZ3D(trat)*HepGeom::RotateX3D(rots)*
                    HepGeom::RotateY3D(rotz)*HepGeom::RotateZ3D(rott);
       m_hasALines = true;
    }
  }

  void MMReadoutElement::setBLinePar(BLinePar* bLine) const
  {
    if(reLog().level() <= MSG::DEBUG) 
      reLog()<<MSG::DEBUG<<"Setting B-line for "<<getStationName().substr(0,3)<<" at eta/phi "<<getStationEta()<<"/"<<getStationPhi()<<endmsg;
    
    m_BLinePar = bLine;
  }

} // namespace MuonGM
