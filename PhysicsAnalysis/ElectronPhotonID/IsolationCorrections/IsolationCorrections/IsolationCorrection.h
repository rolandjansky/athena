/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_ISOLATIONCORRECTION_H
#define ISOLATIONCORRECTION_ISOLATIONCORRECTION_H

#include "AsgTools/AsgTool.h"
#include "TGraph.h"
#include "TObject.h"

#include "IsolationCorrections/ShowerDepthTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODPrimitives/IsolationType.h"

namespace CP {

class IsolationCorrection{
  // Create a proper constructor for Athena
  
  public:
    IsolationCorrection();
    ~IsolationCorrection();

    StatusCode initialize();
    StatusCode finalize();
    
    typedef enum { ELECTRON=0, PHOTON=1 } ParticleType;
    typedef enum { REL17=1, REL17_2=2, REL18=3 } Version;
   
    void SetCorrectionFile(std::string corr_file);
    void SetToolVer(CP::IsolationCorrection::Version);
  
    float GetPtCorrectedIsolation(const xAOD::Egamma&, const xAOD::Iso::IsolationType);
    float GetPtCorrection(const xAOD::Egamma&, const xAOD::Iso::IsolationType);
    
    float GetEtaPointing(const xAOD::Egamma*);
    
    void SetDataMC(bool is_mc);
    void SetDataMC(const xAOD::EventInfo*);
 
  private:
	
    CP::IsolationCorrection::Version m_tool_ver;
	
    std::vector<float> m_eta_bins_fine;
    std::vector<float> m_eta_bins_coarse;
    const unsigned int m_nBinsEtaFine;
    const unsigned int m_nBinsEtaCoarse;
    std::string m_corr_file;
    
    bool m_is_mc;
    bool m_set_mc;    
    
    template <class T> void FreeClear( T & cntr );
    CP::ShowerDepthTool* m_shower;
    
    // ------------------------------------------------------------
    // --------------------- OLD user methods ---------------------
    // ------------------------------------------------------------

    // --------------------------------------------------------
    // --------------- pT leakage corrections -----------------
    // - equivalent to "EtconeXX_pt_corrected" variables
    
    float GetPtCorrectedIsolation(float energy,
				  float etaS2,
				  float etaPointing,
				  float etaCluster,
				  float radius,
				  bool is_mc,
				  float Etcone_value,
				  bool isConversion = false,
				  ParticleType parttype = ELECTRON,
				  Version ver = REL17_2);
  
    // --------------------------------------------------------
    // ---------- pT leakage corrections, topoIso -------------
  
    float GetPtCorrectedTopoIsolation(float energy,
				      float etaS2,
				      float etaPointing,
				      float etaCluster,
				      float radius,
				      bool is_mc,
				      float Etcone_value,
				      bool isConversion = false,
				      ParticleType parttype = ELECTRON,
				      Version ver = REL17_2);


    // ---------------------------------------------------------
    // ----------- errors on pT leakage corrections ------------
  
    // to get the error on the data-derived corrections
    float GetPtCorrectedIsolationError(float energy,
				       float etaS2,
				       float etaPointing,
				       float etaCluster,
				       float radius,
				       bool is_mc,
				       ParticleType parttype = ELECTRON,
				       Version ver = REL17_2);

    void setEtaBins();
    void setIsolCorr();
    
    void set2011Corr();
    void set2012Corr();
    void set2014Corr();

    void load2012Corr();
    void load2014Corr();
    
    // ----------------------------------------------------------------
    // ------------ OLD internal methods ------------------------------
    // ----------------------------------------------------------------
    
    
    float GetPtCorrection (float energy, float etaS2, float etaPointing, float etaCluster, 
                           float radius, bool is_mc=true, 
                           bool isConversion=false, ParticleType parttype = ELECTRON, Version ver = REL17_2);
                           
    float GetPtCorrectionTopo (float energy, float etaS2, float etaPointing, float etaCluster, 
                               float radius, bool is_mc=true, 
                               bool isConversion=false, ParticleType parttype = ELECTRON, Version ver = REL17_2);
                               
    int GetRadius(float radius);
    int GetEtaBinFine(float eta);
    int GetEtaBinCoarse(float eta);
    
