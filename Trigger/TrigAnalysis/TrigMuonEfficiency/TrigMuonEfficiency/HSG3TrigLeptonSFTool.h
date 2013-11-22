// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HSG3TrigLeptonSFTool.h 523458 2012-10-27 01:18:33Z takashi $
#ifndef TRIGMUONEFFICIENCY_HSG3TrigLeptonSFTool_H
#define TRIGMUONEFFICIENCY_HSG3TrigLeptonSFTool_H

// STL include(s):
#include <utility>
#include <string>

// ROOT include(s):
#include "Rtypes.h"

// Forward declaration(s):
class TLorentzVector;
class LeptonTriggerSF;

/**
 *  @short Short description for the class
 *
 *         Longer description for the class...
 *
 * $Revision: 523458 $
 * $Date: 2012-10-27 03:18:33 +0200 (Sat, 27 Oct 2012) $
 */
class HSG3TrigLeptonSFTool {

public:
   HSG3TrigLeptonSFTool( const std::string& dir, Double_t intLumiJ, Double_t intLumiK,
                         Double_t intLumiL, Double_t intLumiM );
   ~HSG3TrigLeptonSFTool();

   Double_t MuEff_CB_HSG3( const std::string& period, Bool_t isData, const TLorentzVector& muon ) const;
   std::pair< Double_t, Double_t >
   MuEffErr_CB_HSG3( const std::string& period, Bool_t isData, const TLorentzVector& muon ) const;

private:
   LeptonTriggerSF* fLeptonTriggerSF;

   Double_t fIntLumiJ;
   Double_t fIntLumiK;
   Double_t fIntLumiL;
   Double_t fIntLumiM;

}; // class HSG3TrigLeptonSFTool

#endif // TRIGMUONEFFICIENCY_HSG3TrigLeptonSFTool_H
