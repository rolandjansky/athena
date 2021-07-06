/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_LocReco/AFP_SIDLocReco.h"


AFP_SIDLocReco::AFP_SIDLocReco(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::AFP_SIDLocReco");
}

AFP_SIDLocReco::~AFP_SIDLocReco()
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::~AFP_SIDLocReco");
}

StatusCode AFP_SIDLocReco::initialize()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::initialize()");
  CHECK( AthReentrantAlgorithm::initialize() );

  // check mon tool
  if (!(m_monTool.name().empty())) {
    CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

  // initialize keys
  CHECK( m_eventInfoKey.initialize() );
  CHECK( m_AFPSiHitContainerKey.initialize() );
  CHECK( m_AFPTrackContainerKey.initialize() );

  // get geometry tool
  CHECK( m_pGeometry.retrieve(EnableTool{true}) );
  m_Config.clear();
  m_pGeometry->SetCfgParams(&m_Config);

  // read geometry
  CHECK( ClearGeometry() );
  CHECK( ReadGeometryDetCS() );

  ATH_MSG_DEBUG("end AFP_SIDLocReco::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::execute()");

  // get event context from as an argument, AthReentrantAlgorithm
  // const EventContext& ctx = Gaudi::Hive::currentContext();

  // get event info
  SG::ReadHandle<xAOD::EventInfo> eventInfo (m_eventInfoKey, ctx);
  if (!eventInfo.isValid())
  {
      ATH_MSG_WARNING("AFP_SIDLocReco, Cannot get eventInfo.");
      return StatusCode::SUCCESS;
  }
  else ATH_MSG_DEBUG("AFP_SIDLocReco::execute(), successfully got eventInfo");
  
  // get SID hits container
  SG::ReadHandle<xAOD::AFPSiHitContainer> siHitContainer( m_AFPSiHitContainerKey, ctx );
  if(!siHitContainer.isValid())
  {
      ATH_MSG_WARNING("AFP_SIDLocReco, cannot get siHitContainer");
      return StatusCode::SUCCESS;
  }
  else ATH_MSG_DEBUG("AFP_SIDLocReco::execute(), successfully got siHitContainer, there are "<<siHitContainer->size()<<" hits");
	
  // get list of hits SID hits from the container
  std::list<SIDHIT> ListSIDHits = AFPCollectionReading(siHitContainer, eventInfo);
  // n.b. even empty list is needed (ListSIDHits.size()==0), as we need to write "nothing" in the container for this event
  
  auto hitsSize = Monitored::Scalar( "HitsSize"   , -1.0 );
  hitsSize = ListSIDHits.size();
  ATH_MSG_DEBUG("AFP_SIDLocReco:: Hits size "<<hitsSize);

  auto monitorIt    = Monitored::Group( m_monTool, hitsSize);

  std::string strAlgoSID;
  for(unsigned int i=0; i<m_vecListAlgoSID.size(); i++)
  {
    strAlgoSID = m_vecListAlgoSID[i];

    //execute SID algorithm, write down the tracks
    StatusCode sc = ExecuteRecoMethod(strAlgoSID, ListSIDHits, siHitContainer, ctx);
			
    if (sc.isFailure())
    {
      ATH_MSG_WARNING("SID Algorithm " << strAlgoSID << " failure!");
      return StatusCode::SUCCESS;
    }
  }

  Monitored::fill(m_monTool,hitsSize);

  ATH_MSG_DEBUG("end AFP_SIDLocReco::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::finalize()
{
  ATH_MSG_DEBUG("in AFP_SIDLocReco::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::ClearGeometry()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::ClearGeometry()");

  ///////
  memset(&m_fsSID, 0, sizeof(m_fsSID));
  memset(&m_fxSID, 0, sizeof(m_fxSID));
  memset(&m_fySID, 0, sizeof(m_fySID));
  memset(&m_fzSID, 0, sizeof(m_fzSID));
  ///////
	
  ATH_MSG_DEBUG("end AFP_SIDLocReco::ClearGeometry()");
  return StatusCode::SUCCESS;
}


std::list<SIDHIT> AFP_SIDLocReco::AFPCollectionReading(SG::ReadHandle<xAOD::AFPSiHitContainer> siHitContainer, SG::ReadHandle<xAOD::EventInfo> eventInfo) const 
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::AFPCollectionReading()");

  SIDHIT SIDHit;
  std::list<SIDHIT> ListSIDHits;
  ListSIDHits.clear();

  for(auto hit : *siHitContainer)
  {
    SIDHit.iEvent 			= eventInfo->eventNumber();
    SIDHit.fADC  			= hit->depositedCharge();
    SIDHit.fTDC 			= 0;
    SIDHit.nDetectorID  		= hit->pixelLayerID();
    SIDHit.nStationID 		= hit->stationID();
    SIDHit.nPixelRow		= hit->pixelVertID();
    SIDHit.nPixelCol		= hit->pixelHorizID();

    ListSIDHits.push_back(SIDHit);
  }
	    
  ATH_MSG_DEBUG("end AFP_SIDLocReco::AFPCollectionReading()");
  return ListSIDHits;
}

StatusCode AFP_SIDLocReco::ReadGeometryDetCS()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::ReadGeometryDetCS()");

  for(Int_t nStationID = 0; nStationID < SIDSTATIONID; nStationID++)
    {				
      for (Int_t nPlateID = 0; nPlateID < SIDCNT; nPlateID++)
	{
			
	  HepGeom::Point3D<double> LocPoint = HepGeom::Point3D<double>(-SID_SENSORXDIM+SID_DEATH_EDGE, -SID_SENSORYDIM+SID_LOWER_EDGE, 0.*CLHEP::mm); //changed! (death edge info from AFP_Geometry)
	  HepGeom::Point3D<double> GloPoint = HepGeom::Point3D<double>();
	  StatusCode sc = m_pGeometry->GetPointInSIDSensorGlobalCS(nStationID, nPlateID, LocPoint, GloPoint);
			
	  if (sc.isFailure())
	    {
	      ATH_MSG_WARNING("AFP_Geometry::GetPointInSIDSensorGlobalCS() Failed");

	      m_fsSID[nStationID][nPlateID] = SID_NOMINALSLOPE;
	      m_fxSID[nStationID][nPlateID] = LocPoint.x();
	      m_fySID[nStationID][nPlateID] = LocPoint.y();
	      m_fzSID[nStationID][nPlateID] = SID_NOMINALSPACING*nPlateID;
	    }
	  else
	    {
	      m_fsSID[nStationID][nPlateID] = SID_NOMINALSLOPE;
	      m_fxSID[nStationID][nPlateID] = GloPoint.x(); 
	      m_fySID[nStationID][nPlateID] = GloPoint.y(); 
	      m_fzSID[nStationID][nPlateID] = GloPoint.z();	
					
	      ATH_MSG_DEBUG("Global edge position of SID sensor: " <<GloPoint.x()<< "\t" <<GloPoint.y()<< "\t" <<GloPoint.z()<< "\t");	
	    }
	}
    }	


  ATH_MSG_DEBUG("end AFP_SIDLocReco::ReadGeometryDetCS()");
  return StatusCode::SUCCESS;
}


