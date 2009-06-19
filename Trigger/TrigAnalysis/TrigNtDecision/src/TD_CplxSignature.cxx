/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//#include <stdlib.h>
//#include "Riostream.h"
//#include "TROOT.h"
//#include "TClass.h"
#include "TObject.h"
#include "TD_CplxSignature.h"
#include "TString.h"
#include <math.h>


ClassImp(TD_CplxSignature)

/* Default Constructor */
TD_CplxSignature::TD_CplxSignature():TObject() {
    name = TString("");
    type = TString("");
    level = TString("");

    level = "";
    xsec = 0.0;
    lumi = 0.0;
    num_evts = 0;
    num_passed = 0.0;
    pnum_passed = 0.0;
    pnum_unique = 0.0;
    do_unique = false;
    sigs_bs.Clear();

    num_passed_veto = 0.0;
    pnum_passed_veto = 0.0;
    pnum_unique_veto = 0.0;

    veto_isPassed = true;

    colls_per_event = 1;
    L1_Parent = TString("");
    L2_Parent = TString("");
}

/* Normal Constructor */
TD_CplxSignature::TD_CplxSignature(TString _name, TString _type, TString _level):TObject() {
    name = _name;
    type = _type;
    level = _level;

    xsec = 0.0;
    lumi = 0.0;
    num_evts = 0;
    num_passed = 0.0;
    pnum_passed = 0.0;
    pnum_unique = 0.0;
    do_unique = false;
    sigs_bs.Clear();

    num_passed_veto = 0.0;
    pnum_passed_veto = 0.0;
    pnum_unique_veto = 0.0;

    veto_isPassed = true;

    colls_per_event = 1;
    L1_Parent = "";
    L2_Parent = "";
}

/* Detructor */
TD_CplxSignature::~TD_CplxSignature() {

}

/***********/
/* Getters */
/***********/

TString* TD_CplxSignature::getName(){
    return &name;
}

TString* TD_CplxSignature::getL1Parent(){
    return &L1_Parent;
}

TString* TD_CplxSignature::getL2Parent(){
    return &L2_Parent;
}

TString* TD_CplxSignature::getType(){
    return &type;
}

TString* TD_CplxSignature::getLevel(){
    return &level;
}

double TD_CplxSignature::getNColls(){
    return colls_per_event;
}

double TD_CplxSignature::getXsec(){
    return xsec;
}

double TD_CplxSignature::getLumi(){
    return lumi;
}

double TD_CplxSignature::getPrescale(){
    if (pnum_passed > 0) {
        return num_passed/pnum_passed;
    }

    return 0.0;
}

int TD_CplxSignature::getNumEvts(){
    return num_evts;
}

double TD_CplxSignature::getEff(){
    return num_passed/((double)num_evts);
}

double TD_CplxSignature::getEffErr(){
    return sqrt(fabs((getEff()*(1-getEff()))/num_evts));
}

double TD_CplxSignature::getPeff(){
    if (num_passed > 0) {
        return getEff()*(pnum_passed/num_passed);
    }
    return 0.0;
}

double TD_CplxSignature::getRate(){
    return getPeff()*lumi*xsec/colls_per_event;
}

double TD_CplxSignature::getRateErr(){
    if (getEff() > 0) {
        return (getRate()/getEff())*sqrt(fabs((getEff()*(1-getEff()))/num_evts));
    }
    return 0.0;
}

double TD_CplxSignature::getNumPassed(){
    return num_passed;
}

double TD_CplxSignature::getPnumPassed(){
    return pnum_passed;
}

double TD_CplxSignature::getPnumUnique(){
    return pnum_unique;
}

double TD_CplxSignature::getUniqueRate(){
    if (num_evts > 0) {
    	return (getPnumUnique()/(num_evts))*lumi*xsec/colls_per_event;
    }
    return 0.0;
}

// ----- Begin VETO Section -----

double TD_CplxSignature::getEffWV(){
    return num_passed_veto/((double)num_evts);
}

double TD_CplxSignature::getEffErrWV(){
    return sqrt(fabs((getEffWV()*(1-getEffWV()))/num_evts));
}

