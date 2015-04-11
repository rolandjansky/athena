/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetCellSubtractorTool.h"
#include "HICaloCellHelper.h"
#include "CxxUtils/prefetch.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "HIEventUtils/HIEventShapeIndex.h"


HIJetCellSubtractorTool::HIJetCellSubtractorTool(const std::string& myname) : HIJetSubtractorToolBase(myname)
{
  
}

void HIJetCellSubtractorTool::Subtract(xAOD::IParticle::FourMom_t& subtr_mom, const xAOD::IParticle* cl_in, const xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index)
{ 

  //if( cl_in->type() == xAOD::Type::CaloCluster )
  //use static cast, derived type of IParticle checked explicitly upstream
  const xAOD::CaloCluster* cl=static_cast<const xAOD::CaloCluster*>(cl_in);
  float E_cl=0;
  float eta_cl=0;
  float phi_cl=0;

  float eta0=cl->eta0();
  float phi0=cl->phi0();

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
    float mod=1;
    // for(auto itr=Harmonics().begin(); itr!=Harmonics().end(); itr++) 
    // {
    // }
    rho*=mod;
    float geoWeight=cellIter.weight();
    float cell_E_w=(*cellIter)->energy()*geoWeight;
    cell_E_w-=rho*HICaloCellHelper::GetAreaEtaPhi(*cellIter)*geoWeight*std::cosh(eta0);
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

void HIJetCellSubtractorTool::UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const HIEventShapeIndex* index, const xAOD::CaloCluster* cl, 
						 std::set<unsigned int>& used_indices, std::set<unsigned int>& used_eta_bins)
{
  float eta0=cl->eta0();
  float phi0=cl->phi0();
  unsigned int tower_index=HI::TowerBins::findEtaPhiBin(eta0,phi0);
  if(used_indices.find(tower_index)==used_indices.end())
  {
    //update event shape!
    xAOD::CaloCluster::const_cell_iterator cellIterEnd = cl->cell_end();
    for(xAOD::CaloCluster::const_cell_iterator cellIter=cl->cell_begin(); cellIter != cellIterEnd; cellIter++ )
    {
      CxxUtils::prefetchNext(cellIter, cellIterEnd);	
      UpdateShape(shape,index,*cellIter,cellIter.weight(),eta0,phi0,true);
    }
    used_indices.insert(tower_index);
    used_eta_bins.insert(HI::TowerBins::findBinEta(eta0));
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
  slice->setNcells(slice->nCells()+sgn);
  slice->setEt(slice->Et()+sgn*cell_et*geoWeight);
  float area=HICaloCellHelper::GetAreaEtaPhi(theCell);
  float rho=0;
  if(area!=0.) rho=cell_et/area;
  slice->setArea(slice->area() + sgn*area*geoWeight);
  slice->setRho(slice->rho() + sgn*rho);

  for(unsigned int ih=0; ih<shape->at(iSlice)->Et_cos().size(); ih++)
  {
    float ih_f=ih;
    float tmp_cos = shape->at(iSlice)->Et_cos().at(ih);
    shape->at(iSlice)->Et_cos()[ih] = tmp_cos + cell_et*cos(ih_f*phi0)*geoWeight;
    
    float tmp_sin = shape->at(iSlice)->Et_sin().at(ih);
    shape->at(iSlice)->Et_sin()[ih] = tmp_sin + cell_et*sin(ih_f*phi0)*geoWeight;
  }
  
}

 
