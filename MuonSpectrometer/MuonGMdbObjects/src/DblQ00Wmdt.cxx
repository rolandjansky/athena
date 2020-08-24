/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wmdt.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>
#include <stdio.h>

namespace MuonGM
{

DblQ00Wmdt::DblQ00Wmdt(std::unique_ptr<IRDBQuery>&& wmdt) :
    m_nObj(0) {
  if(wmdt) {
    wmdt->execute();
    m_nObj = wmdt->size();
    m_d = new WMDT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wmdt banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wmdt->next()) {
        m_d[i].version     = wmdt->data<int>("WMDT_DATA.VERS");    
        m_d[i].iw          = wmdt->data<int>("WMDT_DATA.IW");
        m_d[i].laymdt      = wmdt->data<int>("WMDT_DATA.LAYMDT");
        sprintf(m_d[i].typ,"%s",wmdt->data<std::string>("WMDT_DATA.TYP").c_str());
        m_d[i].x0          = wmdt->data<float>("WMDT_DATA.X0");
        m_d[i].tubpit          = wmdt->data<float>("WMDT_DATA.TUBPIT");
        m_d[i].tubrad          = wmdt->data<float>("WMDT_DATA.TUBRAD");
        m_d[i].tubsta          = wmdt->data<float>("WMDT_DATA.TUBSTA");
        m_d[i].tubdea          = wmdt->data<float>("WMDT_DATA.TUBDEA");
        m_d[i].tubwal          = wmdt->data<float>("WMDT_DATA.TUBWAL");
        for (unsigned int j=0; j<4; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tagx = "WMDT_DATA.TUBXCO_"+tem.str();
            std::string tagy = "WMDT_DATA.TUBYCO_"+tem.str();
            m_d[i].tubxco[j]     = wmdt->data<float>(tagx);        
            m_d[i].tubyco[j]     = wmdt->data<float>(tagy);        
        }
        i++;
    }
    wmdt->finalize();
  }
  else {
    m_d = new WMDT[0];
    std::cerr<<"NO Wmdt banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wmdt::DblQ00Wmdt(AmdcDb* wmdt) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wmdt->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WMDT[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wmdt banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].iw = (*it)->getInt("IW");
     m_d[i].laymdt = (*it)->getInt("LAYMDT");
     sprintf(m_d[i].typ,"%s",(*it)->getString("TYP").c_str());
     m_d[i].x0 = (*it)->getFloat("X0");
     m_d[i].tubpit = (*it)->getFloat("TUBPIT");
     m_d[i].tubrad = (*it)->getFloat("TUBRAD");
     m_d[i].tubsta = (*it)->getFloat("TUBSTA");
     m_d[i].tubdea = (*it)->getFloat("TUBDEA");
     m_d[i].tubwal = (*it)->getFloat("TUBWAL");
     for(unsigned int j=0; j<4; j++)
     {
       std::ostringstream tem;
       tem << j;
       std::string tagx = "TUBXCO_"+tem.str();
       std::string tagy = "TUBYCO_"+tem.str();
       m_d[i].tubxco[j] = (*it)->getFloat(tagx);        
       m_d[i].tubyco[j] = (*it)->getFloat(tagy);        
     }
  }
}

DblQ00Wmdt::~DblQ00Wmdt()
{
    delete [] m_d;
}

} // end of namespace MuonGM
