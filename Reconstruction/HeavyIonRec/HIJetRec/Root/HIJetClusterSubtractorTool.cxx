/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetClusterSubtractorTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "HIJetRec/IHIUEModulatorTool.h"
#include "PathResolver/PathResolver.h"
#include <TH3F.h>
#include <TFile.h>
#include <iomanip>

HIJetClusterSubtractorTool::HIJetClusterSubtractorTool(const std::string& myname) : HIJetSubtractorToolBase(myname),
										    m_init(false),
										    m_h3_w(nullptr),
										    m_h3_eta(nullptr),
										    m_h3_phi(nullptr)
{
  declareProperty("InputFile",m_input_file="cluster.geo.root","File containing cluster geometric moments.");
  declareProperty("ConfigDir",m_config_dir="HIJetRec/","Directory containing configuration file.");
  setUseCells(false);
}

void HIJetClusterSubtractorTool::Subtract(xAOD::IParticle::FourMom_t& subtr_mom, const xAOD::IParticle* cl_in, const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const ToolHandle<IHIUEModulatorTool>& modulator)
{ 
  if(!m_init) 
  {
    if(initializeTool().isFailure()) return;
  }
  const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(cl_in);
  float E_unsubtr=cl->e(HIJetRec::unsubtractedClusterState());
  float eta_unsubtr=cl->eta(HIJetRec::unsubtractedClusterState());
  float phi_unsubtr=cl->phi(HIJetRec::unsubtractedClusterState());

  float E_subtr=0;
  float E_eta_subtr=0;
  float E_phi_subtr=0;
  float eta0=cl->eta0();
  float phi0=cl->phi0();

  for(unsigned int sample=0; sample<24; sample++)
  {
    CaloSampling::CaloSample s=static_cast<CaloSampling::CaloSample>(sample);
    if(!cl->hasSampling(s)) continue;
    
    float nCells=index->getShape(eta0,sample,shape)->nCells();
    float rho=0;
    if(nCells!=0.) rho=index->getShape(eta0,sample,shape)->rho()/nCells;
    float mod=modulator->getModulation(phi0);
    rho*=mod*std::cosh(eta0);

    E_subtr+=rho*getWeight(eta0,phi0,sample);
    E_eta_subtr+=rho*getWeightEta(eta0,phi0,sample);
    E_phi_subtr+=rho*getWeightPhi(eta0,phi0,sample);
  }


  float energy=E_unsubtr-E_subtr;
  float eta=eta0;
  float phi=phi0;


  if(energy!=0.)
  {
    eta=eta_unsubtr+(eta_unsubtr*E_subtr-E_eta_subtr)/energy;
    phi=phi_unsubtr+(phi_unsubtr*E_subtr-E_phi_subtr)/energy;
  }
  setSubtractedEtaPhi(energy,eta,phi,eta0,phi0,energy/E_subtr);
  float ET=energy/std::cosh(eta);
  subtr_mom.SetPxPyPzE(ET*std::cos(phi),ET*std::sin(phi),ET*std::sinh(eta),energy);
}

void HIJetClusterSubtractorTool::UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const xAOD::CaloCluster* cl)
{


  if(!m_init) 
  {
    if(initializeTool().isFailure()) return;
  }
  float eta0=cl->eta0();
  float phi0=cl->phi0();


  for(unsigned int sample=0; sample<24; sample++)
  {
    CaloSampling::CaloSample s=static_cast<CaloSampling::CaloSample>(sample);
    if(!cl->hasSampling(s)) continue;
    float esamp=cl->eSample(s);
    float ET=esamp/std::cosh(eta0);
    xAOD::HIEventShape* slice=shape->at(index->getIndex(eta0,sample));
    
    //update members
    float area_slice=slice->area();
    float area_cluster=getWeight(eta0,phi0,sample);
    float area_sf=1.;
    if(area_slice!=0.) area_sf-=area_cluster/area_slice;
    slice->setNCells( std::floor(area_sf*slice->nCells()) );
    slice->setEt(slice->et()-ET);
    slice->setArea(area_slice-area_cluster);
    slice->setRho(slice->rho() - ET/area_cluster);


    for(unsigned int ih=0; ih<slice->etCos().size(); ih++)
    {
      float ih_f=ih+1;
      float tmp_cos = slice->etCos().at(ih);
      slice->etCos()[ih] = tmp_cos + cos(ih_f*phi0)*ET;
      
      float tmp_sin = slice->etSin().at(ih);
      slice->etSin()[ih] = tmp_sin + sin(ih_f*phi0)*ET;
    }
  }
  
}

void HIJetClusterSubtractorTool::SubtractWithMoments(xAOD::CaloCluster* cl, const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const ToolHandle<IHIUEModulatorTool>& modulator)
{ 

  xAOD::IParticle::FourMom_t subtr_mom;
  Subtract(subtr_mom, cl, shape, index, modulator);
  HIJetRec::setClusterP4(subtr_mom,cl,HIJetRec::subtractedClusterState());
}

StatusCode HIJetClusterSubtractorTool::initializeTool()
{
  std::string local_path=m_config_dir+m_input_file;
  std::string full_path=PathResolverFindCalibFile(local_path);
  TFile* f=TFile::Open(full_path.c_str());
  if(f==nullptr) 
  {
    ATH_MSG_FATAL("Cannot read config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_w=(TH3F*)f->GetObjectChecked("h3_w","TH3F");
  if(m_h3_w==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_w in config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_eta=(TH3F*)f->GetObjectChecked("h3_eta","TH3F");
  if(m_h3_eta==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_eta in config file " << full_path ); 
    return StatusCode::FAILURE;
  }

  m_h3_phi=(TH3F*)f->GetObjectChecked("h3_phi","TH3F");
  if(m_h3_phi==nullptr) 
  {
    ATH_MSG_FATAL("Cannot find TH3F m_h3_phi in config file " << full_path ); 
    return StatusCode::FAILURE;
  }
  m_h3_w->SetDirectory(0);
  m_h3_eta->SetDirectory(0);
  m_h3_phi->SetDirectory(0);
  f->Close();
  m_init=true;
  return StatusCode::SUCCESS;

}



float HIJetClusterSubtractorTool::getWeight(float eta, float phi, int sample) const
{
  return m_h3_w->GetBinContent(m_h3_w->FindBin(eta,phi,sample));
}
float HIJetClusterSubtractorTool::getWeightEta(float eta, float phi, int sample) const
{
  return m_h3_eta->GetBinContent(m_h3_eta->FindBin(eta,phi,sample));
}
float HIJetClusterSubtractorTool::getWeightPhi(float eta, float phi, int sample) const
{
  return m_h3_phi->GetBinContent(m_h3_phi->FindBin(eta,phi,sample));
}
 
// float HIJetClusterSubtractorTool::getWeight(float eta, float phi, int sample) const
// {
//   return 0.01;
// }
// float HIJetClusterSubtractorTool::getWeightEta(float eta, float phi, int sample) const
// {
//   return 0.01*eta;
// }
// float HIJetClusterSubtractorTool::getWeightPhi(float eta, float phi, int sample) const
// {
//   return 0.01*phi;
// }

