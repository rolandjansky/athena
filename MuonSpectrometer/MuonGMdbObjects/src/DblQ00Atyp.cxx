/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Atyp.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdio.h>

namespace MuonGM
{

  DblQ00Atyp::DblQ00Atyp(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode) :
    m_nObj(0) {

  IRDBRecordset_ptr atyp = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

  if(atyp->size()!=0) {
    m_nObj = atyp->size();
    m_d = new ATYP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Atyp banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<atyp->size()) {
        m_d[i].version        = (*atyp)[i]->getInt("VERS");    
        m_d[i].jtyp           = (*atyp)[i]->getInt("JTYP");          
        m_d[i].nsta           = (*atyp)[i]->getInt("NSTA");          
        sprintf(m_d[i].type,"%s",(*atyp)[i]->getString("TYP").c_str());
        i++;
    }
  }
  else {
    m_d = new ATYP[0];
    std::cerr<<"NO Atyp banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Atyp::DblQ00Atyp(AmdcDb* atyp) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = atyp->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new ATYP[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Atyp banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].jtyp = (*it)->getInt("JTYP");
     m_d[i].nsta = (*it)->getInt("NSTA");
     sprintf(m_d[i].type,"%s",(*it)->getString("TYP").c_str());
  }
}

DblQ00Atyp::~DblQ00Atyp()
{
    delete [] m_d;
}

} // end of namespace MuonGM
