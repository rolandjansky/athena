/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCRPhiCoincidenceMap.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {

 extern bool        g_DEBUGLEVEL;
 extern bool        g_USE_CONDDB;

bool TGCRPhiCoincidenceMap::test(int octantId, int moduleId, int subsector, 
				 int type, int pt, 
				 int dr, int dphi) const
{  
  // check pt range
  if (pt<0 || pt>=N_PT_THRESH) return false;
  if (type<0 || type>=N_TMap ) return false; 

  int sector=(moduleId-2)/3+octantId*3;
  int phimod2 = (moduleId==2||moduleId==5||moduleId==8)&&(sector%2==1) ? 1 : 0;
  int addr=SUBSECTORADD(subsector, moduleId, phimod2,type);

  SG::ReadCondHandle<TGCTriggerData> readHandle{m_readCondKey};
  const TGCTriggerData* readCdo{*readHandle};

  std::map<int, std::map<int, int> > readMap;

  if (g_USE_CONDDB) {
    readMap = readCdo->getReadMapBw(m_side, m_octant, pt);
  } else {
    readMap = m_mapDB[pt];
  }

  std::map<int, std::map<int, int> >::const_iterator it = readMap.find(addr);

  if (it==(readMap.end())) return false;
 
  std::map<int, int> aMap = it->second;
  std::map<int, int>::const_iterator itWindow= aMap.find( dr );

  if (itWindow==aMap.end()) return false;
 
  if ( (itWindow->second) & ( 1<<(PHIPOS(dphi,type)) ) ) return true;
  else return false;
}

TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap(const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                                             const std::string& version,
					     int   sideId, int octantId)
  :m_numberOfDR(0), m_numberOfDPhi(0),
   m_verName(version),
   m_side(sideId),
   m_octant(octantId),
   m_fullCW(false),
   m_readCondKey(readCondKey)
{
  if (!g_USE_CONDDB) {
    if (!checkVersion()){
      m_verName = "NA";
      return;
    }
    this->readMap();  // read Coincidence Map 
  
  } 
}

bool TGCRPhiCoincidenceMap::checkVersion()
{
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap");

  // select right database according to a set of thresholds
  std::string dbname="";
  dbname = "RPhiCoincidenceMap."+m_verName+"._12.db";

  //----- 
  std::string fullName;
  fullName = PathResolver::find_file( dbname.c_str(), "DATAPATH" );
  bool isFound =( fullName.length() > 0 );
  if (!isFound) {
    if (g_DEBUGLEVEL) {
      log << MSG::DEBUG 
	  << " Could not found " << dbname.c_str() 
	  << " Default set is chosen !!" << endmsg;
    }
    // default set 
    m_verName = "v000f";
    dbname = "RPhiCoincidenceMap."+m_verName+"._12.db";
    fullName = PathResolver::find_file( dbname.c_str(), "DATAPATH" );
    isFound =( fullName.length() > 0 );
  } 
  if (!isFound) {
    log << MSG::INFO  
	<< " Could not found " << dbname.c_str() << endmsg;
    return false;
  }
  
  std::ifstream file(fullName.c_str());
  std::string buf;
  
  //--- read RPhiCoincidence.XXX.db
  int maxpt[N_PT_THRESH]={0,0,0,0,0,0};

  while( getline(file,buf) ) {
    if( buf.size()>3 && buf.substr(0,1)=="#" ) {	
      std::istringstream ist(buf.substr(1));
      int level = 0 ;
      int ptv=0;
      ist >> level >> ptv;
      if (( level >0 ) && (level <= N_PT_THRESH)) maxpt[level-1]=ptv;
    }
  }
  file.close();

  // use full CW (i.e. different maps for each octant and side)
  m_fullCW = (m_verName == "v0016" || m_verName == "v0017");

  ///////////  
  log << MSG::INFO 
      << " TGC Big Wheel CW version of " << m_verName << " is selected " << endmsg;
  for(int i=0; i<N_PT_THRESH; i++) {
    log << MSG::VERBOSE 
	<< "TGC Pt_Thr: " << std::setw(2) << i+1
	<< "  pt(used)="  << std::setw(3) << maxpt[i] 
      	<< endmsg;
  }
  return true;
}


TGCRPhiCoincidenceMap::~TGCRPhiCoincidenceMap()
{
}

TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap(const TGCRPhiCoincidenceMap& right)
  : m_readCondKey(right.m_readCondKey)
{
  m_numberOfDR=right.m_numberOfDR;
  m_numberOfDPhi=right.m_numberOfDPhi;
  m_verName=right.m_verName;
  m_side=right.m_side;
  m_octant=right.m_octant;
  m_fullCW=right.m_fullCW;
  if (m_verName!="NA") this->readMap();  // read Coincidence Map 
}


TGCRPhiCoincidenceMap& TGCRPhiCoincidenceMap::operator=(const TGCRPhiCoincidenceMap& right)
{
   if (this != &right) {
    m_numberOfDR=right.m_numberOfDR;
    m_numberOfDPhi=right.m_numberOfDPhi;
    m_verName=right.m_verName;
    m_side=right.m_side;
    m_octant=right.m_octant;
    m_fullCW=right.m_fullCW;
    if (m_verName!="NA") this->readMap();  // read Coincidence Map 
   }

  return *this;
}

bool TGCRPhiCoincidenceMap::readMap() 
{
  const int NumberOfModuleType=12;
  const int ModuleNumber[NumberOfModuleType]  =
    {  0,  1,   2,   2,  3,  4,   5,   5,  6,  7,   8,  8 };
  const std::string ModuleName[NumberOfModuleType]=
    {"0","1","2a","2b","3","4","5a","5b","6","7","8a","8b"};
  const std::string SideName[NumberOfSide] = {"A","C"};
  const std::string OctantName[NumberOfOctant] = 
    {  "0", "1", "2", "3", "4", "5", "6", "7"};

  
  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap");

  // initialize
  enum{BufferSize=1024};
  char buf[BufferSize];
  std::string fn, fullName, tag;
  int ssId,ptLevel,bit,mod;

  // loop over all files...
  for(int iModule=0; iModule<NumberOfModuleType; iModule+=1) {
    int phimod2=ModuleName[iModule].find("b")!=std::string::npos ? 1 : 0;
    std::ostringstream modName;
    std::string fn = "RPhiCoincidenceMap.mod" 
                      + ModuleName[iModule] + "." + m_verName +"._12.db";
    if (m_fullCW) {
      if ( (m_side>=0) && (m_side<NumberOfSide) && 
	   (m_octant>=0) && (m_octant<NumberOfOctant)) {
	fn = "RPhiCoincidenceMap." 
	              + SideName[m_side] + OctantName[m_octant]  
                      + ".mod" + ModuleName[iModule] 
                      + "." + m_verName +"._12.db";
      } 
    }

    int type = -1;
    int lDR, hDR, lDPhi, hDPhi;

    fullName = PathResolver::find_file( fn.c_str(), "DATAPATH" );
    if( fullName.length() == 0 ) { 
      log << MSG::ERROR 
	  << " Could not found " 
	  << fn.c_str() << endmsg;
      return false ;  
    } 

    std::ifstream file(fullName.c_str(),std::ios::in);    
    while(file.getline(buf,BufferSize)){
      std::istringstream header(buf);
      header>>tag;
      if(tag=="#"){ // read header part.     
	header>>ptLevel>>ssId>>mod>>lDR>>hDR>>lDPhi>>hDPhi;
	type = getTYPE( lDR, hDR, lDPhi, hDPhi );
	// check moduleNumber and ptLevel
	if(mod!=ModuleNumber[iModule] || ptLevel>N_PT_THRESH || type<0 ) {
	  log << MSG::WARNING 
	      << " illegal parameter in database header : "
	      << header.str()
	      << " in file " << fn 
	      << endmsg;
	  break;
	}

	// get window data
	file.getline(buf,BufferSize);
	std::istringstream cont(buf);
	std::map<int, int> aWindow;
	for(int ir=0; ir<=hDR-DR_offset; ir++) {
	  cont>>bit;
	  if (bit==0) continue; // none of window is opened in this dR
	  aWindow[ir+DR_offset] = bit;
	}
	// Warning : no window 
	if (aWindow.size()==0) {
	  if (g_DEBUGLEVEL) {
	    log << MSG::DEBUG
		<< " No window is opened for (ptLevel,ssId,mod) = (" 
		<< ptLevel << ", " << ssId << ", " << mod << ")" 
		<<endmsg;
	  }
	}
	int addr = SUBSECTORADD(ssId,mod,phimod2,type);
	if (m_mapDB[ptLevel-1].find(addr)!=m_mapDB[ptLevel-1].end()) {
	  if (g_DEBUGLEVEL) {
	    log << MSG::DEBUG
		<< "This subsector was already reserved." 
		<< endmsg;
	  }
	} else {
	  m_mapDB[ptLevel-1][addr]=aWindow;
	}
      }
    }
  }

  return true;
}



} //end of namespace bracket
