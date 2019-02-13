/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigFTKClusterConverter tool
// -------------------------------
// ATLAS Collaboration
//
// 13.06.2013 Package created
//
// Author: Dmitry Emeliyanov, RAL
// e-mail: D.Emeliyanov@rl.ac.uk
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenParticle.h"

#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h" 
#include "TrkTruthData/PRD_MultiTruthCollection.h"

#include "TrigFTKTrackConverter/TrigFTKTrackConverter.h"
#include "TrigFTKTrackConverter/TrigFTKClusterConverterTool.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h" 
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h" 
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h" 

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h" 

#include "TrkRIO_OnTrack/RIO_OnTrack.h" 
#include "TrkEventPrimitives/FitQuality.h" 
#include "TrkEventPrimitives/ParamDefs.h" 
#include "StoreGate/DataHandle.h" 
#include "InDetRIO_OnTrack/SiClusterOnTrack.h" 
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h" 
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h" 
#include "AthenaBaseComps/AthMsgStreamMacros.h"

using Gaudi::Units::micrometer;

TrigFTKClusterConverterTool::TrigFTKClusterConverterTool(const std::string& t, 
					   const std::string& n,
					   const IInterface*  p ): 
  AthAlgTool(t,n,p), 
  m_usePixelCalibSvc(true),
  m_trackFitter("Trk::DistributedKalmanFilter/InDetTrackFitter"),
  m_doFit(false),
  m_doTruth(false),
  m_ftkPixelTruthName("PRD_MultiTruthPixel_FTK"),
  m_ftkSctTruthName("PRD_MultiTruthSCT_FTK"),
  m_mcTruthName("TruthEvent") {

  declareInterface< ITrigFTKClusterConverterTool >( this );
  declareProperty( "UsePixelCalibSvc",m_usePixelCalibSvc); 
  declareProperty( "TrackFitter",m_trackFitter); 
  declareProperty( "DoFit",m_doFit);
  declareProperty( "DoTruth",m_doTruth);
  m_idHelper=NULL;

}

StatusCode TrigFTKClusterConverterTool::initialize() {

  StatusCode sc = AlgTool::initialize();

  if(m_usePixelCalibSvc) {
    ATH_CHECK(m_clusterErrorKey.initialize());
  }
  if(m_doTruth) {
    sc = service( "StoreGateSvc", m_evtStore ); 
    if (sc.isFailure()) { 
      ATH_MSG_FATAL("Unable to retrieve StoreGate service"); 
      return sc; 
    } 
  }
  StoreGateSvc* detStore;
  sc = service("DetectorStore", detStore);
  if ( sc.isFailure() ) { 
    ATH_MSG_FATAL("DetStore service not found"); 
    return StatusCode::FAILURE; 
  }

  // Get SCT & pixel Identifier helpers

  if (detStore->retrieve(m_pixelId, "PixelID").isFailure()) { 
     ATH_MSG_FATAL("Could not get Pixel ID helper");
     return StatusCode::FAILURE;  
  }

  if (detStore->retrieve(m_sctId, "SCT_ID").isFailure()) {  
     ATH_MSG_FATAL("Could not get SCT ID helper");
     return StatusCode::FAILURE;
  }

  sc = detStore->retrieve(m_pixelManager);  
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve Pixel DetectorManager from detStore."); 
    return sc;
  } 

  // ReadCondHandleKey
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

	//Get ID helper
	if (detStore->retrieve(m_idHelper, "AtlasID").isFailure()) {
		ATH_MSG_FATAL("Could not get AtlasDetectorID helper AtlasID");
		return StatusCode::FAILURE;
	}


  sc=m_trackFitter.retrieve(); 
  if(sc.isFailure()) { 
    ATH_MSG_ERROR("Could not retrieve "<<m_trackFitter); 
    return sc; 
  } 

  ATH_CHECK(m_pixelLorentzAngleTool.retrieve());
  ATH_CHECK(m_sctLorentzAngleTool.retrieve());

  ATH_MSG_INFO("TrigFTKClusterConverterTool initialized ");
  return sc;
}

StatusCode TrigFTKClusterConverterTool::finalize() {
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}