StatusCode AFP_SIDLocReco::ExecuteRecoMethod(const std::string strAlgo, const std::list<SIDHIT> &ListSIDHits, SG::ReadHandle<xAOD::AFPSiHitContainer> siHitContainer, const EventContext &ctx) const
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::ExecuteRecoMethod()");
  
  // declare the AFPTrack containers
  auto afpTrkContainer = std::make_unique<xAOD::AFPTrackContainer>();
  auto afpTrkAuxContainer = std::make_unique<xAOD::AFPTrackAuxContainer>();
  afpTrkContainer->setStore(afpTrkAuxContainer.get()); 
    
  std::list<SIDRESULT> listSIDResults;
  listSIDResults.clear();

  std::map<std::string, int> mapRecoMethods;
  mapRecoMethods.clear();
  mapRecoMethods.insert(std::pair<std::string, int>("SIDBasicKalman", 1));

  switch(mapRecoMethods[strAlgo]) {
    case 1:
    {
      // perform the track fitting
      // TODO: it would be better to have a tool that takes everything as an argument (say in a struct) so we don't need to create and delete the whole class over and over again
      AFP_SIDBasicKalman* pSIDBasicKalman = new AFP_SIDBasicKalman();
      CHECK( pSIDBasicKalman->Initialize(m_AmpThresh, m_iDataType, ListSIDHits, m_fsSID, m_fxSID, m_fySID, m_fzSID) );
      CHECK( pSIDBasicKalman->Execute() );
      CHECK( pSIDBasicKalman->Finalize(&listSIDResults) );

      afpTrkContainer->reserve(listSIDResults.size());

      std::list<SIDRESULT>::const_iterator iter;
      for(iter=listSIDResults.begin(); iter!=listSIDResults.end(); iter++) {
        if (iter->nStationID != -1) {
          // fill the container with a "dummy" track, then set it properly
          auto track = afpTrkContainer->push_back(std::make_unique<xAOD::AFPTrack>());

          track->setStationID(iter->nStationID);
          track->setXLocal(iter->x_pos);
          track->setYLocal(iter->y_pos);
          track->setZLocal(iter->z_pos);
          track->setXSlope(iter->x_slope);
          track->setYSlope(iter->y_slope);
          track->setNHoles(iter->nHoles);
          track->setChi2(iter->fChi2);

	  auto trkStationID = Monitored::Scalar("TrkStationID", -999.0);
	  auto trkXLocal = Monitored::Scalar("TrkXLocal", -999.0);
	  auto trkYLocal = Monitored::Scalar("TrkYLocal", -999.0);
	  auto trkZLocal = Monitored::Scalar("TrkZLocal", -999.0);
	  auto trkXSlope = Monitored::Scalar("TrkXSlope", -999.0);
	  auto trkYSlope = Monitored::Scalar("TrkYSlope", -999.0);
	  auto trkNHoles = Monitored::Scalar("TrkNHoles", -999.0);
	  auto trkChi2 = Monitored::Scalar("TrkChi2", -999.0);

	  ATH_MSG_DEBUG("AFP_SIDLocReco::Trk XLocal "<<trkXLocal);
	  ATH_MSG_DEBUG("AFP_SIDLocReco::Trk ZLocal "<<trkZLocal);
	  ATH_MSG_DEBUG("AFP_SIDLocReco::NHoles "<<trkNHoles);
	  
	  trkStationID = track->stationID();
	  trkXLocal = track->xLocal();
	  trkYLocal = track->yLocal();
	  trkZLocal = track->zLocal();
	  trkXSlope = track->xSlope();
	  trkYSlope = track->ySlope();
	  trkNHoles = track->nHoles();
	  trkChi2 = track->chi2();

	  auto monitorItTrkProp = Monitored::Group(m_monTool, trkStationID, trkXLocal, trkYLocal, trkZLocal, trkXSlope, trkYSlope, trkNHoles, trkChi2);
	  Monitored::fill(m_monTool, trkStationID, trkXLocal, trkYLocal, trkZLocal, trkXSlope, trkYSlope, trkNHoles, trkChi2);

          ATH_MSG_DEBUG("Track reconstructed with "<<iter->ListHitID.size()<<" hits.");
          
          // look for hits that forms the track
          std::vector<int>::const_iterator end = iter->ListHitID.end();
          for (std::vector<int>::const_iterator hitIter = iter->ListHitID.begin(); hitIter != end; ++hitIter) {
            const int pixelRow = (*hitIter)%100;
            const int pixelCol = (( (*hitIter)/100) % 1000);
            const int pixelLayer = ( (*hitIter)/100000) % 10;
            const int pixelStation = ( (*hitIter)/1000000) % 10;
            unsigned int result = 0;
            for (auto origHitIter : *siHitContainer) {
              if (
                  (origHitIter)->pixelVertID() == pixelRow
                  && (origHitIter)->pixelHorizID() == pixelCol
                  && (origHitIter)->pixelLayerID() == pixelLayer
                  && (origHitIter)->stationID() == pixelStation
                  )
                break;

              result++;
            }

          // check if the hit was found
            if (result < siHitContainer->size()) {
              ATH_MSG_DEBUG("To the list of hits in a track adding hit "<<result<<"/"<<siHitContainer->size()<<".");
            }
            else {
              ATH_MSG_WARNING("Track hit not found in hits list. HitID: "<<(*hitIter)<<"  station: "<<pixelStation<<"  layer: "<<pixelLayer<<"  row: "<<pixelRow<<"  col: "<<pixelCol<<"  dataType: "<<m_iDataType);
            }
          } // loop over hits
        } // if station ID is reasonable
      } // loop over tracks

      delete pSIDBasicKalman;
      break;
    } // case 1 - SIDBasicKalman
    default:
    {
      ATH_MSG_WARNING("Unable to recognize selected algorithm");
      return StatusCode::SUCCESS;
    }
  } // case of track reco algorithms

  ATH_MSG_DEBUG("There are "<<afpTrkContainer->size()<<" / "<<afpTrkAuxContainer->size()<<" entries in the track container / aux");

  auto trkSize = Monitored::Scalar( "TrksSize"   , -1.0 );
  trkSize = afpTrkContainer->size();
  auto monitorItTrkSize = Monitored::Group( m_monTool, trkSize); 
  Monitored::fill(m_monTool,trkSize);

  // write it down
  SG::WriteHandle<xAOD::AFPTrackContainer> trackContainer(m_AFPTrackContainerKey, ctx);
  CHECK( trackContainer.record(std::move(afpTrkContainer), std::move(afpTrkAuxContainer)) );

  ATH_MSG_DEBUG("end AFP_SIDLocReco::ExecuteRecoMethod()");
  return StatusCode::SUCCESS;
}


void SIDRESULT::clear()
{
  nStationID=-1;	
  x_pos=0.;
  y_pos=0.;
  z_pos=0.;
  x_slope=0.;
  y_slope=0.;
  z_slope=0.;
	
  nHits=0;
  nHoles=0;
  fChi2=0.;
}
