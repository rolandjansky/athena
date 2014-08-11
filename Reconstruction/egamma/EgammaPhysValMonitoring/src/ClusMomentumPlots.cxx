/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusMomentumPlots.h"
using CLHEP::GeV;

namespace Egamma{


ClusMomentumPlots::ClusMomentumPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
m_sParticleType(sParticleType)
{}	

void ClusMomentumPlots::initializePlots(){

 
  secondlambda = Book1D("secondlambda", "Second Lambda; #lambda_{2};" + m_sParticleType, 100, 0., 10000.); 
  secondr      = Book1D("secondr", "Second R; #R_{2};" + m_sParticleType, 100, 0., 5000.); 
  centerlambda = Book1D("centerlambda", "Center Lambda; #lambda_{center};" + m_sParticleType, 100, 100., 500.); 
  engfracmax   = Book1D("engfracmax", "ENG_FRAC_MAX; energy fraction max ;" + m_sParticleType, 100, 0., 1.); 
  longitudinal = Book1D("longitudinal", "LONGITUDINAL; longitudinal ;" + m_sParticleType, 100, 0., 1.); 
  lateral      = Book1D("lateral", "LATERAL;lateral  ;" + m_sParticleType, 100, 0., 1.); 

}

void ClusMomentumPlots::fill(const xAOD::Egamma& egamma){

  const xAOD::CaloCluster* cluster=egamma.caloCluster();
  if(cluster){
    double eng_frac_max,lon,second_lambda,lat,second_r,center_lambda;
    cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX,eng_frac_max);
    cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL,lon);
    cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,second_lambda);
    cluster->retrieveMoment(xAOD::CaloCluster::LATERAL,lat);
    cluster->retrieveMoment(xAOD::CaloCluster::SECOND_R,second_r);
    cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda);

    secondlambda->Fill(second_lambda);
    secondr->Fill(second_r);
    centerlambda->Fill(center_lambda);
    engfracmax->Fill(eng_frac_max);
    longitudinal->Fill(lon);
    lateral->Fill(lat);
  }


}

}
