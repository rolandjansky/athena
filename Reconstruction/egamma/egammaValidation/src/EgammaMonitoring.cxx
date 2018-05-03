/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#include "egammaValidation/EgammaMonitoring.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include <AsgTools/AnaToolHandle.h>
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "PATCore/AcceptData.h"


#include <vector>
#include <cmath>


EgammaMonitoring :: EgammaMonitoring (const std::string& name, ISvcLocator *pSvcLocator) : 
  AthAlgorithm (name, pSvcLocator),
  m_LooseLH ("AsgElectronLikelihoodTool/LooseLH"),
  m_MediumLH("AsgElectronLikelihoodTool/MediumLH"),
  m_TightLH ("AsgElectronLikelihoodTool/TightLH") 
{
  declareProperty( "sampleType", m_sampleType = "Unknown", "Descriptive name for the processed type of particle" );
}

// ******

StatusCode EgammaMonitoring :: initialize ()
{
  ANA_MSG_INFO ("******************************* Initializing *******************************");

  /// Get Histogram Service ///
  ATH_CHECK(service("THistSvc", rootHistSvc));
  
  ANA_MSG_INFO ("******************* Running over " << m_sampleType << "*******************");

  ANA_MSG_DEBUG ("*******************************  Histo INIT  *******************************");

  evtNmb = new TH1D("evtNmb", "Event Number", 1, 0, 1); 
  CHECK( rootHistSvc->regHist("/MONITORING/Basic/evtNmb", evtNmb));

  Eff_ID.initializePlots(); ssp.initializePlots(); ssp_10cut.initializePlots();

  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/hadleak", ssp.hadleak));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/reta"   , ssp.reta   ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/rphi"   , ssp.rphi   ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/weta2"  , ssp.weta2  ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/eratio" , ssp.eratio ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/deltae" , ssp.deltae ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/f1"     , ssp.f1     ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/fside"  , ssp.fside  ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/wtots1" , ssp.wtots1 ));
  CHECK(rootHistSvc->regHist("/MONITORING/All_ShowerShapes/ws3"    , ssp.ws3    ));

  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/hadleak", ssp_10cut.hadleak));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/reta"   , ssp_10cut.reta   ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/rphi"   , ssp_10cut.rphi   ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/weta2"  , ssp_10cut.weta2  ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/eratio" , ssp_10cut.eratio ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/deltae" , ssp_10cut.deltae ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/f1"     , ssp_10cut.f1     ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/fside"  , ssp_10cut.fside  ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/wtots1" , ssp_10cut.wtots1 ));
  CHECK(rootHistSvc->regHist("/MONITORING/10GeVcut_ShowerShapes/ws3"    , ssp_10cut.ws3    ));

  if("electron" == m_sampleType) {

    evtNmb->SetBins(2000, 85000, 87000);

    ssp_fe.initializePlots();
    ep_a.initializePlots(); ep_L.initializePlots(); ep_M.initializePlots(); ep_T.initializePlots();
    
    CHECK(rootHistSvc->regHist("/MONITORING/All/pT_prtcl" , ep_a.pT_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/All/eta_prtcl", ep_a.eta_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/All/phi_prtcl", ep_a.phi_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/All/TrkToEl"  , ep_a.TrkToEl  ));

    CHECK(rootHistSvc->regHist("/MONITORING/LLH/pT_prtcl" , ep_L.pT_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/LLH/eta_prtcl", ep_L.eta_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/LLH/phi_prtcl", ep_L.phi_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/LLH/TrkToEl"  , ep_L.TrkToEl  ));
    
    CHECK(rootHistSvc->regHist("/MONITORING/MLH/pT_prtcl" , ep_M.pT_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/MLH/eta_prtcl", ep_M.eta_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/MLH/phi_prtcl", ep_M.phi_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/MLH/TrkToEl"  , ep_M.TrkToEl  ));
    
    CHECK(rootHistSvc->regHist("/MONITORING/TLH/pT_prtcl" , ep_T.pT_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/TLH/eta_prtcl", ep_T.eta_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/TLH/phi_prtcl", ep_T.phi_prtcl));
    CHECK(rootHistSvc->regHist("/MONITORING/TLH/TrkToEl"  , ep_T.TrkToEl  ));
    
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/hadleak", ssp_fe.hadleak));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/reta"   , ssp_fe.reta   ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/rphi"   , ssp_fe.rphi   ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/weta2"  , ssp_fe.weta2  ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/eratio" , ssp_fe.eratio ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/deltae" , ssp_fe.deltae ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/f1"     , ssp_fe.f1     ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/fside"  , ssp_fe.fside  ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/wtots1" , ssp_fe.wtots1 ));
    CHECK(rootHistSvc->regHist("/MONITORING/FrwdEl_ShowerShapes/ws3"    , ssp_fe.ws3    ));

    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/LLH_pT_Eff", Eff_ID.LLH_pT_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/MLH_pT_Eff", Eff_ID.MLH_pT_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/TLH_pT_Eff", Eff_ID.TLH_pT_Eff));

    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/LLH_eta_Eff", Eff_ID.LLH_eta_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/MLH_eta_Eff", Eff_ID.MLH_eta_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/TLH_eta_Eff", Eff_ID.TLH_eta_Eff));

    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/LLH_phi_Eff", Eff_ID.LLH_phi_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/MLH_phi_Eff", Eff_ID.MLH_phi_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Eff_ID/TLH_phi_Eff", Eff_ID.TLH_phi_Eff));

  } // electron Hists

  if("gamma" == m_sampleType) {
    
    evtNmb->SetBins(250, 33894000, 33896000);

    pp_a.initializePlots(); 
    
    pp_truthConvPhoton.initializePlots(); pp_truthConvRecoConv.initializePlots(); pp_truthConvRecoConv1Si.initializePlots(); pp_truthConvRecoConv1TRT.initializePlots(); pp_truthConvRecoConv2Si.initializePlots(); pp_truthConvRecoConv2TRT.initializePlots(); pp_truthConvRecoConv2SiTRT.initializePlots(); pp_truthConvRecoUnconv.initializePlots(); 

    pp_truthUnconvPhoton.initializePlots(); pp_truthUnconvRecoConv.initializePlots(); pp_truthUnconvRecoConv1Si.initializePlots(); pp_truthUnconvRecoConv1TRT.initializePlots(); pp_truthUnconvRecoConv2Si.initializePlots(); pp_truthUnconvRecoConv2TRT.initializePlots(); pp_truthUnconvRecoConv2SiTRT.initializePlots(); pp_truthUnconvRecoUnconv.initializePlots();

    Eff_Reco.initializePlots();

    CHECK(rootHistSvc->regHist("/MONITORING/All/pT_prtcl"  , pp_a.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/All/eta_prtcl" , pp_a.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/All/phi_prtcl" , pp_a.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/All/convRadius", pp_a.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/All/mu"        , pp_a.mu));
    
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvPhoton/pT_prtcl"  , pp_truthConvPhoton.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvPhoton/eta_prtcl" , pp_truthConvPhoton.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvPhoton/phi_prtcl" , pp_truthConvPhoton.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvPhoton/convRadius", pp_truthConvPhoton.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvPhoton/mu"        , pp_truthConvPhoton.mu));    

    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv/pT_prtcl"  , pp_truthConvRecoConv.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv/eta_prtcl" , pp_truthConvRecoConv.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv/phi_prtcl" , pp_truthConvRecoConv.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv/convRadius", pp_truthConvRecoConv.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv/mu"        , pp_truthConvRecoConv.mu));
    
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1Si/pT_prtcl"  , pp_truthConvRecoConv1Si.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1Si/eta_prtcl" , pp_truthConvRecoConv1Si.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1Si/phi_prtcl" , pp_truthConvRecoConv1Si.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1Si/convRadius", pp_truthConvRecoConv1Si.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1Si/mu"        , pp_truthConvRecoConv1Si.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1TRT/pT_prtcl"  , pp_truthConvRecoConv1TRT.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1TRT/eta_prtcl" , pp_truthConvRecoConv1TRT.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1TRT/phi_prtcl" , pp_truthConvRecoConv1TRT.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1TRT/convRadius", pp_truthConvRecoConv1TRT.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv1TRT/mu"        , pp_truthConvRecoConv1TRT.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2Si/pT_prtcl"  , pp_truthConvRecoConv2Si.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2Si/eta_prtcl" , pp_truthConvRecoConv2Si.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2Si/phi_prtcl" , pp_truthConvRecoConv2Si.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2Si/convRadius", pp_truthConvRecoConv2Si.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2Si/mu"        , pp_truthConvRecoConv2Si.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2TRT/pT_prtcl"  , pp_truthConvRecoConv2TRT.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2TRT/eta_prtcl" , pp_truthConvRecoConv2TRT.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2TRT/phi_prtcl" , pp_truthConvRecoConv2TRT.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2TRT/convRadius", pp_truthConvRecoConv2TRT.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2TRT/mu"        , pp_truthConvRecoConv2TRT.mu));
    
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2SiTRT/pT_prtcl"  , pp_truthConvRecoConv2SiTRT.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2SiTRT/eta_prtcl" , pp_truthConvRecoConv2SiTRT.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2SiTRT/phi_prtcl" , pp_truthConvRecoConv2SiTRT.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2SiTRT/convRadius", pp_truthConvRecoConv2SiTRT.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoConv2SiTRT/mu"        , pp_truthConvRecoConv2SiTRT.mu));
        
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoUnconv/pT_prtcl"  , pp_truthConvRecoUnconv.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoUnconv/eta_prtcl" , pp_truthConvRecoUnconv.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoUnconv/phi_prtcl" , pp_truthConvRecoUnconv.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoUnconv/convRadius", pp_truthConvRecoUnconv.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthConvRecoUnconv/mu"        , pp_truthConvRecoUnconv.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvPhoton/pT_prtcl"  , pp_truthUnconvPhoton.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvPhoton/eta_prtcl" , pp_truthUnconvPhoton.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvPhoton/phi_prtcl" , pp_truthUnconvPhoton.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvPhoton/convRadius", pp_truthUnconvPhoton.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvPhoton/mu"        , pp_truthUnconvPhoton.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv/pT_prtcl"  , pp_truthUnconvRecoConv.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv/eta_prtcl" , pp_truthUnconvRecoConv.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv/phi_prtcl" , pp_truthUnconvRecoConv.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv/convRadius", pp_truthUnconvRecoConv.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv/mu"        , pp_truthUnconvRecoConv.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1Si/pT_prtcl"  , pp_truthUnconvRecoConv1Si.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1Si/eta_prtcl" , pp_truthUnconvRecoConv1Si.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1Si/phi_prtcl" , pp_truthUnconvRecoConv1Si.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1Si/convRadius", pp_truthUnconvRecoConv1Si.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1Si/mu"        , pp_truthUnconvRecoConv1Si.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1TRT/pT_prtcl"  , pp_truthUnconvRecoConv1TRT.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1TRT/eta_prtcl" , pp_truthUnconvRecoConv1TRT.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1TRT/phi_prtcl" , pp_truthUnconvRecoConv1TRT.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1TRT/convRadius", pp_truthUnconvRecoConv1TRT.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv1TRT/mu"        , pp_truthUnconvRecoConv1TRT.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2Si/pT_prtcl"  , pp_truthUnconvRecoConv2Si.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2Si/eta_prtcl" , pp_truthUnconvRecoConv2Si.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2Si/phi_prtcl" , pp_truthUnconvRecoConv2Si.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2Si/convRadius", pp_truthUnconvRecoConv2Si.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2Si/mu"        , pp_truthUnconvRecoConv2Si.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2TRT/pT_prtcl"  , pp_truthUnconvRecoConv2TRT.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2TRT/eta_prtcl" , pp_truthUnconvRecoConv2TRT.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2TRT/phi_prtcl" , pp_truthUnconvRecoConv2TRT.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2TRT/convRadius", pp_truthUnconvRecoConv2TRT.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2TRT/mu"        , pp_truthUnconvRecoConv2TRT.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2SiTRT/pT_prtcl"  , pp_truthUnconvRecoConv2SiTRT.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2SiTRT/eta_prtcl" , pp_truthUnconvRecoConv2SiTRT.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2SiTRT/phi_prtcl" , pp_truthUnconvRecoConv2SiTRT.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2SiTRT/convRadius", pp_truthUnconvRecoConv2SiTRT.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoConv2SiTRT/mu"        , pp_truthUnconvRecoConv2SiTRT.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoUnconv/pT_prtcl"  , pp_truthUnconvRecoUnconv.pT_prtcl  ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoUnconv/eta_prtcl" , pp_truthUnconvRecoUnconv.eta_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoUnconv/phi_prtcl" , pp_truthUnconvRecoUnconv.phi_prtcl ));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoUnconv/convRadius", pp_truthUnconvRecoUnconv.convRadius));
    CHECK(rootHistSvc->regHist("/MONITORING/truthUnconvRecoUnconv/mu"        , pp_truthUnconvRecoUnconv.mu));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRConv_pT_Eff"   , Eff_Reco.tConvRConv_pT_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1Si_pT_Eff"   , Eff_Reco.tConvRC1Si_pT_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1TRT_pT_Eff"  , Eff_Reco.tConvRC1TRT_pT_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2Si_pT_Eff"   , Eff_Reco.tConvRC2Si_pT_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2TRT_pT_Eff"  , Eff_Reco.tConvRC2TRT_pT_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2SiTRT_pT_Eff", Eff_Reco.tConvRC2SiTRT_pT_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRUnconv_pT_Eff" , Eff_Reco.tConvRUnconv_pT_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRConv_eta_Eff"   , Eff_Reco.tConvRConv_eta_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1Si_eta_Eff"   , Eff_Reco.tConvRC1Si_eta_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1TRT_eta_Eff"  , Eff_Reco.tConvRC1TRT_eta_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2Si_eta_Eff"   , Eff_Reco.tConvRC2Si_eta_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2TRT_eta_Eff"  , Eff_Reco.tConvRC2TRT_eta_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2SiTRT_eta_Eff", Eff_Reco.tConvRC2SiTRT_eta_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRUnconv_eta_Eff" , Eff_Reco.tConvRUnconv_eta_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRConv_phi_Eff"   , Eff_Reco.tConvRConv_phi_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1Si_phi_Eff"   , Eff_Reco.tConvRC1Si_phi_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1TRT_phi_Eff"  , Eff_Reco.tConvRC1TRT_phi_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2Si_phi_Eff"   , Eff_Reco.tConvRC2Si_phi_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2TRT_phi_Eff"  , Eff_Reco.tConvRC2TRT_phi_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2SiTRT_phi_Eff", Eff_Reco.tConvRC2SiTRT_phi_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRUnconv_phi_Eff" , Eff_Reco.tConvRUnconv_phi_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRConv_CR_Eff"   , Eff_Reco.tConvRConv_CR_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1Si_CR_Eff"   , Eff_Reco.tConvRC1Si_CR_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1TRT_CR_Eff"  , Eff_Reco.tConvRC1TRT_CR_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2Si_CR_Eff"   , Eff_Reco.tConvRC2Si_CR_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2TRT_CR_Eff"  , Eff_Reco.tConvRC2TRT_CR_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2SiTRT_CR_Eff", Eff_Reco.tConvRC2SiTRT_CR_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRUnconv_CR_Eff" , Eff_Reco.tConvRUnconv_CR_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRConv_mu_Eff"   , Eff_Reco.tConvRConv_mu_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1Si_mu_Eff"   , Eff_Reco.tConvRC1Si_mu_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC1TRT_mu_Eff"  , Eff_Reco.tConvRC1TRT_mu_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2Si_mu_Eff"   , Eff_Reco.tConvRC2Si_mu_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2TRT_mu_Eff"  , Eff_Reco.tConvRC2TRT_mu_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRC2SiTRT_mu_Eff", Eff_Reco.tConvRC2SiTRT_mu_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tConvRUnconv_mu_Eff" , Eff_Reco.tConvRUnconv_mu_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRConv_pT_Eff"   , Eff_Reco.tUnconvRConv_pT_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1Si_pT_Eff"   , Eff_Reco.tUnconvRC1Si_pT_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1TRT_pT_Eff"  , Eff_Reco.tUnconvRC1TRT_pT_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2Si_pT_Eff"   , Eff_Reco.tUnconvRC2Si_pT_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2TRT_pT_Eff"  , Eff_Reco.tUnconvRC2TRT_pT_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2SiTRT_pT_Eff", Eff_Reco.tUnconvRC2SiTRT_pT_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRUnconv_pT_Eff" , Eff_Reco.tUnconvRUnconv_pT_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRConv_eta_Eff"   , Eff_Reco.tUnconvRConv_eta_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1Si_eta_Eff"   , Eff_Reco.tUnconvRC1Si_eta_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1TRT_eta_Eff"  , Eff_Reco.tUnconvRC1TRT_eta_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2Si_eta_Eff"   , Eff_Reco.tUnconvRC2Si_eta_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2TRT_eta_Eff"  , Eff_Reco.tUnconvRC2TRT_eta_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2SiTRT_eta_Eff", Eff_Reco.tUnconvRC2SiTRT_eta_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRUnconv_eta_Eff" , Eff_Reco.tUnconvRUnconv_eta_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRConv_phi_Eff"   , Eff_Reco.tUnconvRConv_phi_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1Si_phi_Eff"   , Eff_Reco.tUnconvRC1Si_phi_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1TRT_phi_Eff"  , Eff_Reco.tUnconvRC1TRT_phi_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2Si_phi_Eff"   , Eff_Reco.tUnconvRC2Si_phi_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2TRT_phi_Eff"  , Eff_Reco.tUnconvRC2TRT_phi_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2SiTRT_phi_Eff", Eff_Reco.tUnconvRC2SiTRT_phi_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRUnconv_phi_Eff" , Eff_Reco.tUnconvRUnconv_phi_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRConv_CR_Eff"   , Eff_Reco.tUnconvRConv_CR_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1Si_CR_Eff"   , Eff_Reco.tUnconvRC1Si_CR_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1TRT_CR_Eff"  , Eff_Reco.tUnconvRC1TRT_CR_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2Si_CR_Eff"   , Eff_Reco.tUnconvRC2Si_CR_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2TRT_CR_Eff"  , Eff_Reco.tUnconvRC2TRT_CR_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2SiTRT_CR_Eff", Eff_Reco.tUnconvRC2SiTRT_CR_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRUnconv_CR_Eff" , Eff_Reco.tUnconvRUnconv_CR_Eff ));

    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRConv_mu_Eff"   , Eff_Reco.tUnconvRConv_mu_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1Si_mu_Eff"   , Eff_Reco.tUnconvRC1Si_mu_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC1TRT_mu_Eff"  , Eff_Reco.tUnconvRC1TRT_mu_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2Si_mu_Eff"   , Eff_Reco.tUnconvRC2Si_mu_Eff   ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2TRT_mu_Eff"  , Eff_Reco.tUnconvRC2TRT_mu_Eff  ));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRC2SiTRT_mu_Eff", Eff_Reco.tUnconvRC2SiTRT_mu_Eff));
    CHECK(rootHistSvc->regHist("/MONITORING/Efficiency_Reco/tUnconvRUnconv_mu_Eff" , Eff_Reco.tUnconvRUnconv_mu_Eff ));

  } // gamma Hists

  //*****************LLH Requirement********************
  ANA_CHECK(m_LooseLH.setProperty("WorkingPoint", "LooseLHElectron"));
  ANA_CHECK(m_LooseLH.initialize());
  ANA_MSG_DEBUG ("*******************************  OK LLH Req  *******************************");
  //****************************************************
    
  
  //*****************MLH Requirement********************
  ANA_CHECK(m_MediumLH.setProperty("WorkingPoint", "MediumLHElectron"));
  ANA_CHECK(m_MediumLH.initialize());
  ANA_MSG_DEBUG ("*******************************  OK MLH Req  *******************************");
  //****************************************************

  
  //*****************TLH Requirement********************
  ANA_CHECK(m_TightLH.setProperty("WorkingPoint", "TightLHElectron"));
  ANA_CHECK(m_TightLH.initialize());
  ANA_MSG_DEBUG ("*******************************  OK TLH Req  *******************************");
  //****************************************************

  
  ANA_MSG_DEBUG ("*******************************   END INIT   *******************************");  
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: beginInputFile ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: firstExecute ()
{
  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: execute ()
{
  
  // Retrieve things from the event store
  const xAOD::EventInfo *eventInfo = nullptr;
  ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

  float mu = eventInfo->averageInteractionsPerCrossing();

  // Retrieve egamma truth particles
  const xAOD::TruthParticleContainer* egTruthParticles = nullptr;
  if ( !evtStore()->retrieve( egTruthParticles, "egammaTruthParticles").isSuccess() ) {
    ATH_MSG_ERROR("Failed to retrieve egamma truth particle container. Exiting.");
    return StatusCode::FAILURE;
  }

  bool isTrueConv = false, isTrueLateConv = false, isRecoConv = false;
  int convType;
  const xAOD::Photon* photon;
  
  if("electron" == m_sampleType) {

    // Retrieve electrons    
    const xAOD::ElectronContainer* RecoEl = nullptr;
    if( !evtStore()->retrieve(RecoEl, "Electrons").isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve electron container. Exiting.");
      return StatusCode::FAILURE;
    }

    // Retrieve forward electrons
    /*
    const xAOD::ElectronContainer* ElectronsFrwd = evtStore()->retrieve<const xAOD::ElectronContainer>("FwdElectrons");
    if(!ElectronsFrwd ) {
      ATH_MSG_ERROR("Failed to retrieve forward electron container. Exiting.");
      return StatusCode::FAILURE;
    }
    
    const xAOD::ElectronContainer* ElectronsFrwd = nullptr;
    if( !evtStore()->retrieve(ElectronsFrwd, "ForwardElectrons").isSuccess()) {
      ATH_MSG_ERROR("Failed to retrieve forward electron container. Exiting.");
      return StatusCode::FAILURE;
    }
    */

    for(auto elrec : *RecoEl) {

      if(!elrec) continue;
      
      if(m_LooseLH ->accept(elrec)) ep_L.fill(*elrec);
      if(m_MediumLH->accept(elrec)) ep_M.fill(*elrec);
      if(m_TightLH ->accept(elrec)) ep_T.fill(*elrec);
      
      ep_a.fill(*elrec);
      
      ssp.fill(*elrec);      
      if((elrec->pt())/1000. > 10.) ssp_10cut.fill(*elrec);
      if(xAOD::EgammaHelpers::isFwdElectron(elrec)) ssp_fe.fill(*elrec);
      
    } // RecoEl Loop

    /*
    for(auto frwdelectron : *ElectronsFrwd){
      if(frwdelectron && frwdelectron->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) ssp_fe.fill(*frwdelectron);
    } // ElectronsFrwd Loop
    */

  } // if electron

  
  if("gamma" == m_sampleType) {
    
    const xAOD::PhotonContainer* RecoPh = nullptr;
    if( !evtStore()->retrieve(RecoPh, "Photons").isSuccess() ) {
      ATH_MSG_ERROR("Failed to retrieve photon container. Exiting.");
      return StatusCode::FAILURE;
    }  

    for(auto phrec : *RecoPh) {

      if(!phrec) continue;
      
      pp_a.fill(*phrec, mu);
      
      ssp.fill(*phrec);
      if(phrec->pt()) ssp_10cut.fill(*phrec);
      
    } // RecoPh Loop

    for (auto egtruth : *egTruthParticles) {

      if(!egtruth) continue;
      
      isTrueConv     = xAOD::EgammaHelpers::isTrueConvertedPhoton(egtruth);
      isTrueLateConv = xAOD::EgammaHelpers::isTrueConvertedPhoton( egtruth, 1200 ) and !isTrueConv;
      photon         = xAOD::EgammaHelpers::getRecoPhoton(egtruth);

      if(!photon) continue;
	
      isRecoConv     = xAOD::EgammaHelpers::isConvertedPhoton(photon);
      convType       = xAOD::EgammaHelpers::conversionType(photon);
      
      if(isTrueConv) {
	
	pp_truthConvPhoton.fill(*egtruth, mu);
	
	if(isRecoConv) {
	  
	  pp_truthConvRecoConv.fill(*egtruth, mu);
	  
	  if(convType == xAOD::EgammaParameters::singleSi   ) pp_truthConvRecoConv1Si   .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::singleTRT  ) pp_truthConvRecoConv1TRT  .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::doubleSi   ) pp_truthConvRecoConv2Si   .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::doubleTRT  ) pp_truthConvRecoConv2TRT  .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::doubleSiTRT) pp_truthConvRecoConv2SiTRT.fill(*egtruth, mu);
	  
	} // isRecoConv
	  else {
	    pp_truthConvRecoUnconv.fill(*egtruth, mu);
	  }
      } //isTrueConv
      else if(!isTrueLateConv) {
	
	pp_truthUnconvPhoton.fill(*egtruth, mu);
	
	if(isRecoConv) {

	  pp_truthUnconvRecoConv.fill(*egtruth, mu);
	  
	  if(convType == xAOD::EgammaParameters::singleSi   ) pp_truthUnconvRecoConv1Si   .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::singleTRT  ) pp_truthUnconvRecoConv1TRT  .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::doubleSi   ) pp_truthUnconvRecoConv2Si   .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::doubleTRT  ) pp_truthUnconvRecoConv2TRT  .fill(*egtruth, mu);
	  if(convType == xAOD::EgammaParameters::doubleSiTRT) pp_truthUnconvRecoConv2SiTRT.fill(*egtruth, mu);
	  
	} // isRecoConv
	else {
	  pp_truthUnconvRecoUnconv.fill(*egtruth, mu);
	}
      } // !isTrueLateConv
    } //egtruth Loop
  } // if gamma

  evtNmb->Fill(eventInfo->eventNumber());

  return StatusCode::SUCCESS;
}

