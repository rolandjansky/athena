/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 DB data - Muon Station components
 -----------------------------------------
 ***************************************************************************/

#include "MuonGMdbObjects/DblQ00Aszt.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "AmdcDb/AmdcDb.h"
#include "AmdcDb/AmdcDbRecord.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>

namespace MuonGM
{
DblQ00Aszt::DblQ00Aszt() : m_d(nullptr)
{
    m_nObj = 0;
}
    
  DblQ00Aszt::DblQ00Aszt(IRDBAccessSvc *pAccessSvc, const std::string & GeoTag, const std::string & GeoNode):
    m_nObj(0) {

    IRDBRecordset_ptr aszt = pAccessSvc->getRecordsetPtr(getName(),GeoTag, GeoNode);

    if(aszt->size()>0) {
    m_nObj = aszt->size();
    m_d = new ASZT[m_nObj];
    if (m_nObj == 0) std::cerr<<"NO Aszt banks in the MuonDD Database"<<std::endl;

    size_t i=0;
    while(i<aszt->size()) {
        m_d[i].version        = (*aszt)[i]->getInt("VERS");    
        m_d[i].line           = (*aszt)[i]->getInt("LINE");          
        m_d[i].jff            = (*aszt)[i]->getInt("JFF");
        m_d[i].jzz            = (*aszt)[i]->getInt("JZZ");
        m_d[i].job            = (*aszt)[i]->getInt("JOB");
        m_d[i].tras           = (*aszt)[i]->getFloat("TRAS");
        m_d[i].traz           = (*aszt)[i]->getFloat("TRAZ");
        m_d[i].trat           = (*aszt)[i]->getFloat("TRAT");
        m_d[i].rots           = (*aszt)[i]->getFloat("ROTS");
        m_d[i].rotz           = (*aszt)[i]->getFloat("ROTZ");
        m_d[i].rott           = (*aszt)[i]->getFloat("ROTT");
        m_d[i].i              = (*aszt)[i]->getInt("I");
        sprintf(m_d[i].type,"%s",(*aszt)[i]->getString("TYP").c_str());
        //std::cerr<<" Aszt:: version, type, jtyp, nsta "<<m_d[i].version<<" "<<m_d[i].type<<" "<<m_d[i].jtyp<<" "<<m_d[i].nsta  <<std::endl;
        i++;
    }
  }
  else {
    m_d = new ASZT[0];
    std::cerr<<"NO Aszt banks in the MuonDD Database"<<std::endl;
  }
}

DblQ00Aszt::DblQ00Aszt(AmdcDb* aszt) :
    m_nObj(0) {
  IRDBRecordset_ptr pIRDBRecordset = aszt->getRecordsetPtr(std::string(getObjName()),"Amdc");
  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();

  m_nObj = pIRDBRecordset->size();
  m_d = new ASZT[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Aszt banks in the AmdcDbRecord"<<std::endl;

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
     m_d[i].line = (*it)->getInt("LINE");          
     m_d[i].jff = (*it)->getInt("JFF");
     m_d[i].jzz = (*it)->getInt("JZZ");
     m_d[i].job = (*it)->getInt("JOB");
     m_d[i].tras = (*it)->getFloat("TRAS");
     m_d[i].traz = (*it)->getFloat("TRAZ");
     m_d[i].trat = (*it)->getFloat("TRAT");
     m_d[i].rots = (*it)->getFloat("ROTS");
     m_d[i].rotz = (*it)->getFloat("ROTZ");
     m_d[i].rott = (*it)->getFloat("ROTT");
     m_d[i].i = (*it)->getInt("I");
     sprintf(m_d[i].type,"%s",(*it)->getString("TYP").c_str());
  }
}

DblQ00Aszt::DblQ00Aszt(const std::string& asciiFileName) {

  std::cerr<<"Aszt with asciiFileName = : <"<<asciiFileName<<"> "<<std::endl;
  // open file and count number of lines
  m_nObj=0;
  std::ifstream asztFile(asciiFileName.c_str());
  if (!asztFile.is_open()) 
    std::cerr<<" bad ascii file: "<<asciiFileName<<std::endl;
  
  
  m_nObj = std::count(std::istreambuf_iterator<char>(asztFile),
		      std::istreambuf_iterator<char>(),'\n');
  std::cout<<"Number of lines in the A-line file <"<<asciiFileName<<"> is "<< m_nObj <<std::endl;
  
  
  m_d = new ASZT[m_nObj];
  if (m_nObj == 0) std::cerr<<"NO Aszt banks in "<<asciiFileName<<std::endl;
  
  int j=0;

  // close and reopen file for input
  asztFile.close();
  asztFile.open(asciiFileName.c_str());

  char AlineMarker;
  while ( asztFile 
          >> AlineMarker 
	  >> m_d[j].type
	  >> m_d[j].jff
	  >> m_d[j].jzz
	  >> m_d[j].job
	  >> m_d[j].tras
	  >> m_d[j].traz
	  >> m_d[j].trat
	  >> m_d[j].rots
	  >> m_d[j].rotz
	  >> m_d[j].rott
	  )
  {  
      m_d[j].line = j+1;
      m_d[j].tras = 0.1*m_d[j].tras; // ProcessAlignments expects cm !
      m_d[j].traz = 0.1*m_d[j].traz; // ProcessAlignments expects cm !
      m_d[j].trat = 0.1*m_d[j].trat; // ProcessAlignments expects cm !
      j++;
  }
  

  if (j!=(int)m_nObj) { 
    std::cerr<<"problem with DblQ00Aszt: j="<<j<<" m_nObj="<<(int)m_nObj<<std::endl; 
  }
}

DblQ00Aszt::~DblQ00Aszt()
{
    if  (m_nObj > 0) delete [] m_d;
}

void DblQ00Aszt::WriteAsztToAsciiFile(const std::string& filename)
{
  std::ofstream asztFile;
  asztFile.open(filename.c_str());
  for (int j=0;j<(int)m_nObj;j++) {
    asztFile
        <<"A "
        << m_d[j].type  <<" " 
        << m_d[j].jff   <<" " 
        << m_d[j].jzz   <<" " 
        << m_d[j].job   <<"  "
        << m_d[j].tras*10.  <<" "  // here mm !
        << m_d[j].traz*10.  <<" "  // here mm !
        << m_d[j].trat*10.  <<" "  // here mm !
        << m_d[j].rots  <<" " 
        << m_d[j].rotz  <<" " 
        << m_d[j].rott  <<" " 
        << "\n";
  }
  asztFile.close();  
}

} // end of namespace MuonGM