double TD_CplxSignature::getPeffWV(){
    if (num_passed_veto > 0) {
        return getEffWV()*(pnum_passed_veto/num_passed_veto);
    }
    return 0.0;
}

double TD_CplxSignature::getRateWV(){
    return getPeffWV()*lumi*xsec/colls_per_event;
}

double TD_CplxSignature::getRateErrWV(){
    if (getEffWV() > 0) {
        return (getRateWV()/getEffWV())*sqrt(fabs((getEffWV()*(1-getEffWV()))/num_evts));
    }
    return 0.0;
}

double TD_CplxSignature::getNumPassedWV(){
    return num_passed_veto;
}

double TD_CplxSignature::getPnumPassedWV(){
    return pnum_passed_veto;
}

double TD_CplxSignature::getPnumUniqueWV(){
    return pnum_unique_veto;
}

double TD_CplxSignature::getUniqueRateWV(){
    if (num_evts > 0) {
    	return (getPnumUniqueWV()/(num_evts))*lumi*xsec/colls_per_event;
    }
    return 0.0;
}

double TD_CplxSignature::getOverlapRateWVWith(TString sig){
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

double TD_CplxSignature::getOverlapPercentWVWith(TString sig){
    int size_temp = overlap_veto.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }
    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return (overlap_veto.at(count))/(pnum_passed_veto);
        }
    }
    return 0.0;
}

double TD_CplxSignature::getOverlapNumWVWith(TString sig){
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

double TD_CplxSignature::getOverlapRateWVAt(int pos){
    if (num_evts > 0) {
        return ((overlap_veto.at(pos))/((double)num_evts))*xsec*lumi/colls_per_event;
    }
         
    return 0.0;
}

double TD_CplxSignature::getOverlapPercentWVAt(int pos){
    if (pnum_passed_veto > 0) {
        return (overlap_veto.at(pos))/(pnum_passed_veto);
    }
         
    return 0.0;
}

double TD_CplxSignature::getOverlapNumWVAt(int pos){
    return overlap_veto.at(pos);
}


// ----- End VETO Section -----

int TD_CplxSignature::getSigIndexAt(int pos){
   return (int)(sig_indices.at(pos));
}

double TD_CplxSignature::getSigPrescaleAt(int pos){
   return (int)(sig_prescales.at(pos));
}

TString* TD_CplxSignature::getSigAt(int pos){
   return &sig_names.at(pos);
}

double TD_CplxSignature::getOverlapRateWith(TString sig){
    int size_temp = overlap.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }
    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return ((overlap.at(count))/((double)num_evts))*xsec*lumi/colls_per_event;
        }
    }
    return 0.0;
}

double TD_CplxSignature::getOverlapPercentWith(TString sig){
    int size_temp = overlap.size();
    if ((int)(overlap_names.size()) != size_temp) {
        return 0.0;
    }
    for (int count = 0; count < size_temp; count++) {
        if ((overlap_names.at(count) == sig) && (num_evts > 0)) {
            return (overlap.at(count))/(pnum_passed);
        }
    }
    return 0.0;
}

