/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
         TGCcablingSvc.h

    Author  : Tadashi Maeno
               H.Kurashige       Sep. 2007
    Email   : Hisaya Kurashige@cern.ch
    Description : online-offline ID mapper for TGC

***************************************************************************/

#include <cmath>
#include <fstream>

#include "TGCcabling/TGCcablingSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "PathResolver/PathResolver.h"

TGCcablingSvc::TGCcablingSvc (const std::string& name, ISvcLocator* svc)
  :  ITGCcablingSvc( name, svc ), m_idHelper(0), m_cabling(0) 
{
  declareProperty("AsideId",m_AsideId=103);
  declareProperty("CsideId",m_CsideId=104);
  declareProperty("rodId",  m_rodId);    //obsolte
  declareProperty("databaseASDToPP", m_databaseASDToPP="ASD2PP.db");
  declareProperty("databaseInPP",    m_databaseInPP="PP.db");
  declareProperty("databasePPToSL",  m_databasePPToSL="PP2SL.db");
  declareProperty("databaseSLBToROD",m_databaseSLBToROD="SLB2ROD.db");
}

StatusCode  TGCcablingSvc::queryInterface(const InterfaceID& riid,void** ppvIF)
{
  if (IID_TGCcablingSvc == riid)
    *ppvIF = dynamic_cast<TGCcablingSvc*>(this);
  else 
    return Service::queryInterface(riid, ppvIF);
  
  addRef();
  return StatusCode::SUCCESS;
}

void TGCcablingSvc::getReadoutIDRanges( int& maxRodId,
                                        int& maxSswId,
                                        int& maxSbloc,
                                        int& minChannelId,
                                        int& maxChannelId) const
{
  maxRodId = LVL1TGCCabling8::TGCCabling::MAXRODID;
  maxSswId = LVL1TGCCabling8::TGCCabling::MAXSSWID;
  maxSbloc = LVL1TGCCabling8::TGCCabling::MAXSBLOC;
  minChannelId = LVL1TGCCabling8::TGCCabling::MINCHANNELID;
  maxChannelId = LVL1TGCCabling8::TGCCabling::MAXCHANNELID;
}

StatusCode TGCcablingSvc::initialize (void)
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "for 1/8 sector  initialize" << endmsg;

  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;

  // TgcIdHelper
  StoreGateSvc* detStore=0;
  sc = serviceLocator()->service("DetectorStore", detStore);

  if (sc.isFailure()){
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return sc;
  }

  sc = detStore->retrieve( m_idHelper, "TGCIDHELPER");
  if (sc.isFailure()){
    log << MSG::FATAL << "Could not get TgcIdHelper !" << endmsg;
    return sc;
  }

  // private databases
  std::string dbASDToPP = PathResolver::find_file (m_databaseASDToPP, "DATAPATH");
  log << MSG::DEBUG << "found  " << dbASDToPP << endmsg;
  std::ifstream inASDToPP;
  if (dbASDToPP != "") {
    inASDToPP.open( dbASDToPP.c_str());
  } else {
    log << MSG::FATAL << "Could not find input file " << m_databaseASDToPP <<
      endmsg;
    return StatusCode::FAILURE;
  }
  if (inASDToPP.bad()) {
    log << MSG::FATAL << "Could not open file " << dbASDToPP << endmsg;
    return StatusCode::FAILURE;
  }



  std::string dbInPP = PathResolver::find_file (m_databaseInPP, "DATAPATH");
  log << MSG::DEBUG << "found  " << dbInPP << endmsg;
  std::ifstream inInPP;
  if (dbInPP != "") {
    inInPP.open( dbInPP.c_str());
  } else {
    log << MSG::FATAL << "Could not find input file " << m_databaseInPP <<
      endmsg;
    return StatusCode::FAILURE;
  }
  if (inInPP.bad()) {
    log << MSG::FATAL << "Could not open file " << dbInPP << endmsg;
    return StatusCode::FAILURE;
  }


  std::string dbPPToSL = PathResolver::find_file (m_databasePPToSL, "DATAPATH");
  log << MSG::DEBUG << "found  " << dbPPToSL << endmsg;
  std::ifstream inPPToSL;
  if (dbPPToSL != "") {
    inPPToSL.open( dbPPToSL.c_str());
  } else {
    log << MSG::FATAL << "Could not find input file " << m_databasePPToSL <<
      endmsg;
    return StatusCode::FAILURE;
  } 
  if (inPPToSL.bad()) {
    log << MSG::FATAL << "Could not open file " << dbPPToSL << endmsg;
    return StatusCode::FAILURE;
  }


  std::string dbSLBToROD = PathResolver::find_file (m_databaseSLBToROD, "DATAPATH");
  log << MSG::DEBUG << "found  " << dbSLBToROD << endmsg;
  std::ifstream inSLBToROD;
  if (dbSLBToROD != "") {
    inSLBToROD.open( dbSLBToROD.c_str());
  } else {
    log << MSG::FATAL << "Could not find input file " << m_databaseSLBToROD <<
      endmsg;
    return StatusCode::FAILURE;
  }
  if (inSLBToROD.bad()) {
    log << MSG::FATAL << "Could not open file " << dbSLBToROD << endmsg;
    return StatusCode::FAILURE;
  }



  // instantiate TGC cabling manager
  m_cabling = new LVL1TGCCabling8::TGCCabling(dbASDToPP,
					      dbInPP,
					      dbPPToSL,
					      dbSLBToROD);

  return StatusCode::SUCCESS;
}  
  