InDet::SCT_Cluster* TrigFTKClusterConverterTool::createSCT_Cluster(IdentifierHash hash, float hCoord, int w){
  
  const InDetDD::SiDetectorElement* pDE = getSCTDetectorElement(hash);
  float locPos = hCoord+0.1; // adding 0.1 to prevent rounding errors

  int strip = (int)(locPos);

  if(w==0) w=1;

  Identifier wafer_id = m_sctId->wafer_id(hash);
  Identifier strip_id = m_sctId->strip_id(wafer_id, strip);

  std::vector<Identifier> rdoList;
  rdoList.push_back(strip_id);

  int firstStrip = (int)(locPos+0.5-0.5*w);
  int lastStrip  = (int)(locPos+0.5+0.5*w) -1 ;

  const int nStrips(lastStrip - firstStrip + 1);

  const InDetDD::SiCellId cell1(firstStrip - 1);  
  const InDetDD::SiCellId cell2(lastStrip + 1);   

  const InDetDD::SiLocalPosition firstStripPos(pDE->rawLocalPositionOfCell(cell1)); 
  const InDetDD::SiLocalPosition lastStripPos(pDE->rawLocalPositionOfCell(cell2)); 

  const double width((double(nStrips)/double(nStrips+1))*( lastStripPos.xPhi()-firstStripPos.xPhi())); 
  const InDetDD::SiLocalPosition centre((firstStripPos+lastStripPos)/2.0); 

  Amg::Vector2D localPos(centre.xPhi(),  centre.xEta()); 

  const InDetDD::SCT_ModuleSideDesign* design; 
  if (m_sctId->is_barrel(wafer_id)){ 
    design = (static_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&pDE->design())); 
  } else{ 
    design = (static_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(&pDE->design())); 
  }  

  const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(centre)); 
  double stripLength(fabs(ends.first.xEta()-ends.second.xEta())); 


  InDet::SiWidth siWidth(Amg::Vector2D(w,1), Amg::Vector2D(width,stripLength) );

  double shift = m_sctLorentzAngleTool->getLorentzShift(hash);

  double derivedPos = localPos[Trk::locX]+shift; 
  double rawPos = (hCoord-0.5*767)*pDE->phiPitch();

  if(fabs(derivedPos-rawPos)>0.5*pDE->phiPitch()) {
    derivedPos = rawPos+shift;
  }

  Amg::Vector2D position(derivedPos, localPos[Trk::locY]);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
	cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()/12; 
  (*cov)(1,1) = siWidth.z()*siWidth.z()/12; 

  // rotation for endcap SCT
 
  if(pDE->design().shape() == InDetDD::Trapezoid) { 
    double sn      = pDE->sinStereoLocal(localPos);  
    double sn2     = sn*sn; 
    double cs2     = 1.-sn2; 
    double w       = pDE->phiPitch(localPos)/pDE->phiPitch();  
    double v0      = (*cov)(0,0)*w*w; 
    double v1      = (*cov)(1,1); 
    (*cov)(0,0) = (cs2*v0+sn2*v1); 
    (*cov)(1,0) = (sn*sqrt(cs2)*(v0-v1)); 
    (*cov)(0,1) = (sn*sqrt(cs2)*(v0-v1)); 
    (*cov)(1,1) = (sn2*v0+cs2*v1); 
  } 
  



  InDet::SCT_Cluster* pCL = new InDet::SCT_Cluster(strip_id, position, rdoList, siWidth, pDE, cov); 

  return pCL;
}


InDet::PixelCluster* TrigFTKClusterConverterTool::createPixelCluster(IdentifierHash hash, float locPosPhi, float locPosEta, float etaWidth, float phiWidth, float eta){
  
  //IdentifierHash hash = h.getIdentifierHash();
  Identifier wafer_id = m_pixelId->wafer_id(hash);


  const InDetDD::SiDetectorElement* pDE = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design 
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pDE->design()));
 
