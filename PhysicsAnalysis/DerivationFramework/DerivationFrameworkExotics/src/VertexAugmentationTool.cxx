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

  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::VertexAugmentationTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
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


  for (auto part: *truth_particles) { 
    if(fabs(part->pdgId())==13 && part->pt()>5.0e3 && part->status()==1 && fabs(part->eta())>2.5){
      std::cout << "particle: " << part->pt() << " eta: " << part->eta() << std::endl;

      for(auto cl : *cls){
	if(fabs(cl->eta())>2.5){
	  hcl_dr->Fill(cl->p4().DeltaR(part->p4()));
	}
	if(cl->p4().DeltaR(part->p4())<0.5){
	  hcl_pt->Fill(cl->pt()/1.0e3);
	  hcl_e->Fill(cl->e()/1.0e3);
	  std::cout <<  "   cluster pT: " <<  cl->pt() << " dR: " << cl->p4().DeltaR(part->p4()) << " etaSize: " << cl->getClusterEtaSize() << " phiSize: " << cl->getClusterPhiSize() << " clSize:" << cl->clusterSize()
		    << " nSample: " << cl->nSamples()
		    << " lay0: " << cl->energyBE(0)
		    << " lay1: " << cl->energyBE(1)
		    << " lay2: " << cl->energyBE(2)
		    << " lay3: " << cl->energyBE(3)
		    << " Endcap?: " << cl->inEndcap()
		    << " Barrel?: " << cl->inBarrel() << std::endl;
	  //for(unsigned iSampl = 0; iSampl<cl->nSamples(); ++iSampl){
	  //std::cout << " samp:"
	  //}
		    //<< " PreSamplerB:" << cl->eSample(0) 
		    //<< " EMB1:" << cl->eSample(1) 
		    //<< " EMB2:" << cl->eSample(2) 
		    //<< " EMB3:" << cl->eSample(3) 
		    //<< " PreSamplerE:" << cl->eSample(4) 
		    //<< " :"<< cl->eSample(5) 
		    //<< " :"<< cl->eSample(6) 
		    //<< " :"<< cl->eSample(7) 
		    //<< " :"<< cl->eSample(8) 
		    //<< " HEC0:"     << cl->eSample(9) 
		    //<< " :"         << cl->eSample(10) 
		    //<< " :"         << cl->eSample(11) 
		    //<< " TileBar0:" << cl->eSample(12) 
		    //<< " :"         << cl->eSample(13) 
		    //<< " :"         << cl->eSample(14) 
		    //<< " TileGap1:" << cl->eSample(15) 
		    //<< " :"         << cl->eSample(16) 
		    //<< " :"         << cl->eSample(17) 
		    //<< " TileExt0:" << cl->eSample(18) 
		    //<< " :"         << cl->eSample(19) 
		    //<< " :"         << cl->eSample(20) 
		    //<< " FCAL0:"    << cl->eSample(21) 
		    //<< " FCAL1:"    << cl->eSample(22) 
		    //<< " FCAL2:"    << cl->eSample(23) 
	    //<< " :" cl->eSample(24) 
	  //<< std::endl;
	}
      }
    }
  }


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

