/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef ANALYSISUTILS_ISELECTORCORE_H 
#define ANALYSISUTILS_ISELECTORCORE_H 

/**
   @Project: AnalysisUtils
   
   @class ISelectorCore
   
   @author Amir Farbin <amir.farbin@cernSPAMNOT.ch>
   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
   
   @date June 2009
   
   @brief Dual use tool (athena/ARA) for any cuts. This is the base class.
*/


#include "AraTool/AraToolBase.h"
#include "EventKernel/INavigable4Momentum.h"

static const InterfaceID IID_ISelectorCore("ISelectorCore", 1, 0);

class ISelectorCore : virtual public AraToolBase
{
 public:
  ISelectorCore(PropertyMgr *pmgr=0);

  virtual ~ISelectorCore() {}

  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ISelectorCore; };

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize()
    {
      return StatusCode::SUCCESS;
    }

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize()
    {
      return StatusCode::SUCCESS;
    }

  // This should be pure virtual, but I'm afraid the ARA wrapper will try to instiate it.
  virtual bool accept(const INavigable4Momentum *) const {return false;}    //=0;  

};


#endif // ANALYSISUTILS_ISELECTORCORE_H 