StatusCode TGCcablingSvc::finalize (void)
{
  delete m_cabling;

  return StatusCode::SUCCESS;
}

// high pt coincidence IDs -> offline IDs
bool TGCcablingSvc::getOfflineIDfromHighPtID(Identifier & offlineID,
					     const int subDetectorID,
					     const int rodID,
					     const int sectorInOctant,
					     const bool isStrip,
					     const bool isForward,
					     const int hpb,
					     const int chip,
					     const int hitID,
					     const int pos) const
{
  // all input is derived from TgcRawData
  int sswID = -1;
  int slbID = -1;
  int channelID = -1;

  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;

  // SignalType, RegionType
  TGCIdBase::SignalType signalType = (isStrip)? 
    TGCIdBase::Strip : TGCIdBase::Wire;
  TGCIdBase::RegionType regionType = (isForward)? 
    TGCIdBase::Forward : TGCIdBase::Endcap;

  TGCIdBase::ModuleType moduleType = TGCIdBase::NoModuleType;
  if(signalType==TGCIdBase::Wire){ 
    moduleType=TGCIdBase::WD;
  } else {
    moduleType=TGCIdBase::SD;
  }

  // Get ReadoutID for pivot plane 
  bool status = m_cabling->getReadoutFromHighPtID(sideType,
						  rodID,
						  sswID,
						  slbID,
						  channelID,
						  signalType,
						  regionType,
						  sectorInOctant,
						  hpb,
						  chip,
						  hitID,
						  pos,
						  moduleType,
						  false);

  if(!status) return false;    
  
  // get OfflineID for pivot plane
  return getOfflineIDfromReadoutID(offlineID,
				   subDetectorID,
				   rodID,
				   sswID,
				   slbID,
				   channelID);
  
}

// offline IDs -> high pt coincidence IDs
bool TGCcablingSvc::getHighPtIDfromOfflineID(const Identifier & offlineID,
					     int & subDetectorID,
					     int & rodID,
					     int & sectorInOctant,
					     bool & isStrip,
					     bool & isForward,
					     int & hpb,
					     int & chip,
					     int & hitID,
					     int & pos) const 
{
  
  int sswID = -1;
  int slbID = -1;
  int channelID = -1;   

  bool status = getReadoutIDfromOfflineID(offlineID,
					  subDetectorID,
					  rodID,
					  sswID,
					  slbID,
					  channelID);
  if(!status) return false;    
  
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;

    
  TGCIdBase::SignalType signalType;
  TGCIdBase::RegionType regionType;
  status = m_cabling->getHighPtIDFromReadout(sideType,
					     rodID,
					     sswID,
					     slbID,
					     channelID,
					     signalType,
					     regionType,
					     sectorInOctant,
					     hpb,
					     chip,
					     hitID,
					     pos);
  if(!status) return false;
  
  isStrip = (signalType==TGCIdBase::Strip); 
  isForward = (regionType==TGCIdBase::Forward); 

  return true;
}


// low pt coincidence IDs -> offline IDs
bool TGCcablingSvc::getOfflineIDfromLowPtCoincidenceID(Identifier & offlineID,
						       const int subDetectorID,
						       const int rodID,
						       const int sswID,
						       const int slbID,
						       const int block,
						       const int pos,
						       bool middle) const
{
  int channelID = -1;

  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;

  bool status = m_cabling->getReadoutFromLowPtCoincidence(sideType,
							  rodID,
							  sswID,
							  slbID,
							  channelID,
							  block,
							  pos,
							  middle);
  if(!status) return false;
  
  return getOfflineIDfromReadoutID(offlineID,
				   subDetectorID,
				   rodID,
				   sswID,
				   slbID,
				   channelID);
}

// offline IDs -> low pt coincidence IDs
bool TGCcablingSvc::getLowPtCoincidenceIDfromOfflineID(const Identifier & offlineID,
						       int & subDetectorID,
						       int & rodID,
						       int & sswID,
						       int & slbID,
						       int & block,
						       int & pos, 
						       bool middle) const
{
  int channelID = -1;   

  bool status = getReadoutIDfromOfflineID(offlineID,
					  subDetectorID,
					  rodID,
					  sswID,
					  slbID,
					  channelID);
  if(!status) return false;    
  
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;

  return m_cabling->getLowPtCoincidenceFromReadout(sideType,
						   rodID,
						   sswID,
						   slbID,
						   channelID,
						   block,
						   pos,
						   middle);
}

  
// Readout ID is ored
bool TGCcablingSvc::isOredChannel(const int subDetectorID,
				  const int rodID,
				  const int sswID,
				  const int slbID,
				  const int channelID) const
{
  Identifier id;
  return getOfflineIDfromReadoutID(id,
				   subDetectorID,
				   rodID,
				   sswID,
				   slbID,
				   channelID,
				   true);
}


