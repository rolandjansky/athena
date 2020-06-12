/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wded.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

DblQ00Wded::DblQ00Wded(std::unique_ptr<IRDBQuery>&& wded) :
    m_nObj(0) {
  if(wded) {
    wded->execute();
    m_nObj = wded->size();
    m_d = new WDED[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wded banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wded->next()) {
        m_d[i].version     = wded->data<int>("WDED_DATA.VERS");    
        m_d[i].jsta        = wded->data<int>("WDED_DATA.JSTA");
        m_d[i].nb        = wded->data<int>("WDED_DATA.NB");
        m_d[i].x0          = wded->data<float>("WDED_DATA.X0");
        m_d[i].auphcb      = wded->data<float>("WDED_DATA.AUPHCB");
        m_d[i].tckded      = wded->data<float>("WDED_DATA.TCKDED");
        i++;
    }
    wded->finalize();
  }
  else {
    m_d = new WDED[0];
    std::cerr<<"NO Wded banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wded::DblQ00Wded(AmdcDb* wded) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wded->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WDED[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wded banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].auphcb = (*it)->getFloat("AUPHCB");
     m_d[i].tckded = (*it)->getFloat("TCKDED");
  }
}

DblQ00Wded::~DblQ00Wded()
{
    delete [] m_d;
}

} // end of namespace MuonGM
