/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "CxxUtils/checker_macros.h"
#include <bitset>
#include <mutex>


class LArCellMaskingTool
  : public extends<AthAlgTool, ICaloCellMakerTool >
{
 
public:    
  
  LArCellMaskingTool(const std::string& type, 
		     const std::string& name, 
		     const IInterface* parent) ;


  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

  virtual StatusCode finalize() override;

 private:
  StatusCode fillIncludedCellsMap(const LArOnOffIdMapping* cabling) const;
  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_offlineID;
 SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  
  std::vector<std::string> m_rejLArChannels;  
  mutable std::bitset<200000> m_includedCellsMap ATLAS_THREAD_SAFE; //Slightly too big but who cares....

  mutable std::atomic<bool> m_mapInitialized;
  IdentifierHash m_offlinehashMax;
  IdentifierHash m_onlinehashMax;
  mutable std::mutex m_mutex;
};

#endif

