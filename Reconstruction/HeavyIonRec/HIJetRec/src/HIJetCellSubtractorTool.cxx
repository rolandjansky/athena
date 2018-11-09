/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetCellSubtractorTool.h"
#include "HICaloCellHelper.h"
#include "CxxUtils/prefetch.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIJetRec/HIJetRecDefs.h"
#include "FourMomUtils/xAODP4Helpers.h"
//forward class decl in base class.
#include "HIJetRec/IHIUEModulatorTool.h"

HIJetCellSubtractorTool::HIJetCellSubtractorTool(const std::string& myname) : HIJetSubtractorToolBase(myname)
{
  
}

void HIJetCellSubtractorTool::Subtract(xAOD::IParticle::FourMom_t& subtr_mom, const xAOD::IParticle* cl_in, const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const ToolHandle<IHIUEModulatorTool>& modulator, const xAOD::HIEventShape* eshape) const
{ 
/*  if(getIndex()==nullptr || getShape()==nullptr)
  {
    ATH_MSG_ERROR("No HIEventShape/Index supplied, cannot do subtraction");
    return;
  }*/

  //if( cl_in->type() == xAOD::Type::CaloCluster )
  //use static cast, derived type of IParticle checked explicitly upstream
  const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(cl_in);
  float E_cl=0;
  float eta_cl=0;
  float phi_cl=0;

  const float eta0=cl->eta0();
  const float phi0=cl->phi0();

  float mod=1.;
  if(modulator) mod=modulator->getModulation(phi0, eshape);

  //unsigned int eta_phi_index=HICaloCellHelper::FindEtaPhiBin(cl->eta0(),cl->phi0());
  xAOD::CaloCluster::const_cell_iterator cellIterEnd = cl->cell_end();
  for(xAOD::CaloCluster::const_cell_iterator cellIter=cl->cell_begin(); cellIter != cellIterEnd; cellIter++ )
  {
    CxxUtils::prefetchNext(cellIter, cellIterEnd);	

    unsigned int sample = (CaloSampling::CaloSample) (*cellIter)->caloDDE()->getSampling();
    float eta=(*cellIter)->eta();
    float phi=(*cellIter)->phi();

    float nCells=index->getShape(eta0,sample,shape)->nCells();
    float rho=0;
    if(nCells!=0.) rho=index->getShape(eta0,sample,shape)->rho()/nCells;

    rho*=mod;
    float geoWeight=cellIter.weight();
    float cell_E_w=(*cellIter)->energy()*geoWeight;
    cell_E_w-=rho*HICaloCellHelper::GetAreaEtaPhi(*cellIter)*geoWeight*std::cosh(eta0);


    E_cl+=cell_E_w;
    eta_cl+=cell_E_w*eta;
    phi_cl+=cell_E_w*phi;
     
  }
  if(E_cl!=0.)
  {
    eta_cl/=E_cl;
    phi_cl/=E_cl;
  }
  //rare case E_cl==0 is also handled by setSubtractedEtaPhi
  float E_unsubtr=cl->e(HIJetRec::unsubtractedClusterState());
  setSubtractedEtaPhi(E_cl,eta_cl,phi_cl,eta0,phi0,E_cl/E_unsubtr);
  float ET_cl=E_cl/std::cosh(eta_cl);
  subtr_mom.SetPxPyPzE(ET_cl*std::cos(phi_cl),ET_cl*std::sin(phi_cl),ET_cl*std::sinh(eta_cl),E_cl);
}

void HIJetCellSubtractorTool::UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const xAOD::CaloCluster* cl) const
{
  float eta0=cl->eta0();
  float phi0=cl->phi0();

  xAOD::CaloCluster::const_cell_iterator cellIterEnd = cl->cell_end();
  for(xAOD::CaloCluster::const_cell_iterator cellIter=cl->cell_begin(); cellIter != cellIterEnd; cellIter++ )
  {
    CxxUtils::prefetchNext(cellIter, cellIterEnd);	
    UpdateShape(shape,index,*cellIter,cellIter.weight(),eta0,phi0,true);
  }
}

