/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    
DblQ00Xtomo::DblQ00Xtomo(std::unique_ptr<IRDBQuery>&& xtomo) :
    m_nObj(0) {
  std::cout << "In DblQ00Xtomo(data)" << std::endl;
  if(xtomo) {
    xtomo->execute();
        
    m_nObj = xtomo->size();
    m_d = new XTOMO[m_nObj];
    if (m_nObj == 0) std::cerr << "No XtomoData entries in the MuonDD Database" << std::endl;

    int i=0;

    while(xtomo->next()) {
      m_d[i].line = i;
      m_d[i].XTOMOCHBERNAME = xtomo->data<std::string>("XTOMODATA_DATA.XTOMOCHBERNAME"); 
      m_d[i].XTOMOSITE = xtomo->data<std::string>("XTOMODATA_DATA.XTOMOSITE"); 
      m_d[i].XTOMOSITEID = xtomo->data<int>("XTOMODATA_DATA.XTOMOSITEID");
      m_d[i].XTOMOTIME = xtomo->data<int>("XTOMODATA_DATA.XTOMOTIME"); 
      m_d[i].XTOMOPASSED = xtomo->data<int>("XTOMODATA_DATA.XTOMOPASSED");
      m_d[i].XTOMOSIDE = xtomo->data<std::string>("XTOMODATA_DATA.XTOMOSIDE");
      m_d[i].XTOMONBERTUBE1 = xtomo->data<int>("XTOMODATA_DATA.XTOMONBERTUBE1");
      m_d[i].XTOMONBERTUBE2 = xtomo->data<int>("XTOMODATA_DATA.XTOMONBERTUBE2");
      m_d[i].XTOMONBERML = xtomo->data<int>("XTOMODATA_DATA.XTOMONBERML");
      m_d[i].XTOMONBERLAYER = xtomo->data<int>("XTOMODATA_DATA.XTOMONBERLAYER");
      try {
	m_d[i].XTOMOML1STAGG = xtomo->data<int>("XTOMODATA_DATA.XTOMOML1STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOML2STAGG = xtomo->data<int>("XTOMODATA_DATA.XTOMOML2STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOD1 = xtomo->data<float>("XTOMODATA_DATA.XTOMOD1");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMONMEZ = xtomo->data<int>("XTOMODATA_DATA.XTOMONMEZ");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      m_d[i].XTOMOML1NYTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NYTUB");
      m_d[i].XTOMOML1NZTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NZTUB"); 
      m_d[i].XTOMOML1NDELA = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NDELA"); 
      m_d[i].XTOMOML1NYPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NYPIT"); 
      m_d[i].XTOMOML1NZPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1NZPIT"); 
      m_d[i].XTOMOML1PYTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PYTUB"); 
      m_d[i].XTOMOML1PZTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PZTUB"); 
      m_d[i].XTOMOML1PDELA = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PDELA"); 
      m_d[i].XTOMOML1PYPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PYPIT"); 
      m_d[i].XTOMOML1PZPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML1PZPIT"); 
      m_d[i].XTOMOML2NYTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NYTUB"); 
      m_d[i].XTOMOML2NZTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NZTUB"); 
      m_d[i].XTOMOML2NDELA = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NDELA"); 
      m_d[i].XTOMOML2NYPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NYPIT"); 
      m_d[i].XTOMOML2NZPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2NZPIT"); 
      m_d[i].XTOMOML2PYTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PYTUB"); 
      m_d[i].XTOMOML2PZTUB = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PZTUB"); 
      m_d[i].XTOMOML2PDELA = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PDELA"); 
      m_d[i].XTOMOML2PYPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PYPIT"); 
      m_d[i].XTOMOML2PZPIT = xtomo->data<float>("XTOMODATA_DATA.XTOMOML2PZPIT"); 
      i++;
    }

    xtomo->finalize();

  } else {
    m_d = nullptr;
    std::cerr << "No XtomoData table in the MuonDD Database" << std::endl;
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

      m_d[i].line = i;
      m_d[i].XTOMOCHBERNAME = (*it)->getString("XTOMODATA_DATA.XTOMOCHBERNAME"); 
      m_d[i].XTOMOSITE = (*it)->getString("XTOMODATA_DATA.XTOMOSITE"); 
      m_d[i].XTOMOSITEID = (*it)->getInt("XTOMODATA_DATA.XTOMOSITEID");
      m_d[i].XTOMOTIME = (*it)->getInt("XTOMODATA_DATA.XTOMOTIME"); 
      m_d[i].XTOMOPASSED = (*it)->getInt("XTOMODATA_DATA.XTOMOPASSED");
      m_d[i].XTOMOSIDE = (*it)->getString("XTOMODATA_DATA.XTOMOSIDE");
      m_d[i].XTOMONBERTUBE1 = (*it)->getInt("XTOMODATA_DATA.XTOMONBERTUBE1");
      m_d[i].XTOMONBERTUBE2 = (*it)->getInt("XTOMODATA_DATA.XTOMONBERTUBE2");
      m_d[i].XTOMONBERML = (*it)->getInt("XTOMODATA_DATA.XTOMONBERML");
      m_d[i].XTOMONBERLAYER = (*it)->getInt("XTOMODATA_DATA.XTOMONBERLAYER");
      try {
  m_d[i].XTOMOML1STAGG = (*it)->getInt("XTOMODATA_DATA.XTOMOML1STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOML2STAGG = (*it)->getInt("XTOMODATA_DATA.XTOMOML2STAGG");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMOD1 = (*it)->getFloat("XTOMODATA_DATA.XTOMOD1");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      try {
        m_d[i].XTOMONMEZ = (*it)->getInt("XTOMODATA_DATA.XTOMONMEZ");
      } catch (std::exception&) {} // ignore exception for now: field missing in MuonSpectrometer-R.06.01-tomotest
      m_d[i].XTOMOML1NYTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1NYTUB");
      m_d[i].XTOMOML1NZTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1NZTUB"); 
      m_d[i].XTOMOML1NDELA = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1NDELA"); 
      m_d[i].XTOMOML1NYPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1NYPIT"); 
      m_d[i].XTOMOML1NZPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1NZPIT"); 
      m_d[i].XTOMOML1PYTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1PYTUB"); 
      m_d[i].XTOMOML1PZTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1PZTUB"); 
      m_d[i].XTOMOML1PDELA = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1PDELA"); 
      m_d[i].XTOMOML1PYPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1PYPIT"); 
      m_d[i].XTOMOML1PZPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML1PZPIT"); 
      m_d[i].XTOMOML2NYTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2NYTUB"); 
      m_d[i].XTOMOML2NZTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2NZTUB"); 
      m_d[i].XTOMOML2NDELA = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2NDELA"); 
      m_d[i].XTOMOML2NYPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2NYPIT"); 
      m_d[i].XTOMOML2NZPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2NZPIT"); 
      m_d[i].XTOMOML2PYTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2PYTUB"); 
      m_d[i].XTOMOML2PZTUB = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2PZTUB"); 
      m_d[i].XTOMOML2PDELA = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2PDELA"); 
      m_d[i].XTOMOML2PYPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2PYPIT"); 
      m_d[i].XTOMOML2PZPIT = (*it)->getFloat("XTOMODATA_DATA.XTOMOML2PZPIT"); 
  }
}

DblQ00Xtomo::~DblQ00Xtomo()
{
    if  (m_nObj > 0 && m_d) delete [] m_d;
}

} // end of namespace MuonGM