double TD_CplxSignature::getOverlapNumWith(TString sig){
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

double TD_CplxSignature::getOverlapRateAt(int pos){
    if (num_evts > 0) {
        return ((overlap.at(pos))/((double)num_evts))*xsec*lumi/colls_per_event;
    }
         
    return 0.0;
}

double TD_CplxSignature::getOverlapPercentAt(int pos){
    if (num_evts > 0) {
        return (overlap.at(pos))/(pnum_passed);
    }
         
    return 0.0;
}

double TD_CplxSignature::getOverlapNumAt(int pos){
    return overlap.at(pos);
}

TString* TD_CplxSignature::getOverlapNameAt(int pos){
    return &overlap_names.at(pos);
}

int TD_CplxSignature::getSigIndicesSize(){
    return sig_indices.size();
}

int TD_CplxSignature::getSigNamesSize(){
    return sig_names.size();
}

int TD_CplxSignature::getOverlapNumSize(){
    return overlap.size();
}

int TD_CplxSignature::getOverlapNamesSize(){
    return overlap_names.size();
}

bool TD_CplxSignature::getDoUnique(){
    return do_unique;
}

TBits* TD_CplxSignature::getSigsBitSet() {
    return &sigs_bs;
}

/***********/
/* Setters */
/***********/

void TD_CplxSignature::setName(TString new_name){
    name = new_name;
}

void TD_CplxSignature::setL1Parent(TString new_Parent){
    L1_Parent = new_Parent;
}

void TD_CplxSignature::setL2Parent(TString new_Parent){
    L2_Parent = new_Parent;
}

void TD_CplxSignature::setType(TString new_type){
    type = new_type;
}

void TD_CplxSignature::setLevel(TString new_level){
    level = new_level;
}

void TD_CplxSignature::setNColls(double new_NColls){
    colls_per_event = new_NColls;
}

void TD_CplxSignature::setXsec(double new_xsec){
    xsec = new_xsec;
}

void TD_CplxSignature::setLumi(double new_lumi){
        lumi = new_lumi;
}

void TD_CplxSignature::setNumEvts(int new_num_evts){
    num_evts = new_num_evts;
}

void TD_CplxSignature::setNumPassed(double new_num_passed){
    num_passed = new_num_passed;
}

void TD_CplxSignature::setPnumPassed(double new_pnum_passed){
    pnum_passed = new_pnum_passed;
}

void TD_CplxSignature::setPnumUnique(double new_pnum_unique){
    pnum_unique = new_pnum_unique;
}

void TD_CplxSignature::setNumPassedWV(double new_num_passed){
    num_passed_veto = new_num_passed;
}

void TD_CplxSignature::setPnumPassedWV(double new_pnum_passed){
    pnum_passed_veto = new_pnum_passed;
}

void TD_CplxSignature::setPnumUniqueWV(double new_pnum_unique){
    pnum_unique_veto = new_pnum_unique;
}

void TD_CplxSignature::addSig(TString sig, int index, double ps){
    sig_indices.push_back(index);
    sig_names.push_back(sig);
    sig_prescales.push_back(ps);
    sigs_bs.SetBitNumber(index);
}

void TD_CplxSignature::addOverlap(TString sig){
    overlap.push_back(0.0);
    overlap_veto.push_back(0.0);
    overlap_names.push_back(sig);
}

void TD_CplxSignature::setOverlapNum(int pos, double value){
    overlap[pos] = value;
}

void TD_CplxSignature::setOverlapNumWV(int pos, double value){
    overlap_veto[pos] = value;
}

void TD_CplxSignature::setDoUnique(bool setting){
    do_unique = setting;
    if (setting) {
        fillSigsBitSet();
    }
}

void TD_CplxSignature::fillSigsBitSet(){
    for (int i = 0; i < (int)sig_indices.size(); i++) {
        sigs_bs.SetBitNumber((int)sig_indices.at(i),1);
    }
}

void TD_CplxSignature::setVetoIsPassed(bool veto) {
    veto_isPassed = veto;
}

/****************/
/* Incrementers */
/****************/

void TD_CplxSignature::incNumEvts(int inc){
    num_evts += inc;
}

void TD_CplxSignature::incNumPassed(double inc){
    num_passed += inc;
    if (veto_isPassed) {
        num_passed_veto += inc;
    }
}

void TD_CplxSignature::incPnumPassed(double inc){
    pnum_passed += inc;
    if (veto_isPassed) {
        pnum_passed_veto += inc;
    }
}

void TD_CplxSignature::incPnumUnique(double inc){
    pnum_unique += inc;
    if (veto_isPassed) {
        pnum_unique_veto += inc;
    }
}

void TD_CplxSignature::incOverlapNum(int pos, double inc){
    overlap[pos] = overlap.at(pos)+inc;
    if (veto_isPassed) {
        overlap_veto[pos] = overlap_veto.at(pos)+inc;
    }
}

/************/
/* Deleters */
/************/

void TD_CplxSignature::clearSigs(){
    sig_indices.clear();
    sig_names.clear();
}
void TD_CplxSignature::clearOverlaps(){
    overlap.clear();
    overlap_veto.clear();
    overlap_names.clear();
}
