/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Alin.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdio.h>

namespace MuonGM
{

  DblQ00Alin::DblQ00Alin(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr alin = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(alin->size()>0) {
    m_nObj = alin->size();
    m_d = new ALIN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Alin banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<alin->size()) {
        m_d[i].version      = (*alin)[i]->getInt("VERS");    
        m_d[i].dx           = (*alin)[i]->getFloat("DX");          
        m_d[i].dy           = (*alin)[i]->getFloat("DY");       
        m_d[i].i            = (*alin)[i]->getInt("I");   
        m_d[i].width_xs     = (*alin)[i]->getFloat("WIDTH_XS");
        m_d[i].width_xl     = (*alin)[i]->getFloat("WIDTH_XL");  
        m_d[i].length_y     = (*alin)[i]->getFloat("LENGTH_Y");  
        m_d[i].excent       = (*alin)[i]->getFloat("EXCENT");    
        m_d[i].dead1        = (*alin)[i]->getFloat("DEAD1");     
        m_d[i].jtyp         = (*alin)[i]->getInt("JTYP");      
        m_d[i].indx         = (*alin)[i]->getInt("INDX");     
        m_d[i].icut         = (*alin)[i]->getInt("ICUT");      
        i++;
    }
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
