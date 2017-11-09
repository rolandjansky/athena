/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/NewCorePlots.h"

namespace Tau{

NewCorePlots::NewCorePlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_ipZ0SinThetaSigLeadTrk(nullptr),
   m_etOverPtLeadTrk(nullptr),
   m_ipSigLeadTrk(nullptr),
   m_massTrkSys(nullptr),
   m_trkWidth2(nullptr),
   m_trFlightPathSig(nullptr),
   m_etEflow(nullptr),
   m_nPi0(nullptr),
   m_tauDRMax(nullptr),
   m_tauSflight(nullptr),
   m_EMRadius(nullptr),
   m_hadRadius(nullptr),
   m_isolFrac(nullptr),
   m_centFrac(nullptr),
   m_stripWidth2(nullptr),
   m_nStrip(nullptr),
   m_trkAvgDist(nullptr),
   m_lead2ClusterEOverAllClusterE(nullptr),
   m_lead3ClusterEOverAllClusterE(nullptr),
   m_caloIso(nullptr),
   m_caloIsoCorrected(nullptr),
   m_mEflowTopo(nullptr),
   m_ptRatioEflowTopo(nullptr),
   m_nPi0Topo(nullptr),
   m_PSSFraction(nullptr),
   m_ChPiEMEOverCaloEME(nullptr),
   m_EMPOverTrkSysP(nullptr),
   m_innerTrkAvgDist(nullptr),
   m_ptRatioEflowApprox(nullptr),
   m_mEflowApprox(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{	
}
	
NewCorePlots::~NewCorePlots()
{
}

void NewCorePlots::initializePlots(){

  //  m_oParamPlots.initialize();

    m_ipZ0SinThetaSigLeadTrk           = Book1D("ipZ0SinThetaSigLeadTrk",m_sTauJetContainerName + "Tau ipZ0SinThetaSigLeadTrk ;ipZ0SinThetaSigLeadTrk ;# of Taus",10,  0,   40      );
    m_etOverPtLeadTrk                  = Book1D("etOverPtLeadTrk",m_sTauJetContainerName + "Tau etOverPtLeadTrk ;etOverPtLeadTrk ;# of Taus",   50,  0,   4.      );
    m_ipSigLeadTrk 		       = Book1D("ipSigLeadTrk",m_sTauJetContainerName + "Tau ipSigLeadTrk ;ipSigLeadTrk ;# of Taus", 30,  -15, 15   );
    m_massTrkSys 		       = Book1D("massTrkSys",m_sTauJetContainerName + "Tau massTrkSys ;massTrkSys ;# of Taus",    20,  0,   2500    );	      
    m_trkWidth2 		       = Book1D("trkWidth2",m_sTauJetContainerName + "Tau trkWidth2 ;trkWidth2 ;# of Taus", 20,  0,   1.0     );		      
    m_trFlightPathSig 	               = Book1D("trFlightPathSig",m_sTauJetContainerName + "Tau trFlightPathSig ;trFlightPathSig ;# of Taus",   100,  -10,   20      );
    //    m_nPi0                             = Book1D("NPio",m_sTauJetContainerName + "Tau NPio ;NPi0 ;# of Taus",  10,  0,   15      );
    m_EMRadius                         = Book1D("Seed_Calo_EMRadius",m_sTauJetContainerName + "Tau Seed Calo EMRadius ;EMRadius ;# of Taus",    10,  -0.5,1       );	  
    m_hadRadius 	               = Book1D("Seed_Calo_hadRadius",m_sTauJetContainerName + "Tau Seed Calo hadRadius ;hadRadius ;# of Taus",   10,  -0.5,2       ); 
    m_isolFrac 	                       = Book1D("Seed_Calo_isolFrac",m_sTauJetContainerName + "Tau Seed Calo isolFrac ;isolFrac ;# of Taus",    40,  -0.0,2       );	  
    m_centFrac 	                       = Book1D("Seed_Calo_centFrac",m_sTauJetContainerName + "Tau Seed Calo centFrac ;centFrac ;# of Taus",    40,  -0.0,2       );
    m_stripWidth2                      = Book1D("Seed_Calo_stripWidth2",m_sTauJetContainerName + "Tau Seed Calo stripWidth2 ;stripWidth2 ;# of Taus", 10,  -0.5,2.5     );
    m_nStrip                           = Book1D("Seed_Calo_nStrip",m_sTauJetContainerName + "Tau Seed Calo nStrip ;nStrip ;# of Taus",  20,  0,   100     );
    m_trkAvgDist                       = Book1D("Seed_Calo_trkavgdist",m_sTauJetContainerName + "Tau Seed Calo trkavgdist ;trkavgdist ;# of Taus",  10,  -0.2,0.8     );
    m_lead2ClusterEOverAllClusterE     = Book1D("Seed_Calo_lead2ClusterEOverAllClusterE",m_sTauJetContainerName + "Tau Seed Calo lead2ClusterEOverAllClusterE ;lead2ClusterEOverAllClusterE ;# of Taus",10,  0,   2       );	
    m_lead3ClusterEOverAllClusterE     = Book1D("Seed_Calo_lead3ClusterEOverAllClusterE",m_sTauJetContainerName + "Tau Seed Calo lead3ClusterEOverAllClusterE ;lead3ClusterEOverAllClusterE ;# of Taus",10,  0,   2       );	
    m_caloIso	        	       = Book1D("Seed_Calo_caloIso",m_sTauJetContainerName + "Tau Seed Calo caloIso ;caloIso ;# of Taus", 15,  0,   200     );
    m_caloIsoCorrected   	       = Book1D("Seed_Calo_caloIsoCorrected",m_sTauJetContainerName + "Tau Seed Calo caloIsoCorrected ;caloIsoCorrected ;# of Taus",    15,  0,   200     );                                 
    m_tauDRMax                         =       Book1D("DRMax",m_sTauJetContainerName + " Tau DR Max track-seed; DRMax; # Taus",10,0.,0.5);		  
    m_tauSflight                       =     Book1D("Sflight",m_sTauJetContainerName + " Tau flight sign. ; Sflight; # Taus",100,-10.,20.);
    // m_mEflowTopo                       = Book1D("mEflowTopo",m_sTauJetContainerName + "Tau mEflowTopo; mEflowTopo; #Taus",30,0,300);
    // m_ptRatioEflowTopo                 = Book1D("ptRatioEflowTopo",m_sTauJetContainerName + "Tau ptRatioEflowTopo; ptRatioEflowTopo; # of Taus", 10, 0, 10);
    // m_nPi0Topo                         = Book1D("nPi0Topo",m_sTauJetContainerName + "Tau nPi0Topo; nPi0Topo; # of Taus", 100, -1, 1);
    m_PSSFraction                      = Book1D("PSSFraction",m_sTauJetContainerName + "Tau PSSFraction; PSSFraction; # of Taus", 30, -15, 15);
    m_ChPiEMEOverCaloEME               = Book1D("ChPiEMEOverCaloEME",m_sTauJetContainerName + "Tau ChPiEMEOverCaloEME; ChPiEMEOverCaloEME; # of Taus", 30, -15, 15);
    m_EMPOverTrkSysP                   = Book1D("EMPOverTrkSysP",m_sTauJetContainerName + "Tau EMPOverTrkSysP; EMPOverTrkSysP; # of Taus", 81, -1, 80);

    m_innerTrkAvgDist                   = Book1D("innerTrkAvgDist",m_sTauJetContainerName + "Tau innerTrkAvgDist; innerTrkAvgDist; # of Taus",10,-0.2,.8);
    m_ptRatioEflowApprox                 = Book1D("ptRatioEflowApprox",m_sTauJetContainerName + "Tau ptRatioEflowApprox; ptRatioEflowApprox; # of Taus", 10, 0, 10);
    m_mEflowApprox						= Book1D("mEflowApprox",m_sTauJetContainerName + "Tau mEflowApprox; mEflowApprox; #Taus",30,0.0,3000.0);
}

void NewCorePlots::fill(const xAOD::TauJet& tau) {

  //  m_tauNCoreTracks->Fill(tau.nTracks(), 1.);
  //  m_tauNWideTracks->Fill(tau.nWideTracks(), 1.);
    float avariable=0;
    int bvariable=0;
    bool test = false;
  
  test=tau.detail(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk, avariable);
  if(test)     m_ipZ0SinThetaSigLeadTrk->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::etOverPtLeadTrk, avariable);
  if(test)     m_etOverPtLeadTrk->Fill(avariable,1.);

  //  test=tau.detail(xAOD::TauJetParameters::leadTrkPt, avariable);
  //  if(test)     m_leadTrkPt->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::ipSigLeadTrk, avariable);
  if(test)     m_ipSigLeadTrk->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::massTrkSys, avariable);
  if(test)     m_massTrkSys->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::trkWidth2, avariable);
  if(test)     m_trkWidth2->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::trFlightPathSig, avariable);
  if(test)     m_trFlightPathSig->Fill(avariable,1.);
  /*
  test=tau.detail(xAOD::TauJetParameters::etEflow, avariable);
  if(test)     m_etEflow->Fill(avariable,1.);
  */
  //  test=tau.detail(xAOD::TauJetParameters::mEflow, avariable);
  //  if(test)     m_mEflow->Fill(avariable,1.);
  
  // test=tau.detail(xAOD::TauJetParameters::nPi0, bvariable);
  // if(test)     m_nPi0->Fill(bvariable,1.);
  
  test=tau.detail(xAOD::TauJetParameters::EMRadius, avariable);
  if(test)     m_EMRadius->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::hadRadius, avariable);
  if(test)     m_hadRadius->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::isolFrac, avariable);
  if(test)     m_isolFrac->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if(test)     m_centFrac->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::stripWidth2, avariable);
  if(test)     m_stripWidth2->Fill(avariable,1.);
  
  test=tau.detail(xAOD::TauJetParameters::nStrip, bvariable);
  if(test)     m_nStrip->Fill(bvariable,1.);
  
  test=tau.detail(xAOD::TauJetParameters::trkAvgDist, avariable);
  if(test)     m_trkAvgDist->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::lead2ClusterEOverAllClusterE, avariable);
  if(test)     m_lead2ClusterEOverAllClusterE->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::lead3ClusterEOverAllClusterE, avariable);
  if(test)     m_lead3ClusterEOverAllClusterE->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::caloIso, avariable);
  if(test)     m_caloIso->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::caloIsoCorrected, avariable);
  if(test)     m_caloIsoCorrected->Fill(avariable,1.);

  // test=tau.detail(xAOD::TauJetParameters::mEflowTopo, avariable);
  // if(test)     m_mEflowTopo ->Fill(avariable/1000,1.);
  
  // test=tau.detail(xAOD::TauJetParameters::ptRatioEflowTopo, avariable);
  // if(test)     m_ptRatioEflowTopo->Fill(avariable,1.);

  // test=tau.detail(xAOD::TauJetParameters::nPi0Topo, bvariable);
  // if(test)     m_nPi0Topo->Fill(bvariable,1.);

  test=tau.detail(xAOD::TauJetParameters::PSSFraction, avariable);
  if(test)     m_PSSFraction->Fill(avariable,1.);
  
  test=tau.detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, avariable);
  if(test)     m_ChPiEMEOverCaloEME->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::EMPOverTrkSysP, avariable);
  if(test)     m_EMPOverTrkSysP->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::dRmax, avariable);
  if (test) m_tauDRMax->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::trFlightPathSig, avariable);
  if (test) m_tauSflight->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::innerTrkAvgDist, avariable);
  if (test) m_innerTrkAvgDist->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::ptRatioEflowApprox, avariable);
  if (test) m_ptRatioEflowApprox->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::mEflowApprox, avariable);
  if (test) m_mEflowApprox->Fill(avariable,1.);



}

}



