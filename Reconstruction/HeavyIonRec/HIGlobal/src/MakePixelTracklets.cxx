/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HIGlobal/MakePixelTracklets.h"
#include "HIGlobal/EtaPhiBins.h"
#include "HIGlobal/HIPixelTracklets.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"

#include <vector>
#include <map>
#include <set>
#include <cmath>

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpEventInfo.h"


#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "VxVertex/VxContainer.h"
#include "CLHEP/Geometry/Point3D.h"

#include "InDetIdentifier/PixelID.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "InDetPrepRawData/SiWidth.h"
#include "GaudiKernel/IPartPropSvc.h"

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

MakePixelTracklets::MakePixelTracklets(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),   
  m_pixelID(nullptr),
  m_iBeamCondSvc("BeamCondSvc",name),
  m_calo_dd_man(nullptr),
  m_ths("THistSvc", name),
  m_particleDataTable(nullptr),
  m_tesIO(nullptr),
  m_Ntuple(nullptr),
  m_event_id_run_number(0),
  m_event_id_evt_number(0),
  m_event_id_lumiBlock(0),
  m_event_id_bc_id(0),
  m_trackletsTree(nullptr),
  m_NumVrtRec(0),
  m_NumVrtFinder(0),
  m_vz_finder(0),
  m_vx(0),
  m_vy(0),
  m_vz(0),
  m_sim_vx(0),
  m_sim_vy(0),
  m_sim_vz(0),
  m_ntr(0),
  m_ntrF(0),
  m_ntrT(0),
  m_n0(0),
  m_n1(0),
  m_n2(0),
  m_nCalo(0),
  m_effTree0(nullptr),
  m_effTree1(nullptr)
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
  declareProperty("VrtPhiCut", m_vrt_phi_cut=0.08);
  declareProperty("VrtZCut", m_vrt_z_cut=1.4);
  declareProperty("EtaMax", m_etaMax=2.);
  declareProperty("NtupleLoc", m_ntupleLoc = "AANT");
  declareProperty("MatchR", m_matchR=0.15);
  declareProperty("PtPhi",m_ptphi=0.0111);
  declareProperty("doStartingPoint3",m_doStartingPoint3=true);
  declareProperty("McAvailable",m_mc_available=false);
  declareProperty("doHolesEfficiency", m_doHolesEfficiency=false);
  declareProperty("PRDTruthCollectionPixel",m_multiTruthCollectionPixelName = "PRD_MultiTruthPixel");
  declareProperty("pixelTrackletsCollection",m_pixelTrackletsCollection = "pixelTrackletsCollection");




  NewVectors();
}

MakePixelTracklets::~MakePixelTracklets() 
{
  delete m_tesIO;
  DeleteNewVectors();   
}

StatusCode MakePixelTracklets::initialize() {
  ATH_MSG_DEBUG( "In initialize()" );
  ATH_CHECK( m_ths.retrieve() );
  ATH_CHECK( detStore()->retrieve(m_pixelID,"PixelID") );

  //get the Particle properties service
  ServiceHandle<IPartPropSvc> partPropSvc ("PartPropSvc", name());
  ATH_CHECK( partPropSvc.retrieve() );
  m_particleDataTable = partPropSvc->PDT();

  // Removed for sake of AtlasReconstruction
  if( m_mc_available )
    m_tesIO = new TruthHelper::GenAccessIO();


  ATH_CHECK( m_iBeamCondSvc.retrieve() );

  //pointer to detector manager
  m_calo_dd_man = CaloDetDescrManager::instance();
  
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
#if 0
  for(int idead=0; idead<5; idead++){
    deadLayer0.push_back(etaPhiModule0[idead]);
    deadLayer1.push_back(etaPhiModule1[idead]);
  }
#endif
  

  //output tree for tracklets
  m_trackletsTree = new TTree("trackletsTree","tracklet parameters");
  m_trackletsTree->Branch("NumVrtRec",&m_NumVrtRec);
  m_trackletsTree->Branch("NumVrtFinder",&m_NumVrtFinder);
  m_trackletsTree->Branch("vz_finder",&m_vz_finder);
  m_trackletsTree->Branch("event_id_run_number",&m_event_id_run_number);
  m_trackletsTree->Branch("event_id_evt_number",&m_event_id_evt_number);
  m_trackletsTree->Branch("event_id_lumiBlock",&m_event_id_lumiBlock);
  m_trackletsTree->Branch("event_id_bc_id",&m_event_id_bc_id);
  m_trackletsTree->Branch("matched",&m_matched.p);
  m_trackletsTree->Branch("matchedNumber",&m_matchedNumber.p);
  m_trackletsTree->Branch("matchedToWeakDecay",&m_matchedToWeakDecay.p);
  m_trackletsTree->Branch("startingPoint",&m_startingPoint.p);
  m_trackletsTree->Branch("threeLayerTrk",&m_threeLayerTrk.p);
  m_trackletsTree->Branch("deleted",&m_deleted);
  m_trackletsTree->Branch("deletedBecauseOf",&m_deletedBecauseOf);
  m_trackletsTree->Branch("vx",&m_vx);
  m_trackletsTree->Branch("vy",&m_vy);
  m_trackletsTree->Branch("vz",&m_vz);
  m_trackletsTree->Branch("sim_vx",&m_sim_vx);
  m_trackletsTree->Branch("sim_vy",&m_sim_vy);
  m_trackletsTree->Branch("sim_vz",&m_sim_vz);
  m_trackletsTree->Branch("x0_all",&m_x0_all);
  m_trackletsTree->Branch("y0_all",&m_y0_all);
  m_trackletsTree->Branch("z0_all",&m_z0_all);
  m_trackletsTree->Branch("x1_all",&m_x1_all);
  m_trackletsTree->Branch("y1_all",&m_y1_all);
  m_trackletsTree->Branch("z1_all",&m_z1_all);
  m_trackletsTree->Branch("x0",&m_x0_tr);
  m_trackletsTree->Branch("y0",&m_y0_tr);
  m_trackletsTree->Branch("z0",&m_z0_tr);
  m_trackletsTree->Branch("x1",&m_x1_tr);
  m_trackletsTree->Branch("y1",&m_y1_tr);
  m_trackletsTree->Branch("z1",&m_z1_tr);
  m_trackletsTree->Branch("ntr",&m_ntr);
  m_trackletsTree->Branch("multip",&m_multipTrks);
  m_trackletsTree->Branch("ntrF",&m_ntrF);
  m_trackletsTree->Branch("ntrT",&m_ntrT);
  m_trackletsTree->Branch("n0",&m_n0);
  m_trackletsTree->Branch("n1",&m_n1);
  m_trackletsTree->Branch("n2",&m_n2);
  m_trackletsTree->Branch("pt",&m_pt_tr);
  m_trackletsTree->Branch("p",&m_p_tr);
  m_trackletsTree->Branch("px",&m_px_tr);
  m_trackletsTree->Branch("py",&m_py_tr);
  m_trackletsTree->Branch("pz",&m_pz_tr);
  m_trackletsTree->Branch("part_id",&m_partid_tr);
  m_trackletsTree->Branch("parent_id",&m_parentid_tr);
  m_trackletsTree->Branch("eta0_all",&m_eta0_all);
  m_trackletsTree->Branch("phi0_all",&m_phi0_all);
  m_trackletsTree->Branch("eta1_all",&m_eta1_all);
  m_trackletsTree->Branch("phi1_all",&m_phi1_all);
  m_trackletsTree->Branch("eta2_all",&m_eta2_all);
  m_trackletsTree->Branch("phi2_all",&m_phi2_all);
  m_trackletsTree->Branch("dedx0_all",&m_dedx0_all);
  m_trackletsTree->Branch("dedx1_all",&m_dedx1_all);
  m_trackletsTree->Branch("dedx2_all",&m_dedx2_all);
  m_trackletsTree->Branch("pathLength0_all",&m_pathLength0_all);
  m_trackletsTree->Branch("pathLength1_all",&m_pathLength1_all);
  m_trackletsTree->Branch("pathLength2_all",&m_pathLength2_all);
  m_trackletsTree->Branch("csize0_all",&m_csize0_all);
  m_trackletsTree->Branch("csize1_all",&m_csize1_all);
  m_trackletsTree->Branch("ccol0_all",&m_ccol0_all);
  m_trackletsTree->Branch("ccol1_all",&m_ccol1_all);
  m_trackletsTree->Branch("crow0_all",&m_crow0_all);
  m_trackletsTree->Branch("crow1_all",&m_crow1_all);
  m_trackletsTree->Branch("crphi0_all",&m_crphi0_all);
  m_trackletsTree->Branch("crphi1_all",&m_crphi1_all);
  m_trackletsTree->Branch("cz0_all",&m_cz0_all);
  m_trackletsTree->Branch("cz1_all",&m_cz1_all);
  m_trackletsTree->Branch("eta0",&m_eta0_tr);
  m_trackletsTree->Branch("phi0",&m_phi0_tr);
  m_trackletsTree->Branch("eta1",&m_eta1_tr);
  m_trackletsTree->Branch("phi1",&m_phi1_tr);
  m_trackletsTree->Branch("csize0",&m_csize0_tr);
  m_trackletsTree->Branch("csize1",&m_csize1_tr);
  m_trackletsTree->Branch("ccol0",&m_ccol0_tr);
  m_trackletsTree->Branch("ccol1",&m_ccol1_tr);
  m_trackletsTree->Branch("crow0",&m_crow0_tr);
  m_trackletsTree->Branch("crow1",&m_crow1_tr);
  m_trackletsTree->Branch("crphi0",&m_crphi0_tr);
  m_trackletsTree->Branch("crphi1",&m_crphi1_tr);
  m_trackletsTree->Branch("cz0",&m_cz0_tr);
  m_trackletsTree->Branch("cz1",&m_cz1_tr);
  m_trackletsTree->Branch("dedx0",&m_dedx0_tr);
  m_trackletsTree->Branch("dedx1",&m_dedx1_tr);
  m_trackletsTree->Branch("pathLength0",&m_pathLength0_tr);
  m_trackletsTree->Branch("pathLength1",&m_pathLength1_tr);
  m_trackletsTree->Branch("eta0F",&m_eta0F_tr);
  m_trackletsTree->Branch("phi0F",&m_phi0F_tr);
  m_trackletsTree->Branch("eta1F",&m_eta1F_tr);
  m_trackletsTree->Branch("phi1F",&m_phi1F_tr);
  m_trackletsTree->Branch("deta",&m_deta_tr);
  m_trackletsTree->Branch("dphi",&m_dphi_tr);
  m_trackletsTree->Branch("detaF",&m_detaF_tr);
  m_trackletsTree->Branch("dphiF",&m_dphiF_tr);
  m_trackletsTree->Branch("etaT",&m_etaT);
  m_trackletsTree->Branch("phiT",&m_phiT);
  m_trackletsTree->Branch("ptT",&m_ptT);
  m_trackletsTree->Branch("pT",&m_pT);
  m_trackletsTree->Branch("pxT",&m_pxT);
  m_trackletsTree->Branch("pyT",&m_pyT);
  m_trackletsTree->Branch("pzT",&m_pzT);
  m_trackletsTree->Branch("partT_id",&m_partTid);
  m_trackletsTree->Branch("nparentT",&m_nparentT);
  m_trackletsTree->Branch("parentT_id",&m_parentTid);
  m_trackletsTree->Branch("ganged0",&m_ganged0_tr);
  m_trackletsTree->Branch("ganged1",&m_ganged1_tr);
  m_trackletsTree->Branch("nPixelClustersT",&m_nPixelClustersT);
  m_trackletsTree->Branch("nBLayerClustersT",&m_nBLayerClustersT);
  m_trackletsTree->Branch("x0_true",&m_x0_true);
  m_trackletsTree->Branch("y0_true",&m_y0_true);
  m_trackletsTree->Branch("z0_true",&m_z0_true);
  m_trackletsTree->Branch("x1_true",&m_x1_true);
  m_trackletsTree->Branch("y1_true",&m_y1_true);
  m_trackletsTree->Branch("z1_true",&m_z1_true);
  m_trackletsTree->Branch("eta0_true",&m_eta0_true);
  m_trackletsTree->Branch("phi0_true",&m_phi0_true);
  m_trackletsTree->Branch("eta1_true",&m_eta1_true);
  m_trackletsTree->Branch("phi1_true",&m_phi1_true);
  m_trackletsTree->Branch("deta_true",&m_deta_true);
  m_trackletsTree->Branch("dphi_true",&m_dphi_true);
  //m_trackletsTree->Branch("pdgid",&m_pdgid);
  m_trackletsTree->Branch("prod_vrtxT",&m_prod_vrtxT);
  m_trackletsTree->Branch("prod_vrtyT",&m_prod_vrtyT);
  m_trackletsTree->Branch("prod_vrtzT",&m_prod_vrtzT);
  m_trackletsTree->Branch("primaryDecayLabel",&m_primaryDecayLabel);
  m_trackletsTree->Branch("unassociated0_layer0",&m_unassociated0[0]);
  m_trackletsTree->Branch("unassociated0_layer1",&m_unassociated0[1]);
  m_trackletsTree->Branch("unassociated0_layer2",&m_unassociated0[2]);
  m_trackletsTree->Branch("unassociated1_layer0",&m_unassociated1[0]);
  m_trackletsTree->Branch("unassociated1_layer1",&m_unassociated1[1]);
  m_trackletsTree->Branch("unassociated1_layer2",&m_unassociated1[2]);
  m_trackletsTree->Branch("unassociated2_layer0",&m_unassociated2[0]);
  m_trackletsTree->Branch("unassociated2_layer1",&m_unassociated2[1]);
  m_trackletsTree->Branch("unassociated2_layer2",&m_unassociated2[2]);
  m_trackletsTree->Branch("nCalo",&m_nCalo);
  m_trackletsTree->Branch("eta_ca",&m_eta_ca);
  m_trackletsTree->Branch("phi_ca",&m_phi_ca);
  m_trackletsTree->Branch("ene_ca",&m_ene_ca);

  m_effTree0 = new TTree("effTree0","Efficiency Tree for detecting holes in layer 0");
  m_effTree0->Branch("NumVrtRec",&m_NumVrtRec);
  m_effTree0->Branch("event_id_run_number",&m_event_id_run_number);
  m_effTree0->Branch("event_id_evt_number",&m_event_id_evt_number);
  m_effTree0->Branch("event_id_lumiBlock",&m_event_id_lumiBlock);
  m_effTree0->Branch("event_id_bc_id",&m_event_id_bc_id);
  m_effTree0->Branch("projected",&m_projected_eff);
  m_effTree0->Branch("phiRef",&m_phi_eff);
  m_effTree0->Branch("etaRef",&m_eta_eff);
  m_effTree0->Branch("pt",&m_pt_eff);
  m_effTree1 = (TTree*)m_effTree0->Clone("effTree1");
  m_effTree1->SetTitle("Efficiency Tree for detectin holes in layer 1");
  m_effTree0->CopyAddresses(m_effTree1);
  
    
  ATH_CHECK( m_ths->regTree("/"+m_ntupleLoc+"/tracklets/trackletsTree",m_trackletsTree) );
  ATH_CHECK( m_ths->regTree("/"+m_ntupleLoc+"/tracklets/effTree0",m_effTree0) );
  ATH_CHECK( m_ths->regTree("/"+m_ntupleLoc+"/tracklets/effTree1",m_effTree1) );
  return StatusCode::SUCCESS;
}

