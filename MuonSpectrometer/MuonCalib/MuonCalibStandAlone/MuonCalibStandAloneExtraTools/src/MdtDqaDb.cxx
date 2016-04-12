/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneExtraTools/MdtDqaDb.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string.h>
#include <vector>


MdtDqaDb::MdtDqaDb(std::string chamName) : m_numNoisyMezz(-1), m_numNoisyTube(-1) {
  Clear();
  m_chamberName = chamName;
}
MdtDqaDb::MdtDqaDb() : m_numNoisyMezz(-1), m_numNoisyTube(-1) {
  Clear();
}

MdtDqaDb::~MdtDqaDb() {}

void MdtDqaDb::Clear() {
  m_listDeadML.clear();
  m_listDeadMezz.clear();
  m_listDeadTube.clear();
  m_listNoisyMezz.clear();
  m_listNoisyTube.clear();
  m_chamberName = "";
  m_t0ML1 = -1;
  m_t0ML2 = -1;
  m_tdriftML1 = -1;
  m_tdriftML2 = -1;
  m_chi2ndofML1 = -1;
  m_chi2ndofML2 = -1;
  m_t0errML1 = -1;
  m_t0errML2 = -1;
  m_tdrifterrML1 = -1;
  m_tdrifterrML2 = -1;
  m_numDeadML = -1;
  m_numDeadMezz = -1;
  m_numDeadTube = -1;
  m_timeFlag = -1;
  m_deadFlag = -1;
}

void MdtDqaDb::Print(std::ofstream *outputFile) {
  *outputFile  << "  ==================================================== " << std::endl ;
  *outputFile  << "  MdtDqaDb output: ---- Chamber name : " << m_chamberName << std::endl ;
  *outputFile  << " timeFlag : " << getTimeFlag() << std::endl ;
  *outputFile  << " deadFlag : " << getDeadFlag() << std::endl ;
  *outputFile  << " chi2 per dof ML1 : " << m_chi2ndofML1 << std::endl ;
  *outputFile  << " chi2 per dof ML2 : " << m_chi2ndofML2 << std::endl ;
  *outputFile  << " t0 ML1 (ns)  " << m_t0ML1 << "    +/-    " << m_t0errML1 << std::endl ;
  *outputFile  << " t0 ML2 (ns)  " << m_t0ML2 << "    +/-    " << m_t0errML2 << std::endl ;
  *outputFile  << " tdrift ML1 (ns)  " << m_tdriftML1 << "    +/-    " << m_tdrifterrML1 << std::endl ;
  *outputFile  << " tdrift ML2 (ns)  " << m_tdriftML2 << "    +/-    " << m_tdrifterrML2 << std::endl ;
  
  *outputFile  << " Number of Dead ML : " << m_numDeadML << "  ---- list of dead/inefficient Multilayers : " << std::endl ;
  for ( int n=0 ; n< m_numDeadML ; n++ ) *outputFile  << m_listDeadML[n] << std::endl ;
  *outputFile  << " Number of Dead Mezzanine : " << m_numDeadMezz << "  ---- list of dead/inefficient Mezzanine : " << std::endl ;
  for ( int n=0 ; n< m_numDeadMezz ; n++ ) *outputFile  << m_listDeadMezz[n] << std::endl ;
  *outputFile  << " Number of Dead Tubes : " << m_numDeadTube << "  ---- list of dead/inefficient Tubes : " << std::endl ;
  for ( int n=0 ; n< m_numDeadTube ; n++ ) *outputFile  << m_listDeadTube[n] << std::endl ;
  
  *outputFile  << std::endl ;
}

void MdtDqaDb::Print() {    
  //  std::cout  << "  ==================================================== " << std::endl ;
  //  std::cout  << "  MdtDqaDb output: ---- Chamber name : " << m_chamberName << std::endl ;
  //  std::cout  << " timeFlag : " << getTimeFlag() << std::endl ;
  //  std::cout  << " deadFlag : " << getDeadFlag() << std::endl ;
  //  std::cout << " chi2 per dof ML1 : " << m_chi2ndofML1 << std::endl ;
  //  std::cout << " chi2 per dof ML2 : " << m_chi2ndofML2 << std::endl ;
  //  std::cout << " t0 ML1 (ns)  " << m_t0ML1 << "    +/-    " << m_t0errML1 << std::endl ;
  //  std::cout << " t0 ML2 (ns)  " << m_t0ML2 << "    +/-    " << m_t0errML2 << std::endl ;
  //  std::cout  << " tdrift ML1 (ns)  " << m_tdriftML1 << "    +/-    " << m_tdrifterrML1 << std::endl ;
  //  std::cout  << " tdrift ML2 (ns)  " << m_tdriftML2 << "    +/-    " << m_tdrifterrML2 << std::endl ;
  
  //  std::cout << " Number of Dead ML : " << m_numDeadML << "  ---- list of dead/inefficient Multilayers : " << std::endl ;
  //  for ( int n=0 ; n< m_numDeadML ; n++ ) std::cout << m_listDeadML[n] << std::endl ;
  //  std::cout << " Number of Dead Mezzanine : " << m_numDeadMezz << "  ---- list of dead/inefficient Mezzanine : " << std::endl ;
  //  for ( int n=0 ; n< m_numDeadMezz ; n++ ) std::cout << m_listDeadMezz[n] << std::endl ;
  //  std::cout << " Number of Dead Tubes : " << m_numDeadTube << "  ---- list of dead/inefficient Tubes : " << std::endl ;
  //  for ( int n=0 ; n< m_numDeadTube ; n++ ) std::cout << m_listDeadTube[n] << std::endl ;
  
  //  std::cout << std::endl ;
}

