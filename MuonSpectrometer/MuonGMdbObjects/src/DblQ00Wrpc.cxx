/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wrpc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

  DblQ00Wrpc::DblQ00Wrpc(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode) :
    m_nObj(0) {
  IRDBRecordset_ptr wrpc = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);
  if(wrpc->size()>0) {
    m_nObj = wrpc->size();
    m_d = new WRPC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wrpc banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<wrpc->size()) {
        m_d[i].version     = (*wrpc)[i]->getInt("VERS");    
        m_d[i].nvrs        = (*wrpc)[i]->getInt("NVRS");
        m_d[i].layrpc      = (*wrpc)[i]->getInt("LAYRPC");
        m_d[i].tckrla      = (*wrpc)[i]->getFloat("TCKRLA");
        m_d[i].tottck      = (*wrpc)[i]->getFloat("TOTTCK");
        m_d[i].tckfsp      = (*wrpc)[i]->getFloat("TCKFSP");
        m_d[i].ackfsp      = (*wrpc)[i]->getFloat("ACKFSP");
        m_d[i].tlohcb      = (*wrpc)[i]->getFloat("TLOHCB");
        m_d[i].alohcb      = (*wrpc)[i]->getFloat("ALOHCB");
        m_d[i].tckbak      = (*wrpc)[i]->getFloat("TCKBAK");
        m_d[i].tckgas      = (*wrpc)[i]->getFloat("TCKGAS");
        m_d[i].tckssu      = (*wrpc)[i]->getFloat("TCKSSU");
        m_d[i].tckstr      = (*wrpc)[i]->getFloat("TCKSTR");
        m_d[i].sdedmi      = (*wrpc)[i]->getFloat("SDEDMI");
        m_d[i].zdedmi      = (*wrpc)[i]->getFloat("ZDEDMI");
        m_d[i].spdiam      = (*wrpc)[i]->getFloat("SPDIAM");
        m_d[i].sppitc      = (*wrpc)[i]->getFloat("SPPITC");
        m_d[i].stroff[0]   = (*wrpc)[i]->getFloat("STROFF_0");
        m_d[i].stroff[1]   = (*wrpc)[i]->getFloat("STROFF_1");
        m_d[i].stroff[2]   = (*wrpc)[i]->getFloat("STROFF_2");
        i++;
    }
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
  if (pAmdcDbRecord == nullptr){
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
  for( ; it<pIRDBRecordset->end(); ++it){
     pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
     if(pAmdcDbRecord == nullptr){
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
