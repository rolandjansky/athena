/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 The MM detector = an assembly module = STGC in amdb 
 ----------------------------------------------------
 Copyright (C) 2004 by ATLAS Collaboration
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

#define MMReadout_verbose false

namespace MuonGM {


  MMReadoutElement::MMReadoutElement(GeoVFullPhysVol* pv, std::string stName,
				     int zi, int fi, int mL, bool is_mirrored,
				     MuonDetectorManager* mgr)
    : MuonClusterReadoutElement(pv, stName, zi, fi, is_mirrored, mgr)
  {
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
	      std::cerr<<"number of MM layers > 4: increase transform array size"<< std::endl;
              continue;
	    }
	    //std::cerr<<" navigating MM named "
	    //       <<childname<<" child "
	    //       <<ich<<" is a layer with tag "<<llay<<std::endl;
	    m_Xlg[llay-1] = Amg::CLHEPTransformToEigen(pvc->getXToChildVol(ich));
            // save layer dimensions
            if (llay==1) {
              if (pc->getLogVol()->getShape()->type()=="Trd") {
		const GeoTrd* trd = dynamic_cast<const GeoTrd*> (pc->getLogVol()->getShape());
		//std::cerr<<"MM layer shape dimensions:minX,maxX,minY,maxY,halfZ:"<< trd->getXHalfLength1()<<"," << trd->getXHalfLength2()
		//	 <<"," << trd->getYHalfLength1() <<"," << trd->getYHalfLength2() <<"," << trd->getZHalfLength() << std::endl;
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
		  std::cerr<<"MM layer base shape not trapezoid? " << sub->type()<< std::endl;
		} else {
		  //std::cerr<<"MM layer shape dimensions:minX,maxX,minY,maxY,halfZ:"<< trd->getXHalfLength1()<<"," << trd->getXHalfLength2()
		  //	 <<"," << trd->getYHalfLength1() <<"," << trd->getYHalfLength2() <<"," << trd->getZHalfLength() << std::endl;
		  m_halfX =  trd->getZHalfLength();
		  m_minHalfY = trd->getYHalfLength1();
		  m_maxHalfY = trd->getYHalfLength2();
		  //m_minHalfY = (sName[2]=='L') ? 0.5*MM_A_ACTIVE[1][abs(zi)-1] : 0.5*MM_A_ACTIVE[0][abs(zi)-1];
		  //m_maxHalfY = (sName[2]=='L') ? 0.5*MM_B_ACTIVE[1][abs(zi)-1] : 0.5*MM_B_ACTIVE[0][abs(zi)-1];       
		  foundShape = true;
                }
              } else {
	        std::cerr<<"MM layer shape not recognized:" <<pc->getLogVol()->getShape()->type()<< std::endl;
	      }
	    }
	  }
	}
        m_nlayers=llay;
      } else {
	std::cerr<<"Cannot perform a dynamic cast ! "<<std::endl;
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
    
    MMDetectorHelper aHelper;

    for (int il=0; il<m_nlayers; il++) {

      // identifier of the first channel to retrieve max number of strips
      Identifier id = manager()->mmIdHelper()->channelID(getStationName(),getStationEta(),getStationPhi(),m_ml, il+1, 1);
      int chMax =  manager()->mmIdHelper()->channelMax(id);
      if ( chMax < 0 ) chMax = 2500;
      
      char side = getStationEta() < 0 ? 'C' : 'A';
      char sector_l = getStationName().substr(2,1)=="L" ? 'L' : 'S';
      MMDetectorHelper aHelper;
      MMDetectorDescription* mm = aHelper.Get_MMDetector(sector_l, abs(getStationEta()), getStationPhi(), m_ml, side);
      MMReadoutParameters roParam = mm->GetReadoutParameters();

      m_etaDesign[il].type=0;

      m_etaDesign[il].xSize=2*m_halfX;
      m_etaDesign[il].minYSize=2*m_minHalfY;
      m_etaDesign[il].maxYSize=2*m_maxHalfY;

      m_etaDesign[il].deadO = 0.;
      m_etaDesign[il].deadI = 0.;
      m_etaDesign[il].deadS = 0.;

      double pitch =  getStationName().substr(2,1)=="L" ? roParam.stripPitch : roParam.stripPitch; 
      m_etaDesign[il].inputPitch = pitch;
      m_etaDesign[il].inputLength = m_etaDesign[il].minYSize;
      m_etaDesign[il].inputWidth = pitch;
      m_etaDesign[il].thickness = roParam.gasThickness;

      if (m_ml == 1) m_etaDesign[il].sAngle = (roParam.stereoAngel).at(il);
      else if (m_ml == 2) m_etaDesign[il].sAngle = (roParam.stereoAngel).at(il);
      else reLog()<<MSG::WARNING
	          <<"MMReadoutElement -- Unexpected Multilayer: m_ml= " << m_ml <<endmsg;
      
      if (m_etaDesign[il].sAngle == 0.) {    // stereo angle 0.
	
	m_etaDesign[il].firstPos = -0.5*m_etaDesign[il].xSize + 0.5*pitch;
	m_etaDesign[il].signY  = 1 ;
	
	m_etaDesign[il].nch = (int) (m_etaDesign[il].xSize/pitch) + 1;

	if (m_etaDesign[il].nch > chMax) {    // fix with help of dead zone

	  double dead = 0.5*(m_etaDesign[il].xSize - chMax*pitch);
	  m_etaDesign[il].deadO = dead;
	  m_etaDesign[il].deadI = dead;
	  m_etaDesign[il].firstPos += dead;
	  m_etaDesign[il].nch = chMax;      
	}
	
      } else {
	
	m_etaDesign[il].signY  = il==2? 1 : -1 ;
        // firstPos in the position of the centre of the 1st strip on locX axis
	m_etaDesign[il].firstPos = -0.5*m_etaDesign[il].xSize         // lower module boundary 
	  +0.5*pitch/cos(m_etaDesign[il].sAngle)   // half of the strip size in x projection
	  -0.5*m_etaDesign[il].minYSize*tan(fabs(m_etaDesign[il].sAngle));   // offset needed to fill the corner  
        // the position of the centre of the last strip on locX axis (in order to cover the module)
        double lastPos =  0.5*m_etaDesign[il].xSize         // upper module boundary 
	  -0.5*pitch/cos(m_etaDesign[il].sAngle)   // half of the strip size in x projection
	  +0.5*m_etaDesign[il].maxYSize*tan(fabs(m_etaDesign[il].sAngle));   // offset needed to fill the corner  
        // number of channels needed to cover the module
        m_etaDesign[il].nch = int((lastPos -m_etaDesign[il].firstPos)/(pitch*cos(m_etaDesign[il].sAngle)))+1 ;

	if (m_etaDesign[il].nch > chMax) {    // dead zone does not help here - just limit number of channels

	  std::cerr<<"number of strips exceeds the maximum, adjusted:"<<m_etaDesign[il].nch<<"->"<<chMax << std::endl;
	  m_etaDesign[il].nch = chMax;      

	}

      }       
      
      m_nStrips.push_back(m_etaDesign[il].nch);

      reLog()<<MSG::INFO 
	     <<"initDesign:" << getStationName()<< " layer " << il << ", strip pitch " << m_etaDesign[il].inputPitch << ", nstrips " << m_etaDesign[il].nch << endmsg;

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

      // move the readout plane to the sensitive volume boundary 
      double shift = 0.5*m_etaDesign[layer].thickness;
      double rox = ( layer==0 || layer==2) ? shift : -shift;

      // need to operate switch x<->z because of GeoTrd definition
      m_surfaceData->m_layerSurfaces.push_back( new Trk::PlaneSurface(*this, id) );
      m_surfaceData->m_layerTransforms.push_back(absTransform()*m_Xlg[layer]*Amg::Translation3D(rox,0.,0.)*
						 Amg::AngleAxis3D(-90.*CLHEP::deg,Amg::Vector3D(0.,1.,0.)) );

      // surface info (center, normal) 
      m_surfaceData->m_layerCenters.push_back(m_surfaceData->m_layerTransforms.back().translation());
      m_surfaceData->m_layerNormals.push_back(m_surfaceData->m_layerTransforms.back().linear()*Amg::Vector3D(0.,0.,-1.));

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
    std::cout<<"locPos in the gg      r.f. "<<locPos<<std::endl;
    std::cout<<"locP in the multilayer r.f. "<<locP<<std::endl;
    
    return absTransform()*locP;
  }

} // namespace MuonGM
