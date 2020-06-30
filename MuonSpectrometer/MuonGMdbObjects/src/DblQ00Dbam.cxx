/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Dbam.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdexcept>

namespace MuonGM
{

DblQ00Dbam::DblQ00Dbam(std::unique_ptr<IRDBQuery>&& dbam) :
    m_nObj(0) {
  if(dbam) {
    dbam->execute();
    m_nObj = dbam->size();
    m_d = new DBAM[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Dbam banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(dbam->next()) {
        m_d[i].version     = dbam->data<int>("DBAM_DATA.VERS");    
        m_d[i].nvrs        = dbam->data<int>("DBAM_DATA.NVRS");
        m_d[i].mtyp        = dbam->data<int>("DBAM_DATA.MTYP");
        m_d[i].numbox      = dbam->data<int>("DBAM_DATA.NUMBOX");
        sprintf(m_d[i].amdb,"%s",dbam->data<std::string>("DBAM_DATA.AMDB").c_str()); 
        try {
            sprintf(m_d[i].test,"%s",dbam->data<std::string>("DBAM_DATA.TEST").c_str());
        }
        catch (const std::runtime_error&)
        {
            //std::cerr<<"no TEST field available in DBAM"<<std::endl;
            sprintf(m_d[i].test,"unknown");
        }
        for (unsigned int j=0; j<53; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tag = "DBAM_DATA.NAME_"+tem.str();
            try {
                sprintf(m_d[i].name[j],"%s",dbam->data<std::string>(tag).c_str());
            }
            catch (const std::runtime_error&)
            {
                break;
            }
        }
        i++;
    }
    dbam->finalize();
  }
  else {
    m_d = new DBAM[0];
    std::cerr<<"NO Dbam banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Dbam::DblQ00Dbam(AmdcDb* dbam) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = dbam->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new DBAM[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Dbam banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].nvrs = (*it)->getInt("NVRS");
     m_d[i].mtyp = (*it)->getInt("MTYP");
     m_d[i].numbox = (*it)->getInt("NUMBOX");
     sprintf(m_d[i].amdb,"%s",(*it)->getString("AMDB").c_str()); 
     if(((*it)->getString("TEST")).compare("NOT FOUND") == 0 ) sprintf(m_d[i].test,"unknown");
     else sprintf(m_d[i].test,"%s",(*it)->getString("TEST").c_str());

     for(unsigned int j=0; j<53; j++)
     {
       std::ostringstream tem;
       tem << j;
       std::string tag = "NAME_"+tem.str();
       if(((*it)->getString(tag)).compare("NOT FOUND") == 0 ) break;
       sprintf(m_d[i].name[j],"%s",(*it)->getString(tag).c_str());
     }
  }
}

DblQ00Dbam::~DblQ00Dbam()
{
    delete [] m_d;
}

} // end of namespace MuonGM
