/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Acut.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdexcept>

namespace MuonGM
{

DblQ00Acut::DblQ00Acut(std::unique_ptr<IRDBQuery>&& acut) :
    m_nObj(0) {
  if(acut) {
    acut->execute();
    m_nObj = acut->size();
    m_d = new ACUT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(acut->next()) {
	m_d[i].version = acut->data<int>("ACUT_DATA.VERS");
	m_d[i].i       = acut->data<int>("ACUT_DATA.I");
	m_d[i].icut    = acut->data<int>("ACUT_DATA.ICUT");
	m_d[i].n       = acut->data<int>("ACUT_DATA.N");
	i++;
    }
    acut->finalize();
  }
  else {
    m_d = new ACUT[0];
    std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Acut::DblQ00Acut(AmdcDb* acut) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = acut->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new ACUT[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Acut banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].i = (*it)->getInt("I");
     m_d[i].icut = (*it)->getInt("ICUT");
     m_d[i].n = (*it)->getInt("N");
  }
}

DblQ00Acut::~DblQ00Acut()
{
    delete [] m_d;
    m_d = 0;
}

} // end of namespace MuonGM
