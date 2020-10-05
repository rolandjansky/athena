/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_CALOHADDMCOEFFCHECK_H
#define CALOLOCALHADCALIB_CALOHADDMCOEFFCHECK_H

/**
 * @class CaloHadDMCoeffCheck
 * @version \$Id: CaloHadDMCoeffCheck.h,v 1.1 2009/11/03 11:10:01 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 3-November-2009
 * @brief Runs toy reconstruction to validate dead material constants
 *
 * Class is used for quick validation of dead material constants, it runs on
 * data provided by special dead material tree and fills dmreco .vs. dmtrue
 * histograms for different dead material areas
 *
 *
 * */

#include <vector>
#include <string>
#include <math.h>
#include "CxxUtils/checker_macros.h"
class CaloLocalHadCoeff;
class CaloLocalHadCoeffHelper;
class CaloHadDMCoeffData;
class TProfile;
class TH2F;
class TH1F;



/* 
 * Not thread safe due to ROOT facilities like gStyle
 * but is used only for validation/plotting
 * so should be fine
 */
class ATLAS_NOT_THREAD_SAFE CaloHadDMCoeffCheck {
  public :

    CaloHadDMCoeffCheck();
    ~CaloHadDMCoeffCheck();

    int process(CaloHadDMCoeffData *myData, CaloLocalHadCoeff *myHadDMCoeff, bool isSingleParticle=true, bool tbflag=false);
    void make_report(std::string &sfname);

  private:
    enum keys_comp {kENG_RECO, kENG_TRUTH, kENG_NORECO};
    enum dm_area_keys {
      sDM_EMB0_EMB1,   // 0 before PreSamplerB, between PreSamplerB and EMB1
      sDM_EMB3_TILE0,  // 1 between barrel and tile
      sDM_SCN,         // 2 before TileGap3 (scintillator)
      sDM_EME0_EME12,  // 3 before PreSamplerE, between PreSamplerE and EME1
      sDM_EME3_HEC0,   // 4 between EME3 and HEC0
      sDM_FCAL,        // 5 between HEC and FCAL, before FCAL
      sDM_LEAKAGE,     // 6 leakage behind calorimeter
      sDM_UNCLASS,     // 7 unclassified DM enegry
      sDM_DMTOT,       // 8 sum of DM energy over all zones
      sDM
    };

    void getDmReco(std::vector<std::vector<double > > &engDmReco);

    CaloHadDMCoeffData *m_data;
    CaloLocalHadCoeffHelper *m_HadDMHelper;
    CaloLocalHadCoeff *m_HadDMCoeff;

    bool  m_isTestbeam;
    float m_energyMin;
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
    float m_dlogener;
    int   m_nrecobin;
    int   m_npdg;
    bool  m_doTailRejection;

    std::vector<std::vector<std::vector<TH2F *> > > m_h2_etrue_vs_ereco; // [sDM][m_netabin][m_nlogenerbin]
    std::vector<std::vector<std::vector<TProfile *> > > m_hp_etrue_vs_ereco; // [sDM][m_netabin][m_nlogenerbin]
    std::vector<std::vector<std::vector<std::vector<TProfile *> > > > m_hp_engRecoOverTruth_vs_eta; //[npdg][sDM][nrecocases][m_nlogenerbin]
    std::vector<std::vector<std::vector<std::vector<std::vector<TH1F *> > > > > m_engRecSpect; // [npdg][sDM][nrecocases][nenerbin][netabin]

    int GetRmsWithoutTails(const TH1F *h1, float &mean, float &rms, float ths=3.0);

private:
    CaloHadDMCoeffCheck (const CaloHadDMCoeffCheck&);
    CaloHadDMCoeffCheck& operator= (const CaloHadDMCoeffCheck&);
};

#endif

