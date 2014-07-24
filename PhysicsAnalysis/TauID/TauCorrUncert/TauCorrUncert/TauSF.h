/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSF_H
#define TAUSF_H

#include "CommonSF.h"

namespace TauCorrUncert {

  typedef enum {
    IDNONE,
    BDTLOOSE,
    BDTMEDIUM,
    BDTTIGHT,
    BDTFAIL,
    BDTOTHER,
    LLHLOOSE,
    LLHMEDIUM,
    LLHTIGHT,
    LLHFAIL
  } ID;

  typedef enum {
    NONE,
    LOOSE,
    MEDIUM,
    TIGHT,
    OTHER
  } EVETO;

  typedef enum {
    OLRNONE,
    LOOSEPP,
    MEDIUMPP,
    TIGHTPP,
    OLROTHER
  } OLR;


  class TauSF : public CommonSF
  {

  public:

    TauSF(const std::string& sharepath,
	  bool debug=false);

    ~TauSF()
    {
      delete m_mEVeto;
      delete m_mEVeto_3P;
    }

    double GetIDSF(ID level,
		   double eta,
		   int prongness,
		   double pT=30);

    double GetIDSysUnc(ID level,
		      double eta,
		      int prongness,
		      double pT=30);

    double GetIDStatUnc(ID level,
			double eta,
			int prongness,
			double pT=30);

    double GetExclIDSF(ID level,
		       double eta,
		       int prongness,
		       double pT=30);

    double GetExclIDSysUnc(ID level,
			   double eta,
			   int prongness,
			   double pT=30);

    double GetExclIDStatUnc(ID level,
			    double eta,
			    int prongness,
			    double pT=30);

    double GetEVetoSF(float trackEta,
		      ID level,
		      EVETO evLevel,
		      OLR olr);

    double GetEVetoSFUnc(float trackEta,
			 ID level,
			 EVETO evLevel,
			 OLR olr,
			 int direction);

    double GetEVetoSF(float trackEta,
		      int nTrack,
		      ID level,
		      EVETO evLevel,
		      OLR olr);

    double GetEVetoSFUnc(float trackEta,
			 int nTrack,
			 ID level,
			 EVETO evLevel,
			 OLR olr,
			 int direction);
    
    void SwitchOnPtBinnedIDSF(bool bSwitch = true);
    void SwitchOnEtaBinnedIDSF(bool bSwitch = true);
    void SwitchOnHighPtUncert(bool bSwitch = true);

  private:
    std::string ConvertEtaToString(const float& fEta);
    std::string ConvertIDToString(ID level);
    std::string ConvertEVetoToString(EVETO level);
    std::string ConvertOLRToString(OLR level);
    void checkTrackEtaValidity(float& trackEta);
    float GetIDValue(const std::string& sWorkingPoint, const float& fPt);

    SFMAP* m_mEVeto;
    SFMAP* m_mEVeto_3P;
    bool m_bUseHighPtUncert;
    std::string m_sIDpt;
    bool m_bUseInclusiveEta;
    ClassDef(TauSF, 1)
  };
} // namespace TauCorrUncert

#endif // TAUSF_H

/*
#ifndef TAUSF_H
#define TAUSF_H

#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>
#include <typeinfo>

#include "TROOT.h"
#include "TClass.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TObject.h"
#include "TLorentzVector.h"
#include "TKey.h"
#include "TParameter.h"

namespace TauCorrUncert {

  typedef enum {
    IDNONE,
    BDTLOOSE,
    BDTMEDIUM,
    BDTTIGHT,
    BDTFAIL,
    BDTOTHER,
    LLHLOOSE,
    LLHMEDIUM,
    LLHTIGHT,
    LLHFAIL
  } ID;

  typedef enum {
    NONE,
    LOOSE,
    MEDIUM,
    TIGHT,
    OTHER
  } EVETO;

  typedef enum {
    OLRNONE,
    LOOSEPP,
    MEDIUMPP,
    TIGHTPP,
    OLROTHER
  } OLR;


  class TauSF : public TObject
  {

  public:

    TauSF(const std::string& sharepath,
	  bool debug=false);

    ~TauSF()
    {
      delete m_mSF;
      delete m_mEVeto;
      delete m_mEVeto_3P;
    }

    double GetIDSF(ID level,
		   double eta,
		   int prongness,
		   double pT=30);

    double GetIDSysUnc(ID level,
		      double eta,
		      int prongness,
		      double pT=30);

    double GetIDStatUnc(ID level,
			double eta,
			int prongness,
			double pT=30);

    double GetExclIDSF(ID level,
		       double eta,
		       int prongness,
		       double pT=30);

    double GetExclIDSysUnc(ID level,
			   double eta,
			   int prongness,
			   double pT=30);

    double GetExclIDStatUnc(ID level,
			    double eta,
			    int prongness,
			    double pT=30);

    double GetEVetoSF(float trackEta,
		      ID level,
		      EVETO evLevel,
		      OLR olr);

    double GetEVetoSFUnc(float trackEta,
			 ID level,
			 EVETO evLevel,
			 OLR olr,
			 int direction);

    double GetEVetoSF(float trackEta,
		      int nTrack,
		      ID level,
		      EVETO evLevel,
		      OLR olr);

    double GetEVetoSFUnc(float trackEta,
			 int nTrack,
			 ID level,
			 EVETO evLevel,
			 OLR olr,
			 int direction);
    
    void SwitchOnPtBinnedIDSF(bool bSwitch = true);
    void SwitchOnEtaBinnedIDSF(bool bSwitch = true);
    void SwitchOnHighPtUncert(bool bSwitch = true);

  private:

    std::string ConvertProngToString(int prongness);
    std::string ConvertIDToString(ID level);
    std::string ConvertEVetoToString(EVETO level);
    std::string ConvertOLRToString(OLR level);
    std::string ConvertEtaToString(const float& fEta);
    void checkTrackEtaValidity(float& trackEta);
    float GetIDValue(const std::string& sWorkingPoint, const float& fPt);

    typedef std::map<std::string, TH1F*> SFMAP;
    SFMAP* m_mSF;
    SFMAP* m_mEVeto;
    SFMAP* m_mEVeto_3P;
    bool m_bUseHighPtUncert;
    std::string m_sIDpt;
    bool m_bUseInclusiveEta;
    bool m_debug;
    ClassDef(TauSF, 1)

    template<class T>
    void ReadInputs(TFile* f, std::map<std::string, T>* map)
    {
      if(!f || !f->IsOpen())
	{
	  Fatal("ReadInputs", "Could not open %s", f->GetName());
	  return;
	}
      assert(f->IsOpen());
      TKey *key;
      TIter next(f->GetListOfKeys());
      while ((key = (TKey*)next()))
	{
	  TClass *cl = gROOT->GetClass(key->GetClassName());
	  if (!cl->InheritsFrom("TH1"))
	    continue;
	  T obj = (T)key->ReadObj();
	  obj->SetDirectory(0);
	  (*map)[std::string(obj->GetName())] = obj;
	}
      Info("ReadInputs", "data loaded from %s", f->GetName());
    }
  };
} // namespace TauCorrUncert

#endif // TAUSF_H
*/
