/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Awln.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

DblQ00Awln::DblQ00Awln(std::unique_ptr<IRDBQuery>&& awln) :
    m_nObj(0) {
  if(awln) {
    awln->execute();
    m_nObj = awln->size();
    m_d = new AWLN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Awln banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(awln->next()) {
        m_d[i].version       = awln->data<int>("AWLN_DATA.VERS");    
        m_d[i].jsta          = awln->data<int>("AWLN_DATA.JSTA");    
        m_d[i].spitch        = awln->data<float>("AWLN_DATA.SPITCH");  
        m_d[i].zpitch        = awln->data<float>("AWLN_DATA.ZPITCH");  
        m_d[i].dedstr        = awln->data<float>("AWLN_DATA.DEDSTR");  
        m_d[i].nsrest        = awln->data<int>("AWLN_DATA.NSREST");  
        m_d[i].nzrest        = awln->data<int>("AWLN_DATA.NZREST");
        m_d[i].sfirst        = awln->data<float>("AWLN_DATA.SFIRST");
        m_d[i].zfirst        = awln->data<float>("AWLN_DATA.ZFIRST");
        m_d[i].dedsep        = awln->data<float>("AWLN_DATA.DEDSEP");
        m_d[i].nsrost        = awln->data<int>("AWLN_DATA.NSROST");  
        m_d[i].nzrost        = awln->data<int>("AWLN_DATA.NZROST");
        i++;
    }
    awln->finalize();
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