// Offline ID has adjacent Readout ID
bool TGCcablingSvc::hasAdjacentChannel(const Identifier & offlineID) const
{
  int subDetectorID;
  int rodID;
  int sswID;
  int slbID;
  int channelID;
  return getReadoutIDfromOfflineID(offlineID,
				   subDetectorID,
				   rodID,
				   sswID,
				   slbID,
				   channelID,
				   true);
}


// Online ID has adjacent Readout ID
bool TGCcablingSvc::hasAdjacentChannel(const int subsystemNumber,
				       const int octantNumber,
				       const int moduleNumber,
				       const int layerNumber,
				       const int rNumber,
				       const int wireOrStrip,
				       const int channelNumber) const
{
  int subDetectorID;
  int rodID;
  int sswID;
  int slbID;
  int channelID;
  return getReadoutIDfromOnlineID(subDetectorID,
				  rodID,
				  sswID,
				  slbID,
				  channelID,
				  subsystemNumber,
				  octantNumber,
				  moduleNumber,
				  layerNumber,
				  rNumber,
				  wireOrStrip,
				  channelNumber,
				  true);
}


// readout IDs -> offline IDs
bool TGCcablingSvc::getOfflineIDfromReadoutID(Identifier & offlineID,
					      const int subDetectorID,
					      const int rodID,
					      const int sswID,
					      const int slbID,
					      const int channelID,
					      bool orChannel) const
{
  // This function used to memoize its result, in a non-thread-safe manner.
  // Removed for now, but we can reimplement it if it makes a difference.

  int subsystemNumber;
  int octantNumber;
  int moduleNumber;
  int layerNumber;
  int rNumber;
  int wireOrStrip;
  int channelNumber;
  
  // ReadoutID -> OnlineID
  bool status = getOnlineIDfromReadoutID(subDetectorID,
					 rodID,
					 sswID,
					 slbID,
					 channelID,
					 subsystemNumber,
					 octantNumber,
					 moduleNumber,
					 layerNumber,
					 rNumber,
					 wireOrStrip,
					 channelNumber,
					 orChannel);
  if(!status) {
    return status;
  }
  
  // OnlineID -> OfflineID
  status = getOfflineIDfromOnlineID(offlineID,
				    subsystemNumber,
				    octantNumber,
				    moduleNumber,
				    layerNumber,
				    rNumber,
				    wireOrStrip,
				    channelNumber);
  
  return status;
}


// offline IDs -> readout IDs
bool TGCcablingSvc::getReadoutIDfromOfflineID(const Identifier & offlineID,
					      int & subDetectorID,
					      int & rodID,
					      int & sswID,
					      int & slbID,
					      int & channelID,
					      bool adChannel) const
{
  int subsystemNumber;
  int octantNumber;
  int moduleNumber;
  int layerNumber;
  int rNumber;
  int wireOrStrip;
  int channelNumber;

  MsgStream log( msgSvc(), name() );

  // OfflineID -> OnlineID
  bool status = getOnlineIDfromOfflineID(offlineID,
					 subsystemNumber,
					 octantNumber,
					 moduleNumber,
					 layerNumber,
					 rNumber,
					 wireOrStrip,
					 channelNumber);
  if (!status) {
    log << MSG::DEBUG 
        << " Fail to getOnlineIDfromOfflineID " << endmsg;
  }
  if(!status) return status; 
  log << MSG::DEBUG
      << " side=" << subsystemNumber << " octant=" << octantNumber 
      << " module=" << moduleNumber << " layer=" << layerNumber
      << " chamber=" << rNumber << " w/s=" << wireOrStrip
      << " channel=" << channelNumber  << endmsg;


  // OnlineID -> ReadoutID
  status = getReadoutIDfromOnlineID(subDetectorID,
				    rodID,
				    sswID,
				    slbID,
				    channelID,
				    subsystemNumber,
				    octantNumber,
				    moduleNumber,
				    layerNumber,
				    rNumber,
				    wireOrStrip,
				    channelNumber,
				    adChannel);
  if (!status) {
    log << MSG::DEBUG
        << " Fail to getReadoutIDfromOnlineID" << endmsg;
    log << MSG::DEBUG
        << " side=" << subsystemNumber << " octant=" << octantNumber 
        << " module=" << moduleNumber << " layer=" << layerNumber
        << " chamber=" << rNumber << " w/s=" << wireOrStrip
        << " channel=" << channelNumber 
	<< " adjacent=" << adChannel << endmsg;
  } else {
    log << MSG::DEBUG
        << " SUCCESS  getReadoutIDfromOnlineID" << endmsg;
    log << MSG::DEBUG
        << " side=" << subsystemNumber << " octant=" << octantNumber 
        << " module=" << moduleNumber << " layer=" << layerNumber
        << " chamber=" << rNumber << " w/s=" << wireOrStrip
        << " channel=" << channelNumber 
	<< " adjacent=" << adChannel << endmsg;
    log << MSG::DEBUG
        << " subDetectorID" << subDetectorID 
        << " rodID=" << rodID 
        << " sswID=" << sswID
        << " channelID=" << channelID  <<  endmsg;
  }

  return status;

}


