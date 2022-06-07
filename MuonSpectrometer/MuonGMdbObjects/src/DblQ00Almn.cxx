/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Almn.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdio.h>

namespace MuonGM
{

  DblQ00Almn::DblQ00Almn(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr almn = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(almn->size()>0) {
    m_nObj = almn->size();
    m_d = new ALMN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Almn banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<almn->size()) {
        m_d[i].version =           (*almn)[i]->getInt("VERS");
        m_d[i].i =                 (*almn)[i]->getInt("I");
        m_d[i].dx =                (*almn)[i]->getFloat("DX");
        m_d[i].dy =                (*almn)[i]->getFloat("DY");
        m_d[i].dz =                (*almn)[i]->getFloat("DZ");
        m_d[i].job =               (*almn)[i]->getInt("JOB");
        sprintf(m_d[i].tec,"%s",(*almn)[i]->getString("TEC").c_str());
        m_d[i].iw =                (*almn)[i]->getInt("IW");
        m_d[i].isplit_x =          (*almn)[i]->getInt("ISPLIT_X");
        m_d[i].isplit_y =          (*almn)[i]->getInt("ISPLIT_Y");
        m_d[i].ishape =            (*almn)[i]->getInt("ISHAPE");
        m_d[i].width_xs =          (*almn)[i]->getFloat("WIDTH_XS");
        m_d[i].width_xl =          (*almn)[i]->getFloat("WIDTH_XL");
        m_d[i].length_y =          (*almn)[i]->getFloat("LENGTH_Y");
        m_d[i].excent =            (*almn)[i]->getFloat("EXCENT");
        m_d[i].dead1 =             (*almn)[i]->getFloat("DEAD1");
        m_d[i].dead2 =             (*almn)[i]->getFloat("DEAD2");
        m_d[i].dead3 =             (*almn)[i]->getFloat("DEAD3");
        m_d[i].jtyp =              (*almn)[i]->getInt("JTYP");
        m_d[i].indx =              (*almn)[i]->getInt("INDX");
        //std::cerr<<" here we are i = "<<i<<" dx, dy, dz "<<m_d[i].dx<<" "<<m_d[i].dy<<" "<<m_d[i].dz<<" tec "<<m_d[i].tec<<std::endl;
        i++;
    }
  }
  else {
    m_d = new ALMN[0];
    std::cerr<<"NO Almn banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Almn::DblQ00Almn(AmdcDb* almn) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = almn->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new ALMN[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Almn banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].i = (*it)->getInt("I");
     m_d[i].dx = (*it)->getFloat("DX");
     m_d[i].dy = (*it)->getFloat("DY");
     m_d[i].dz = (*it)->getFloat("DZ");
     m_d[i].job = (*it)->getInt("JOB");
     sprintf(m_d[i].tec,"%s",(*it)->getString("TEC").c_str());
     m_d[i].iw = (*it)->getInt("IW");
     m_d[i].isplit_x = (*it)->getInt("ISPLIT_X");
     m_d[i].isplit_y = (*it)->getInt("ISPLIT_Y");
     m_d[i].ishape = (*it)->getInt("ISHAPE");
     m_d[i].width_xs = (*it)->getFloat("WIDTH_XS");
     m_d[i].width_xl = (*it)->getFloat("WIDTH_XL");
     m_d[i].length_y = (*it)->getFloat("LENGTH_Y");
     m_d[i].excent = (*it)->getFloat("EXCENT");
     m_d[i].dead1 = (*it)->getFloat("DEAD1");
     m_d[i].dead2 = (*it)->getFloat("DEAD2");
     m_d[i].dead3 = (*it)->getFloat("DEAD3");
     m_d[i].jtyp =  (*it)->getInt("JTYP");
     m_d[i].indx =  (*it)->getInt("INDX");
  }
}

DblQ00Almn::~DblQ00Almn()
{
    delete [] m_d;
}

} // end of namespace MuonGM
