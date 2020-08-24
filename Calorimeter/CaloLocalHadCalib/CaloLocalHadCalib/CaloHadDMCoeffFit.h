/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_CALOHADDMCOEFFFIT_H
#define CALOLOCALHADCALIB_CALOHADDMCOEFFFIT_H

/**
 * @class CaloHadDMCoeffFit
 * @version \$Id: CaloHadDMCoeffFit.h,v 1.1 2009/03/03 17:30:21 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 25-Februar-2009
 * @brief To fill and fit TProfile histograms using special dead material tree
 *
 * Class is used to fill and fit TProfile histograms of type
 * ( edmtrue .versus. eng_presampler) or
 * ( edmtrue .versus. sqrt(eng_emb3*eng_tile0)
 * and also for filling of lookup tables for energy leakages and unclassified 
 * dead material areas to get dead material correction coefficients.
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
class TProfile2D;


class ATLAS_NOT_THREAD_SAFE CaloHadDMCoeffFit {
  public :

  // average and rms on-the-flight calculation
  class PrepData {
    public:
      unsigned int n_entries;
      double m_sum;
      double m_aver;
      double m_rms;
      double m_sw;
      PrepData(): n_entries(0),m_sum(0.0),m_aver(0.0),m_rms(0.0),m_sw(0.0) { }
      ~PrepData(){}
      void add(double xx, double w=1.0)
      {
        n_entries++;
        m_sum += xx;
        m_rms = (m_sw/(m_sw+w))*(m_rms+(w/(m_sw+w))*(xx-m_aver)*(xx-m_aver));
        m_aver = m_aver+(xx-m_aver)*w/(m_sw+w);
        m_sw += w;
      }
      unsigned int size() { return n_entries;}
    };

    // fit data
    class FitData {
      public:
        bool isOK;
        float p0, p1, s0, s1;
        std::string descr;
        FitData() : isOK(true), p0(0), p1(0), s0(0), s1(0), descr("def") {}
        FitData(float the_p0, float the_s0, float the_p1, float the_s1)
          : isOK(true), p0(the_p0), p1(the_p1), s0(the_s0), s1(the_s1), descr("def") {}
        void getInverted(float &p0inv, float &s0inv, float &p1inv, float &s1inv)
        {
          p0inv=0.0; s0inv=0.0; p1inv=0.0; s1inv=0.0; 
          if(p1!=0) {
            p0inv = -1.0*p0/p1;
            p1inv = 1./p1;
            s0inv = sqrt((s0*s0)/(p1*p1) + (s1*s1)*(p0*p0)/(p1*p1*p1*p1));
            s1inv = s1/(p1*p1);
          }
        }
    };

    CaloHadDMCoeffFit();
    ~CaloHadDMCoeffFit();

    CaloLocalHadCoeff * process(CaloHadDMCoeffData *myData, CaloLocalHadCoeff *myHadDMCoeff, bool isSingleParticle=true, bool tbflag=false);
    void make_report(std::string &sfname);
    void SetNormalizationType(std::string &stype) {m_NormalizationType = stype;}

  private:
    CaloHadDMCoeffData *m_data;
    CaloLocalHadCoeffHelper *m_HadDMHelper;

    CaloLocalHadCoeff *m_HadDMCoeff;

    bool m_isTestbeam;
    double m_energyMin;
    double m_weightMax;
    double m_distance_cut;

    std::string m_NormalizationType;
    int m_NormalizationTypeNumber;

    std::vector<PrepData *> m_engClus;
    std::vector<PrepData *> m_engPrep;
    std::vector<PrepData *> m_engDm;
    std::vector<PrepData *> m_engDmOverClus;
    std::vector<TProfile *> m_hp_DmVsPrep;
    std::vector<TH2F *>     m_h2_DmVsPrep;
    std::vector<TH1F *>     m_h1_engDmOverClus;
    std::vector<FitData *>  m_FitData;

    std::vector<TProfile2D *> m_hp2_DmWeight;

    void clear();
    double ProfileRefiner(TProfile *pH, double ev_ratio=0.92);
    double GetAverageWithoutRightTail(TH1F *pH, double ev_ratio=0.92);
    int getFirstEnerLambdaBin(int ibin);

    CaloHadDMCoeffFit (const CaloHadDMCoeffFit&);
    CaloHadDMCoeffFit& operator= (const CaloHadDMCoeffFit&);
};

#endif

