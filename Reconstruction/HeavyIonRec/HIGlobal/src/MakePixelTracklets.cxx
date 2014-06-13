/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HIGlobal/MakePixelTracklets.h"
#include "HIGlobal/EtaPhiBins.h"
#include "HIGlobal/HIPixelTracklets.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <cmath>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"


#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGate.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "VxVertex/VxContainer.h"
#include "CLHEP/Geometry/Point3D.h"

#include "InDetIdentifier/PixelID.h"

//#include "InDetRecInput/InDetRecInputCollection.h"
//#include "InDetRecInput/InDetRecInputContainer.h"
//#include "InDetRecInput/InDetRecInputCLASS_DEF.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"

#include "GaudiKernel/AlgFactory.h"

#include "TMath.h"

#include "Particle/TrackParticleContainer.h"

#include "TNtuple.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include "TruthHelper/IsGenStable.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloEvent/CaloCell.h"

#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>
#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Geometry/Vector3D.h>
#include <CLHEP/Geometry/Point3D.h>

// Create a factory for this algorithm.
// static const AlgFactory<MakePixelTracklets> s_factory;
// const IAlgFactory& MakePixelTracklets_AlgFactory = s_factory; 

MakePixelTracklets::MakePixelTracklets(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),   
  m_iBeamCondSvc("BeamCondSvc",name)
{  

  // Declare the properties
  declareProperty("PixelClusterContainerName", 
		  m_PixelClustersName="PixelClusters");
  declareProperty("CaloClusterContainerName",
		  m_CaloClustersContainerName="LArClusterEM");
  declareProperty("EtaCut", m_etaCut=0.01);
  declareProperty("PhiCut", m_phiCut=0.08);
  declareProperty("EtaMatch",m_etaMatch=0.005);
  declareProperty("PhiMatch",m_phiMatch=0.04);
  declareProperty("VrtPhiCut", vrt_phi_cut=0.08);
  declareProperty("VrtZCut", vrt_z_cut=1.4);
  declareProperty("EtaMax", m_etaMax=2.);
  declareProperty("NtupleLoc", m_ntupleLoc = "AANT");
  declareProperty("MatchR", m_matchR=0.15);
  declareProperty("PtPhi",m_ptphi=0.0111);
  declareProperty("doStartingPoint3",m_doStartingPoint3=true);
  declareProperty("McAvailable",m_mc_available=false);
  declareProperty("doHolesEfficiency", m_doHolesEfficiency=false);
  declareProperty("PRDTruthCollectionPixel",m_multiTruthCollectionPixelName = "PRD_MultiTruthPixel");
  declareProperty("pixelTrackletsCollection",m_pixelTrackletsCollection = "pixelTrackletsCollection");


    sim_vz=0;
  sim_vy=0;
  sim_vx=0;
  ntrT=0;
  ntrF=0;
  ntr=0;
  nCalo=0;
  n2=0;
  n1=0;
  n0=0;
  multisatisfaction=0;
  m_numberOfEvents=0;
  m_eventNumber=0;
  event_id_run_number=0;
  event_id_lumiBlock=0;
  event_id_evt_number=0;
  event_id_bc_id=0;
  NumVrtRec=0;
  NumVrtFinder=0;
  vz_finder=0;
  vz=0;
  vy_ref=0;
  vy=0;
  vx_ref=0;
  vx=0;

  detStore = 0;
  effTree0 = 0;
  effTree1 = 0;
  m_Ntuple = 0;
  m_StoreGate = 0;
  m_caloCellHelper = 0;
  m_caloMgr = 0;
  m_calo_dd_man = 0;
  m_clusterCont = 0;
  m_particleDataTable = 0;
  m_tesIO = 0;
  m_ths = 0;
  m_tileID = 0;
  m_truthCollectionPixel = 0;
  pixelID = 0;
  trackletsTree = 0;

  NewVectors();
  
}

MakePixelTracklets::~MakePixelTracklets() 
{
  
  DeleteNewVectors();   
  
}