// offline ID -> online IDs
bool TGCcablingSvc::getOnlineIDfromOfflineID(const Identifier & offlineId,
					     int & subSystemNumber,
					     int & octantNumber,
					     int & moduleNumber,
					     int & layerNumber,
					     int & rNumber,
					     int & wireOrStrip,
					     int & channelNumber) const
{
  // get station name in string format : T1F,T1E,T2F...
  const int iStation  =  m_idHelper->stationName(offlineId);
  const std::string stationName_str = m_idHelper->stationNameString(iStation);

  // eta and phi
  int iEta = m_idHelper->stationEta(offlineId);
  int iPhi = m_idHelper->stationPhi(offlineId);
       
  // station type : ex. T1E -> 1, T2F->2
  std::string station_str = stationName_str.substr(1, 1);
  const int stationType = atoi(station_str.c_str());
  // check station Number
  if ((stationType<1) || (stationType>4) ) return false;

  // forward/endcap
  enum {FORWARD, ENDCAP};
  const int regionType  = m_idHelper->isForward(offlineId) ? FORWARD : ENDCAP;
                                          
  // octant index and module index
  int sector;
  if ((regionType==ENDCAP) && (stationType!=4)) {
    sector = (iPhi+1)%48;
  } else {
    sector = iPhi%24;
  }
  const int max_module = (regionType==FORWARD||stationType==4) ? 3 : 6;
  const int iOctant = static_cast<int>(sector/max_module);
  const int iModule = sector % max_module;
     
  // R index
  const int rIndex = std::abs(iEta);

  // Gas gap
  const int iGasGap = m_idHelper->gasGap(offlineId);

  // convert to ASD-Out index

  subSystemNumber = (iEta > 0) ? 1 : -1;

  octantNumber = iOctant;
  
  // module# convention
  //         <---- phi ----      
  //
  //     7 6 4 3 1 0        11 10  9
  //      8   5   2         14 13 12
  // 
  //     [M1,M2, M3]         [EI/FI]  
  
  // module mapping        N/A  A   B   C  D  E  F
  const int modmapE[7]  = { -1, 0,  1,  3, 4, 6, 7 };
  const int modmapF[4]  = { -1, 2,  5,  8  };
  const int modmapEI[4] = { -1, 9,  10, 11 };
  const int modmapFI[4] = { -1, 12, 13, 14 };

  if (stationType != 4) // T1, T2, T3
    {  
      if(regionType==ENDCAP) 
	{
	  moduleNumber = modmapE[iModule+1];
	  rNumber = 5-rIndex;
	} 
      else
	{
	  moduleNumber = modmapF[iModule+1];
	  rNumber = rIndex-1;
	}
    } 
  else // TI
    {
      if(regionType==ENDCAP) 
	{
	  moduleNumber = modmapEI[iModule+1];
	  rNumber = rIndex-1;  // 0-offset
	} 
      else
	{
	  moduleNumber = modmapFI[iModule+1];
	  rNumber = rIndex-1;
	}
    }
  
  // layer# 
  // 0, (1), 2, ... 5, 6 (pivot) / 7, 8 (EI/FI)
  //                        N/A  T1  T2  T3  TI
  const int lyr_offset[5]= { 0,  -1,  2,  4, 6 };
  layerNumber = iGasGap + lyr_offset[stationType];

  // wire (0) or strip (1)
  wireOrStrip = m_idHelper->isStrip(offlineId);


  // Offline ID channel
  const int channel = m_idHelper->channel(offlineId);

  // Offline ID cahnnel -> Online ID channel

  // SideType
  TGCIdBase::SideType sideType = 
    TGCIdBase::NoSideType;
  if(subSystemNumber==1)  sideType = TGCIdBase::Aside;
  if(subSystemNumber==-1) sideType = TGCIdBase::Cside;

  // SignalType
  TGCIdBase::SignalType signalType = 
    TGCIdBase::NoSignalType;
  if(wireOrStrip==0) signalType = TGCIdBase::Wire;
  if(wireOrStrip==1) signalType = TGCIdBase::Strip;

  // RegionType
  TGCIdBase::RegionType region = 
    TGCIdBase::NoRegionType;
  if(regionType==FORWARD) region = TGCIdBase::Forward;
  if(regionType==ENDCAP)  region = TGCIdBase::Endcap;


  // ASDIn
  LVL1TGCCabling8::TGCChannelASDIn asdin(sideType,
					 signalType,
					 region,
					 iPhi,
					 layerNumber,
					 rIndex,
					 channel);
  if(!asdin.isValid()) return false;
  
  // offline Id -> online Id
  LVL1TGCCabling8::TGCChannelId* asdout =
    m_cabling->getChannel(&asdin,
			  LVL1TGCCabling8::TGCChannelId::ASDOut,
			  false);
  if(asdout==0) return false;
  if(!asdout->isValid()){
    delete asdout;
    return false;
  }
  channelNumber = asdout->getChannel();
  delete asdout;

  return true;
}
      
