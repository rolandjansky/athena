/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSLateralShapeTuning.h"

//temporary includes
#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TH1D.h>

#include <TGraph.h>

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

//=============================================
//======= TFCSLateralShapeTuning ==============
//=============================================

TFCSLateralShapeTuning::TFCSLateralShapeTuning(const char* name, const char* title, const std::string& pathToModelParameters, int intMinEta, int intMaxEta, int layer) : TFCSLateralShapeParametrizationHitBase(name, title), m_layer(layer)
{
  
  std::unique_ptr<TFile> modelParametersFile = std::unique_ptr<TFile>(TFile::Open(pathToModelParameters.c_str(), "READ"));
  modelParametersFile -> cd();
  
  std::vector<std::string> parameterModelNames;
  
  if (m_layer == 1 || m_layer == 5) parameterModelNames = {"a0", "a1", "a2", "a3"};
  else if (m_layer == 2 || m_layer == 6) parameterModelNames = {"eta_s", "phi_s"};

  for (auto &parameterName: parameterModelNames) {

    //interpolate the model parameters
    TFCSEnergyInterpolationPiecewiseLinear * linModelInterpol = new TFCSEnergyInterpolationPiecewiseLinear("", "");

    TGraph * modelParameterGraph = (TGraph*)gDirectory->Get(Form("photons/layer%d/m%d_m%d_%d_%d/%s", m_layer, 
                                                                                                     intMaxEta, intMinEta, 
                                                                                                     intMinEta, intMaxEta, 
                                                                                                     parameterName.c_str()));

    linModelInterpol -> InitFromArrayInEkin(modelParameterGraph->GetN(), modelParameterGraph->GetX(), modelParameterGraph->GetY());

    m_parameterInterpol.insert(std::make_pair(parameterName, linModelInterpol));
  
  }

  modelParametersFile -> Close();

}

TFCSLateralShapeTuning::~TFCSLateralShapeTuning()
{
  //clear parameter intpolation map and free memory
  m_parameterInterpol.clear();
}

FCSReturnCode TFCSLateralShapeTuning::simulate_hit(Hit &hit, TFCSSimulationState &, const TFCSTruthState* truth, const TFCSExtrapolationState*)
{
  
    //set maximum scaling factor
    float maxScaling = 500;

    //retrieve particle data
    const int     pdgId    = truth->pdgid();
    const double  charge   = HepPDT::ParticleID(pdgId).charge();

    //retreive hit information
    const double centerEta = hit.center_eta(); 
    const double centerPhi = hit.center_phi();
    const double centerZ   = hit.center_z();
    //retrieve truth kinetic energy for interpolation
    const float Ekin = truth->Ekin();


    ATH_MSG_DEBUG("[TFCSLateralShapeTuning] Initializing with pdgId=" << pdgId << ", charge=" << charge<< ", caloSample=" << m_layer);


    ATH_MSG_DEBUG("[TFCSLateralShapeTuning] Got hit position: " <<" hit.eta="    <<hit.eta()
                                                                <<", hit.phi="    <<hit.phi()
                                                                <<", hit.r="      <<hit.r()
                                                                );

    //compute deltaEta and deltaPhi
    const double deltaEta    = hit.eta() - centerEta;
    const double deltaPhi    = hit.phi() - centerPhi;

    if(m_layer == 2 || m_layer == 6){

        double etaScaleFactor = m_parameterInterpol["eta_s"]->evaluate(Ekin);
        double phiScaleFactor = m_parameterInterpol["phi_s"]->evaluate(Ekin);

        // add a maximum scaling threshold to prevent unreasonable scalings
        etaScaleFactor = std::abs(etaScaleFactor) < maxScaling ? etaScaleFactor : maxScaling;
        etaScaleFactor = std::abs(phiScaleFactor) < maxScaling ? phiScaleFactor : maxScaling;

        double deltaEtaCorr = etaScaleFactor*deltaEta;
        double deltaPhiCorr = phiScaleFactor*deltaPhi;

        hit.setEtaPhiZE(centerEta + deltaEtaCorr, centerPhi + deltaPhiCorr, centerZ, hit.E());

    } 

    else if(m_layer == 1 || m_layer == 5){

        double etaScaleFactor = getSeriesScalingFactor( m_parameterInterpol["a0"]->evaluate(Ekin), 
                                                        m_parameterInterpol["a1"]->evaluate(Ekin), 
                                                        m_parameterInterpol["a2"]->evaluate(Ekin), 
                                                        m_parameterInterpol["a3"]->evaluate(Ekin), 
                                                        std::abs(deltaEta)
                                                      );
        
        
        // add a maximum scaling threshold to prevent unreasonable scalings
        etaScaleFactor = std::abs(etaScaleFactor) < maxScaling ? etaScaleFactor : maxScaling;

        double deltaEtaCorr = etaScaleFactor*deltaEta;

        hit.setEtaPhiZE(centerEta + deltaEtaCorr, centerPhi + deltaPhi, centerZ, hit.E());

    }

    return FCSSuccess;

}


double TFCSLateralShapeTuning::getSeriesScalingFactor(double a0, double a1, double a2, double a3, double distToShowerCenter){

    double meanDistToShowerCentre = 0.0039;
    double scaleFactor  = 1 + a0 + a1*distToShowerCenter/meanDistToShowerCentre + a2*std::pow(distToShowerCenter/meanDistToShowerCentre, 2) + a3*std::pow(distToShowerCenter/meanDistToShowerCentre, 3);

    return scaleFactor;
    
}