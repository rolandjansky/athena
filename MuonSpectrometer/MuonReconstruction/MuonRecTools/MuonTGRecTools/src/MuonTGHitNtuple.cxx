/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTGRecTools/MuonTGHitNtuple.h"

// Gaudi
#include "TTree.h"
#include "TFile.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkTrack/Track.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonPrepRawData/MdtPrepData.h" 
#include "MuonSimData/MuonSimData.h" 
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MdtHitIdHelper.h"
#include "MuonSimEvent/RpcHitIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h"
#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/RPCSimHit.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"

// Tool service 
#include "GaudiKernel/IToolSvc.h"

// HepPDT
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"

Muon::MuonTGHitNtuple::MuonTGHitNtuple(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_mdtIdHelper(0),
  m_rpcIdHelper(0),
  m_tgcIdHelper(0),
  m_cscIdHelper(0),
  m_mdtHelper(0),
  m_rpcHelper(0),
  m_cscHelper(0),
  m_tgcHelper(0),
  m_muonMgr(0),
  m_holesTool("Muon::MuonHolesOnTrackTool/MuonHolesOnTrackTool"),
  m_measTool("Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"),
  m_trackingGeometry(0),
  m_trackingGeometryName("MuonStandaloneTrackingGeometry"),
  m_detStore(0),
  m_activeStore(0),
  m_StoreGate(0),
  m_inputTracks(0),
  m_inputTrackCollection("ConvertedMBoyTracks"),
  m_inputFatrasTracks(0),
  m_inputFatrasTrackCollection("FatrasMuonTracks"),
  m_mcEventCollection("TruthEvent"),
  m_particlePropSvc("PartPropSvc","PartPropSvc"),
  m_particleDataTable( 0),
  m_processHoles(false),
  m_identifyHoles(false),
  m_processFatras(true),
  m_ptree(0),
  m_THistSvc(0),
  m_treeLocation("/tree/hits")

{  
  declareProperty("InputTrackCollection",          m_inputTrackCollection);
  declareProperty("McEventCollection",             m_mcEventCollection);
  declareProperty("TreeFolderLocation",            m_treeLocation);
  declareProperty("InputFatrasTrackCollection",    m_inputFatrasTrackCollection);
  declareProperty("TrackingGeometryName",          m_trackingGeometryName);
  declareProperty("ProcessHoles",                  m_processHoles);
  declareProperty("IdentifyHoles",                 m_identifyHoles);
  declareProperty("ProcessFatrasTracks",           m_processFatras);
  declareProperty("Extrapolator",                  m_extrapolator);

  m_MaxNberOfHits = 1000; // not used in code
  m_nSim = 0;
  m_nSimHit = 0;
  m_nRec = 0;
  m_nHit = 0;
  m_nHole = 0;
  m_nFSim = 0;
  m_nFHit = 0;
  
}

StatusCode Muon::MuonTGHitNtuple::initialize() 
{
  ATH_MSG_INFO("MuonTGHitNtuple::initialize()");
 
  StatusCode sc;

  // create new root file
  //m_outFile = new TFile( "ntuple.root", "RECREATE");
  // pointer to directory 
  //TDirectory* dir = m_outFile->mkdir( "hits" );
   
  m_ptree = new TTree("MuonTGHitNtuple","MuonTGHitNtuple");
  m_ptree->Branch("nSim",         &m_nSim,          "nSim/I");
  m_ptree->Branch("sim_pdg",      m_sim_pdg,       "sim_pdg[nSim]/I");
  m_ptree->Branch("sim_index",    m_sim_index,     "sim_index[nSim]/I");
  m_ptree->Branch("sim_mom",      m_sim_mom,       "sim_mom[nSim][4]");
  m_ptree->Branch("sim_theta",    m_sim_theta,     "sim_theta[nSim]");
  m_ptree->Branch("sim_phi",      m_sim_phi,       "sim_phi[nSim]");
  m_ptree->Branch("sim_pos",      m_sim_pos,       "sim_pos[nSim][3]");
  m_ptree->Branch("nSimHit",          &m_nSimHit,      "nSimHit/I");
  m_ptree->Branch("sim_hit_id",       m_sim_hit_id,    "sim_hit_id[nSimHit]/I");
  m_ptree->Branch("sim_hit_index",    m_sim_hit_index,     "sim_hit_index[nSimHit]/I");
  m_ptree->Branch("sim_hit_techn",    m_sim_hit_techn,     "sim_hit_techn[nSimHit]/I");
  m_ptree->Branch("sim_hit_station_name",  m_sim_hit_station_name,   "sim_hit_station_name[nSimHit]/I");
  m_ptree->Branch("sim_hit_station_eta",  m_sim_hit_station_eta,   "sim_hit_station_eta[nSimHit]/I");
  m_ptree->Branch("sim_hit_station_phi",  m_sim_hit_station_phi,   "sim_hit_station_phi[nSimHit]/I");
  m_ptree->Branch("sim_hit_pos",      m_sim_hit_pos,       "sim_hit_pos[nSimHit][3]");
  m_ptree->Branch("sim_hit_residual",      m_sim_hit_residual,       "sim_hit_residual[nSimHit]");
  m_ptree->Branch("sim_hit_eloss",  m_sim_hit_eloss,   "sim_hit_eloss[nSimHit]");
  m_ptree->Branch("sim_hit_length",  m_sim_hit_length,   "sim_hit_length[nSimHit]");
  m_ptree->Branch("sim_hit_match",    m_sim_hit_match,     "sim_hit_match[nSimHit]/I");
  m_ptree->Branch("sim_hole_match",   m_sim_hole_match,    "sim_hole_match[nSimHit]/I");
  m_ptree->Branch("nRec",         &m_nRec,         "nRec/I");     
  m_ptree->Branch("rec_author",    m_rec_author,   "rec_author[nRec]/I");
  m_ptree->Branch("rec_match",     m_rec_match,    "rec_match[nRec]/I");
  m_ptree->Branch("rec_mom",       m_rec_mom,      "rec_mom[nRec]");
  m_ptree->Branch("rec_theta",     m_rec_theta,    "rec_theta[nRec]");
  m_ptree->Branch("rec_phi",       m_rec_phi,      "rec_phi[nRec]");
  m_ptree->Branch("nHit",         &m_nHit,         "nHit/I");     
  m_ptree->Branch("hit_id",        m_hit_id,       "hit_id[nHit]/I");
  m_ptree->Branch("hit_techn",        m_hit_techn,       "hit_techn[nHit]/I");
  m_ptree->Branch("hit_match",     m_hit_match,    "hit_match[nHit]/I");
  m_ptree->Branch("hit_track",     m_hit_track,    "hit_track[nHit]/I");
  m_ptree->Branch("hit_station_name",  m_hit_station_name,   "hit_station_name[nHit]/I");
  m_ptree->Branch("hit_station_eta",  m_hit_station_eta,   "hit_station_eta[nHit]/I");
  m_ptree->Branch("hit_station_phi",  m_hit_station_phi,   "hit_station_phi[nHit]/I");
  m_ptree->Branch("nHole",         &m_nHole,         "nHole/I");     
  m_ptree->Branch("hole_id",        m_hole_id,       "hole_id[nHole]/I");
  m_ptree->Branch("hole_techn",     m_hole_techn,    "hole_techn[nHole]/I");
  m_ptree->Branch("hole_match",     m_hole_match,    "hole_match[nHole]/I");
  m_ptree->Branch("hole_residual",  m_hole_residual, "hole_residual[nHole]");
  m_ptree->Branch("hole_track",     m_hole_track,    "hole_track[nHole]/I");
  m_ptree->Branch("hole_station_name",  m_hole_station_name,   "hole_station_name[nHole]/I");
  m_ptree->Branch("hole_station_eta",  m_hole_station_eta,   "hole_station_eta[nHole]/I");
  m_ptree->Branch("hole_station_phi",  m_hole_station_phi,   "hole_station_phi[nHole]/I");
  m_ptree->Branch("nFSim",         &m_nFSim,         "nFSim/I");     
  m_ptree->Branch("fsim_hit_id",        m_fsim_hit_id,       "fsim_hit_id[nFSim]/I");
  m_ptree->Branch("fsim_hit_techn",        m_fsim_hit_techn,       "fsim_hit_techn[nFSim]/I");
  m_ptree->Branch("fsim_hit_station_name",  m_fsim_hit_station_name,   "fsim_hit_station_name[nFSim]/I");
  m_ptree->Branch("fsim_hit_station_eta",  m_fsim_hit_station_eta,   "fsim_hit_station_eta[nFSim]/I");
  m_ptree->Branch("fsim_hit_station_phi",  m_fsim_hit_station_phi,   "fsim_hit_station_phi[nFSim]/I");
  m_ptree->Branch("fsim_hit_residual",  m_fsim_hit_residual,   "fsim_hit_residual[nFSim]");
  m_ptree->Branch("fsim_hit_index",  m_fsim_hit_index,   "fsim_hit_index[nFSim]/I");
  m_ptree->Branch("fsim_hit_eloss",  m_fsim_hit_eloss,   "fsim_hit_eloss[nFSim]");
  m_ptree->Branch("fsim_hit_length",  m_fsim_hit_length,   "fsim_hit_length[nFSim]");
  m_ptree->Branch("nFHit",         &m_nFHit,         "nFHit/I");     
  m_ptree->Branch("fhit_id",        m_fhit_id,       "fhit_id[nFHit]/I");
  m_ptree->Branch("fhit_techn",        m_fhit_techn,       "fhit_techn[nFHit]/I");
  m_ptree->Branch("fhit_station_name",  m_fhit_station_name,   "fhit_station_name[nFHit]/I");
  m_ptree->Branch("fhit_station_eta",  m_fhit_station_eta,   "fhit_station_eta[nFHit]/I");
  m_ptree->Branch("fhit_station_phi",  m_fhit_station_phi,   "fhit_station_phi[nFHit]/I");
  m_ptree->Branch("fhit_residual",  m_fhit_residual,   "fhit_residual[nFHit]");

  sc = service( "THistSvc", m_THistSvc, true );
  if( !sc.isSuccess() ) {
    ATH_MSG_ERROR("!! Unable to locate the THistSvc service !!");
     return sc;
  }
 
  m_THistSvc->regTree(m_treeLocation, m_ptree);

  // Store Gate active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get ActiveStoreSvc ");
    return sc ;
  }

  // Initialize the IdHelper
  //StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure())   {
    ATH_MSG_ERROR("Can't locate the DetectorStore"); 
    return sc;
  }

  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  } 

  // retrieve the id helper
  sc = m_detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Cannot retrieve MdtIdHelper");
      return sc;
    }
  //simulation identifier helper
  m_mdtHelper = MdtHitIdHelper::GetHelper();

  // retrieve the id helper
  sc = m_detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Cannot retrieve RpcIdHelper");
      return sc;
    }
  //simulation identifier helper
  m_rpcHelper = RpcHitIdHelper::GetHelper();
  m_cscHelper = CscHitIdHelper::GetHelper();
  m_tgcHelper = TgcHitIdHelper::GetHelper();

  // retrieve the id helper
  sc = m_detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Cannot retrieve TgcIdHelper");
      return sc;
    }

  // retrieve the id helper
  sc = m_detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Cannot retrieve CscIdHelper");
      return sc;
    }

  sc = m_detStore->retrieve(m_muonMgr);
  if (sc.isFailure())
    {
      ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager...");
      return sc;
    }

  // get holes-on-track tool
  if (m_processHoles) {
    sc = m_holesTool.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not find holes-on-track tool ");
    } else {
      ATH_MSG_INFO("holes-on-track tool booked ");
    }
  }

  // get measurement tool
  sc = m_measTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find TG measurement tool ");
  } else {
    ATH_MSG_INFO("TG measurement tool booked ");
  }

  // get extrapolator
  sc = m_extrapolator.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not find extrapolator tool. Exiting.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Extrapolator tool booked ");
  }

  //  get the ParticleProperties 
  if (m_particlePropSvc.retrieve().isFailure())
    {
      ATH_MSG_FATAL("Can not retrieve " << m_particlePropSvc );
      return StatusCode::FAILURE;
    }
  // and the particle data table 
  m_particleDataTable = m_particlePropSvc->PDT();
  if( !m_particleDataTable )   {
    ATH_MSG_ERROR("Could not get ParticleDataTable!"
	<< " Cannot associate pdg code with charge!");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode Muon::MuonTGHitNtuple::execute()
{
  ATH_MSG_INFO("filling ntuple");
  
  fillSimNtuple();

  if (m_processFatras) {
    // get input fatras tracks 
    if ( m_inputFatrasTrackCollection != "") {
      StatusCode sc = m_StoreGate->retrieve(m_inputFatrasTracks, m_inputFatrasTrackCollection);
      if (sc.isFailure()){
	ATH_MSG_ERROR("Track collection named " << m_inputFatrasTrackCollection << " not found ");
      }
    }
    else {
      ATH_MSG_ERROR("m_inputFatrasTrackCollection not set");
      m_inputFatrasTracks = 0;
    }

    fillFatras();
    
  } 
  
  // get input tracks 
  if (m_inputTrackCollection != "") {
    StatusCode sc = m_StoreGate->retrieve(m_inputTracks, m_inputTrackCollection);
    if (sc.isFailure()){
	ATH_MSG_ERROR("Track collection named " << m_inputTrackCollection << " not found ");
    }
  }
  else {
    ATH_MSG_ERROR("m_inputTrackCollection not set");
      m_inputTracks = 0;
  }
  
  if (m_inputTracks) {
    
    fillRecNtuple(m_inputTracks);
    
    if (m_processHoles) fillHoles(m_inputTracks);
    
  } else if (!m_inputTracks && m_processHoles)  {
    
    m_inputTracks = holesFromSim();
    
    if (m_inputTracks) {
      
      fillRecNtuple(m_inputTracks);
      
      // record the new track collection
      StatusCode sc = m_StoreGate->record(m_inputTracks,"HolesFromSim");
      if (sc.isFailure()){
	ATH_MSG_ERROR("New Track Container could not be recorded in StoreGate !");   
      } else {
	ATH_MSG_INFO("HolesFromSim recorded in StoreGate");   
      }
    }
  }

  m_ptree->Fill();

  return StatusCode::SUCCESS;
}

StatusCode Muon::MuonTGHitNtuple::finalize() 
{
  ATH_MSG_INFO("MuonTGHitNtuple::finalize()");

  // write output and close file
  // m_outFile->Write();
  // m_outFile->Close();

  return StatusCode::SUCCESS;
}

void Muon::MuonTGHitNtuple::fillFatras() const 
{
  MsgStream log(msgSvc(), name());
  //  
  Trk::Perigee* peri = 0;

  if (m_nSim==1) {
    Amg::Vector3D origin(0.,0.,0.);
    // retrieve gen initial parameters
    Amg::Vector3D mom(m_sim_mom[0][0],m_sim_mom[0][1],m_sim_mom[0][2]);
    Amg::Vector3D pos(m_sim_pos[0][0],m_sim_pos[0][1],m_sim_pos[0][2]);
    // charge
    const HepPDT::ParticleData* ap = m_particleDataTable->particle(abs(m_sim_pdg[0]));
    double q = ap->charge();
    if (m_sim_pdg[0]<0) q=-q;
    peri = new Trk::Perigee(pos,mom,q,origin);
  }

  m_nFSim  = 0;
  std::string key = "MDT_Hits_Fatras";
  const DataHandle<MDTSimHitCollection> mdt_hit;
  if ((*m_activeStore)->retrieve(mdt_hit, key)) {
    MDTSimHitCollection::const_iterator ci =mdt_hit->begin();
    MDTSimHitCollection::const_iterator ce =mdt_hit->end();
    ATH_MSG_INFO("MDT Fatras Hits Container retrieved " << ce-ci );
    for (; ci != ce; ci++) {
      const int id = ci->MDTid();
      std::string stationName = m_mdtHelper->GetStationName(id);
      int stationEta = m_mdtHelper->GetZSector(id);
      int stationPhi  = m_mdtHelper->GetPhiSector(id);
      int multilayer = m_mdtHelper->GetMultiLayer(id);
      int layer = m_mdtHelper->GetLayer(id);
      int tube = m_mdtHelper->GetTube(id);
      
      Identifier iMdt = m_mdtIdHelper->channelID(stationName, stationEta,
						 stationPhi, multilayer, layer, tube);
      //int barcode = ci->particleNum();   // just an index    
      int barcode = ci->trackNumber()+1;   // just an index 
      // belongs to selected muon ?
      bool selected = true;
      //if ( muon_map.size()==1 && barcode == 0 ) selected = true;
      /* accept all hits
      for (unsigned int im=0; im < muon_map.size(); im++) {
	if ( barcode == (*(muon_map[im]))->barcode() ) { selected = true; hitCount[im]++; break; }
      }
      */
      if (selected) {
	m_fsim_hit_id[m_nFSim]    = iMdt.get_identifier32().get_compact();
	m_fsim_hit_index[m_nFSim] = barcode+1;      // link to mother particle
	m_fsim_hit_techn[m_nFSim] = 0;    
	m_fsim_hit_station_name[m_nFSim] = m_mdtIdHelper->stationName(iMdt);
	m_fsim_hit_station_eta[m_nFSim] = m_mdtIdHelper->stationEta(iMdt);
	m_fsim_hit_station_phi[m_nFSim] = m_mdtIdHelper->stationPhi(iMdt);
	int sign = ci->localPosition()[0]>0 ? -1 : 1;
	m_fsim_hit_pos[m_nFSim][0] = sign*ci->driftRadius(); 
	m_fsim_hit_residual[m_nFSim] = 10000.; 
	// extrapolate gen. track to get residual
	if (peri) { 
	  // destination surface
	  const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(iMdt);
	  if ( mdtROE ) { 
	    const Trk::TrackParameters* trPar = m_extrapolator->extrapolate(*peri,mdtROE->surface(iMdt),
									    Trk::alongMomentum,false,Trk::muon) ;
	    if (trPar) m_fsim_hit_residual[m_nFSim] = trPar->parameters()[Trk::locR]-m_fsim_hit_pos[m_nFSim][0];
	  }
	}
	m_nFSim++;
      }
    }   
  }

  key = "RPC_Hits_Fatras";
  const DataHandle<RPCSimHitCollection> rpc_hit;
  if ((*m_activeStore)->retrieve(rpc_hit, key)) {
    RPCSimHitCollection::const_iterator ci =rpc_hit->begin();
    RPCSimHitCollection::const_iterator ce =rpc_hit->end();
    ATH_MSG_INFO( key << " container retrieved " << ce-ci );
    for (; ci != ce; ci++) {

      Identifier iRpc = getRpcId(&(*ci));
      
      int barcode = ci->trackNumber()+1;   // just an index
      bool selected = true;
      if (selected) {
	m_fsim_hit_id[m_nFSim] = iRpc.get_identifier32().get_compact();
	m_fsim_hit_index[m_nFSim] = barcode;      // link to mother particle
	m_fsim_hit_techn[m_nFSim] = 1;
        if (m_rpcIdHelper->measuresPhi(iRpc)) m_fsim_hit_techn[m_nFSim] += 10;
	//m_fsim_hit_match[m_nFSim] = -1;
	//m_fsim_hole_match[m_nFSim] = -1;
	m_fsim_hit_station_name[m_nFSim] = m_rpcIdHelper->stationName(iRpc);
	m_fsim_hit_station_eta[m_nFSim] = m_rpcIdHelper->stationEta(iRpc);
	m_fsim_hit_station_phi[m_nFSim] = m_rpcIdHelper->stationPhi(iRpc);
        for (unsigned int i=0;i<3;i++) m_fsim_hit_pos[m_nFSim][i] = ci->localPosition()[i]; 
	m_fsim_hit_residual[m_nFSim] = 10000.; 
	// extrapolate gen. track to get residual
	if (peri) { 
	  // destination surface
	  const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(iRpc);
	  if ( rpcROE ) { 
	    const Trk::TrackParameters* trPar = m_extrapolator->extrapolate(*peri,rpcROE->surface(iRpc),
									    Trk::alongMomentum,false,Trk::muon) ;
            if (trPar) { 
	      const Amg::Vector3D locExtr = rpcROE->globalToLocalCoords(trPar->position(),iRpc);
	      m_fsim_hit_residual[m_nFSim] = (m_rpcIdHelper->measuresPhi(iRpc) ) ? locExtr[1] - m_fsim_hit_pos[m_nFSim][1]:
		                                                                   locExtr[2] - m_fsim_hit_pos[m_nFSim][2];
	    }
	  }
	}
        
	m_nFSim++;
      }
    }
  }

  key = "CSC_Hits_Fatras";
  const DataHandle<CSCSimHitCollection> csc_hit;
  if ((*m_activeStore)->retrieve(csc_hit, key)) {
    CSCSimHitCollection::const_iterator ci =csc_hit->begin();
    CSCSimHitCollection::const_iterator ce =csc_hit->end();
    ATH_MSG_INFO( key << " container retrieved " << ce-ci );
    for (; ci != ce; ci++) {
      int barcode = ci->trackNumber()+1;   // just an index

      //std::cout << "csc global time:"<< ci->globalTime() << std::endl;

      m_fsim_hit_eloss[m_nFSim] = ci->energyDeposit();
      m_fsim_hit_length[m_nFSim] = (ci->getHitEnd()-ci->getHitStart()).mag();

      m_fsim_hit_id[m_nFSim] = 0;
      m_fsim_hit_index[m_nFSim] = barcode;      // link to mother particle
      m_fsim_hit_techn[m_nFSim] = 3;
      m_fsim_hit_station_name[m_nFSim] = -1;
      m_fsim_hit_station_eta[m_nFSim] = -1;
      m_fsim_hit_station_phi[m_nFSim] = -1;
      //for (unsigned int i=0;i<3;i++) m_fsim_hit_pos[m_nFSim][i] = (*ci)->localPosition()[i]; 
      m_fsim_hit_residual[m_nFSim] = 10000.; 

      m_nFSim++;
    }
  }  

  key = "TGC_Hits_Fatras";
  const DataHandle<TGCSimHitCollection> tgc_hit;
  if ((*m_activeStore)->retrieve(tgc_hit, key)) {
    TGCSimHitCollection::const_iterator ci =tgc_hit->begin();
    TGCSimHitCollection::const_iterator ce =tgc_hit->end();
    ATH_MSG_INFO( key << " container retrieved " << ce-ci );
    for (; ci != ce; ci++) {
      int barcode = ci->trackNumber()+1;   // just an index

      //Identifier iTgc = getTgcId(&(*ci));
      m_fsim_hit_id[m_nFSim] = 0;
      m_fsim_hit_index[m_nFSim] = barcode;      // link to mother particle
      m_fsim_hit_techn[m_nFSim] = 2;
      m_fsim_hit_station_name[m_nFSim] = -1;
      m_fsim_hit_station_eta[m_nFSim] = -1;
      m_fsim_hit_station_phi[m_nFSim] = -1;
      //for (unsigned int i=0;i<3;i++) m_fsim_hit_pos[m_nFSim][i] = (*ci)->localPosition()[i]; 
      //std::cout << "fatras tgc sim hit time:position:dir:"<<iTgc<<","<<(*ci)->globalTime()<<","<< (*ci)->localPosition() << ","<< (*ci)->localDireCos() << std::endl; 
      //std::cout << "number of gas gaps :"<<m_muonMgr->getTgcReadoutElement(iTgc)->Ngasgaps()<< std::endl;

      m_fsim_hit_residual[m_nFSim] = 10000.; 

      m_nFSim++;
    }
  }  

  m_nFHit   = 0;

  if (!m_inputFatrasTracks) return;

  for (TrackCollection::const_iterator itr  = (*m_inputFatrasTracks).begin(); itr < (*m_inputFatrasTracks).end(); itr++){

    const DataVector<const Trk::TrackStateOnSurface>* recTSOS = (*itr)->trackStateOnSurfaces(); 
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = recTSOS->begin(); 
    for ( ;iter!= recTSOS->end(); iter++) {
      if ( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ) {        // hit
	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ((*iter)->measurementOnTrack());
	if ( rio ) { 
	  Identifier dig_id = rio->identify();             
          m_fhit_id[m_nFHit] =  dig_id.get_identifier32().get_compact();
          if ( m_mdtIdHelper->is_mdt(dig_id) ) m_fhit_techn[m_nFHit] = 0; 
          if ( m_mdtIdHelper->is_rpc(dig_id) ) m_fhit_techn[m_nFHit] = 1; 
          if ( m_mdtIdHelper->is_tgc(dig_id) ) m_fhit_techn[m_nFHit] = 2; 
          if ( m_mdtIdHelper->is_csc(dig_id) ) m_fhit_techn[m_nFHit] = 3; 
          m_fhit_station_name[m_nFHit] = m_mdtIdHelper->stationName(dig_id);
          m_fhit_station_eta[m_nFHit] = m_mdtIdHelper->stationEta(dig_id);
          m_fhit_station_phi[m_nFHit] = m_mdtIdHelper->stationPhi(dig_id);
          // distinguish phi hits
          if (m_fhit_techn[m_nFHit]==1 && m_rpcIdHelper->measuresPhi(dig_id)) m_fhit_techn[m_nFHit] +=10; 
          if (m_fhit_techn[m_nFHit]==2 && m_tgcIdHelper->isStrip(dig_id)) m_fhit_techn[m_nFHit] +=10; 
          if (m_fhit_techn[m_nFHit]==3 && m_cscIdHelper->measuresPhi(dig_id)) m_fhit_techn[m_nFHit] +=10; 
          // calculate residual as distance of layer intersection from tube/strip center ( check of nearest detEl identification )
          m_fhit_residual[m_nFHit] = 10000.;  // dummy default
          if (m_fhit_techn[m_nFHit]==0) {
	    const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(dig_id);
	    if (mdtROE){      
	      const Amg::Vector3D tubeposition=mdtROE->globalToLocalCoords(mdtROE->tubePos(dig_id), dig_id );
	      const Amg::Vector3D localhit=mdtROE->globalToLocalCoords((*iter)->trackParameters()->position(), dig_id);    
	      m_fhit_residual[m_nFHit] = (tubeposition[1] - localhit[1])/15.0175; 
	    }
	  }
          if (m_fhit_techn[m_nFHit]==1 || m_fhit_techn[m_nFHit]==11) {      // rpc hits
	    const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(dig_id);
	    if (rpcROE){      
	      const Amg::Vector3D stripposition=rpcROE->surface(dig_id).transform().inverse()*rpcROE->stripPos(dig_id);
	      const Amg::Vector3D localhit=rpcROE->surface(dig_id).transform().inverse()*(*iter)->trackParameters()->position();    
	      m_fhit_residual[m_nFHit] = 2*(stripposition[0] - localhit[0])/
		rpcROE->StripPitch(m_rpcIdHelper->measuresPhi(dig_id)); 
	    }
	  }
          if (m_fhit_techn[m_nFHit]==2 || m_fhit_techn[m_nFHit]==12) {      // tgc hits
	    const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(dig_id);
	    if (tgcROE){      
	      const Amg::Vector3D stripposition=tgcROE->surface(dig_id).transform().inverse()*tgcROE->channelPos(dig_id);
	      const Amg::Vector3D localhit=tgcROE->surface(dig_id).transform().inverse()*(*iter)->trackParameters()->position(); 
	      int plane = m_tgcIdHelper->gasGap(dig_id);
              if (m_tgcIdHelper->isStrip(dig_id)) {    
		m_fhit_residual[m_nFHit] = 2*(stripposition[0] - localhit[0])/
		  tgcROE->StripPitch(plane,m_tgcIdHelper->channel(dig_id),localhit[1]);
              } else {
		m_fhit_residual[m_nFHit] = (stripposition[0] - localhit[0])/
		  (*iter)->measurementOnTrack()->localCovariance()(Trk::locX,Trk::locX);
	      }
	    }
	  }
          if (m_fhit_techn[m_nFHit]==3 || m_fhit_techn[m_nFHit]==13) {      // csc hits
	    const MuonGM::CscReadoutElement* cscROE = m_muonMgr->getCscReadoutElement(dig_id);
	    if (cscROE){      
	      const Amg::Vector3D stripposition=cscROE->surface(dig_id).transform().inverse()*cscROE->stripPos(dig_id);
	      const Amg::Vector3D localhit=cscROE->surface(dig_id).transform().inverse()*(*iter)->trackParameters()->position();    
	      m_fhit_residual[m_nFHit] = 2*(stripposition[0] - localhit[0])/
		cscROE->StripPitch(m_cscIdHelper->measuresPhi(dig_id)); 
	    }
	  }
          m_nFHit++;
	}
      } 
    }
  }
}

void Muon::MuonTGHitNtuple::fillRecNtuple(const TrackCollection* tracks ) const 
{
  MsgStream log(msgSvc(), name());

  m_nRec = 0;
  m_nHit   = 0;
  m_nHole = 0;
  int unmatched_hole=0;
  int unmatched_hit=0;

  if (!tracks) return;

  for (TrackCollection::const_iterator itr  = (*tracks).begin(); itr < (*tracks).end(); itr++){

    const DataVector<const Trk::TrackStateOnSurface>* recTSOS = (*itr)->trackStateOnSurfaces(); 
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = recTSOS->begin(); 
    std::vector< std::pair<int,int> > match_tracks;   
    for ( ;iter!= recTSOS->end(); iter++) {
      if ( (*iter)->type(Trk::TrackStateOnSurface::Perigee) )  {        // perigee
	m_rec_theta[m_nRec] = (*iter)->trackParameters()->momentum().theta();
	m_rec_phi[m_nRec]   = (*iter)->trackParameters()->momentum().phi();
	m_rec_mom[m_nRec]   = (*iter)->trackParameters()->momentum().mag();
      } else if ( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ) {        // hit
	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ((*iter)->measurementOnTrack());
	if ( rio ) { 
	  Identifier dig_id = rio->identify();             
          m_hit_id[m_nHit] =  dig_id.get_identifier32().get_compact();
          if ( m_mdtIdHelper->is_mdt(dig_id) ) m_hit_techn[m_nHit] = 0; 
          if ( m_mdtIdHelper->is_rpc(dig_id) ) m_hit_techn[m_nHit] = 1; 
          if ( m_mdtIdHelper->is_tgc(dig_id) ) m_hit_techn[m_nHit] = 2; 
          if ( m_mdtIdHelper->is_csc(dig_id) ) m_hit_techn[m_nHit] = 3; 
          // distinguish phi hits
          if (m_hit_techn[m_nHit]==1 && m_rpcIdHelper->measuresPhi(dig_id)) m_hit_techn[m_nHit] +=10; 
          if (m_hit_techn[m_nHit]==2 && m_tgcIdHelper->isStrip(dig_id)) m_hit_techn[m_nHit] +=10; 
          if (m_hit_techn[m_nHit]==3 && m_cscIdHelper->measuresPhi(dig_id)) m_hit_techn[m_nHit] +=10; 
          //
	  m_hit_track[m_nHit] = m_nRec;
          m_hit_station_name[m_nHit] = m_mdtIdHelper->stationName(dig_id);
          m_hit_station_eta[m_nHit] = m_mdtIdHelper->stationEta(dig_id);
          m_hit_station_phi[m_nHit] = m_mdtIdHelper->stationPhi(dig_id);
          // find matching simulated hit
          m_hit_match[m_nHit] = -1;
          for (int is=0; is < m_nSimHit; is++) {
            if (m_sim_hit_id[is] == dig_id.get_identifier32().get_compact() ) { m_hit_match[m_nHit] = is; m_sim_hit_match[is] = m_nRec; break; }
          }
          if (m_hit_match[m_nHit] <0 ) unmatched_hit++;
          if (m_hit_match[m_nHit]>-1) {   // use hit match to match tracks
            bool found = false;
            for (unsigned int it=0;it<match_tracks.size(); it++) {
              if ( m_sim_hit_index[m_hit_match[m_nHit]] == match_tracks[it].first ) {
                found = true;
                (match_tracks[it].second)++; 
              }             
            }
            if (!found) match_tracks.push_back(std::pair<int,int> ( m_sim_hit_index[m_hit_match[m_nHit]],1) );
          }
          m_nHit++;
	}
      }  else if ( (*iter)->type(Trk::TrackStateOnSurface::Hole) ) {        // hole
	const Trk::TrackParameters* hole = (*iter)->trackParameters();
        const Trk::TrackParameters* holeLayer = 0;
	if ( hole ) { 
          Identifier dig_id(0);
	  const Trk::Layer* layer = m_trackingGeometry->associatedLayer(hole->associatedSurface().center());
          if (m_identifyHoles) {
	    dig_id = hole->associatedSurface().associatedDetectorElementIdentifier();             
            if (layer) holeLayer = m_measTool->detElToLayer(layer,hole,dig_id);
          } else {
            if (layer) dig_id = Identifier(layer->layerType());
	    holeLayer = hole;
	  } 
          m_hole_id[m_nHole] =  dig_id.get_identifier32().get_compact();
          if (dig_id.get_identifier32().get_compact()>0) {  
	    if ( m_mdtIdHelper->is_mdt(dig_id) ) m_hole_techn[m_nHole] = 0; 
	    if ( m_mdtIdHelper->is_rpc(dig_id) ) m_hole_techn[m_nHole] = 1; 
	    if ( m_mdtIdHelper->is_tgc(dig_id) ) m_hole_techn[m_nHole] = 2; 
	    if ( m_mdtIdHelper->is_csc(dig_id) ) m_hole_techn[m_nHole] = 3; 
	    m_hole_track[m_nHit] = m_nRec;
	    m_hole_station_name[m_nHole] = m_mdtIdHelper->stationName(dig_id);
	    m_hole_station_eta[m_nHole] = m_mdtIdHelper->stationEta(dig_id);
	    m_hole_station_phi[m_nHole] = m_mdtIdHelper->stationPhi(dig_id);
	    // find matching simulated hit
	    m_hole_match[m_nHole] = -1;
            m_hole_residual[m_nHole] = 10000.;
	    for (int is=0; is < m_nSimHit; is++) {
              Identifier sim_id(m_sim_hit_id[is]);
	      if ( m_sim_hit_techn[is]==m_hole_techn[m_nHole] && layerMatch(sim_id,dig_id) ) { 
		m_hole_match[m_nHole] = is;
		m_sim_hole_match[is] = m_nHole;
		if (m_hole_techn[m_nHole]==0 && layer && holeLayer) m_hole_residual[m_nHole] =
								      float( m_measTool->residual(layer,holeLayer,Identifier(m_sim_hit_id[is])) ) ;
	      }
	    }
	    if (m_hole_match[m_nHole] <0 ) unmatched_hole++;
	    if (m_hole_match[m_nHole]>-1) {   // use hole match to match tracks
	      bool found = false;
	      for (unsigned int it=0;it<match_tracks.size(); it++) {
		if ( m_sim_index[m_hole_match[m_nHole]] == match_tracks[it].first ) {
		  found = true;
		  (match_tracks[it].second)++; 
		}             
	      }
	      if (!found) match_tracks.push_back(std::pair<int,int> ( m_sim_index[m_hole_match[m_nHole]],1) );
	    }
	  }
          m_nHole++;
	} else {
          ATH_MSG_DEBUG("missing hole parameters?");
	}
      }
    }
    //loop over matched hit statistics to set track match
    m_rec_match[m_nRec]=-1;
    int numHit = 0;
    for (unsigned int it=0;it<match_tracks.size(); it++) {
      if ( match_tracks[it].second > numHit ) {
	numHit =  match_tracks[it].second;
	m_rec_match[m_nRec]= match_tracks[it].first;
      }             
    }    
    m_nRec++;
  }

  int unmatched_sim = 0;
  for (int is=0;is<m_nSimHit;is++) if (m_sim_hole_match[is]<0) unmatched_sim++; 
  ATH_MSG_DEBUG("number of simHit, recHits, holes, in this event:" << m_nSimHit <<"," << m_nHit <<","<< m_nHole );
  ATH_MSG_DEBUG("number of unmatched simHit, recHits, holes, in this event:" << unmatched_sim <<","<< unmatched_hit<<"," << unmatched_hole );
}

void Muon::MuonTGHitNtuple::fillSimNtuple() const
{      
  m_nSim = 0;
  const McEventCollection* McEventCollection = 0;
  StatusCode sc = m_StoreGate->retrieve(McEventCollection, m_mcEventCollection);
 
  if (StatusCode::SUCCESS == sc ) {
    ATH_MSG_DEBUG(" McEventCollection collection retrieved ");
    
  } else {
    ATH_MSG_WARNING(" McEventCollection collection NOT retrieved ");
    return;
  }
  const HepMC::GenEvent*    myGenEvent = *(McEventCollection -> begin());
  HepMC::GenEvent::particle_const_iterator particle = myGenEvent->particles_begin();
  HepMC::GenEvent::particle_const_iterator particle_end = myGenEvent->particles_end();
  std::vector< HepMC::GenEvent::particle_const_iterator > muon_map;
  for(; particle!=particle_end;++particle){
    //std::cout << "generated particle:"<<(*particle)->pdg_id()<<","<<(*particle)->barcode()<<","<<(*particle)->momentum().e()<< std::endl;
    if( std::abs( (*particle)->pdg_id() ) == 13) {
      if ( (*particle)->momentum().e() > 5000 ) muon_map.push_back(particle);
    }
  }

  // create clean muon record
  m_nSim=0;
  for(unsigned int im=0; im<muon_map.size();im++){
    //if (hitCount[im]) {
      m_sim_pdg[m_nSim] = (*(muon_map[im]))->pdg_id();           
      m_sim_index[m_nSim] = (*(muon_map[im]))->barcode();           
      Amg::Vector3D mom( (*(muon_map[im]))->momentum().x(), (*(muon_map[im]))->momentum().y(), (*(muon_map[im]))->momentum().z() );
      for (unsigned int i=0;i<3;i++) m_sim_mom[m_nSim][i]=mom[i];
      m_sim_mom[m_nSim][3]=(*(muon_map[im]))->momentum().e();
      m_sim_theta[m_nSim]=mom.theta(); 
      m_sim_phi[m_nSim]=mom.phi();
      if ((*(muon_map[im]))->production_vertex() ) {
	m_sim_pos[m_nSim][0]= (*(muon_map[im]))->production_vertex()->point3d().x();
	m_sim_pos[m_nSim][1]= (*(muon_map[im]))->production_vertex()->point3d().y();
	m_sim_pos[m_nSim][2]= (*(muon_map[im]))->production_vertex()->point3d().z();
      }
      m_nSim++;      
      //}
  }
  //  
  Trk::Perigee* peri = 0;

  if (muon_map.size()==1) {
    Amg::Vector3D origin(0.,0.,0.);
    // retrieve gen initial parameters
    Amg::Vector3D mom(m_sim_mom[0][0],m_sim_mom[0][1],m_sim_mom[0][2]);
    Amg::Vector3D pos(m_sim_pos[0][0],m_sim_pos[0][1],m_sim_pos[0][2]);
    // charge
    const HepPDT::ParticleData* ap = m_particleDataTable->particle(abs(m_sim_pdg[0]));
    double q = ap->charge();
    if (m_sim_pdg[0]<0) q=-q;
    peri = new Trk::Perigee(pos,mom,q,origin);
  }
 
  
  std::vector<int> hitCount(muon_map.size());
  // simulated hits
  m_nSimHit = 0;

  std::string key = "MDT_Hits";
  const DataHandle<MDTSimHitCollection> mdt_hit;
  if ((*m_activeStore)->retrieve(mdt_hit, key)) {
    MDTSimHitCollection::const_iterator ci =mdt_hit->begin();
    MDTSimHitCollection::const_iterator ce =mdt_hit->end();
    ATH_MSG_INFO(" MDT Hits Container retrieved " << ce-ci<<"," << muon_map.size() );
    for (; ci != ce; ci++) {
      const int id = ci->MDTid();
      std::string stationName = m_mdtHelper->GetStationName(id);
      int stationEta = m_mdtHelper->GetZSector(id);
      int stationPhi  = m_mdtHelper->GetPhiSector(id);
      int multilayer = m_mdtHelper->GetMultiLayer(id);
      int layer = m_mdtHelper->GetLayer(id);
      int tube = m_mdtHelper->GetTube(id);
      
      Identifier iMdt = m_mdtIdHelper->channelID(stationName, stationEta,
						 stationPhi, multilayer, layer, tube);
      //int barcode = ci->particleNum();   // just an index    
      int barcode = ci->trackNumber();   // just an index 
      // belongs to selected muon ?
      bool selected = true;
      //if ( muon_map.size()==1 && barcode == 0 ) selected = true;
      /* accept all hits
      for (unsigned int im=0; im < muon_map.size(); im++) {
	if ( barcode == (*(muon_map[im]))->barcode() ) { selected = true; hitCount[im]++; break; }
      }
      */
      if (selected) {
	m_sim_hit_id[m_nSimHit]    = iMdt.get_identifier32().get_compact();
	m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
	m_sim_hit_techn[m_nSimHit] = 0;    
	m_sim_hit_match[m_nSimHit] = -1;    
	m_sim_hole_match[m_nSimHit] = -1;    
	m_sim_hit_station_name[m_nSimHit] = m_mdtIdHelper->stationName(iMdt);
	m_sim_hit_station_eta[m_nSimHit] = m_mdtIdHelper->stationEta(iMdt);
	m_sim_hit_station_phi[m_nSimHit] = m_mdtIdHelper->stationPhi(iMdt);
	int sign = ci->localPosition()[0]>0 ? -1 : 1;
	m_sim_hit_pos[m_nSimHit][0] = sign * ci->driftRadius(); 
	// extrapolate gen. track to get residual
	m_sim_hit_residual[m_nSimHit] = 10000.; 
	if (peri) { 
	  // destination surface
	  const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(iMdt);
	  if ( mdtROE ) { 
	    const Trk::TrackParameters* trPar = m_extrapolator->extrapolate(*peri,mdtROE->surface(iMdt),
									    Trk::alongMomentum,false,Trk::muon) ;
	    if (trPar) m_sim_hit_residual[m_nSimHit] = trPar->parameters()[Trk::locR]-m_sim_hit_pos[m_nSimHit][0];
	    //if (trPar) std::cout << "localR:"<< trPar->parameters()[Trk::locR]<<","<<m_sim_hit_pos[m_nSimHit][0]<< std::endl;
	  }
	}
	m_nSimHit++;
      }
    }   
  }

  key = "RPC_Hits";
  const DataHandle<RPCSimHitCollection> rpc_hit;
  if ((*m_activeStore)->retrieve(rpc_hit, key)) {
    RPCSimHitCollection::const_iterator ci =rpc_hit->begin();
    RPCSimHitCollection::const_iterator ce =rpc_hit->end();
    ATH_MSG_INFO( key << " container retrieved " << ce-ci );
    for (; ci != ce; ci++) {
      
      Identifier iRpc = getRpcId(&(*ci));
      int barcode = ci->trackNumber();   // just an index
      bool selected = true;
      if (selected) {
	m_sim_hit_id[m_nSimHit] = iRpc.get_identifier32().get_compact();
	m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
	m_sim_hit_techn[m_nSimHit] = 1;
        if (m_rpcIdHelper->measuresPhi(iRpc)) m_sim_hit_techn[m_nSimHit] += 10;
	m_sim_hit_match[m_nSimHit] = -1;
	m_sim_hole_match[m_nSimHit] = -1;
	m_sim_hit_station_name[m_nSimHit] = m_rpcIdHelper->stationName(iRpc);
	m_sim_hit_station_eta[m_nSimHit] = m_rpcIdHelper->stationEta(iRpc);
	m_sim_hit_station_phi[m_nSimHit] = m_rpcIdHelper->stationPhi(iRpc);
        for (unsigned int i=0;i<3;i++) m_sim_hit_pos[m_nSimHit][i] = ci->localPosition()[i]; 
	// extrapolate gen. track to get residual
	m_sim_hit_residual[m_nSimHit] = 10000.; 
	if (peri) { 
	  // destination surface
	  const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(iRpc);
	  if ( rpcROE ) { 
	    const Trk::TrackParameters* trPar = m_extrapolator->extrapolate(*peri,rpcROE->surface(iRpc),
									    Trk::alongMomentum,false,Trk::muon) ;
            if (trPar) { 
	      const Amg::Vector3D locExtr = rpcROE->globalToLocalCoords(trPar->position(),iRpc);
	      m_sim_hit_residual[m_nSimHit] = (m_rpcIdHelper->measuresPhi(iRpc) ) ? locExtr[1] - m_sim_hit_pos[m_nSimHit][1]:
		                                                                    locExtr[2] - m_sim_hit_pos[m_nSimHit][2];
	    }
	  }
	}
	m_nSimHit++;
      }
    }
  }

  key = "TGC_Hits";
  const DataHandle<TGCSimHitCollection> tgc_hit;
  if ((*m_activeStore)->retrieve(tgc_hit, key)) {
    TGCSimHitCollection::const_iterator ci =tgc_hit->begin();
    TGCSimHitCollection::const_iterator ce =tgc_hit->end();
    ATH_MSG_INFO( key << " container retrieved " << ce-ci );
    for (; ci != ce; ci++) {
      //Identifier iTgc = getTgcId(&(*ci));

      int barcode = ci->trackNumber();   // just an index
      m_sim_hit_id[m_nSimHit] = 0;
      m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
      m_sim_hit_techn[m_nSimHit] = 2;
      m_sim_hit_station_name[m_nSimHit] = -1;
      m_sim_hit_station_eta[m_nSimHit] = -1;
      m_sim_hit_station_phi[m_nSimHit] = -1;
      //for (unsigned int i=0;i<3;i++) m_sim_hit_pos[m_nSimHit][i] = (*ci)->localPosition()[i]; 
      //if (barcode>0) std::cout << "tgc sim hit time:position:dir:"<< iTgc<< ","<<(*ci)->globalTime()<<","<<(*ci)->localPosition()<<","<< (*ci)->localDireCos() << std::endl; 
      //const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(iTgc);
      //if (tgcROE) std::cout << "number of gas gaps :"<<tgcROE->Ngasgaps()<< std::endl;
      //else std::cout << "tgc ROE not found!" <<  std::endl; 

      m_sim_hit_residual[m_nSimHit] = 10000.; 

      m_nSimHit++;
    }
  }


  key = "CSC_Hits";
  const DataHandle<CSCSimHitCollection> csc_hit;
  if ((*m_activeStore)->retrieve(csc_hit, key)) {
    CSCSimHitCollection::const_iterator ci =csc_hit->begin();
    CSCSimHitCollection::const_iterator ce =csc_hit->end();
    ATH_MSG_INFO( key << " container retrieved " << ce-ci );
    for (; ci != ce; ci++) {
      Identifier iCsc = getCscId(&(*ci));
      //std::cout << "csc global time:"<< ci->globalTime() << std::endl;
      m_sim_hit_eloss[m_nSimHit] = ci->energyDeposit();
      m_sim_hit_length[m_nSimHit] = (ci->getHitEnd()-ci->getHitStart()).mag();

      int barcode = ci->trackNumber();   // just an index
      m_sim_hit_id[m_nSimHit] = 0;
      m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
      m_sim_hit_techn[m_nSimHit] = 3;
      if (m_cscIdHelper->measuresPhi(iCsc)) m_sim_hit_techn[m_nSimHit] += 10;
      m_sim_hit_station_name[m_nSimHit] = -1;
      m_sim_hit_station_eta[m_nSimHit] = -1;
      m_sim_hit_station_phi[m_nSimHit] = -1;
      //for (unsigned int i=0;i<3;i++) m_sim_hit_pos[m_nSimHit][i] = (*ci)->localPosition()[i]; 
      m_sim_hit_residual[m_nSimHit] = 10000.; 

      m_nSimHit++;
    }
  }

  if(peri) { delete peri; peri = NULL; }

  /*
  std::string key = "MDT_SDO";
  const DataHandle<MuonSimDataCollection> mdt_sdo;
  StatusCode sc_read = (*m_activeStore)->retrieve(mdt_sdo, key);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve MDT SDO Container ");
  } else {
    MuonSimDataCollection::const_iterator ci =mdt_sdo->begin();
    MuonSimDataCollection::const_iterator ce =mdt_sdo->end();
    for (; ci != ce; ci++) {
      Identifier iMdt = ci->first;    
      for (unsigned int i=0; i< ci->second.getdeposits().size(); i++) {
	const std::pair<HepMcParticleLink,MuonMCData> deposit =(ci->second.getdeposits())[i] ;
        int barcode = (deposit.first).barcode();
        // belongs to selected muon ?
        bool selected = false;
	for (unsigned int im=0; im < muon_map.size(); im++) {
          if ( barcode == (*(muon_map[im]))->barcode() ) { selected = true; hitCount[im]++; break; }
        }
        if (selected) {
          m_sim_hit_id[m_nSimHit]    = iMdt;
          m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
          m_sim_hit_techn[m_nSimHit] = 0;    
          m_sim_hit_match[m_nSimHit] = -1;    
          m_sim_hole_match[m_nSimHit] = -1;    
          m_sim_hit_station_name[m_nSimHit] = m_mdtIdHelper->stationName(iMdt);
          m_sim_hit_station_eta[m_nSimHit] = m_mdtIdHelper->stationEta(iMdt);
          m_sim_hit_station_phi[m_nSimHit] = m_mdtIdHelper->stationPhi(iMdt);
          m_nSimHit++;
	}
      }
    }
  }  

  key = "RPC_SDO";
  const DataHandle<MuonSimDataCollection> rpc_sdo;
  sc_read = (*m_activeStore)->retrieve(rpc_sdo, key);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve RPC SDO Container ");
  } else {
    MuonSimDataCollection::const_iterator ci =rpc_sdo->begin();
    MuonSimDataCollection::const_iterator ce =rpc_sdo->end();
    for (; ci != ce; ci++) {
      Identifier iRpc = ci->first;      
      for (unsigned int i=0; i< ci->second.getdeposits().size(); i++) {
	const std::pair<HepMcParticleLink,MuonMCData> deposit =(ci->second.getdeposits())[i] ;
        int barcode = (deposit.first).barcode();
        // belongs to selected muon ?
        bool selected = false;
	for (unsigned int im=0; im < muon_map.size(); im++) {
          if ( barcode ==  (*(muon_map[im]))->barcode() ) { selected = true; hitCount[im]++; break; }
        }
        if (selected) {
          m_sim_hit_id[m_nSimHit]    = iRpc;
          m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
          m_sim_hit_techn[m_nSimHit] = 1;    
          m_sim_hit_match[m_nSimHit] = -1;    
          m_sim_hole_match[m_nSimHit] = -1;    
          m_sim_hit_station_name[m_nSimHit] = m_mdtIdHelper->stationName(iRpc);
          m_sim_hit_station_eta[m_nSimHit] = m_mdtIdHelper->stationEta(iRpc);
          m_sim_hit_station_phi[m_nSimHit] = m_mdtIdHelper->stationPhi(iRpc);
          m_nSimHit++;
	}
      }
    }
  }  

  key = "TGC_SDO";
  const DataHandle<MuonSimDataCollection> tgc_sdo;
  sc_read = (*m_activeStore)->retrieve(tgc_sdo, key);
  if (sc_read.isFailure()) {
    ATH_MSG_ERROR(" Cannot retrieve TGC SDO Container ");
  } else {
    MuonSimDataCollection::const_iterator ci =tgc_sdo->begin();
    MuonSimDataCollection::const_iterator ce =tgc_sdo->end();
    for (; ci != ce; ci++) {
      Identifier iTgc = ci->first;      
      for (unsigned int i=0; i< ci->second.getdeposits().size(); i++) {
	const std::pair<HepMcParticleLink,MuonMCData> deposit =(ci->second.getdeposits())[i] ;
        int barcode = (deposit.first).barcode();
        // belongs to selected muon ?
        bool selected = false;
	for (unsigned int im=0; im < muon_map.size(); im++) {
          if ( barcode ==  (*(muon_map[im]))->barcode() ) { selected = true; hitCount[im]++; break; }
        }
        if (selected) {
          m_sim_hit_id[m_nSimHit]    = iTgc;
          m_sim_hit_index[m_nSimHit] = barcode;      // link to mother particle
          m_sim_hit_techn[m_nSimHit] = 2;    
          m_sim_hit_match[m_nSimHit] = -1;    
          m_sim_hole_match[m_nSimHit] = -1;    
          m_sim_hit_station_name[m_nSimHit] = m_mdtIdHelper->stationName(iTgc);
          m_sim_hit_station_eta[m_nSimHit] = m_mdtIdHelper->stationEta(iTgc);
          m_sim_hit_station_phi[m_nSimHit] = m_mdtIdHelper->stationPhi(iTgc);
          m_nSimHit++;
	}
      }
    }
  } 
  */ 
}
 
void Muon::MuonTGHitNtuple::fillHoles(const TrackCollection* tracks) const
{
  MsgStream log(msgSvc(), name());

  if (!tracks) return;

  TrackCollection* looseEndsColl = new TrackCollection;

  m_nHole = 0;
  int unmatched_hole = 0;
  for (TrackCollection::const_iterator itr  = (*tracks).begin(); itr < (*tracks).end(); itr++){
    const DataVector<const Trk::TrackStateOnSurface>* holes = m_holesTool->getHolesOnTrack(**itr,Trk::muon);
    DataVector<const Trk::TrackStateOnSurface>* looseEnds = new DataVector<const Trk::TrackStateOnSurface>;
    // loop over holes, match with true hits
    if (holes) {
      for (unsigned int ih=0;ih<holes->size(); ih++) {
	Identifier idh(0);
	if ((*holes)[ih]->trackParameters() )
	  idh = (*holes)[ih]->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
	m_hole_id[m_nHole] = idh.get_identifier32().get_compact();
	m_hole_techn[m_nHole] = -1;
	m_hole_match[m_nHole] = -1;
	m_hole_track[m_nHole] = itr-(*tracks).begin();
	if (m_hole_id[m_nHole]) {    // find matching simulated hit (if exists)
	  if ( m_mdtIdHelper->is_mdt(idh) ) m_hole_techn[m_nHole] = 0; 
	  if ( m_mdtIdHelper->is_rpc(idh) ) m_hole_techn[m_nHole] = 1; 
	  if ( m_mdtIdHelper->is_tgc(idh) ) m_hole_techn[m_nHole] = 2; 
	  if ( m_mdtIdHelper->is_csc(idh) ) m_hole_techn[m_nHole] = 3; 
	  m_hole_station_name[m_nHole] = m_mdtIdHelper->stationName(idh);
	  m_hole_station_eta[m_nHole] = m_mdtIdHelper->stationEta(idh);
	  m_hole_station_phi[m_nHole] = m_mdtIdHelper->stationPhi(idh);
	  for (int is=0; is < m_nSimHit; is++) {
	    Identifier ids(m_sim_hit_id[is]);
	    if ( m_sim_hit_techn[is]==m_hole_techn[m_nHole] && layerMatch(ids,idh) ) { 
	      m_hole_match[m_nHole] = is;
	      m_sim_hole_match[is] = m_nHole;
	    }
	  }   
	}
	if (m_hole_match[m_nHole]<0) looseEnds->push_back((*holes)[ih]);
	if (m_hole_match[m_nHole]<0) unmatched_hole++;
	m_nHole++;
      }
      // create a new track containing unmatched holes & unmatched simulated hits to help to see
      // what goes wrong
      // add unmatched simulated hits
      /*
	if (m_rec_match[itr-(*tracks).begin()]>-1) {
	int simIndex = m_rec_match[itr-(*tracks).begin()];
	for (unsigned int isim=0;isim<m_nSimHit; isim++) {
	if ( m_sim_hit_index[isim] == simIndex && m_sim_hole_match[isim]<0 ) {
	// create fake rio ? 
        }
	}
	}
      */
      Trk::Track* unmatched = new Trk::Track((*itr)->info(),looseEnds, new Trk::FitQuality());   
      looseEndsColl->push_back(unmatched);
    }
  }

  ATH_MSG_DEBUG("fillHoles: number of simHit, holes, in this event:" << m_nSimHit <<"," << m_nHole );
  ATH_MSG_DEBUG("fillHoles: number of unmatched holes, in this event:" << unmatched_hole );

  // record the new track collection
  StatusCode sc = m_StoreGate->record(looseEndsColl,"LooseEnds");
  if (sc.isFailure()){
    ATH_MSG_ERROR("New Track Container could not be recorded in StoreGate !");   
  } else {
    ATH_MSG_INFO("LooseEnds recorded in StoreGate");   
  }

  // alternative way of generating holes : create new track with holes
  // create new collection of tracks to write in storegate
 
  TrackCollection* newTracks = new TrackCollection;
  for (TrackCollection::const_iterator itr  = (*tracks).begin(); itr < (*tracks).end(); itr++){
    const Trk::Track* new_track = m_holesTool->getTrackWithHoles(**itr,Trk::muon);
    if (new_track) newTracks->push_back(new Trk::Track(*new_track));
  }
  // record the new track collection
  sc = m_StoreGate->record(newTracks,"TracksWithHoles");
  if (sc.isFailure()){
    ATH_MSG_ERROR("New Track Container could not be recorded in StoreGate !");   
  } else {
    ATH_MSG_INFO("TracksWithHoles recorded in StoreGate");   
  }

  // loop over the collection to check hole parameters
  for (TrackCollection::const_iterator itr  = (*newTracks).begin(); itr < (*newTracks).end(); itr++){
    const DataVector<const Trk::TrackStateOnSurface>* recTSOS = (*itr)->trackStateOnSurfaces(); 
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = recTSOS->begin(); 
    for ( ;iter!= recTSOS->end(); iter++) {
      if ( (*iter)->type(Trk::TrackStateOnSurface::Hole) ) {
        if (!(*iter)->trackParameters()) ATH_MSG_ERROR("hole without parameters!");
        else ATH_MSG_DEBUG("hole at position: " << (*iter)->trackParameters()->position() );
      }
    }
  }


}

const TrackCollection* Muon::MuonTGHitNtuple::holesFromSim() const
{
  MsgStream log(msgSvc(), name());

  TrackCollection* htracks = 0;

  if ( !m_trackingGeometry ) {
    StatusCode sc = m_detStore->retrieve(m_trackingGeometry, m_trackingGeometryName);
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Could not find geometry "<< m_trackingGeometryName<<". Exiting.");
      return 0;
    } else {
      ATH_MSG_DEBUG("  geometry Svc \""<<m_trackingGeometryName<<"\" booked ");
    }
  }

  Amg::Vector3D origin(0.,0.,0.);
  if (m_nSim > 0) {
    htracks = new TrackCollection;
    for ( int isim = 0; isim < m_nSim; isim++ ) {
      // retrieve sim initial parameters
      Amg::Vector3D mom(m_sim_mom[isim][0],m_sim_mom[isim][1],m_sim_mom[isim][2]);
      Amg::Vector3D pos(m_sim_pos[isim][0],m_sim_pos[isim][1],m_sim_pos[isim][2]);
      // charge
      const HepPDT::ParticleData* ap = m_particleDataTable->particle(abs(m_sim_pdg[isim]));
      double q = ap->charge();
      if (m_sim_pdg[isim]<0) q=-q;
      Trk::Perigee* peri = new Trk::Perigee(pos,mom,q,origin);
      // get ordered sim layers
      const std::vector<std::pair< const Trk::Layer*,std::vector<Identifier> > >* simLayers = getOrderedSimLayers(m_sim_index[isim],mom);
      std::vector<std::pair< const Trk::Layer*,std::vector<Identifier> > >::const_iterator sIter = simLayers->begin();

      DataVector<const Trk::TrackStateOnSurface>*  holes= new DataVector<const Trk::TrackStateOnSurface>;
      // loop over active layers & create holes
      const Trk::TrackParameters* currPar = peri;
      while ( currPar ) {

	std::pair<const Trk::TrackParameters*,const Trk::Layer*> next = m_extrapolator->extrapolateToNextActiveLayer(*currPar,
               Trk::alongMomentum,true,Trk::nonInteracting);
        const Trk::TrackParameters* nextPar = next.first;
	const Trk::Layer* layer = next.second;
	if (!nextPar || !layer) break;

	// we are interested in muon layers only !
	if (layer) {
	  const Trk::DetachedTrackingVolume* detVol = layer->enclosingDetachedTrackingVolume();
	  if (!detVol) ATH_MSG_ERROR("MuonHolesOnTrackTool::getHolesOnTrack: no enclosing detached volume?!");
	  if (!detVol) layer=0;
	}
	if (!layer) {
	  ATH_MSG_ERROR("MuonTGHitNtuple::holesFromSim: no associated layer?!");
	} else {
          // compare with expected
          if ( layer != (*sIter).first && sIter+1 != simLayers->end() ) {
            if ( layer == (*(sIter+1)).first ) sIter++;
            else {
              Trk::DistanceSolution distSol = (*(sIter+1)).first->surfaceRepresentation().straightLineDistanceEstimate(nextPar->position(),nextPar->momentum().unit()); 
	      if ( distSol.numberOfSolutions() && distSol.first()<0.) {
		ATH_MSG_DEBUG("missed simLayer:current active:" << (*(sIter+1)).first->surfaceRepresentation().center() <<","
			  << nextPar->position() << "," << distSol.first() );
		const Trk::TrackParameters* repPar = m_extrapolator->extrapolate(*currPar,(*(sIter+1)).first->surfaceRepresentation(),
										 Trk::alongMomentum,false,Trk::muon) ;
		if (repPar) {
		  nextPar = repPar;
                  layer = (*(sIter+1)).first;
		  sIter++;
		}
	      }
	    }
	  }	    

          int assocMeas = 0;
          unsigned int lId = layer->layerType();
	  const Identifier layId(lId);
          if (m_mdtIdHelper->is_mdt(layId) ) {
	    bool measPhi = false;
	    const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	    if (hole) holes->push_back(hole); 
	  } else {
	    if (m_tgcIdHelper->is_tgc(layId) ) {
	      if (lId > 0 && m_tgcIdHelper->gasGap(layId)==2 && m_tgcIdHelper->gasGapMax(layId)==3) assocMeas+=2;
	      if (lId == 0) std::cout << "TGC layer not identified, this phi hole can be fake" << std::endl;
	    }
	    if (assocMeas<2){
	      bool measPhi = true;
	      const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	      if (hole) holes->push_back(hole); 
	    }
	    if (assocMeas==0 || assocMeas==2){
	      bool measPhi = false;
	      const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	      if (hole) holes->push_back(hole); 
	    }
	  }
	}
	currPar=nextPar;
      }
      // create track
      htracks->push_back(new Trk::Track(Trk::TrackInfo(Trk::TrackInfo::Unknown,Trk::muon),holes, new Trk::FitQuality(0,0)));  
      delete simLayers;
    }
  }

  return htracks;
}