//  float locPosPhi = h.getCoord(0);
//  float locPosEta = h.getCoord(1);
//
//  int etaWidth = h.getEtaWidth(); 
//  int phiWidth = h.getPhiWidth(); 

  bool isIBL = (m_pixelId->barrel_ec(wafer_id)==0) && (m_pixelId->layer_disk(wafer_id)==0);


  double phiPos=0.0, etaPos=0.0;

  if (isIBL) {
    phiPos = (locPosPhi-0.5*336 + 0.5 - 0.14)*0.05; // + half a pixel  - 7um
    etaPos = (locPosEta-0.5*160)*0.25 * (162.+0.400/0.250) / 160;
  } else {
    if (m_pixelId->barrel_ec(wafer_id)==0) {
      phiPos = (locPosPhi-0.5*328+0.5)*0.05; // + half a pixel for barrel layers
    } else {
      phiPos = (locPosPhi-0.5*328)*0.05;
    }
    etaPos = (locPosEta-0.5*144)*0.4 * 152./144.;
  }

  locPosPhi = phiPos;
  locPosEta = etaPos;
  
  InDetDD::SiLocalPosition silPos(locPosEta, locPosPhi);// use the converted positions in mm !!!
  InDetDD::SiCellId cell =  design->cellIdOfPosition(silPos);  
  if(!cell.isValid()) {
    return NULL;
  }
  int phi_index = cell.phiIndex();
  int eta_index = cell.etaIndex();

  Identifier pixel_id = m_pixelId->pixel_id(wafer_id, phi_index, eta_index); 

  int colMin = (int)(eta_index-0.5*etaWidth);
  int colMax = colMin+etaWidth;

  int rowMin = (int)(phi_index-0.5*phiWidth);
  int rowMax = rowMin+phiWidth;

  double etaW = design->widthFromColumnRange(colMin, colMax-1); 
  double phiW = design->widthFromRowRange(rowMin, rowMax-1); 

  InDet::SiWidth siWidth(Amg::Vector2D(phiWidth,etaWidth),Amg::Vector2D(phiW,etaW)); 

  double shift = m_pixelLorentzAngleTool->getLorentzShift(hash);
  Amg::Vector2D position(phiPos+shift,etaPos);

  std::vector<Identifier> rdoList;
  rdoList.push_back(pixel_id);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
	cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()/12; 
  (*cov)(1,1) = siWidth.z()*siWidth.z()/12; 

  if(m_usePixelCalibSvc) {
    const Amg::Vector2D& colRow = siWidth.colRow();
    double averageZPitch = siWidth.z()/colRow.y();

    if(averageZPitch > 399*micrometer && averageZPitch < 401*micrometer){ 

      SG::ReadCondHandle<PixelCalib::PixelOfflineCalibData> offlineCalibData(m_clusterErrorKey);
      if(pDE->isBarrel()){ 
        int ibin = offlineCalibData->getPixelClusterErrorData()->getBarrelBin(eta,int(colRow.y()),int(colRow.x()));
 	      (*cov)(0,0) = pow(offlineCalibData->getPixelClusterErrorData()->getPixelBarrelPhiError(ibin),2);   
        (*cov)(1,1) = pow(offlineCalibData->getPixelClusterErrorData()->getPixelBarrelEtaError(ibin),2);
      } 
      else{ 
        int ibin = offlineCalibData->getPixelClusterErrorData()->getEndcapBin(int(colRow.y()),int(colRow.x()));
        (*cov)(0,0) = pow(offlineCalibData->getPixelClusterErrorData()->getPixelEndcapPhiError(ibin),2);
        (*cov)(1,1) = pow(offlineCalibData->getPixelClusterErrorData()->getPixelEndcapRError(ibin),2);
      } 
    } else { 
      (*cov)(0,0) = pow(siWidth.phiR()/colRow.x(),2)/12; 
      (*cov)(1,1) = pow(averageZPitch,2)/12; 
    }
  }
  InDet::PixelCluster* pCL = new InDet::PixelCluster(pixel_id, position, rdoList, siWidth, 
						     pDE, cov);
  
  return pCL;
}


void TrigFTKClusterConverterTool::createSCT_Truth(Identifier id, const MultiTruth& mt, PRD_MultiTruthCollection*  sctTruth, const McEventCollection*  mcEventCollection, StoreGateSvc* evtStore,  std::string mcTruthName) {

  StatusCode sc = evtStore->retrieve(mcEventCollection, mcTruthName ); 
  
  if ( sc.isFailure() ){ 
    ATH_MSG_WARNING("Cannot retrieve McEventCollection "<<mcTruthName); 
  } 
  int barCode = mt.best_barcode();
  unsigned eventIdx=0;

  McEventCollection::const_iterator event = mcEventCollection->begin();
  for ( ; event != mcEventCollection->end(); ++event, ++eventIdx ){
    const HepMC::GenParticle* p = (*event)->barcode_to_particle(barCode);
    if(p!=NULL) {
      sctTruth->insert(std::make_pair(id,HepMcParticleLink(p,eventIdx)));
      break;
    }
  }
  return;
}