// online IDs -> offline ID
bool TGCcablingSvc::getOfflineIDfromOnlineID(Identifier & offlineId,
					     const int subSystemNumber,
					     const int octantNumber,
					     const int moduleNumber,
					     const int layerNumber,
					     const int rNumber,
					     const int wireOrStrip,
					     const int channelNumber) const
{
  // SideType
  TGCIdBase::SideType sideType = 
    TGCIdBase::NoSideType;
  if(subSystemNumber==1)  sideType = TGCIdBase::Aside;
  if(subSystemNumber==-1) sideType = TGCIdBase::Cside;

  // SignalType
  TGCIdBase::SignalType signalType = 
    TGCIdBase::NoSignalType;
  if(wireOrStrip==0) signalType = TGCIdBase::Wire;
  if(wireOrStrip==1) signalType = TGCIdBase::Strip;


  // ASDOut
  LVL1TGCCabling8::TGCChannelASDOut asdout(sideType,
					   signalType,
					   octantNumber,
					   moduleNumber,
					   layerNumber,
					   rNumber,
					   channelNumber);  
  if(!asdout.isValid()) return false;

  // online Id -> offline Id
  LVL1TGCCabling8::TGCChannelId* asdin =
    m_cabling->getChannel(&asdout,
			  LVL1TGCCabling8::TGCChannelId::ASDIn,
			  false);
  if(asdin==0) return false;
  if(!asdin->isValid()){
    delete asdin;
    return false;
  }  
  // build identifier
  std::string stationNameStr;
  switch(asdin->getStation()){
  case 0:
    stationNameStr = (asdin->isForward())? "T1F" : "T1E";
    break;
  case 1:
    stationNameStr = (asdin->isForward())? "T2F" : "T2E";
    break;
  case 2:
    stationNameStr = (asdin->isForward())? "T3F" : "T3E";
    break;
  case 3:
    stationNameStr = (asdin->isForward())? "T4F" : "T4E";
    break;
  default:
    return false;
  }
  int stationEta = asdin->getChamber();
  if(asdin->getSideType()==TGCIdBase::Cside)
    stationEta *= -1;
  int stationPhi = asdin->getSector();
  int gasGap = asdin->getGasGap();
  int isStrip = (asdin->isStrip())? 1 : 0;
  int channel = asdin->getChannel();

  offlineId =  m_idHelper->channelID(stationNameStr,
				     stationEta,
				     stationPhi,
				     gasGap,
				     isStrip,
				     channel);
  
  delete asdin;
  return true;
}

// readout IDs -> online IDs
bool TGCcablingSvc::getOnlineIDfromReadoutID(const int subDetectorID,
					     const int rodID,
					     const int sswID,
					     const int slbID,
					     const int channelID,
					     int & subsystemNumber,
					     int & octantNumber,
					     int & moduleNumber,
					     int & layerNumber,
					     int & rNumber,
					     int & wireOrStrip,
					     int & channelNumber,
					     bool orChannel) const
{
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;

  // readout channel -> chamber channel
  LVL1TGCCabling8::TGCChannelId* asdout =
    m_cabling->getASDOutFromReadout(sideType,
				    rodID,
				    sswID,
				    slbID,
				    channelID,
				    orChannel);
  if(asdout==0) return false;
  if(!asdout->isValid()){
    delete asdout;
    return false;
  }
  // SubsystemNumber
  subsystemNumber = (asdout->isAside())? 1 : -1;

  // OctantNumber
  octantNumber = asdout->getOctant();

  // RNumber
  rNumber = asdout->getChamber();

  // ModuleNumber
  moduleNumber = asdout->getSectorModule();

  // LayerNumber
  layerNumber = asdout->getLayer();

  // WireOrStrip
  wireOrStrip = (asdout->isStrip())? 1 : 0;

  // ChannelNumber
  channelNumber = asdout->getChannel();

  delete asdout;
  return true;
}

// online IDs -> readout IDs
bool TGCcablingSvc::getReadoutIDfromOnlineID(int & subDetectorID,
					     int & rodID,
					     int & sswID,
					     int & slbID,
					     int & channelID,
					     const int subsystemNumber,
					     const int octantNumber,
					     const int moduleNumber,
					     const int layerNumber,
					     const int rNumber,
					     const int wireOrStrip,
					     const int channelNumber,
					     bool adChannel) const
{
  MsgStream log( msgSvc(), name() );

  // SideType
  TGCIdBase::SideType sideType = 
    TGCIdBase::NoSideType;
  if(subsystemNumber==1)  sideType = TGCIdBase::Aside;
  if(subsystemNumber==-1) sideType = TGCIdBase::Cside;

  // SignalType
  TGCIdBase::SignalType signalType = 
    TGCIdBase::NoSignalType;
  if(wireOrStrip==0) signalType = TGCIdBase::Wire;
  if(wireOrStrip==1) signalType = TGCIdBase::Strip;

  // ASDOut
  LVL1TGCCabling8::TGCChannelASDOut asdout(sideType,
					   signalType,
					   octantNumber,
					   moduleNumber,
					   layerNumber,
					   rNumber,
					   channelNumber);  
  if (!asdout.isValid()) {
    log << MSG::DEBUG
        << "getReadoutIDfromOnlineID  Invalid ASDOut" << endmsg;
  } 

  if(!asdout.isValid()) return false;

  // chamber channel -> readout channel
  bool status = m_cabling->getReadoutFromASDOut(&asdout,
						sideType,
						rodID,
						sswID,
						slbID,
						channelID,
						adChannel);
  if (!status) {
    log << MSG::DEBUG
        << "getReadoutIDfromASDOut fails" << endmsg;
  }
  if(!status) return false;


  // SubDetectorID
  if(sideType==TGCIdBase::Aside) subDetectorID = m_AsideId.value();
  if(sideType==TGCIdBase::Cside) subDetectorID = m_CsideId.value();

  return status;
}