const Trk::TrackStateOnSurface* Muon::MuonTGHitNtuple::createHole(const Trk::TrackParameters* nextPar,
								       const Trk::Layer* layer,
								       bool measPhi) const
{
  const Trk::TrackStateOnSurface* hole = 0;
  const Trk::MaterialEffectsBase* dummy_sa=0;  
  const Trk::RIO_OnTrack* dummy_rio=0;
  const Trk::FitQualityOnSurface* dummy_fitQuality=0;
  const Trk::TrackParameters* hPar = 0;
  bool inRange = true;
  if ( m_identifyHoles &&  m_measTool ) {
    double pitch;
    Identifier refId = m_measTool->nearestDetEl(layer,nextPar,measPhi,pitch);
    if ( refId.get_identifier32().get_compact() > 0 ) {
      hPar = m_measTool->layerToDetEl(layer,nextPar,refId);
    } else {
      inRange = false;  
    }
  } else {
    hPar = nextPar->clone();
  }
  if (inRange && hPar) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
    typePattern.set(Trk::TrackStateOnSurface::Hole);       		
    hole=new Trk::TrackStateOnSurface(dummy_rio,hPar,dummy_fitQuality,dummy_sa,typePattern); 
  }
  return hole;
}

bool Muon::MuonTGHitNtuple::layerMatch(Identifier id1, Identifier id2) const
{  
  if (   m_mdtIdHelper->stationName(id1) ==   m_mdtIdHelper->stationName(id2)
      && m_mdtIdHelper->stationEta(id1)  ==   m_mdtIdHelper->stationEta(id2)
      && m_mdtIdHelper->stationPhi(id1)  ==   m_mdtIdHelper->stationPhi(id2) ) {

    if (   m_mdtIdHelper->is_mdt(id1) &&  m_mdtIdHelper->is_mdt(id2)  ) {
      if   (( m_mdtIdHelper->multilayer(id1)== m_mdtIdHelper->multilayer(id2) )
	   && ( m_mdtIdHelper->tubeLayer(id1) == m_mdtIdHelper->tubeLayer(id2)) ) {
	return true; 
      } else {
        return false;
      }
    }
    if  (   m_rpcIdHelper->is_rpc(id1) &&  m_rpcIdHelper->is_rpc(id2) ) {
      if ( m_rpcIdHelper->doubletR(id1) == m_rpcIdHelper->doubletR(id2)
	   && m_rpcIdHelper->doubletZ(id1) == m_rpcIdHelper->doubletZ(id2)
	   && m_rpcIdHelper->gasGap(id1)   == m_rpcIdHelper->gasGap(id2)
	   && m_rpcIdHelper->measuresPhi(id1) == m_rpcIdHelper->measuresPhi(id2) ) 
	return true;
      else 
	return false;
    }
    if (    m_tgcIdHelper->is_tgc(id1) == m_tgcIdHelper->is_tgc(id2) ) {
      if (  m_tgcIdHelper->gasGap(id1) == m_tgcIdHelper->gasGap(id2)
	    && m_tgcIdHelper->isStrip(id1) == m_tgcIdHelper->isStrip(id2)
            && m_muonMgr->getTgcReadoutElement(id1) == m_muonMgr->getTgcReadoutElement(id2) )  
	return true;
      else 
        return false;
    }
  }
  return false;
}

