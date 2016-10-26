/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonCnvPlots.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"

namespace Egamma{


PhotonCnvPlots::PhotonCnvPlots(PlotBase* pParent, std::string sDir, std::string sParticleType):PlotBase(pParent, sDir), 
											       m_oKinAllPlots(this, "All/KinPlots/", "Reco " + sParticleType +" Photon"), 
											       m_oKinIsoPlots(this, "Iso/KinPlots/", "Reco " + sParticleType +" Photon"), 
											       m_sParticleType(sParticleType),
											       m_nParticles(0),
											       m_nVtx(0),
											       m_convR(0),
											       m_convRvsEta(0),
                                                                                               m_convRvsType(nullptr),
											       m_convType(0),
											       m_convDeltaEta(0),
											       m_convDeltaPhi(0)


{}	

void PhotonCnvPlots::initializePlots(){
  m_nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + ";Events", 15, 0., 15.);
  m_nVtx = Book1D("nVtx","Number of vertexes"+ m_sParticleType + ";nVtx;Events", 10, -0.5, 9.5);
  m_convR = Book1D("convR", "Radius Of conversion vertex" + m_sParticleType + ";convR;Conversions", 1200,0.,1200.);
  m_convRvsEta = Book2D("convRvsEta", "Radius Of conversion vertex vs #eta" + m_sParticleType + ";convR;#eta", 1200,0.,1200., 1000.,-5.,5.);
  m_convRvsType = Book2D("convRvsType", "Radius Of conversion vertex vs Type" + m_sParticleType + ";convR;Type", 1200,0.,1200., 7.,-.5,6.5);
  m_convType = Book1D("convType", "Conversion Type"+ m_sParticleType + ";convType;Conversions", 7, -0.5, 6.5);
  m_convDeltaEta = Book1D("convDeltaEta","Conversion #Delta #eta"+ m_sParticleType +";#eta; Conversions ", 100,-0.5,0.5);
  m_convDeltaPhi = Book1D("ConvDeltaPhi","Conversion #Delta #varphi"+ m_sParticleType +";#varphi; Conversions ",100 ,-0.5,0.5);
}



 void PhotonCnvPlots::fill(const xAOD::Photon& photon, bool isPrompt){
 


   if(!xAOD::EgammaHelpers::isConvertedPhoton(&photon)) return;
   m_oKinAllPlots.fill(photon);

   if(!isPrompt) return;

   int nvtx=photon.nVertices();
   m_nVtx->Fill(nvtx);

   xAOD::EgammaParameters::ConversionType cvtype=xAOD::EgammaHelpers::conversionType(&photon); 
   m_convType->Fill(cvtype);
   float vtxRad = xAOD::EgammaHelpers::conversionRadius(&photon);  

   float cnvDeltaEta1, cnvDeltaPhi1;
   photon.vertexCaloMatchValue(cnvDeltaEta1, xAOD::EgammaParameters::convMatchDeltaEta1);
   photon.vertexCaloMatchValue(cnvDeltaPhi1, xAOD::EgammaParameters::convMatchDeltaPhi1);
   m_convDeltaEta->Fill(cnvDeltaEta1); 
   m_convDeltaPhi->Fill(cnvDeltaPhi1);

   m_convR->Fill(vtxRad);
   m_convRvsEta->Fill(vtxRad,photon.eta());
   m_convRvsType->Fill(vtxRad,cvtype);
   m_oKinIsoPlots.fill(photon);


 }
}
