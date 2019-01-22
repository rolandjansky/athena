/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TheLArHV2Ntuple.h
//

#ifndef LARCONDUTILS_LARHV2NTUPLE_H
#define LARCONDUTILS_LARHV2NTUPLE_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class CaloCell_ID;
class LArOnlineID;
class LArCablingService;
class Identifier;
class HWIdentifier;
class CaloDetDescrManager;

class LArHV2Ntuple : public AthAlgorithm {
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
  ITHistSvc* m_thistSvc;
  TTree* m_tree;

  bool m_addcells;

  // tree variables
   int m_bec;
   int m_isPresampler;
   float m_eta;
   float m_phi;
   int m_electrode;
   int m_gap;
   int m_hvline;
   float m_hv;
   float m_current;

   int m_barrelec;
   int m_posneg;
   int m_FT;
   int m_slot;
   int m_channel;

  // ID helper
  const CaloCell_ID* m_caloId; 
  const LArOnlineID* m_onlId;
  ToolHandle<LArCablingService> m_cabling;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;
  std::map<int, std::vector<HWIdentifier> >m_hvonlId_map;

  std::vector<int> GetHVLines(const Identifier& id) ;
};
#endif
