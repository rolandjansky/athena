/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wtgc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdexcept>

namespace MuonGM
{

DblQ00Wtgc::DblQ00Wtgc(std::unique_ptr<IRDBQuery>&& wtgc) :
    m_nObj(0) {
  if(wtgc) {
    wtgc->execute();
    m_nObj = wtgc->size();
    m_d = new WTGC[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wtgc banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wtgc->next()) {
        m_d[i].version     = wtgc->data<int>("WTGC_DATA.VERS");    
        m_d[i].jsta        = wtgc->data<int>("WTGC_DATA.JSTA");
        m_d[i].nbevol      = wtgc->data<int>("WTGC_DATA.NBEVOL");
        m_d[i].x0          = wtgc->data<float>("WTGC_DATA.X0");
        m_d[i].widchb      = wtgc->data<float>("WTGC_DATA.WIDCHB");
        m_d[i].fwirch      = wtgc->data<float>("WTGC_DATA.FWIRCH");
        m_d[i].fwixch      = wtgc->data<float>("WTGC_DATA.FWIXCH");
        for (unsigned int j=0; j<9; j++)
        {
            std::ostringstream tem;
            tem << j;
            std::string tag = "WTGC_DATA.ALLNAME_"+tem.str();
            try {
	      sprintf(m_d[i].allname[j],"%s",wtgc->data<std::string>(tag).c_str());
            }
            catch (const std::runtime_error&)
            {
                break;
            }
        }
        i++;
    }
    wtgc->finalize();
  }
  else {
    m_d = new WTGC[0];
    std::cerr<<"NO Wtgc banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wtgc::DblQ00Wtgc(AmdcDb* wtgc) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wtgc->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WTGC[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wtgc banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].nbevol = (*it)->getInt("NBEVOL");
     m_d[i].x0 = (*it)->getFloat("X0");
     m_d[i].widchb = (*it)->getFloat("WIDCHB");
     m_d[i].fwirch = (*it)->getFloat("FWIRCH");
     m_d[i].fwixch = (*it)->getFloat("FWIXCH");
     for(unsigned int j=0; j<9; j++)
     {
       std::ostringstream tem;
       tem << j;
       std::string tag = "ALLNAME_"+tem.str();
       if(((*it)->getString(tag)).compare("NOT FOUND") == 0 ) break;
       sprintf(m_d[i].allname[j],"%s",(*it)->getString(tag).c_str());
     }
  }
}

DblQ00Wtgc::~DblQ00Wtgc()
{
    delete [] m_d;
}

} // end of namespace MuonGM
