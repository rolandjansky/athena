/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Acut.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdexcept>

namespace MuonGM
{

  DblQ00Acut::DblQ00Acut(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr acut = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(acut->size()>0) {
    m_nObj = acut->size();
    m_d = new ACUT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Acut banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<acut->size()) {
	m_d[i].version = (*acut)[i]->getInt("VERS");
	m_d[i].i       = (*acut)[i]->getInt("I");
	m_d[i].icut    = (*acut)[i]->getInt("ICUT");
	m_d[i].n       = (*acut)[i]->getInt("N");
	i++;
    }
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
     m_d[i].i = (*it)->getInt("I");
     m_d[i].icut = (*it)->getInt("ICUT");
     m_d[i].n = (*it)->getInt("N");
  }
}

DblQ00Acut::~DblQ00Acut()
{
    delete [] m_d;
    m_d = nullptr;
}

} // end of namespace MuonGM
