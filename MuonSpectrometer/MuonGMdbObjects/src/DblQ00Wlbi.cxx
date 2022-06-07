/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace MuonGM
{

  DblQ00Wlbi::DblQ00Wlbi(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr wlbi = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(wlbi->size()>0) {
    m_nObj = wlbi->size();
    m_d = new WLBI[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Wlbi banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<wlbi->size()) {
        m_d[i].version     = (*wlbi)[i]->getInt("VERS");    
        m_d[i].jsta        = (*wlbi)[i]->getInt("JSTA");
        m_d[i].num         = (*wlbi)[i]->getInt("NUM");
        m_d[i].height      = (*wlbi)[i]->getFloat("HEIGHT");
        m_d[i].thickness   = (*wlbi)[i]->getFloat("THICKNESS");
	try
	  {
	    m_d[i].lowerThickness   = (*wlbi)[i]->getFloat("LOWERTHICK");
	  }
	catch (std::runtime_error &) {
	  m_d[i].lowerThickness   = m_d[i].thickness ;
	}
	try
	  {
	    m_d[i].yShift   = (*wlbi)[i]->getFloat("SHIFTYSTATION");
	  }
	catch(std::runtime_error &) {
	  m_d[i].yShift   = 0.;
	}
        i++;
    }
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
