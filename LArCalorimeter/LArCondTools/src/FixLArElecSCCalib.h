/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FIXLARELECSCCALIB_H
#define FIXLARELECSCCALIB_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "Identifier/HWIdentifier.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArCalibLineMapping.h"
#include <string>

class  LArEM_ID ;
class  LArHEC_ID;
class  LArFCAL_ID;
class  LArOnlineID_Base;
class  LArOnlineID;
class  LArOnline_SuperCellID;
class  CaloCell_SuperCell_ID;

#include "LArRawConditions/LArRampComplete.h"
#include "LArRawConditions/LArPedestalComplete.h"

// class to collect fixes, needed for SuperCells conditions
// mainly to record, what was done, and as example, if other problems will need a fix
class FixLArElecSCCalib : public AthAlgorithm
{
 public:
  FixLArElecSCCalib(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~FixLArElecSCCalib();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;}  
  virtual StatusCode stop() override;  

  void print (const HWIdentifier& hwid, const LArOnlineID_Base* onlineID, const CaloCell_Base_ID* caloCellID, 
              const Identifier *id = nullptr, std::vector<HWIdentifier>* calibIDs=nullptr,std::ostream& out=std::cout);

  StatusCode fix1();  
  StatusCode fix2(const LArOnOffIdMapping *cabling, const LArCalibLineMapping *cl);  

 private: 

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKeySC{this,"SCCablingKey","LArOnOffIdMapSC","SG Key of SC LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<LArCalibLineMapping>  m_CLKeySC{this, "SCCalibLineKey", "LArCalibLineMapSC", "SG calib line key"};


  int  m_fixFlag ; 
  double m_fixFactor;

  std::string m_infile; 

  const LArEM_ID* m_em_idhelper;
  const LArHEC_ID* m_hec_idhelper;
  const LArFCAL_ID* m_fcal_idhelper;
  const LArOnlineID* m_online_idhelper;
  const LArEM_SuperCell_ID* m_sem_idhelper;
  const LArHEC_SuperCell_ID* m_shec_idhelper;
  const LArFCAL_SuperCell_ID* m_sfcal_idhelper;
  const LArOnline_SuperCellID* m_sonline_idhelper;
  const CaloCell_SuperCell_ID* m_scell_idhelper;
};

#endif // FixLArElecSCCalib
