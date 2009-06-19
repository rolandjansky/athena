/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNtDecision_ROOT_TD_CplxSignature
#define TrigNtDecision_ROOT_TD_CplxSignature

#include "TROOT.h"
#include "TSystem.h"
#include "TClass.h"
#include "TObject.h"
#include "TString.h"
#include "TBits.h"
#include <vector>

class TD_CplxSignature : public TObject {

private:
    TString name;
    TString type;
    TString level;
    TString L1_Parent;
    TString L2_Parent;
    double xsec;
    double lumi;
    int num_evts;
    double num_passed;
    double pnum_passed;
    double pnum_unique;
    double colls_per_event;
    std::vector<int>     sig_indices;
    std::vector<double>  sig_prescales;
    std::vector<TString> sig_names;
    std::vector<double>  overlap;
    std::vector<TString> overlap_names;
    bool do_unique;
    TBits sigs_bs;
 
    double num_passed_veto;
    double pnum_passed_veto;
    double pnum_unique_veto;
    std::vector<double> overlap_veto;

    bool veto_isPassed;
public:
    TD_CplxSignature();
    TD_CplxSignature(TString name, TString type, TString level);
    virtual ~TD_CplxSignature();

    TString* getName();
    TString* getL1Parent();
    TString* getL2Parent();
    TString* getType();
    TString* getLevel();
    double getNColls();
    double getXsec();
    double getLumi();
    double getPrescale();
    int getNumEvts();
    double getEff();
    double getEffErr();
    double getPeff();
    double getRate();
    double getRateErr();
    double getNumPassed();
    double getPnumPassed();
    double getPnumUnique();
    double getUniqueRate();
    // ----- Begin VETO Section -----
    double getEffWV();
    double getEffErrWV();
    double getPeffWV();
    double getRateWV();
    double getRateErrWV();
    double getNumPassedWV();
    double getPnumPassedWV();
    double getPnumUniqueWV();
    double getUniqueRateWV();
    double getOverlapRateWVWith(TString sig);
    double getOverlapPercentWVWith(TString sig);
    double getOverlapNumWVWith(TString sig);
    double getOverlapRateWVAt(int pos);
    double getOverlapPercentWVAt(int pos);
    double getOverlapNumWVAt(int pos);
    // ----- End VETO Section -----
    int getSigIndexAt(int pos);
    double getSigPrescaleAt(int pos);
    TString* getSigAt(int pos);
    double getOverlapRateWith(TString sig);
    double getOverlapPercentWith(TString sig);
    double getOverlapNumWith(TString sig);
    double getOverlapRateAt(int pos);
    double getOverlapPercentAt(int pos);
    double getOverlapNumAt(int pos);
    TString* getOverlapNameAt(int pos);
    int getSigIndicesSize();
    int getSigNamesSize();
    int getOverlapNumSize();
    int getOverlapNamesSize();
    bool getDoUnique();
    TBits* getSigsBitSet();
    
    void setName(TString new_name);
    void setL1Parent(TString new_parent);
    void setL2Parent(TString new_parent);
    void setType(TString new_type);
    void setLevel(TString new_level);
    void setNColls(double new_NColls);
    void setXsec(double new_xsec);
    void setLumi(double new_xsec);
    void setNumEvts(int new_num_evts);
    void setNumPassed(double new_num_passed);
    void setPnumPassed(double new_pnum_passed);
    void setPnumUnique(double new_pnum_unique);
    void setNumPassedWV(double new_num_passed);
    void setPnumPassedWV(double new_pnum_passed);
    void setPnumUniqueWV(double new_pnum_unique);
    void addSig(TString sig, int index, double ps);
    void addOverlap(TString sig);
    void setOverlapNum(int pos, double value);
    void setOverlapNumWV(int pos, double value);
    void setDoUnique(bool setting);
    void fillSigsBitSet();
    void setVetoIsPassed(bool veto);
    
    void incNumEvts(int inc);
    void incNumPassed(double inc);
    void incPnumPassed(double inc);
    void incPnumUnique(double inc);
    void incOverlapNum(int pos, double inc);

    void clearSigs();
    void clearOverlaps();
    
    ClassDef(TD_CplxSignature,7)
};

#endif