StatusCode MakePixelTracklets::finalize() {
  ATH_MSG_DEBUG( "In finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode MakePixelTracklets::execute() {
  const DataHandle<EventInfo> eventInfo;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  int eventNumber= eventInfo->event_ID()->event_number();
  ATH_MSG_DEBUG( "event number " << eventNumber);
  
  const VxContainer *vxc = nullptr;
  ATH_CHECK( evtStore()->retrieve( vxc, "VxPrimaryCandidate" ) );

  //some events may fail to find a vertex, how to take this into consideration??
  //excluding dummy vertex, from PUVertex.cxx
  //This event fails to reconstruct a vertex 
  if( vxc->empty() )
    {
      ATH_MSG_INFO( "Event failed to produce a vertex!" );
      m_NumVrtRec = 0;
      //m_trackletsTree->Fill();
      if( m_doHolesEfficiency ){
	m_effTree0->Fill();
	m_effTree1->Fill();
      }
      //return StatusCode::SUCCESS;

      m_vx = 0;
      m_vy = 0;
      m_vz = 0;
    }
  else {
    m_NumVrtRec = vxc->size()-1;
    const Trk::RecVertex& thisVx = (*vxc)[0]->recVertex();
    m_vx = thisVx.position().x();
    m_vy = thisVx.position().y();
    m_vz = thisVx.position().z();
  }

  //const Amg::Vector3D& v_ref = m_iBeamCondSvc->beamVtx().position();
  
  m_NumVrtFinder = 0;
  ClearVectors();
  ClusterLayers_t clusterLayers;
  std::map<int,int> threeLayerTrkMap;
  LayerClusters(clusterLayers); //the first two layer clusters now available
  // LayerClusters();
//   //my own version of z vertex finder
//   std::vector<double> zVrts;
//   if( m_NumVrtRec==0 ) {
//     ZVrtCollection(zVrts, v_ref, clusterLayers);
//     ZVrtFinder(zVrts);
//   }
 
  std::vector<std::vector<PixelCluster> > trackletsCol; //tracklets collection
  if( m_NumVrtRec!=0 ){
    std::vector<std::vector<PixelCluster> > outputTracklets[2];
    MakeTracklets(true,1,clusterLayers,clusterLayers[0],clusterLayers[1],trackletsCol,threeLayerTrkMap);
    for(int i=0; i<3; i++) {
      //typedef std::map<int,int>::iterator mit;
      for(int ic=0; ic<(int)clusterLayers[i].size(); ic++) {
	if( clusterLayers[i].used[ic]==(int)0 ) {  //unassociated hits
	  m_unassociated0[i].push_back(ic);
	}
      }
    }
    
    MakeTracklets(true,2,clusterLayers,clusterLayers[0],clusterLayers[2],outputTracklets[0],threeLayerTrkMap);
    for(int i=0; i<3; i++) {
      for(int ic=0; ic<(int)clusterLayers[i].size(); ic++) {
	if( clusterLayers[i].used[ic]==0 ) {  //unassociated hits
	  m_unassociated1[i].push_back(ic);
        }
      }
    }
    
    
    if( m_doStartingPoint3==true ) {
      MakeTracklets(true,3,clusterLayers,clusterLayers[1],clusterLayers[2],outputTracklets[1],threeLayerTrkMap);
      for(int i=0; i<3; i++) {
	for(int ic=0; ic<(int)clusterLayers[i].size(); ic++) {
	  if( clusterLayers[i].used[ic]==0 ) {  //unassociated hits
	    m_unassociated2[i].push_back(ic);
	  }
	}
      }
    }
    
    ATH_MSG_DEBUG( "01:02:12=" << trackletsCol.size() << ":" << outputTracklets[0].size() << ":" << outputTracklets[1].size() );
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
    ATH_MSG_DEBUG( "tracklets found by MakeTracklets(): " << trackletsCol.size() );
  }
  
  std::vector<const HepMC::GenParticle*> primaryGenParts;
  std::vector<const HepMC::GenParticle*> weakDecayParts;
  std::vector<std::vector<PixelCluster> > genPartCol; //genParticle clusters collection for primaries
  std::vector<std::vector<PixelCluster> > weakDecayCol; //clusters collection for weak decays
  if( m_mc_available )
    {
      ATH_CHECK( RetrieveTruth (clusterLayers,
                                primaryGenParts,
                                weakDecayParts,
                                genPartCol,
                                weakDecayCol) );
      TruthAssociation (trackletsCol,
                        primaryGenParts,
                        weakDecayParts,
                        genPartCol,
                        weakDecayCol);
    }
  
  //ATH_CHECK( AnaCaloClusters() );
  ATH_CHECK( FillNTuple (clusterLayers,
                         trackletsCol,
                         primaryGenParts,
                         genPartCol,
                         threeLayerTrkMap) );
  if( m_NumVrtRec==1 && m_doHolesEfficiency )
    {
      CalEfficiency(0, clusterLayers, threeLayerTrkMap);
      CalEfficiency(1, clusterLayers, threeLayerTrkMap);
    }
  
  return StatusCode::SUCCESS;
}

void MakePixelTracklets::LayerClusters (ClusterLayers_t& clusterLayers)
{
  const DataHandle<PixelClusterContainer> clusters;
  InDet::PixelClusterContainer::const_iterator colNext = clusters->begin();
  InDet::PixelClusterContainer::const_iterator colEnd = clusters->end();
  for (; colNext!= colEnd; ++colNext)
    {
      
      InDet::PixelClusterCollection::const_iterator nextCluster = (*colNext)->begin();
      InDet::PixelClusterCollection::const_iterator lastCluster = (*colNext)->end();
      
      for (; nextCluster!=lastCluster; nextCluster++)
        {
          const PixelCluster& cluster = **nextCluster;
          //this is very important to make sure barrel layer cluster is used                   
          int barrel = m_pixelID->barrel_ec(cluster.detectorElement()->identify());
          if( barrel!=0 ) continue; //it must be an endcap           
	  int layer = m_pixelID->layer_disk(cluster.detectorElement()->identify());
	  std::vector<int> etaPhiModule;
	  etaPhiModule.push_back(m_pixelID->eta_module(cluster.detectorElement()->identify()));
	  etaPhiModule.push_back(m_pixelID->phi_module(cluster.detectorElement()->identify()));
	  //bool dead = false;
	  clusterLayers[layer].push_back(cluster);
	  
	}
    }
}

void MakePixelTracklets::MakeTracklets(bool trkoreff, int sumlayer,
                                       ClusterLayers_t& clusterLayers,
                                       const std::vector<PixelCluster>& clusterColl0,const std::vector<PixelCluster>& clusterColl1, std::vector<std::vector<PixelCluster> > & outputTracklets,
                                       std::map<int,int>& threeLayerTrkMap)
{ 
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
      if( clusterLayers[firstLayer].used[i0] ) continue;
      //double rmin = 9999.;
      
      const PixelCluster& clusi0 = clusterColl0[i0];
      double cxi0 = clusi0.globalPosition().x() - m_vx;
      double cyi0 = clusi0.globalPosition().y() - m_vy;
      double czi0 = clusi0.globalPosition().z() - m_vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      
      if(firstLayer==1) ATH_MSG_DEBUG( "innerIndex:" << i0 );
      std::vector<int> minclus1label;
      std::vector<double> rcan;
      std::vector<double> detaCan;
      for(int i1=0; i1<nclus1; i1++)
	{
	  if( clusterLayers[secondLayer].used[i1] ) {
	    continue;
	  }
	  const PixelCluster& clusi1 = clusterColl1[i1];
	  double cxi1 = clusi1.globalPosition().x() - m_vx;
	  double cyi1 = clusi1.globalPosition().y() - m_vy;
	  double czi1 = clusi1.globalPosition().z() - m_vz;
	  double phii1 = atan2(cyi1,cxi1);
	  double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
	  double thetai1 = atan2(ri1,czi1);
	  double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
	  
	  double deta = etai0 - etai1;
	  double dphi = phii0 - phii1;
	  
	  if( dphi>M_PI )  dphi -= 2*M_PI;
	  if( dphi<-M_PI ) dphi += 2*M_PI;
	  if(firstLayer==1 ) ATH_MSG_DEBUG( "outIndex:deta:dphi=" << i1 << ":" << deta << ":" << dphi );
	  if( fabs(deta)<etaCutUsed && fabs(dphi)<phiCutUsed )
	    {
	      minclus1label.push_back(i1);
	      double r = sqrt(deta*deta/(etaCutUsed*etaCutUsed)+dphi*dphi/(phiCutUsed*phiCutUsed) );
	      rcan.push_back(r);
	      detaCan.push_back(deta);
	    }
	} //end of 2nd layer loop
      
      int minclus1can = -1;
      //int multip = rcan.size();
      //if( multip>1 ) multisatisfaction++;
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
      clusterLayers[secondLayer].used[minclus1can]++;
      clusterLayers[firstLayer].used[i0]++;
      std::vector<PixelCluster> twoClusters;
      twoClusters.push_back(clusi0);
      twoClusters.push_back(minClus);
      
      //if(0) ATH_MSG_DEBUG( "innerIndex:outIndex=" << i0 <<":" << minclus1can );
      trackletsColCol.push_back(twoClusters);

      //project to 2 layer if the tracklet comes out from the B-layer and 1 layer
      //frobid projected cluster used again and label the tracklets as three layer tracklets 
      if( sumlayer==1 ) {
	for(unsigned int i2=0; i2<clusterLayers[2].size(); i2++) {
	  const PixelCluster& clusi2 = clusterLayers[2][i2];
          double cxi2 = clusi2.globalPosition().x() - m_vx;
          double cyi2 = clusi2.globalPosition().y() - m_vy;
          double czi2 = clusi2.globalPosition().z() - m_vz;
          double phii2 = atan2(cyi2,cxi2);
          double ri2 = sqrt(cxi2*cxi2+cyi2*cyi2);
          double thetai2 = atan2(ri2,czi2);
          double etai2 = -TMath::Log(TMath::Tan(thetai2/2));
	  
          double deta = etai0 - etai2;
          double dphi = phii0 - phii2;
	  
          if( dphi>M_PI )  dphi -= 2*M_PI;
	  if( dphi<-M_PI ) dphi += 2*M_PI;

          if( fabs(deta)<2.0*etaCutUsed && fabs(dphi)<2.0*phiCutUsed*(122.5-50.5)/(88.5-50.5)){
	    //if(0) ATH_MSG_DEBUG( "projection Index:" << i2 );
	    //three layer cluster
	    clusterLayers[2].used[i2]++;
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
	
	double cx00 = clus00.globalPosition().x() - m_vx;
	double cy00 = clus00.globalPosition().y() - m_vy;
	double r00 = sqrt(cx00*cx00+cy00*cy00);
	double cx10 = clus10.globalPosition().x() - m_vx;
        double cy10 = clus10.globalPosition().y() - m_vy;
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
	
	double cx01 = clus01.globalPosition().x() - m_vx;
	double cy01 = clus01.globalPosition().y() - m_vy;
	double r01 = sqrt(cx01*cx01+cy01*cy01);
	double cx11 = clus11.globalPosition().x() - m_vx;
        double cy11 = clus11.globalPosition().y() - m_vy;
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
// 	if(removed) { m_deleted.push_back(1); m_deletedBecauseOf.push_back(deleteReasonMap[i]); }
// 	else { m_deleted.push_back(0); m_deletedBecauseOf.push_back(-1); }
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
    double cx20 = clus20.globalPosition().x() - m_vx;
    double cy20 = clus20.globalPosition().y() - m_vy;
    double cz20 = clus20.globalPosition().z() - m_vz;
    double phi20 = atan2(cy20,cx20);
    double r20 = sqrt(cx20*cx20+cy20*cy20);
    double theta20 = atan2(r20,cz20);
    double eta20 = -TMath::Log(TMath::Tan(theta20/2));
    
    for(unsigned int ir=0; ir<outputTracklets0.size(); ir++) {
      const PixelCluster *aux = &outputTracklets0[ir][0];
      if( layer02or12==1 || layer02or12==2 ) aux = &outputTracklets0[ir][1];
      const PixelCluster& clus10 = *aux;
      double cx10 = clus10.globalPosition().x() - m_vx;
      double cy10 = clus10.globalPosition().y() - m_vy;
      double cz10 = clus10.globalPosition().z() - m_vz;
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
  const PixelCluster& clus10 = tracklet1[0];
  const PixelCluster& clus11 = tracklet1[1];
  const PixelCluster& clus20 = tracklet2[0];
  const PixelCluster& clus21 = tracklet2[1];
  double cx10 = clus10.globalPosition().x() - m_vx;
  double cy10 = clus10.globalPosition().y() - m_vy;
  double cz10 = clus10.globalPosition().z() - m_vz;
  double phi10 = atan2(cy10,cx10);
  double r10 = sqrt(cx10*cx10+cy10*cy10);
  double theta10 = atan2(r10,cz10);
  double eta10 = -TMath::Log(TMath::Tan(theta10/2));
  double cx11 = clus11.globalPosition().x() - m_vx;
  double cy11 = clus11.globalPosition().y() - m_vy;
  double cz11 = clus11.globalPosition().z() - m_vz;
  double phi11 = atan2(cy11,cx11);
  double r11 = sqrt(cx11*cx11+cy11*cy11);
  double theta11 = atan2(r11,cz11);
  double eta11 = -TMath::Log(TMath::Tan(theta11/2));
  double cx20 = clus20.globalPosition().x() - m_vx;
  double cy20 = clus20.globalPosition().y() - m_vy;
  double cz20 = clus20.globalPosition().z() - m_vz;
  double phi20 = atan2(cy20,cx20);
  double r20 = sqrt(cx20*cx20+cy20*cy20);
  double theta20 = atan2(r20,cz20);
  double eta20 = -TMath::Log(TMath::Tan(theta20/2));
  double cx21 = clus21.globalPosition().x() - m_vx;
  double cy21 = clus21.globalPosition().y() - m_vy;
  double cz21 = clus21.globalPosition().z() - m_vz;
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
    ATH_MSG_DEBUG( "eta0:eta1=" << eta10 << ":" << eta20 );
    ATH_MSG_DEBUG( "gange00:gange10=" << clus10.gangedPixel() << ":" << clus20.gangedPixel() );
    ATH_MSG_DEBUG( "gange01:gange11=" << clus11.gangedPixel() << ":" << clus21.gangedPixel() );
    if(clus10.gangedPixel() && clus20.gangedPixel() ) gangeLayer0=1;
    if(clus11.gangedPixel() && clus21.gangedPixel() ) gangeLayer1=1;
    int phi10M = m_pixelID->phi_module(clus10.detectorElement()->identify()); 
    int eta10M = m_pixelID->eta_module(clus10.detectorElement()->identify());
    int phi11M = m_pixelID->phi_module(clus11.detectorElement()->identify());
    int eta11M = m_pixelID->eta_module(clus11.detectorElement()->identify());
    int phi20M = m_pixelID->phi_module(clus20.detectorElement()->identify());
    int eta20M = m_pixelID->eta_module(clus20.detectorElement()->identify());
    int phi21M = m_pixelID->phi_module(clus21.detectorElement()->identify());
    int eta21M = m_pixelID->eta_module(clus21.detectorElement()->identify());
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

StatusCode MakePixelTracklets::FillNTuple (const ClusterLayers_t& clusterLayers,
                                           std::vector<std::vector<PixelCluster> >& trackletsCol,
                                           const std::vector<const HepMC::GenParticle*>& primaryGenParts,
                                           const std::vector<std::vector<PixelCluster> >& genPartCol,
                                           std::map<int,int>& threeLayerTrkMap)
{
  //
  const DataHandle<EventInfo> eventInfo;
  ATH_CHECK( evtStore()->retrieve(eventInfo) );

  m_event_id_run_number = eventInfo->event_ID()->run_number();
  m_event_id_evt_number = eventInfo->event_ID()->event_number();
  m_event_id_lumiBlock = eventInfo->event_ID()->lumi_block();
  m_event_id_bc_id = eventInfo->event_ID()->bunch_crossing_id();
  
  m_n0 = clusterLayers[0].size();
  m_n1 = clusterLayers[1].size();
  m_n2 = clusterLayers[2].size(); 
  for(int i0=0; i0<m_n0; i0++) {
    const PixelCluster& clusi0 = clusterLayers[0][i0];
    double cxi0 = clusi0.globalPosition().x() - m_vx;
    double cyi0 = clusi0.globalPosition().y() - m_vy;
    double czi0 = clusi0.globalPosition().z() - m_vz;
    double phii0 = atan2(cyi0,cxi0);
    double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
    double thetai0 = atan2(ri0,czi0);
    double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
    m_eta0_all.push_back(etai0);
    m_phi0_all.push_back(phii0);
    m_dedx0_all.push_back(clusi0.energyLoss());
    const InDetDD::SiDetectorElement* siDetectorElt0= clusi0.detectorElement();
    double pathLength = siDetectorElt0->thickness()/((siDetectorElt0->surface().normal().x()*cxi0+siDetectorElt0->surface().normal().y()*cyi0+siDetectorElt0->surface().normal().z()*czi0)/sqrt(cxi0*cxi0+cyi0*cyi0+czi0*czi0));
    m_pathLength0_all.push_back(pathLength);
    m_csize0_all.push_back(clusi0.rdoList().size());
    m_ccol0_all.push_back((clusi0.width().colRow())[0]);
    m_crow0_all.push_back((clusi0.width().colRow())[1]);
    m_crphi0_all.push_back((clusi0.width().widthPhiRZ())[0]);
    m_cz0_all.push_back((clusi0.width().widthPhiRZ())[1]);
    m_x0_all.push_back(clusi0.globalPosition().x());
    m_y0_all.push_back(clusi0.globalPosition().y());
    m_z0_all.push_back(clusi0.globalPosition().z());
  }      
  for(int i1=0; i1<m_n1; i1++) {
    const PixelCluster& clusi1 = clusterLayers[1][i1];
    double cxi1 = clusi1.globalPosition().x() - m_vx;
    double cyi1 = clusi1.globalPosition().y() - m_vy;
    double czi1 = clusi1.globalPosition().z() - m_vz;
    double phii1 = atan2(cyi1,cxi1);
    double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
    double thetai1 = atan2(ri1,czi1);
    double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
    m_eta1_all.push_back(etai1);
    m_phi1_all.push_back(phii1);
    m_dedx1_all.push_back(clusi1.energyLoss());
    const InDetDD::SiDetectorElement* siDetectorElt1= clusi1.detectorElement();
    double pathLength = siDetectorElt1->thickness()/((siDetectorElt1->surface().normal().x()*cxi1+siDetectorElt1->surface().normal().y()*cyi1+siDetectorElt1->surface().normal().z()*czi1)/sqrt(cxi1*cxi1+cyi1*cyi1+czi1*czi1));
    m_pathLength1_all.push_back(pathLength);
    m_csize1_all.push_back(clusi1.rdoList().size());
    m_ccol1_all.push_back((clusi1.width().colRow())[0]);
    m_crow1_all.push_back((clusi1.width().colRow())[1]);
    m_crphi1_all.push_back((clusi1.width().widthPhiRZ())[0]);
    m_cz1_all.push_back((clusi1.width().widthPhiRZ())[1]);
    m_x1_all.push_back(clusi1.globalPosition().x());
    m_y1_all.push_back(clusi1.globalPosition().y());
    m_z1_all.push_back(clusi1.globalPosition().z());
  }
  for(int i2=0; i2<m_n2; i2++) {
    const PixelCluster& clusi2 = clusterLayers[2][i2];
    double cxi2 = clusi2.globalPosition().x() - m_vx;
    double cyi2 = clusi2.globalPosition().y() - m_vy;
    double czi2 = clusi2.globalPosition().z() - m_vz;
    double phii2 = atan2(cyi2,cxi2);
    double ri2 = sqrt(cxi2*cxi2+cyi2*cyi2);
    double thetai2 = atan2(ri2,czi2);
    double etai2 = -TMath::Log(TMath::Tan(thetai2/2));
    m_eta2_all.push_back(etai2);
    m_phi2_all.push_back(phii2);
    m_dedx2_all.push_back(clusi2.energyLoss());
    const InDetDD::SiDetectorElement* siDetectorElt2= clusi2.detectorElement();
    double pathLength = siDetectorElt2->thickness()/((siDetectorElt2->surface().normal().x()*cxi2+siDetectorElt2->surface().normal().y()*cyi2+siDetectorElt2->surface().normal().z()*czi2)/sqrt(cxi2*cxi2+cyi2*cyi2+czi2*czi2));
    m_pathLength2_all.push_back(pathLength);
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
  m_ntr = trackletsCol.size(); 
  //log << MSG::DEBUG << "tracklets number: " << m_ntr << endreq;
  for(int it=0; it<m_ntr; it++)
    {
      m_threeLayerTrk.push_back(threeLayerTrkMap[it]);
      
      const PixelCluster& clusi0 = trackletsCol[it][0];
      const PixelCluster& clusi1 = trackletsCol[it][1];
      m_startingPoint.push_back(m_pixelID->layer_disk(clusi0.detectorElement()->identify())+m_pixelID->layer_disk(clusi1.detectorElement()->identify()));
      double cxi0 = clusi0.globalPosition().x() - m_vx;
      double cyi0 = clusi0.globalPosition().y() - m_vy;
      double czi0 = clusi0.globalPosition().z() - m_vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      double cxi1 = clusi1.globalPosition().x() - m_vx;
      double cyi1 = clusi1.globalPosition().y() - m_vy;
      double czi1 = clusi1.globalPosition().z() - m_vz;
      double phii1 = atan2(cyi1,cxi1);
      double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
      double thetai1 = atan2(ri1,czi1);
      double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
      m_eta0_tr.push_back(etai0);
      m_phi0_tr.push_back(phii0);
      m_eta1_tr.push_back(etai1);
      m_phi1_tr.push_back(phii1);
      m_dedx0_tr.push_back(clusi0.energyLoss());
      m_dedx1_tr.push_back(clusi1.energyLoss());
      const InDetDD::SiDetectorElement* siDetectorElt0= clusi0.detectorElement();
      const InDetDD::SiDetectorElement* siDetectorElt1= clusi1.detectorElement();
      double pathLength0 = siDetectorElt0->thickness()/((siDetectorElt0->surface().normal().x()*cxi0+siDetectorElt0->surface().normal().y()*cyi0+siDetectorElt0->surface().normal().z()*czi0)/sqrt(cxi0*cxi0+cyi0*cyi0+czi0*czi0));
      double pathLength1 = siDetectorElt1->thickness()/((siDetectorElt1->surface().normal().x()*cxi1+siDetectorElt1->surface().normal().y()*cyi1+siDetectorElt1->surface().normal().z()*czi1)/sqrt(cxi1*cxi1+cyi1*cyi1+czi1*czi1));
      m_pathLength0_tr.push_back(pathLength0);
      m_pathLength1_tr.push_back(pathLength1);
      m_csize0_tr.push_back(clusi0.rdoList().size());
      m_csize1_tr.push_back(clusi1.rdoList().size());
      m_ccol0_tr.push_back((clusi0.width().colRow())[0]);
      m_crow0_tr.push_back((clusi0.width().colRow())[1]);
      m_crphi0_tr.push_back((clusi0.width().widthPhiRZ())[0]);
      m_cz0_tr.push_back((clusi0.width().widthPhiRZ())[1]);
      m_ccol1_tr.push_back((clusi1.width().colRow())[0]);
      m_crow1_tr.push_back((clusi1.width().colRow())[1]);
      m_crphi1_tr.push_back((clusi1.width().widthPhiRZ())[0]);
      m_cz1_tr.push_back((clusi1.width().widthPhiRZ())[1]);
      m_x0_tr.push_back(clusi0.globalPosition().x());
      m_y0_tr.push_back(clusi0.globalPosition().y());
      m_z0_tr.push_back(clusi0.globalPosition().z());
      m_x1_tr.push_back(clusi1.globalPosition().x());
      m_y1_tr.push_back(clusi1.globalPosition().y());
      m_z1_tr.push_back(clusi1.globalPosition().z());
      m_deta_tr.push_back(etai0-etai1);
      double dphi = phii0-phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      m_dphi_tr.push_back(dphi);
      
      //double dis = sqrt( (clusi0.globalPosition().x()-clusi1.globalPosition().x())*(clusi0.globalPosition().x()-clusi1.globalPosition().x()) + (clusi0.globalPosition().y()-clusi1.globalPosition().y())*(clusi0.globalPosition().y()-clusi1.globalPosition().y()) + (clusi0.globalPosition().z()-clusi1.globalPosition().z())*(clusi0.globalPosition().z()-clusi1.globalPosition().z()) );
      //double pt = fabs(0.0003*dis/dphi);
      double x0p = clusi0.globalPosition().x()-m_vx;
      double y0p = clusi0.globalPosition().y()-m_vy;
      //double z0p = clusi0.globalPosition().z()-m_vz;
      double x1p = clusi1.globalPosition().x()-m_vx;
      double y1p = clusi1.globalPosition().y()-m_vy;
      //double z1p = clusi1.globalPosition().z()-vz;
      double O1 = 0.5*(y1p-y0p+x1p*x1p/y1p-x0p*x0p/y0p)/(x1p/y1p-x0p/y0p);//circle center x with respect to vx, vy
      double O2 = 0.5*(x1p-x0p+y1p*y1p/x1p-y0p*y0p/x0p)/(y1p/x1p-y0p/x0p);//circle center y with respect to vx, vy
      double radius = sqrt(O1*O1+O2*O2);
      double pt = 0.0006*radius;
      double pz = pt*TMath::SinH(etai0);
      double p = sqrt(pt*pt+pz*pz);
      double px = pt*cos(phii0);
      double py = pt*sin(phii0);
      m_pt_tr.push_back(pt);
      m_p_tr.push_back(p);
      m_px_tr.push_back(px);
      m_py_tr.push_back(py);
      m_pz_tr.push_back(pz);
      
      Rec::TrackParticle* trkp = new Rec::TrackParticle();
      trkp->setPx(px);
      trkp->setPy(py);
      trkp->setPz(pz);
      trkp->setE(p);
      trkpcon->push_back(trkp);

      const HepGeom::Point3D<double> vPosition(m_vx,m_vy,m_vz);
      std::vector<PixelCluster*> twoClusters;
      twoClusters.push_back(&trackletsCol[it][0]);
      twoClusters.push_back(&trackletsCol[it][1]);
      trkcoll->push_back(new HIPixelTracklets(vPosition,twoClusters));
    }
  ATH_CHECK( evtStore()->record(trkpcon,"PixelTrackletTrackParticles") );
  ATH_CHECK( evtStore()->record(trkcoll,m_pixelTrackletsCollection) );
  
  //ATH_MSG_DEBUG( "fill tracklets finished." );
  if( m_mc_available )
    {
      //initialize number of truth tracks
      m_ntrT = 0;
      
      for(std::vector<const HepMC::GenParticle*>::const_iterator pitr = primaryGenParts.begin();pitr != primaryGenParts.end(); pitr++)
	{ 
	  //m_pdgid.push_back((*pitr)->pdg_id());
	  m_partTid.push_back((*pitr)->pdg_id());
	  
	  //13.07.11 Savannah #84284 Scott fix 
	  //int vBarcode = 0;
	  if( (*pitr)->production_vertex() )
	    {
	      const HepMC::GenVertex *pvrt = (*pitr)->production_vertex();
	      int ingoingparticles = pvrt->particles_in_size();
	      m_nparentT.push_back(ingoingparticles);
	      //ATH_MSG_DEBUG( ingoingparticles << " parents particles. " );
	      if( ingoingparticles!=1 ){
		m_parentTid.push_back(0);
	      }
	      else {
		m_parentTid.push_back((*(pvrt->particles_in_const_begin()))->pdg_id());
	      }
	      
	      //13.07.11 Savannah #84284 Scott fix
	      //vBarcode = (*pitr)->production_vertex()->barcode();
	      //ATH_MSG_DEBUG( "vertex barcode: " << vBarcode );
	      //right now only by position to identify true primary
	      m_prod_vrtxT.push_back((*pitr)->production_vertex()->position().x());
	      m_prod_vrtyT.push_back((*pitr)->production_vertex()->position().y());
	      m_prod_vrtzT.push_back((*pitr)->production_vertex()->position().z());
	      if( (*pitr)->production_vertex()->position().z()==m_sim_vz && 
		  (*pitr)->production_vertex()->position().x()==m_sim_vx &&
		  (*pitr)->production_vertex()->position().y()==m_sim_vy )
		
		m_primaryDecayLabel.push_back(0);
	      else
		m_primaryDecayLabel.push_back(1);
	    }
	  else
	    {
	      m_primaryDecayLabel.push_back(-1);
	      m_prod_vrtxT.push_back(-100000);
	      m_prod_vrtyT.push_back(-100000);
	      m_prod_vrtzT.push_back(-100000);
	      m_nparentT.push_back(-100000);
	      m_parentTid.push_back(-100000);
	      ATH_MSG_DEBUG( "gen particle has not production vertex! What does this mean. " );
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
	  
	  m_etaT.push_back(eta);
	  m_phiT.push_back(phi);
	  m_ptT.push_back(pt/1000.);
	  m_pT.push_back(p/1000.);
	  m_pxT.push_back(px/1000.);
	  m_pyT.push_back(py/1000.);
	  m_pzT.push_back(pz/1000.);
	  m_ntrT++;
	}
      
      for(std::vector<std::vector<PixelCluster> >::const_iterator ino = genPartCol\
	    .begin(); ino!=genPartCol.end(); ino++) {
	std::vector<PixelCluster> genClusters = *ino;
	m_nPixelClustersT.push_back(genClusters.size());
	int nBLayerCl = 0;
	for(unsigned int icl=0; icl<genClusters.size(); icl++) {
	  Identifier id = genClusters[icl].identify();
	  if(m_pixelID->layer_disk(id)==0) nBLayerCl++;
	}
	m_nBLayerClustersT.push_back(nBLayerCl);
      }
      //ATH_MSG_DEBUG( "number of genParticles: " << genPartCol.size() );
      // int igenpart=-1;
//       for(std::vector<std::vector<PixelCluster> >::iterator ino = genPartCol.begin(); ino!=genPartCol.end(); ino++)
// 	{
// 	  igenpart++;
// 	  std::vector<PixelCluster> genClusters = *ino;
// 	  //log << MSG::DEBUG << "number of clusters per gen particle. " << genClusters.size() << endreq;
// 	  m_nPixelClustersT.push_back(genClusters.size());
// 	  if( genClusters.size()==0 )
// 	    {
// 	      m_x0_true.push_back(-100);
// 	      m_y0_true.push_back(-100);
// 	      m_z0_true.push_back(-100);
// 	      m_x1_true.push_back(-100);
// 	      m_y1_true.push_back(-100);
// 	      m_z1_true.push_back(-100);
// 	      m_eta0_true.push_back(-100);
// 	      m_phi0_true.push_back(-100);
// 	      m_eta1_true.push_back(-100);
// 	      m_phi1_true.push_back(-100);
// 	      m_deta_true.push_back(-100);
// 	      m_dphi_true.push_back(-100);
// 	    }
// 	  if( genClusters.size()==1 )
// 	    {
// 	      const PixelCluster& clusi0=genClusters[0]; //not necessary the first layer
// 	      int layer = m_pixelID->layer_disk(clusi0.identify());
// 	      double cxi0 = clusi0.globalPosition().x() - m_vx;
//               double cyi0 = clusi0.globalPosition().y() - m_vy;
//               double czi0 = clusi0.globalPosition().z() - m_vz;
//               double phii0 = atan2(cyi0,cxi0);
//               double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
//               double thetai0 = atan2(ri0,czi0);
//               double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
// 	      if( layer==0 ) //first layer
// 		{
// 		  m_x0_true.push_back(clusi0.globalPosition().x());
// 		  m_y0_true.push_back(clusi0.globalPosition().y());
// 		  m_z0_true.push_back(clusi0.globalPosition().z());
// 		  m_eta0_true.push_back(etai0);
// 		  m_phi0_true.push_back(phii0);
// 		  m_x1_true.push_back(-100);
//                   m_y1_true.push_back(-100);
//                   m_z1_true.push_back(-100);
// 		  m_eta1_true.push_back(-100);
// 		  m_phi1_true.push_back(-100);
// 		  m_deta_true.push_back(-100);
// 		  m_dphi_true.push_back(-100);
		  
// 		}
// 	      if( layer==1 )
// 		{
// 		  m_x1_true.push_back(clusi0.globalPosition().x());
//                   m_y1_true.push_back(clusi0.globalPosition().y());
//                   m_z1_true.push_back(clusi0.globalPosition().z());
// 		  m_eta1_true.push_back(etai0);
// 		  m_phi1_true.push_back(phii0);
// 		  m_x0_true.push_back(-100);
//                   m_y0_true.push_back(-100);
//                   m_z0_true.push_back(-100);
// 		  m_eta0_true.push_back(-100);
// 		  m_phi0_true.push_back(-100);
// 		  m_deta_true.push_back(-100);
// 		  m_dphi_true.push_back(-100);
// 		}
// 	    }
// 	  if( genClusters.size()>1 )
// 	    {
// 	      int label0=-1;
// 	      int label1=-1;
// 	      for(unsigned int icl=0; icl<genClusters.size(); icl++)
// 		{
// 		  if(m_pixelID->layer_disk(genClusters[icl].identify())==0)
// 		    label0=icl;
// 		  if(m_pixelID->layer_disk(genClusters[icl].identify())==1)
// 		    label1=icl;
// 		}
// 	      if( label1==-1 ) //both are first layer
// 		{
// 		  const PixelCluster& clusi0=genClusters[label0];
// 		  double cxi0 = clusi0.globalPosition().x() - m_vx;
//                   double cyi0 = clusi0.globalPosition().y() - m_vy;
//                   double czi0 = clusi0.globalPosition().z() - m_vz;
//                   double phii0 = atan2(cyi0,cxi0);
//                   double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
//                   double thetai0 = atan2(ri0,czi0);
//                   double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
// 		  m_x0_true.push_back(clusi0.globalPosition().x());
//                   m_y0_true.push_back(clusi0.globalPosition().y());
//                   m_z0_true.push_back(clusi0.globalPosition().z());
// 		  m_eta0_true.push_back(etai0);
//                   m_phi0_true.push_back(phii0);
// 		  m_x1_true.push_back(-100);
//                   m_y1_true.push_back(-100);
//                   m_z1_true.push_back(-100);
//                   m_eta1_true.push_back(-100);
//                   m_phi1_true.push_back(-100);
//                   m_deta_true.push_back(-100);
//                   m_dphi_true.push_back(-100);
// 		}
// 	      else if ( label0==-1 ) //both are second layer
// 		{
// 		  const PixelCluster& clusi1=genClusters[label1];
// 		  double cxi1 = clusi1.globalPosition().x() - m_vx;
//                   double cyi1 = clusi1.globalPosition().y() - m_vy;
//                   double czi1 = clusi1.globalPosition().z() - m_vz;
//                   double phii1 = atan2(cyi1,cxi1);
//                   double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
//                   double thetai1 = atan2(ri1,czi1);
//                   double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
// 		  m_x1_true.push_back(clusi1.globalPosition().x());
//                   m_y1_true.push_back(clusi1.globalPosition().y());
//                   m_z1_true.push_back(clusi1.globalPosition().z());
// 		  m_eta1_true.push_back(etai1);
//                   m_phi1_true.push_back(phii1);
// 		  m_x0_true.push_back(-100);
//                   m_y0_true.push_back(-100);
//                   m_z0_true.push_back(-100);
//                   m_eta0_true.push_back(-100);
//                   m_phi0_true.push_back(-100);
//                   m_deta_true.push_back(-100);
//                   m_dphi_true.push_back(-100);
// 		}
// 	      else
// 		{
// 		  const PixelCluster& clusi0=genClusters[label0];
// 		  const PixelCluster& clusi1=genClusters[label1];
// 		  double cxi0 = clusi0.globalPosition().x() - m_vx;
// 		  double cyi0 = clusi0.globalPosition().y() - m_vy;
// 		  double czi0 = clusi0.globalPosition().z() - m_vz;
// 		  double phii0 = atan2(cyi0,cxi0);
// 		  double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
// 		  double thetai0 = atan2(ri0,czi0);
// 		  double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
// 		  double cxi1 = clusi1.globalPosition().x() - m_vx;
// 		  double cyi1 = clusi1.globalPosition().y() - m_vy;
// 		  double czi1 = clusi1.globalPosition().z() - m_vz;
// 		  double phii1 = atan2(cyi1,cxi1);
// 		  double ri1 = sqrt(cxi1*cxi1+cyi1*cyi1);
// 		  double thetai1 = atan2(ri1,czi1);
// 		  double etai1 = -TMath::Log(TMath::Tan(thetai1/2));
// 		  m_x0_true.push_back(clusi0.globalPosition().x());
//                   m_y0_true.push_back(clusi0.globalPosition().y());
//                   m_z0_true.push_back(clusi0.globalPosition().z());
// 		  m_eta0_true.push_back(etai0);
// 		  m_phi0_true.push_back(phii0);
// 		  m_x1_true.push_back(clusi1.globalPosition().x());
//                   m_y1_true.push_back(clusi1.globalPosition().y());
//                   m_z1_true.push_back(clusi1.globalPosition().z());
// 		  m_eta1_true.push_back(etai1);
// 		  m_phi1_true.push_back(phii1);
// 		  double detat = etai0-etai1;
// 		  m_deta_true.push_back(detat);
// 		  double dphit = phii0-phii1;
// 		  if( dphit>M_PI ) dphit -= 2*M_PI;
// 		  if( dphit<-M_PI) dphit += 2*M_PI;
// 		  m_dphi_true.push_back(dphit);
// 		}
// 	    }
// 	}
      //log << MSG::DEBUG << "test. genCol end" << endreq;
      //    //used for test to see where are those fake tracklets coming from
//       for(int i=0; i<m_ntr; i++)
// 	{
// 	  if( trackletsMatch[i] ) continue;
// 	  if( (*m_matchedToWeakDecay)[i] ) continue;
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

  //ATH_MSG_DEBUG( "NumVrtRec: " << m_NumVrtRec << " ntr: " << m_ntr << " ntrT: " << m_ntrT );
  //ATH_MSG_DEBUG( "eta0 size: " << m_eta0_tr->size() << " eta0T size: " << m_eta0_true->size() );
  m_trackletsTree->Fill();
  return StatusCode::SUCCESS;
}

StatusCode
MakePixelTracklets::RetrieveTruth (const ClusterLayers_t& clusterLayers,
                                   std::vector<const HepMC::GenParticle*>& primaryGenParts,
                                   std::vector<const HepMC::GenParticle*>& weakDecayParts,
                                   std::vector<std::vector<PixelCluster> >& genPartCol,
                                   std::vector<std::vector<PixelCluster> >& weakDecayCol)
{
  //ATH_MSG_DEBUG( "RetrieveTruth()::beginning. " );
  if( !m_mc_available )
    return StatusCode::SUCCESS;
  //retrieve all primary genparticles
  //get the simulated event position
  const McEventCollection *mcCollection = nullptr;
  ATH_CHECK( evtStore()->retrieve(mcCollection,"TruthEvent") );
  
  //generated primary vertex
  McEventCollection::const_iterator it = mcCollection->begin();
  const HepMC::GenEvent* genEvent = (*it);
  if( genEvent==0 )
    {
      ATH_MSG_INFO( " GenEvent not found. " );
      return StatusCode::FAILURE;
    }
  HepMC::GenEvent::vertex_const_iterator pv;
  pv = genEvent->vertices_begin();
 
  //log << MSG::DEBUG << "num of vertices: " << nvrts << endreq;
  if( evtStore()->contains<PileUpEventInfo>("OverlayEvent")  )
    {
      ATH_MSG_INFO( "more that one mc vertices, pileup events? " );
      ATH_MSG_INFO( "Anyway right now only use the first mc vertex." );
    }
  
  //ATH_MSG_DEBUG( " signal vertex barcode: " << (*pv)->barcode() );
  if( !(*pv) ) {
    ATH_MSG_INFO( "ERROR: Truth vertex not exist!" );
    return StatusCode::SUCCESS;
  }
  m_sim_vx = (*pv)->position().x();
  m_sim_vy = (*pv)->position().y();
  m_sim_vz = (*pv)->position().z();
  
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
      //ATH_MSG_DEBUG( "stable particle barcode: " << part->barcode() );
      if( part->barcode() <200000 ) continue;
      weakDecayParts.push_back(part);
    }
    
  }
  
  //ATH_MSG_DEBUG( "particles from weak decay: " << weakDecayParts.size() );
  //ATH_MSG_DEBUG( " signal vertex x,y,z: " << m_sim_vx << "," << m_sim_vy << "," << m_sim_vz );
  McEventCollection::const_iterator ie = mcCollection->begin();
  ie++;
  for( ; ie!=mcCollection->end(); ie++){
    ATH_MSG_DEBUG( "test. " );
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
	  ATH_MSG_INFO( "no genparticle found" );
	  continue;
	}
      int barcode = part->barcode();
      //ATH_MSG_DEBUG( " barcode: " << barcode );
      
      //  if(fabs((*pitr)->production_vertex()->position().z() - m_vz)>1.0) 
      // 	    continue;
      //primary particle cut???not sure
      if( barcode>200000 ) continue;
      //stable??
      if( part->status()!= 1 ) continue;
      
      //if(fabs((*pitr)->production_vertex()->position().z() - sim_vrt_z)>0.5)
      //ATH_MSG_DEBUG( " vertex z:x:y " << (*pitr)->production_vertex()->position().z() << ":" << (*pitr)->production_vertex()->position().x() << ":" << (*pitr)->production_vertex()->position().y() << " simulation vertex z: " << sim_vrt_z );
      
      Int_t pid = abs((*pitr)->pdg_id());
      // select stable hadrons
      //if (pid!=211&&pid!=321&&pid!=2212) continue; 
      //charged or not
      double charge = 0.0;
      //Is there something wrong with the particle data group. Some (*m_particleDataTable)[pid] doesn't exist and cause crash. e.g. pid=21
      if( (*m_particleDataTable)[pid] )
	charge = (*m_particleDataTable)[pid]->charge();
      else {
        ATH_MSG_INFO( "WARNING: this M_PID is not in particle data table." << pid );
      }
      if( charge==0.0 ) continue;
      double eta = (*pitr)->momentum().eta();
      //only consider particles in the pixel acceptance region              
      if( fabs(eta)>m_etaMax+1.0 ) continue;
      primaryGenParts.push_back(part);
      
      //ATH_MSG_DEBUG( "primary:" << barcode );
    }

  const PRD_MultiTruthCollection *truthCollectionPixel;
  if( evtStore()->contains<PRD_MultiTruthCollection>(m_multiTruthCollectionPixelName)) 
    {
      StatusCode sc = evtStore()->retrieve(truthCollectionPixel, m_multiTruthCollectionPixelName);
      if(sc.isFailure()){
	ATH_MSG_INFO( " could not open PRD_MultiTruthCollection : " <<  m_multiTruthCollectionPixelName );
	sc=StatusCode::SUCCESS; // not having truth information is not a failure
	truthCollectionPixel=0;
	return StatusCode::SUCCESS;
      }
    }
  
  std::multimap<int,PixelCluster> partClustersMap;
  //std::map<Identifier,PixelCluster> clusIDMap;
  //std::set<int> genParticles;
  for(int la=0; la<3; la++) {
    for(unsigned int i0=0; i0<clusterLayers[la].size(); i0++) {
      const PixelCluster& clusi0 = clusterLayers[la][i0];
      Identifier ID = clusi0.identify();
      std::pair<PRD_MultiTruthCollection::const_iterator,PRD_MultiTruthCollection::const_iterator> range;
      range= truthCollectionPixel->equal_range(ID);
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
//       range= truthCollectionPixel->equal_range(ID);
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
//       range= truthCollectionPixel->equal_range(ID);
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
  //ATH_MSG_DEBUG( "weak decay particles. " << weakDecayParts.size() );
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


void MakePixelTracklets::TruthAssociation(const std::vector<std::vector<PixelCluster> >& trackletsCol,
                                          const std::vector<const HepMC::GenParticle*>& primaryGenParts,
                                          const std::vector<const HepMC::GenParticle*>& weakDecayParts,
                                          const std::vector<std::vector<PixelCluster> >& genPartCol,
                                          const std::vector<std::vector<PixelCluster> >& weakDecayCol)
{
  //ATH_MSG_DEBUG( "TruthAssociation()::beginning" );
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
	
	m_matched.push_back(1);
	m_matchedToWeakDecay.push_back(0);
	m_matchedNumber.push_back(ipart);
	m_partid_tr.push_back(pdgid);
	m_parentid_tr.push_back(parentPdgid);
	break;
      }
    }
    
    if( foundPri ) continue;
    else   m_matched.push_back(0); //not matched to primaries
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
	
	m_matchedToWeakDecay.push_back(1);
	m_matchedNumber.push_back(ipart);
	m_partid_tr.push_back(pdgid);
	m_parentid_tr.push_back(parentPdgid);
	break;
      }
    }
    //combinatorics, not matched to parimaries or secondaries
    if( !found ) { 
      m_matchedToWeakDecay.push_back(0);
      m_matchedNumber.push_back(-1);
      m_partid_tr.push_back(0);
      m_parentid_tr.push_back(0);
    }
    
  }

}


void MakePixelTracklets::CalEfficiency(int layer,
                                       ClusterLayers_t& clusterLayers,
                                       std::map<int,int>& threeLayerTrkMap)
{
  //this should be done, because the function was called twice
  m_projected_eff.clear();
  m_phi_eff.clear();
  m_eta_eff.clear();
  m_pt_eff.clear();
  std::vector<PixelCluster> clusterLayerRef;
  std::vector<PixelCluster> clusterLayerHole;
  if( layer==0 )
    {
      for(unsigned int i=0; i<clusterLayers[1].size(); i++)
	clusterLayerRef.push_back(clusterLayers[1][i]);
      for(unsigned int i=0; i<clusterLayers[0].size(); i++)
	clusterLayerHole.push_back(clusterLayers[0][i]);
    }
  else if( layer==1 )
    {
      for(unsigned int i=0; i<clusterLayers[0].size(); i++)
	clusterLayerRef.push_back(clusterLayers[0][i]);
      for(unsigned int i=0; i<clusterLayers[1].size(); i++)
	clusterLayerHole.push_back(clusterLayers[1][i]);
    }
  else {
    ATH_MSG_ERROR( "CalEfficiency() with a wrong input parameter!" );
  }
  std::vector<double> etaHoles;
  std::vector<double> phiHoles;
  for(unsigned int ih=0; ih<clusterLayerHole.size(); ih++)
    {
      const PixelCluster& clusi0 = clusterLayerHole[ih];
      double cxi0 = clusi0.globalPosition().x() - m_vx;
      double cyi0 = clusi0.globalPosition().y() - m_vy;
      double czi0 = clusi0.globalPosition().z() - m_vz;
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
  MakeTracklets(false,0,clusterLayers, clusterLayerRef, clusterLayers[2], trackletsEff,threeLayerTrkMap);
  unsigned int ntrksEff = trackletsEff.size();
  for(unsigned int it=0; it<ntrksEff; it++)
    {
      std::vector<PixelCluster> twoClusters = trackletsEff[it];
      const PixelCluster& clusi0 = twoClusters[0];
      const PixelCluster& clusi1 = twoClusters[1];
      //ATH_MSG_DEBUG( "tracklets first layer:" << m_pixelID->layer_disk(clusi0.identify()));
      double cxi0 = clusi0.globalPosition().x() - m_vx;
      double cyi0 = clusi0.globalPosition().y() - m_vy;
      double czi0 = clusi0.globalPosition().z() - m_vz;
      double phii0 = atan2(cyi0,cxi0);
      double ri0 = sqrt(cxi0*cxi0+cyi0*cyi0);
      double thetai0 = atan2(ri0,czi0);
      double etai0 = -TMath::Log(TMath::Tan(thetai0/2));
      double cxi1 = clusi1.globalPosition().x() - m_vx;
      double cyi1 = clusi1.globalPosition().y() - m_vy;
      double czi1 = clusi1.globalPosition().z() - m_vz;
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
      m_eta_eff.push_back(etai0);
      m_phi_eff.push_back(phii0);
      m_pt_eff.push_back(pt);
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
	m_projected_eff.push_back(1);
      else
	m_projected_eff.push_back(0);
    }

  if( layer==0 )
    m_effTree0->Fill();
  if( layer==1 )
    m_effTree1->Fill();
}

void MakePixelTracklets::NewVectors()
{
}

void MakePixelTracklets::DeleteNewVectors()
{
}

void MakePixelTracklets::ClearVectors()
{
  //clear all vectors that are global variable
   m_multipTrks.clear();
   //log << MSG::DEBUG << "Starting fill trees." << endreq;
  m_matched.clear();
  m_matchedNumber.clear();
  m_matchedToWeakDecay.clear();
  m_startingPoint.clear();
  m_threeLayerTrk.clear();
  m_deleted.clear();
  m_deletedBecauseOf.clear();
  m_pt_tr.clear();
  m_p_tr.clear();
  m_px_tr.clear();
  m_py_tr.clear();
  m_pz_tr.clear();
  m_partid_tr.clear();
  m_parentid_tr.clear();
  m_eta0_tr.clear();
  m_phi0_tr.clear();
  m_eta1_tr.clear();
  m_phi1_tr.clear();
  m_dedx0_tr.clear();
  m_dedx1_tr.clear();
  m_pathLength0_tr.clear();
  m_pathLength1_tr.clear();
  m_csize0_tr.clear();
  m_ccol0_tr.clear();
  m_ccol1_tr.clear();
  m_crow0_tr.clear();
  m_crow1_tr.clear();
  m_crphi0_tr.clear();
  m_crphi1_tr.clear();
  m_cz0_tr.clear();
  m_cz1_tr.clear();
  m_csize1_tr.clear();
  m_eta0F_tr.clear();
  m_phi0F_tr.clear();
  m_eta1F_tr.clear();
  m_phi1F_tr.clear();
  m_deta_tr.clear();
  m_dphi_tr.clear();
  m_detaF_tr.clear();
  m_dphiF_tr.clear();
  m_etaT.clear();
  m_phiT.clear();
  m_ptT.clear();
  m_pT.clear();
  m_pxT.clear();
  m_pyT.clear();
  m_pzT.clear();
  m_nparentT.clear();
  m_parentTid.clear();
  m_partTid.clear();
  //parameters for all the clusters in layer 0 and 1
  m_eta0_all.clear();
  m_phi0_all.clear();
  m_eta1_all.clear();
  m_phi1_all.clear();
  m_eta2_all.clear();
  m_phi2_all.clear();
  m_dedx0_all.clear();
  m_dedx1_all.clear();
  m_dedx2_all.clear();
  m_pathLength0_all.clear();
  m_pathLength1_all.clear();
  m_pathLength2_all.clear();
  m_csize0_all.clear();
  m_csize1_all.clear();
  m_ccol0_all.clear();
  m_ccol1_all.clear();
  m_crow0_all.clear();
  m_crow1_all.clear();
  m_crphi0_all.clear();
  m_crphi1_all.clear();
  m_cz0_all.clear();
  m_cz1_all.clear();
  m_ganged0_tr.clear();
  m_ganged1_tr.clear();

  //all coordination to test overlap
  m_x0_all.clear();
  m_y0_all.clear();
  m_z0_all.clear();
  m_x1_all.clear();
  m_y1_all.clear();
  m_z1_all.clear();
  m_x0_tr.clear();
  m_y0_tr.clear();
  m_z0_tr.clear();
  m_x1_tr.clear();
  m_y1_tr.clear();
  m_z1_tr.clear();
  //
  m_nPixelClustersT.clear();
  m_nBLayerClustersT.clear();
  m_x0_true.clear();
  m_y0_true.clear();
  m_z0_true.clear();
  m_eta0_true.clear();
  m_phi0_true.clear();
  m_x1_true.clear();
  m_y1_true.clear();
  m_z1_true.clear();
  m_eta1_true.clear();
  m_phi1_true.clear();
  m_deta_true.clear();
  m_dphi_true.clear();
  //
  //m_pdgid.clear();
  m_prod_vrtxT.clear();
  m_prod_vrtyT.clear();
  m_prod_vrtzT.clear();
  //primary decay label
  m_primaryDecayLabel.clear();
  //
  for(int i=0; i<3; i++ ) {
    m_unassociated0[i].clear();
    m_unassociated1[i].clear();
    m_unassociated2[i].clear();
  }
  
  m_projected_eff.clear();
  m_phi_eff.clear();
  m_eta_eff.clear();
  m_pt_eff.clear();

  m_eta_ca.clear();
  m_phi_ca.clear();
  m_ene_ca.clear();
  
}

void MakePixelTracklets::ZVrtCollection(std::vector<double> & zVrts,
                                        const Amg::Vector3D& v_ref,
                                        const ClusterLayers_t& clusterLayers)
{
  double vx_ref = v_ref.x();
  double vy_ref = v_ref.y();

  for(unsigned int i0=0; i0<clusterLayers[0].size(); i0++){
    const PixelCluster& clusi0 = clusterLayers[0][i0];
    double xi0 = clusi0.globalPosition().x() - vx_ref;
    double yi0 = clusi0.globalPosition().y() - vy_ref;
    double phii0 = atan2(yi0,xi0);
    double ri0 = sqrt(xi0*xi0+yi0*yi0);
    double zi0 = clusi0.globalPosition().z();
    for(unsigned int i1=0; i1<clusterLayers[1].size(); i1++) {
      const PixelCluster& clusi1 = clusterLayers[1][i1];
      double xi1 = clusi1.globalPosition().x() - vx_ref;
      double yi1 = clusi1.globalPosition().y() - vy_ref;
      double phii1 = atan2(yi1,xi1);
      double ri1 = sqrt(xi1*xi1+yi1*yi1);
      double zi1 = clusi1.globalPosition().z();
      
      double dphi = phii0 - phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      if( fabs(dphi)<m_vrt_phi_cut ) {
	double zCan = zi0-ri0*(zi0-zi1)/(ri0-ri1);
	zVrts.push_back(zCan);
      }
    }
    for(unsigned int i1=0; i1<clusterLayers[2].size(); i1++) {
      const PixelCluster& clusi1 = clusterLayers[2][i1];
      double xi1 = clusi1.globalPosition().x() - vx_ref;
      double yi1 = clusi1.globalPosition().y() - vy_ref;
      double phii1 = atan2(yi1,xi1);
      double ri1 = sqrt(xi1*xi1+yi1*yi1);
      double zi1 = clusi1.globalPosition().z();
      
      double dphi = phii0 - phii1;
      if( dphi>M_PI )  dphi -= 2*M_PI;
      if( dphi<-M_PI ) dphi += 2*M_PI;
      if( fabs(dphi)<m_vrt_phi_cut ) {
	double zCan = zi0-ri0*(zi0-zi1)/(ri0-ri1);
	zVrts.push_back(zCan);
      }
    }
  }
  //ATH_MSG_DEBUG( "n0:n1:n2 " << clusterLayer0.size() << ":" << clusterLayer1.size() << ":" << clusterLayer2.size() << "# of candidate: " << zVrts.size() );
}

int MakePixelTracklets::ZVrtFinder(std::vector<double> & zVrts){
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
  
  // ATH_MSG_DEBUG( "after sorting: " );
//   for(int iz=0; iz<zsize; iz++) {
//     ATH_MSG_DEBUG( zVrts[iz] );
//   }
  
  std::vector<std::set<double> > zClusters;
  for(int iz=0; iz<zsize; iz++) {
    std::set<double> zColls;
    while( fabs(zVrts[iz]-zVrts[iz+1])<m_vrt_z_cut )
      {
	zColls.insert(zVrts[iz]);
	zColls.insert(zVrts[iz+1]);
	iz++;
      }
    if( zColls.size()!=0 ) zClusters.push_back(zColls);
  }
  if( zClusters.size()==0 ) {
    //ATH_MSG_DEBUG( " ZVrtFinder() failed to find a vertex. " );
    m_NumVrtFinder = 0;
    m_vz_finder = 0.0;
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

  if (max_clus != 0)
    zAvg /= max_clus;
  m_NumVrtFinder = 1;
  m_vz_finder = zAvg;
  //ATH_MSG_DEBUG( "find z vertex position: " << zAvg );
  return 1;
}

StatusCode MakePixelTracklets::AnaCaloClusters(){
  const CaloClusterContainer* clusterCont = nullptr;
  ATH_CHECK( evtStore()->retrieve( clusterCont, m_CaloClustersContainerName) );
  const double m_maxAxisAngle = 20*CLHEP::deg; 
  m_nCalo = clusterCont->size();
  ATH_MSG_DEBUG( "ncalo cluster: " << m_nCalo );
  ATH_MSG_DEBUG( "vertex x,y,z=" << m_vx << "," << m_vy << "," << m_vz );
  CaloClusterContainer::const_iterator itb = clusterCont->begin();
  CaloClusterContainer::const_iterator ite = clusterCont->end();
  for( ; itb!=ite; itb++ ) {
    const CaloCluster* caloclus = *itb;
    //if( caloclus->inBarrel() ) {
      m_eta_ca.push_back(caloclus->eta());
      m_phi_ca.push_back(caloclus->phi());
      m_ene_ca.push_back(caloclus->energy());
      ATH_MSG_DEBUG( "calo cluster: energy: " << caloclus->energy() << " inBarrel:" << caloclus->inBarrel() );
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
	  ATH_MSG_INFO( "ERROR:calo cell not exists! " );
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
	  ATH_MSG_DEBUG( "Cell position (x,y,z)=(" << xi << "," << yi << "," << zi );
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
	ATH_MSG_DEBUG( "shower axis before: (x,y,z)=(" << showerAxis.x() << "," << showerAxis.y() << "," << showerAxis.z() );
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
	      ATH_MSG_DEBUG( "principal Direction (" << prAxis.x() << ", "<< prAxis.y() << ", " << prAxis.z() << ") deviates more than "<< m_maxAxisAngle/CLHEP::deg << " CLHEP::deg from IP-to-ClusterCenter-axis (" << showerAxis.x() << ", "<< showerAxis.y() << ", " << showerAxis.z() << ")");
	  }
	}
	if( foundShowerAxis ) {
	  ATH_MSG_DEBUG( "Shower Axis after= (" << showerAxis.x() << ", "<< showerAxis.y() << ", " << showerAxis.z() << ")" );
	  ATH_MSG_DEBUG( "ncell=" << ncell );
	  ATH_MSG_DEBUG( "test y,z=" <<yc-showerAxis.y()* (xc-m_vx)/showerAxis.x()<<"," << zc-showerAxis.z()*(xc-m_vx)/showerAxis.x() );
	}
      }
      // }
  }

  return StatusCode::SUCCESS;
}