// ******

StatusCode EgammaMonitoring :: finalize ()
{
  ANA_MSG_INFO ("******************************** Finalizing ********************************");

  if("electron" == m_sampleType) {
    
    Eff_ID.divide("pT_LLH", ep_L.pT_prtcl, ep_a.pT_prtcl);
    Eff_ID.divide("pT_MLH", ep_M.pT_prtcl, ep_a.pT_prtcl);
    Eff_ID.divide("pT_TLH", ep_T.pT_prtcl, ep_a.pT_prtcl);
    
    Eff_ID.divide("eta_LLH", ep_L.eta_prtcl, ep_a.eta_prtcl);
    Eff_ID.divide("eta_MLH", ep_M.eta_prtcl, ep_a.eta_prtcl);
    Eff_ID.divide("eta_TLH", ep_T.eta_prtcl, ep_a.eta_prtcl);
    
    Eff_ID.divide("phi_LLH", ep_L.phi_prtcl, ep_a.phi_prtcl);
    Eff_ID.divide("phi_MLH", ep_M.phi_prtcl, ep_a.phi_prtcl);
    Eff_ID.divide("phi_TLH", ep_T.phi_prtcl, ep_a.phi_prtcl);

  } // if electron sampleType

  if("gamma" == m_sampleType) {

    Eff_Reco.divide("tConvRConv_pT"   , pp_truthConvRecoConv.pT_prtcl      , pp_truthConvPhoton.pT_prtcl);
    Eff_Reco.divide("tConvRC1Si_pT"   , pp_truthConvRecoConv1Si.pT_prtcl   , pp_truthConvPhoton.pT_prtcl);
    Eff_Reco.divide("tConvRC1TRT_pT"  , pp_truthConvRecoConv1TRT.pT_prtcl  , pp_truthConvPhoton.pT_prtcl); 
    Eff_Reco.divide("tConvRC2Si_pT"   , pp_truthConvRecoConv2Si.pT_prtcl   , pp_truthConvPhoton.pT_prtcl); 
    Eff_Reco.divide("tConvRC2TRT_pT"  , pp_truthConvRecoConv2TRT.pT_prtcl  , pp_truthConvPhoton.pT_prtcl);
    Eff_Reco.divide("tConvRC2SiTRT_pT", pp_truthConvRecoConv2SiTRT.pT_prtcl, pp_truthConvPhoton.pT_prtcl);
    Eff_Reco.divide("tConvRUnconv_pT" , pp_truthConvRecoUnconv.pT_prtcl    , pp_truthConvPhoton.pT_prtcl);
    
    Eff_Reco.divide("tConvRConv_eta"   , pp_truthConvRecoConv.eta_prtcl      , pp_truthConvPhoton.eta_prtcl);
    Eff_Reco.divide("tConvRC1Si_eta"   , pp_truthConvRecoConv1Si.eta_prtcl   , pp_truthConvPhoton.eta_prtcl);
    Eff_Reco.divide("tConvRC1TRT_eta"  , pp_truthConvRecoConv1TRT.eta_prtcl  , pp_truthConvPhoton.eta_prtcl);
    Eff_Reco.divide("tConvRC2Si_eta"   , pp_truthConvRecoConv2Si.eta_prtcl   , pp_truthConvPhoton.eta_prtcl);
    Eff_Reco.divide("tConvRC2TRT_eta"  , pp_truthConvRecoConv2TRT.eta_prtcl  , pp_truthConvPhoton.eta_prtcl);
    Eff_Reco.divide("tConvRC2SiTRT_eta", pp_truthConvRecoConv2SiTRT.eta_prtcl, pp_truthConvPhoton.eta_prtcl);
    Eff_Reco.divide("tConvRUnconv_eta" , pp_truthConvRecoUnconv.eta_prtcl    , pp_truthConvPhoton.eta_prtcl);
    
    Eff_Reco.divide("tConvRConv_phi"   , pp_truthConvRecoConv.phi_prtcl      , pp_truthConvPhoton.phi_prtcl);
    Eff_Reco.divide("tConvRC1Si_phi"   , pp_truthConvRecoConv1Si.phi_prtcl   , pp_truthConvPhoton.phi_prtcl);
    Eff_Reco.divide("tConvRC1TRT_phi"  , pp_truthConvRecoConv1TRT.phi_prtcl  , pp_truthConvPhoton.phi_prtcl);
    Eff_Reco.divide("tConvRC2Si_phi"   , pp_truthConvRecoConv2Si.phi_prtcl   , pp_truthConvPhoton.phi_prtcl);
    Eff_Reco.divide("tConvRC2TRT_phi"  , pp_truthConvRecoConv2TRT.phi_prtcl  , pp_truthConvPhoton.phi_prtcl);
    Eff_Reco.divide("tConvRC2SiTRT_phi", pp_truthConvRecoConv2SiTRT.phi_prtcl, pp_truthConvPhoton.phi_prtcl);
    Eff_Reco.divide("tConvRUnconv_phi" , pp_truthConvRecoUnconv.phi_prtcl    , pp_truthConvPhoton.phi_prtcl);
    
    Eff_Reco.divide("tConvRConv_mu"   , pp_truthConvRecoConv.mu      , pp_truthConvPhoton.mu);
    Eff_Reco.divide("tConvRC1Si_mu"   , pp_truthConvRecoConv1Si.mu   , pp_truthConvPhoton.mu);
    Eff_Reco.divide("tConvRC1TRT_mu"  , pp_truthConvRecoConv1TRT.mu  , pp_truthConvPhoton.mu);
    Eff_Reco.divide("tConvRC2Si_mu"   , pp_truthConvRecoConv2Si.mu   , pp_truthConvPhoton.mu);
    Eff_Reco.divide("tConvRC2TRT_mu"  , pp_truthConvRecoConv2TRT.mu  , pp_truthConvPhoton.mu);
    Eff_Reco.divide("tConvRC2SiTRT_mu", pp_truthConvRecoConv2SiTRT.mu, pp_truthConvPhoton.mu);
    Eff_Reco.divide("tConvRUnconv_mu" , pp_truthConvRecoUnconv.mu    , pp_truthConvPhoton.mu);
    
    Eff_Reco.divide("tConvRConv_CR"   , pp_truthConvRecoConv.convRadius      , pp_truthConvPhoton.convRadius);
    Eff_Reco.divide("tConvRC1Si_CR"   , pp_truthConvRecoConv1Si.convRadius   , pp_truthConvPhoton.convRadius);
    Eff_Reco.divide("tConvRC1TRT_CR"  , pp_truthConvRecoConv1TRT.convRadius  , pp_truthConvPhoton.convRadius);
    Eff_Reco.divide("tConvRC2Si_CR"   , pp_truthConvRecoConv2Si.convRadius   , pp_truthConvPhoton.convRadius);
    Eff_Reco.divide("tConvRC2TRT_CR"  , pp_truthConvRecoConv2TRT.convRadius  , pp_truthConvPhoton.convRadius);
    Eff_Reco.divide("tConvRC2SiTRT_CR", pp_truthConvRecoConv2SiTRT.convRadius, pp_truthConvPhoton.convRadius);
    Eff_Reco.divide("tConvRUnconv_CR" , pp_truthConvRecoUnconv.convRadius    , pp_truthConvPhoton.convRadius);
    
    Eff_Reco.divide("tUnconvRConv_pT"   , pp_truthUnconvRecoConv.pT_prtcl      , pp_truthUnconvPhoton.pT_prtcl);
    Eff_Reco.divide("tUnconvRC1Si_pT"   , pp_truthUnconvRecoConv1Si.pT_prtcl   , pp_truthUnconvPhoton.pT_prtcl);
    Eff_Reco.divide("tUnconvRC1TRT_pT"  , pp_truthUnconvRecoConv1TRT.pT_prtcl  , pp_truthUnconvPhoton.pT_prtcl);
    Eff_Reco.divide("tUnconvRC2Si_pT"   , pp_truthUnconvRecoConv2Si.pT_prtcl   , pp_truthUnconvPhoton.pT_prtcl);
    Eff_Reco.divide("tUnconvRC2TRT_pT"  , pp_truthUnconvRecoConv2TRT.pT_prtcl  , pp_truthUnconvPhoton.pT_prtcl);
    Eff_Reco.divide("tUnconvRC2SiTRT_pT", pp_truthUnconvRecoConv2SiTRT.pT_prtcl, pp_truthUnconvPhoton.pT_prtcl);
    Eff_Reco.divide("tUnconvRUnconv_pT" , pp_truthUnconvRecoUnconv.pT_prtcl    , pp_truthUnconvPhoton.pT_prtcl);
    
    Eff_Reco.divide("tUnconvRConv_eta"   , pp_truthUnconvRecoConv.eta_prtcl      , pp_truthUnconvPhoton.eta_prtcl);
    Eff_Reco.divide("tUnconvRC1Si_eta"   , pp_truthUnconvRecoConv1Si.eta_prtcl   , pp_truthUnconvPhoton.eta_prtcl);
    Eff_Reco.divide("tUnconvRC1TRT_eta"  , pp_truthUnconvRecoConv1TRT.eta_prtcl  , pp_truthUnconvPhoton.eta_prtcl);
    Eff_Reco.divide("tUnconvRC2Si_eta"   , pp_truthUnconvRecoConv2Si.eta_prtcl   , pp_truthUnconvPhoton.eta_prtcl);
    Eff_Reco.divide("tUnconvRC2TRT_eta"  , pp_truthUnconvRecoConv2TRT.eta_prtcl  , pp_truthUnconvPhoton.eta_prtcl);
    Eff_Reco.divide("tUnconvRC2SiTRT_eta", pp_truthUnconvRecoConv2SiTRT.eta_prtcl, pp_truthUnconvPhoton.eta_prtcl);
    Eff_Reco.divide("tUnconvRUnconv_eta" , pp_truthUnconvRecoUnconv.eta_prtcl    , pp_truthUnconvPhoton.eta_prtcl);
    
    Eff_Reco.divide("tUnconvRConv_phi"   , pp_truthUnconvRecoConv.phi_prtcl      , pp_truthUnconvPhoton.phi_prtcl);
    Eff_Reco.divide("tUnconvRC1Si_phi"   , pp_truthUnconvRecoConv1Si.phi_prtcl   , pp_truthUnconvPhoton.phi_prtcl);
    Eff_Reco.divide("tUnconvRC1TRT_phi"  , pp_truthUnconvRecoConv1TRT.phi_prtcl  , pp_truthUnconvPhoton.phi_prtcl);
    Eff_Reco.divide("tUnconvRC2Si_phi"   , pp_truthUnconvRecoConv2Si.phi_prtcl   , pp_truthUnconvPhoton.phi_prtcl);
    Eff_Reco.divide("tUnconvRC2TRT_phi"  , pp_truthUnconvRecoConv2TRT.phi_prtcl  , pp_truthUnconvPhoton.phi_prtcl);
    Eff_Reco.divide("tUnconvRC2SiTRT_phi", pp_truthUnconvRecoConv2SiTRT.phi_prtcl, pp_truthUnconvPhoton.phi_prtcl);
    Eff_Reco.divide("tUnconvRUnconv_phi" , pp_truthUnconvRecoUnconv.phi_prtcl    , pp_truthUnconvPhoton.phi_prtcl);
    
    Eff_Reco.divide("tUnconvRConv_mu"   , pp_truthUnconvRecoConv.mu      , pp_truthUnconvPhoton.mu);
    Eff_Reco.divide("tUnconvRC1Si_mu"   , pp_truthUnconvRecoConv1Si.mu   , pp_truthUnconvPhoton.mu);
    Eff_Reco.divide("tUnconvRC1TRT_mu"  , pp_truthUnconvRecoConv1TRT.mu  , pp_truthUnconvPhoton.mu);
    Eff_Reco.divide("tUnconvRC2Si_mu"   , pp_truthUnconvRecoConv2Si.mu   , pp_truthUnconvPhoton.mu);
    Eff_Reco.divide("tUnconvRC2TRT_mu"  , pp_truthUnconvRecoConv2TRT.mu  , pp_truthUnconvPhoton.mu);
    Eff_Reco.divide("tUnconvRC2SiTRT_mu", pp_truthUnconvRecoConv2SiTRT.mu, pp_truthUnconvPhoton.mu);
    Eff_Reco.divide("tUnconvRUnconv_mu" , pp_truthUnconvRecoUnconv.mu    , pp_truthUnconvPhoton.mu);
    
    Eff_Reco.divide("tUnconvRConv_CR"   , pp_truthUnconvRecoConv.convRadius      , pp_truthUnconvPhoton.convRadius);
    Eff_Reco.divide("tUnconvRC1Si_CR"   , pp_truthUnconvRecoConv1Si.convRadius   , pp_truthUnconvPhoton.convRadius);
    Eff_Reco.divide("tUnconvRC1TRT_CR"  , pp_truthUnconvRecoConv1TRT.convRadius  , pp_truthUnconvPhoton.convRadius);
    Eff_Reco.divide("tUnconvRC2Si_CR"   , pp_truthUnconvRecoConv2Si.convRadius   , pp_truthUnconvPhoton.convRadius);
    Eff_Reco.divide("tUnconvRC2TRT_CR"  , pp_truthUnconvRecoConv2TRT.convRadius  , pp_truthUnconvPhoton.convRadius);
    Eff_Reco.divide("tUnconvRC2SiTRT_CR", pp_truthUnconvRecoConv2SiTRT.convRadius, pp_truthUnconvPhoton.convRadius);
    Eff_Reco.divide("tUnconvRUnconv_CR" , pp_truthUnconvRecoUnconv.convRadius    , pp_truthUnconvPhoton.convRadius);

  } // if gamma sampleType

  return StatusCode::SUCCESS;
}