void MdtDqaDb::SetChamberName(std::string chamName) {
  m_chamberName = chamName; 
}

void MdtDqaDb::SetTimeFlag(int timeFlag) {
  m_timeFlag = timeFlag;
}

void MdtDqaDb::SetDeadFlag(int deadFlag) {
  m_deadFlag = deadFlag;
}

void MdtDqaDb::SetML1(float t0, float tdrift, float chi2ndof, float t0err, float tdrifterr) {
  m_t0ML1 = t0;
  m_tdriftML1 = tdrift;
  m_chi2ndofML1 = chi2ndof;
  m_t0errML1 = t0err;
  m_tdrifterrML1 = tdrifterr;
}

void MdtDqaDb::SetML2(float t0, float tdrift, float chi2ndof, float t0err, float tdrifterr) {
  m_t0ML2 = t0;
  m_tdriftML2 = tdrift;
  m_chi2ndofML2 = chi2ndof;
  m_t0errML2 = t0err;
  m_tdrifterrML2 = tdrifterr;
}

void MdtDqaDb::SetDeadML(std::vector<int> deadML) {
  m_listDeadML = deadML;
  m_numDeadML = m_listDeadML.size();
}

void MdtDqaDb::SetDeadMezz(std::vector<int> deadMezz) {
  m_listDeadMezz = deadMezz;
  m_numDeadMezz = m_listDeadMezz.size();
}

void MdtDqaDb::SetDeadTube(std::vector<int> deadTube) {
  m_listDeadTube = deadTube;
  m_numDeadTube = m_listDeadTube.size();
}

void MdtDqaDb::SetNoisyMezz(std::vector<std::pair<int,float> > noisyMezz) {
  m_listNoisyMezz = noisyMezz;
  m_numNoisyMezz = m_listNoisyMezz.size();
}

void MdtDqaDb::SetNoisyTube(std::vector<std::pair<int, float> > noisyTube) {
  m_listNoisyTube = noisyTube;
  m_numNoisyTube = m_listNoisyTube.size();
}

//Get methods
std::string MdtDqaDb::getChamberName() { return m_chamberName; }

int MdtDqaDb::getTimeFlag() {
  if(m_timeFlag == -1) { //calculate m_timeFlag if needed
    int timeFlag = 0 ;
    // HERE the LIMITS are ...CARVED OUT OF THE ROCK !
    float limit_chi2Min = 0.1 ;
    float limit_chi2Max = 10. ;
    float limit_tdriftMin = 600. ; 
    float limit_tdriftMax = 760. ;
    
    if (m_chi2ndofML1 < limit_chi2Min || m_chi2ndofML1 > limit_chi2Max  ) timeFlag++ ;
    if (m_chi2ndofML2 < limit_chi2Min || m_chi2ndofML2 > limit_chi2Max  ) timeFlag++ ;
    if (m_tdriftML1 < limit_tdriftMin || m_tdriftML1 > limit_tdriftMax  ) timeFlag++ ; // HERE also the error should be taken into account
    if (m_tdriftML2 < limit_tdriftMin || m_tdriftML2 > limit_tdriftMax  ) timeFlag++ ; // HERE also the error should be taken into account
    
    m_timeFlag = timeFlag ;
  }
  return m_timeFlag;
}  //end MdtDqaDb::getTimeFlag

int MdtDqaDb::getDeadFlag() {
  if(m_deadFlag == -1) {
    int deadFlag = 0 ;
    // HERE the LIMITS are ...CARVED OUT OF THE ROCK !
    if ( m_numDeadML > 0 || m_numDeadMezz > 0 || m_numDeadTube > 0 ) deadFlag = 1 ;
    
    m_deadFlag = deadFlag ;
  }
  return m_deadFlag ;
}

int MdtDqaDb::getNumDeadML() { return m_numDeadML; }
int MdtDqaDb::getNumDeadMezz() { return m_numDeadMezz; }
int MdtDqaDb::getNumDeadTube() { return m_numDeadTube; }
int MdtDqaDb::getNumNoisyMezz() { return m_numNoisyMezz; }
int MdtDqaDb::getNumNoisyTube() { return m_numNoisyTube; }
float MdtDqaDb::getT0ML1() { return m_t0ML1; }
float MdtDqaDb::getT0ML2() { return m_t0ML2; }
float MdtDqaDb::getTdriftML1() { return m_tdriftML1; }
float MdtDqaDb::getTdriftML2() { return m_tdriftML2; }
float MdtDqaDb::getChi2ndofML1() { return m_chi2ndofML1; }
float MdtDqaDb::getChi2ndofML2() { return m_chi2ndofML2; }
float MdtDqaDb::getT0errML1() { return m_t0errML1; }
float MdtDqaDb::getT0errML2() { return m_t0errML2; }
float MdtDqaDb::getTdrifterrML1() { return m_tdrifterrML1; }
float MdtDqaDb::getTdrifterrML2() { return m_tdrifterrML2; }
std::vector<int> MdtDqaDb::getDeadML() { return m_listDeadML; }
std::vector<int> MdtDqaDb::getDeadMezz() { return m_listDeadMezz; }
std::vector<int> MdtDqaDb::getDeadTube() { return m_listDeadTube; }
std::vector<std::pair<int,float> > MdtDqaDb::getNoisyMezz() { return m_listNoisyMezz; }
std::vector<std::pair<int,float> > MdtDqaDb::getNoisyTube() { return m_listNoisyTube; }

