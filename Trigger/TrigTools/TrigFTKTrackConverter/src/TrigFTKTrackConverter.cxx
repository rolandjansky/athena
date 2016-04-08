/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// TrigFTKTrackConverter tool
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
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenParticle.h"

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

TrigFTKTrackConverter::TrigFTKTrackConverter(const std::string& t, 
					   const std::string& n,
					   const IInterface*  p ): 
  AthAlgTool(t,n,p), 
  m_offlineCalibSvc("PixelOfflineCalibSvc", n),
  m_usePixelCalibSvc(true),
  m_trackFitter("Trk::DistributedKalmanFilter/InDetTrackFitter"),
  m_clusterConverterTool("TrigFTKClusterConverterTool"),
  m_doFit(false),
  m_doTruth(false),
  m_ftkPixelTruthName("PRD_MultiTruthPixel_FTK"),
  m_ftkSctTruthName("PRD_MultiTruthSCT_FTK"),
  m_mcTruthName("TruthEvent") {

  declareInterface< ITrigFTKTrackConverter >( this );
  declareProperty( "PixelOfflineCalibSvc",m_offlineCalibSvc);
  declareProperty( "UsePixelCalibSvc",m_usePixelCalibSvc); 
  declareProperty( "TrackFitter",m_trackFitter); 
  declareProperty( "DoFit",m_doFit);
  declareProperty( "DoTruth",m_doTruth);
  m_idHelper=NULL;

}

