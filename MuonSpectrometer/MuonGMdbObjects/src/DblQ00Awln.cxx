/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Awln.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

  DblQ00Awln::DblQ00Awln(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr awln = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(awln->size()>0) {
    m_nObj = awln->size();
    m_d = new AWLN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Awln banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<awln->size()) {
        m_d[i].version       = (*awln)[i]->getInt("VERS");    
        m_d[i].jsta          = (*awln)[i]->getInt("JSTA");    
        m_d[i].spitch        = (*awln)[i]->getFloat("SPITCH");  
        m_d[i].zpitch        = (*awln)[i]->getFloat("ZPITCH");  
        m_d[i].dedstr        = (*awln)[i]->getFloat("DEDSTR");  
        m_d[i].nsrest        = (*awln)[i]->getInt("NSREST");  
        m_d[i].nzrest        = (*awln)[i]->getInt("NZREST");
        m_d[i].sfirst        = (*awln)[i]->getFloat("SFIRST");
        m_d[i].zfirst        = (*awln)[i]->getFloat("ZFIRST");
        m_d[i].dedsep        = (*awln)[i]->getFloat("DEDSEP");
        m_d[i].nsrost        = (*awln)[i]->getInt("NSROST");  
        m_d[i].nzrost        = (*awln)[i]->getInt("NZROST");
        i++;
    }
  }
  else {
    m_d = new AWLN[0];
    std::cerr<<"NO Awln banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Awln::DblQ00Awln(AmdcDb* awln) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = awln->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new AWLN[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Aszt banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].jsta = (*it)->getInt("JSTA");    
     m_d[i].spitch = (*it)->getFloat("SPITCH");  
     m_d[i].zpitch = (*it)->getFloat("ZPITCH");  
     m_d[i].dedstr = (*it)->getFloat("DEDSTR");  
     m_d[i].nsrest = (*it)->getInt("NSREST");  
     m_d[i].nzrest = (*it)->getInt("NZREST");
     m_d[i].sfirst = (*it)->getFloat("SFIRST");
     m_d[i].zfirst = (*it)->getFloat("ZFIRST");
     m_d[i].dedsep = (*it)->getFloat("DEDSEP");
     m_d[i].nsrost = (*it)->getInt("NSROST");  
     m_d[i].nzrost = (*it)->getInt("NZROST");
  }
}

DblQ00Awln::~DblQ00Awln()
{
    delete [] m_d;
}

} // end of namespace MuonGM
