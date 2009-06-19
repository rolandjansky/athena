/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNtDecision_ROOT_TD_Signature
#define TrigNtDecision_ROOT_TD_Signature

#include "TROOT.h"
#include "TSystem.h"
#include "TClass.h"
#include "TObject.h"
#include "TString.h"
#include <vector>

class TD_Signature : public TObject {

private:
    TString name;
    TString type;
    TString level;
    TString L1_Parent;
    TString L2_Parent;
    double xsec;
    double lumi;
    double prescale;
    double passthrough;
    double colls_per_event;
    int num_evts;
    double num_passed;
    double pnum_unique;
    double indep;
    std::vector<double> overlap;
    std::vector<TString> overlap_names;

    double num_passed_veto;
    double pnum_unique_veto;
    double indep_veto;
    std::vector<double> overlap_veto;
   
    bool veto_isPassed;
public:
    TD_Signature();
    TD_Signature(TString name, TString level, double prescale, double passthrough);
    virtual ~TD_Signature();

    TString* getName();
    TString* getL1Parent();
    TString* getL2Parent();
    TString* getLevel();
    double   getNColls();
    double   getXsec();
    double   getLumi();
    double   getPrescale();
    double   getPassthrough();
    int      getNumEvts();
    double   getEff();
    double   getEffErr();
    double   getPeff();
    double   getRate();
    double   getRateErr();
    double   getIndepFrac();
    double   getUniqueFrac();
    double   getUniqueRate();
    double   getNumPassed();
    double   getPnumUnique();
    double   getIndep();
    double   getOverlapRateWith(TString sig);
    double   getOverlapPercentWith(TString sig);
    double   getOverlapNumWith(TString sig);
    double   getOverlapRateAt(int pos);
    double   getOverlapPercentAt(int pos);
    double   getOverlapNumAt(int pos);
    // ----- Begin VETO Section -----
    double   getEffWV();
    double   getEffErrWV();
    double   getPeffWV();
    double   getRateWV();
    double   getRateErrWV();
    double   getIndepFracWV();
    double   getUniqueFracWV();
    double   getUniqueRateWV();
    double   getNumPassedWV();
    double   getPnumUniqueWV();
    double   getIndepWV();
    double   getOverlapRateWVWith(TString sig);
    double   getOverlapPercentWVWith(TString sig);
    double   getOverlapNumWVWith(TString sig);
    double   getOverlapRateWVAt(int pos);
    double   getOverlapPercentWVAt(int pos);
    double   getOverlapNumWVAt(int pos);
    // ----- End VETO Section -----
    TString* getOverlapNameAt(int pos);
    int getOverlapNumSize();
    int getOverlapNamesSize();
    
    void setName(TString new_name);
    void setL1Parent(TString new_parent);
    void setL2Parent(TString new_parent);
    void setLevel(TString new_level);
    void setNColls(double new_NColls);
    void setXsec(double new_xsec);
    void setLumi(double new_lumi);
    void setPrescale(double new_prescale);
    void setPassthrough(double new_passthrough);
    void setNumEvts(int new_num_evts);
    void setNumPassed(double new_num_passed);
    void setPnumUnique(double new_pnum_unique);
    void setIndep(double new_indep);
    void setNumPassedWV(double new_num_passed);
    void setPnumUniqueWV(double new_pnum_unique);
    void setIndepWV(double new_indep);
    void addOverlap(TString sig);
    void setOverlapNum(int pos, double value);
    void setOverlapNumWV(int pos, double value);
    void setVetoIsPassed(bool veto);
    
    void incNumEvts(int inc);
    void incNumPassed(double inc);
    void incPnumUnique(double inc);
    void incIndep(double inc);
    void incOverlapNum(int pos, double inc);

    void clearOverlaps();
    
    ClassDef(TD_Signature,7);
};

#endif
