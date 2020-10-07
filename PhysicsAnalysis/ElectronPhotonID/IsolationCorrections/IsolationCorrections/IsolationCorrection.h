/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_ISOLATIONCORRECTION_H
#define ISOLATIONCORRECTION_ISOLATIONCORRECTION_H

#include "AsgTools/AsgTool.h"
#include "AsgMessaging/AsgMessaging.h"
#include "TGraph.h"
#include "TObject.h"
#include "TF1.h"

#include "IsolationCorrections/ShowerDepthTool.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODPrimitives/IsolationType.h"

namespace CP {

class IsolationCorrection : public asg::AsgMessaging{
  // Create a proper constructor for Athena

  public:
    IsolationCorrection(std::string name);
    ~IsolationCorrection();

    StatusCode initialize();
    StatusCode finalize();

    typedef enum { ELECTRON=0, PHOTON=1 } ParticleType;
    typedef enum { REL17=1, REL17_2=2, REL20=6, REL20_2=7 } Version;

    void SetCorrectionFile( std::string corr_file, std::string corr_ddshift_file, std::string corr_ddsmearing_file, 
                            std::string corr_ddshift_2015_file);
    void SetToolVer(CP::IsolationCorrection::Version);

    float GetPtCorrectedIsolation(const xAOD::Egamma&, const xAOD::Iso::IsolationType);
    float GetPtCorrection(const xAOD::Egamma&, const xAOD::Iso::IsolationType) const;
    float GetDDCorrection(const xAOD::Egamma&) const;
    float GetDDCorrection_2015(const xAOD::Egamma&, const xAOD::Iso::IsolationType) const;

    float GetEtaPointing(const xAOD::Egamma*);

    void SetDataMC(bool is_mc);
    void SetDataMC(const xAOD::EventInfo*);
    void SetAFII(bool AFII_corr);
    //void SetDD(bool apply_dd);
    void SetCorrectEtcone(bool correct_etcone);
    void SetTroubleCategories(bool trouble_categories);

  private:

    CP::IsolationCorrection::Version m_tool_ver;

    std::vector<float> m_eta_bins_fine;
    std::vector<float> m_eta_bins_coarse;
    std::vector<float> m_eta_bins_dd;
    std::vector<float> m_eta_bins_dd_2015;
    const unsigned int m_nBinsEtaFine;
    const unsigned int m_nBinsEtaCoarse;
    const unsigned int m_nBinsEtaDD;
    const unsigned int m_nBinsEtaDD_2015;
    std::string m_corr_file;
    std::string m_corr_ddshift_2015_file;
    std::string m_corr_ddshift_file;
    std::string m_corr_ddsmearing_file;

    // for v3 DD : absolute eta
    unsigned int m_nBinsfEtaDD_2015;
    std::vector<float> m_feta_bins_dd_2015;
    
    bool m_is_mc;
    bool m_AFII_corr;
    bool m_set_mc;
    bool m_correct_etcone;
    bool m_trouble_categories;

    CP::ShowerDepthTool* m_shower;

    // booleans for helping with versions
    bool m_isv2;
    bool m_isv3;

    template <class T> void FreeClear( T & cntr );

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
				  ParticleType parttype = ELECTRON);

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
    void set2015Corr();
    void setDDCorr();
    void setDDCorr_2015();

    void load2012Corr();
    void load2015Corr();
    void loadDDCorr();
    void load2015DDCorr();

    float getPtAtFirstMeasurement( const xAOD::TrackParticle* tp) const;
    int GetConversionType(int conversion_flag, float conv_radius, float conv_ratio) const;

    // ----------------------------------------------------------------
    // ------------ OLD internal methods ------------------------------
    // ----------------------------------------------------------------


    float GetPtCorrection (float energy, float etaS2, float etaPointing, float etaCluster,
                           float radius, bool is_mc=true,
                           bool isConversion=false, ParticleType parttype = ELECTRON) const;

    float GetPtCorrectionTopo (float energy, float etaS2, float etaPointing, float etaCluster,
                               float radius, bool is_mc=true,
                               bool isConversion=false, ParticleType parttype = ELECTRON, Version ver = REL17_2,
			       int convFlag_int=0, int author=0, float conv_radius=0., float conv_ratio=0.) const;
    int GetRadius(float radius) const;
    int GetEtaBinFine(float eta) const;
    int GetEtaBinCoarse(float eta) const;
    int GetEtaBinDD(float eta) const;

    float GetPtCorrectionFactor(float eta, std::vector<float> mc_leakage_corrections_ptr =std::vector<float>(),
				                        std::vector<float> data_leakage_corrections_ptr= std::vector<float>()) const;
    float GetPtCorrectionValue(float energy, float etaPointing, float etaCluster, float scale_factor) const;
    float GetPtCorrection_FromGraph(float energy,float etaS2,float etaPointing,float etaCluster,float radius,bool isConversion,ParticleType parttype) const;
    float GetPtCorrection_FromGraph_2015(float energy, float etaS2, float radius, int conversion_flag, int author, float conv_radius, float conv_ratio, ParticleType parttype) const;

