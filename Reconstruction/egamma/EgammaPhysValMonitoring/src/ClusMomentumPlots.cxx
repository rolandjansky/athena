/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "ClusMomentumPlots.h"

using CLHEP::GeV;

namespace Egamma{


  ClusMomentumPlots::ClusMomentumPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType):PlotBase(pParent, sDir), 
												       m_sParticleType(std::move(sParticleType)),
												       secondlambda(nullptr),
												       secondr(nullptr),     
												       centerlambda(nullptr),	
												       engfracmax(nullptr),
												       longitudinal(nullptr),
												       lateral(nullptr)     
  {}
  


  void  ClusMomentumPlots::initializePlots(){
    secondlambda = Book1D("secondlambda", "Second Lambda; #lambda_{2};" + m_sParticleType, 100, 0., 10000.); 
    secondr      = Book1D("secondr", "Second R; #R_{2};" + m_sParticleType, 100, 0., 5000.); 
    centerlambda = Book1D("centerlambda", "Center Lambda; #lambda_{center};" + m_sParticleType, 100, 100., 500.); 
    engfracmax   = Book1D("engfracmax", "ENG_FRAC_MAX; energy fraction max ;" + m_sParticleType, 100, 0., 1.); 
    longitudinal = Book1D("longitudinal", "LONGITUDINAL; longitudinal ;" + m_sParticleType, 100, 0., 1.); 
    lateral      = Book1D("lateral", "LATERAL;lateral  ;" + m_sParticleType, 100, 0., 1.); 

  }

  void ClusMomentumPlots::fill(const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo) const{

    float weight = 1.;
    weight = eventInfo.beamSpotWeight();

    const xAOD::CaloCluster* cluster=egamma.caloCluster();
    if(cluster){
      double eng_frac_max;
      double lon;
      double second_lambda;
      double lat;
      double second_r;
      double center_lambda;
      if(cluster->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX,eng_frac_max)){
	engfracmax->Fill(eng_frac_max,weight);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::LONGITUDINAL,lon)){
	longitudinal->Fill(lon,weight);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,second_lambda)){
	secondlambda->Fill(second_lambda,weight);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::LATERAL,lat)){
	lateral->Fill(lat,weight);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::SECOND_R,second_r)){
	secondr->Fill(second_r,weight);
      }
      if(cluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda)){
	centerlambda->Fill(center_lambda,weight);
      }
    }
  }

}
