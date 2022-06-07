/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Xtomo.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>

namespace MuonGM
{
DblQ00Xtomo::DblQ00Xtomo() :
    m_nObj(0) {
   m_d = nullptr;
}
    
  DblQ00Xtomo::DblQ00Xtomo(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr xtomo = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);
    if(xtomo->size()>0) {

        
    m_nObj = xtomo->size();
    m_d = new XTOMO[m_nObj];
    if (m_nObj == 0) std::cerr << "No XtomoData entries in the MuonDD Database" << std::endl;

    size_t i=0;

    while(i<xtomo->size()) {
      m_d[i].line = i;
      m_d[i].XTOMOCHBERNAME = (*xtomo)[i]->getString("XTOMOCHBERNAME"); 
      m_d[i].XTOMOSITE = (*xtomo)[i]->getString("XTOMOSITE"); 
      m_d[i].XTOMOSITEID = (*xtomo)[i]->getInt("XTOMOSITEID");
      m_d[i].XTOMOTIME = (*xtomo)[i]->getInt("XTOMOTIME"); 
      m_d[i].XTOMOPASSED = (*xtomo)[i]->getInt("XTOMOPASSED");
      m_d[i].XTOMOSIDE = (*xtomo)[i]->getString("XTOMOSIDE");
      m_d[i].XTOMONBERTUBE1 = (*xtomo)[i]->getInt("XTOMONBERTUBE1");
      m_d[i].XTOMONBERTUBE2 = (*xtomo)[i]->getInt("XTOMONBERTUBE2");
      m_d[i].XTOMONBERML = (*xtomo)[i]->getInt("XTOMONBERML");
      m_d[i].XTOMONBERLAYER = (*xtomo)[i]->getInt("XTOMONBERLAYER");
      try {
	m_d[i].XTOMOML1STAGG = (*xtomo)[i]->getInt("XTOMOML1STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOML2STAGG = (*xtomo)[i]->getInt("XTOMOML2STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOD1 = (*xtomo)[i]->getFloat("XTOMOD1");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMONMEZ = (*xtomo)[i]->getInt("XTOMONMEZ");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      m_d[i].XTOMOML1NYTUB = (*xtomo)[i]->getFloat("XTOMOML1NYTUB");
      m_d[i].XTOMOML1NZTUB = (*xtomo)[i]->getFloat("XTOMOML1NZTUB"); 
      m_d[i].XTOMOML1NDELA = (*xtomo)[i]->getFloat("XTOMOML1NDELA"); 
      m_d[i].XTOMOML1NYPIT = (*xtomo)[i]->getFloat("XTOMOML1NYPIT"); 
      m_d[i].XTOMOML1NZPIT = (*xtomo)[i]->getFloat("XTOMOML1NZPIT"); 
      m_d[i].XTOMOML1PYTUB = (*xtomo)[i]->getFloat("XTOMOML1PYTUB"); 
      m_d[i].XTOMOML1PZTUB = (*xtomo)[i]->getFloat("XTOMOML1PZTUB"); 
      m_d[i].XTOMOML1PDELA = (*xtomo)[i]->getFloat("XTOMOML1PDELA"); 
      m_d[i].XTOMOML1PYPIT = (*xtomo)[i]->getFloat("XTOMOML1PYPIT"); 
      m_d[i].XTOMOML1PZPIT = (*xtomo)[i]->getFloat("XTOMOML1PZPIT"); 
      m_d[i].XTOMOML2NYTUB = (*xtomo)[i]->getFloat("XTOMOML2NYTUB"); 
      m_d[i].XTOMOML2NZTUB = (*xtomo)[i]->getFloat("XTOMOML2NZTUB"); 
      m_d[i].XTOMOML2NDELA = (*xtomo)[i]->getFloat("XTOMOML2NDELA"); 
      m_d[i].XTOMOML2NYPIT = (*xtomo)[i]->getFloat("XTOMOML2NYPIT"); 
      m_d[i].XTOMOML2NZPIT = (*xtomo)[i]->getFloat("XTOMOML2NZPIT"); 
      m_d[i].XTOMOML2PYTUB = (*xtomo)[i]->getFloat("XTOMOML2PYTUB"); 
      m_d[i].XTOMOML2PZTUB = (*xtomo)[i]->getFloat("XTOMOML2PZTUB"); 
      m_d[i].XTOMOML2PDELA = (*xtomo)[i]->getFloat("XTOMOML2PDELA"); 
      m_d[i].XTOMOML2PYPIT = (*xtomo)[i]->getFloat("XTOMOML2PYPIT"); 
      m_d[i].XTOMOML2PZPIT = (*xtomo)[i]->getFloat("XTOMOML2PZPIT"); 
      i++;
    }


  } else {
    m_d = nullptr;
  }

}

DblQ00Xtomo::DblQ00Xtomo(AmdcDb* xtomo) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = xtomo->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new XTOMO[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Xtomo banks in the AmdcDbRecord"<<std::endl;

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

      m_d[i].line = i;
      m_d[i].XTOMOCHBERNAME = (*it)->getString("XTOMOCHBERNAME"); 
      m_d[i].XTOMOSITE = (*it)->getString("XTOMOSITE"); 
      m_d[i].XTOMOSITEID = (*it)->getInt("XTOMOSITEID");
      m_d[i].XTOMOTIME = (*it)->getInt("XTOMOTIME"); 
      m_d[i].XTOMOPASSED = (*it)->getInt("XTOMOPASSED");
      m_d[i].XTOMOSIDE = (*it)->getString("XTOMOSIDE");
      m_d[i].XTOMONBERTUBE1 = (*it)->getInt("XTOMONBERTUBE1");
      m_d[i].XTOMONBERTUBE2 = (*it)->getInt("XTOMONBERTUBE2");
      m_d[i].XTOMONBERML = (*it)->getInt("XTOMONBERML");
      m_d[i].XTOMONBERLAYER = (*it)->getInt("XTOMONBERLAYER");
      try {
  m_d[i].XTOMOML1STAGG = (*it)->getInt("XTOMOML1STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOML2STAGG = (*it)->getInt("XTOMOML2STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOD1 = (*it)->getFloat("XTOMOD1");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMONMEZ = (*it)->getInt("XTOMONMEZ");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      m_d[i].XTOMOML1NYTUB = (*it)->getFloat("XTOMOML1NYTUB");
      m_d[i].XTOMOML1NZTUB = (*it)->getFloat("XTOMOML1NZTUB"); 
      m_d[i].XTOMOML1NDELA = (*it)->getFloat("XTOMOML1NDELA"); 
      m_d[i].XTOMOML1NYPIT = (*it)->getFloat("XTOMOML1NYPIT"); 
      m_d[i].XTOMOML1NZPIT = (*it)->getFloat("XTOMOML1NZPIT"); 
      m_d[i].XTOMOML1PYTUB = (*it)->getFloat("XTOMOML1PYTUB"); 
      m_d[i].XTOMOML1PZTUB = (*it)->getFloat("XTOMOML1PZTUB"); 
      m_d[i].XTOMOML1PDELA = (*it)->getFloat("XTOMOML1PDELA"); 
      m_d[i].XTOMOML1PYPIT = (*it)->getFloat("XTOMOML1PYPIT"); 
      m_d[i].XTOMOML1PZPIT = (*it)->getFloat("XTOMOML1PZPIT"); 
      m_d[i].XTOMOML2NYTUB = (*it)->getFloat("XTOMOML2NYTUB"); 
      m_d[i].XTOMOML2NZTUB = (*it)->getFloat("XTOMOML2NZTUB"); 
      m_d[i].XTOMOML2NDELA = (*it)->getFloat("XTOMOML2NDELA"); 
      m_d[i].XTOMOML2NYPIT = (*it)->getFloat("XTOMOML2NYPIT"); 
      m_d[i].XTOMOML2NZPIT = (*it)->getFloat("XTOMOML2NZPIT"); 
      m_d[i].XTOMOML2PYTUB = (*it)->getFloat("XTOMOML2PYTUB"); 
      m_d[i].XTOMOML2PZTUB = (*it)->getFloat("XTOMOML2PZTUB"); 
      m_d[i].XTOMOML2PDELA = (*it)->getFloat("XTOMOML2PDELA"); 
      m_d[i].XTOMOML2PYPIT = (*it)->getFloat("XTOMOML2PYPIT"); 
      m_d[i].XTOMOML2PZPIT = (*it)->getFloat("XTOMOML2PZPIT"); 
  }
}

DblQ00Xtomo::~DblQ00Xtomo()
{
    if  (m_nObj > 0 && m_d) delete [] m_d;
}

} // end of namespace MuonGM