void HIJetCellSubtractorTool::UpdateShape(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const CaloCell* theCell, float geoWeight, float eta0, float phi0, bool isNeg) const
{
  float sgn=(isNeg) ? -1 : 1;

  int layer = theCell->caloDDE()->getSampling();
  float cell_et = theCell->et();

  unsigned int iSlice=index->getIndex(eta0,layer);


  xAOD::HIEventShape* slice=shape->at(iSlice);
  //update members
  slice->setNCells(slice->nCells()+sgn);
  slice->setEt(slice->et()+sgn*cell_et*geoWeight);
  float area=HICaloCellHelper::GetAreaEtaPhi(theCell);
  float rho=0;
  if(area!=0.) rho=cell_et/area;
  slice->setArea(slice->area() + sgn*area*geoWeight);
  slice->setRho(slice->rho() + sgn*rho);

  for(unsigned int ih=0; ih<shape->at(iSlice)->etCos().size(); ih++)
  {
    float ih_f=ih+1;
    float tmp_cos = shape->at(iSlice)->etCos().at(ih);
    shape->at(iSlice)->etCos()[ih] = tmp_cos + cell_et*cos(ih_f*phi0)*geoWeight;
    
    float tmp_sin = shape->at(iSlice)->etSin().at(ih);
    shape->at(iSlice)->etSin()[ih] = tmp_sin + cell_et*sin(ih_f*phi0)*geoWeight;
  }
  
}

void HIJetCellSubtractorTool::SubtractWithMoments(xAOD::CaloCluster* cl, const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const ToolHandle<IHIUEModulatorTool>& modulator, const xAOD::HIEventShape* eshape) const
{ 
  if( index==nullptr || shape==nullptr)
  {
    ATH_MSG_ERROR("No HIEventShape/Index supplied, cannot do subtraction");
    return;
  }

  //if( cl_in->type() == xAOD::Type::CaloCluster )
  //use static cast, derived type of IParticle checked explicitly upstream

  float E_cl=0;
  float eta_cl=0;
  float phi_cl=0;

  const float eta0=cl->eta0();
  const float phi0=cl->phi0();

  float mod=1;
  if(modulator) mod=modulator->getModulation(phi0, eshape);

  std::vector<float> E_sample(CaloSampling::Unknown,0);
  uint32_t samplingPattern=0;
  //unsigned int eta_phi_index=HICaloCellHelper::FindEtaPhiBin(cl->eta0(),cl->phi0());
  xAOD::CaloCluster::cell_iterator cellIterEnd = cl->cell_end();
  for(xAOD::CaloCluster::cell_iterator cellIter=cl->cell_begin(); cellIter != cellIterEnd; cellIter++ )
  {
    CxxUtils::prefetchNext(cellIter, cellIterEnd);	

    unsigned int sample = (CaloSampling::CaloSample) (*cellIter)->caloDDE()->getSampling();
    samplingPattern |= (0x1U<<sample);
    float eta=(*cellIter)->eta();
    float phi=(*cellIter)->phi();

    float nCells=index->getShape(eta0,sample,shape)->nCells();
    float rho=0;
    if(nCells!=0.) rho=index->getShape(eta0,sample,shape)->rho()/nCells;

    rho*=mod;
    float geoWeight=cellIter.weight();
    float cell_E_w=(*cellIter)->energy()*geoWeight;
    cell_E_w-=rho*HICaloCellHelper::GetAreaEtaPhi(*cellIter)*geoWeight*std::cosh(eta0);

    E_cl+=cell_E_w;
    eta_cl+=cell_E_w*eta;
    phi_cl+=cell_E_w*phi;

    E_sample[sample]+=cell_E_w;
  }

  if(E_cl!=0.)
  {
    eta_cl/=E_cl;
    phi_cl/=E_cl;
  }
  //rare case E_cl==0 is also handled by setSubtractedEtaPhi
  float E_unsubtr=cl->e(HIJetRec::unsubtractedClusterState());
  setSubtractedEtaPhi(E_cl,eta_cl,phi_cl,eta0,phi0,E_cl/E_unsubtr);

  float ET_cl=E_cl/std::cosh(eta_cl);
  xAOD::IParticle::FourMom_t subtr_mom;
  subtr_mom.SetPxPyPzE(ET_cl*std::cos(phi_cl),ET_cl*std::sin(phi_cl),ET_cl*std::sinh(eta_cl),E_cl);
  HIJetRec::setClusterP4(subtr_mom,cl,HIJetRec::subtractedClusterState());

  cl->setSamplingPattern(samplingPattern);
  for(unsigned int isample=0; isample < E_sample.size(); isample++)
  {
    if( samplingPattern & (0x1U << isample) )
    {
      float current_energy=E_sample.at(isample);
      xAOD::CaloCluster::CaloSample s=static_cast<xAOD::CaloCluster::CaloSample>(isample);
      cl->setEnergy(s,current_energy);
    }
  }
}

