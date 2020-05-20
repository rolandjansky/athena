/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETPHYSVALMONITORING_InDetPerfPlot_hitResidual
#define INDETPHYSVALMONITORING_InDetPerfPlot_hitResidual
/**
 * @file InDetPerfPlot_hitResidual.h
 * @author shaun roe
 **/


// std includes
#include <string>

// local includes

#include "InDetPlotBase.h"

// could be fwd declared?
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"

// fwd declaration
class TH1;
class TProfile;

///class holding res plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_hitResidual: public InDetPlotBase {
public:
  InDetPerfPlot_hitResidual(InDetPlotBase* pParent, const std::string& dirName);

  void fill(const xAOD::TrackParticle& trkprt);
  ~InDetPerfPlot_hitResidual() {/** nop **/
  }

  void FillAdditionalITkPlots(bool fill=false);
  // we need a custom book method here to avoid having to hardcode identical histos for 28 layers in the XML
  void bookProfileForLayer(TProfile*& pHisto, const std::string& histoIdentifier, int index,const std::string& folder = "default");
  
  template <class HISTO>
  void bookProfileForType(HISTO*& pHisto, const std::string& histoIdentifier, const std::string& label,const std::string& folder = "default") {
    // first, load the identifier for all layers of this kind
    SingleHistogramDefinition hd = retrieveDefinition(histoIdentifier, folder);
    if (hd.empty()) {
      ATH_MSG_WARNING("Histogram definition is empty for identifier " << histoIdentifier);
    }
    // then give it a unique name 
    hd.name = histoIdentifier+"_"+label; 
    hd.title += std::string(" - "+label);
    book(pHisto, hd);
  }

private:
  // enum copied from the hitDecorator tool in InDetPhysValMonitoring
  enum Subdetector {
    INVALID_DETECTOR=-1, L0PIXBARR, PIXEL, SCT, TRT, DBM, N_SUBDETECTORS
  };
  enum Region {
    INVALID_REGION=-1, BARREL, ENDCAP, N_REGIONS
  };
  enum LayerType {
    INVALID_LAYER=-1, INNERMOST, NEXT_TO_INNERMOST, OTHER, N_TYPES
  };
  
  const static int N_LAYERS = 33;
  const static int N_SCTLAYERS = 4;
    
  void initializePlots();

  bool m_fillAdditionalITkPlots;

  TH1* m_residualx_1hit[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualx_2ormorehits[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualx[N_SUBDETECTORS][N_REGIONS];
  //
  TH1* m_residualy_1hit[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualy_2ormorehits[TRT][N_REGIONS]; // excludes TRT, DBM
  TH1* m_residualy[N_SUBDETECTORS][N_REGIONS];
  //
  TH1* m_residualpullx[N_SUBDETECTORS][N_REGIONS];
  TH1* m_residualpully[N_SUBDETECTORS][N_REGIONS];
  TH1* m_phiWidth[N_SUBDETECTORS][N_REGIONS];
  TH1* m_etaWidth[N_SUBDETECTORS][N_REGIONS];
  TProfile* m_phiWidth_eta[N_SUBDETECTORS][N_REGIONS];
  TProfile* m_etaWidth_eta[N_SUBDETECTORS][N_REGIONS];
  
  // only if m_fillAdditionalITkPlots == True
  TProfile* m_phiWidth_eta_perLayer[N_LAYERS][N_SUBDETECTORS][N_REGIONS];
  TProfile* m_etaWidth_eta_perLayer[N_LAYERS][N_SUBDETECTORS][N_REGIONS];
  // only for pixel subdetector 
  TH1* m_residualx_1hit_perType[N_TYPES];
  TH1* m_residualx_2ormorehits_perType[N_TYPES];
  TH1* m_residualx_perType[N_TYPES];
  //
  TH1* m_residualy_1hit_perType[N_TYPES];
  TH1* m_residualy_2ormorehits_perType[N_TYPES];
  TH1* m_residualy_perType[N_TYPES];
  //
  TH1* m_residualpullx_perType[N_TYPES];
  TH1* m_residualpully_perType[N_TYPES];
  TH1* m_phiWidth_perType[N_TYPES];
  TH1* m_etaWidth_perType[N_TYPES];
  TProfile* m_phiWidth_eta_perType[N_TYPES];
  TProfile* m_etaWidth_eta_perType[N_TYPES];
  
};


#endif
