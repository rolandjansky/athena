/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/LhoodMMFitInfo.h"
#include "TDirectory.h"
#include "TTree.h"
#include <string.h>
 
using namespace CP;

LhoodMMFitInfo::LhoodMMFitInfo(int nlep) {
  reset();
  resizeVectors(nlep);
}

void LhoodMMFitInfo::reset() {

  totEvents = 0;

  event_cat.clear();
  event_sumw2.clear();
  coeffs_num.clear();

  normterms.clear();

  OSfrac_num.clear();
  OSfrac_denom.clear();

  // start out with vectors large enough to hold info from single-lepton events
  resizeVectors(1);

}

void LhoodMMFitInfo::add(LhoodMMFitInfo& rhs, int nLepMax) {


  totEvents += rhs.totEvents;
   
  for (int ilep = 0; ilep < nLepMax; ilep++) {
    eventCount[ilep] += rhs.eventCount[ilep];
  }

  for (int ilep = 0; ilep < nLepMax; ilep++) {
    for (int jlep = 0; jlep < (0x1 << (ilep+1)); jlep++) {
      for (int klep = 0; klep < (0x1 << (ilep+1)); klep++) {
    	coeffs_num[ilep][jlep][klep].add(rhs.coeffs_num[ilep][jlep][klep]);
      }
    }
    
    for (int ientry = 0; ientry < (0x1 << (ilep+1)); ientry++) {
      event_cat[ilep][ientry] += rhs.event_cat[ilep][ientry];
      event_sumw2[ilep][ientry] += rhs.event_sumw2[ilep][ientry];
    }
    
  }

  for (int ilep = 0; ilep <nLepMax; ilep++) {
    for (int jlep = 0; jlep < pow(2,2*(ilep+1)); jlep++) {
      normterms[ilep][jlep].add(rhs.normterms[ilep][jlep]);
    }  
  }

  for (int ilep = 0; ilep <nLepMax; ilep++) {
    for (int jlep = 0; jlep < ilep; jlep++) {
      OSfrac_num[ilep][jlep]+=rhs.OSfrac_num[ilep][jlep];
      OSfrac_denom[ilep][jlep]+=rhs.OSfrac_denom[ilep][jlep];
    }  
  }

}

void LhoodMMFitInfo::resizeVectors(unsigned nlep) {

  // check that we're not being asked to shrink...
  if (nlep < event_cat.size()) {
    return;
  }

  event_cat.resize(nlep);
  event_sumw2.resize(nlep);
  coeffs_num.resize(nlep);
  eventCount.resize(nlep);

  for (unsigned ilep = 0; ilep < nlep; ilep++) {
    coeffs_num[ilep].resize(pow(2,ilep+1));
    event_cat[ilep].resize(pow(2,ilep+1));
    event_sumw2[ilep].resize(pow(2,ilep+1));
    for (int jlep = 0; jlep < pow(2,ilep+1); jlep++) {
      coeffs_num[ilep][jlep].resize(pow(2,ilep+1));
    }
  }
 
  normterms.resize(nlep);
  for (unsigned ilep = 0; ilep <nlep; ilep++) {
    normterms[ilep].resize(pow(2,2*(ilep+1)));
  }  

  OSfrac_num.resize(nlep);
  OSfrac_denom.resize(nlep);
  for (unsigned ilep = 0; ilep <nlep; ilep++) {
    OSfrac_num[ilep].resize(ilep+2);
    OSfrac_denom[ilep].resize(ilep+2);  
  }

}
