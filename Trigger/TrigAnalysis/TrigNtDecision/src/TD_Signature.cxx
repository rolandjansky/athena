/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <stdlib.h>
//#include "Riostream.h"
//#include "TROOT.h"
//#include "TClass.h"
#include "TObject.h"
#include "TD_Signature.h"
#include "TString.h"
#include <math.h>

ClassImp(TD_Signature)

/* Default Constructor */
TD_Signature::TD_Signature():TObject() {
    name = TString("");
    level = TString("");
    xsec = 0.0;
    lumi = 0.0;
    prescale = 0;
    passthrough = 0;

    num_evts = 0;
    num_passed = 0.0;
    pnum_unique = 0.0;
    indep = 0.0;

    num_passed_veto = 0.0;
    pnum_unique_veto = 0.0;
    indep_veto = 0.0;

    veto_isPassed = true;

    colls_per_event = 1;
    L1_Parent = TString("");
    L2_Parent = TString("");
}

/* Normal Constructor */
TD_Signature::TD_Signature(TString _name, TString _level, double _prescale, double _passthrough):TObject() {
    name = _name;
    level = _level;
    xsec = 0.0;
    lumi = 0.0;
    prescale = _prescale;
    passthrough = _passthrough;

    num_evts = 0;
    num_passed = 0.0;
    pnum_unique = 0.0;
    indep = 0.0;

    num_passed_veto = 0.0;
    pnum_unique_veto = 0.0;
    indep_veto = 0.0;

    veto_isPassed = true;

    colls_per_event = 1;
    L1_Parent = "";
    L2_Parent = "";

}

/* Detructor */
TD_Signature::~TD_Signature() {

}

/***********/
/* Getters */
/***********/

TString* TD_Signature::getName(){
    return &name;
}

TString* TD_Signature::getL1Parent(){
    return &L1_Parent;
}

TString* TD_Signature::getL2Parent(){
    return &L2_Parent;
}

TString* TD_Signature::getLevel(){
    return &level;
}

double TD_Signature::getNColls(){
    return colls_per_event;
}

double TD_Signature::getXsec(){
    return xsec;
}

double TD_Signature::getLumi(){
    return lumi;
}

double TD_Signature::getPrescale(){
    return prescale;
}

double TD_Signature::getPassthrough(){
    return passthrough;
}

int TD_Signature::getNumEvts(){
    return num_evts;
}

double TD_Signature::getEff(){
    return num_passed/((double)num_evts);
}

double TD_Signature::getEffErr(){
    return sqrt(fabs((getEff()*(1-getEff()))/num_evts));
}

double TD_Signature::getPeff(){
    if (prescale > 0) {
        return getEff()/prescale;
    }
    return 0.0;
}

double TD_Signature::getRate(){
    return getPeff()*lumi*xsec/colls_per_event;
}

double TD_Signature::getRateErr(){
    if (getEff() > 0) {
        return (getRate()/getEff())*sqrt(fabs((getEff()*(1-getEff()))/num_evts));
    }
    return 0.0;
}

double TD_Signature::getIndepFrac(){
    if (num_passed > 0) {
        return indep/num_passed;
    }
    return 0.0;
}

double TD_Signature::getUniqueFrac(){
    if (num_passed > 0 && pnum_unique > 0) {
        return (pnum_unique/num_passed)*prescale;
    }
    return 0.0;
}

double TD_Signature::getUniqueRate(){
    if (num_passed > 0 && pnum_unique > 0 && num_evts > 0) {
        return pnum_unique*xsec*lumi/((double)num_evts) /colls_per_event;
    }
    return 0.0;
}

double TD_Signature::getNumPassed(){
    return num_passed;
}

double TD_Signature::getPnumUnique(){
    return pnum_unique;
}

double TD_Signature::getIndep(){
    return indep;
}

double TD_Signature::getOverlapRateWith(TString sig){
    int size_temp = overlap.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }

    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return ((overlap.at(count))/((double)num_evts))*xsec*lumi;
        }
    }

    return 0.0;
}

double TD_Signature::getOverlapPercentWith(TString sig){
    int size_temp = overlap.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }

    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return ((overlap.at(count))/((double)num_passed))*prescale;
        }
    }

    return 0.0;
}

double TD_Signature::getOverlapNumWith(TString sig){
    int size_temp = overlap.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }

    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return overlap.at(count);
        }
    }

    return 0.0;
}

double TD_Signature::getOverlapRateAt(int pos){
    if (num_evts > 0) {
        return ((overlap.at(pos))/((double)num_evts))*xsec*lumi/colls_per_event;
    }

    return 0.0;
}

double TD_Signature::getOverlapPercentAt(int pos){
    if (num_evts > 0) {
        return ((overlap.at(pos))/((double)num_passed))*prescale;
    }

    return 0.0;
}

double TD_Signature::getOverlapNumAt(int pos){
    return overlap.at(pos);
}

// ------ Begin VETO Section -----

double TD_Signature::getEffWV(){
    return num_passed_veto/((double)num_evts);
}

double TD_Signature::getEffErrWV(){
    return sqrt(fabs((getEffWV()*(1-getEffWV()))/num_evts));
}

double TD_Signature::getPeffWV(){
    if (prescale > 0) {
        return getEffWV()/prescale;
    }
    return 0.0;
}

double TD_Signature::getRateWV(){
    return getPeffWV()*lumi*xsec/colls_per_event;
}

double TD_Signature::getRateErrWV(){
    if (getEffWV() > 0) {
        return (getRateWV()/getEffWV())*sqrt(fabs((getEffWV()*(1-getEffWV()))/num_evts));
    }
    return 0.0;
}

