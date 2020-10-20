/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

#include "TrigT1TGC/TGCRPhiCoincidenceMap.h"
#include "TrigT1TGC/TGCDatabaseManager.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/ReadCondHandle.h"
#include "MuonCondSvc/TGCTriggerData.h"

namespace LVL1TGCTrigger {

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

  if (tgcArgs()->USE_CONDDB()){
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




int TGCRPhiCoincidenceMap::test_Run3(int octantId, int moduleId, int subsector, 
                                     int type, int dr, int dphi) const
{  
  // check pt range
  if (type<0 || type>=N_TMap ) return false; 

  int sector=(moduleId-2)/3+octantId*3;
  int phimod2 = (moduleId==2||moduleId==5||moduleId==8)&&(sector%2==1) ? 1 : 0;
  int addr=SUBSECTORADD(subsector, moduleId, phimod2,type);

  std::map<int, std::map<int, std::map<int, char> > >::const_iterator it=m_mapDB_Run3.find(addr);
  if (it==m_mapDB_Run3.end()) return false;

  std::map<int, std::map<int, char> > mapR = it->second;
  std::map<int, std::map<int, char> >::const_iterator itR=mapR.find(dr);
  if (itR==mapR.end()) return false;

  std::map<int, char> mapPhi = itR->second;
  std::map<int, char>::const_iterator itPhi=mapPhi.find(dphi);
  if (itPhi==mapPhi.end()) return false;

  char pT_char=itPhi->second;
  int pT_int=m_pTdef.find(pT_char)->second;

  return  pT_int;
}





TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap(TGCArguments* tgcargs,
					     const SG::ReadCondHandleKey<TGCTriggerData>& readCondKey,
                                             const std::string& version,
					     int   sideId, int octantId)
  :m_numberOfDR(0), m_numberOfDPhi(0),
   m_verName(version),
   m_side(sideId),
   m_octant(octantId),
   m_fullCW(false),
   m_tgcArgs(tgcargs),
   m_readCondKey(readCondKey)
{
  if (!tgcArgs()->USE_CONDDB()) {
    if(!tgcArgs()->useRun3Config()){
      if (!checkVersion()){
      m_verName = "NA";
      return;
      }
      this->readMap();  // read Coincidence Map for Run2 (6 thresholds)
    }
    else{
      this -> readMap_Run3();// read Coincidence Map for Run3 (15 thresholds)
    }

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
    if (tgcArgs()->MSGLEVEL() <= MSG::DEBUG) {
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
  if(m_verName!="NA" && !tgcArgs()->useRun3Config()) this->readMap();  // read Coincidence Map for Run2 (6 thresholds)
  if(m_verName!="NA" &&  tgcArgs()->useRun3Config()){this -> readMap_Run3();}// read Coincidence Map for Run3 (15 thresholds)
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
    if(m_verName!="NA" && !tgcArgs()->useRun3Config()) this->readMap();  // read Coincidence Map for Run2 (6 thresholds)
    if(m_verName!="NA" &&  tgcArgs()->useRun3Config()){ this -> readMap_Run3();} // read Coincidence Map for Run3 (15 thresholds)
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
	  if (tgcArgs()->MSGLEVEL() <= MSG::DEBUG) {
	    log << MSG::DEBUG
		<< " No window is opened for (ptLevel,ssId,mod) = (" 
		<< ptLevel << ", " << ssId << ", " << mod << ")" 
		<<endmsg;
	  }
	}
	int addr = SUBSECTORADD(ssId,mod,phimod2,type);
	if (m_mapDB[ptLevel-1].find(addr)!=m_mapDB[ptLevel-1].end()) {
	  if (tgcArgs()->MSGLEVEL() <= MSG::DEBUG) {
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


bool TGCRPhiCoincidenceMap::readMap_Run3() 
{
  const int moduleNumber[NumberOfModuleType]  =
    {  0,  1,   2,   2,  3,  4,   5,   5,  6,  7,   8,  8 };
  const std::string moduleName[NumberOfModuleType]=
    {"0a","1a","2a","2b","3a","4a","5a","5b","6a","7a","8a","8b"};
  const std::string sideName[NumberOfSide] = {"a","c"};
  const std::string octantName[NumberOfOctant] =
    {  "0", "1", "2", "3", "4", "5", "6", "7"};
  const std::string coincidenceTypeName[NumberOfCoincidenceType] = {"HH","HL","LH","LL"};


  IMessageSvc* msgSvc = 0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  if (svcLocator->service("MessageSvc", msgSvc) == StatusCode::FAILURE) {
    return false;
  }
  MsgStream log(msgSvc, "TGCRPhiCoincidenceMap::TGCRPhiCoincidenceMap");


  // initialize
  std::string buf;
  std::string fn, fullName, tag;
  int ssId;
  char delimiter = '\n';

  // loop over all files...
  for(int iModule=0; iModule<NumberOfModuleType; iModule+=1) {
    for(int iCoinType=0; iCoinType!=NumberOfCoincidenceType; iCoinType++){
      int phimod2=moduleName[iModule].find("b")!=std::string::npos ? 1 : 0;
      std::string fn;
      if (m_fullCW) {
        fn = "/BW/cm_"+ sideName[m_side]+octantName[m_octant]+moduleName[iModule]+coincidenceTypeName[iCoinType]+"_"+m_verName+".db";}
      else{
        fn = "/BW/cm_"+ sideName[m_side]+moduleName[iModule]+coincidenceTypeName[iCoinType]+"_Octant_"+m_verName+".db";
      }
      bool Forward_type1=(moduleName[iModule]=="2b"||moduleName[iModule]=="5a"||moduleName[iModule]=="8b");
      bool Forward_type2=(moduleName[iModule]=="2a"||moduleName[iModule]=="5b"||moduleName[iModule]=="8a");
      if(m_octant%2==0 && Forward_type1){continue;}
      if(m_octant%2==1 && Forward_type2){continue;}
      int type = -1;
      int lDR, hDR, lDPhi, hDPhi;

      fullName = PathResolver::FindCalibDirectory("dev")+"/TrigT1TGC"+fn;
      if( fullName.length() == 0 ) { 
        log << MSG::ERROR
            << " Could not found "
            << fn.c_str()
            <<endmsg;
        continue;  
      } 

      std::ifstream file(fullName.c_str(),std::ios::in);   
      if(!file){
        log << MSG::ERROR
            << " Could not found "
            << fullName.c_str()
            <<endmsg;
        continue; 
      } 
      while(getline(file,buf,delimiter)){
        std::istringstream header(buf);
        header>>tag;
        if(tag=="#") { // read header part.     
          header>>ssId>>lDR>>hDR>>lDPhi>>hDPhi;
          type = getTYPE( lDR, hDR, lDPhi, hDPhi );
          // check moduleNumber and ptLevel
          if( type<0 ) {
            log << MSG::WARNING
                << " illegal parameter in database header : "
                <<  header.str()
                <<" in file "
                << fn
                <<endmsg;
            break;
          }

          // get window data
          std::map<int, std::map<int, char> >  bWindow;//<R,<~>>
          char pT;
          for(int ir=0; ir<=hDR-lDR; ir++) {
            getline(file,buf,delimiter);
            std::map<int, char> aWindow;//<Phi,pT>
            for(int iphi=0; iphi<=hDPhi-lDPhi; iphi++){
              pT = buf[iphi];
              if (pT=='X'){continue;} // none of window is opened in this dR
              aWindow[iphi+DPhi_offset] = pT;
            }
            // Warning : no window 
            if (aWindow.size()==0) {
              log << MSG::DEBUG
                  << " No window is opened for (ptLevel,ssId,mod) = ("
                  <<  ssId
                  <<" ) "
                  <<endmsg;
            } else {
              bWindow[ir+DR_offset]=aWindow;
            }
          }

          int addr = SUBSECTORADD(ssId,moduleNumber[iModule],phimod2,type);
          if (m_mapDB_Run3.find(addr)!=m_mapDB_Run3.end()) {
            log << MSG::DEBUG
                << " This subsector was already reserved."
                <<endmsg;
          } else {
            m_mapDB_Run3[addr]=bWindow;
          }
        }
      }
      
    }
  }
  return true;

}


} //end of namespace bracket