void TrigFTKClusterConverterTool::createPixelTruth(Identifier id, const MultiTruth& mt, PRD_MultiTruthCollection*  pxlTruth, const McEventCollection*  mcEventCollection, StoreGateSvc* evtStore, std::string mcTruthName) {

  StatusCode sc = evtStore->retrieve(mcEventCollection, mcTruthName ); 
  
  if ( sc.isFailure() ){ 
    ATH_MSG_WARNING("Cannot retrieve McEventCollection "<<mcTruthName); 
  } 

  int barCode = mt.best_barcode();
  //if (barCode>100000) return;
  unsigned eventIdx=0;

  McEventCollection::const_iterator event = mcEventCollection->begin();
  for ( ; event != mcEventCollection->end(); ++event, ++eventIdx ){
    const HepMC::GenParticle* p = (*event)->barcode_to_particle(barCode);
    if(p!=NULL) {
      pxlTruth->insert(std::make_pair(id,HepMcParticleLink(p,eventIdx)));
      break;
    }
  }
  return;
}


InDet::PixelClusterCollection* TrigFTKClusterConverterTool::getCollection(InDet::PixelClusterContainer* cont, 
									  IdentifierHash hashId) {

  InDet::PixelClusterContainer::const_iterator it = cont->indexFind(hashId);
  if(it!=cont->end()) {
    InDet::PixelClusterCollection* pcoll = const_cast<InDet::PixelClusterCollection*>(&(**it));
    return pcoll;
  }
  Identifier id = m_pixelId->wafer_id(hashId);
  InDet::PixelClusterCollection* pcoll = new InDet::PixelClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = cont->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    delete pcoll;
    return NULL;
  }
  return pcoll;
}

InDet::SCT_ClusterCollection*  TrigFTKClusterConverterTool::getCollection(InDet::SCT_ClusterContainer* cont, 
								    IdentifierHash hashId) {

  InDet::SCT_ClusterContainer::const_iterator it = cont->indexFind(hashId);
  if(it!=cont->end()) {
    InDet::SCT_ClusterCollection* pcoll = const_cast<InDet::SCT_ClusterCollection*>(&(**it));
    return pcoll;
  }
  Identifier id = m_sctId->wafer_id(hashId);
  InDet::SCT_ClusterCollection* pcoll = new InDet::SCT_ClusterCollection(hashId);
  pcoll->setIdentifier(id);
  StatusCode sc = cont->addCollection(pcoll,hashId);
  if (sc.isFailure()){
    delete pcoll;
    return NULL;
  }
  return pcoll;
}

StatusCode TrigFTKClusterConverterTool::getMcTruthCollections(StoreGateSvc* evtStore,  const McEventCollection*  mcEventCollection, std::string ftkPixelTruthName, std::string ftkSctTruthName, std::string mcTruthName) {

  StatusCode sc = evtStore->retrieve(mcEventCollection, mcTruthName ); 

  if ( sc.isFailure() ){ 
    ATH_MSG_WARNING("Cannot retrieve McEventCollection "<<mcTruthName); 
    return StatusCode::FAILURE; 
  } 
  if(!evtStore->contains<PRD_MultiTruthCollection>(ftkPixelTruthName)) { 

     m_ftkPixelTruth = new PRD_MultiTruthCollection;
            
     sc=evtStore->record(m_ftkPixelTruth,ftkPixelTruthName,false); 
     if(sc.isFailure()) { 
       ATH_MSG_WARNING("Pixel FTK Truth Container " << ftkPixelTruthName  
           <<" cannot be recorded in StoreGate !"); 
       return sc; 
     } else { 
       ATH_MSG_DEBUG("Pixel FTK Truth Container " << ftkPixelTruthName   
         << " is recorded in StoreGate"); 
     } 
  } else {     
    ATH_MSG_DEBUG("Pixel FTK Truth Container " << ftkPixelTruthName     
        << " is found in StoreGate"); 
  }

  if(!evtStore->contains<PRD_MultiTruthCollection>(ftkSctTruthName)) { 

     m_ftkSctTruth = new PRD_MultiTruthCollection;
            
     sc=evtStore->record(m_ftkSctTruth,ftkSctTruthName,false); 
     if(sc.isFailure()) { 
       ATH_MSG_WARNING("SCT FTK Truth Container " << ftkSctTruthName  
		 <<" cannot be recorded in StoreGate !"); 
       return sc; 
     } else { 
	 ATH_MSG_DEBUG("SCT FTK Truth Container " << ftkSctTruthName   
		   << " is recorded in StoreGate"); 
     } 
  } else {     
      ATH_MSG_DEBUG("SCT FTK Truth Container " << ftkSctTruthName     
		<< " is found in StoreGate"); 
  }
  return sc;
}

const InDetDD::SiDetectorElement* TrigFTKClusterConverterTool::getSCTDetectorElement(const IdentifierHash hash) const {
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey};
  if (not condData.isValid()) return nullptr;
  return condData->getDetectorElement(hash);
}