double TD_Signature::getIndepFracWV(){
    if (num_passed_veto > 0) {
        return indep_veto/num_passed_veto;
    }
    return 0.0;
}

double TD_Signature::getUniqueFracWV(){
    if (num_passed_veto > 0 && pnum_unique_veto > 0) {
        return (pnum_unique_veto/num_passed_veto)*prescale;
    }
    return 0.0;
}

double TD_Signature::getUniqueRateWV(){
    if (num_passed_veto > 0 && pnum_unique_veto > 0 && num_evts > 0) {
        return pnum_unique_veto*xsec*lumi/((double)num_evts) /colls_per_event;
    }
    return 0.0;
}

double TD_Signature::getNumPassedWV(){
    return num_passed_veto;
}

double TD_Signature::getPnumUniqueWV(){
    return pnum_unique_veto;
}

double TD_Signature::getIndepWV(){
    return indep_veto;
}

double TD_Signature::getOverlapRateWVWith(TString sig){
    int size_temp = overlap_veto.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }

    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return ((overlap_veto.at(count))/((double)num_evts))*xsec*lumi;
        }
    }

    return 0.0;
}

double TD_Signature::getOverlapPercentWVWith(TString sig){
    int size_temp = overlap_veto.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }

    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return ((overlap_veto.at(count))/((double)num_passed_veto))*prescale;
        }
    }

    return 0.0;
}

double TD_Signature::getOverlapNumWVWith(TString sig){
    int size_temp = overlap_veto.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }

    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return overlap_veto.at(count);
        }
    }

    return 0.0;
}

double TD_Signature::getOverlapRateWVAt(int pos){
    if (num_evts > 0) {
        return ((overlap_veto.at(pos))/((double)num_evts))*xsec*lumi/colls_per_event;
    }

    return 0.0;
}

double TD_Signature::getOverlapPercentWVAt(int pos){
    if (num_evts > 0) {
        return ((overlap_veto.at(pos))/((double)num_passed_veto))*prescale;
    }

    return 0.0;
}

double TD_Signature::getOverlapNumWVAt(int pos){
    return overlap_veto.at(pos);
}

// ------ End VETO Section -----

TString* TD_Signature::getOverlapNameAt(int pos){
    return &(overlap_names.at(pos));
}

int TD_Signature::getOverlapNumSize(){
    return overlap.size();
}

int TD_Signature::getOverlapNamesSize(){
    return overlap_names.size();
}


/***********/
/* Setters */
/***********/

void TD_Signature::setName(TString new_name){
    name = new_name;
}

void TD_Signature::setL1Parent(TString new_Parent){
    L1_Parent = new_Parent;
}

void TD_Signature::setL2Parent(TString new_Parent){
    L2_Parent = new_Parent;
}

void TD_Signature::setLevel(TString new_level){
    level = new_level;
}

void TD_Signature::setNColls(double new_NColls){
    colls_per_event = new_NColls;
}

void TD_Signature::setXsec(double new_xsec){
    xsec = new_xsec;
}

void TD_Signature::setLumi(double new_lumi){
    lumi = new_lumi;
}

void TD_Signature::setPrescale(double new_prescale){
    prescale = new_prescale;
}

void TD_Signature::setPassthrough(double new_passthrough){
    passthrough = new_passthrough;
}
    
void TD_Signature::setNumEvts(int new_num_evts){
    num_evts = new_num_evts;
}

void TD_Signature::setNumPassed(double new_num){
    num_passed = new_num;
}

void TD_Signature::setPnumUnique(double new_pnum_unique){
    pnum_unique = new_pnum_unique;
}

void TD_Signature::setIndep(double new_indep){
    indep = new_indep;
}

void TD_Signature::setNumPassedWV(double new_num){
    num_passed_veto = new_num;
}

void TD_Signature::setPnumUniqueWV(double new_pnum_unique){
    pnum_unique_veto = new_pnum_unique;
}

void TD_Signature::setIndepWV(double new_indep){
    indep_veto = new_indep;
}

void TD_Signature::addOverlap(TString sig){
    overlap.push_back(0.0);
    overlap_veto.push_back(0.0);
    overlap_names.push_back(sig);
}

void TD_Signature::setOverlapNum(int pos, double value){
    overlap[pos] = value;
}

void TD_Signature::setOverlapNumWV(int pos, double value){
    overlap_veto[pos] = value;
}

void TD_Signature::setVetoIsPassed(bool veto){
    veto_isPassed = veto;
}

/****************/
/* Incrementers */
/****************/

void TD_Signature::incNumEvts(int inc){
    num_evts += inc;
}

void TD_Signature::incNumPassed(double inc){
    num_passed += inc;
    if (veto_isPassed) {
        num_passed_veto += inc;
    }
}

void TD_Signature::incPnumUnique(double inc){
    pnum_unique += inc;
    if (veto_isPassed) {
        pnum_unique_veto += inc;
    }
}

void TD_Signature::incIndep(double inc){
    indep += inc;
    if (veto_isPassed) {
        indep_veto += inc;
    }
}

void TD_Signature::incOverlapNum(int pos, double inc){
    overlap[pos] = overlap.at(pos)+inc;
    if (veto_isPassed) {
        overlap_veto[pos] = overlap_veto.at(pos)+inc;
    }
}

/************/
/* Deleters */
/************/

void TD_Signature::clearOverlaps() {
    overlap.clear();
    overlap_veto.clear();
    overlap_names.clear();
}

