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
											       nParticles(0),
											       nVtx(0),
											       convR(0),
											       convRvsEta(0),
											       convType(0),
											       convDeltaEta(0),
											       convDeltaPhi(0)


{}	

void PhotonCnvPlots::initializePlots(){
  nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + ";Events", 15, 0., 15.);
  nVtx = Book1D("nVtx","Number of vertexes"+ m_sParticleType + ";nVtx;Events", 10, -0.5, 9.5);
  convR = Book1D("convR", "Radius Of conversion vertex" + m_sParticleType + ";convR;Conversions", 1200,0.,1200.);
  convRvsEta = Book2D("convRvsEta", "Radius Of conversion vertex vs #eta" + m_sParticleType + ";convR;#eta", 1200,0.,1200., 1000.,-5.,5.);
  convRvsType = Book2D("convRvsType", "Radius Of conversion vertex vs Type" + m_sParticleType + ";convR;Type", 1200,0.,1200., 7.,-.5,6.5);
  convType = Book1D("convType", "Conversion Type"+ m_sParticleType + ";convType;Conversions", 7, -0.5, 6.5);
  convDeltaEta = Book1D("convDeltaEta","Conversion #Delta #eta"+ m_sParticleType +";#eta; Conversions ", 100,-0.5,0.5);
  convDeltaPhi = Book1D("ConvDeltaPhi","Conversion #Delta #varphi"+ m_sParticleType +";#varphi; Conversions ",100 ,-0.5,0.5);
}



 void PhotonCnvPlots::fill(const xAOD::Photon& photon, bool isPrompt){
 


   if(!xAOD::EgammaHelpers::isConvertedPhoton(&photon)) return;
   m_oKinAllPlots.fill(photon);

   if(!isPrompt) return;

   int nvtx=photon.nVertices();
   nVtx->Fill(nvtx);

   xAOD::EgammaParameters::ConversionType cvtype=xAOD::EgammaHelpers::conversionType(&photon); 
   convType->Fill(cvtype);
   float vtxRad = xAOD::EgammaHelpers::conversionRadius(&photon);  

   float cnvDeltaEta1, cnvDeltaPhi1;
   photon.vertexCaloMatchValue(cnvDeltaEta1, xAOD::EgammaParameters::convMatchDeltaEta1);
   photon.vertexCaloMatchValue(cnvDeltaPhi1, xAOD::EgammaParameters::convMatchDeltaPhi1);
   convDeltaEta->Fill(cnvDeltaEta1); 
   convDeltaPhi->Fill(cnvDeltaPhi1);

   convR->Fill(vtxRad);
   convRvsEta->Fill(vtxRad,photon.eta());
   convRvsType->Fill(vtxRad,cvtype);
   m_oKinIsoPlots.fill(photon);


 }
}
