/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TtresChi2_H
#define TtresChi2_H

#include<iostream>
#include <string>
#include <vector>
#include "TLorentzVector.h"
#include "TopEventReconstructionTools/TtresNeutrinoBuilder.h"


class TtresChi2{
 public:
  TtresChi2(std::string units="GeV");
  virtual ~TtresChi2();

  enum WReco {ROTATION, REALPART, DECREASING};
  enum Chi2Version {DATA2011SUMMER2012, DATA2012AUTOMN2012, DATA2012SUMMER2013, DATA2012SUMMER2013PT100};
  enum PtDiff {NOPTDIFF, PTDIFF, PTDIFFREL, PTDIFFMASS};
  enum Btag {STDBTAG, AFFECTBTAG, NO_BTAGHM, NO_BTAG};
  enum RunMode {RUNDEFAULT, RUNSTUDY};

 private:
  TtresChi2(const TtresChi2&);
  TtresChi2& operator=(const TtresChi2&);
  std::vector<TLorentzVector*> GetNeutrinos(TLorentzVector*, TLorentzVector*);

  double MjjP,  SMjjP,  m_TopMinusW_had_mean,  m_TopMinusW_had_sigma,  m_Top_lep_mean,  m_Top_lep_sigma, m_PtDiff_mean, m_PtDiff_sigma, m_PtDiffRel_mean, m_PtDiffRel_sigma, m_PtDiffRelMass_mean, m_PtDiffRelMass_sigma, MTHJJ, STHJJ;
  double m_WhChi2Value,m_ThWhChi2Value,m_TlChi2Value,m_PtDiffChi2Value;
  int    m_debug;
  PtDiff m_UsePtDiff;
  Btag   m_Btag;
  WReco  m_WReco;
  RunMode m_RunMode;
  double m_Units;
  TtresNeutrinoBuilder* m_NeutrinoBuilder;
  double m_highJetMass;
  int m_category;

  double res_chi2All, res_chi2WH, res_chi2TopH, res_chi2TopL, res_Mtl, res_Mwh, res_Mth, res_Mtt;
  TLorentzVector res_Tt;

  std::vector < double > m_chi2Values;
  std::vector < double > m_chi2Wh_Values;
  std::vector < double > m_chi2Th_Values;
  std::vector < double > m_chi2ThWh_Values;
  std::vector < double > m_chi2Tl_Values;
  std::vector < double > m_chi2PtDiff_Values;
  
  std::vector < double > m_PtDiff_Values;
  std::vector < double > m_Wh_Values;
  std::vector < double > m_ThWh_Values;
  std::vector < double > m_Th_Values;
  std::vector < double > m_Tl_Values;
  
  std::vector < int > m_i_Wq1;
  std::vector < int > m_i_Wq2;
  std::vector < int > m_i_Thb;
  std::vector < int > m_i_Tlb;
  std::vector < int > m_i_n;
  
  int m_nChi2Values;
  
 public:
  void Init(Chi2Version version = DATA2012SUMMER2013, double highJetMass = 60.0);
  void Reset();
  bool findMinChiSquare_HMelseLM(TLorentzVector*, const std::vector<TLorentzVector*>*, const std::vector<bool>*, TLorentzVector*, int&, int&, int&, int&, int&, double&, double&, double&);
  bool findMinChiSquare_LMelseHM(TLorentzVector*, const std::vector<TLorentzVector*>*, const std::vector<bool>*, TLorentzVector*, int&, int&, int&, int&, int&, double&, double&, double&);

  bool findMinChiSquare_AllRanges(TLorentzVector*, const std::vector<TLorentzVector*>*, const std::vector<bool>*, TLorentzVector*, int&, int&, int&, int&, int&, double&, double&, double&);

  bool findMinChiSquare(TLorentzVector*, const std::vector<TLorentzVector*>*, const std::vector<bool>*, TLorentzVector*, int&, int&, int&, int&, int&, double&, double&, double&);

  bool findMinChiSquare_HighMass(TLorentzVector*, const std::vector<TLorentzVector*>*, const std::vector<bool>*, TLorentzVector*, int&, int&, int&, int&, double&, double&, double&);

  bool findMinChiSquare_VeryHighMass(TLorentzVector*, const std::vector<TLorentzVector*>*, const std::vector<bool>*, TLorentzVector*, int&, int&, int&, double&, double&, double&);

  inline void SetDebugMode(bool X){m_debug=X;if(m_debug>2)m_NeutrinoBuilder->setdebuglevel(m_debug);}
  inline void SetWReconstruction(WReco X){m_WReco=X;}
  inline void SetBtagHandeling(Btag X){m_Btag=X;}
  inline void SetHighMThreshold(double highJetMass) {m_highJetMass = highJetMass;}
  inline void UsePtDiff(PtDiff X){m_UsePtDiff=X;}
  inline void SetRunMode(RunMode X){m_RunMode=X;}

  inline double getResult_Chi2All() 	{return res_chi2All;}
  inline double getResult_Chi2WH()  	{return res_chi2WH;}
  inline double getResult_Chi2TopH()	{return res_chi2TopH;}
  inline double getResult_Chi2TopL()	{return res_chi2TopL;}
  inline double getResult_Mtl()     	{return res_Mtl;}
  inline double getResult_Mwh()     	{return res_Mwh;}
  inline double getResult_Mth()     	{return res_Mth;}
  inline double getResult_Mtt()     	{return res_Mtt;}
  
  inline std::vector < double > getAllChi2Values() 		{return m_chi2Values;	    }
  inline std::vector < double > getAllWh_Chi2Values() 		{return m_chi2Wh_Values;    }
  inline std::vector < double > getAllTh_Chi2Values() 		{return m_chi2Th_Values;    }
  inline std::vector < double > getAllThWh_Chi2Values()		{return m_chi2ThWh_Values;  }
  inline std::vector < double > getAllTl_Chi2Values() 		{return m_chi2Tl_Values;    }
  inline std::vector < double > getAllPtDiff_Chi2Values() 	{return m_chi2PtDiff_Values;}
  inline std::vector < int > getAllIndicesWq1()			{return m_i_Wq1;	    }
  inline std::vector < int > getAllIndicesWq2()			{return m_i_Wq2;	    }
  inline std::vector < int > getAllIndicesThb()			{return m_i_Thb;	    }
  inline std::vector < int > getAllIndicesTlb()			{return m_i_Tlb;	    }
  inline std::vector < int > getAllIndicesN()			{return m_i_n;	   	    }
  
  inline std::vector < double > getAllPtDiff_Values() 		{return m_PtDiff_Values;	}
  inline std::vector < double > getAllWh_Values() 		{return m_Wh_Values;		}
  inline std::vector < double > getAllThWh_Values() 		{return m_ThWh_Values;		}
  inline std::vector < double > getAllTh_Values() 		{return m_Th_Values;		}
  inline std::vector < double > getAllTl_Values() 		{return m_Tl_Values;		}
  
  inline double getWhChi2Value()	{return m_WhChi2Value;	}
  inline double getThWhChi2Value()   	{return m_ThWhChi2Value;}
  inline double getTlChi2Value()   	{return m_TlChi2Value;	}
  inline double getPtDiffChi2Value()   	{return m_PtDiffChi2Value;}
  inline int getNChi2Values()       	{return m_nChi2Values;	}
  inline int getCategory()		{return m_category;	}
  inline TLorentzVector getResult_Tlv()	{return res_Tt;		}

};


#endif