const std::vector<std::pair<const Trk::Layer*,std::vector<Identifier> > >* Muon::MuonTGHitNtuple::getOrderedSimLayers(int index, Amg::Vector3D mom) const
{
  const std::vector<std::pair<const Trk::Layer*,std::vector<Identifier> > >* simLayers = 0;

  std::vector<std::pair<const Trk::Layer*,std::vector<Identifier> > > lays;
  std::vector<std::pair<const Trk::Layer*,std::vector<Identifier> > >::iterator lIter;

  for (int is=0; is < m_nSimHit; is++) {
    if (m_sim_hit_index[is]==index) {
      Identifier ids(m_sim_hit_id[is]);
     
      if (m_mdtIdHelper->is_mdt(ids)) { 
	//Get the MdtReadoutElement and the tube position from it
	const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(ids);			
	Amg::Vector3D pos = mdtROE->tubePos(ids);
        // associated MTG layer
        const Trk::Layer* lay = m_trackingGeometry->associatedLayer(pos);
        if (!lay ) std::cout << "ERROR : no MTG associated layer for MDT sim hit at position " << pos << std::endl;
        else {
          bool found = false;
          for (lIter=lays.begin(); lIter!=lays.end(); lIter++) {
            if (lay==(*lIter).first) { found = true; (*lIter).second.push_back(ids); break; }
	    Trk::DistanceSolution distSol = (*lIter).first->surfaceRepresentation().straightLineDistanceEstimate(pos,mom.unit()); 
            if ( distSol.numberOfSolutions() && distSol.first()>0.) {
	      std::vector<Identifier> hits;
              hits.push_back(ids);
              lIter = lays.insert(lIter,std::pair<const Trk::Layer*, std::vector<Identifier> > (lay,hits) ); found = true; break;
	    }    
	  }
          if (!found) {
	    std::vector<Identifier> hits;
	    hits.push_back(ids);
	    lays.push_back(std::pair<const Trk::Layer*, std::vector<Identifier> > (lay,hits));
	  }
        }
      } 
     
      if (m_rpcIdHelper->is_rpc(ids)) { 
	//Get the RpcReadoutElement and the strip position from it
	const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(ids);			
	Amg::Vector3D pos = rpcROE->stripPos(ids);
        // associated MTG layer
        const Trk::Layer* lay = m_trackingGeometry->associatedLayer(pos);
        if (!lay ) std::cout << "ERROR : no MTG associated layer for RPC sim hit at position " << pos << std::endl;
        else {
          bool found = false;
          for (lIter=lays.begin(); lIter!=lays.end(); lIter++) {
            if (lay==(*lIter).first) { found = true; (*lIter).second.push_back(ids); break; }
	    Trk::DistanceSolution distSol = (*lIter).first->surfaceRepresentation().straightLineDistanceEstimate(pos,mom.unit()); 
            if ( distSol.numberOfSolutions() && distSol.first()>0.) {
	      std::vector<Identifier> hits;
              hits.push_back(ids);
              lIter = lays.insert(lIter,std::pair<const Trk::Layer*, std::vector<Identifier> > (lay,hits) ); found = true; break;
	    }    
	  }
          if (!found) {
	    std::vector<Identifier> hits;
	    hits.push_back(ids);
	    lays.push_back(std::pair<const Trk::Layer*, std::vector<Identifier> > (lay,hits));
	  }
        }
      } 
     
      if (m_tgcIdHelper->is_tgc(ids)) { 
	//Get the TgcReadoutElement and the strip position from it
	const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(ids);			
	Amg::Vector3D pos = tgcROE->channelPos(ids);
        // associated MTG layer
        const Trk::Layer* lay = m_trackingGeometry->associatedLayer(pos);
        if (!lay ) std::cout << "ERROR : no MTG associated layer for TGC sim hit at position " << pos << std::endl;
        else {
          bool found = false;
          for (lIter=lays.begin(); lIter!=lays.end(); lIter++) {
            if (lay==(*lIter).first) { found = true; break; }
	    Trk::DistanceSolution distSol = (*lIter).first->surfaceRepresentation().straightLineDistanceEstimate(pos,mom.unit()); 
            if ( distSol.numberOfSolutions() && distSol.first()>0.) {
 	      std::vector<Identifier> hits;
              hits.push_back(ids);
              lIter = lays.insert(lIter,std::pair<const Trk::Layer*, std::vector<Identifier> > (lay,hits) ); found = true; break;
	    }    
	  }
          if (!found) {
	    std::vector<Identifier> hits;
	    hits.push_back(ids);
	    lays.push_back(std::pair<const Trk::Layer*, std::vector<Identifier> > (lay,hits));
	  }
        }
      } 
    }
  }

  //for (unsigned int i=0;i<lays.size();i++) std::cout << "ordered sim layers:"<<i<<","<<lays[i].first->surfaceRepresentation().center()<<std::endl; 
  return simLayers = new std::vector<std::pair<const Trk::Layer*,std::vector<Identifier> > >(lays);
}

