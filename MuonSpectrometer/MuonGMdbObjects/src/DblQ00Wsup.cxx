/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wsup.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

DblQ00Wsup::DblQ00Wsup(std::unique_ptr<IRDBQuery>&& wsup) :
    m_nObj(0) {
  if(wsup) {
    wsup->execute();
    m_nObj = wsup->size();
    m_d = new WSUP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wsup banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wsup->next()) {
        m_d[i].version     = wsup->data<int>("WSUP_DATA.VERS");    
        m_d[i].jsta        = wsup->data<int>("WSUP_DATA.JSTA");
        m_d[i].nxxsup      = wsup->data<int>("WSUP_DATA.NXXSUP");
        m_d[i].nzzsup      = wsup->data<int>("WSUP_DATA.NZZSUP");
        m_d[i].x0          = wsup->data<float>("WSUP_DATA.X0");
        m_d[i].thickn      = wsup->data<float>("WSUP_DATA.THICKN");
        for (unsigned int j=0; j<4; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tagx = "WSUP_DATA.XXSUP_"+tem.str();
            std::string tagy = "WSUP_DATA.ZZSUP_"+tem.str();
            m_d[i].xxsup[j]     = wsup->data<float>(tagx);        
            m_d[i].zzsup[j]     = wsup->data<float>(tagy);        
        }
        i++;
   }
    wsup->finalize();
  }
  else {
    m_d = new WSUP[0];
    std::cerr<<"NO Wsup banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wsup::DblQ00Wsup(AmdcDb* wsup) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wsup->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WSUP[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wsup banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].nxxsup = (*it)->getInt("NXXSUP");
     m_d[i].nzzsup = (*it)->getInt("NZZSUP");
     m_d[i].x0 = (*it)->getFloat("X0");
     m_d[i].thickn = (*it)->getFloat("THICKN");
     for(unsigned int j=0; j<4; j++)
     {
       std::ostringstream tem;
       tem << j;
       std::string tagx = "XXSUP_"+tem.str();
       std::string tagy = "ZZSUP_"+tem.str();
       m_d[i].xxsup[j] = (*it)->getFloat(tagx);        
       m_d[i].zzsup[j] = (*it)->getFloat(tagy);        
     }
  }
}

DblQ00Wsup::~DblQ00Wsup()
{
    delete [] m_d;
}

} // end of namespace MuonGM
