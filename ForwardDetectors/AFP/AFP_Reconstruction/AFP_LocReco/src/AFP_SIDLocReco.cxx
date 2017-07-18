/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AFP_LocReco/AFP_SIDLocReco.h"

#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h" 

#include "AthLinks/ElementLink.h"

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"
#include "xAODForward/AFPTrack.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrackAuxContainer.h"

using namespace std;


AFP_SIDLocReco::AFP_SIDLocReco(const string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_recoToolHandle("AFPSiDBasicKalmanTool")
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::AFP_SIDLocReco");

  declareProperty("recoTool",m_recoToolHandle);

  m_Config.clear();
  m_pGeometry = new AFP_Geometry(&m_Config);


  // data type using in the local reconstruction
  // for the simulation data the value is 0, for the real data the value is 1. Unset value is -1
  declareProperty("DataType", m_iDataType=0, "data type using in the local reconstruction");

  //reconstruction methods properties
  declareProperty("AmpThresh", m_AmpThresh=10.);


  //reconstruction method selection for TD
  declareProperty("AlgoSID", m_strAlgoSID="SIDBasicKalman");


  m_vecListAlgoSID.clear();
  declareProperty("ListAlgoSID", m_vecListAlgoSID);


  m_strKeyGeometryForReco			= "AFP_GeometryForReco";

  m_pSIDLocRecoEvCollection = NULL;
  m_pSIDLocRecoEvent = NULL;
  m_storeGate = NULL;

  m_eventNum = 0;
  m_iEvent   = 0;
  m_iRunNum  = 0;

  ATH_MSG_DEBUG("end AFP_SIDLocReco::AFP_SIDLocReco");
}

AFP_SIDLocReco::~AFP_SIDLocReco()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::~AFP_SIDLocReco");

  ATH_MSG_DEBUG("begin AFP_SIDLocReco::~AFP_SIDLocReco");
}

StatusCode AFP_SIDLocReco::initialize()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::initialize()");

  StatusCode sc;
  ClearGeometry();

  sc=m_recoToolHandle.retrieve(); //Dot, not asterisk! This is a method of the ToolHandle, not of the tool it holds.
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve AlgTool " << m_recoToolHandle);
    return sc;
  }

  sc = service("StoreGateSvc",m_storeGate);
  if(sc.isFailure())
    {
      ATH_MSG_WARNING("reconstruction: unable to retrieve pointer to StoreGateSvc");
      return sc;
    } 
	
  //read geometry
  if(ReadGeometryDetCS())
    {
      ATH_MSG_DEBUG("Geometry loaded successfully");
    }
  else
    {
      ATH_MSG_WARNING("Could not load geometry");
      return StatusCode::SUCCESS;
    }

  m_iEvent = 0;

  ATH_MSG_DEBUG("end AFP_SIDLocReco::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::execute()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::execute()");

  StatusCode sc;

  m_eventNum = 0;
  m_iRunNum  = 0;
  const EventInfo* eventInfo;
  sc = m_storeGate->retrieve( eventInfo );
  if (sc.isFailure())
    {
      ATH_MSG_WARNING("AFP_SIDLocReco, Cannot get event info.");
      return StatusCode::SUCCESS;
    }
  else
    {
      // current event number
      m_eventNum = eventInfo->event_ID()->event_number();
      m_iRunNum = eventInfo->event_ID()->run_number();
    }

  CHECK( m_recoToolHandle->reconstructTracks() );

  m_iEvent++;
  ATH_MSG_DEBUG("end AFP_SIDLocReco::execute()");
  return StatusCode::SUCCESS;
}

StatusCode AFP_SIDLocReco::finalize()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::finalize()");


  ATH_MSG_DEBUG("end AFP_SIDLocReco::finalize()");
  return StatusCode::SUCCESS;
}

void AFP_SIDLocReco::ClearGeometry()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::ClearGeometry()");

  memset(&m_fsSID, 0, sizeof(m_fsSID));
  memset(&m_fxSID, 0, sizeof(m_fxSID));
  memset(&m_fySID, 0, sizeof(m_fySID));
  memset(&m_fzSID, 0, sizeof(m_fzSID));
	
  ATH_MSG_DEBUG("end AFP_SIDLocReco::ClearGeometry()");
}


bool AFP_SIDLocReco::ReadGeometryDetCS()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::ReadGeometryDetCS()");

  StatusCode sc;
	
  for(Int_t nStationID = 0; nStationID < SIDSTATIONID; nStationID++)
    {				
      for (Int_t nPlateID = 0; nPlateID < SIDCNT; nPlateID++)
	{
			
	  HepGeom::Point3D<double> LocPoint = HepGeom::Point3D<double>(-SID_SENSORXDIM+SID_DEATH_EDGE, -SID_SENSORYDIM+SID_LOWER_EDGE, 0.*CLHEP::mm); //changed! (death edge info from AFP_Geometry)
	  HepGeom::Point3D<double> GloPoint = HepGeom::Point3D<double>();
	  sc = m_pGeometry->GetPointInSIDSensorGlobalCS(nStationID, nPlateID, LocPoint, GloPoint);
			
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

  return 1;
}

bool AFP_SIDLocReco::StoreReconstructionGeometry(/*const char* szDataDestination*/)
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::StoreReconstructionGeometry()");

  ATH_MSG_DEBUG("end AFP_SIDLocReco::StoreReconstructionGeometry()");

  return true;
}

void AFP_SIDLocReco::SaveGeometry()
{
  ATH_MSG_DEBUG("begin AFP_SIDLocReco::SaveGeometry()");

  ATH_MSG_DEBUG("end AFP_SIDLocReco::SaveGeometry()");
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
