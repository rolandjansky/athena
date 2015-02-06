/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetCellSubtractorTool.h"
#include "CxxUtils/prefetch.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "HIGlobal/HICaloHelper.h"

HIJetCellSubtractorTool::HIJetCellSubtractorTool(const std::string& myname) : HIJetSubtractorToolBase(myname)
{
  
}

void HIJetCellSubtractorTool::Subtract(xAOD::IParticle::FourMom_t& subtr_mom, const xAOD::IParticle* cl_in, const xAOD::HIEventShapeContainer* shape)
{ 
  //if( cl_in->type() == xAOD::Type::CaloCluster )
  //use static cast, derived type of IParticle checked explicitly upstream
  const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(cl_in);
  float E_cl=0;
  float eta_cl=0;
  float phi_cl=0;

  float eta0=cl->eta0();
  float phi0=cl->phi0();

  //unsigned int eta_phi_index=HICaloHelper::FindEtaPhiBin(cl->eta0(),cl->phi0());
  xAOD::CaloCluster::const_cell_iterator cellIterEnd = cl->cell_end();
  for(xAOD::CaloCluster::const_cell_iterator cellIter=cl->cell_begin(); cellIter != cellIterEnd; cellIter++ )
  {
    CxxUtils::prefetchNext(cellIter, cellIterEnd);	

    unsigned int sample = (CaloSampling::CaloSample) (*cellIter)->caloDDE()->getSampling();
    float eta=(*cellIter)->eta();
    float phi=(*cellIter)->phi();
    float rho=HICaloHelper::GetRho(shape,eta0,sample);

    float mod=1;
    for(auto itr=Harmonics().begin(); itr!=Harmonics().end(); itr++) mod+=HICaloHelper::GetHarmonicModulation(shape,*itr,eta0,phi0,sample);
    //rho*=mod;
    float geoWeight=cellIter.weight();
    float cell_E_w=(*cellIter)->energy()*geoWeight;
    cell_E_w-=rho*HICaloHelper::GetAreaEtaPhi(*cellIter)*geoWeight*std::cosh(eta0);
    //unsigned int sample = (CaloSampling::CaloSample) (*cellIter)->caloDDE()->getSampling();

    
    E_cl+=cell_E_w;
    eta_cl+=cell_E_w*eta;
    phi_cl+=cell_E_w*phi;
     
  }
  if(E_cl < MinEnergyForMoments())
  {
    eta_cl=eta0;
    phi_cl=phi0;
  }
  else
  {
    eta_cl/=E_cl;
    phi_cl/=E_cl;
  }

  float ET_cl=E_cl/std::cosh(eta_cl);
  subtr_mom.SetPxPyPzE(ET_cl*std::cos(phi_cl),ET_cl*std::sin(phi_cl),ET_cl*std::sinh(eta_cl),E_cl);
 
}

