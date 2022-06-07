/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Atln.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <string> 
#include <iostream>
#include <stdio.h>

namespace MuonGM
{

  DblQ00Atln::DblQ00Atln(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr atln = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(atln->size()>0) {
    m_nObj = atln->size();
    m_d = new ATLN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Atln banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<atln->size()) {
        m_d[i].version     = (*atln)[i]->getInt("VERS");    
        m_d[i].i           = (*atln)[i]->getInt("I");          
        m_d[i].icovol      = (*atln)[i]->getInt("ICOVOL");
        m_d[i].zpovol      = (*atln)[i]->getFloat("ZPOVOL");
        m_d[i].widvol      = (*atln)[i]->getFloat("WIDVOL");
        sprintf(m_d[i].namvol,"%s",(*atln)[i]->getString("NAMVOL").c_str());
        m_d[i].jsta        = (*atln)[i]->getInt("JSTA");          
        i++;
    }
  }
  else {
    m_d = new ATLN[0];
    std::cerr<<"NO Atln banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Atln::DblQ00Atln(AmdcDb* atln) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = atln->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new ATLN[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Atln banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].icovol = (*it)->getInt("ICOVOL");
     m_d[i].zpovol = (*it)->getFloat("ZPOVOL");
     m_d[i].widvol = (*it)->getFloat("WIDVOL");
     sprintf(m_d[i].namvol,"%s",(*it)->getString("NAMVOL").c_str());
     m_d[i].jsta = (*it)->getInt("JSTA");
  }
}

DblQ00Atln::~DblQ00Atln()
{
    delete [] m_d;
}

} // end of namespace MuonGM
