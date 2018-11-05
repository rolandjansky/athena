/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLMASKINGTOOL_H
#define LARCELLREC_LARCELLMASKINGTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include <bitset>


class LArCellMaskingTool: public AthAlgTool,
			  virtual public ICaloCellMakerTool 

{
 
public:    
  
  LArCellMaskingTool(const std::string& type, 
		     const std::string& name, 
		     const IInterface* parent) ;


  virtual StatusCode initialize() ; 

  // update theCellContainer
  virtual StatusCode process( CaloCellContainer * theCellContainer) ;

  virtual StatusCode finalize();

 private:
  StatusCode fillIncludedCellsMap(const LArOnOffIdMapping* cabling);
  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_offlineID;
 SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  
  std::vector<std::string> m_rejLArChannels;  
  std::bitset<200000> m_includedCellsMap; //Slightly too big but who cares....

  bool m_mapInitialized=false;
  IdentifierHash m_offlinehashMax;
  IdentifierHash m_onlinehashMax;
};

#endif

