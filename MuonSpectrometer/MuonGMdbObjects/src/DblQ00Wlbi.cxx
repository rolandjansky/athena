/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Wlbi.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>

namespace MuonGM
{

DblQ00Wlbi::DblQ00Wlbi(std::unique_ptr<IRDBQuery>&& wlbi) :
    m_nObj(0) {
  if(wlbi) {
    wlbi->execute();
    m_nObj = wlbi->size();
    m_d = new WLBI[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wlbi banks in the MuonDD Database"<<std::endl;

    int i=0;
    while(wlbi->next()) {
        m_d[i].version     = wlbi->data<int>("WLBI_DATA.VERS");    
        m_d[i].jsta        = wlbi->data<int>("WLBI_DATA.JSTA");
        m_d[i].num         = wlbi->data<int>("WLBI_DATA.NUM");
        m_d[i].height      = wlbi->data<float>("WLBI_DATA.HEIGHT");
        m_d[i].thickness   = wlbi->data<float>("WLBI_DATA.THICKNESS");
	if (!wlbi->isNull("WLBI_DATA.LOWERTHICK"))
	  {
	    m_d[i].lowerThickness   = wlbi->data<float>("WLBI_DATA.LOWERTHICK");
	  }
	else m_d[i].lowerThickness   = m_d[i].thickness ;
	if (!wlbi->isNull("WLBI_DATA.SHIFTYSTATION"))
	  {
	    m_d[i].yShift   = wlbi->data<float>("WLBI_DATA.SHIFTYSTATION");
	  }
	else m_d[i].yShift   = 0.;
        i++;
    }
    wlbi->finalize();
  }
  else {
    m_d = new WLBI[0];
    std::cerr<<"NO Wlbi banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Wlbi::DblQ00Wlbi(AmdcDb* wlbi) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = wlbi->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new WLBI[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Wlbi banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].num = (*it)->getInt("NUM");
     m_d[i].height = (*it)->getFloat("HEIGHT");
     m_d[i].thickness = (*it)->getFloat("THICKNESS");
     if((*it)->getFloat("LOWERTHICK") != 999999999999.) m_d[i].lowerThickness   = (*it)->getFloat("LOWERTHICK");
     else m_d[i].lowerThickness   = m_d[i].thickness ;
     if ((*it)->getFloat(".SHIFTYSTATION") != 999999999999.) m_d[i].yShift   = (*it)->getFloat("SHIFTYSTATION");
     else m_d[i].yShift   = 0.;
  }
}

DblQ00Wlbi::~DblQ00Wlbi()
{
    delete [] m_d;
}

} // end of namespace MuonGM
