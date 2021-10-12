/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCSINGLEPIONSPERF_H
#define CALOLOCALHADCALIB_GETLCSINGLEPIONSPERF_H

/**
 * @class GetLCSinglePionsPerf
 * @version \$Id: GetLCSinglePionsPerf.h,v 1.1 2009-07-01 14:36:01 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 1-July-2009
 * @brief Top algorithm to get local hadronic calibration performance plots for single pions
 *
 */


#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "StoreGate/ReadHandleKey.h"
#include <vector>
#include <string>
#include <set>


class AtlasDetectorID;
class CaloCell_ID;
class CaloDmDescrManager;

class TFile;
class TH1F;
class TProfile;
class TProfile2D;

class GetLCSinglePionsPerf : public AthAlgorithm
{

  public:
    GetLCSinglePionsPerf(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~GetLCSinglePionsPerf();
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

    static void as_in_atlas ( double &eta,  double &phi, double X0,  double Y0,  double xCryo, int iCalo=2);
    static double angle_mollier_factor(double x);

 private:
    typedef std::pair<std::string,xAOD::CaloCluster::MomentType> moment_name_pair;
    typedef std::vector<moment_name_pair> moment_name_vector;

    enum keys_topo {kTOPO_EM, kTOPO_W, kTOPO_OOC, kTOPO};
    enum keys_tagged {kTAGEM, kTAGHAD, kTAGUNK, kTAG};
    enum keys_momsums {kMOM_CLS0, kMOM_CLS1, kMOM_CLS2, kMOM_SUMCLS, kMOM_MISS, kMOM_MCENER};
    enum keys_norm_level {kLEVEL_PARTICLE, kLEVEL_CALIBHIT, kLEVEL_CALIBHIT_IDEAL };

    const AtlasDetectorID* m_id_helper;
    const CaloCell_ID* m_calo_id;
    const CaloDmDescrManager *m_caloDmDescrManager;

    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterBasicCollName;
    SG::ReadHandleKeyArray<xAOD::CaloClusterContainer> m_clusterCollNames;
    std::string m_outputFileName;
    TFile * m_outputFile;
    SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_CalibrationHitContainerNames;
    SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_DMCalibrationHitContainerNames;
    moment_name_vector m_validMoments;

    double m_distance_cut;

    int   m_netabin;
    float m_etamin;
    float m_etamax;
    float m_deta;
    int   m_nphibin;
    float m_phimin;
    float m_phimax;
    float m_dphi;
    int   m_nlogenerbin;
    float m_logenermin;
    float m_logenermax;
    int   m_nlogenerbin2;
    float m_dlogener;
    float m_xcryo_min;
    float m_xcryo_max;
    float m_ytable_min;
    float m_ytable_max;
    int   m_nnormtype;        // pion energy, calibration level, calibration level with ideal previous step
    int   m_ncluscoll;        // CaloTopoCluster, CaloWTopoCluster, CaloOOCTopoCluster, CaloCalTopoCluster
    int   m_ntagcases;        // TAGGEDEM, TAGGEDHAD, TAGGEDUNKNOWN
    int   m_nmoments;         // number of moments assigned to clusters we want to study
    int   m_nmomsums;         // ratio of moment assigned to cls0, cls1, cls2, cls_sum, non assigned
//     int   m_nhitsums;         // number of histograms for calibration hits validation
   
    float m_truthPiEngFraction;
    bool m_isTestbeam;        // H6 testbeam flag
    bool m_doEngRecOverTruth;
    bool m_doEngTag;
    bool m_doEngRecSpect;
    bool m_doEngNoiseClus;
    bool m_doClusMoments;
    bool m_doRecoEfficiency;
    bool m_useRecoEfficiency;
    bool m_useGoodClus;
    bool m_doCalibHitsValidation;
    bool m_usePionClusters;

    // ereco over truth after each step of calibration
    std::vector<std::vector<std::vector<TProfile *> > > m_engRecOverTruth_vs_eta;   // [ncluscoll][nnormtype][nenerbin]
    std::vector<std::vector<std::vector<TProfile *> > > m_engRecOverTruth_vs_ebeam; // [ncluscoll][nnormtype][netabin]
    // classification performance
    std::vector<std::vector<TProfile *> > m_engTag_vs_eta;   // [ntagcases][nenerbin]
    std::vector<std::vector<TProfile *> > m_engTag_vs_ebeam; // [ntagcases][netabin]
    // reconstructed energy spectras after each step of calibration
    std::vector<std::vector<std::vector<TH1F *> > > m_engRecSpect; // [ncluscoll][nenerbin][netabin]
    // average incident pion energy in bins
    std::vector<std::vector<TH1F *> > m_engPionSpect;   // [nenerbin][netabin]
    // spectra of noise clusters
    std::vector<std::vector<TH1F *> > m_engNoiseClusSpect; // [ncluscoll][netabin]
    std::vector<TProfile *> m_engNoiseClus_vs_eta; // [ncluscoll]
    // cluster moments
    std::vector<std::vector<std::vector<TProfile *> > > m_clusMoment_vs_eta;   // [nmoments][nmomsums][nenerbin]
    std::vector<std::vector<std::vector<TProfile *> > > m_clusMoment_vs_ebeam; // [nmoments][nmomsums][netabin]
    // reconstruction efficiency
    std::vector<TH1F *> m_RecoEfficiency_vs_eta[2]; // [nenerbin]
    std::vector<TH1F *> m_RecoEfficiency_vs_ebeam[2]; // [netabin]
    // calibration hits
    std::vector<TProfile *> m_SumCalibHitOverEbeam_vs_eta;               // [nenerbin]
    //std::vector<TProfile *> m_SumCalibHitOverEbeam_vs_ebeam;             // [netabin]
    std::vector<TProfile *> m_DefaultCalculatorOverEbeam_vs_eta;         // [nenerbin]
    //std::vector<TProfile *> m_DefaultCalculatorOverEbeam_vs_ebeam;       // [netabin]
    std::vector<TProfile *> m_SumCalibHitAssignedOverEbeam_vs_eta; // [nenerbin]
    std::vector<TProfile2D *> m_SumCalibHitAssignedOverEbeam_vs_etaphi; // [nenerbin]

    double m_mc_ener;
    double m_mc_eta;
    double m_mc_phi;
    int m_mc_etabin;
    int m_mc_enerbin;
    int m_mc_phibin;

    int fill_reco (const xAOD::CaloClusterContainer& clusColl,
                   const EventContext& ctx);
    int fill_moments (const xAOD::CaloClusterContainer& clusColl,
                      const EventContext& ctx);
    int fill_calibhits (const xAOD::CaloClusterContainer& clusColl,
                        const EventContext& ctx);

};

#endif // LOCALHADCALIBCHECK_CHECKHITSASSIGNMENT_H

