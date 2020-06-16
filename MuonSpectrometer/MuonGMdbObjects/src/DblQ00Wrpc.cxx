/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wrpc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

DblQ00Wrpc::DblQ00Wrpc(std::unique_ptr<IRDBQuery>&& wrpc) :
    m_nObj(0) {
  if(wrpc) {
    wrpc->execute();
    m_nObj = wrpc->size();
    m_d = new WRPC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wrpc banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wrpc->next()) {
        m_d[i].version     = wrpc->data<int>("WRPC_DATA.VERS");    
        m_d[i].nvrs        = wrpc->data<int>("WRPC_DATA.NVRS");
        m_d[i].layrpc      = wrpc->data<int>("WRPC_DATA.LAYRPC");
        m_d[i].tckrla      = wrpc->data<float>("WRPC_DATA.TCKRLA");
        m_d[i].tottck      = wrpc->data<float>("WRPC_DATA.TOTTCK");
        m_d[i].tckfsp      = wrpc->data<float>("WRPC_DATA.TCKFSP");
        m_d[i].ackfsp      = wrpc->data<float>("WRPC_DATA.ACKFSP");
        m_d[i].tlohcb      = wrpc->data<float>("WRPC_DATA.TLOHCB");
        m_d[i].alohcb      = wrpc->data<float>("WRPC_DATA.ALOHCB");
        m_d[i].tckbak      = wrpc->data<float>("WRPC_DATA.TCKBAK");
        m_d[i].tckgas      = wrpc->data<float>("WRPC_DATA.TCKGAS");
        m_d[i].tckssu      = wrpc->data<float>("WRPC_DATA.TCKSSU");
        m_d[i].tckstr      = wrpc->data<float>("WRPC_DATA.TCKSTR");
        m_d[i].sdedmi      = wrpc->data<float>("WRPC_DATA.SDEDMI");
        m_d[i].zdedmi      = wrpc->data<float>("WRPC_DATA.ZDEDMI");
        m_d[i].spdiam      = wrpc->data<float>("WRPC_DATA.SPDIAM");
        m_d[i].sppitc      = wrpc->data<float>("WRPC_DATA.SPPITC");
        m_d[i].stroff[0]   = wrpc->data<float>("WRPC_DATA.STROFF_0");
        m_d[i].stroff[1]   = wrpc->data<float>("WRPC_DATA.STROFF_1");
        m_d[i].stroff[2]   = wrpc->data<float>("WRPC_DATA.STROFF_2");
        i++;
    }
    wrpc->finalize();
  }
  else {
    m_d = new WRPC[0];
    std::cerr<<"NO Wrpc banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wrpc::DblQ00Wrpc(AmdcDb* wrpc) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wrpc->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WRPC[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wrpc banks in the AmdcDbRecord"<<std::endl;

  const AmdcDbRecord* pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
  if (pAmdcDbRecord == 0){
    std::cerr << "No way to cast in AmdcDbRecord for " << getObjName() << std::endl;
    return;
  }
  
  std::vector< std::string> VariableList = pAmdcDbRecord->getVariableList();
  int ItemTot = VariableList.size() ;
  for(int Item=0 ; Item<ItemTot ; Item++){
    std::string DbVar = VariableList[Item];
  }

  int i = -1;
  it = pIRDBRecordset->begin();
  for( ; it<pIRDBRecordset->end(); it++){
     pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
     if(pAmdcDbRecord == 0){
       std::cerr << "No way to cast in AmdcDbRecord for " << getObjName() << std::endl;
       return;
     }

     i = i + 1;

     m_d[i].version = (*it)->getInt("VERS");    
     m_d[i].nvrs = (*it)->getInt("NVRS");
     m_d[i].layrpc = (*it)->getInt("LAYRPC");
     m_d[i].tckrla = (*it)->getFloat("TCKRLA");
     m_d[i].tottck = (*it)->getFloat("TOTTCK");
     m_d[i].tckfsp = (*it)->getFloat("TCKFSP");
     m_d[i].ackfsp = (*it)->getFloat("ACKFSP");
     m_d[i].tlohcb = (*it)->getFloat("TLOHCB");
     m_d[i].alohcb = (*it)->getFloat("ALOHCB");
     m_d[i].tckbak = (*it)->getFloat("TCKBAK");
     m_d[i].tckgas = (*it)->getFloat("TCKGAS");
     m_d[i].tckssu = (*it)->getFloat("TCKSSU");
     m_d[i].tckstr = (*it)->getFloat("TCKSTR");
     m_d[i].sdedmi = (*it)->getFloat("SDEDMI");
     m_d[i].zdedmi = (*it)->getFloat("ZDEDMI");
     m_d[i].spdiam = (*it)->getFloat("SPDIAM");
     m_d[i].sppitc = (*it)->getFloat("SPPITC");
     m_d[i].stroff[0] = (*it)->getFloat("STROFF_0");
     m_d[i].stroff[1] = (*it)->getFloat("STROFF_1");
     m_d[i].stroff[2] = (*it)->getFloat("STROFF_2");
  }
}

DblQ00Wrpc::~DblQ00Wrpc()
{
    delete [] m_d;
}

} // end of namespace MuonGM
