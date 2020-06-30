/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Almn.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <iostream>
#include <stdio.h>

namespace MuonGM
{

DblQ00Almn::DblQ00Almn(std::unique_ptr<IRDBQuery>&& almn) :
    m_nObj(0) {
  if(almn) {
    almn->execute();
    m_nObj = almn->size();
    m_d = new ALMN[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Almn banks in the MuonDD Database"<<std::endl;

     unsigned fieldVers(1);
     unsigned fieldI(2);
     unsigned fieldDx(3);
     unsigned fieldDy(4);
     unsigned fieldDz(5);
     unsigned fieldJob(6);
     unsigned fieldTec(7);
     unsigned fieldIw(8);
     unsigned fieldIsplit_x(9);
     unsigned fieldIsplit_y(10);
     unsigned fieldIshape(11);
     unsigned fieldWidth_xs(12);
     unsigned fieldWidth_xl(13);
     unsigned fieldLength_y(14);
     unsigned fieldExcent(15);
     unsigned fieldDead1(16);
     unsigned fieldDead2(17);
     unsigned fieldDead3(18);
     unsigned fieldJtyp(19);
     unsigned fieldIndx(20);

    int i=0;
    while(almn->next()) {
        m_d[i].version =           almn->data<int>(fieldVers);
        m_d[i].i =                 almn->data<int>(fieldI);
        m_d[i].dx =                almn->data<float>(fieldDx);
        m_d[i].dy =                almn->data<float>(fieldDy);
        m_d[i].dz =                almn->data<float>(fieldDz);
        m_d[i].job =               almn->data<int>(fieldJob);
        sprintf(m_d[i].tec,"%s",almn->data<std::string>(fieldTec).c_str());
        m_d[i].iw =                almn->data<int>(fieldIw);
        m_d[i].isplit_x =          almn->data<int>(fieldIsplit_x);
        m_d[i].isplit_y =          almn->data<int>(fieldIsplit_y);
        m_d[i].ishape =            almn->data<int>(fieldIshape);
        m_d[i].width_xs =          almn->data<float>(fieldWidth_xs);
        m_d[i].width_xl =          almn->data<float>(fieldWidth_xl);
        m_d[i].length_y =          almn->data<float>(fieldLength_y);
        m_d[i].excent =            almn->data<float>(fieldExcent);
        m_d[i].dead1 =             almn->data<float>(fieldDead1);
        m_d[i].dead2 =             almn->data<float>(fieldDead2);
        m_d[i].dead3 =             almn->data<float>(fieldDead3);
        m_d[i].jtyp =              almn->data<int>(fieldJtyp);
        m_d[i].indx =              almn->data<int>(fieldIndx);
        //std::cerr<<" here we are i = "<<i<<" dx, dy, dz "<<m_d[i].dx<<" "<<m_d[i].dy<<" "<<m_d[i].dz<<" tec "<<m_d[i].tec<<std::endl;
        i++;
    }
    almn->finalize();
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
