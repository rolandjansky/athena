/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Aptp.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>
#include <stdio.h>

namespace MuonGM
{

DblQ00Aptp::DblQ00Aptp(std::unique_ptr<IRDBQuery>&& aptp) :
    m_nObj(0) {
  if(aptp) {
    aptp->execute();
    m_nObj = aptp->size();
    m_d = new APTP[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Aptp banks in the MuonDD Database"<<std::endl;

     unsigned fieldVers(1);
     unsigned fieldLine(2);
     unsigned fieldTyp(3);
     unsigned fieldI(4);
     unsigned fieldIcut(5);
     unsigned fieldIphi(6);
     unsigned fieldIz(14);
     unsigned fieldDphi(15);
     unsigned fieldZ(16);
     unsigned fieldR(17);
     unsigned fieldS(18);
     unsigned fieldAlfa(19);
     unsigned fieldBeta(20);
     unsigned fieldGamma(21);

    int i=0;
    while(aptp->next()) {
        m_d[i].version     = aptp->data<int>(fieldVers);    
        m_d[i].line        = aptp->data<int>(fieldLine);
        sprintf(m_d[i].type,"%s",aptp->data<std::string>(fieldTyp).c_str()); 
        m_d[i].i           = aptp->data<int>(fieldI);
        m_d[i].icut        = aptp->data<int>(fieldIcut);
        for (unsigned int j=0; j<8; j++)
        {
            m_d[i].iphi[j]     = aptp->data<int>(fieldIphi+j);        
        }
        m_d[i].iz          = aptp->data<int>(fieldIz);
        m_d[i].dphi        = aptp->data<float>(fieldDphi);  
        m_d[i].z           = aptp->data<float>(fieldZ);  
        m_d[i].r           = aptp->data<float>(fieldR);    
        m_d[i].s           = aptp->data<float>(fieldS);     
        m_d[i].alfa        = aptp->data<float>(fieldAlfa);      
        m_d[i].beta        = aptp->data<float>(fieldBeta);     
        m_d[i].gamma       = aptp->data<float>(fieldGamma);
        i++;
    }
    aptp->finalize();
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