// element ID -> readout IDs
bool TGCcablingSvc::getReadoutIDfromElementID(const Identifier & elementID,
                                              int & subdetectorID,
                                              int & rodID) const 
{
  // get station name in string format : T1F,T1E,T2F...
  const int iStation  =  m_idHelper->stationName(elementID);
  const std::string stationName_str = m_idHelper->stationNameString(iStation);

  int iEta = m_idHelper->stationEta(elementID);
  int iPhi = m_idHelper->stationPhi(elementID);
       
  // station type : ex. T1E -> 1, T2F->2
  std::string station_str = stationName_str.substr(1, 1);
  const int stationType = atoi(station_str.c_str());

  // forward/endcap
  enum {FORWARD, ENDCAP};
  const int regionType  = m_idHelper->isForward(elementID) ? FORWARD : ENDCAP;
                                          
  // octant index and module index
  const int max_module = (regionType==FORWARD||stationType==4) ? 3 : 6;
  const int max_phi = max_module*8;
  const int sector = ((iPhi-1)+max_module/3+max_phi)%max_phi;
  const int iOctant = static_cast<int>(sector/max_module);

  // SideType
  subdetectorID = (iEta > 0) ? m_AsideId.value() : m_CsideId.value();
  
  // rodID = 0..7
  if ( subdetectorID == m_AsideId.value() ) {
    rodID = iOctant;
  } else {
    rodID = iOctant ;
  }    

  return true;
}

// readout IDs -> element ID
bool TGCcablingSvc::getElementIDfromReadoutID(Identifier & elementID,
					      const int subDetectorID,
					      const int rodID,
					      const int sswID,
					      const int slbID,
					      const int channelID,
					      bool orChannel) const
{
  Identifier offlineID;

  bool status = getOfflineIDfromReadoutID(offlineID,
					  subDetectorID,
					  rodID,
					  sswID,
					  slbID,
					  channelID,
					  orChannel);
  if(!status) return false;

  elementID = m_idHelper->elementID(offlineID);
  return true;
}

// readout ID -> HPT ID
bool TGCcablingSvc::getHPTIDfromReadoutID(int & phi,
					  bool & isAside,
					  bool & isEndcap,
					  bool & isStrip,int & id,
					  const int subsectorID,
					  const int rodID,
					  const int sswID,
					  const int slbID) const
{
  if(subsectorID==m_AsideId)isAside=true;
  else if(subsectorID==m_CsideId)isAside=false;
  else return false;
  
  if(sswID==13&&0<=slbID&&slbID<=17){
    isEndcap=true;
    phi=(6*rodID+slbID/3+46)%48+1;
    isStrip=(slbID%3==2);
    if(slbID%3==1)id=1;
    else id=0;
    return true;
  }
  else if(sswID==14&&0<=slbID&&slbID<=5){
    isEndcap=false;
    phi=(3*rodID+slbID/2+23)%24+1;
    isStrip=(slbID%2==1);
    id=0;
    return true;
  }
  return false;
}

// HPT ID -> readout ID
bool TGCcablingSvc::getReadoutIDfromHPTID(const int phi,
					  const bool isAside,
					  const bool isEndcap,
					  const bool isStrip,
					  const int id,
					  int & subsectorID,
					  int & rodID,
					  int & sswID,
					  int & slbID) const
{
  if(1<=phi&&phi<= 48){
    if(isAside)subsectorID=m_AsideId;
    else subsectorID=m_CsideId;
    if(isEndcap==true&&0<=id&&id<=1){
      sswID=13;
      rodID=(phi+1)%48/6;
      slbID=(phi+1)%48%6*3+2*isStrip+id;
      return true;
    }
    else if(isEndcap==false&&0<=id&&id<=0){
      sswID=14;
      rodID=phi%24/3;
      slbID=phi%24%3*2+isStrip;
      return true;
    }
    return false;
  }
  return false;
}

// readout ID -> SLB ID
bool TGCcablingSvc::getSLBIDfromReadoutID(int & phi,
					  bool & isAside,
					  bool & isEndcap,
					  int & moduleType,
					  int & id,
					  const int subsectorID,
					  const int rodID,
					  const int sswID,
					  const int slbID) const
{
  isAside = subsectorID==m_AsideId ? 0 : 1;
  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside :TGCIdBase::Cside;
  const LVL1TGCCabling8::TGCModuleId * slb = m_cabling->getSLBFromReadout(side, rodID, sswID, slbID);
  if (!slb) return false;

  isEndcap = slb->getRegionType()==TGCIdBase::Endcap ? 0 : 1;
  phi = isEndcap ? (slb->getSector()+46)%48+1 : (slb->getSector()+23)%23+1;
  moduleType = (int)slb->getModuleType();
  id = slb->getId();

  return true;
}

// readout ID -> rxID
bool TGCcablingSvc::getSLBAddressfromReadoutID(int & slbAddr,
					       const int ,
					       const int ,
					       const int ,
					       const int     ) const
{
  //  Not Valid for TGCcabling for Octant segmentation
  slbAddr = -1;
  return false;
}

bool TGCcablingSvc::getRxIDfromReadoutID(int &     rxId,
					 const int /*subsectorID*/,
					 const int /*rodID*/,
					 const int /*sswID*/,
					 const int slbID ) const
{
  //  rxId is same as slbID for TGCcabling for Octant segmentation
  rxId = slbID;
  return true;
}

