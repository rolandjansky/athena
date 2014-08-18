/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTRIGGERTOOL_LARTTCELLMAP_H
#define CALOTRIGGERTOOL_LARTTCELLMAP_H

#include "Identifier/Identifier.h" 
#include "CaloTriggerTool/LArTTCell.h" 
#include "CaloTriggerTool/LArTTCell_P.h" 
#include "CaloIdentifier/LArID_Exception.h" 

#include <vector>
#include <map>

class MsgStream ; 

/**
   @brief This is a StoreGate object holding the offline TT-cell map

   Its persistifiable version is the LArTTCellMap_P class

   @author Hong Ma
   @author maintained by F. Ledroit
 */

class LArTTCellMap
{
public:

  typedef LArTTCell PDATA; 

  /* Constructor
  **/ 
  LArTTCellMap( )  ; 

  /* Destructor
  **/ 
  ~LArTTCellMap()  ;

  /**  initialize from Nova */
  void set(  const PDATA& t ) ; 

  /**
     Return a vector of offline Identifiers (corresponding helpers =
     LArEM_ID, LArHEC_ID, LArFCAL_ID) for a TT-layer offline id (helper=CaloLVL1_ID)
  */
  const std::vector<Identifier>& createCellIDvec(const Identifier& id) const ; 

  /**
     return the offline id (CaloLVL1_ID) of the TT to which a cell (CaloCell_ID) belongs
  */
  Identifier whichTTID(const Identifier & id) const; 

  /** return the persistified map */
  LArTTCell_P* getP() ; 
 
  /**  initialize from POOL */
  void set(  const LArTTCell_P& t ) ; 

private: 

  void convert_to_P(const LArTTCell &t) ; // from NOVA to POOL
  void convert_to_D(const LArTTCell_P &t, LArTTCell& d) ; // POOL to NOVA

  /* Offline TT Identifier to Offline cell Identifier mapping */ 
  std::map<Identifier,Identifier>  m_cell2ttIdMap;
  std::map<Identifier,std::vector<Identifier> >  m_tt2cellIdMap ;

  //  std::map<HWIdentifier,Identifier>  m_on2offIdMap;

  LArTTCell_P m_persData; 

  MsgStream* m_msg ; 
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( LArTTCellMap,174159653,1)

#endif 
