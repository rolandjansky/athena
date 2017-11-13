/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/CaloCellGroup2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "StoreGate/StoreGateSvc.h"

CaloCellGroup2Ntuple::CaloCellGroup2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): 
  LArCond2NtupleBase(name, pSvcLocator) { 
  declareProperty("group",m_groupInit);

  m_ntTitle="CaloCellGroup";
  m_ntpath="/NTUPLES/FILE1/CaloCellGroup";
   

}

CaloCellGroup2Ntuple::~CaloCellGroup2Ntuple() 
{}

StatusCode CaloCellGroup2Ntuple::stop () {
  StatusCode sc;

  NTuple::Array<float> values;
  NTuple::Item<long> nValues;

  sc=m_nt->addItem("nvalues",nValues,0,1000);
  if (sc!=StatusCode::SUCCESS)
    {ATH_MSG_ERROR( "addItem 'nvalues' failed" );
    return StatusCode::FAILURE;
    }
 
  sc=m_nt->addItem("values",nValues,values);
  if (sc!=StatusCode::SUCCESS)
    {ATH_MSG_ERROR( "addItem 'values' failed" );
    return StatusCode::FAILURE;
    }

  


  const CaloCell_ID* caloCellId;
  sc = m_detStore->retrieve(caloCellId,"CaloCell_ID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get CaloCellID helper !" );
    return StatusCode::FAILURE;
  }
  
  bool stat=m_cellGroupList.setDefinition(caloCellId,m_groupInit,msg());
  if (!stat) {
    ATH_MSG_ERROR( "CaloCellGroupList::setDefinition failed!" );
    return StatusCode::FAILURE;
  }
  m_cellGroupList.printDef();




 std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
 std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
 for(; itOnId!=itOnIdEnd;++itOnId){
   const HWIdentifier hwid = *itOnId;
   if (m_larCablingSvc->isOnlineConnected(hwid)) {
     fillFromIdentifier(hwid);
     Identifier id=m_larCablingSvc->cnvToIdentifier(hwid);
     const std::vector<float>& v=m_cellGroupList.valuesForCell(id);
     nValues=v.size();
     for (size_t i=0;i<v.size();i++) 
       values[i]=v[i];
     //cellCounter++;
     sc=ntupleSvc()->writeRecord(m_nt);
     if (sc!=StatusCode::SUCCESS) {
       ATH_MSG_ERROR( "writeRecord failed" );
       return StatusCode::FAILURE;
     }
   }//end if isConnected
 }//end loop over online ID

 ATH_MSG_INFO( "CaloCellGroup2Ntuple has finished." );
 return StatusCode::SUCCESS;
}// end finalize-method.
