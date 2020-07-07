/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Alin.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdio.h>

namespace MuonGM
{

DblQ00Alin::DblQ00Alin(std::unique_ptr<IRDBQuery>&& alin) :
    m_nObj(0) {
  if(alin) {
    alin->execute();
    m_nObj = alin->size();
    m_d = new ALIN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Alin banks in the MuonDD Database"<<std::endl;

    // field indexes
    unsigned fieldVers(1);
    unsigned fieldDx(2);
    unsigned fieldDy(3);
    unsigned fieldI(4);
    unsigned fieldWidth_xs(5);
    unsigned fieldWidth_xl(6);
    unsigned fieldLength_y(7);
    unsigned fieldExcent(8);
    unsigned fieldDead1(9);
    unsigned fieldJtyp(10);
    unsigned fieldIndx(11);
    unsigned fieldIcut(12);

    int i=0;
    while(alin->next()) {
        m_d[i].version      = alin->data<int>(fieldVers);    
        m_d[i].dx           = alin->data<float>(fieldDx);          
        m_d[i].dy           = alin->data<float>(fieldDy);       
        m_d[i].i            = alin->data<int>(fieldI);   
        m_d[i].width_xs     = alin->data<float>(fieldWidth_xs);
        m_d[i].width_xl     = alin->data<float>(fieldWidth_xl);  
        m_d[i].length_y     = alin->data<float>(fieldLength_y);  
        m_d[i].excent       = alin->data<float>(fieldExcent);    
        m_d[i].dead1        = alin->data<float>(fieldDead1);     
        m_d[i].jtyp         = alin->data<int>(fieldJtyp);      
        m_d[i].indx         = alin->data<int>(fieldIndx);     
        m_d[i].icut         = alin->data<int>(fieldIcut);      
        i++;
    }
    alin->finalize();
  }
  else {
    m_d = new ALIN[0];
    std::cerr<<"NO Alin banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Alin::DblQ00Alin(AmdcDb* alin) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = alin->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new ALIN[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Alin banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].dx = (*it)->getFloat("DX");
     m_d[i].dy = (*it)->getFloat("DY");
     m_d[i].i = (*it)->getInt("I");
     m_d[i].width_xs = (*it)->getFloat("WIDTH_XS");
     m_d[i].width_xl = (*it)->getFloat("WIDTH_XL");
     m_d[i].length_y = (*it)->getFloat("LENGTH_Y");
     m_d[i].excent = (*it)->getFloat("EXCENT");
     m_d[i].dead1 = (*it)->getFloat("DEAD1");
     m_d[i].jtyp = (*it)->getInt("JTYP");
     m_d[i].indx = (*it)->getInt("INDX");
     m_d[i].icut = (*it)->getInt("ICUT");
  }
}

DblQ00Alin::~DblQ00Alin()
{
    delete [] m_d;
}

} // end of namespace MuonGM