/*EXTRA CODE*/
    /*  m_tauCharge      = Book1D("Charge",m_sTauJetContainerName + " Tau charge; charge; # Taus",12,-3.,3.);
  m_tauNCoreTracks = Book1D("NCoreTracks",m_sTauJetContainerName + " Tau n core tracks; nCore; # Taus",10,0.,10.);
  m_tauNWideTracks = Book1D("NWideTracks",m_sTauJetContainerName + " Tau n wide tracks; nWide; # Taus",10,0.,10.);
  m_tauCoreFrac =    Book1D("CoreFrac",m_sTauJetContainerName + " Tau CoreFrac; CoreFrac; # Taus",50,0.,1.5);
  m_tauEoverPTrk =   Book1D("EoverPTrk",m_sTauJetContainerName + " Tau E over pLeadTrk; EoverP; # Taus",50,0.,4.);
  m_tauTrkAvgDist =  Book1D("TrkAvgDist",m_sTauJetContainerName + " Tau track avg dist; TrkAvgDist; # Taus",50,0.,1.5);
  m_tauDRMax =       Book1D("DRMax",m_sTauJetContainerName + " Tau DR Max track-seed; DRMax; # Taus",10,0.,0.5);		  
  m_tauSflight =     Book1D("Sflight",m_sTauJetContainerName + " Tau flight sign. ; Sflight; # Taus",100,-10.,20.);
  m_tauMtrks =       Book1D("Mtracks",m_sTauJetContainerName + " Tau tracks mass ; Mtrks; # Taus",100,-10.,20.);
  m_tauIpSig =       Book1D("IpSig",m_sTauJetContainerName + " Tau lead track IP signif. ; IpSig; # Taus",100,-10.,10.);
    */

  /*
  = Book1D("PT",m_sTauJetContainerName + "Tau PT ;PT [GeV];# of Taus",    20,  0,   300     );
  = Book1D("Mass",m_sTauJetContainerName + "Tau Mass ;Mass ;# of Taus",  20,  -0.2,0.2     );
  = Book1D("NWideTacks",m_sTauJetContainerName + "Tau NWideTacks ;NWideTracks ;# of Taus",    10,  0,   15      );
  = Book1D("Seed Calo nIsolLooseTrk",m_sTauJetContainerName + "Tau Seed Calo nIsolLooseTrk ;nIsolLooseTrk ;# of Taus",   40,  -1500,   1500    );
  */

  // test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  // if(test)     m_tauCoreFrac->Fill(avariable,1.);
 
  // test=tau.detail(xAOD::TauJetParameters::etOverPtLeadTrk, avariable);
  // if(test) m_tauEoverPTrk->Fill(avariable, 1.);

  // test=tau.detail(xAOD::TauJetParameters::trkAvgDist, avariable);
  // if (test) m_tauTrkAvgDist->Fill(avariable, 1.);

  // test=tau.detail(xAOD::TauJetParameters::dRmax, avariable);
  // if (test) m_tauDRMax->Fill(avariable,1.);

  // test=tau.detail(xAOD::TauJetParameters::trFlightPathSig, avariable);
  // if (test) m_tauSflight->Fill(avariable,1.);

  // test=tau.detail(xAOD::TauJetParameters::massTrkSys, avariable);
  // if (test) m_tauMtrks->Fill(avariable/1000.,1.);

  // test=tau.detail(xAOD::TauJetParameters::ipSigLeadTrk, avariable);
  // if (test) m_tauIpSig->Fill(avariable,1.);