Identifier Muon::MuonTGHitNtuple::getRpcId(const RPCSimHit* hit) const
{
  const int id = hit->RPCid();
  std::string stationName =m_rpcHelper->GetStationName(id);
  int stationEta = m_rpcHelper->GetZSector(id);
  int stationPhi  = m_rpcHelper->GetPhiSector(id);
  int doubletR = m_rpcHelper->GetDoubletR(id);
  int doubletZ = m_rpcHelper->GetDoubletZ(id);
  int doubletPhi = m_rpcHelper->GetDoubletPhi(id);
  int gasGap = m_rpcHelper->GetGasGapLayer(id);
  int measPhi = m_rpcHelper->GetMeasuresPhi(id);

  const Identifier id1 = m_rpcIdHelper->channelID(stationName, stationEta, stationPhi, doubletR,
                                                        doubletZ, doubletPhi,gasGap, measPhi, 1);
  // this id is sufficient to define surface (for extrapolation et cet.)
  return id1;

  // identify channel
//  const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(id1);
//
//  int  nStrips = rpcROE->Nstrips(measPhi);  
//  const Identifier idN = m_rpcIdHelper->channelID(stationName, stationEta, stationPhi, doubletR,
//                                                        doubletZ, doubletPhi,gasGap, measPhi, nStrips);
//
//  const Amg::Vector3D loc1 = rpcROE->globalToLocalCoords(rpcROE->stripPos(id1),id1);
//  const Amg::Vector3D locN = rpcROE->globalToLocalCoords(rpcROE->stripPos(idN),idN);
//
//  int strip = 0;
//  float pitch = measPhi ? (locN[1]-loc1[1])/fmax(1,nStrips-1) : (locN[2]-loc1[2])/fmax(1,nStrips-1); 
//  double dstrip = measPhi ? ((hit->localPosition()[1]-loc1[1])/pitch+0.5) :
//                            ((hit->localPosition()[2]-loc1[2])/pitch+0.5) ;
//  strip = dstrip>=0. ? int(dstrip)+1 : 0;  
//  
//  if (strip<1 || strip > nStrips) return Identifier(0);
//
//  const Identifier idHit = m_rpcIdHelper->channelID(stationName, stationEta, stationPhi, doubletR,
//						    doubletZ, doubletPhi,gasGap, measPhi, strip);
//
//  return idHit;
}