StatusCode MakePixelTracklets::initialize() {
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "In initialize()" << endreq;
  StatusCode sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  sc = service("THistSvc",m_ths);
  if (sc.isFailure()) {
    log << MSG::FATAL << "THistSvc service not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  
  //get the Particle properties service
  IPartPropSvc* partPropSvc;
  sc = service("PartPropSvc",partPropSvc,true);
  if(sc.isFailure())
    {
      log << MSG::FATAL << "Particle Properties Service not found!" << endreq;
      return StatusCode::FAILURE;
    }
  m_particleDataTable = partPropSvc->PDT();

  // Removed for sake of AtlasReconstruction
  if( m_mc_available )
    m_tesIO = new TruthHelper::GenAccessIO();


  if( m_iBeamCondSvc.retrieve().isFailure()){
    log << MSG::DEBUG << "Could not find BeamCondSvc." << endreq;
    return StatusCode::FAILURE;
  }
  //pointer to detector manager
  m_calo_dd_man = CaloDetDescrManager::instance();
  
  //TFile* m_file = new TFile("make_pixel_tracklets.root","RECREATE");
  //m_file->cd();
  multisatisfaction=0;
  //initialize dead areas;
  std::vector<int> etaPhiModule0[8];
  // etaPhiModule0[0].push_back(-5);
  //etaPhiModule0[0].push_back(6);
  etaPhiModule0[0].push_back(-4);
  etaPhiModule0[0].push_back(5);
  etaPhiModule0[1].push_back(-3);
  etaPhiModule0[1].push_back(6);
  etaPhiModule0[2].push_back(0);
  etaPhiModule0[2].push_back(14);
  etaPhiModule0[3].push_back(1);
  etaPhiModule0[3].push_back(7);
  etaPhiModule0[4].push_back(1);
  etaPhiModule0[4].push_back(8);
  //etaPhiModule0[5].push_back(2);
  //etaPhiModule0[5].push_back(18);
  etaPhiModule0[6].push_back(3);
  etaPhiModule0[6].push_back(6);
  etaPhiModule0[7].push_back(3);
  etaPhiModule0[7].push_back(13);
  std::vector<int> etaPhiModule1[6];
  etaPhiModule1[0].push_back(-5);
  etaPhiModule1[0].push_back(27);
  //etaPhiModule1[1].push_back(-3);
  // etaPhiModule1[1].push_back(9);
  etaPhiModule1[1].push_back(-3);
  etaPhiModule1[1].push_back(34);
  //etaPhiModule1[3].push_back(-2);
  //etaPhiModule1[3].push_back(9);
  etaPhiModule1[2].push_back(-1);
  etaPhiModule1[2].push_back(18);
  etaPhiModule1[3].push_back(0);
  etaPhiModule1[3].push_back(8);
  //etaPhiModule1[6].push_back(1);
  //etaPhiModule1[6].push_back(10);
  //etaPhiModule1[7].push_back(1);
  //etaPhiModule1[7].push_back(24);
  etaPhiModule1[4].push_back(1);
  etaPhiModule1[4].push_back(27);
  //log << MSG::DEBUG << "test. " << endreq;
  for(int idead=0; idead<5; idead++){
    deadLayer0.push_back(etaPhiModule0[idead]);
    deadLayer1.push_back(etaPhiModule1[idead]);
  }
  

  //output tree for tracklets
  trackletsTree = new TTree("trackletsTree","tracklet parameters");
  trackletsTree->Branch("NumVrtRec",&NumVrtRec);
  trackletsTree->Branch("NumVrtFinder",&NumVrtFinder);
  trackletsTree->Branch("vz_finder",&vz_finder);
  trackletsTree->Branch("event_id_run_number",&event_id_run_number);
  trackletsTree->Branch("event_id_evt_number",&event_id_evt_number);
  trackletsTree->Branch("event_id_lumiBlock",&event_id_lumiBlock);
  trackletsTree->Branch("event_id_bc_id",&event_id_bc_id);
  trackletsTree->Branch("matched",&matched);
  trackletsTree->Branch("matchedNumber",&matchedNumber);
  trackletsTree->Branch("matchedToWeakDecay",&matchedToWeakDecay);
  trackletsTree->Branch("startingPoint",&startingPoint);
  trackletsTree->Branch("threeLayerTrk",&threeLayerTrk);
  trackletsTree->Branch("deleted",&deleted);
  trackletsTree->Branch("deletedBecauseOf",&deletedBecauseOf);
  trackletsTree->Branch("vx",&vx);
  trackletsTree->Branch("vy",&vy);
  trackletsTree->Branch("vz",&vz);
  trackletsTree->Branch("sim_vx",&sim_vx);
  trackletsTree->Branch("sim_vy",&sim_vy);
  trackletsTree->Branch("sim_vz",&sim_vz);
  trackletsTree->Branch("x0_all",&x0_all);
  trackletsTree->Branch("y0_all",&y0_all);
  trackletsTree->Branch("z0_all",&z0_all);
  trackletsTree->Branch("x1_all",&x1_all);
  trackletsTree->Branch("y1_all",&y1_all);
  trackletsTree->Branch("z1_all",&z1_all);
  trackletsTree->Branch("x0",&x0_tr);
  trackletsTree->Branch("y0",&y0_tr);
  trackletsTree->Branch("z0",&z0_tr);
  trackletsTree->Branch("x1",&x1_tr);
  trackletsTree->Branch("y1",&y1_tr);
  trackletsTree->Branch("z1",&z1_tr);
  trackletsTree->Branch("ntr",&ntr);
  trackletsTree->Branch("multip",&multipTrks);
  trackletsTree->Branch("ntrF",&ntrF);
  trackletsTree->Branch("ntrT",&ntrT);
  trackletsTree->Branch("n0",&n0);
  trackletsTree->Branch("n1",&n1);
  trackletsTree->Branch("n2",&n2);
  trackletsTree->Branch("pt",&pt_tr);
  trackletsTree->Branch("p",&p_tr);
  trackletsTree->Branch("px",&px_tr);
  trackletsTree->Branch("py",&py_tr);
  trackletsTree->Branch("pz",&pz_tr);
  trackletsTree->Branch("part_id",&partid_tr);
  trackletsTree->Branch("parent_id",&parentid_tr);
  trackletsTree->Branch("eta0_all",&eta0_all);
  trackletsTree->Branch("phi0_all",&phi0_all);
  trackletsTree->Branch("eta1_all",&eta1_all);
  trackletsTree->Branch("phi1_all",&phi1_all);
  trackletsTree->Branch("eta2_all",&eta2_all);
  trackletsTree->Branch("phi2_all",&phi2_all);
  trackletsTree->Branch("dedx0_all",&dedx0_all);
  trackletsTree->Branch("dedx1_all",&dedx1_all);
  trackletsTree->Branch("dedx2_all",&dedx2_all);
  trackletsTree->Branch("pathLength0_all",&pathLength0_all);
  trackletsTree->Branch("pathLength1_all",&pathLength1_all);
  trackletsTree->Branch("pathLength2_all",&pathLength2_all);
  trackletsTree->Branch("csize0_all",&csize0_all);
  trackletsTree->Branch("csize1_all",&csize1_all);
  trackletsTree->Branch("ccol0_all",&ccol0_all);
  trackletsTree->Branch("ccol1_all",&ccol1_all);
  trackletsTree->Branch("crow0_all",&crow0_all);
  trackletsTree->Branch("crow1_all",&crow1_all);
  trackletsTree->Branch("crphi0_all",&crphi0_all);
  trackletsTree->Branch("crphi1_all",&crphi1_all);
  trackletsTree->Branch("cz0_all",&cz0_all);
  trackletsTree->Branch("cz1_all",&cz1_all);
  trackletsTree->Branch("eta0",&eta0_tr);
  trackletsTree->Branch("phi0",&phi0_tr);
  trackletsTree->Branch("eta1",&eta1_tr);
  trackletsTree->Branch("phi1",&phi1_tr);
  trackletsTree->Branch("csize0",&csize0_tr);
  trackletsTree->Branch("csize1",&csize1_tr);
  trackletsTree->Branch("ccol0",&ccol0_tr);
  trackletsTree->Branch("ccol1",&ccol1_tr);
  trackletsTree->Branch("crow0",&crow0_tr);
  trackletsTree->Branch("crow1",&crow1_tr);
  trackletsTree->Branch("crphi0",&crphi0_tr);
  trackletsTree->Branch("crphi1",&crphi1_tr);
  trackletsTree->Branch("cz0",&cz0_tr);
  trackletsTree->Branch("cz1",&cz1_tr);
  trackletsTree->Branch("dedx0",&dedx0_tr);
  trackletsTree->Branch("dedx1",&dedx1_tr);
  trackletsTree->Branch("pathLength0",&pathLength0_tr);
  trackletsTree->Branch("pathLength1",&pathLength1_tr);
  trackletsTree->Branch("eta0F",&eta0F_tr);
  trackletsTree->Branch("phi0F",&phi0F_tr);
  trackletsTree->Branch("eta1F",&eta1F_tr);
  trackletsTree->Branch("phi1F",&phi1F_tr);
  trackletsTree->Branch("deta",&deta_tr);
  trackletsTree->Branch("dphi",&dphi_tr);
  trackletsTree->Branch("detaF",&detaF_tr);
  trackletsTree->Branch("dphiF",&dphiF_tr);
  trackletsTree->Branch("etaT",&etaT);
  trackletsTree->Branch("phiT",&phiT);
  trackletsTree->Branch("ptT",&ptT);
  trackletsTree->Branch("pT",&pT);
  trackletsTree->Branch("pxT",&pxT);
  trackletsTree->Branch("pyT",&pyT);
  trackletsTree->Branch("pzT",&pzT);
  trackletsTree->Branch("partT_id",&partTid);
  trackletsTree->Branch("nparentT",&nparentT);
  trackletsTree->Branch("parentT_id",&parentTid);
  trackletsTree->Branch("ganged0",&ganged0_tr);
  trackletsTree->Branch("ganged1",&ganged1_tr);
  trackletsTree->Branch("nPixelClustersT",&nPixelClustersT);
  trackletsTree->Branch("nBLayerClustersT",&nBLayerClustersT);
  trackletsTree->Branch("x0_true",&x0_true);
  trackletsTree->Branch("y0_true",&y0_true);
  trackletsTree->Branch("z0_true",&z0_true);
  trackletsTree->Branch("x1_true",&x1_true);
  trackletsTree->Branch("y1_true",&y1_true);
  trackletsTree->Branch("z1_true",&z1_true);
  trackletsTree->Branch("eta0_true",&eta0_true);
  trackletsTree->Branch("phi0_true",&phi0_true);
  trackletsTree->Branch("eta1_true",&eta1_true);
  trackletsTree->Branch("phi1_true",&phi1_true);
  trackletsTree->Branch("deta_true",&deta_true);
  trackletsTree->Branch("dphi_true",&dphi_true);
  //trackletsTree->Branch("pdgid",&pdgid);
  trackletsTree->Branch("prod_vrtxT",&prod_vrtxT);
  trackletsTree->Branch("prod_vrtyT",&prod_vrtyT);
  trackletsTree->Branch("prod_vrtzT",&prod_vrtzT);
  trackletsTree->Branch("primaryDecayLabel",&primaryDecayLabel);
  trackletsTree->Branch("unassociated0_layer0",&unassociated0[0]);
  trackletsTree->Branch("unassociated0_layer1",&unassociated0[1]);
  trackletsTree->Branch("unassociated0_layer2",&unassociated0[2]);
  trackletsTree->Branch("unassociated1_layer0",&unassociated1[0]);
  trackletsTree->Branch("unassociated1_layer1",&unassociated1[1]);
  trackletsTree->Branch("unassociated1_layer2",&unassociated1[2]);
  trackletsTree->Branch("unassociated2_layer0",&unassociated2[0]);
  trackletsTree->Branch("unassociated2_layer1",&unassociated2[1]);
  trackletsTree->Branch("unassociated2_layer2",&unassociated2[2]);
  trackletsTree->Branch("nCalo",&nCalo);
  trackletsTree->Branch("eta_ca",&eta_ca);
  trackletsTree->Branch("phi_ca",&phi_ca);
  trackletsTree->Branch("ene_ca",&ene_ca);

  effTree0 = new TTree("effTree0","Efficiency Tree for detecting holes in layer 0");
  effTree0->Branch("NumVrtRec",&NumVrtRec);
  effTree0->Branch("event_id_run_number",&event_id_run_number);
  effTree0->Branch("event_id_evt_number",&event_id_evt_number);
  effTree0->Branch("event_id_lumiBlock",&event_id_lumiBlock);
  effTree0->Branch("event_id_bc_id",&event_id_bc_id);
  effTree0->Branch("projected",&projected_eff);
  effTree0->Branch("phiRef",&phi_eff);
  effTree0->Branch("etaRef",&eta_eff);
  effTree0->Branch("pt",&pt_eff);
  effTree1 = (TTree*)effTree0->Clone("effTree1");
  effTree1->SetTitle("Efficiency Tree for detectin holes in layer 1");
  effTree0->CopyAddresses(effTree1);
  
    
  sc = m_ths->regTree("/"+m_ntupleLoc+"/tracklets/trackletsTree",trackletsTree);
  if( sc==StatusCode::FAILURE ) return sc;
  sc = m_ths->regTree("/"+m_ntupleLoc+"/tracklets/effTree0",effTree0);
  if( sc==StatusCode::FAILURE ) return sc;
  sc = m_ths->regTree("/"+m_ntupleLoc+"/tracklets/effTree1",effTree1);
  
  return sc;
}

StatusCode MakePixelTracklets::finalize() {
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "In finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode MakePixelTracklets::execute() {
  StatusCode sc;
  MsgStream log(msgSvc(),name());
  sc = RetrievePixelClusters();
  if(sc == StatusCode::FAILURE) return sc;
  //sc = RetrieveCaloClusters();
  //if(sc == StatusCode::FAILURE) return sc;
  // get the det store
  
  sc = service("DetectorStore",detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Detector store not found !" << endreq;
    return StatusCode::FAILURE;
  } else {}

  //const InDetDD::SiDetectorManager* geoManager;
  
  sc=detStore->retrieve(pixelID,"PixelID");
  if( sc.isFailure())
    {
      log << MSG::FATAL << "PixelID helper not found !" << endreq;
      return StatusCode::FAILURE;
    }

  sc=detStore->retrieve(m_caloMgr);
  if(sc.isFailure()) {
    log << MSG::ERROR <<  "Unable to retrieve CaloIdManager from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  m_caloCellHelper = m_caloMgr->getCaloCell_ID();
  if (!m_caloCellHelper) {
    log << MSG::ERROR << "Could not access CaloCell_ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  
  // pixelID = dynamic_cast<const PixelID *>(geoManager->getIdHelper());

  const VxContainer *vxc;
  // Trk::RecVertex thisVx;

  sc = m_StoreGate->retrieve( vxc, "VxPrimaryCandidate" );
  if( sc.isFailure())
    {
      log << MSG::FATAL << "VxPrimaryCandidate not found !" << endreq;
      return StatusCode::FAILURE;
    }
  //some events may fail to find a vertex, how to take this into consideration??
  //excluding dummy vertex, from PUVertex.cxx
  VxContainer::const_iterator vxb = vxc->begin();
  VxContainer::const_iterator vxe = vxc->end();
  --vxe;
  //This event fails to reconstruct a vertex 
  if( vxb==vxe )
    {
      log <<MSG::INFO << "Event failed to produce a vertex!" << endreq;
      NumVrtRec = 0;
      //trackletsTree->Fill();
      if( m_doHolesEfficiency ){
	effTree0->Fill();
	effTree1->Fill();
      }
      //return StatusCode::SUCCESS;
    }
  else {
    thisVx = (*vxc)[0]->recVertex();
    //log <<MSG::DEBUG << "reconstructed vertex postion x,y,z: " << (*vxb)->recVertex().position().x() << "," << (*vxb)->recVertex().position().y() << "," << vz << endreq;
    NumVrtRec = vxc->size()-1;
  }
  vx = thisVx.position().x();                                    
  vy = thisVx.position().y(); 
  vz = thisVx.position().z();

  beamposition = m_iBeamCondSvc->beamVtx();
  vx_ref = beamposition.position().x();
  vy_ref = beamposition.position().y();
  
  //log << MSG::DEBUG << "beam postion x,y,z: " << beamposition.position().x() << "," << beamposition.position().y() << "," << beamposition.position().z() << endreq;

  NumVrtFinder = 0;
  ClearVectors();
  LayerClusters(); //the first two layer clusters now available
  // LayerClusters();
//   //my own version of z vertex finder
//   std::vector<double> zVrts;
//   if( NumVrtRec==0 ) {
//     ZVrtCollection(zVrts);
//     ZVrtFinder(zVrts);
//   }
 
  if( NumVrtRec!=0 ){
    std::vector<std::vector<PixelCluster> > outputTracklets[2];
    MakeTracklets(true,1,clusterLayer[0],clusterLayer[1],trackletsCol);
    for(int i=0; i<3; i++) {
      //log << MSG::DEBUG << "layer: " << i << endreq;
      //typedef std::map<int,int>::iterator mit;
      for(int ic=0; ic<(int)clusterLayer[i].size(); ic++) {
	if( usedClusterLayer[i][ic]==(int)0 ) {  //unassociated hits
	  (*unassociated0[i]).push_back(ic);
	}
      }
    }
    
    MakeTracklets(true,2,clusterLayer[0],clusterLayer[2],outputTracklets[0]);
    for(int i=0; i<3; i++) {
      //log << MSG::DEBUG << "layer: " << i << endreq;
      //typedef std::map<int,int>::iterator mit;
      for(int ic=0; ic<(int)clusterLayer[i].size(); ic++) {
	if( usedClusterLayer[i][ic]==0 ) {  //unassociated hits
	  (*unassociated1[i]).push_back(ic);
        }
      }
    }
    
    
    if( m_doStartingPoint3==true ) {
      MakeTracklets(true,3,clusterLayer[1],clusterLayer[2],outputTracklets[1]);
      for(int i=0; i<3; i++) {
	//log << MSG::DEBUG << "layer: " << i << endreq;
	//typedef std::map<int,int>::iterator mit;
	for(int ic=0; ic<(int)clusterLayer[i].size(); ic++) {
	  if( usedClusterLayer[i][ic]==0 ) {  //unassociated hits
	    (*unassociated2[i]).push_back(ic);
	  }
	}
      }
    }
    
    log << MSG::DEBUG << "01:02:12=" << trackletsCol.size() << ":" << outputTracklets[0].size() << ":" << outputTracklets[1].size() << endreq;
    std::vector<std::vector<PixelCluster> > cleanedTracklets[3];
    RemoveDuplicate(0,outputTracklets[0],trackletsCol,cleanedTracklets[0]);
   
    RemoveDuplicate(1,outputTracklets[1],trackletsCol,cleanedTracklets[1]);
    //should also check for the duplication from cleanedTracklets[0] and cleanedTracklets[1]????
    RemoveDuplicate(2,cleanedTracklets[1],cleanedTracklets[0],cleanedTracklets[2]);
    for(int il=0; il<3; il++) {
      if( il==1 ) continue;
      for(unsigned int it=0; it<cleanedTracklets[il].size(); it++) {
	trackletsCol.push_back(cleanedTracklets[il][it]);
	threeLayerTrkMap[trackletsCol.size()-1] = 0;
      }
    }
    log << MSG::DEBUG << "tracklets found by MakeTracklets(): " << trackletsCol.size() << endreq;
  }
  
  if( m_mc_available )
    {
      sc = RetrieveTruth();  if (sc.isFailure()) {return sc;} //AO
      TruthAssociation();
    }
  
  //AnaCaloClusters();
  sc = FillNTuple(); //with or without a reconstructed vertex
  if( sc = StatusCode::FAILURE ) return sc;
  if( NumVrtRec==1 && m_doHolesEfficiency )
    {
      CalEfficiency(0);
      CalEfficiency(1);
    }
  
  return StatusCode::SUCCESS;
}

StatusCode MakePixelTracklets::RetrievePixelClusters(){

    MsgStream log(msgSvc(), name());
    const DataHandle<EventInfo> eventInfo;
    StatusCode sc;
    sc = (m_StoreGate->retrieve(eventInfo));
    if (sc.isFailure()){
	log << MSG::FATAL << "could not get event"<<endreq;
	return StatusCode::FAILURE;
    }

    m_eventNumber= eventInfo->event_ID()->event_number();
    log<< MSG::DEBUG<< "event number " << m_eventNumber<<endreq;
    m_numberOfEvents++;

    // get space points from TDS
    sc = m_StoreGate->retrieve(m_clusters, m_PixelClustersName);
    if (sc.isFailure()){
      log << MSG::ERROR <<"Si cluster container not found"<< endreq;
    }
    else{ 
      log <<MSG::DEBUG <<"Si Cluster container found" <<endreq;
    }
    return sc;
    
}

void MakePixelTracklets::LayerClusters()
{
  MsgStream log(msgSvc(), name());
  InDet::PixelClusterContainer::const_iterator colNext = m_clusters->begin();
  InDet::PixelClusterContainer::const_iterator colEnd = m_clusters->end();
  for (; colNext!= colEnd; ++colNext)
    {
      
      InDet::PixelClusterCollection::const_iterator nextCluster = (*colNext)->begin();
      InDet::PixelClusterCollection::const_iterator lastCluster = (*colNext)->end();
      
      for (; nextCluster!=lastCluster; nextCluster++)
        {
          const PixelCluster& cluster = **nextCluster;
          //this is very important to make sure barrel layer cluster is used                   
          int barrel = pixelID->barrel_ec(cluster.detectorElement()->identify());
          if( barrel!=0 ) continue; //it must be an endcap           
	  int layer = pixelID->layer_disk(cluster.detectorElement()->identify());
	  std::vector<int> etaPhiModule;
	  etaPhiModule.push_back(pixelID->eta_module(cluster.detectorElement()->identify()));
	  etaPhiModule.push_back(pixelID->phi_module(cluster.detectorElement()->identify()));
	  //bool dead = false;
	  clusterLayer[layer].push_back(cluster);
	  
	}
    }
}

void MakePixelTracklets::MakeTracklets(bool trkoreff, int sumlayer, const std::vector<PixelCluster>& clusterColl0,const std::vector<PixelCluster>& clusterColl1, std::vector<std::vector<PixelCluster> > & outputTracklets)
{ 
  MsgStream log(msgSvc(),name());
  double etaCutUsed = m_etaCut;
  double phiCutUsed = m_phiCut;
  if( trkoreff==false )
    {
      etaCutUsed = m_etaMatch;
      phiCutUsed = m_phiMatch;
    }
  if(sumlayer==2) { phiCutUsed *= (122.5-50.5)/(88.5-50.5); etaCutUsed *= 1.6; }
  if(sumlayer==3) phiCutUsed *= (122.5-88.5)/(88.5-50.5);
  
  int nclus0 = clusterColl0.size();
  int nclus1 = clusterColl1.size();
  int firstLayer = sumlayer/3;
  int secondLayer = sumlayer-firstLayer;
  //log << MSG::DEBUG << "nclus0:nclus1 " << nclus0 << ":" << nclus1 << endreq;
  
  std::vector<std::vector<PixelCluster> > trackletsColCol;
  std::map<int,int> threeLayerTrkLabel; //label tracklets as three layer, only used for B-1 tracklets
  //do not allow the share of the second layer cluster
  //std::map<int, int> usedLabel;
  
  for(int i0=0; i0<nclus0; i0++)
    {
      if( usedClusterLayer[firstLayer][i0] ) continue;
      //double rmin = 9999.;
      
      const PixelCluster& clusi0 = clusterColl0[i0];
      double cxi0 = clusi0.globalPosition().x() - vx;
      double cyi0 = clusi0.globalPosition().y() - vy;
      double czi0 = clusi0.globalPosition().z() - vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      
      if(firstLayer==1) log << MSG::DEBUG << "innerIndex:" << i0 << endreq;
      std::vector<int> minclus1label;
      std::vector<double> rcan;
      std::vector<double> detaCan;
      for(int i1=0; i1<nclus1; i1++)
	{
	  if( usedClusterLayer[secondLayer][i1] ) {
	    continue;
	  }
	  const PixelCluster& clusi1 = clusterColl1[i1];
	  double cxi1 = clusi1.globalPosition().x() - vx;
	  double cyi1 = clusi1.globalPosition().y() - vy;
	  double czi1 = clusi1.globalPosition().z() - vz;
	  double phii1 = atan2(cyi1,cxi1);
	  double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
	  double thetai1 = atan2(ri1,czi1);
	  double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
	  
	  double deta = etai0 - etai1;
	  double dphi = phii0 - phii1;
	  
	  if( dphi>M_PI )  dphi -= 2*M_PI;
	  if( dphi<-M_PI ) dphi += 2*M_PI;
	  if(firstLayer==1 ) log << MSG::DEBUG << "outIndex:deta:dphi=" << i1 << ":" << deta << ":" << dphi << endreq;	
	  if( fabs(deta)<etaCutUsed && fabs(dphi)<phiCutUsed )
	    {
	      minclus1label.push_back(i1);
	      double r = sqrt(deta*deta/(etaCutUsed*etaCutUsed)+dphi*dphi/(phiCutUsed*phiCutUsed) );
	      rcan.push_back(r);
	      detaCan.push_back(deta);
	    }
	} //end of 2nd layer loop
      
      int minclus1can = -1;
      int multip = rcan.size();
      if( multip>1 ) multisatisfaction++;
      // for(unsigned int ir=0; ir<rcan.size(); ir++){
//       	if( rcan[ir] < rmin ){
//       	  rmin = rcan[ir];
//       	  minclus1can = minclus1label[ir];
//       	}
//       }
      //as Brian suggested, used the smallest deta instead of the smallest R
      double detaMin = 9999.0;
      for(unsigned int ir=0; ir<detaCan.size(); ir++) {
	if( fabs(detaCan[ir])<detaMin ) {
	  detaMin = fabs(detaCan[ir]);
	  minclus1can = minclus1label[ir];
	}
      }
      
      if( minclus1can == -1 ) continue;
      
      const PixelCluster& minClus = clusterColl1[minclus1can];
      usedClusterLayer[secondLayer][minclus1can]++;
      usedClusterLayer[firstLayer][i0]++;
      std::vector<PixelCluster> twoClusters;
      twoClusters.push_back(clusi0);
      twoClusters.push_back(minClus);
      
      if(0) log << MSG::DEBUG << "innerIndex:outIndex=" << i0 <<":" << minclus1can << endreq;
      trackletsColCol.push_back(twoClusters);

      //project to 2 layer if the tracklet comes out from the B-layer and 1 layer
      //frobid projected cluster used again and label the tracklets as three layer tracklets 
      if( sumlayer==1 ) {
	for(unsigned int i2=0; i2<clusterLayer[2].size(); i2++) {
	  const PixelCluster& clusi2 = clusterLayer[2][i2];
          double cxi2 = clusi2.globalPosition().x() - vx;
          double cyi2 = clusi2.globalPosition().y() - vy;
          double czi2 = clusi2.globalPosition().z() - vz;
          double phii2 = atan2(cyi2,cxi2);
          double ri2 = sqrt(cxi2*cxi2+cyi2*cyi2);
          double thetai2 = atan2(ri2,czi2);
          double etai2 = -TMath::Log(TMath::Tan(thetai2/2));
	  
          double deta = etai0 - etai2;
          double dphi = phii0 - phii2;
	  
          if( dphi>M_PI )  dphi -= 2*M_PI;
	  if( dphi<-M_PI ) dphi += 2*M_PI;

          if( fabs(deta)<2.0*etaCutUsed && fabs(dphi)<2.0*phiCutUsed*(122.5-50.5)/(88.5-50.5)){
	    if(0) log << MSG::DEBUG << "projection Index:" << i2 << endreq;
	    //three layer cluster
	    usedClusterLayer[2][i2]++;
	    threeLayerTrkLabel[trackletsColCol.size()-1]++;
	  }
	}
      }
    } //end of loop first layer
  
  std::map<int,int> deleteReasonMap;
  std::vector<std::vector<PixelCluster> > removedTrs;
  for(unsigned int ntrs=0; ntrs<trackletsColCol.size(); ntrs++ ) {
    std::vector<PixelCluster> twoClusters = trackletsColCol[ntrs];
    bool removed = false;
    for(unsigned int irm=0; irm<removedTrs.size(); irm++) {
      if( twoClusters[0].identify()==removedTrs[irm][0].identify() &&
	  twoClusters[1].identify()==removedTrs[irm][1].identify() ) {
	removed = true;
	break;
      }
    }
    if(removed) continue;
    for(unsigned int jtrs=ntrs+1; jtrs<trackletsColCol.size(); jtrs++) {
      int gangeLayer0 = 0;
      int gangeLayer1 = 0;
      int overlapLayer0 = 0;
      int overlapLayer1 = 0;
      GangedOrOverlap(twoClusters,trackletsColCol[jtrs],gangeLayer0, gangeLayer1, overlapLayer0, overlapLayer1);
      if( gangeLayer0==0 && gangeLayer1==0 && overlapLayer0==0 && overlapLayer1==0 ) continue;
      //decide which one to keep
      if( gangeLayer0 ) {
	if( (twoClusters[0].rdoList().size())<(trackletsColCol[jtrs][0].rdoList().size()) ) { removedTrs.push_back(twoClusters); deleteReasonMap[ntrs]=jtrs; }
	else { removedTrs.push_back(trackletsColCol[jtrs]); deleteReasonMap[jtrs] = ntrs; }
      } //ganged0
      else if ( gangeLayer1 ) {
	if( (twoClusters[1].rdoList().size())<(trackletsColCol[jtrs][1].rdoList().size()) ) { removedTrs.push_back(twoClusters); deleteReasonMap[ntrs]=jtrs; }
	else { removedTrs.push_back(trackletsColCol[jtrs]); deleteReasonMap[jtrs] = ntrs; }
      } //ganged1
      //else { removedTrs.push_back(trackletsColCol[jtrs]); deleteReasonMap[jtrs] = ntrs; }
      //change this for test
      else if( overlapLayer0 ) {
	const PixelCluster & clus00 = twoClusters[0];
	const PixelCluster & clus10 = trackletsColCol[jtrs][0];
	
	double cx00 = clus00.globalPosition().x() - vx;
	double cy00 = clus00.globalPosition().y() - vy;
	double r00 = sqrt(cx00*cx00+cy00*cy00);
	double cx10 = clus10.globalPosition().x() - vx;
        double cy10 = clus10.globalPosition().y() - vy;
        double r10 = sqrt(cx10*cx10+cy10*cy10);
	if( r00>r10 ) {
	  removedTrs.push_back(twoClusters); deleteReasonMap[ntrs]=jtrs;
	}
	else {
	  removedTrs.push_back(trackletsColCol[jtrs]); deleteReasonMap[jtrs] = ntrs;
	}
      } //overlap0
      else {
	const PixelCluster & clus01 = twoClusters[1];
	const PixelCluster & clus11 = trackletsColCol[jtrs][1];
	
	double cx01 = clus01.globalPosition().x() - vx;
	double cy01 = clus01.globalPosition().y() - vy;
	double r01 = sqrt(cx01*cx01+cy01*cy01);
	double cx11 = clus11.globalPosition().x() - vx;
        double cy11 = clus11.globalPosition().y() - vy;
        double r11 = sqrt(cx11*cx11+cy11*cy11);
	if( r01>r11 ) {
	  removedTrs.push_back(twoClusters); deleteReasonMap[ntrs]=jtrs;
	}
	else {
	  removedTrs.push_back(trackletsColCol[jtrs]); deleteReasonMap[jtrs] = ntrs;
	}
      } //overlap1
      
    }
    
  }
  
  for(unsigned int i=0; i<trackletsColCol.size(); i++)
    {
      std::vector<PixelCluster> testClusters = trackletsColCol[i];
      bool removed = false;
      for(unsigned int j=0; j<removedTrs.size(); j++)
	{
	  std::vector<PixelCluster> removedClusters = removedTrs[j];
	  if( testClusters[0].identify()==removedClusters[0].identify() && testClusters[1].identify()==removedClusters[1].identify() )
	    {
	      removed = true;
	      break;
	    }
	}
      //to study the effect of remove overlap to the calculation of fake rate
      //maybe temporary
      //tracklets found by 0-2, 1-2 layers are not considered yet, see execute()
      // outputTracklets.push_back(testClusters);
//       if( sumlayer==1 ) {
// 	if(removed) { (*deleted).push_back(1); (*deletedBecauseOf).push_back(deleteReasonMap[i]); }
// 	else { (*deleted).push_back(0); (*deletedBecauseOf).push_back(-1); }
//       }
      if( !removed ) {
       	outputTracklets.push_back(testClusters);
	if( sumlayer==1 ) {
	  threeLayerTrkMap[outputTracklets.size()-1] = threeLayerTrkLabel[i];
	}
      }
    }
}

void MakePixelTracklets::RemoveDuplicate(int layer02or12, const std::vector<std::vector<PixelCluster> > &outputTracklets1, const std::vector<std::vector<PixelCluster> >&outputTracklets0, std::vector<std::vector<PixelCluster> > &cleanedTracklets1) {
  //remove duplicate
  //layer02or12=0,02tracklets, layer02or12=1,12tracklets compared with 01tracklets
  //layer02or12=2,12tracklets compared with 02 tracklets
  std::vector<std::vector<PixelCluster> > removedTrs;
  for(unsigned int it=0; it<outputTracklets1.size(); it++) {
    bool removed = false;
    std::vector<PixelCluster> twoClusters = outputTracklets1[it];
    const PixelCluster* p12 = &twoClusters[0];
    if( layer02or12==2 ) p12 = &twoClusters[1];
    const PixelCluster& clus20 = *p12;
    double cx20 = clus20.globalPosition().x() - vx;
    double cy20 = clus20.globalPosition().y() - vy;
    double cz20 = clus20.globalPosition().z() - vz;
    double phi20 = atan2(cy20,cx20);
    double r20 = sqrt(cx20*cx20+cy20*cy20);
    double theta20 = atan2(r20,cz20);
    double eta20 = -TMath::Log(TMath::Tan(theta20/2));
    
    for(unsigned int ir=0; ir<outputTracklets0.size(); ir++) {
      const PixelCluster *aux = &outputTracklets0[ir][0];
      if( layer02or12==1 || layer02or12==2 ) aux = &outputTracklets0[ir][1];
      const PixelCluster& clus10 = *aux;
      double cx10 = clus10.globalPosition().x() - vx;
      double cy10 = clus10.globalPosition().y() - vy;
      double cz10 = clus10.globalPosition().z() - vz;
      double phi10 = atan2(cy10,cx10);
      double r10 = sqrt(cx10*cx10+cy10*cy10);
      double theta10 = atan2(r10,cz10);
      double eta10 = -TMath::Log(TMath::Tan(theta10/2));
      double deta = eta10 - eta20;
      double dphi = phi10 - phi20;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      if( fabs(deta)<m_etaCut && fabs(dphi)<m_phiCut ) {
	removed = true;
	break;
      }
    }
    if(removed) removedTrs.push_back(twoClusters);
  }
  for(unsigned int it=0; it<outputTracklets1.size(); it++) {
    std::vector<PixelCluster> testClusters = outputTracklets1[it];
    bool removed = false;
    for(unsigned int j=0; j<removedTrs.size(); j++)
      {
	std::vector<PixelCluster> removedClusters = removedTrs[j];
	if( testClusters[0].identify()==removedClusters[0].identify() && testClusters[1].identify()==removedClusters[1].identify() )
	  {
	    removed = true;
	    //log << MSG::DEBUG << " delete ids: " << removedClusters[0].identify() << ":" << removedClusters[1].identify() << endreq;
	    break;
	  }
      }
    if( !removed )
      cleanedTracklets1.push_back(testClusters);
  }
}

void MakePixelTracklets::GangedOrOverlap(const std::vector<PixelCluster> & tracklet1, const std::vector<PixelCluster> & tracklet2, int & gangeLayer0, int & gangeLayer1, int & overlapLayer0, int & overlapLayer1) {
  MsgStream log(msgSvc(), name());
  const PixelCluster& clus10 = tracklet1[0];
  const PixelCluster& clus11 = tracklet1[1];
  const PixelCluster& clus20 = tracklet2[0];
  const PixelCluster& clus21 = tracklet2[1];
  double cx10 = clus10.globalPosition().x() - vx;
  double cy10 = clus10.globalPosition().y() - vy;
  double cz10 = clus10.globalPosition().z() - vz;
  double phi10 = atan2(cy10,cx10);
  double r10 = sqrt(cx10*cx10+cy10*cy10);
  double theta10 = atan2(r10,cz10);
  double eta10 = -TMath::Log(TMath::Tan(theta10/2));
  double cx11 = clus11.globalPosition().x() - vx;
  double cy11 = clus11.globalPosition().y() - vy;
  double cz11 = clus11.globalPosition().z() - vz;
  double phi11 = atan2(cy11,cx11);
  double r11 = sqrt(cx11*cx11+cy11*cy11);
  double theta11 = atan2(r11,cz11);
  double eta11 = -TMath::Log(TMath::Tan(theta11/2));
  double cx20 = clus20.globalPosition().x() - vx;
  double cy20 = clus20.globalPosition().y() - vy;
  double cz20 = clus20.globalPosition().z() - vz;
  double phi20 = atan2(cy20,cx20);
  double r20 = sqrt(cx20*cx20+cy20*cy20);
  double theta20 = atan2(r20,cz20);
  double eta20 = -TMath::Log(TMath::Tan(theta20/2));
  double cx21 = clus21.globalPosition().x() - vx;
  double cy21 = clus21.globalPosition().y() - vy;
  double cz21 = clus21.globalPosition().z() - vz;
  double phi21 = atan2(cy21,cx21);
  double r21 = sqrt(cx21*cx21+cy21*cy21);
  double theta21 = atan2(r21,cz21);
  double eta21 = -TMath::Log(TMath::Tan(theta21/2));
  double deta = eta10 - eta20;
  double dphi = phi10 - phi20;
  if( dphi>M_PI )  dphi -= 2*M_PI;
  if( dphi<-M_PI ) dphi += 2*M_PI;
  double deta1 = eta11 - eta21;
  double dphi1 = phi11 - phi21;
  if( dphi1>M_PI )  dphi1 -= 2*M_PI;
  if( dphi1<-M_PI ) dphi1 += 2*M_PI;
  // int gangeLayer0 = 0;
  //   int overlapLayer0 = 0;
  //   int gangeLayer1 = 0;
  //   int overlapLayer1 = 0;
  if( fabs(deta)<m_etaCut && fabs(dphi)<m_phiCut && fabs(deta1)<m_etaCut && fabs(dphi1)<m_phiCut ) {
    //if( fabs(deta)<m_etaCut && fabs(dphi)<m_phiCut ) {  
    log << MSG::DEBUG << "eta0:eta1=" << eta10 << ":" << eta20 << endreq;
    log << MSG::DEBUG << "gange00:gange10=" << clus10.gangedPixel() << ":" << clus20.gangedPixel() << endreq;
    log << MSG::DEBUG << "gange01:gange11=" << clus11.gangedPixel() << ":" << clus21.gangedPixel() << endreq; 
    if(clus10.gangedPixel() && clus20.gangedPixel() ) gangeLayer0=1;
    if(clus11.gangedPixel() && clus21.gangedPixel() ) gangeLayer1=1;
    int phi10M = pixelID->phi_module(clus10.detectorElement()->identify()); 
    int eta10M = pixelID->eta_module(clus10.detectorElement()->identify());
    int phi11M = pixelID->phi_module(clus11.detectorElement()->identify());
    int eta11M = pixelID->eta_module(clus11.detectorElement()->identify());
    int phi20M = pixelID->phi_module(clus20.detectorElement()->identify());
    int eta20M = pixelID->eta_module(clus20.detectorElement()->identify());
    int phi21M = pixelID->phi_module(clus21.detectorElement()->identify());
    int eta21M = pixelID->eta_module(clus21.detectorElement()->identify());
    if( phi10M!=phi20M || eta10M!=eta20M ) overlapLayer0 = 1;
    if( phi11M!=phi21M || eta11M!=eta21M ) overlapLayer1 = 1;
    //if( gangeLayer0*gangeLayer1 || gangeLayer0*overlapLayer1 || overlapLayer0*gangeLayer1 || overlapLayer0*overlapLayer1 )
    // if( overlapLayer0 || gangeLayer0 || overlapLayer1 || gangeLayer1 ) {
//       gangoroverlap = 1;
//       log << MSG::DEBUG << "overlap or gang: " << gangoroverlap << endreq;
//     }
    //log << MSG::DEBUG << "overlap0:gang0 " << overlapLayer0 <<":"<< gangeLayer0 << " , " << "overlap1:gang1 " << overlapLayer1 << ":" << gangeLayer1 << endreq;
  }
}

StatusCode MakePixelTracklets::FillNTuple()
{
  MsgStream log(msgSvc(), name());
  //
  const DataHandle<EventInfo> eventInfo;
  StatusCode sc = m_StoreGate->retrieve(eventInfo);
  if (sc.isFailure()){
    log<<MSG::FATAL << "could not get event"<<endreq;
    return StatusCode::FAILURE;
  }

  event_id_run_number = eventInfo->event_ID()->run_number();
  event_id_evt_number = eventInfo->event_ID()->event_number();
  event_id_lumiBlock = eventInfo->event_ID()->lumi_block();
  event_id_bc_id = eventInfo->event_ID()->bunch_crossing_id();
  
  n0 = clusterLayer[0].size();
  n1 = clusterLayer[1].size();
  n2 = clusterLayer[2].size(); 
  for(int i0=0; i0<n0; i0++) {
    const PixelCluster& clusi0 = clusterLayer[0][i0];
    double cxi0 = clusi0.globalPosition().x() - vx;
    double cyi0 = clusi0.globalPosition().y() - vy;
    double czi0 = clusi0.globalPosition().z() - vz;
    double phii0 = atan2(cyi0,cxi0);
    double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
    double thetai0 = atan2(ri0,czi0);
    double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
    (*eta0_all).push_back(etai0);
    (*phi0_all).push_back(phii0);
    (*dedx0_all).push_back(clusi0.energyLoss());
    const InDetDD::SiDetectorElement* siDetectorElt0= clusi0.detectorElement();
    double pathLength = siDetectorElt0->thickness()/((siDetectorElt0->surface().normal().x()*cxi0+siDetectorElt0->surface().normal().y()*cyi0+siDetectorElt0->surface().normal().z()*czi0)/sqrt(cxi0*cxi0+cyi0*cyi0+czi0*czi0));
    (*pathLength0_all).push_back(pathLength);
    (*csize0_all).push_back(clusi0.rdoList().size());
    (*ccol0_all).push_back((clusi0.width().colRow())[0]);
    (*crow0_all).push_back((clusi0.width().colRow())[1]);
    (*crphi0_all).push_back((clusi0.width().widthPhiRZ())[0]);
    (*cz0_all).push_back((clusi0.width().widthPhiRZ())[1]);
    (*x0_all).push_back(clusi0.globalPosition().x());
    (*y0_all).push_back(clusi0.globalPosition().y());
    (*z0_all).push_back(clusi0.globalPosition().z());
  }      
  for(int i1=0; i1<n1; i1++) {
    const PixelCluster& clusi1 = clusterLayer[1][i1];
    double cxi1 = clusi1.globalPosition().x() - vx;
    double cyi1 = clusi1.globalPosition().y() - vy;
    double czi1 = clusi1.globalPosition().z() - vz;
    double phii1 = atan2(cyi1,cxi1);
    double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
    double thetai1 = atan2(ri1,czi1);
    double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
    (*eta1_all).push_back(etai1);
    (*phi1_all).push_back(phii1);
    (*dedx1_all).push_back(clusi1.energyLoss());
    const InDetDD::SiDetectorElement* siDetectorElt1= clusi1.detectorElement();
    double pathLength = siDetectorElt1->thickness()/((siDetectorElt1->surface().normal().x()*cxi1+siDetectorElt1->surface().normal().y()*cyi1+siDetectorElt1->surface().normal().z()*czi1)/sqrt(cxi1*cxi1+cyi1*cyi1+czi1*czi1));
    (*pathLength1_all).push_back(pathLength);
    (*csize1_all).push_back(clusi1.rdoList().size());
    (*ccol1_all).push_back((clusi1.width().colRow())[0]);
    (*crow1_all).push_back((clusi1.width().colRow())[1]);
    (*crphi1_all).push_back((clusi1.width().widthPhiRZ())[0]);
    (*cz1_all).push_back((clusi1.width().widthPhiRZ())[1]);
    (*x1_all).push_back(clusi1.globalPosition().x());
    (*y1_all).push_back(clusi1.globalPosition().y());
    (*z1_all).push_back(clusi1.globalPosition().z());
  }
  for(int i2=0; i2<n2; i2++) {
    const PixelCluster& clusi2 = clusterLayer[2][i2];
    double cxi2 = clusi2.globalPosition().x() - vx;
    double cyi2 = clusi2.globalPosition().y() - vy;
    double czi2 = clusi2.globalPosition().z() - vz;
    double phii2 = atan2(cyi2,cxi2);
    double ri2 = sqrt(cxi2*cxi2+cyi2*cyi2);
    double thetai2 = atan2(ri2,czi2);
    double etai2 = -TMath::Log(TMath::Tan(thetai2/2));
    (*eta2_all).push_back(etai2);
    (*phi2_all).push_back(phii2);
    (*dedx2_all).push_back(clusi2.energyLoss());
    const InDetDD::SiDetectorElement* siDetectorElt2= clusi2.detectorElement();
    double pathLength = siDetectorElt2->thickness()/((siDetectorElt2->surface().normal().x()*cxi2+siDetectorElt2->surface().normal().y()*cyi2+siDetectorElt2->surface().normal().z()*czi2)/sqrt(cxi2*cxi2+cyi2*cyi2+czi2*czi2));
    (*pathLength2_all).push_back(pathLength);
    //     (*csize2_all).push_back(clusi2.rdoList().size());
    //     (*ccol2_all).push_back((clusi2.width().colRow())[0]);
    //     (*crow2_all).push_back((clusi2.width().colRow())[1]);
    //     (*crphi2_all).push_back((clusi2.width().widthPhiRZ())[0]);
    //     (*cz2_all).push_back((clusi2.width().widthPhiRZ())[1]);
    //     (*x2_all).push_back(clusi2.globalPosition().x());
    //     (*y2_all).push_back(clusi2.globalPosition().y());
    //     (*z2_all).push_back(clusi2.globalPosition().z());
  }      
  
  

  Rec::TrackParticleContainer* trkpcon = new Rec::TrackParticleContainer();
  HIPixelTrackletsCollection* trkcoll = new HIPixelTrackletsCollection();
  ntr = trackletsCol.size(); 
  //log << MSG::DEBUG << "tracklets number: " << ntr << endreq;
  for(int it=0; it<ntr; it++)
    {
      (*threeLayerTrk).push_back(threeLayerTrkMap[it]);
      
      const PixelCluster& clusi0 = trackletsCol[it][0];
      const PixelCluster& clusi1 = trackletsCol[it][1];
      (*startingPoint).push_back(pixelID->layer_disk(clusi0.detectorElement()->identify())+pixelID->layer_disk(clusi1.detectorElement()->identify()));
      double cxi0 = clusi0.globalPosition().x() - vx;
      double cyi0 = clusi0.globalPosition().y() - vy;
      double czi0 = clusi0.globalPosition().z() - vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      double cxi1 = clusi1.globalPosition().x() - vx;
      double cyi1 = clusi1.globalPosition().y() - vy;
      double czi1 = clusi1.globalPosition().z() - vz;
      double phii1 = atan2(cyi1,cxi1);
      double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
      double thetai1 = atan2(ri1,czi1);
      double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
      (*eta0_tr).push_back(etai0);
      (*phi0_tr).push_back(phii0);
      (*eta1_tr).push_back(etai1);
      (*phi1_tr).push_back(phii1);
      (*dedx0_tr).push_back(clusi0.energyLoss());
      (*dedx1_tr).push_back(clusi1.energyLoss());
      const InDetDD::SiDetectorElement* siDetectorElt0= clusi0.detectorElement();
      const InDetDD::SiDetectorElement* siDetectorElt1= clusi1.detectorElement();
      double pathLength0 = siDetectorElt0->thickness()/((siDetectorElt0->surface().normal().x()*cxi0+siDetectorElt0->surface().normal().y()*cyi0+siDetectorElt0->surface().normal().z()*czi0)/sqrt(cxi0*cxi0+cyi0*cyi0+czi0*czi0));
      double pathLength1 = siDetectorElt1->thickness()/((siDetectorElt1->surface().normal().x()*cxi1+siDetectorElt1->surface().normal().y()*cyi1+siDetectorElt1->surface().normal().z()*czi1)/sqrt(cxi1*cxi1+cyi1*cyi1+czi1*czi1));
      (*pathLength0_tr).push_back(pathLength0);
      (*pathLength1_tr).push_back(pathLength1);
      (*csize0_tr).push_back(clusi0.rdoList().size());
      (*csize1_tr).push_back(clusi1.rdoList().size());
      (*ccol0_tr).push_back((clusi0.width().colRow())[0]);
      (*crow0_tr).push_back((clusi0.width().colRow())[1]);
      (*crphi0_tr).push_back((clusi0.width().widthPhiRZ())[0]);
      (*cz0_tr).push_back((clusi0.width().widthPhiRZ())[1]);
      (*ccol1_tr).push_back((clusi1.width().colRow())[0]);
      (*crow1_tr).push_back((clusi1.width().colRow())[1]);
      (*crphi1_tr).push_back((clusi1.width().widthPhiRZ())[0]);
      (*cz1_tr).push_back((clusi1.width().widthPhiRZ())[1]);
      (*x0_tr).push_back(clusi0.globalPosition().x());
      (*y0_tr).push_back(clusi0.globalPosition().y());
      (*z0_tr).push_back(clusi0.globalPosition().z());
      (*x1_tr).push_back(clusi1.globalPosition().x());
      (*y1_tr).push_back(clusi1.globalPosition().y());
      (*z1_tr).push_back(clusi1.globalPosition().z());
      (*deta_tr).push_back(etai0-etai1);
      double dphi = phii0-phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      (*dphi_tr).push_back(dphi);
      
      //double dis = sqrt( (clusi0.globalPosition().x()-clusi1.globalPosition().x())*(clusi0.globalPosition().x()-clusi1.globalPosition().x()) + (clusi0.globalPosition().y()-clusi1.globalPosition().y())*(clusi0.globalPosition().y()-clusi1.globalPosition().y()) + (clusi0.globalPosition().z()-clusi1.globalPosition().z())*(clusi0.globalPosition().z()-clusi1.globalPosition().z()) );
      //double pt = fabs(0.0003*dis/dphi);
      double x0p = clusi0.globalPosition().x()-vx;
      double y0p = clusi0.globalPosition().y()-vy;
      //double z0p = clusi0.globalPosition().z()-vz;
      double x1p = clusi1.globalPosition().x()-vx;
      double y1p = clusi1.globalPosition().y()-vy;
      //double z1p = clusi1.globalPosition().z()-vz;
      double O1 = 0.5*(y1p-y0p+x1p*x1p/y1p-x0p*x0p/y0p)/(x1p/y1p-x0p/y0p);//circle center x with respect to vx, vy
      double O2 = 0.5*(x1p-x0p+y1p*y1p/x1p-y0p*y0p/x0p)/(y1p/x1p-y0p/x0p);//circle center y with respect to vx, vy
      double radius = sqrt(O1*O1+O2*O2);
      double pt = 0.0006*radius;
      double pz = pt*TMath::SinH(etai0);
      double p = sqrt(pt*pt+pz*pz);
      double px = pt*cos(phii0);
      double py = pt*sin(phii0);
      (*pt_tr).push_back(pt);
      (*p_tr).push_back(p);
      (*px_tr).push_back(px);
      (*py_tr).push_back(py);
      (*pz_tr).push_back(pz);
      
      Rec::TrackParticle* trkp = new Rec::TrackParticle();
      trkp->setPx(px);
      trkp->setPy(py);
      trkp->setPz(pz);
      trkp->setE(p);
      trkpcon->push_back(trkp);

      const HepGeom::Point3D<double> vPosition(vx,vy,vz);
      std::vector<PixelCluster*> twoClusters;
      twoClusters.push_back(&trackletsCol[it][0]);
      twoClusters.push_back(&trackletsCol[it][1]);
      trkcoll->push_back(new HIPixelTracklets(vPosition,twoClusters));
    }
  sc = m_StoreGate->record(trkpcon,"PixelTrackletTrackParticles");
  if( sc.isFailure() ){log << MSG::ERROR << "Could not save converted PixelTracklet Particles! " << endreq;
  return StatusCode::FAILURE;}
  sc = m_StoreGate->record(trkcoll,m_pixelTrackletsCollection);
  if( sc.isFailure() ) {log << MSG::ERROR << "Could not record pixelTrackletsCollection to storegage!" << endreq; 
  return StatusCode::FAILURE; }
  
  //log << MSG::DEBUG << "fill tracklets finished." << endreq;
  if( m_mc_available )
    {
      //initialize number of truth tracks
      ntrT = 0;
      
      for(std::vector<const HepMC::GenParticle*>::iterator pitr = primaryGenParts.begin();pitr != primaryGenParts.end(); pitr++)
	{ 
	  //(*pdgid).push_back((*pitr)->pdg_id());
	  (*partTid).push_back((*pitr)->pdg_id());
	  
	  //13.07.11 Savannah #84284 Scott fix 
	  //int vBarcode = 0;
	  if( (*pitr)->production_vertex() )
	    {
	      const HepMC::GenVertex *pvrt = (*pitr)->production_vertex();
	      int ingoingparticles = pvrt->particles_in_size();
	      (*nparentT).push_back(ingoingparticles);
	      //log << MSG::DEBUG << ingoingparticles << " parents particles. " << endreq;
	      if( ingoingparticles!=1 ){
		(*parentTid).push_back(0);
	      }
	      else {
		(*parentTid).push_back((*(pvrt->particles_in_const_begin()))->pdg_id());
	      }
	      
	      //13.07.11 Savannah #84284 Scott fix
	      //vBarcode = (*pitr)->production_vertex()->barcode();
	      //log << MSG::DEBUG << "vertex barcode: " << vBarcode << endreq;
	      //right now only by position to identify true primary
	      (*prod_vrtxT).push_back((*pitr)->production_vertex()->position().x());
	      (*prod_vrtyT).push_back((*pitr)->production_vertex()->position().y());
	      (*prod_vrtzT).push_back((*pitr)->production_vertex()->position().z());
	      if( (*pitr)->production_vertex()->position().z()==sim_vz && 
		  (*pitr)->production_vertex()->position().x()==sim_vx &&
		  (*pitr)->production_vertex()->position().y()==sim_vy )
		
		(*primaryDecayLabel).push_back(0);
	      else
		(*primaryDecayLabel).push_back(1);
	    }
	  else
	    {
	      (*primaryDecayLabel).push_back(-1);
	      (*prod_vrtxT).push_back(-100000);
	      (*prod_vrtyT).push_back(-100000);
	      (*prod_vrtzT).push_back(-100000);
	      (*nparentT).push_back(-100000);
	      (*parentTid).push_back(-100000);
	      log << MSG::DEBUG << "gen particle has not production vertex! What does this mean. " << endreq;
	    }
	  
	  double eta = (*pitr)->momentum().eta();
	  double phi = (*pitr)->momentum().phi();
	  double px = (*pitr)->momentum().px();
	  double py = (*pitr)->momentum().py();
	  double pz = (*pitr)->momentum().pz();
	  double pt = sqrt(px*px+py*py);
	  double p = sqrt(pt*pt + pz*pz);
	  //make 100MeV cuts
	  //if( pt<100.0 ) continue;
	  
	  (*etaT).push_back(eta);
	  (*phiT).push_back(phi);
	  (*ptT).push_back(pt/1000.);
	  (*pT).push_back(p/1000.);
	  (*pxT).push_back(px/1000.);
	  (*pyT).push_back(py/1000.);
	  (*pzT).push_back(pz/1000.);
	  ntrT++;
	}
      
      for(std::vector<std::vector<PixelCluster> >::iterator ino = genPartCol\
	    .begin(); ino!=genPartCol.end(); ino++) {
	std::vector<PixelCluster> genClusters = *ino;
	(*nPixelClustersT).push_back(genClusters.size());
	int nBLayerCl = 0;
	for(unsigned int icl=0; icl<genClusters.size(); icl++) {
	  Identifier id = genClusters[icl].identify();
	  if(pixelID->layer_disk(id)==0) nBLayerCl++;
	}
	(*nBLayerClustersT).push_back(nBLayerCl);
      }
      //log << MSG::DEBUG << "number of genParticles: " << genPartCol.size() << endreq;
      // int igenpart=-1;
//       for(std::vector<std::vector<PixelCluster> >::iterator ino = genPartCol.begin(); ino!=genPartCol.end(); ino++)
// 	{
// 	  igenpart++;
// 	  std::vector<PixelCluster> genClusters = *ino;
// 	  //log << MSG::DEBUG << "number of clusters per gen particle. " << genClusters.size() << endreq;
// 	  (*nPixelClustersT).push_back(genClusters.size());
// 	  if( genClusters.size()==0 )
// 	    {
// 	      (*x0_true).push_back(-100);
// 	      (*y0_true).push_back(-100);
// 	      (*z0_true).push_back(-100);
// 	      (*x1_true).push_back(-100);
// 	      (*y1_true).push_back(-100);
// 	      (*z1_true).push_back(-100);
// 	      (*eta0_true).push_back(-100);
// 	      (*phi0_true).push_back(-100);
// 	      (*eta1_true).push_back(-100);
// 	      (*phi1_true).push_back(-100);
// 	      (*deta_true).push_back(-100);
// 	      (*dphi_true).push_back(-100);
// 	    }
// 	  if( genClusters.size()==1 )
// 	    {
// 	      const PixelCluster& clusi0=genClusters[0]; //not necessary the first layer
// 	      int layer = pixelID->layer_disk(clusi0.identify());
// 	      double cxi0 = clusi0.globalPosition().x() - vx;
//               double cyi0 = clusi0.globalPosition().y() - vy;
//               double czi0 = clusi0.globalPosition().z() - vz;
//               double phii0 = atan2(cyi0,cxi0);
//               double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
//               double thetai0 = atan2(ri0,czi0);
//               double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
// 	      if( layer==0 ) //first layer
// 		{
// 		  (*x0_true).push_back(clusi0.globalPosition().x());
// 		  (*y0_true).push_back(clusi0.globalPosition().y());
// 		  (*z0_true).push_back(clusi0.globalPosition().z());
// 		  (*eta0_true).push_back(etai0);
// 		  (*phi0_true).push_back(phii0);
// 		  (*x1_true).push_back(-100);
//                   (*y1_true).push_back(-100);
//                   (*z1_true).push_back(-100);
// 		  (*eta1_true).push_back(-100);
// 		  (*phi1_true).push_back(-100);
// 		  (*deta_true).push_back(-100);
// 		  (*dphi_true).push_back(-100);
		  
// 		}
// 	      if( layer==1 )
// 		{
// 		  (*x1_true).push_back(clusi0.globalPosition().x());
//                   (*y1_true).push_back(clusi0.globalPosition().y());
//                   (*z1_true).push_back(clusi0.globalPosition().z());
// 		  (*eta1_true).push_back(etai0);
// 		  (*phi1_true).push_back(phii0);
// 		  (*x0_true).push_back(-100);
//                   (*y0_true).push_back(-100);
//                   (*z0_true).push_back(-100);
// 		  (*eta0_true).push_back(-100);
// 		  (*phi0_true).push_back(-100);
// 		  (*deta_true).push_back(-100);
// 		  (*dphi_true).push_back(-100);
// 		}
// 	    }
// 	  if( genClusters.size()>1 )
// 	    {
// 	      int label0=-1;
// 	      int label1=-1;
// 	      for(unsigned int icl=0; icl<genClusters.size(); icl++)
// 		{
// 		  if(pixelID->layer_disk(genClusters[icl].identify())==0)
// 		    label0=icl;
// 		  if(pixelID->layer_disk(genClusters[icl].identify())==1)
// 		    label1=icl;
// 		}
// 	      if( label1==-1 ) //both are first layer
// 		{
// 		  const PixelCluster& clusi0=genClusters[label0];
// 		  double cxi0 = clusi0.globalPosition().x() - vx;
//                   double cyi0 = clusi0.globalPosition().y() - vy;
//                   double czi0 = clusi0.globalPosition().z() - vz;
//                   double phii0 = atan2(cyi0,cxi0);
//                   double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
//                   double thetai0 = atan2(ri0,czi0);
//                   double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
// 		  (*x0_true).push_back(clusi0.globalPosition().x());
//                   (*y0_true).push_back(clusi0.globalPosition().y());
//                   (*z0_true).push_back(clusi0.globalPosition().z());
// 		  (*eta0_true).push_back(etai0);
//                   (*phi0_true).push_back(phii0);
// 		  (*x1_true).push_back(-100);
//                   (*y1_true).push_back(-100);
//                   (*z1_true).push_back(-100);
//                   (*eta1_true).push_back(-100);
//                   (*phi1_true).push_back(-100);
//                   (*deta_true).push_back(-100);
//                   (*dphi_true).push_back(-100);
// 		}
// 	      else if ( label0==-1 ) //both are second layer
// 		{
// 		  const PixelCluster& clusi1=genClusters[label1];
// 		  double cxi1 = clusi1.globalPosition().x() - vx;
//                   double cyi1 = clusi1.globalPosition().y() - vy;
//                   double czi1 = clusi1.globalPosition().z() - vz;
//                   double phii1 = atan2(cyi1,cxi1);
//                   double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
//                   double thetai1 = atan2(ri1,czi1);
//                   double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
// 		  (*x1_true).push_back(clusi1.globalPosition().x());
//                   (*y1_true).push_back(clusi1.globalPosition().y());
//                   (*z1_true).push_back(clusi1.globalPosition().z());
// 		  (*eta1_true).push_back(etai1);
//                   (*phi1_true).push_back(phii1);
// 		  (*x0_true).push_back(-100);
//                   (*y0_true).push_back(-100);
//                   (*z0_true).push_back(-100);
//                   (*eta0_true).push_back(-100);
//                   (*phi0_true).push_back(-100);
//                   (*deta_true).push_back(-100);
//                   (*dphi_true).push_back(-100);
// 		}
// 	      else
// 		{
// 		  const PixelCluster& clusi0=genClusters[label0];
// 		  const PixelCluster& clusi1=genClusters[label1];
// 		  double cxi0 = clusi0.globalPosition().x() - vx;
// 		  double cyi0 = clusi0.globalPosition().y() - vy;
// 		  double czi0 = clusi0.globalPosition().z() - vz;
// 		  double phii0 = atan2(cyi0,cxi0);
// 		  double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
// 		  double thetai0 = atan2(ri0,czi0);
// 		  double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
// 		  double cxi1 = clusi1.globalPosition().x() - vx;
// 		  double cyi1 = clusi1.globalPosition().y() - vy;
// 		  double czi1 = clusi1.globalPosition().z() - vz;
// 		  double phii1 = atan2(cyi1,cxi1);
// 		  double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
// 		  double thetai1 = atan2(ri1,czi1);
// 		  double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
// 		  (*x0_true).push_back(clusi0.globalPosition().x());
//                   (*y0_true).push_back(clusi0.globalPosition().y());
//                   (*z0_true).push_back(clusi0.globalPosition().z());
// 		  (*eta0_true).push_back(etai0);
// 		  (*phi0_true).push_back(phii0);
// 		  (*x1_true).push_back(clusi1.globalPosition().x());
//                   (*y1_true).push_back(clusi1.globalPosition().y());
//                   (*z1_true).push_back(clusi1.globalPosition().z());
// 		  (*eta1_true).push_back(etai1);
// 		  (*phi1_true).push_back(phii1);
// 		  double detat = etai0-etai1;
// 		  (*deta_true).push_back(detat);
// 		  double dphit = phii0-phii1;
// 		  if( dphit>M_PI ) dphit -= 2*M_PI;
// 		  if( dphit<-M_PI) dphit += 2*M_PI;
// 		  (*dphi_true).push_back(dphit);
// 		}
// 	    }
// 	}
      //log << MSG::DEBUG << "test. genCol end" << endreq;
      //    //used for test to see where are those fake tracklets coming from
//       for(int i=0; i<ntr; i++)
// 	{
// 	  if( trackletsMatch[i] ) continue;
// 	  if( (*matchedToWeakDecay)[i] ) continue;
// 	  std::vector<PixelCluster> twoClusters = trackletsCol[i];
// 	  const PixelCluster& clus0 = twoClusters[0];
// 	  const PixelCluster& clus1 = twoClusters[1];
// 	  Identifier id0 = clus0.identify();
// 	  Identifier id1 = clus1.identify();
// 	  //log <<MSG::DEBUG << "id0:id1 " << id0 << ":" << id1 << endreq;
// 	  std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator> range0;
// 	  std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator> range1;
// 	  range0= m_truthCollectionPixel->equal_range(id0);
// 	  range1= m_truthCollectionPixel->equal_range(id1);
// 	  //log << MSG::DEBUG << "not matched particle." << endreq;
// 	  for(PRD_MultiTruthCollection::const_iterator ip = range0.first; ip != range0.second; ip++)
// 	    {
// 	      const HepMC::GenParticle& part = *(ip->second);
// 	      int barcode = part.barcode();
// 	      //log << MSG::DEBUG << "layer 0:barcode," << barcode <<  ":stable," << part.status() <<  ":pdgid," << part.pdg_id() << ":eta," << part.momentum().eta() << endreq;
// 	    }
// 	  for(PRD_MultiTruthCollection::const_iterator ip = range1.first; ip != range1.second; ip++)
// 	    {
// 	      const HepMC::GenParticle& part = *(ip->second);
// 	      int barcode = part.barcode();
// 	      log << MSG::DEBUG << "layer 1:barcode: " << barcode << endreq;
	      
// 	    }
// 	}
    }

  //log << MSG::DEBUG << "NumVrtRec: " << NumVrtRec << " ntr: " << ntr << " ntrT: " << ntrT << endreq;
  //log << MSG::DEBUG << "eta0 size: " << eta0_tr->size() << " eta0T size: " << eta0_true->size() << endreq;
  trackletsTree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode MakePixelTracklets::RetrieveTruth()
{
  MsgStream log(msgSvc(), name());
  //log << MSG::DEBUG << "RetireveTruth()::beginning. " << endreq;
  if( !m_mc_available )
    return StatusCode::SUCCESS;
  //retrieve all primary genparticles
  //get the simulated event position
  const McEventCollection *mcCollection(0);
  StatusCode sc = m_StoreGate->retrieve(mcCollection,"TruthEvent");
  if( sc.isFailure() )
    {
      log << MSG::ERROR << "TruthEvent not found" << endreq;
      log << MSG::ERROR << "the monte carlo flag tells us it there however!" << endreq;
      return sc;
    }
  
  //generated primary vertex
  McEventCollection::const_iterator it = mcCollection->begin();
  const HepMC::GenEvent* genEvent = (*it);
  if( genEvent==0 )
    {
      log << MSG::INFO << " GenEvent not found. " << endreq;
      return StatusCode::FAILURE;
    }
  HepMC::GenEvent::vertex_const_iterator pv;
  pv = genEvent->vertices_begin();
 
  //log << MSG::DEBUG << "num of vertices: " << nvrts << endreq;
  if( m_StoreGate->contains<PileUpEventInfo>("OverlayEvent")  )
    {
      log << MSG::INFO << "more that one mc vertices, pileup events? " << endreq;
      log << MSG::INFO << "Anyway right now only use the first mc vertex." << endreq;
    }
  
  //log << MSG::DEBUG << " signal vertex barcode: " << (*pv)->barcode() << endreq;
  if( !(*pv) ) { log<< MSG::INFO << "ERROR: Truth vertex not exist!" << endreq; return StatusCode::SUCCESS; }
  sim_vx = (*pv)->position().x();
  sim_vy = (*pv)->position().y();
  sim_vz = (*pv)->position().z();
  
  //int nvrts = genEvent->vertices_size();
  pv++;
  for( ; pv!=genEvent->vertices_end(); pv++){
    const HepMC::GenVertex* genVrt = *pv;
    if( !genVrt ) continue;
    
    HepMC::GenVertex::particles_out_const_iterator ito;
    for(ito=genVrt->particles_out_const_begin(); ito!=genVrt->particles_out_const_end(); ito++){
      const HepMC::GenParticle* part = *ito;
      if( !part ) continue;
      double eta = part->momentum().eta();
      if( fabs(eta)>m_etaMax+1.0 ) continue;
      if( part->status()!=1 ) continue;
      //log << MSG::DEBUG << "stable particle barcode: " << part->barcode() << endreq;
      if( part->barcode() <200000 ) continue;
      weakDecayParts.push_back(part);
    }
    
  }
  
  //log << MSG::DEBUG << "particles from weak decay: " << weakDecayParts.size() << endreq;
  //log << MSG::DEBUG << " signal vertex x,y,z: " << sim_vx << "," << sim_vy << "," << sim_vz << endreq;
  McEventCollection::const_iterator ie = mcCollection->begin();
  ie++;
  for( ; ie!=mcCollection->end(); ie++){
    log << MSG::DEBUG << "test. " << endreq;
  }
  
  TruthHelper::IsGenStable ifs;
  std::vector<const HepMC::GenParticle*> particles;
  StatusCode stat = m_tesIO->getMC(particles, &ifs);
  if(stat.isFailure()) {return stat;} //AO
  for(std::vector<const HepMC::GenParticle*>::iterator pitr = particles.begin();pitr != particles.end(); pitr++)
    {
      const HepMC::GenParticle* part = *pitr;
      if( part==0 )
	{
	  log << MSG::INFO << "no genparticle found" << endreq;
	  continue;
	}
      int barcode = part->barcode();
      //log << MSG::DEBUG << " barcode: " << barcode << endreq;
      
      //  if(fabs((*pitr)->production_vertex()->position().z() - vz)>1.0) 
      // 	    continue;
      //primary particle cut???not sure
      if( barcode>200000 ) continue;
      //stable??
      if( part->status()!= 1 ) continue;
      
      //if(fabs((*pitr)->production_vertex()->position().z() - sim_vrt_z)>0.5)
      //log << MSG::DEBUG << " vertex z:x:y " << (*pitr)->production_vertex()->position().z() << ":" << (*pitr)->production_vertex()->position().x() << ":" << (*pitr)->production_vertex()->position().y() << " simulation vertex z: " << sim_vrt_z << endreq;
      
      Int_t pid = abs((*pitr)->pdg_id());
      // select stable hadrons
      //if (pid!=211&&pid!=321&&pid!=2212) continue; 
      //charged or not
      double charge = 0.0;
      //Is there something wrong with the particle data group. Some (*m_particleDataTable)[pid] doesn't exist and cause crash. e.g. pid=21
      if( (*m_particleDataTable)[pid] )
	charge = (*m_particleDataTable)[pid]->charge();
      else { log<<MSG::INFO << "WARNING: this M_PID is not in particle data table." << pid << endreq; }
      if( charge==0.0 ) continue;
      double eta = (*pitr)->momentum().eta();
      //only consider particles in the pixel acceptance region              
      if( fabs(eta)>m_etaMax+1.0 ) continue;
      primaryGenParts.push_back(part);
      
      //log << MSG::DEBUG << "primary:" << barcode << endreq;
    }
  
  if(m_StoreGate->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionPixelName)) 
    {
      sc = m_StoreGate->retrieve(m_truthCollectionPixel, m_multiTruthCollectionPixelName);
      if(sc.isFailure()){
	log << MSG::INFO << " could not open PRD_MultiTruthCollection : " <<  m_multiTruthCollectionPixelName << endreq;
	sc=StatusCode::SUCCESS; // not having truth information is not a failure
	m_truthCollectionPixel=0;
	return StatusCode::SUCCESS;
      }
    }
  
  std::multimap<int,PixelCluster> partClustersMap;
  //std::map<Identifier,PixelCluster> clusIDMap;
  //std::set<int> genParticles;
  for(int la=0; la<3; la++) {
    for(unsigned int i0=0; i0<clusterLayer[la].size(); i0++) {
      const PixelCluster& clusi0 = clusterLayer[la][i0];
      Identifier ID = clusi0.identify();
      std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator> range;
      range= m_truthCollectionPixel->equal_range(ID);
      //clusIDMap.insert(std::pair<Identifier,PixelCluster>(ID,clusi0));
      for(PRD_MultiTruthCollection::const_iterator i = range.first; i != range.second; i++) {
	const HepMC::GenParticle& part = *(i->second);
	int barcode = part.barcode();
	//if( barcode>200000 ) //only consider primary now 
	//  continue;
	
	partClustersMap.insert(std::pair<int,PixelCluster>(barcode,clusi0));
	//genParticles.insert(barcode);
      }
    }
  }
  
  for( std::vector<const HepMC::GenParticle*>::iterator pitr = primaryGenParts.begin(); pitr!=primaryGenParts.end(); pitr++)
    {
      int barcode = (*pitr)->barcode();
      std::vector<PixelCluster> twoClusters;
      std::pair<std::multimap<int,PixelCluster>::const_iterator, std::multimap<int,PixelCluster>::const_iterator > range;
      range = partClustersMap.equal_range(barcode);
      for(std::multimap<int,PixelCluster>::const_iterator siter=range.first; siter!=range.second; siter++)
	{
	  const PixelCluster& clus = siter->second;
	  twoClusters.push_back(clus);
	  //if(barcode==48)
	  //log << MSG::DEBUG << "particle 48 ids: " << clus.identify() << endreq;
	}
      genPartCol.push_back(twoClusters);
    }
  
  // partClustersMap.clear();
//   //for ks, lambda, sigma decay products, find the corresbonding clusters
//   for(unsigned int i0=0; i0<clusterLayer0.size(); i0++)
//     {
//       const PixelCluster& clusi0 = clusterLayer0[i0];
//       Identifier ID = clusi0.identify();
//       std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator> range;
//       range= m_truthCollectionPixel->equal_range(ID);
//       //clusIDMap.insert(std::pair<Identifier,PixelCluster>(ID,clusi0));
//       for(PRD_MultiTruthCollection::const_iterator i = range.first; i != range.second; i++)
// 	{
// 	  const HepMC::GenParticle& part = *(i->second);
// 	  int barcode = part.barcode();
// 	  double eta = part.momentum().eta();
// 	  if( fabs(eta)>m_etaMax+1.0) continue;
// 	  partClustersMap.insert(std::pair<int,PixelCluster>(barcode,clusi0));
// 	}
//     }
//   for(unsigned int i1=0; i1<clusterLayer1.size(); i1++)
//     {
//       const PixelCluster& clusi1 = clusterLayer1[i1];
//       Identifier ID = clusi1.identify();
//       std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator> range;
//       range= m_truthCollectionPixel->equal_range(ID);
//       //clusIDMap.insert(std::pair<Identifier,PixelCluster>(ID,clusi1));
//       for(PRD_MultiTruthCollection::const_iterator i = range.first; i != range.second; i++)
//         {
//           const HepMC::GenParticle& part = *(i->second);
//           int barcode = part.barcode();
	  
// 	  double eta = part.momentum().eta();
// 	  if( fabs(eta)>m_etaMax+1.0) continue;
// 	  partClustersMap.insert(std::pair<int,PixelCluster>(barcode,clusi1));
// 	}
//     }
  //log << MSG::DEBUG << "weak decay particles. " << weakDecayParts.size() << endreq;
  for( std::vector<const HepMC::GenParticle*>::iterator pitr = weakDecayParts.begin(); pitr!=weakDecayParts.end(); pitr++)
    {
      int barcode = (*pitr)->barcode();
      std::vector<PixelCluster> twoClusters;
      std::pair<std::multimap<int,PixelCluster>::const_iterator, std::multimap<int,PixelCluster>::const_iterator > range;
      range = partClustersMap.equal_range(barcode);
      for(std::multimap<int,PixelCluster>::const_iterator siter=range.first; siter!=range.second; siter++)
	{
	  const PixelCluster& clus = siter->second;
	  twoClusters.push_back(clus);
	}
      weakDecayCol.push_back(twoClusters);
    }
  
  return StatusCode::SUCCESS;
}


void MakePixelTracklets::TruthAssociation() {
  MsgStream log(msgSvc(), name());
  //log << MSG::DEBUG << "TruthAssociation()::beginning" << endreq;
  for(unsigned int itr=0; itr<trackletsCol.size(); itr++) {
    int pdgid = 0;
    int parentPdgid = 0;
    std::vector<PixelCluster> twoClusters = trackletsCol[itr];
    std::set<Identifier> twoIds;
    for(unsigned int icl=0; icl<twoClusters.size(); icl++)
      {
	twoIds.insert(twoClusters[icl].identify());
      }
    //match to primaries
    bool foundPri = false;
    for(unsigned int ipart=0; ipart<genPartCol.size(); ipart++) {
      std::vector<PixelCluster> trueClusters = genPartCol[ipart];
      std::set<Identifier> trueIds;
      for(unsigned int icl=0; icl<trueClusters.size(); icl++) {
	Identifier id = trueClusters[icl].identify();
	trueIds.insert(id);
      }
      if( trueIds==twoIds || std::includes(trueIds.begin(),trueIds.end(),twoIds.begin(),twoIds.end()) ) {
	foundPri = true;
	const HepMC::GenParticle* ppart = primaryGenParts[ipart];
	pdgid = ppart->pdg_id();
	const HepMC::GenVertex *pvrt = ppart->production_vertex();
	if( pvrt ) parentPdgid = (*(pvrt->particles_in_const_begin()))->pdg_id();
	
	(*matched).push_back(1);
	(*matchedToWeakDecay).push_back(0);
	(*matchedNumber).push_back(ipart);
	(*partid_tr).push_back(pdgid);
	(*parentid_tr).push_back(parentPdgid);
	break;
      }
    }
    
    if( foundPri ) continue;
    else   (*matched).push_back(0); //not matched to primaries
    //match to secondaries
    bool found = false;
    for(unsigned int ipart=0; ipart<weakDecayCol.size(); ipart++) {
      std::vector<PixelCluster> trueClusters = weakDecayCol[ipart];
      std::set<Identifier> trueIds;
      for(unsigned int icl=0; icl<trueClusters.size(); icl++)
	{
	  Identifier id = trueClusters[icl].identify();
	  trueIds.insert(id);
	}
      
      if( trueIds==twoIds || std::includes(trueIds.begin(),trueIds.end(),twoIds.begin(),twoIds.end()) ) {
	found = true;
	const HepMC::GenParticle* wpart = weakDecayParts[ipart];
	pdgid = wpart->pdg_id();
	const HepMC::GenVertex *pvrt = wpart->production_vertex();
	if( pvrt ) parentPdgid = (*(pvrt->particles_in_const_begin()))->pdg_id();
	
	(*matchedToWeakDecay).push_back(1);
	(*matchedNumber).push_back(ipart);
	(*partid_tr).push_back(pdgid);
	(*parentid_tr).push_back(parentPdgid);
	break;
      }
    }
    //combinatorics, not matched to parimaries or secondaries
    if( !found ) { 
      (*matchedToWeakDecay).push_back(0);
      (*matchedNumber).push_back(-1);
      (*partid_tr).push_back(0);
      (*parentid_tr).push_back(0);
    }
    
  }

}


void MakePixelTracklets::CalEfficiency(int layer)
{
  //this should be done, because the function was called twice
  (*projected_eff).clear();
  (*phi_eff).clear();
  (*eta_eff).clear();
  (*pt_eff).clear();
  MsgStream log(msgSvc(), name());
  std::vector<PixelCluster> clusterLayerRef;
  std::vector<PixelCluster> clusterLayerHole;
  if( layer==0 )
    {
      for(unsigned int i=0; i<clusterLayer[1].size(); i++)
	clusterLayerRef.push_back(clusterLayer[1][i]);
      for(unsigned int i=0; i<clusterLayer[0].size(); i++)
	clusterLayerHole.push_back(clusterLayer[0][i]);
    }
  else if( layer==1 )
    {
      for(unsigned int i=0; i<clusterLayer[0].size(); i++)
	clusterLayerRef.push_back(clusterLayer[0][i]);
      for(unsigned int i=0; i<clusterLayer[1].size(); i++)
	clusterLayerHole.push_back(clusterLayer[1][i]);
    }
  else {
    log << MSG::ERROR << "CalEfficiency() with a wrong input parameter!" << endreq;
  }
  std::vector<double> etaHoles;
  std::vector<double> phiHoles;
  for(unsigned int ih=0; ih<clusterLayerHole.size(); ih++)
    {
      const PixelCluster& clusi0 = clusterLayerHole[ih];
      double cxi0 = clusi0.globalPosition().x() - vx;
      double cyi0 = clusi0.globalPosition().y() - vy;
      double czi0 = clusi0.globalPosition().z() - vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      etaHoles.push_back(etai0);
      phiHoles.push_back(phii0);
    }
  std::vector<double> etaLayer2;
  std::vector<double> phiLayer2;
  std::vector<double> etaRef;
  std::vector<double> phiRef;
  std::vector<double> ptTrks;
  std::vector<std::vector<PixelCluster> > trackletsEff;
  MakeTracklets(false,0,clusterLayerRef, clusterLayer[2], trackletsEff);
  unsigned int ntrksEff = trackletsEff.size();
  for(unsigned int it=0; it<ntrksEff; it++)
    {
      std::vector<PixelCluster> twoClusters = trackletsEff[it];
      const PixelCluster& clusi0 = twoClusters[0];
      const PixelCluster& clusi1 = twoClusters[1];
      //log << MSG::DEBUG << "tracklets first layer:" << pixelID->layer_disk(clusi0.identify())<<endreq;
      double cxi0 = clusi0.globalPosition().x() - vx;
      double cyi0 = clusi0.globalPosition().y() - vy;
      double czi0 = clusi0.globalPosition().z() - vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      double cxi1 = clusi1.globalPosition().x() - vx;
      double cyi1 = clusi1.globalPosition().y() - vy;
      double czi1 = clusi1.globalPosition().z() - vz;
      double phii1 = atan2(cyi1,cxi1);
      double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
      double thetai1 = atan2(ri1,czi1);
      double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
      double dphi = phii0-phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      
      //double dis = sqrt((cxi0-cxi1)*(cxi0-cxi1)+(cyi0-cyi1)*(cyi0-cyi1)+(czi0-czi1)*(czi0-czi1));
      //double pt = fabs(0.0003*dis/dphi);
      double x0p = cxi0;
      double y0p = cyi0;
      //double z0p = clusi0.globalPosition().z()-thisVx.position.z();
      double x1p = cxi1;
      double y1p = cyi1;
      //double z1p = clusi1.globalPosition().z()-thisVx.position.z();
      double O1 = 0.5*(y1p-y0p+x1p*x1p/y1p-x0p*x0p/y0p)/(x1p/y1p-x0p/y0p);//circle center x with respect to vx, vy
      double O2 = 0.5*(x1p-x0p+y1p*y1p/x1p-y0p*y0p/x0p)/(y1p/x1p-y0p/x0p);//circle center y with respect to vx, vy
      double radius = sqrt(O1*O1+O2*O2);
      double pt = 0.0006*radius;
      
      etaRef.push_back(etai0);
      phiRef.push_back(phii0);
      etaLayer2.push_back(etai1);
      phiLayer2.push_back(phii1);
      ptTrks.push_back(pt);
    }
  
  for(unsigned int i=0; i<ntrksEff; i++)
    {
      double etai0 = etaRef[i];
      double phii0 = phiRef[i];
      double etai1 = etaLayer2[i];
      double phii1 = phiLayer2[i];
      double pt = ptTrks[i];
      (*eta_eff).push_back(etai0);
      (*phi_eff).push_back(phii0);
      (*pt_eff).push_back(pt);
      bool found = false;
      for(unsigned int ih=0; ih<etaHoles.size(); ih++)
	{
	  double etai2 = etaHoles[ih];
	  double phii2 = phiHoles[ih];
	  double etadif0 = etai0 - etai2;
	  double etadif1 = etai1 - etai2;
	  double phidif0 = phii0 - phii2;
	  double phidif1 = phii1 - phii2;
	  if( phidif0>M_PI )  phidif0 -= 2*M_PI;
	  if( phidif0<-M_PI ) phidif0 += 2*M_PI;
	  if( phidif1>M_PI )  phidif1 -= 2*M_PI;
          if( phidif1<-M_PI ) phidif1 += 2*M_PI;
	  
	  if( fabs(etadif0)<m_etaCut && fabs(etadif1)<m_etaCut && fabs(phidif0)<m_phiCut && fabs(phidif1)<m_phiCut )
	    {
	      found = true;
	      break;
	    }
	}
      if( found )
	(*projected_eff).push_back(1);
      else
	(*projected_eff).push_back(0);
    }

  if( layer==0 )
    effTree0->Fill();
  if( layer==1 )
    effTree1->Fill();
}

void MakePixelTracklets::NewVectors()
{
  //new all the vectors that will be used for the output trees
  
  matched = new std::vector<int>;
  matchedNumber = new std::vector<int>;
  matchedToWeakDecay = new std::vector<int>;
  startingPoint = new std::vector<int>;
  threeLayerTrk = new std::vector<int>;
  deleted = new std::vector<int>;
  deletedBecauseOf = new std::vector<int>;
  pt_tr = new std::vector<double>;
  p_tr = new std::vector<double>;
  px_tr = new std::vector<double>;
  py_tr = new std::vector<double>;
  pz_tr = new std::vector<double>;
  ptF_tr = new std::vector<double>;
  pF_tr = new std::vector<double>;
  eta0_tr = new std::vector<double>;
  phi0_tr = new std::vector<double>;
  csize0_tr = new std::vector<int>;
  eta1_tr = new std::vector<double>;
  phi1_tr = new std::vector<double>;
  csize1_tr = new std::vector<int>;
  ccol0_tr = new std::vector<double>;
  ccol1_tr = new std::vector<double>;
  crow0_tr = new std::vector<double>;
  crow1_tr = new std::vector<double>;
  crphi0_tr = new std::vector<double>;
  crphi1_tr = new std::vector<double>;
  cz0_tr = new std::vector<double>;
  cz1_tr = new std::vector<double>;
  dedx0_tr = new std::vector<double>;
  dedx1_tr = new std::vector<double>;
  pathLength0_tr = new std::vector<double>;
  pathLength1_tr = new std::vector<double>;
  partid_tr = new std::vector<int>;
  parentid_tr = new std::vector<int>;
  eta0F_tr = new std::vector<double>;
  phi0F_tr = new std::vector<double>;
  eta1F_tr = new std::vector<double>;
  phi1F_tr = new std::vector<double>;
  deta_tr = new std::vector<double>;
  dphi_tr = new std::vector<double>;
  detaF_tr = new std::vector<double>;
  dphiF_tr = new std::vector<double>;
  etaT = new std::vector<double>;
  phiT = new std::vector<double>;
  ptT = new std::vector<double>;
  pT = new std::vector<double>;
  pxT = new std::vector<double>;
  pyT = new std::vector<double>;
  pzT = new std::vector<double>;
  partTid = new std::vector<int>;
  nparentT = new std::vector<int>; //number of parent particles,
  parentTid = new std::vector<int>; //can only record with one parent particle, which are interested for k short, lambda, and sigma
  multipTrks = new std::vector<int>;
  
  //parameters for all the clusters in layer 0 and 1                                        
  eta0_all = new std::vector<double>;
  phi0_all = new std::vector<double>;
  csize0_all = new std::vector<int>;
  eta1_all = new std::vector<double>;
  phi1_all = new std::vector<double>;
  csize1_all = new std::vector<int>;
  ccol0_all = new std::vector<double>;
  ccol1_all = new std::vector<double>;
  crow0_all = new std::vector<double>;
  crow1_all = new std::vector<double>;
  crphi0_all = new std::vector<double>;
  crphi1_all = new std::vector<double>;
  cz0_all = new std::vector<double>;
  cz1_all = new std::vector<double>;
  dedx0_all = new std::vector<double>;
  dedx1_all = new std::vector<double>;
  pathLength0_all = new std::vector<double>;
  pathLength1_all = new std::vector<double>;

  eta2_all = new std::vector<double>;
  phi2_all = new std::vector<double>;
  dedx2_all = new std::vector<double>;
  pathLength2_all = new std::vector<double>;
  
  //ganged flag
  ganged0_tr = new std::vector<int>;
  ganged1_tr = new std::vector<int>;
  
  x0_all = new std::vector<double>;
  y0_all = new std::vector<double>;
  z0_all = new std::vector<double>;
  
  x1_all = new std::vector<double>;
  y1_all = new std::vector<double>;
  z1_all = new std::vector<double>;
  
  x0_tr = new std::vector<double>;
  y0_tr = new std::vector<double>;
  z0_tr = new std::vector<double>;
  
  x1_tr = new std::vector<double>;
  y1_tr = new std::vector<double>;
  z1_tr = new std::vector<double>;
  
   //
  nPixelClustersT = new std::vector<int>;
  nBLayerClustersT = new std::vector<int>;
  //
  x0_true = new std::vector<double>;
  y0_true = new std::vector<double>;
  z0_true = new std::vector<double>;
  eta0_true = new std::vector<double>;
  phi0_true = new std::vector<double>;
  x1_true = new std::vector<double>;
  y1_true = new std::vector<double>;
  z1_true = new std::vector<double>;
  eta1_true = new std::vector<double>;
  phi1_true = new std::vector<double>;
  deta_true = new std::vector<double>;
  dphi_true = new std::vector<double>;
  //
  //pdgid = new std::vector<int>;
  prod_vrtxT = new std::vector<double>;
  prod_vrtyT = new std::vector<double>;
  prod_vrtzT = new std::vector<double>;
  //primary or others
  primaryDecayLabel = new std::vector<int>;
  for(int i=0; i<3; i++) {
    unassociated0[i] = new std::vector<int>;
    unassociated1[i] = new std::vector<int>;
    unassociated2[i] = new std::vector<int>;
  }
  //
  projected_eff = new std::vector<int>;
  phi_eff = new std::vector<double>;
  eta_eff = new std::vector<double>;
  pt_eff = new std::vector<double>;

  eta_ca = new std::vector<double>;
  phi_ca = new std::vector<double>;
  ene_ca = new std::vector<double>;
}

void MakePixelTracklets::DeleteNewVectors()
{
  //delete all new vectors created in constructor
  
  delete matched;
  delete matchedNumber;
  delete matchedToWeakDecay;
  delete startingPoint;
  delete threeLayerTrk;
  delete deleted;
  delete deletedBecauseOf;
  delete pt_tr;
  delete p_tr;
  delete px_tr;
  delete py_tr;
  delete pz_tr;
  delete ptF_tr;
  delete pF_tr;
  delete eta0_tr;
  delete phi0_tr;
  delete csize0_tr;
  delete eta1_tr;
  delete phi1_tr;
  delete csize1_tr;
  delete ccol0_tr;
  delete ccol1_tr;
  delete crow0_tr;
  delete crow1_tr;
  delete crphi0_tr;
  delete crphi1_tr;
  delete cz0_tr;
  delete cz1_tr;
  delete dedx0_tr;
  delete dedx1_tr;
  delete pathLength0_tr;
  delete pathLength1_tr;
  delete partid_tr;
  delete parentid_tr;
  delete eta0F_tr;
  delete phi0F_tr;
  delete eta1F_tr;
  delete phi1F_tr;
  delete deta_tr;
  delete dphi_tr;
  delete detaF_tr;
  delete dphiF_tr;
  delete etaT;
  delete phiT;
  delete ptT;
  delete pT;
  delete pxT;
  delete pyT;
  delete pzT;
  delete partTid;
  delete nparentT; 
  delete parentTid; 
  delete multipTrks;
  
  //parameters for all the clusters in layer 0 and 1                                        
  delete eta0_all;
  delete phi0_all;
  delete csize0_all;
  delete eta1_all;
  delete phi1_all;
  delete csize1_all;
  delete ccol0_all;
  delete ccol1_all;
  delete crow0_all;
  delete crow1_all;
  delete crphi0_all;
  delete crphi1_all;
  delete cz0_all;
  delete cz1_all;
  delete dedx0_all;
  delete dedx1_all;
  delete pathLength0_all;
  delete pathLength1_all;

  delete eta2_all;
  delete phi2_all;
  delete dedx2_all;
  delete pathLength2_all;
  
  //ganged flag
  delete ganged0_tr;
  delete ganged1_tr;
  
  delete x0_all;
  delete y0_all;
  delete z0_all;
  
  delete x1_all;
  delete y1_all;
  delete z1_all;
  
  delete x0_tr;
  delete y0_tr;
  delete z0_tr;
  
  delete x1_tr;
  delete y1_tr;
  delete z1_tr;
  
  //
  delete nPixelClustersT;
  delete nBLayerClustersT;
  //
  delete x0_true;
  delete y0_true;
  delete z0_true;
  delete eta0_true;
  delete phi0_true;
  delete x1_true;
  delete y1_true;
  delete z1_true;
  delete eta1_true;
  delete phi1_true;
  delete deta_true;
  delete dphi_true;
  //
  //pdgid = new std::vector<int>;
  delete prod_vrtxT;
  delete prod_vrtyT;
  delete prod_vrtzT;
  //primary or others
  delete primaryDecayLabel;
  for(int i=0; i<3; i++) {
    delete unassociated0[i];
    delete unassociated1[i];
    delete unassociated2[i];
  }
  //
  delete projected_eff;
  delete phi_eff;
  delete eta_eff;
  delete pt_eff;

  delete eta_ca;
  delete phi_ca;
  delete ene_ca;
}

void MakePixelTracklets::ClearVectors()
{
  //clear all vectors that are global variable
   trackletsCol.clear();
  (*multipTrks).clear();
  clusterLayer[0].clear();
  clusterLayer[1].clear();
  clusterLayer[2].clear();
  usedClusterLayer[0].clear();
  usedClusterLayer[1].clear();
  usedClusterLayer[2].clear();
   //log << MSG::DEBUG << "Starting fill trees." << endreq;
  (*matched).clear();
  (*matchedNumber).clear();
  (*matchedToWeakDecay).clear();
  (*startingPoint).clear();
  (*threeLayerTrk).clear();
  (*deleted).clear();
  (*deletedBecauseOf).clear();
  (*pt_tr).clear();
  (*p_tr).clear();
  (*px_tr).clear();
  (*py_tr).clear();
  (*pz_tr).clear();
  (*partid_tr).clear();
  (*parentid_tr).clear();
  (*ptF_tr).clear();
  (*pF_tr).clear();
  (*eta0_tr).clear();
  (*phi0_tr).clear();
  (*eta1_tr).clear();
  (*phi1_tr).clear();
  (*dedx0_tr).clear();
  (*dedx1_tr).clear();
  (*pathLength0_tr).clear();
  (*pathLength1_tr).clear();
  (*csize0_tr).clear();
  (*ccol0_tr).clear();
  (*ccol1_tr).clear();
  (*crow0_tr).clear();
  (*crow1_tr).clear();
  (*crphi0_tr).clear();
  (*crphi1_tr).clear();
  (*cz0_tr).clear();
  (*cz1_tr).clear();
  
  (*csize1_tr).clear();
  (*eta0F_tr).clear();
  (*phi0F_tr).clear();
  (*eta1F_tr).clear();
  (*phi1F_tr).clear();
  (*deta_tr).clear();
  (*dphi_tr).clear();
  (*detaF_tr).clear();
  (*dphiF_tr).clear();
  (*etaT).clear();
  (*phiT).clear();
  (*ptT).clear();
  (*pT).clear();
  (*pxT).clear();
  (*pyT).clear();
  (*pzT).clear();
  (*nparentT).clear();
  (*parentTid).clear();
  (*partTid).clear();
  //parameters for all the clusters in layer 0 and 1
  (*eta0_all).clear();
  (*phi0_all).clear();
  (*eta1_all).clear();
  (*phi1_all).clear();
  (*eta2_all).clear();
  (*phi2_all).clear();
  (*dedx0_all).clear();
  (*dedx1_all).clear();
  (*dedx2_all).clear();
  (*pathLength0_all).clear();
  (*pathLength1_all).clear();
  (*pathLength2_all).clear();
  (*csize0_all).clear();
  (*csize1_all).clear();
  (*ccol0_all).clear();
  (*ccol1_all).clear();
  (*crow0_all).clear();
  (*crow1_all).clear();
  (*crphi0_all).clear();
  (*crphi1_all).clear();
  (*cz0_all).clear();
  (*cz1_all).clear();
  //ganged flag
  (*ganged0_tr).clear();
  (*ganged1_tr).clear();

  //all coordination to test overlap
  (*x0_all).clear();
  (*y0_all).clear();
  (*z0_all).clear();
  (*x1_all).clear();
  (*y1_all).clear();
  (*z1_all).clear();
  (*x0_tr).clear();
  (*y0_tr).clear();
  (*z0_tr).clear();
  (*x1_tr).clear();
  (*y1_tr).clear();
  (*z1_tr).clear();
  //
  (*nPixelClustersT).clear();
  (*nBLayerClustersT).clear();
  (*x0_true).clear();
  (*y0_true).clear();
  (*z0_true).clear();
  (*eta0_true).clear();
  (*phi0_true).clear();
  (*x1_true).clear();
  (*y1_true).clear();
  (*z1_true).clear();
  (*eta1_true).clear();
  (*phi1_true).clear();
  (*deta_true).clear();
  (*dphi_true).clear();
  //
  //(*pdgid).clear();
  (*prod_vrtxT).clear();
  (*prod_vrtyT).clear();
  (*prod_vrtzT).clear();
  //primary decay label
  (*primaryDecayLabel).clear();
  //
  for(int i=0; i<3; i++ ) {
    (*unassociated0[i]).clear();
    (*unassociated1[i]).clear();
    (*unassociated2[i]).clear();
  }
  primaryGenParts.clear();
  weakDecayParts.clear();
  genPartCol.clear();
  weakDecayCol.clear();
  threeLayerTrkMap.clear();
  //
  
  //
  (*projected_eff).clear();
  (*phi_eff).clear();
  (*eta_eff).clear();
  (*pt_eff).clear();

  (*eta_ca).clear();
  (*phi_ca).clear();
  (*ene_ca).clear();
  
}

void MakePixelTracklets::ZVrtCollection(std::vector<double> & zVrts) {
  for(unsigned int i0=0; i0<clusterLayer[0].size(); i0++){
    const PixelCluster& clusi0 = clusterLayer[0][i0];
    double xi0 = clusi0.globalPosition().x() - vx_ref;
    double yi0 = clusi0.globalPosition().y() - vy_ref;
    double phii0 = atan2(yi0,xi0);
    double ri0 = sqrt(xi0*xi0+yi0*yi0);
    double zi0 = clusi0.globalPosition().z();
    for(unsigned int i1=0; i1<clusterLayer[1].size(); i1++) {
      const PixelCluster& clusi1 = clusterLayer[1][i1];
      double xi1 = clusi1.globalPosition().x() - vx_ref;
      double yi1 = clusi1.globalPosition().y() - vy_ref;
      double phii1 = atan2(yi1,xi1);
      double ri1 = sqrt(xi1*xi1+yi1*yi1);
      double zi1 = clusi1.globalPosition().z();
      
      double dphi = phii0 - phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      if( fabs(dphi)<vrt_phi_cut ) {
	double zCan = zi0-ri0*(zi0-zi1)/(ri0-ri1);
	zVrts.push_back(zCan);
      }
    }
    for(unsigned int i1=0; i1<clusterLayer[2].size(); i1++) {
      const PixelCluster& clusi1 = clusterLayer[2][i1];
      double xi1 = clusi1.globalPosition().x() - vx_ref;
      double yi1 = clusi1.globalPosition().y() - vy_ref;
      double phii1 = atan2(yi1,xi1);
      double ri1 = sqrt(xi1*xi1+yi1*yi1);
      double zi1 = clusi1.globalPosition().z();
      
      double dphi = phii0 - phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      if( fabs(dphi)<vrt_phi_cut ) {
	double zCan = zi0-ri0*(zi0-zi1)/(ri0-ri1);
	zVrts.push_back(zCan);
      }
    }
  }
  //log << MSG::DEBUG << "n0:n1:n2 " << clusterLayer0.size() << ":" << clusterLayer1.size() << ":" << clusterLayer2.size() << "# of candidate: " << zVrts.size() << endreq;
}

int MakePixelTracklets::ZVrtFinder(std::vector<double> & zVrts){
  MsgStream log(msgSvc(),name());
  int zsize = zVrts.size();
  bool exchange;
  for(int iz=0; iz<zsize; iz++) {
    exchange = false;
    for(int jz=iz+1; jz<zsize; jz++) {
      if( zVrts[jz]<zVrts[iz] ){
	exchange = true;
	double temp = zVrts[iz];
	zVrts[iz] = zVrts[jz];
	zVrts[jz] = temp;
      }
    }
    if( exchange == false ) break;
  }
  
  // log << MSG::DEBUG << "after sorting: " << endreq;
//   for(int iz=0; iz<zsize; iz++) {
//     log << MSG::DEBUG << zVrts[iz] << endreq;
//   }
  
  std::vector<std::set<double> > zClusters;
  for(int iz=0; iz<zsize; iz++) {
    std::set<double> zColls;
    while( fabs(zVrts[iz]-zVrts[iz+1])<vrt_z_cut )
      {
	zColls.insert(zVrts[iz]);
	zColls.insert(zVrts[iz+1]);
	iz++;
      }
    if( zColls.size()!=0 ) zClusters.push_back(zColls);
  }
  if( zClusters.size()==0 ) {
    //log << MSG::DEBUG << " ZVrtFinder() failed to find a vertex. " << endreq;
    NumVrtFinder = 0;
    vz_finder = 0.0;
    return 0;
  }
  unsigned int max_clus=0;
  int max_clus_pos = 0;
  for(unsigned int iz=0; iz<zClusters.size(); iz++) {
    std::set<double> zColls = zClusters[iz];
    if( zColls.size()>max_clus ) {
      max_clus_pos = iz;
      max_clus = zColls.size();
    }
  }
  std::set<double> vrtZPos = zClusters[max_clus_pos];
  double zAvg = 0.0;
  std::set<double>::iterator it=vrtZPos.begin();
  for( ; it!=vrtZPos.end(); it++){
    zAvg += *it;
  }
  
  zAvg /= max_clus;
  NumVrtFinder = 1;
  vz_finder = zAvg;
  //log << MSG::DEBUG << "find z vertex position: " << zAvg << endreq;
  return 1;
}

StatusCode MakePixelTracklets::RetrieveCaloClusters() {
  MsgStream log(msgSvc(),name());
  
  StatusCode sc = m_StoreGate->retrieve(m_clusterCont, m_CaloClustersContainerName);
  if( sc.isFailure() || !m_clusterCont ) {
    log << MSG::INFO << "No CaloCluster container found in TDS." << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

void MakePixelTracklets::AnaCaloClusters(){
  MsgStream log(msgSvc(),name());
  const double m_maxAxisAngle = 20*CLHEP::deg; 
  nCalo = m_clusterCont->size();
  log << MSG::DEBUG << "ncalo cluster: " << nCalo << endreq;
  log << MSG::DEBUG << "vertex x,y,z=" << vx << "," << vy << "," << vz << endreq;
  CaloClusterContainer::const_iterator itb = m_clusterCont->begin();
  CaloClusterContainer::const_iterator ite = m_clusterCont->end();
  for( ; itb!=ite; itb++ ) {
    const CaloCluster* caloclus = *itb;
    //if( caloclus->inBarrel() ) {
      (*eta_ca).push_back(caloclus->eta());
      (*phi_ca).push_back(caloclus->phi());
      (*ene_ca).push_back(caloclus->energy());
      log << MSG::DEBUG << "calo cluster: energy: " << caloclus->energy() << " inBarrel:" << caloclus->inBarrel() << endreq;
      double xc = 0.0;
      double yc = 0.0;
      double zc = 0.0;
      double w = 0.0;
      int ncell = 0;
      CLHEP::HepSymMatrix C(3,0);
      
      CaloCluster::cell_iterator cellIter = caloclus->cell_begin();
      CaloCluster::cell_iterator cellIterEnd = caloclus->cell_end();
      std::vector<double> cellxv;
      std::vector<double> cellyv;
      std::vector<double> cellzv;
      std::vector<double> cellenev;
      bool foundShowerAxis = false;
      for(; cellIter != cellIterEnd; cellIter++ ){
	const CaloCell* pCell = (*cellIter);
	if( !pCell ) {
	  log << MSG::INFO << "ERROR:calo cell not exists! " << endreq;
	  break;
	}
	Identifier myId = pCell->ID();
	const CaloDetDescrElement* myCDDE =
          m_calo_dd_man->get_element(myId);
	double ene = pCell->e();
	double weight = caloclus->getCellWeight(cellIter);
	if( myCDDE && ene>0. && weight>0) {
	  double xi = myCDDE->x();
	  double yi = myCDDE->y();
	  double zi = myCDDE->z();
	  log << MSG::DEBUG << "Cell position (x,y,z)=(" << xi << "," << yi << "," << zi << endreq;
	  cellxv.push_back(xi);
	  cellyv.push_back(yi);
	  cellzv.push_back(zi);
	  cellenev.push_back(ene*weight);
	  xc += ene*weight*xi;
	  yc += ene*weight*yi;
	  zc += ene*weight*zi;
	  w += ene*weight;
	  ncell++;
	}
      }
      if( w>0.0 ) {
	xc /= w;
	yc /= w;
	zc /= w;
	HepGeom::Point3D<double> showerCenter(xc,yc,zc);
	w = 0.0;
	HepGeom::Vector3D<double> showerAxis(xc,yc,zc);
	showerAxis.setMag(1);
	log << MSG::DEBUG << "shower axis before: (x,y,z)=(" << showerAxis.x() << "," << showerAxis.y() << "," << showerAxis.z() << endreq;
	double angle = 0.0;
	if( ncell>2 ) {
	  for(int ic=0; ic<ncell; ic++ ){
	    double xi = cellxv[ic];
	    double yi = cellyv[ic];
	    double zi = cellzv[ic];
	    double energy = cellenev[ic];
	    double e2 = energy*energy;
	    C[0][0] += e2*(xi-xc)*(xi-xc);
	    C[1][0] += e2*(xi-xc)*(yi-yc);
	    C[2][0] += e2*(xi-xc)*(zi-zc);
	    C[1][1] += e2*(yi-yc)*(yi-yc);
	    C[1][2] += e2*(yi-yc)*(zi-zc);
	    C[2][2] += e2*(zi-zc)*(zi-zc);
	    w += e2;
	  }
	  for(int i=0;i<3;i++) {
	    for(int j=0;j<=i;j++) {
	      C[i][j] /= w;
	    }
	  }
	  CLHEP::HepSymMatrix S(C);
	  CLHEP::HepMatrix U = diagonalize(&S);
	  // don't use the principal axes if at least one of the 3 
	  // diagonal elements is 0
	  if ( !( S[0][0] == 0.0 || S[1][1] == 0.0 || S[2][2] == 0.0 ) ) {
	    HepGeom::Vector3D<double> prAxis(showerAxis);
	    int iEigen = -1;
	    for (int i=0;i<3;i++) {
	      if ( S[i][i] != 0 ) {
		HepGeom::Vector3D<double> tmpAxis = HepGeom::Vector3D<double>(U[0][i],U[1][i],U[2][i]);
		double tmpAngle = tmpAxis.angle(showerAxis);
		if ( tmpAngle > 90*CLHEP::deg ) { 
		  tmpAngle = 180*CLHEP::deg - tmpAngle;
		  tmpAxis = -tmpAxis;
		}
		if ( iEigen == -1 || tmpAngle < angle ) {
		  iEigen = i;
		  angle = tmpAngle;
		  prAxis = tmpAxis;
		}
	      }
	    }
	    //double deltaPhi = range.diff(showerAxis.phi(),prAxis.phi());
	    //double deltaTheta = showerAxis.theta() - prAxis.theta();
	    //check the angle
	    if ( angle < m_maxAxisAngle ){
	      showerAxis = prAxis;
	      if(angle!=0.0) foundShowerAxis = true;
	    }
	    else 
	      log << MSG::DEBUG << "principal Direction (" << prAxis.x() << ", "<< prAxis.y() << ", " << prAxis.z() << ") deviates more than "<< m_maxAxisAngle/CLHEP::deg << " CLHEP::deg from IP-to-ClusterCenter-axis (" << showerAxis.x() << ", "<< showerAxis.y() << ", " << showerAxis.z() << ")"<< endreq;
	  }
	}
	if( foundShowerAxis ) {
	  log << MSG::DEBUG << "Shower Axis after= (" << showerAxis.x() << ", "<< showerAxis.y() << ", " << showerAxis.z() << ")" << endreq;
	  log << MSG::DEBUG << "ncell=" << ncell << endreq;
	  log << MSG::DEBUG << "test y,z=" <<yc-showerAxis.y()* (xc-vx)/showerAxis.x()<<"," << zc-showerAxis.z()*(xc-vx)/showerAxis.x() << endreq;
	}
      }
      // }
  }

}
