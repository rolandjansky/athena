/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ClusMomentumPlots.h"
using CLHEP::GeV;

namespace Egamma{


  ClusMomentumPlots::ClusMomentumPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
												       m_sParticleType(sParticleType),
												       secondlambda(0),
												       secondr(0),     
												       centerlambda(0),	
												       engfracmax(0),
												       longitudinal(0),
												       lateral(0)     
  {}
  


  void  ClusMomentumPlots::initializePlots(){
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
      if(cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX,eng_frac_max)){
      engfracmax->Fill(eng_frac_max);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL,lon)){
	longitudinal->Fill(lon);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,second_lambda)){
	secondlambda->Fill(second_lambda);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::LATERAL,lat)){
	lateral->Fill(lat);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::SECOND_R,second_r)){
	secondr->Fill(second_r);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda)){
	centerlambda->Fill(center_lambda);
      }
    }
  }

}
