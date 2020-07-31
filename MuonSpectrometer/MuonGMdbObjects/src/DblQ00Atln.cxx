/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Atln.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdio.h>

namespace MuonGM
{

DblQ00Atln::DblQ00Atln(std::unique_ptr<IRDBQuery>&& atln) :
    m_nObj(0) {
  if(atln) {
    atln->execute();
    m_nObj = atln->size();
    m_d = new ATLN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Atln banks in the MuonDD Database"<<std::endl;

     unsigned fieldVers(1);
     unsigned fieldI(2);
     unsigned fieldIcovol(3);
     unsigned fieldZpovol(4);
     unsigned fieldWidvol(5);
     unsigned fieldNamvol(6);
     unsigned fieldJsta(7);

    int i=0;
    while(atln->next()) {
        m_d[i].version     = atln->data<int>(fieldVers);    
        m_d[i].i           = atln->data<int>(fieldI);          
        m_d[i].icovol      = atln->data<int>(fieldIcovol);
        m_d[i].zpovol      = atln->data<float>(fieldZpovol);
        m_d[i].widvol      = atln->data<float>(fieldWidvol);
        sprintf(m_d[i].namvol,"%s",atln->data<std::string>(fieldNamvol).c_str());
        m_d[i].jsta        = atln->data<int>(fieldJsta);          
        i++;
    }
    atln->finalize();
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
