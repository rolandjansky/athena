/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __TJVFREWEIGHTING__
#define __TJVFREWEIGHTING__

/**
   @class TJVFReweighting
   @brief Tool to Tool to reweight JVF in WTR control region.

   @author Dan Short

*/

/******************************************************************************
Name:        TJVFReweighting

Author:      Dan Short
Created:     July 2011

Description: Tool to reweight JVF in WTR control region.
******************************************************************************/

#include "TNamed.h"
#include <TFile.h>
#include <TH1F.h>
#include <TString.h>

namespace SUSY {
  class TJVFReweighting : public TNamed {

  public: 
    /** Standard constructor */
    TJVFReweighting(const char* name="TJVFReweighting");
    /** Standard destructor */
    ~TJVFReweighting();
  
  public:
    /** Initialize this class once before the event loop starts */
    int initialize(const TString& weightsFileName, const TString& channel, int nJets);
    
    /** Method to get jet weight and return status code */
    float getJVFWeight(bool passJvf);
    
    // Private members
  private:
    
    /** The weights ROOT file */
    TFile* m_jvfWeightsFile; //! Don't persistify (PROOF)
    
    /** The vector of weight histograms */
    TH1D * m_jvfHist;

    ClassDef(TJVFReweighting,1)

  }; // End: class definition

} // End: namespace SUSY

#endif