// ROD_ID / SSW_ID / RX_ID -> SLB ID
bool TGCcablingSvc::getSLBIDfromRxID(int &phi,
                                     bool & isAside,
                                     bool & isEndcap,
                                     int & moduleType,
                                     int & id,
                                     const int subsectorID,
                                     const int rodID,
                                     const int sswID,
                                     const int rxId) const
{
  return getSLBIDfromReadoutID(phi,
			       isAside,
			       isEndcap,
			       moduleType,
			       id,
			       subsectorID,
			       rodID,
			       sswID,
			       rxId);
}

// SLB ID -> readout ID
bool TGCcablingSvc::getReadoutIDfromSLBID(const int phi,
					  const bool isAside,
					  const bool isEndcap,
					  const int moduleType,
					  const int id,
					  int & subsectorID,
					  int & rodID,
					  int & sswID,
					  int & slbID) const
{
  MsgStream log( msgSvc(), name() );
  
  TGCIdBase::ModuleType module = (TGCIdBase::ModuleType)moduleType;
  TGCIdBase::RegionType region = isEndcap ? TGCIdBase::Endcap : TGCIdBase::Forward;
  int sector = isEndcap ? (phi+1)%48 : phi%24; // sector=0-47(EC), 0-23(FWD), ?(INNER)
  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside :TGCIdBase::Cside;
  LVL1TGCCabling8::TGCModuleSLB slb(side, module, region, sector, id);
  
  if (!slb.isValid()) {
    log << MSG::DEBUG
        << "getReadoutIDfromSLBID "
        << " phi=" << phi
        << " side=" << ( (isAside)? "A": "C") 
        << " region=" << ( (isEndcap)? "Endcap": "Forward")
        << " type=" << moduleType
        << " id=" << id 
        << "Invalid SLB" << endmsg;
  }
  if (!slb.isValid()) return false;
  
  subsectorID = (isAside ? m_AsideId : m_CsideId);
  bool status = m_cabling->getReadoutFromSLB(&slb, side, rodID, sswID, slbID);
  if (!status) {
    log << MSG::DEBUG
        << " FAIL  getReadoutIDfromSLBID" << endmsg;
  } else {
    log << MSG::DEBUG
        << " SUCCESS  getReadoutIDfromSLBID" << endmsg;
    log << MSG::DEBUG
        << " phi=" << phi
        << " side=" << ( (isAside)? "A": "C") 
        << " region=" << ( (isEndcap)? "Endcap": "Forward")
        << " type=" << moduleType
        << " id=" << id 
        << " side" << side 
        << " rodID=" << rodID 
        << " sswID=" << sswID
        << " slbID=" << slbID  <<  endmsg;
  } 
  if (!status) return false;
  return true;
}

// readout ID -> SL ID 
bool TGCcablingSvc::getSLIDfromReadoutID(int & phi,
					 bool & isAside,
					 bool & isEndcap,
					 const int subsectorID,
					 const int rodID,
					 const int sswID,
					 const int slbID) const
{
  if(subsectorID==m_AsideId)isAside=true;
  else if(subsectorID==m_CsideId)isAside=false;
  else return false;

  if(sswID==15){
    if(0<=slbID&&slbID<= 5){
      isEndcap=true;
      phi=(6*rodID+slbID+46)%48+1;
      return true;
    }
    else if( 6 <= slbID && slbID <= 8){
      isEndcap=false;
      phi=(3*rodID+slbID-6+23)%24+1;
      return true;
    }
    return false;
  }
  return false;  
}

// SL ID -> readout ID
bool TGCcablingSvc::getReadoutIDfromSLID(const int phi,
					 const bool isAside,
					 const bool isEndcap,
					 int & subsectorID,
					 int & rodID,
					 int & sswID,
					 int & slbID) const
{
  if(isAside)subsectorID=m_AsideId;
  else subsectorID=m_CsideId;
  sswID=15;
  if(1<=phi&&phi<= 48){
    if(isEndcap){
      rodID=(phi+1)%48/6;
      slbID=(phi+1)%48%6;
      return true;
    }
    else{
      rodID=phi%24/3;
      slbID=phi%24%3+6;
      return true;
    }
    return false; 
  }
  return false;
}

// channel connection
LVL1TGCCabling8::TGCChannelId* 
TGCcablingSvc::getChannel (const LVL1TGCCabling8::TGCChannelId* channelId,
			   TGCIdBase::ChannelIdType type,
			   bool orChannel) const 
{

  return m_cabling->getChannel(channelId,type,orChannel);
}


// module connection
LVL1TGCCabling8::TGCModuleMap* 
TGCcablingSvc::getModule (const LVL1TGCCabling8::TGCModuleId* moduleId,
			  LVL1TGCCabling8::TGCModuleId::ModuleIdType type) const
{

  return m_cabling->getModule(moduleId,type);
}


// give phi-range which a ROD covers  
bool TGCcablingSvc::getCoveragefromRodID(
					 const int rodID,
					 double & startPhi,
					 double & endPhi
					 ) const
{
  int sectorInReadout = rodID;
  if (sectorInReadout>= 8) return false;
  
  startPhi = -1.*M_PI/12. + (3.*M_PI/12.)*sectorInReadout;
  endPhi = startPhi + 3.*M_PI/12.;
 
  return true; 
}

