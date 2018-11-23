/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkExotics/VertexAugmentationTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODCore/ShallowCopy.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTracking/VertexContainer.h"
#include "PhotonVertexSelection/PhotonVertexHelpers.h"
#include <vector>

DerivationFramework::VertexAugmentationTool::VertexAugmentationTool(const std::string& t,
                                                            const std::string& n,
                                                            const IInterface* p) :
  AthAlgTool(t, n, p)
{

  declareInterface<DerivationFramework::IAugmentationTool>(this);

  declareProperty("derivationPrefix", m_derivationPrefix = "");
  declareProperty("vertexContainer", m_vertexContainerName = "PrimaryVertices");
}

DerivationFramework::VertexAugmentationTool::~VertexAugmentationTool() {
}

StatusCode DerivationFramework::VertexAugmentationTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ..");
  fout = TFile::Open("fout.root","RECREATE");
  hcl_pt = new TH1F("cl_pt","cl_pt",400,0.0,100.0);
  hcl_e = new TH1F("cl_e","cl_e",400,0.0,100.0);
  hcl_dr = new TH1F("cl_dr","cl_dr",200,0.0,10.0);
  hcl_pt->SetDirectory(fout);
  hcl_e->SetDirectory(fout);
  hcl_dr->SetDirectory(fout);

  tree_muon = new TTree("muon","muon");
  tree_NOTmuon = new TTree("NOTmuon","NOTmuon");
  tree_muon->SetDirectory(fout);
  tree_NOTmuon->SetDirectory(fout);

  m_mu_pt       = new std::vector<float>(0);
  m_mu_eta      = new std::vector<float>(0);
  m_mu_phi      = new std::vector<float>(0);
  m_cl_pt       = new std::vector<float>(0);
  m_cl_eta      = new std::vector<float>(0);
  m_cl_phi      = new std::vector<float>(0);
  m_cl_e        = new std::vector<float>(0);
  m_cl_dr       = new std::vector<float>(0);
  m_cl_sizePhi  = new std::vector<int>(0);
  m_cl_sizeEta  = new std::vector<int>(0);
  m_cl_nSample  = new std::vector<int>(0);
  m_cl_calo     = new std::vector<int>(0);
  m_cl_layer0   = new std::vector<float>(0);
  m_cl_layer1   = new std::vector<float>(0);
  m_cl_layer2   = new std::vector<float>(0);
  m_cl_layer3   = new std::vector<float>(0);
  m_clem_layer0   = new std::vector<float>(0);
  m_clem_layer1   = new std::vector<float>(0);
  m_clem_layer2   = new std::vector<float>(0);
  m_clem_layer3   = new std::vector<float>(0);

  // clusters outside a muon
  o_cl_pt       = new std::vector<float>(0);
  o_cl_eta      = new std::vector<float>(0);
  o_cl_phi      = new std::vector<float>(0);
  o_cl_e        = new std::vector<float>(0);
  o_cl_dr       = new std::vector<float>(0);
  o_cl_sizePhi  = new std::vector<int>(0);
  o_cl_sizeEta  = new std::vector<int>(0);
  o_cl_nSample  = new std::vector<int>(0);
  o_cl_calo     = new std::vector<int>(0);
  o_cl_layer0   = new std::vector<float>(0);
  o_cl_layer1   = new std::vector<float>(0);
  o_cl_layer2   = new std::vector<float>(0);
  o_cl_layer3   = new std::vector<float>(0);
  o_clem_layer0   = new std::vector<float>(0);
  o_clem_layer1   = new std::vector<float>(0);
  o_clem_layer2   = new std::vector<float>(0);
  o_clem_layer3   = new std::vector<float>(0);

  // near a muon
  tree_muon->Branch("mu_pt",&m_mu_pt);
  tree_muon->Branch("mu_eta",&m_mu_eta);
  tree_muon->Branch("mu_phi",&m_mu_phi);
  tree_muon->Branch("cl_pt",&m_cl_pt);
  tree_muon->Branch("cl_eta",&m_cl_eta);
  tree_muon->Branch("cl_phi",&m_cl_phi);
  tree_muon->Branch("cl_e",&m_cl_e);
  tree_muon->Branch("cl_dr",&m_cl_dr);
  tree_muon->Branch("cl_sizePhi",&m_cl_sizePhi);
  tree_muon->Branch("cl_sizeEta",&m_cl_sizeEta);
  tree_muon->Branch("cl_nSample",&m_cl_nSample);
  tree_muon->Branch("cl_calo",&m_cl_calo);
  tree_muon->Branch("cl_layer0",&m_cl_layer0);
  tree_muon->Branch("cl_layer1",&m_cl_layer1);
  tree_muon->Branch("cl_layer2",&m_cl_layer2);
  tree_muon->Branch("cl_layer3",&m_cl_layer3);

  tree_muon->Branch("clem_layer0",&m_clem_layer0);
  tree_muon->Branch("clem_layer1",&m_clem_layer1);
  tree_muon->Branch("clem_layer2",&m_clem_layer2);
  tree_muon->Branch("clem_layer3",&m_clem_layer3);

  // not near a muon
  tree_NOTmuon->Branch("cl_pt",&o_cl_pt);
  tree_NOTmuon->Branch("cl_eta",&o_cl_eta);
  tree_NOTmuon->Branch("cl_phi",&o_cl_phi);
  tree_NOTmuon->Branch("cl_e",&o_cl_e);
  tree_NOTmuon->Branch("cl_dr",&o_cl_dr);
  tree_NOTmuon->Branch("cl_sizePhi",&o_cl_sizePhi);
  tree_NOTmuon->Branch("cl_sizeEta",&o_cl_sizeEta);
  tree_NOTmuon->Branch("cl_nSample",&o_cl_nSample);
  tree_NOTmuon->Branch("cl_calo",&o_cl_calo);
  tree_NOTmuon->Branch("cl_layer0",&o_cl_layer0);
  tree_NOTmuon->Branch("cl_layer1",&o_cl_layer1);
  tree_NOTmuon->Branch("cl_layer2",&o_cl_layer2);
  tree_NOTmuon->Branch("cl_layer3",&o_cl_layer3);
  tree_NOTmuon->Branch("clem_layer0",&o_clem_layer0);
  tree_NOTmuon->Branch("clem_layer1",&o_clem_layer1);
  tree_NOTmuon->Branch("clem_layer2",&o_clem_layer2);
  tree_NOTmuon->Branch("clem_layer3",&o_clem_layer3);

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::VertexAugmentationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  tree_muon->Write();
  tree_NOTmuon->Write();
  fout->Write();
  fout->Close();
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::VertexAugmentationTool::addBranches() const {

  const xAOD::EventInfo* eventInfo = evtStore()->retrieve< const xAOD::EventInfo >();
  if(!eventInfo) {
    ATH_MSG_ERROR ("Couldn't retrieve xAOD::EventInfo");
    return StatusCode::FAILURE;
  }

  float m_sumPt2;
  float m_sumPt;

  // Retrieve PV collection from TEvent
  const xAOD::VertexContainer* vertices = 0;
  const xAOD::Vertex* prime_vertex=0;
  if (evtStore()->retrieve(vertices, m_vertexContainerName).isFailure()) {
    ATH_MSG_WARNING("Couldn't retrieve " << m_vertexContainerName << " from TEvent, returning nullptr.");
    prime_vertex = nullptr;
    return StatusCode::FAILURE;
  }

  //prime_vertex = xAOD::PVHelpers::getHardestVertex(vertices);

  for (auto vertex: *vertices) {
    if (vertex->vertexType() != xAOD::VxType::VertexType::PriVtx and
	vertex->vertexType() != xAOD::VxType::VertexType::PileUp) continue;

      // Set input variables for MVA
      SG::AuxElement::Decorator<float> sumPt(m_derivationPrefix + "sumPt");
      if (sumPt.isAvailable(*vertex)) {
        m_sumPt = sumPt(*vertex);
      } else {
        m_sumPt  = xAOD::PVHelpers::getVertexSumPt(vertex);
	sumPt(*vertex)=m_sumPt;
      }

      //SG::AuxElement::Decorator<float> sumPt2(m_derivationPrefix + "sumPt2");
      //if (sumPt2.isAvailable(*vertex)){
      //	m_sumPt2 = sumPt2(*vertex);
      //}else{
      //  m_sumPt2 = xAOD::PVHelpers::getVertexSumPt(vertex, 2);
      //	sumPt2(*vertex)=m_sumPt2;
      //}

      SG::AuxElement::Decorator<float> Pt(m_derivationPrefix + "Pt");
      SG::AuxElement::Decorator<float> Eta(m_derivationPrefix + "Eta");
      SG::AuxElement::Decorator<float> Phi(m_derivationPrefix + "Phi");
      SG::AuxElement::Decorator<float> M(m_derivationPrefix + "M");
      TLorentzVector v = xAOD::PVHelpers::getVertexMomentum(vertex);
      Pt(*vertex)=v.Pt();
      Eta(*vertex)=v.Eta();
      Phi(*vertex)=v.Phi();
      M(*vertex)=v.M();

  }

  // read in clusters
  const xAOD::CaloClusterContainer* cls = 0;
  if (evtStore()->retrieve(cls, "CaloCalTopoClusters").isFailure()) {
    ATH_MSG_WARNING("Couldn't retrieve CaloCalTopoClusters from TEvent, returning nullptr.");
    return StatusCode::FAILURE;
  }
  //"CaloCalTopoClusters"

  // access the truth particle container
  const xAOD::TruthParticleContainer* truth_particles = 0;    
  if (evtStore()->retrieve(truth_particles, "TruthParticles").isFailure()) {
    ATH_MSG_WARNING("Couldn't retrieve " << m_vertexContainerName << " from TEvent, returning nullptr.");
    return StatusCode::FAILURE;
  }

  // make a new container of clusters with |eta|>2.5.
  //for(auto cl : *cls){
  //  if(fabs(cl->eta())>2.5){
  //
  //  }
  //}

  // sort the clusters by E. then group clusters within 0.5 and remove them from the list. Then afterward veto if the cluster is nearby a truth muon.

  //std::set<unsigned> used;
  //used.insert(iCl);
  //if(used.find(iCl)!=used.end())

  //for(auto cl : *cls){
  //  bool skip=false;
  //  if(fabs(cl->eta())>2.5){
  //    //hcl_dr->Fill(cl->p4().DeltaR(part->p4()));
  //    // veto within 0.9 of a truth muons
  //    for (auto part: *truth_particles) { 
  //	if(fabs(part->pdgId())==13 && part->pt()>5.0e3 && part->status()==1 && fabs(part->eta())>2.5){
  //	  if(cl->p4().DeltaR(part->p4())<0.5){
  //	    skip=true;
  //	    break;
  //	  }
  //	}
  //	if(skip) continue;
  //	// sum the other topoclusters nearby this one with cone 0.5.
  //	
  //
  //    }
  //    
  //  }
  //}

  std::vector<TLorentzVector> my_muons;
  for (auto part: *truth_particles) { 
    if(fabs(part->pdgId())==13 && part->pt()>3.0e3 && part->status()==1 && fabs(part->eta())>2.5){
      if(false) std::cout << "particle: " << part->pt() << " eta: " << part->eta() << std::endl;
      my_muons.push_back(part->p4());
      m_mu_pt ->clear();
      m_mu_eta->clear();
      m_mu_phi->clear();
      m_mu_pt->push_back(part->pt()/1.0e3);
      m_mu_eta->push_back(part->eta());
      m_mu_phi->push_back(part->phi());

      m_cl_pt->clear();
      m_cl_dr->clear();
      m_cl_eta->clear();
      m_cl_phi->clear();
      m_cl_e->clear();
      m_cl_nSample->clear();
      m_cl_sizePhi->clear();
      m_cl_sizeEta->clear();
      m_cl_calo   ->clear();     
      m_cl_layer0 ->clear();     
      m_cl_layer1 ->clear();     
      m_cl_layer2 ->clear();     
      m_cl_layer3 ->clear();     
      m_clem_layer0 ->clear();     
      m_clem_layer1 ->clear();     
      m_clem_layer2 ->clear();     
      m_clem_layer3 ->clear();     
      
      for(auto cl : *cls){
	if(fabs(cl->eta())>2.5){
	  hcl_dr->Fill(cl->p4().DeltaR(part->p4()));
	}
	if(cl->p4().DeltaR(part->p4())<1.2){
   
	  m_cl_pt->push_back(cl->pt()/1.0e3);
	  m_cl_eta->push_back(cl->eta());
	  m_cl_phi->push_back(cl->phi());
	  m_cl_e->push_back(cl->e()/1.0e3);
	  m_cl_dr->push_back(cl->p4().DeltaR(part->p4()));
	  m_cl_nSample->push_back(cl->nSamples());
	  m_cl_sizePhi->push_back(cl->getClusterPhiSize());
	  m_cl_sizeEta->push_back(cl->getClusterEtaSize());
	  int calo_code=0;
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC0))>0 ? 1 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC1))>0 ? 1 <<1 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC2))>0 ? 1 <<2 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC3))>0 ? 1 <<3 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::FCAL0))>0 ? 1 <<4 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::FCAL1))>0 ? 1 <<5 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::FCAL2))>0 ? 1 <<6 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL0))>0 ? 1 << 7 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL1))>0 ? 1 << 8 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL2))>0 ? 1 << 9 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL3))>0 ? 1 << 10 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::Unknown))>0 ? 1 << 11 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::TileExt0))>0 ? 1 << 12 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::TileExt1))>0 ? 1 << 13 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::TileExt2))>0 ? 1 << 14 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::PreSamplerE))>0 ? 1 << 15 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::EME1))>0 ? 1 << 16 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::EME2))>0 ? 1 << 17 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::EME3))>0 ? 1 << 18 :0; 
	  float layer0 = cl->eSample(CaloSampling::HEC0) +cl->eSample(CaloSampling::FCAL0) +cl->eSample(CaloSampling::MINIFCAL0);
	  float layer1 = cl->eSample(CaloSampling::HEC1) +cl->eSample(CaloSampling::FCAL1)+cl->eSample(CaloSampling::MINIFCAL1);
	  float layer2 = cl->eSample(CaloSampling::HEC2) +cl->eSample(CaloSampling::MINIFCAL2);
	  float layer3 = cl->eSample(CaloSampling::HEC3) +cl->eSample(CaloSampling::FCAL2)+cl->eSample(CaloSampling::MINIFCAL3);
	  m_cl_calo->push_back(calo_code);
	  m_cl_layer0->push_back(layer0/1.0e3);
	  m_cl_layer1->push_back(layer1/1.0e3);
	  m_cl_layer2->push_back(layer2/1.0e3);
	  m_cl_layer3->push_back(layer3/1.0e3);

	  m_clem_layer0->push_back(cl->eSample(CaloSampling::PreSamplerE)/1.0e3);
	  m_clem_layer1->push_back(cl->eSample(CaloSampling::EME1)/1.0e3);
	  m_clem_layer2->push_back(cl->eSample(CaloSampling::EME2)/1.0e3);
	  m_clem_layer3->push_back(cl->eSample(CaloSampling::EME3)/1.0e3);

	  hcl_pt->Fill(cl->pt()/1.0e3);
	  hcl_e->Fill(cl->e()/1.0e3);
	  if(false)
	    std::cout <<  "   cluster pT: " <<  cl->pt() << " dR: " << cl->p4().DeltaR(part->p4()) << " etaSize: " << cl->getClusterEtaSize() << " phiSize: " << cl->getClusterPhiSize() << " clSize:" << cl->clusterSize()
		    << " nSample: " << cl->nSamples()
		    << " lay0: " << cl->energyBE(0)
		    << " lay1: " << cl->energyBE(1)
		    << " lay2: " << cl->energyBE(2)
		    << " lay3: " << cl->energyBE(3)
		    << " Endcap?: " << cl->inEndcap()
		    << " Barrel?: " << cl->inBarrel()
		    << " pattern: " << cl->samplingPattern()
		    << std::endl;
	  //for(unsigned iSampl = 0; iSampl<cl->nSamples(); ++iSampl){
	  if(false)
	    std::cout << " samp:"
	  //}
		    << " PreSamplerB:" << cl->eSample(CaloSampling::PreSamplerB)  //<<std::endl;
		    << " EMB1:" << cl->eSample(CaloSampling::EMB1) 
		    << " EMB2:" << cl->eSample(CaloSampling::EMB2) 
		    << " EMB3:" << cl->eSample(CaloSampling::EMB3) 
		    << " PreSamplerE:" << cl->eSample(CaloSampling::PreSamplerE) 
		    << " :"<< cl->eSample(CaloSampling::EME1) 
		    << " :"<< cl->eSample(CaloSampling::EME2) 
		    << " :"<< cl->eSample(CaloSampling::EME3) 
	    //<< " :"<< cl->eSample(CaloSampling::EME4) 
		    << " HEC0:"     << cl->eSample(CaloSampling::HEC0) 
		    << " :"         << cl->eSample(CaloSampling::HEC1) 
		    << " :"         << cl->eSample(CaloSampling::HEC2) 
		    << " :"         << cl->eSample(CaloSampling::HEC3) 
		    << " TileBar0:" << cl->eSample(CaloSampling::TileBar0) 
		    << " :"         << cl->eSample(CaloSampling::TileBar1) 
		    << " :"         << cl->eSample(CaloSampling::TileBar2) 
		    << " TileGap1:" << cl->eSample(CaloSampling::TileGap1) 
		    << " :"         << cl->eSample(CaloSampling::TileGap2) 
		    << " :"         << cl->eSample(CaloSampling::TileGap3) 
		    << " TileExt0:" << cl->eSample(CaloSampling::TileExt0) 
		    << " :"         << cl->eSample(CaloSampling::TileExt1) 
		    << " :"         << cl->eSample(CaloSampling::TileExt2) 
		    << " FCAL0:"    << cl->eSample(CaloSampling::FCAL0) 
		    << " FCAL1:"    << cl->eSample(CaloSampling::FCAL1) 
		    << " FCAL2:"    << cl->eSample(CaloSampling::FCAL2) 
		    << " MiniFCAL0:"    << cl->eSample(CaloSampling::MINIFCAL0) 
		    << " MiniFCAL1:"    << cl->eSample(CaloSampling::MINIFCAL1) 
		    << " MiniFCAL2:"    << cl->eSample(CaloSampling::MINIFCAL2) 
		    << " MiniFCAL3:"    << cl->eSample(CaloSampling::MINIFCAL3) 
		    << " Unk:"    << cl->eSample(CaloSampling::Unknown) 
	    //<< " :" cl->eSample(24) 
		    << std::endl;
	}
      }
      tree_muon->Fill();
    }
  }

  // not near a muon
  unsigned iCl=0;
      o_cl_pt->clear();
      o_cl_dr->clear();
      o_cl_eta->clear();
      o_cl_phi->clear();
      o_cl_e->clear();
      o_cl_nSample->clear();
      o_cl_sizePhi->clear();
      o_cl_sizeEta->clear();
      o_cl_calo   ->clear();     
      o_cl_layer0 ->clear();     
      o_cl_layer1 ->clear();     
      o_cl_layer2 ->clear();     
      o_cl_layer3 ->clear();     
      o_clem_layer0 ->clear();     
      o_clem_layer1 ->clear();     
      o_clem_layer2 ->clear();     
      o_clem_layer3 ->clear();     

  //std::set<unsigned> cl_set;
      for(auto cl : *cls){

	++iCl;
	if(fabs(cl->eta())<2){
	  continue;
	}
	bool skip=false;
	for(unsigned i=0; i<my_muons.size(); ++i){
	  if(my_muons.at(i).DeltaR(cl->p4())<1.2){
	    skip=true;
	    break;
	  }
	}

	//cl_set.insert(iCl);
	if(skip) continue;


	  o_cl_pt->push_back(cl->pt()/1.0e3);
	  o_cl_eta->push_back(cl->eta());
	  o_cl_phi->push_back(cl->phi());
	  o_cl_e->push_back(cl->e()/1.0e3);
	  //o_cl_dr->push_back(cl->p4().DeltaR(part->p4()));
	  o_cl_nSample->push_back(cl->nSamples());
	  o_cl_sizePhi->push_back(cl->getClusterPhiSize());
	  o_cl_sizeEta->push_back(cl->getClusterEtaSize());
	  int calo_code=0;
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC0))>0 ? 1 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC1))>0 ? 1 <<1 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC2))>0 ? 1 <<2 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::HEC3))>0 ? 1 <<3 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::FCAL0))>0 ? 1 <<4 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::FCAL1))>0 ? 1 <<5 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::FCAL2))>0 ? 1 <<6 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL0))>0 ? 1 << 7 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL1))>0 ? 1 << 8 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL2))>0 ? 1 << 9 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::MINIFCAL3))>0 ? 1 << 10 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::Unknown))>0 ? 1 << 11 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::TileExt0))>0 ? 1 << 12 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::TileExt1))>0 ? 1 << 13 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::TileExt2))>0 ? 1 << 14 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::PreSamplerE))>0 ? 1 << 15 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::EME1))>0 ? 1 << 16 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::EME2))>0 ? 1 << 17 :0; 
	  calo_code+= fabs(cl->eSample(CaloSampling::EME3))>0 ? 1 << 18 :0; 
	  float layer0 = cl->eSample(CaloSampling::HEC0) +cl->eSample(CaloSampling::FCAL0) +cl->eSample(CaloSampling::MINIFCAL0);
	  float layer1 = cl->eSample(CaloSampling::HEC1) +cl->eSample(CaloSampling::FCAL1)+cl->eSample(CaloSampling::MINIFCAL1);
	  float layer2 = cl->eSample(CaloSampling::HEC2) +cl->eSample(CaloSampling::MINIFCAL2);
	  float layer3 = cl->eSample(CaloSampling::HEC3) +cl->eSample(CaloSampling::FCAL2)+cl->eSample(CaloSampling::MINIFCAL3);
	  o_cl_calo->push_back(calo_code);
	  o_cl_layer0->push_back(layer0/1.0e3);
	  o_cl_layer1->push_back(layer1/1.0e3);
	  o_cl_layer2->push_back(layer2/1.0e3);
	  o_cl_layer3->push_back(layer3/1.0e3);
	  o_clem_layer0->push_back(cl->eSample(CaloSampling::PreSamplerE)/1.0e3);
	  o_clem_layer1->push_back(cl->eSample(CaloSampling::EME1)/1.0e3);
	  o_clem_layer2->push_back(cl->eSample(CaloSampling::EME2)/1.0e3);
	  o_clem_layer3->push_back(cl->eSample(CaloSampling::EME3)/1.0e3);

	}
      tree_NOTmuon->Fill();


  // add truth muons
  //const xAOD::TruthParticleContainer* mus = 0;  
  ////ANA_CHECK (evtStore()->retrieve( mus, "TruthMuons"));
  //
  //if (evtStore()->retrieve(mus, "TruthMuons").isFailure()) {
  //  ATH_MSG_WARNING("Couldn't retrieve TruthMuons from TEvent, returning nullptr.");
  //  return StatusCode::FAILURE;
  //}
  //
  //for (const auto&  mu_itr : *mus){
  //  if(mu_itr->pt() > 5.0e3 && fabs(mu_itr->eta())>2.6){
  //    std::cout << "TruthMuon: " << mu_itr->pt()/1.0e3 << " eta: " << mu_itr->eta() << std::endl;
  //  }
  //}
  
  return StatusCode::SUCCESS;
}

