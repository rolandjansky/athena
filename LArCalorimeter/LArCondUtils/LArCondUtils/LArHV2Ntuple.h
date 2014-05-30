/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TheLArHV2Ntuple.h
//

#ifndef _LArCondUtils_LArHV2Ntuple_H
#define _LArCondUtils_LArHV2Ntuple_H

#include <string>

// Gaudi includes

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"


class LArHV2Ntuple : public Algorithm {
  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    LArHV2Ntuple(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~LArHV2Ntuple();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize(){return StatusCode::SUCCESS;}
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  /** The StoreGate Service */
  StoreGateSvc* m_sgSvc;
  StoreGateSvc* m_detStore;

  ITHistSvc* m_thistSvc;
  TTree* m_tree;

   int m_bec;
   int m_isPresampler;
   float m_eta;
   float m_phi;
   int m_electrode;
   int m_gap;
   int m_hvline;
   float m_hv;
   float m_current;

};
#endif