bool TGCcablingSvc::getCoveragefromRodID(
					 const int rodID,
					 int & startEndcapSector,
					 int & coverageOfEndcapSector,
					 int & startForwardSector,
					 int & coverageOfForwardSector
					 ) const
{
  int sectorInReadout = rodID;
  if (sectorInReadout>= 8) return false;

  coverageOfEndcapSector =  6;
  startEndcapSector = sectorInReadout *  coverageOfEndcapSector;
  coverageOfForwardSector =  3;
  startForwardSector = sectorInReadout * coverageOfForwardSector;  
 
  return true; 
}

// HighPtID used in Simulation -> HighPtID in RDO
//  This package is a cabling Svc for simulation in 12.X.0 13.X.0 
//  So no conversion to RDO 
bool TGCcablingSvc::getRDOHighPtIDfromSimHighPtID(const bool ,//isForward
                                                  const bool ,//isStrip
                                                  int & index,
                                                  int & chip,
                                                  int &  //hitId
						  ) const
{
  chip = index*2 + chip;  
  index = 0;
  return true;
}

// HighPtID in RDO -> HighPtID used in Simulation
//  This package is a cabling Svc for simulation in 12.X.0 13.X.0 
//  So no conversion to SIM 
bool TGCcablingSvc::getSimHighPtIDfromRDOHighPtID(const bool ,//isForward
                                                  const bool ,//isStrip
                                                  int & index,
                                                  int & chip,
                                                  int &  //hitId
						  ) const
{
  index = chip/2;
  chip  = chip%2; 
  return true;
}

///////////////////////////////////////////////////////////////
bool TGCcablingSvc::getROINumberfromHighPtID(int &roi,
					     bool isForward,
					     int hpb_wire,
					     int chip_wire,
					     int hitId_wire,
					     int sub_wire,
					     int         , //chip_strip
					     int hitId_strip,
					     int sub_strip) const
{
  // for Strip, there is some ambiguity in the relation between hitID and ROI
  bool status = true;
  int RoiRow = 0;
  int RoiColumn = 0;

  if(isForward){
    RoiRow = 12*chip_wire + 2*hitId_wire + sub_wire;
    RoiColumn = 2*( hitId_strip%2) + sub_strip;
  } else {
    RoiColumn = 2*( hitId_strip%2) + sub_strip; 
    chip_wire = hpb_wire *2 + chip_wire; 
    switch(chip_wire){
    case 0: 
      RoiRow=0; 
      break;
    case 1:
    case 2:
    case 3: 
      RoiRow=12*(chip_wire-1) + 2*(hitId_wire) + sub_wire + 1; 
      break;
    default: 
      break;
    }
  }

  roi = 4*RoiRow + RoiColumn;  

  return status;
}

///////////////////////////////////////////////////////////////
bool TGCcablingSvc::getHighPtIDfromROINumber(int roi,
					     bool isForward,
					     bool isStrip,
					     int & hpb,
					     int & chip,
					     int & hitId,
					     int & sub) const
{
  // for Strip, there is some ambiguity in the relation between hitID and ROI
  bool status = true;
  int RoiRow = static_cast<int> (roi/4);
  int RoiColumn = static_cast<int> (roi%4);
  
  if(isForward){
    if(!isStrip){
      // Forward Wire
      hpb  = 0;
      chip = 0;
      hitId = RoiRow/2;
      sub   = RoiRow%2;
      if ( hitId >5 ){
	chip =1;
	hitId -= 6;
      }
    } else {
      // Forward Strip
      hpb  = 0;
      chip = 0;
      hitId = RoiColumn/2;
      sub   = RoiColumn%2;
    }
  } else {
    if (!isStrip) {
      //Endcap Wire
      hitId = RoiRow/2 + 5;
      sub   = (RoiRow+1)%2;
      if ( hitId >17 ){
	hpb  = 1;
	chip = 1;
	hitId -= 18;
      } else if ( hitId >11 ){
	hpb  = 1;
	chip = 0;
	hitId -= 12;
      } else if ( hitId >5 ){
	hpb  = 0;
	chip = 1;
	hitId -= 6;
      } else {
	hpb  = 0;
	chip = 0;
      }
    } else {
      hpb = 0;
      if (RoiColumn<2){
	sub = RoiColumn; 
	if     (RoiRow <  5) { chip=0; hitId=0; }
	else if(RoiRow <  8) { chip=0; hitId=2; }
	else if(RoiRow < 12) { chip=0; hitId=4; }
	else if(RoiRow < 25) { chip=1; hitId=0; }
	else if(RoiRow < 37) { chip=1; hitId=4; }
	else status=false;
      } else {
	sub = RoiColumn -2;
        if     (RoiRow < 5)  { chip=0; hitId=1; }
        else if(RoiRow < 8)  { chip=0; hitId=3; }
        else if(RoiRow < 12) { chip=0; hitId=5; }
        else if(RoiRow < 25) { chip=1; hitId=1; }
        else if(RoiRow < 37) { chip=1; hitId=5; }
        else status=false;
      }
    }
  }

  return status;
}
