/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonCnvPlots.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

namespace Egamma{


PhotonCnvPlots::PhotonCnvPlots(PlotBase* pParent, const std::string& sDir, const std::string& sParticleType):PlotBase(pParent, sDir), 
											       m_oKinAllPlots(this, "All/KinPlots/", "Reco " + sParticleType +" Photon"), 
											       m_oKinIsoPlots(this, "Iso/KinPlots/", "Reco " + sParticleType +" Photon"), 
											       m_sParticleType(sParticleType),
											       m_nParticles(nullptr),
											       m_nParticles_weighted(nullptr),
											       m_nVtx(nullptr),
											       m_convR(nullptr),
											       m_convRvsEta(nullptr),
                                                                                               m_convRvsType(nullptr),
											       m_convType(nullptr),
											       m_convDeltaEta(nullptr),
											       m_convDeltaPhi(nullptr)


{}	

void PhotonCnvPlots::initializePlots(){
  m_nParticles = Book1D("n", "Number of "+ m_sParticleType + "s;#" + m_sParticleType + ";Events", 15, 0., 15.);
  m_nParticles_weighted = Book1D("n_weighted", "Number of "+ m_sParticleType + "s;#" + m_sParticleType + ";Events", 15, 0., 15.);
  m_nVtx = Book1D("nVtx","Number of vertexes"+ m_sParticleType + ";nVtx;Events", 10, -0.5, 9.5);
  m_convR = Book1D("convR", "Radius Of conversion vertex" + m_sParticleType + ";convR;Conversions", 1200,0.,1200.);
  m_convRvsEta = Book2D("convRvsEta", "Radius Of conversion vertex vs #eta" + m_sParticleType + ";convR;#eta", 1200,0.,1200., 1000.,-5.,5.);
  m_convRvsType = Book2D("convRvsType", "Radius Of conversion vertex vs Type" + m_sParticleType + ";convR;Type", 1200,0.,1200., 7.,-.5,6.5);
  m_convType = Book1D("convType", "Conversion Type"+ m_sParticleType + ";convType;Conversions", 7, -0.5, 6.5);
  m_convDeltaEta = Book1D("convDeltaEta","Conversion #Delta #eta"+ m_sParticleType +";#eta; Conversions ", 100,-0.5,0.5);
  m_convDeltaPhi = Book1D("ConvDeltaPhi","Conversion #Delta #varphi"+ m_sParticleType +";#varphi; Conversions ",100 ,-0.5,0.5);
}



  void PhotonCnvPlots::fill(const xAOD::Photon& photon, const xAOD::EventInfo& eventInfo, bool isPrompt) const{
 
    float weight = 1.;
    weight = eventInfo.beamSpotWeight();

   if(!xAOD::EgammaHelpers::isConvertedPhoton(&photon)) return;
   m_oKinAllPlots.fill(photon,eventInfo);

   if(!isPrompt) return;

   int nvtx=photon.nVertices();
   m_nVtx->Fill(nvtx,weight);

   xAOD::EgammaParameters::ConversionType cvtype=xAOD::EgammaHelpers::conversionType(&photon); 
   m_convType->Fill(cvtype);
   float vtxRad = xAOD::EgammaHelpers::conversionRadius(&photon);  

   float cnvDeltaEta1;
   float cnvDeltaPhi1;
   photon.vertexCaloMatchValue(cnvDeltaEta1, xAOD::EgammaParameters::convMatchDeltaEta1);
   photon.vertexCaloMatchValue(cnvDeltaPhi1, xAOD::EgammaParameters::convMatchDeltaPhi1);
   m_convDeltaEta->Fill(cnvDeltaEta1,weight); 
   m_convDeltaPhi->Fill(cnvDeltaPhi1,weight);

   m_convR->Fill(vtxRad);
   m_convRvsEta->Fill(vtxRad,photon.eta(),weight);
   m_convRvsType->Fill(vtxRad,cvtype,weight);
   m_oKinIsoPlots.fill(photon,eventInfo);


 }
}
