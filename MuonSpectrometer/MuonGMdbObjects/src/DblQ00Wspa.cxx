/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wspa.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

DblQ00Wspa::DblQ00Wspa(std::unique_ptr<IRDBQuery>&& wspa) :
    m_nObj(0) {
  if(wspa) {
    wspa->execute();
    m_nObj = wspa->size();
    m_d = new WSPA[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wspa banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wspa->next()) {
        m_d[i].version     = wspa->data<int>("WSPA_DATA.VERS");    
        m_d[i].jsta        = wspa->data<int>("WSPA_DATA.JSTA");
        m_d[i].nb        = wspa->data<int>("WSPA_DATA.NB");
        m_d[i].x0          = wspa->data<float>("WSPA_DATA.X0");
        m_d[i].tckspa      = wspa->data<float>("WSPA_DATA.TCKSPA");
        i++;
    }
    wspa->finalize();
  }
  else {
    m_d = new WSPA[0];
    std::cerr<<"NO Wspa banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wspa::DblQ00Wspa(AmdcDb* wspa) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wspa->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WSPA[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wspa banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].jsta = (*it)->getInt("JSTA");
     m_d[i].nb = (*it)->getInt("NB");
     m_d[i].x0 = (*it)->getFloat("X0");
     m_d[i].tckspa = (*it)->getFloat("TCKSPA");
  }
}

DblQ00Wspa::~DblQ00Wspa()
{
    delete [] m_d;
}

} // end of namespace MuonGM