    float GetPtCorrectionFactor(float eta, std::vector<float> mc_leakage_corrections_ptr =std::vector<float>(), 
				                std::vector<float> data_leakage_corrections_ptr= std::vector<float>());
    float GetPtCorrectionValue(float energy, float etaPointing, float etaCluster, float scale_factor);
    float GetPtCorrection_FromGraph(float energy,float etaS2,float etaPointing,float etaCluster,float radius,bool isConversion,ParticleType parttype);
    float GetPtCorrection_FromGraph_2014(float energy,float etaS2,float etaPointing,float etaCluster,float radius,bool isConversion,ParticleType parttype);

	// -------------------------------------------------------------------------------------------
    // ------------- full 2014 (rel 18) leakage corrections --------------------------------------
    // corrections recomputed for topological isolation: fine grained in eta, derived from MC14

    //---- Rel 17.2 pT leakage correction file
    std::vector<TGraph*> graph_2014_cone40_photon_unconverted;
    std::vector<TGraph*> graph_2014_cone30_photon_unconverted;
    std::vector<TGraph*> graph_2014_cone20_photon_unconverted;

    std::vector<TGraph*> graph_2014_cone40_photon_converted;
    std::vector<TGraph*> graph_2014_cone30_photon_converted;
    std::vector<TGraph*> graph_2014_cone20_photon_converted;

    std::vector<TGraph*> graph_2014_cone40_electron;
    std::vector<TGraph*> graph_2014_cone30_electron;
    std::vector<TGraph*> graph_2014_cone20_electron;
	  
    // -------------------------------------------------------------------------------------------
    // ------------- full 2012 (rel 17.2) leakage corrections --------------------------------------
    // corrections recomputed for topological isolation: fine grained in eta, derived from MC12

    //---- Rel 17.2 pT leakage correction file
    std::vector<TGraph*> graph_cone40_photon_unconverted;
    std::vector<TGraph*> graph_cone30_photon_unconverted;
    std::vector<TGraph*> graph_cone20_photon_unconverted;

    std::vector<TGraph*> graph_cone40_photon_converted;
    std::vector<TGraph*> graph_cone30_photon_converted;
    std::vector<TGraph*> graph_cone20_photon_converted;

    std::vector<TGraph*> graph_cone40_electron;
    std::vector<TGraph*> graph_cone30_electron;
    std::vector<TGraph*> graph_cone20_electron;

    // ----------------------------------------------------------------------------
    
    // -------------------------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) leakage corrections --------------------------------------
    // CURRENT isolation corrections: fine grained in eta, derived from MC11
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_15;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_20;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_25;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_30;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_35;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_40;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_tp20;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_tp30;
    std::vector<float> mc_rel17_leakage_correction_slopes_electron_tp40;
    
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_15;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_20;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_25;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_30;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_35;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_40;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_tp20;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_tp30;
    std::vector<float> mc_rel17_leakage_correction_offsets_electron_tp40;
    
    // photons: last eta bin isn't used
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_15;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_20;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_25;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_30;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_35;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_40;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_tp20;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_tp30;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_converted_tp40;
    
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_15;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_20;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_25;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_30;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_35;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_40;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_tp20;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_tp30;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_converted_tp40;
    
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_15;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_20;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_25;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_30;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_35;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_40;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_tp20;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_tp30;
    std::vector<float> mc_rel17_leakage_correction_slopes_photon_unconverted_tp40;
    
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_15;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_20;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_25;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_30;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_35;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_40;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_tp20;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_tp30;
    std::vector<float> mc_rel17_leakage_correction_offsets_photon_unconverted_tp40;
    // ------------------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------------------
    
    // -------------------------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) pileup corrections --------------------------------------
    // all corrections use >= 2 tracks/vertex
    
    // linear corrections from Zee rel 17 full 2011 data
    // - not used in favor of quadratic corrections below
    //     std::vector<float> rel17_npv_correction_slopes_20;
    //     std::vector<float> rel17_npv_correction_slopes_30;
    //     std::vector<float> rel17_npv_correction_slopes_40;
 
    // quadratic corrections from rel 17 full 2011 data    
    std::vector<float> rel17_npv_correction_slopes_20;
    std::vector<float> rel17_npv_correction_slopes_30;
    std::vector<float> rel17_npv_correction_slopes_40;

    std::vector<float> rel17_npv_correction_quads_20;
    std::vector<float> rel17_npv_correction_quads_30;
    std::vector<float> rel17_npv_correction_quads_40;

    // -------------------------------------------------------------------------------------------
  };

}

#endif