StatusCode TrigFTKTrackConverter::initialize() {

  StatusCode sc = AlgTool::initialize();

  if(m_usePixelCalibSvc) {

    if ( !m_offlineCalibSvc.empty() ) { 
      StatusCode sc = m_offlineCalibSvc.retrieve(); 
      if (sc.isFailure() || !m_offlineCalibSvc ) { 
        ATH_MSG_ERROR(m_offlineCalibSvc.type() << " not found! ");  
	return sc; 
      } 
      else{ 
        ATH_MSG_INFO("Retrieved tool " <<  m_offlineCalibSvc.type());
      } 
    } 
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

  sc = detStore->retrieve(m_SCT_Manager);
  if( sc.isFailure() ) {
    ATH_MSG_ERROR("Could not retrieve SCT DetectorManager from detStore.");
    return sc;
  } 
	
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

  ATH_MSG_INFO("TrigFTKTrackConverter initialized ");
  return sc;
}

StatusCode TrigFTKTrackConverter::finalize() {
  StatusCode sc = AlgTool::finalize(); 
  return sc;
}

StatusCode TrigFTKTrackConverter::addSiHitInfo(const FTKTrack* p, TrigInDetTrack* track) {
  int nPixelHits=0, nSctHits=0;
  long pattern=0x0;

  unsigned int nHits = p->getNPlanes();
  for(unsigned int i=0;i<nHits;i++) {
    const FTKHit& h = p->getFTKHit(i);
    int dim = h.getDim();
    switch (dim) {
    case 0: {
	//missing hit - just ignore this for the time being
    }
      break;
    case 1: {
      nSctHits++;
      IdentifierHash hash = h.getIdentifierHash();
      Identifier wafer_id = m_sctId->wafer_id(hash);
      int layId = m_sctId->layer_disk(wafer_id);
      long layer=0;
      if (m_sctId->is_barrel(wafer_id)){ 
	layer = layId+4;
      } else { 
	layer = layId+11;
      }
      long mask = 1 << layer;
      pattern |= mask;
    }
      break; 
    case 2: {
      nPixelHits++;
      IdentifierHash hash = h.getIdentifierHash();
      Identifier wafer_id = m_pixelId->wafer_id(hash);
      int layId = m_pixelId->layer_disk(wafer_id);
      long layer=0;
      if (m_pixelId->is_barrel(wafer_id)){ 
	layer = layId;
      } else { 
	layer = layId+8;
      }
      long mask = 1 << layer;
      pattern |= mask;
    }
      break;
    default:
      ATH_MSG_WARNING("cannot determine detector type, dim=");
    }
  }
  track->HitPattern(pattern);
  track->NPixelSpacePoints(nPixelHits);
  track->NSCT_SpacePoints(nSctHits/2);

  return StatusCode::SUCCESS;
}

StatusCode TrigFTKTrackConverter::convert(const std::vector<FTKTrack*>& vFT, 
					  InDet::PixelClusterContainer* pixCont, 
					  InDet::SCT_ClusterContainer* sctCont, 
					  TrackCollection* tracks) {

  if(m_doTruth) {//get MC-truth collections
    m_collectionsReady=true;
    StatusCode sc = getMcTruthCollections();
    if(sc.isFailure()) {
      ATH_MSG_WARNING("PRD FTK Truth making failed ...");
      m_collectionsReady=false;
    }
  }
  int outputLevel = msgSvc()->outputLevel( name() );
  
  for(std::vector<FTKTrack*>::const_iterator it = vFT.begin();it!=vFT.end();++it) {
    const FTKTrack* p = (*it);


    DataVector<const Trk::TrackStateOnSurface>* pParVec = new DataVector<const Trk::TrackStateOnSurface>; 

    double trkTheta = atan2(1.0,p->getCotTheta());
    double trkQOverP = sin(trkTheta)/p->getPt();
		Trk::PerigeeSurface perigeeSurface;
    Trk::Perigee* trkPerigee = new Trk::Perigee( p->getIP(),
                                                 p->getZ0(),
                                                 p->getPhi(),
                                                 trkTheta,
                                                 trkQOverP, perigeeSurface);

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
    typePattern.set(Trk::TrackStateOnSurface::Perigee);
    const Trk::TrackStateOnSurface*trkTSoS= new Trk::TrackStateOnSurface(0,trkPerigee,0,0,typePattern);
    pParVec->push_back(trkTSoS);

    unsigned int nHits = p->getNPlanes();
    for(unsigned int i=0;i<nHits;i++) {
      const FTKHit& h = p->getFTKHit(i);
      int dim = h.getDim();
      switch (dim) {
      case 0: {
	//missing hit - just ignore this for the time being
      }
	break;
      case 1: {
	//create SCT cluster
	//std::cout<<"FTK SCT hit hash id = "<<h.getIdentifierHash()<<std::endl;
	InDet::SCT_Cluster* pSctCL = m_clusterConverterTool->createSCT_Cluster(h.getIdentifierHash(), h.getCoord(0), h.getNStrips() ); //createSCT_Cluster(h);
	if(pSctCL!=NULL) {
	  InDet::SCT_ClusterCollection* pColl = getCollection(sctCont, h.getIdentifierHash());
	  if(pColl!=NULL) {
	    pSctCL->setHashAndIndex(pColl->identifyHash(), pColl->size()); 
	    pColl->push_back(pSctCL); 
	    if(m_doTruth && m_collectionsReady) {
	      const MultiTruth& t = h.getTruth();
	      if(outputLevel<=MSG::DEBUG) {
          ATH_MSG_DEBUG("SCT cluster MC barcode = "<<t.best_barcode());
	      }
	      createSCT_Truth(pSctCL->identify(), t);
	    }
	  }

	  const IdentifierHash idHash = m_sctId->wafer_hash(m_sctId->wafer_id(pSctCL->identify())); 
		InDet::SCT_ClusterOnTrack* pSC=new InDet::SCT_ClusterOnTrack(pSctCL,
  	                                                             pSctCL->localPosition(),
  	                                                             pSctCL->localCovariance(),
  	                                                             idHash,
  	                                                             pSctCL->globalPosition()); 
	  
	  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
	  typePattern.set(Trk::TrackStateOnSurface::Measurement); 
	  
	  Trk::TrackStateOnSurface* trackStateOnSurface=new Trk::TrackStateOnSurface(pSC,  
										     0,  
										     0,   
										     0, typePattern); 
	  pParVec->push_back(trackStateOnSurface); 
	}

	
      }
	break; 
      case 2: {
	//create Pixel cluster
	//std::cout<<"FTK pixel hit hash id = "<<h.getIdentifierHash()<<std::endl;

	InDet::PixelCluster* pPixCL = m_clusterConverterTool->createPixelCluster(h.getIdentifierHash(), h.getCoord(0), h.getCoord(1), h.getEtaWidth(), h.getPhiWidth(),trkPerigee->eta());

	if(pPixCL!=NULL) {
	  InDet::PixelClusterCollection* pColl = getCollection(pixCont, h.getIdentifierHash());
	  if(pColl!=NULL) {
	    pPixCL->setHashAndIndex(pColl->identifyHash(), pColl->size()); 
	    pColl->push_back(pPixCL); 
	    if(m_doTruth && m_collectionsReady) {
	      const MultiTruth& t = h.getTruth();
        ATH_MSG_DEBUG("PIX cluster MC barcode = "<<t.best_barcode());
	        createPixelTruth(pPixCL->identify(), t);
	    }
	  }

	  const IdentifierHash idHash = m_pixelId->wafer_hash(m_pixelId->wafer_id(pPixCL->identify())); 
		InDet::PixelClusterOnTrack* pSC=new InDet::PixelClusterOnTrack(pPixCL,
  	                                                               pPixCL->localPosition(),
  	                                                               pPixCL->localCovariance(),
  	                                                               idHash,
  	                                                               pPixCL->globalPosition(), 
  	                                                               pPixCL->gangedPixel()); 
	  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
	  typePattern.set(Trk::TrackStateOnSurface::Measurement); 
	  Trk::TrackStateOnSurface* trackStateOnSurface=new Trk::TrackStateOnSurface(pSC,  
										     0,  
										     0,   
										     0, typePattern); 
	  pParVec->push_back(trackStateOnSurface); 
	}
      }
	break;
      default:
  ATH_MSG_WARNING("cannot determine detector type, dim=");
      }
    }
    Trk::TrackInfo trkTrackInfo(Trk::TrackInfo::Unknown, Trk::pion);
    const Trk::FitQuality* pFQ = new Trk::FitQuality(p->getChi2(), p->getChi2()/p->getChi2ndof());
    Trk::Track* pT = new Trk::Track(trkTrackInfo,pParVec,pFQ); 

    if(!m_doFit) {
      tracks->push_back(pT); 
      ATH_MSG_DEBUG("++++++++ new track created : +++++++");
      ATH_MSG_DEBUG((*pT));
    }
    else {
      Trk::Track* newtrack = m_trackFitter->fit(*pT,false, Trk::pion); 
      if(newtrack!=NULL) {
        if(outputLevel<=MSG::DEBUG) {
          ATH_MSG_DEBUG("++++++++ track after refit : +++++++");
          ATH_MSG_DEBUG(*newtrack);
          const Trk::Perigee* P = newtrack->perigeeParameters();
          ATH_MSG_DEBUG(*P);
        }
        tracks->push_back(newtrack);
        delete pT;
      }
      else {
        ATH_MSG_DEBUG("track refit failed, keeping the old track ...");
        tracks->push_back(pT); 
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigFTKTrackConverter::convert(const TrackCollection* offlineTracks, TrigInDetTrackCollection* trigInDetTracks) {

  for (TrackCollection::const_iterator offlineTrack = offlineTracks->begin(); offlineTrack != offlineTracks->end(); ++offlineTrack) {

    const Trk::TrackParameters* trackPars = (*offlineTrack)->perigeeParameters();
    if(trackPars==nullptr) {
      continue;
    }

    if(trackPars->covariance()==nullptr) {
      continue;
		}
    
    float d0 = trackPars->parameters()[Trk::d0]; 
    float z0 = trackPars->parameters()[Trk::z0]; 
    float phi0 = trackPars->parameters()[Trk::phi0]; 
    float theta = trackPars->parameters()[Trk::theta]; 
    if(phi0>M_PI) phi0-=2*M_PI; 
    if(phi0<-M_PI) phi0+=2*M_PI;
    float eta = -log(tan(0.5*theta)); 
    
    float qOverP = trackPars->parameters()[Trk::qOverP]; 
    float pT=sin(theta)/qOverP;

    //calculate chi2 and ndofs

    double chi2=0.0;
    int ndof=-5;

    int nPix=0, nSct=0;

    for(DataVector<const Trk::TrackStateOnSurface>::const_iterator tSOS = (*offlineTrack)->trackStateOnSurfaces()->begin(); 
				tSOS!=(*offlineTrack)->trackStateOnSurfaces()->end(); ++tSOS) {
      const Trk::FitQualityOnSurface* fq =  (*tSOS)->fitQualityOnSurface();
      if(!fq) continue;
      double x2 = fq->chiSquared();
      chi2 += x2;
      int nd = fq->numberDoF();
      ndof += nd;
      if(nd==2) nPix++;
      if(nd==1) nSct++;
    }
		
		long hitPattern=0x0;
		for (DataVector<const Trk::MeasurementBase>::const_iterator tMOT = (*offlineTrack)->measurementsOnTrack()->begin();
				 tMOT != (*offlineTrack)->measurementsOnTrack()->end(); ++tMOT) {
			Identifier id = (*tMOT)->associatedSurface().associatedDetectorElement()->identify();
			IdentifierHash hash = (*tMOT)->associatedSurface().associatedDetectorElement()->identifyHash();
			if(m_idHelper->is_sct(id)) {
				Identifier wafer_id = m_sctId->wafer_id(hash);
				int layId = m_sctId->layer_disk(wafer_id);
				long layer=0;
				if (m_sctId->is_barrel(wafer_id)){ 
					layer = layId+3;
				} else { 
					layer = layId+10;
				}
				long mask = 1 << layer;
				hitPattern |= mask;
			}
			else if(m_idHelper->is_pixel(id)) {
				Identifier wafer_id = m_pixelId->wafer_id(hash);
				int layId = m_pixelId->layer_disk(wafer_id);
				long layer=0;
				if (m_pixelId->is_barrel(wafer_id)){ 
					layer = layId;
				} else { 
					layer = layId+7;
				}
				long mask = 1 << layer;
				hitPattern |= mask;
			}
			else {
        ATH_MSG_WARNING("cannot determine detector type, hash="<<hash);
			}
		}

		//Calculate	covariance matrix in TID track parameter convention
		const AmgSymMatrix(5) cov_off = *(trackPars->covariance());
		float A = -0.5*((1.0+tan(0.5*theta)*tan(0.5*theta))/(tan(0.5*theta))); //deta_by_dtheta
		float B = cos(theta)/qOverP; //dpT_by_dtheta
		float C = -sin(theta)/(qOverP*qOverP); //dpT_by_dqOverP

		//Convenient std::vector{} brace initialisation - C++11 only
		std::vector<double>* cov = new std::vector<double>
    {cov_off(0,0), cov_off(2,0), cov_off(1,0), A*cov_off(3,0), B*cov_off(3,0) + C*cov_off(4,0),
                    cov_off(2,2), cov_off(2,1), A*cov_off(3,2), B*cov_off(3,2) + C*cov_off(4,2),
                                   cov_off(1,1), A*cov_off(3,1), B*cov_off(3,1) + C*cov_off(4,1),
                                                A*A*cov_off(3,3), A*(B*cov_off(3,3) + C*cov_off(4,3)),
		                                                               B*(B*cov_off(3,3) + 2*C*cov_off(4,3)) + C*C*cov_off(4,4)};

    float ed0   = sqrt(cov->at(0));
		float	ephi0 = sqrt(cov->at(5));
		float	ez0   = sqrt(cov->at(9));
		float	eeta  = sqrt(cov->at(12));
		float	epT   = sqrt(cov->at(14));
    
    const TrigInDetTrackFitPar* tidtfp = new TrigInDetTrackFitPar(d0, phi0, z0, eta, pT, ed0, ephi0, ez0, eeta, epT,cov); 
    std::vector<const TrigSiSpacePoint*>* pvsp = new std::vector<const TrigSiSpacePoint*>;
    TrigInDetTrack* pTrack = new TrigInDetTrack(pvsp,tidtfp);

    if(ndof>0) {
      chi2/=ndof;
    }
    else chi2 = 1e8;
    pTrack->chi2(chi2);
    pTrack->NPixelSpacePoints(nPix); 
    pTrack->NSCT_SpacePoints(nSct/2); 
    pTrack->HitPattern(hitPattern); 

    pTrack->algorithmId(TrigInDetTrack::STRATEGY_FR_ID);
    trigInDetTracks->push_back(pTrack);
  }
  return StatusCode::SUCCESS;
}

InDet::PixelClusterCollection* TrigFTKTrackConverter::getCollection(InDet::PixelClusterContainer* cont, 
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

InDet::SCT_ClusterCollection*  TrigFTKTrackConverter::getCollection(InDet::SCT_ClusterContainer* cont, 
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

StatusCode TrigFTKTrackConverter::getMcTruthCollections() {
  

  StatusCode sc = m_evtStore->retrieve(m_mcEventCollection, m_mcTruthName ); 

  if ( sc.isFailure() ){ 
    ATH_MSG_WARNING("Cannot retrieve McEventCollection "<<m_mcTruthName); 
    return StatusCode::FAILURE; 
  } 

  if(!m_evtStore->contains<PRD_MultiTruthCollection>(m_ftkPixelTruthName)) { 

     m_ftkPixelTruth = new PRD_MultiTruthCollection;
            
     sc=m_evtStore->record(m_ftkPixelTruth,m_ftkPixelTruthName,false); 
     if(sc.isFailure()) { 
       ATH_MSG_WARNING("Pixel FTK Truth Container " << m_ftkPixelTruthName  
           <<" cannot be recorded in StoreGate !"); 
       return sc; 
     } else { 
       ATH_MSG_DEBUG("Pixel FTK Truth Container " << m_ftkPixelTruthName   
         << " is recorded in StoreGate"); 
     } 
  } else {     
    ATH_MSG_DEBUG("Pixel FTK Truth Container " << m_ftkPixelTruthName     
        << " is found in StoreGate"); 
  }

  if(!m_evtStore->contains<PRD_MultiTruthCollection>(m_ftkSctTruthName)) { 

     m_ftkSctTruth = new PRD_MultiTruthCollection;
            
     sc=m_evtStore->record(m_ftkSctTruth,m_ftkSctTruthName,false); 
     if(sc.isFailure()) { 
       ATH_MSG_WARNING("SCT FTK Truth Container " << m_ftkSctTruthName  
		 <<" cannot be recorded in StoreGate !"); 
       return sc; 
     } else { 
	 ATH_MSG_DEBUG("SCT FTK Truth Container " << m_ftkSctTruthName   
		   << " is recorded in StoreGate"); 
     } 
  } else {     
      ATH_MSG_DEBUG("SCT FTK Truth Container " << m_ftkSctTruthName     
		<< " is found in StoreGate"); 
  }
  return sc;
}

void TrigFTKTrackConverter::createSCT_Truth(Identifier id, const MultiTruth& mt) {

  int barCode = mt.best_barcode();
  //  if(barCode>1000000) return;
  unsigned eventIdx=0;
  McEventCollection::const_iterator event = m_mcEventCollection->begin();    
  for ( ; event != m_mcEventCollection->end(); ++event, ++eventIdx ){ 
    const HepMC::GenParticle* p = (*event)->barcode_to_particle(barCode);
    if(p!=NULL) {
      m_ftkSctTruth->insert(std::make_pair(id,HepMcParticleLink(p,eventIdx)));
      break;
    }
  }
}

void TrigFTKTrackConverter::createPixelTruth(Identifier id, const MultiTruth& mt) {
  int barCode = mt.best_barcode();
  //  if(barCode>1000000) return;
  unsigned eventIdx=0;
  McEventCollection::const_iterator event = m_mcEventCollection->begin();    
  for ( ; event != m_mcEventCollection->end(); ++event, ++eventIdx ){ 
    const HepMC::GenParticle* p = (*event)->barcode_to_particle(barCode);
    if(p!=NULL) {
      m_ftkPixelTruth->insert(std::make_pair(id,HepMcParticleLink(p,eventIdx)));
      break;
    }
  }
}
