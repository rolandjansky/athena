/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkTop/JetMSVAugmentation.h"

#include "xAODJet/JetContainer.h"
#include "DerivationFrameworkTop/TTbarPlusHeavyFlavorFilterTool.h"

#include "xAODBTagging/SecVtxHelper.h"
#include "xAODTracking/Vertex.h"
#include "xAODBTagging/BTagging.h"


namespace DerivationFramework {


JetMSVAugmentation::JetMSVAugmentation(const std::string& t, const std::string& n, const IInterface* p):
  AthAlgTool(t,n,p)
{

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty("JetCollectionName",m_jetCollectionName="AntiKt4EMTopoJets");
    declareProperty("vertexAlgName",m_vtxAlgName="MSV");

}



JetMSVAugmentation::~JetMSVAugmentation(){}



StatusCode JetMSVAugmentation::initialize(){

  ATH_MSG_INFO("Initialize " );


  return StatusCode::SUCCESS;

}



StatusCode JetMSVAugmentation::finalize(){

  return StatusCode::SUCCESS;

}



StatusCode JetMSVAugmentation::addBranches() const{


  const xAOD::JetContainer* jets;
  if( evtStore()->retrieve( jets, m_jetCollectionName ).isFailure() ) {
    ATH_MSG_ERROR ("Couldn't retrieve jets with key: " << m_jetCollectionName );
    return StatusCode::FAILURE;
  }


  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxmass(m_vtxAlgName+"_vtxmass");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxpt(m_vtxAlgName+"_vtxpt");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxeta(m_vtxAlgName+"_vtxeta");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxphi(m_vtxAlgName+"_vtxphi");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxefrac(m_vtxAlgName+"_vtxefrac");

  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxx(m_vtxAlgName+"_vtxx");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxy(m_vtxAlgName+"_vtxy");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxz(m_vtxAlgName+"_vtxz");

  static SG::AuxElement::Decorator<std::vector<int> > dec_vtxntrk(m_vtxAlgName+"_vtxntrk");
  static SG::AuxElement::Decorator<std::vector<float> > dec_vtxdls(m_vtxAlgName+"_vtxdls");


  for(auto jet : *jets){
    const xAOD::BTagging* bjet = jet->btagging();

    if(!bjet){
      ATH_MSG_WARNING("btagging information not available" );
      continue;
    }

    std::vector< ElementLink< xAOD::VertexContainer > > msvVertices;
    bjet->variable<std::vector<ElementLink<xAOD::VertexContainer> > >(m_vtxAlgName, "vertices", msvVertices);

    std::vector<float> vtx_mass;
    std::vector<float> vtx_pt;
    std::vector<float> vtx_eta;
    std::vector<float> vtx_phi;
    std::vector<float> vtx_efrac;
    std::vector<float> vtx_x;
    std::vector<float> vtx_y;
    std::vector<float> vtx_z;
    std::vector<int> vtx_ntrk;
    std::vector<float> vtx_dls;


    for(auto vtx : msvVertices){//loop in vertices

      int   ntrk = xAOD::SecVtxHelper::VtxNtrk(*vtx);
      float mass = xAOD::SecVtxHelper::VertexMass(*vtx);
      float efrc = xAOD::SecVtxHelper::EnergyFraction(*vtx);
      float pt   = xAOD::SecVtxHelper::Vtxpt(*vtx);
      float eta  = xAOD::SecVtxHelper::Vtxeta(*vtx);
      float phi  = xAOD::SecVtxHelper::Vtxphi(*vtx);
      float dls  = xAOD::SecVtxHelper::VtxnormDist(*vtx);
      float xp   = (*vtx)->x();
      float yp   = (*vtx)->y();
      float zp   = (*vtx)->z();
      // float chi  = (*vtx)->chiSquared();
      // float ndf  = (*vtx)->numberDoF();

      TLorentzVector p;
      p.SetPtEtaPhiM(pt,eta,phi,mass);

      vtx_mass.push_back(mass);
      vtx_pt.push_back(pt);
      vtx_eta.push_back(eta);
      vtx_phi.push_back(phi);
      vtx_efrac.push_back(efrc);
      vtx_x.push_back(xp);
      vtx_y.push_back(yp);
      vtx_z.push_back(zp);
      vtx_ntrk.push_back(ntrk);
      vtx_dls.push_back(dls);
    }

    dec_vtxmass(*bjet)=vtx_mass;
    dec_vtxpt(*bjet)=vtx_pt;
    dec_vtxeta(*bjet)=vtx_eta;
    dec_vtxphi(*bjet)=vtx_phi;
    dec_vtxefrac(*bjet)=vtx_efrac;
    dec_vtxx(*bjet)=vtx_x;
    dec_vtxy(*bjet)=vtx_y;
    dec_vtxz(*bjet)=vtx_z;
    dec_vtxdls(*bjet)=vtx_dls;
    dec_vtxntrk(*bjet)=vtx_ntrk;

  }

  return StatusCode::SUCCESS;

}



} /// namespace
