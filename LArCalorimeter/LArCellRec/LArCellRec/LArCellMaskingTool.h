/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLMASKINGTOOL_H
#define LARCELLMASKINGTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include <bitset>

class StoreGateSvc; 
class LArCablingService;
class MsgStream;

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
  StatusCode fillIncludedCellsMap();
  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_offlineID;
  LArCablingService* m_larCablingSvc;
  
  std::vector<std::string> m_rejLArChannels;  
  std::bitset<200000> m_includedCellsMap; //Slightly too big but who cares....


  IdentifierHash m_offlinehashMax;
  IdentifierHash m_onlinehashMax;
};

#endif