    // -------------------------------------------------------------------------------------------
    // ------------- data-driven corrections based on 2015 data ----------------------------------
    // corrections recomputed for topological isolation, 0.4/0.2 cone. special eta binning -------
    // ------------------------- -----------------------------------------------------------------

    std::vector<TGraph*> m_graph_dd_2015_cone40_unconv_photon_shift;
    std::vector<TGraph*> m_graph_dd_2015_cone40_conv_photon_shift;
    std::vector<TGraph*> m_graph_dd_2015_cone20_unconv_photon_shift;
    std::vector<TGraph*> m_graph_dd_2015_cone20_conv_photon_shift;

    // -------------------------------------------------------------------------------------------
    // ------------- data-driven corrections based on 2012 inclusive photon sample ---------------
    // corrections recomputed for topological isolation, 0.4 cone. special eta binning -----------
    // ------------------------- https://cds.cern.ch/record/2008664 ------------------------------

    std::vector<TF1*> m_graph_dd_cone40_photon_shift;
    std::vector<TF1*> m_graph_dd_cone40_photon_smearing;

    // -------------------------------------------------------------------------------------------
    // ------------- full 2015 (rel 20_2) leakage corrections --------------------------------------
    // corrections recomputed for topological isolation: fine grained in eta, derived from MC15

    //---- Rel 20_2 pT leakage correction file : all categories
    std::vector<TF1*> m_function_2015_cone40_photon_unconverted;
    std::vector<TF1*> m_function_2015_cone30_photon_unconverted;
    std::vector<TF1*> m_function_2015_cone20_photon_unconverted;

    std::vector<TF1*> m_function_2015_cone40_photon_converted_ok;
    std::vector<TF1*> m_function_2015_cone30_photon_converted_ok;
    std::vector<TF1*> m_function_2015_cone20_photon_converted_ok;

    std::vector<TF1*> m_function_2015_cone40_photon_converted_trouble;
    std::vector<TF1*> m_function_2015_cone30_photon_converted_trouble;
    std::vector<TF1*> m_function_2015_cone20_photon_converted_trouble;

    std::vector<TF1*> m_function_2015_cone40_author_1_electron;
    std::vector<TF1*> m_function_2015_cone30_author_1_electron;
    std::vector<TF1*> m_function_2015_cone20_author_1_electron;

    std::vector<TF1*> m_function_2015_cone40_author_16_electron;
    std::vector<TF1*> m_function_2015_cone30_author_16_electron;
    std::vector<TF1*> m_function_2015_cone20_author_16_electron;

    //---- Rel 20_2 pT leakage correction with histogram mean file : for "trouble categories", below pT threshold
    std::vector<TGraph*> m_graph_histoMean_2015_cone40_photon_converted_trouble;
    std::vector<TGraph*> m_graph_histoMean_2015_cone30_photon_converted_trouble;
    std::vector<TGraph*> m_graph_histoMean_2015_cone20_photon_converted_trouble;

    std::vector<TGraph*> m_graph_histoMean_2015_cone40_author_16_electron;
    std::vector<TGraph*> m_graph_histoMean_2015_cone30_author_16_electron;
    std::vector<TGraph*> m_graph_histoMean_2015_cone20_author_16_electron;


    // -------------------------------------------------------------------------------------------
    // ------------- full 2012 (rel 17.2) leakage corrections --------------------------------------
    // corrections recomputed for topological isolation: fine grained in eta, derived from MC12

    //---- Rel 17.2 pT leakage correction file
    std::vector<TGraph*> m_graph_cone40_photon_unconverted;
    std::vector<TGraph*> m_graph_cone30_photon_unconverted;
    std::vector<TGraph*> m_graph_cone20_photon_unconverted;

    std::vector<TGraph*> m_graph_cone40_photon_converted;
    std::vector<TGraph*> m_graph_cone30_photon_converted;
    std::vector<TGraph*> m_graph_cone20_photon_converted;

    std::vector<TGraph*> m_graph_cone40_electron;
    std::vector<TGraph*> m_graph_cone30_electron;
    std::vector<TGraph*> m_graph_cone20_electron;

    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // ------------- full 2011 (rel 17) etcone leakage corrections ---------------
    // 
    std::vector<float> m_mc_rel17_leakage_correction_slopes_electron_15;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_electron_20;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_electron_25;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_electron_30;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_electron_35;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_electron_40;

    std::vector<float> m_mc_rel17_leakage_correction_offsets_electron_15;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_electron_20;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_electron_25;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_electron_30;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_electron_35;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_electron_40;

    // photons: last eta bin isn't used
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_converted_15;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_converted_20;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_converted_25;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_converted_30;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_converted_35;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_converted_40;
 
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_converted_15;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_converted_20;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_converted_25;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_converted_30;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_converted_35;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_converted_40;

    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_unconverted_15;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_unconverted_20;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_unconverted_25;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_unconverted_30;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_unconverted_35;
    std::vector<float> m_mc_rel17_leakage_correction_slopes_photon_unconverted_40;

    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_unconverted_15;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_unconverted_20;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_unconverted_25;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_unconverted_30;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_unconverted_35;
    std::vector<float> m_mc_rel17_leakage_correction_offsets_photon_unconverted_40;

};

}

#endif
