/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Aptp.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>
#include <stdio.h>

namespace MuonGM
{

  DblQ00Aptp::DblQ00Aptp(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr aptp = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(aptp->size()>0) {
    m_nObj = aptp->size();
    m_d = new APTP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Aptp banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<aptp->size()) {
        m_d[i].version     = (*aptp)[i]->getInt("VERS");    
        m_d[i].line        = (*aptp)[i]->getInt("LINE");
        sprintf(m_d[i].type,"%s",(*aptp)[i]->getString("TYP").c_str()); 
        m_d[i].i           = (*aptp)[i]->getInt("I");
        m_d[i].icut        = (*aptp)[i]->getInt("ICUT");
	m_d[i].iphi[0]     = (*aptp)[i]->getInt("IPHI_0");        
	m_d[i].iphi[1]     = (*aptp)[i]->getInt("IPHI_1");        
	m_d[i].iphi[2]     = (*aptp)[i]->getInt("IPHI_2");        
	m_d[i].iphi[3]     = (*aptp)[i]->getInt("IPHI_3");        
	m_d[i].iphi[4]     = (*aptp)[i]->getInt("IPHI_4");        
	m_d[i].iphi[5]     = (*aptp)[i]->getInt("IPHI_5");        
	m_d[i].iphi[6]     = (*aptp)[i]->getInt("IPHI_6");        
	m_d[i].iphi[7]     = (*aptp)[i]->getInt("IPHI_7");        
        m_d[i].iz          = (*aptp)[i]->getInt("IZ");
        m_d[i].dphi        = (*aptp)[i]->getFloat("DPHI");  
        m_d[i].z           = (*aptp)[i]->getFloat("Z");  
        m_d[i].r           = (*aptp)[i]->getFloat("R");    
        m_d[i].s           = (*aptp)[i]->getFloat("S");     
        m_d[i].alfa        = (*aptp)[i]->getFloat("ALFA");      
        m_d[i].beta        = (*aptp)[i]->getFloat("BETA");     
        m_d[i].gamma       = (*aptp)[i]->getFloat("GAMMA");
        i++;
    }
  }
  else {
    m_d = new APTP[0];
    std::cerr<<"NO Aptp banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Aptp::DblQ00Aptp(AmdcDb* aptp) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = aptp->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new APTP[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Aptp banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].line =  (*it)->getInt("LINE");
     sprintf(m_d[i].type,"%s",(*it)->getString("TYP").c_str());
     m_d[i].i = (*it)->getInt("I");
     m_d[i].icut = (*it)->getInt("ICUT");
     for(int DB_JFF=0; DB_JFF<8 ; DB_JFF++) {
        std::string DbVar = "";
        std::ostringstream Aostringstream;
        Aostringstream << DB_JFF;
        DbVar = "IPHI_"+Aostringstream.str();
        m_d[i].iphi[DB_JFF] =(*it)->getInt(DbVar);
     }
     m_d[i].iz = (*it)->getInt("IZ");
     m_d[i].dphi = (*it)->getFloat("DPHI");
     m_d[i].z = (*it)->getFloat("Z");
     m_d[i].r = (*it)->getFloat("R");
     m_d[i].s = (*it)->getFloat("S");
     m_d[i].alfa = (*it)->getFloat("ALFA");
     m_d[i].beta = (*it)->getFloat("BETA");
     m_d[i].gamma = (*it)->getFloat("GAMMA");
  }
}

DblQ00Aptp::~DblQ00Aptp()
{
    delete [] m_d;
}

} // end of namespace MuonGM
