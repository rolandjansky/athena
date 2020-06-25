/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_CALOHADDMCOEFFMINIM_H
#define CALOLOCALHADCALIB_CALOHADDMCOEFFMINIM_H

/**
 * @class CaloHadDMCoefMinim
 * @version \$Id: CaloHadDMCoeffMinim.h,v 1.2 2009-05-18 20:31:52 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 25-Februar-2009
 * @brief Runs minimisation to get correction coefficients (i.e. new sampling weights)
 * for the energy deposited in dead material in front of FCAL
 *
 * */

#include <vector>
#include <string>
#include <map>

#include <TROOT.h>
#include "CaloGeoHelpers/CaloSampling.h"
#include "CxxUtils/checker_macros.h"

class CaloLocalHadCoeff;
class CaloLocalHadCoeffHelper;
class CaloHadDMCoeffData;


class ATLAS_NOT_THREAD_SAFE CaloHadDMCoeffMinim {
  public :

    // to handle minuit parameters and fit results
    class MinimPar {
      public:
        std::string name;
        bool        fixIt;
        double      initVal;
        double      initErr;
        double      lowerLim;
        double      upperLim;
        double      value;
        double      error;
        MinimPar(std::string the_name, double the_initVal, double the_initErr,
                   double the_lowerLim, double the_upperLim) 
        : name(the_name),
          fixIt(true),
          initVal(the_initVal),
          initErr(the_initErr),
          lowerLim(the_lowerLim),
          upperLim(the_upperLim),
          value(0.0),
          error(0.0)
        { }
    };

    // data is used for minimization
    class MinimSample {
      public:
        std::vector<float >   clsm_smp_energy_unw;
        int ibin;
        double edmtrue;
        double weight;
        double sigma2;
    };
 
    CaloHadDMCoeffMinim();
    ~CaloHadDMCoeffMinim();

    CaloLocalHadCoeff * process(CaloHadDMCoeffData *myData, CaloLocalHadCoeff *myHadDMCoeff , bool isSingleParticle=true, bool tbflag=false);
    void make_report(std::string &sfname);
    void SetNormalizationType(std::string &stype) {m_NormalizationType = stype;}

    std::vector<std::string> m_validNames;
  private:
    void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    static void fcnWrapper(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
    {
      s_instance->fcn(npar, gin, f, par, iflag);
    }

    static CaloHadDMCoeffMinim *s_instance;
    CaloHadDMCoeffData *m_data;
    CaloLocalHadCoeffHelper *m_HadDMHelper;
    CaloLocalHadCoeff *m_HadDMCoeff;

    bool m_isTestbeam;
    int m_nstep_fcn;
    int m_iBinGlob;
    std::vector<MinimPar > m_minimPars;
    std::map<int, std::vector<MinimPar > > m_minimResults;
    std::vector<MinimSample *> m_minimSample;
    std::vector<MinimSample *> m_minimSubSample;
    std::vector<int > m_sample_size;
    double m_engClusMin;
    double m_engBeamMin;
    int m_area_index;
    double m_distance_cut;

    std::string m_NormalizationType;
    int m_NormalizationTypeNumber;

    CaloHadDMCoeffMinim (const CaloHadDMCoeffMinim&);
    CaloHadDMCoeffMinim& operator= (const CaloHadDMCoeffMinim&);
};

#endif