Identifier Muon::MuonTGHitNtuple::getCscId( const CSCSimHit* hit) const
{
  const int id = hit->CSCid();
  std::string stationName =m_cscHelper->GetStationName(id);
  int stationEta = m_cscHelper->GetZSector(id);
  int stationPhi  = m_cscHelper->GetPhiSector(id);
  int chamberLayer = m_cscHelper->GetChamberLayer(id);
  int wireLayer = m_cscHelper->GetWireLayer(id);

  const Identifier idr = m_cscIdHelper->channelID(stationName, stationEta, stationPhi, chamberLayer,
                                                 wireLayer,0, 1);
  // this id is sufficient to define surface (for extrapolation et cet.)
  return idr;
}

Identifier Muon::MuonTGHitNtuple::getTgcId( const TGCSimHit* hit) const
{
  const int id = hit->TGCid();
  std::string stationName =m_tgcHelper->GetStationName(id);
  int stationEta = m_tgcHelper->GetStationEta(id);
  int stationPhi  = m_tgcHelper->GetStationPhi(id);
  int gasGap = m_tgcHelper->GetGasGap(id);

  const Identifier idr = m_tgcIdHelper->channelID(stationName, stationEta, stationPhi, gasGap, 0, 1);
  // this id is sufficient to define surface (for extrapolation et cet.)
  return idr;
}
