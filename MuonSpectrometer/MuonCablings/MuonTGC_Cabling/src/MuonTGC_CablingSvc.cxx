/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    MuonTGC_CablingSvc.cxx

    Author : Susumu.Oda@cern.ch, Hisaya.Kurashige@cern.ch
    Created from TGCcabling12Svc.cxx in June, 2009
    Description : online-offline ID mapper for TGC

***************************************************************************/

#include "MuonTGC_Cabling/MuonTGC_CablingSvc.h"

#include "PathResolver/PathResolver.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "MuonTGC_Cabling/TGCChannelASDIn.h"
#include "MuonTGC_Cabling/TGCChannelASDOut.h"
#include "MuonTGC_Cabling/TGCModuleSLB.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <cmath>
#include <fstream>

///////////////////////////////////////////////////////////////
MuonTGC_CablingSvc::MuonTGC_CablingSvc(const std::string& name, ISvcLocator* svc) :
    ITGCcablingSvc(name, svc),
    m_cabling(nullptr) {
  declareProperty("AsideId", m_AsideId=103);
  declareProperty("CsideId", m_CsideId=104);
  declareProperty("rodId", m_rodId);  //obsolete
  declareProperty("databaseASDToPP",  m_databaseASDToPP="MuonTGC_Cabling_ASD2PP.db");
  declareProperty("databaseInPP",     m_databaseInPP="MuonTGC_Cabling_PP.db");
  declareProperty("databasePPToSL",   m_databasePPToSL="MuonTGC_Cabling_PP2SL.db");
  declareProperty("databaseSLBToROD", m_databaseSLBToROD="MuonTGC_Cabling_SLB2ROD.db");
}

///////////////////////////////////////////////////////////////
StatusCode  MuonTGC_CablingSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if(ITGCcablingSvc::interfaceID().versionMatch(riid)) {
    *ppvIF = dynamic_cast<ITGCcablingSvc*>(this);
  } else {
    return Service::queryInterface(riid, ppvIF);
  }
  
  addRef();
  return StatusCode::SUCCESS;
}
  
///////////////////////////////////////////////////////////////
void MuonTGC_CablingSvc::getReadoutIDRanges(int& maxRodId,
					    int& maxSswId,
					    int& maxSbloc,
					    int& minChannelId,
					    int& maxChannelId) const
{
  maxRodId = MuonTGC_Cabling::TGCCabling::MAXRODID;
  maxSswId = MuonTGC_Cabling::TGCCabling::MAXSSWID;
  maxSbloc = MuonTGC_Cabling::TGCCabling::MAXSBLOC;
  minChannelId = MuonTGC_Cabling::TGCCabling::MINCHANNELID;
  maxChannelId = MuonTGC_Cabling::TGCCabling::MAXCHANNELID;
}

///////////////////////////////////////////////////////////////
StatusCode MuonTGC_CablingSvc::initialize(void)
{ 
  ATH_MSG_INFO("for 1/12 sector initialize");

  StoreGateSvc* detStore=nullptr;
  ATH_CHECK(serviceLocator()->service("DetectorStore", detStore));
  ATH_CHECK(m_idHelperSvc.retrieve());

  // private databases
  std::string dbASDToPP = PathResolver::find_file(m_databaseASDToPP, "DATAPATH");
  ATH_MSG_DEBUG("found  " << dbASDToPP);
  std::ifstream inASDToPP;
  if(dbASDToPP!= "") {
    inASDToPP.open(dbASDToPP.c_str());
  } else {
    ATH_MSG_FATAL("Could not find input file " << m_databaseASDToPP);
    return StatusCode::FAILURE;
  }
  if(inASDToPP.bad()) {
    ATH_MSG_FATAL("Could not open file " << dbASDToPP);
    return StatusCode::FAILURE;
  }
  inASDToPP.close();


  std::string dbInPP = PathResolver::find_file(m_databaseInPP, "DATAPATH");
  ATH_MSG_DEBUG("found  " << dbInPP);
  std::ifstream inInPP;
  if(dbInPP!="") {
    inInPP.open(dbInPP.c_str());
  } else {
    ATH_MSG_FATAL("Could not find input file " << m_databaseInPP);
    return StatusCode::FAILURE;
  }
  if(inInPP.bad()) {
    ATH_MSG_FATAL("Could not open file " << dbInPP);
    return StatusCode::FAILURE;
  }
  inInPP.close();


  std::string dbPPToSL = PathResolver::find_file(m_databasePPToSL, "DATAPATH");
  ATH_MSG_DEBUG("found  " << dbPPToSL);
  std::ifstream inPPToSL;
  if(dbPPToSL!="") {
    inPPToSL.open(dbPPToSL.c_str());
  } else {
    ATH_MSG_FATAL("Could not find input file " << m_databasePPToSL);
    return StatusCode::FAILURE;
  }
  if(inPPToSL.bad()) {
    ATH_MSG_FATAL("Could not open file " << dbPPToSL);
    return StatusCode::FAILURE;
  }
  inPPToSL.close();



  std::string dbSLBToROD = PathResolver::find_file(m_databaseSLBToROD, "DATAPATH");
  ATH_MSG_DEBUG("found  " << dbSLBToROD);
  std::ifstream inSLBToROD;
  if(dbSLBToROD!="") {
    inSLBToROD.open(dbSLBToROD.c_str());
  } else {
    ATH_MSG_FATAL("Could not find input file " << m_databaseSLBToROD);
    return StatusCode::FAILURE;
  }
  if(inSLBToROD.bad()) {
    ATH_MSG_FATAL("Could not open file " << dbSLBToROD);
    return StatusCode::FAILURE;
  }
  inSLBToROD.close();


  // instantiate TGC cabling manager
  m_cabling = new MuonTGC_Cabling::TGCCabling(dbASDToPP,
					      dbInPP,
					      dbPPToSL,
					      dbSLBToROD);
  
  ATH_CHECK(m_condDataTool.retrieve());
  std::string folderName = m_condDataTool->getFolderName();

  bool isContained = detStore->contains<CondAttrListCollection>(folderName);
  if(!isContained) {
    ATH_MSG_FATAL("Could not found " << folderName << " in DetectorStore.");
    return StatusCode::FAILURE;
  } 

  if(!m_cabling->updateCableASDToPP().isSuccess()) {
    ATH_MSG_WARNING("updateCableASDToPP failed");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}  
  
///////////////////////////////////////////////////////////////
StatusCode MuonTGC_CablingSvc::finalize(void)
{
  delete m_cabling;
  m_cabling = nullptr;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////
// give phi-range which a ROD covers  
bool MuonTGC_CablingSvc::getCoveragefromRodID(const int rodID,
					      double & startPhi,
					      double & endPhi) const
{
  int sectorInReadout = rodID - 1;  //rodID = 1..12
  if(sectorInReadout>= MuonTGC_Cabling::TGCId::NumberOfReadoutSector) return false;
  
  startPhi = 2.*M_PI*(sectorInReadout-0.5)/MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
  endPhi = startPhi + 2.*M_PI/MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
 
  return true; 
}

///////////////////////////////////////////////////////////////
bool MuonTGC_CablingSvc::getCoveragefromRodID(const int rodID,
					      int & startEndcapSector,
					      int & coverageOfEndcapSector,
					      int & startForwardSector,
					      int & coverageOfForwardSector) const
{
  int sectorInReadout = rodID - 1;  //rodID = 1..12
  if(sectorInReadout>= MuonTGC_Cabling::TGCId::NumberOfReadoutSector) return false;
  
  coverageOfEndcapSector =  
    MuonTGC_Cabling::TGCId::NumberOfEndcapSector /
    MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
  startEndcapSector = sectorInReadout *  coverageOfEndcapSector;
  coverageOfForwardSector =  
    MuonTGC_Cabling::TGCId::NumberOfForwardSector /
    MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
  startForwardSector = sectorInReadout *coverageOfForwardSector;  
  
  return true; 
}

///////////////////////////////////////////////////////////////
// Readout ID is ored
bool MuonTGC_CablingSvc::isOredChannel(const int subDetectorID,
				       const int rodID,
				       const int sswID,
				       const int sbLoc,
				       const int channelID) const
{
  Identifier id;
  return getOfflineIDfromReadoutID(id,
				   subDetectorID,
				   rodID,
				   sswID,
				   sbLoc,
				   channelID,
				   true);
}


///////////////////////////////////////////////////////////////
// Offline ID has adjacent Readout ID
bool MuonTGC_CablingSvc::hasAdjacentChannel(const Identifier & offlineID) const
{
  int subDetectorID;
  int rodID;
  int sswID;
  int sbLoc;
  int channelID;
  return getReadoutIDfromOfflineID(offlineID,
				   subDetectorID,
				   rodID,
				   sswID,
				   sbLoc,
				   channelID,
				   true);
}


///////////////////////////////////////////////////////////////
// Online ID has adjacent Readout ID
bool MuonTGC_CablingSvc::hasAdjacentChannel(const int subsystemNumber,
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
  int sbLoc;
  int channelID;
  ATH_MSG_DEBUG("hasAdjacentChannel() "
  << " side=" << subsystemNumber << " octant=" << octantNumber 
  << " module=" << moduleNumber << " layer=" << layerNumber
  << " chamber=" << rNumber << " w/s=" << wireOrStrip
  << " channel=" << channelNumber);
 
  return getReadoutIDfromOnlineID(subDetectorID,
				  rodID,
				  sswID,
				  sbLoc,
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


///////////////////////////////////////////////////////////////
// readout IDs -> offline IDs
bool MuonTGC_CablingSvc::getOfflineIDfromReadoutID(Identifier & offlineID,
						   const int subDetectorID,
						   const int rodID,
						   const int sswID,
						   const int sbLoc,
						   const int channelID,
						   bool orChannel) const
{
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
					 sbLoc,
					 channelID,
					 subsystemNumber,
					 octantNumber,
					 moduleNumber,
					 layerNumber,
					 rNumber,
					 wireOrStrip,
					 channelNumber,
					 orChannel);
  if((!status) && (!orChannel)) {
    ATH_MSG_VERBOSE(
	     " getOfflineIDfromReadoutID :"
	  << " Cannot get OnineID for "
	  << " subdetectorID=" << subDetectorID
	  << " rodID=" << rodID 
	  << " sswID=" << sswID
	  << " sbLoc=" << sbLoc  
	  << " channel=" << channelID
	  << " [ Or =" << orChannel <<"] ");
  }
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
  
  if(!status)  {
    ATH_MSG_VERBOSE(" getOfflineIDfromReadoutID :"
	  << " Cannot get OfflineID for "
	  << " side=" << subsystemNumber << " octant=" << octantNumber 
	  << " module=" << moduleNumber << " layer=" << layerNumber
	  << " chamber=" << rNumber << " w/s=" << wireOrStrip
	  << " channel=" << channelNumber);
  }
  
  return status;
}


///////////////////////////////////////////////////////////////
// offline IDs -> readout IDs
bool MuonTGC_CablingSvc::getReadoutIDfromOfflineID(const Identifier & offlineID,
						   int & subDetectorID,
						   int & rodID,
						   int & sswID,
						   int & sbLoc,
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
  
  // OfflineID -> OnlineID
  bool status = getOnlineIDfromOfflineID(offlineID,
					 subsystemNumber,
					 octantNumber,
					 moduleNumber,
					 layerNumber,
					 rNumber,
					 wireOrStrip,
					 channelNumber);
  

  if(!status) {
    ATH_MSG_WARNING(" Fail to getOnlineIDfromOfflineID " << " for OfflineID=" << offlineID);
  } 
  if(!status) return status;  

  ATH_MSG_DEBUG("getOnlineIDfromOfflineID() "
	<< " offlineID=" << offlineID
	<< " onlineID:  "
	<< " side=" << subsystemNumber << " octant=" << octantNumber 
	<< " module=" << moduleNumber << " layer=" << layerNumber
	<< " chamber=" << rNumber << " w/s=" << wireOrStrip
	<< " channel=" << channelNumber);

  // OnlineID -> ReadoutID
  status = getReadoutIDfromOnlineID(subDetectorID,
				    rodID,
				    sswID,
				    sbLoc,
				    channelID,
				    subsystemNumber,
				    octantNumber,
				    moduleNumber,
				    layerNumber,
				    rNumber,
				    wireOrStrip,
				    channelNumber,
				    adChannel);

  if(!status) {
    if(adChannel) {
      ATH_MSG_DEBUG(" Fail to getReadoutIDfromOnlineID");
      ATH_MSG_DEBUG(" side=" << subsystemNumber << " octant=" << octantNumber 
	      << " module=" << moduleNumber << " layer=" << layerNumber
	      << " chamber=" << rNumber << " w/s=" << wireOrStrip
	      << " channel=" << channelNumber);
    } else {
      ATH_MSG_WARNING(" getReadoutIDfromOnlineID: "
	    << " Cannot get ReadoutID for "
	    << " side=" << subsystemNumber << " octant=" << octantNumber 
	    << " module=" << moduleNumber << " layer=" << layerNumber
	    << " chamber=" << rNumber << " w/s=" << wireOrStrip
	    << " channel=" << channelNumber);
    }
  } else {
      ATH_MSG_DEBUG(" SUCCESS  getReadoutIDfromOnlineID");
      ATH_MSG_DEBUG(" side=" << subsystemNumber << " octant=" << octantNumber 
	    << " module=" << moduleNumber << " layer=" << layerNumber
	    << " chamber=" << rNumber << " w/s=" << wireOrStrip
	    << " channel=" << channelNumber 
	    << "adjacent=" << adChannel);
      ATH_MSG_DEBUG(" subDetectorID" << subDetectorID 
	    << " rodID=" << rodID 
	    << " sswID=" << sswID
	    << " channelID=" << channelID);
  }

  return status;
}


///////////////////////////////////////////////////////////////
// offline ID -> online IDs
bool MuonTGC_CablingSvc::getOnlineIDfromOfflineID(const Identifier & offlineId,
						  int & subSystemNumber,
						  int & octantNumber,
						  int & moduleNumber,
						  int & layerNumber,
						  int & rNumber,
						  int & wireOrStrip,
						  int & channelNumber) const
{
  // get station name in string format : T1F,T1E,T2F...
  const int iStation  =  m_idHelperSvc->tgcIdHelper().stationName(offlineId);
  const int stationType = (iStation - 39)/2;

  if((stationType <1) || (stationType >4)) return false; 
  
  // eta and phi
  int iEta = m_idHelperSvc->tgcIdHelper().stationEta(offlineId);
  int iPhi = m_idHelperSvc->tgcIdHelper().stationPhi(offlineId);
  
  // forward/endcap
  enum {FORWARD, ENDCAP};
  const int regionType  = m_idHelperSvc->tgcIdHelper().isForward(offlineId) ? FORWARD : ENDCAP;
  
  // octant index and module index
  const int sectorEI[] = {-1,  1,  2,   3,  4,  5,   6,  7,  8,   10,  11,  
			  12, 13, 14,  15, 16,      18, 19, 20,   22,  23,
			   0};
  int sector = -1;
  if(regionType==ENDCAP) {
    if(stationType!=4) {
      // Endcap
      sector = (iPhi+1) % MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
    } else {
      // EI
      sector  = sectorEI[ iPhi ];
      // iPhi is redefined for internal use
      if(sector == 0) {
	iPhi = sector + MuonTGC_Cabling::TGCId::NumberOfInnerSector;
      } else {
	iPhi = sector; 
      }
    } 
  } else {
    if(stationType!=4) {      
      // Forward
      sector = iPhi % MuonTGC_Cabling::TGCId::NumberOfForwardSector;
    } else {
      // FI 
      sector = iPhi % MuonTGC_Cabling::TGCId::NumberOfInnerSector;
    }
  }
  int max_module = (regionType==FORWARD||stationType==4) ? 
    MuonTGC_Cabling::TGCId::NumberOfForwardSector 
    : MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
  max_module /=  MuonTGC_Cabling::TGCId::NumberOfOctant;
  const int iOctant = static_cast<int>(sector/max_module);
  const int iModule = sector % max_module;
  
  // R index
  const int rIndex = std::abs(iEta);
  
  // Gas gap
  const int iGasGap = m_idHelperSvc->tgcIdHelper().gasGap(offlineId);

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
  
  if(stationType != 4) { // T1, T2, T3   
    if(regionType==ENDCAP) {
      moduleNumber = modmapE[iModule+1];
      rNumber = 5-rIndex;
    } else {
      moduleNumber = modmapF[iModule+1];
      rNumber = rIndex-1;
    }
  } else {// TI
    if(regionType==ENDCAP) {
      moduleNumber = modmapEI[iModule+1];
      rNumber = rIndex-1;  // 0-offset
    }  else {
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
  wireOrStrip = m_idHelperSvc->tgcIdHelper().isStrip(offlineId);


  // Offline ID channel
  int channel = m_idHelperSvc->tgcIdHelper().channel(offlineId);

  // Offline ID cahnnel -> Online ID channel
  // T11S : EI @ phi=2,11,13,14,15,19,20,21
  //  total number of wire channel = 16 
  //  (T11 : 24 channel) 
  const int OffsetForT11S = 8;
  // T10S : FI @ phi =2,5,8,11,14,17,20,23
  //  total number of wire channel = 30
  //  (T10 32 channel)  

  if((wireOrStrip ==0) && (stationType == 4)) { // TI wire 
    if(regionType==ENDCAP) { // EI
      // iPhi here is onlineID from 1 to 24. 
      switch(iPhi) { 
      case  2:
      case 12:
      case 14:
      case 15:
      case 16:
      case 22:
      case 23:
      case 24:
	// T11S
	channel += OffsetForT11S;
 	break;
      default:
	break;
      }
    }
  }
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subSystemNumber==1)  sideType = TGCIdBase::Aside;
  if(subSystemNumber==-1) sideType = TGCIdBase::Cside;

  // SignalType
  TGCIdBase::SignalType signalType = TGCIdBase::NoSignalType;
  if(wireOrStrip==0) signalType = TGCIdBase::Wire;
  if(wireOrStrip==1) signalType = TGCIdBase::Strip;

  // RegionType
  TGCIdBase::RegionType region = TGCIdBase::NoRegionType;
  if(regionType==FORWARD) region = TGCIdBase::Forward;
  if(regionType==ENDCAP)  region = TGCIdBase::Endcap;


  // ASDIn
  MuonTGC_Cabling::TGCChannelASDIn asdin(sideType,
					 signalType,
					 region,
					 iPhi,
					 layerNumber,
					 rIndex,
					 channel);
  if(!asdin.isValid()) return false;
  
  // offline Id -> online Id
  MuonTGC_Cabling::TGCChannelId* asdout =
    m_cabling->getChannel(&asdin,
			  TGCIdBase::ASDOut,
			  false);
  if(asdout==0) return false;
  if(!asdout->isValid()) {
    delete asdout;
    asdout=0;
    return false;
  }
  channelNumber = asdout->getChannel();
  delete asdout;
  asdout=0;

  return true;
}
      
///////////////////////////////////////////////////////////////
// online IDs -> offline ID
bool MuonTGC_CablingSvc::getOfflineIDfromOnlineID(Identifier & offlineId,
						  const int subSystemNumber,
						  const int octantNumber,
						  const int moduleNumber,
						  const int layerNumber,
						  const int rNumber,
						  const int wireOrStrip,
						  const int channelNumber) const
{
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subSystemNumber==1)  sideType = TGCIdBase::Aside;
  if(subSystemNumber==-1) sideType = TGCIdBase::Cside;

  // SignalType
  TGCIdBase::SignalType signalType = TGCIdBase::NoSignalType;
  if(wireOrStrip==0) signalType = TGCIdBase::Wire;
  if(wireOrStrip==1) signalType = TGCIdBase::Strip;

  ATH_MSG_VERBOSE("getOfflineIDfromOnlineID for " 
	<< " side=" << sideType << " octant=" << octantNumber 
	<< " module=" << moduleNumber << " layer=" << layerNumber
	<< " chamber=" << rNumber << " w/s=" << signalType
	<< " channel=" << channelNumber);
  
  // ASDOut
  MuonTGC_Cabling::TGCChannelASDOut asdout(sideType,
					   signalType,
					   octantNumber,
					   moduleNumber,
					   layerNumber,
					   rNumber,
					   channelNumber);  
  if(!asdout.isValid()) {
    ATH_MSG_WARNING(" Illegal AsdOut for "
	  << " side=" << sideType << " octant=" << octantNumber 
	  << " module=" << moduleNumber << " layer=" << layerNumber
	  << " chamber=" << rNumber << " w/s=" << signalType
	  << " channel=" << channelNumber);
  }
  if(!asdout.isValid()) return false;

  // online Id -> offline Id
  MuonTGC_Cabling::TGCChannelId* asdin =
    m_cabling->getChannel(&asdout,
			  TGCIdBase::ASDIn,
			  false);
  if(!asdin || !asdin->isValid()) {
    ATH_MSG_WARNING(" getOfflineIDfromOnlineID :"
	  << " Illegal AsdIn for "
	  << " side=" << sideType << " octant=" << octantNumber 
	  << " module=" << moduleNumber << " layer=" << layerNumber
	  << " chamber=" << rNumber << " w/s=" << signalType
	  << " channel=" << channelNumber);
  }
  if(asdin==0) return false;
  if(!asdin->isValid()) {
    delete asdin;
    asdin = 0;
    return false;
  }  
  // build identifier
  std::string stationNameStr;
  switch(asdin->getStation()) {
  case 0: // Triplet
    stationNameStr = (asdin->isForward()) ? "T1F" : "T1E";
    break;
  case 1: // Middle Doublet
    stationNameStr = (asdin->isForward()) ? "T2F" : "T2E";
    break;
  case 2:  //Pivot Doublet
    stationNameStr = (asdin->isForward()) ? "T3F" : "T3E";
    break;
  case 3:  // Inner
    stationNameStr = (asdin->isForward()) ? "T4F" : "T4E";
    break;
  default:
    delete asdin; asdin = 0;
    return false;
  }
  int stationEta = asdin->getChamber();
  if(asdin->getSideType()==TGCIdBase::Cside) stationEta *= -1;
  int stationPhi = asdin->getSector();
  int gasGap = asdin->getGasGap();
  int isStrip = (asdin->isStrip()) ? 1 : 0;
  int channel = asdin->getChannel();

  if((asdin->getStation()==3) && (asdin->isEndcap())) {
    // special treatment for EI
    const int phiIE[] = { -1,  1,  2,   3,  4,  5,   6,  7,  8,  -1,  9, 10, 
			  11, 12, 13,  14, 15, -1,  16, 17, 18,  -1, 19, 20, 
			  21};
    stationPhi = phiIE[ asdin->getSector() ];
    if(stationPhi<0) {
      delete asdin;
      asdin = 0;
      return false;
    }
  }

  // OnlineID --> OfflineID
  // T11S : EI @ stationPhi=2,11,13,14,15,19,20,21
  //  total number of wire channel = 16
  //  (T11  24 channel)  
  const int OffsetForT11S = 8;
  // T10S : FI @ statioPhi =2,5,8,11,14,17,20,23
  //  total number of wire channel = 30 
  //  (T10  32 channel) 
  if((asdin->getStation()==3) && (!isStrip)) { //  Inner Wire
    if(asdin->isEndcap()) {
      switch(stationPhi) {
      case  2:
      case 11:
      case 13:
      case 14:
      case 15:
      case 19:
      case 20:
      case 21:
	// T11S
	channel -= OffsetForT11S;
	if(channel<=0) {
          delete asdin;
          return false;
        }
 	break;
      default:
	break;
      }      
    }
  }


  offlineId = m_idHelperSvc->tgcIdHelper().channelID(stationNameStr,
				    stationEta,
				    stationPhi,
				    gasGap,
				    isStrip,
				    channel);
  
  delete asdin;
  asdin = 0;
  return true;
}

///////////////////////////////////////////////////////////////
// readout IDs -> online IDs
bool MuonTGC_CablingSvc::getOnlineIDfromReadoutID(const int subDetectorID,
						  const int rodID,
						  const int sswID,
						  const int sbLoc,
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
  MuonTGC_Cabling::TGCChannelId* asdout =
    m_cabling->getASDOutFromReadout(sideType,
				    rodID,
				    sswID,
				    sbLoc,
				    channelID,
				    orChannel);
  if(asdout==0) {
    if(!orChannel) {
      ATH_MSG_VERBOSE(" getOnlineIDfromReadoutID :"
          << " Cannot get OnlineID of "
          << " side=" << sideType
          << " rodID=" << rodID 
          << " sswID=" << sswID
          << " sbLoc=" << sbLoc  
          << " channel=" << channelID
          << " [ Or =" << orChannel <<"] ");
    }
    return false;
  }
  if(!asdout->isValid()) {
    ATH_MSG_WARNING(" getOnlineIDfromReadoutID :"
	  << " Illegal ASDout for "
	  << " side=" << sideType
	  << " rodID=" << rodID 
	  << " sswID=" << sswID
	  << " sbLoc=" << sbLoc  
	  << " channel=" << channelID
	  << " [ Or =" << orChannel <<"] ");

    delete asdout;
    asdout = 0;
    return false;
  }
  // SubsystemNumber
  subsystemNumber = (asdout->isAside()) ? 1 : -1;
  
  // OctantNumber
  octantNumber = asdout->getOctant();
  
  // RNumber
  rNumber = asdout->getChamber();
  
  // ModuleNumber
  moduleNumber = asdout->getSectorModule();

  layerNumber = asdout->getLayer();

  // WireOrStrip
  wireOrStrip = (asdout->isStrip()) ? 1 : 0;

  // ChannelNumber
  channelNumber = asdout->getChannel();

  delete asdout;
  asdout = 0;
  return true;
}

//////////////////////////////////////////////////////////
// online IDs -> readout IDs
bool MuonTGC_CablingSvc::getReadoutIDfromOnlineID(int & subDetectorID,
						  int & rodID,
						  int & sswID,
						  int & sbLoc,
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
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subsystemNumber==1)  sideType = TGCIdBase::Aside;
  if(subsystemNumber==-1) sideType = TGCIdBase::Cside;

  // SignalType
  TGCIdBase::SignalType signalType = TGCIdBase::NoSignalType;
  if(wireOrStrip==0) signalType = TGCIdBase::Wire;
  if(wireOrStrip==1) signalType = TGCIdBase::Strip;

  // ASDOut
  MuonTGC_Cabling::TGCChannelASDOut asdout(sideType,
					   signalType,
					   octantNumber,
					   moduleNumber,
					   layerNumber,
					   rNumber,
					   channelNumber);  
  if(!asdout.isValid()) {
    ATH_MSG_WARNING(" getReadoutIDfromOnlineID() :"
	  << " Cannot get ASDout for  "
	  << " side=" << sideType
	  << " signal=" << signalType
	  << " octant=" << octantNumber 
	  << " module=" << moduleNumber
	  << " layer=" << layerNumber
	  << " chamber=" << rNumber
	  << " channel=" << channelNumber);
  }
  
  if(!asdout.isValid()) return false;
  
  // chamber channel -> readout channel
  bool status = m_cabling->getReadoutFromASDOut(&asdout,
						sideType,
						rodID,
						sswID,
						sbLoc,
						channelID,
						adChannel);
  if(!status) {
    if(adChannel) {
      ATH_MSG_DEBUG("getReadoutIDfromASDOut fails for adjacent");
    }
    else {
      ATH_MSG_WARNING(" getReadoutIDfromASDOut :"
	    << " Cannot get ReadoutID for "
	    << " side=" << sideType
	    << " signal=" << signalType
	    << " octant=" << octantNumber 
	    << " module=" << moduleNumber
	    << " layer=" << layerNumber
	    << " chamber=" << rNumber
	    << " channel=" << channelNumber);
    }
  }
  if(!status) return false;

  // SubDetectorID
  if(sideType==TGCIdBase::Aside) subDetectorID = m_AsideId.value();
  if(sideType==TGCIdBase::Cside) subDetectorID = m_CsideId.value();

  return status;
}

// element ID -> readout IDs
bool MuonTGC_CablingSvc::getReadoutIDfromElementID(const Identifier & elementID,
						   int & subdetectorID,
						   int & rodID) const 
{
  // get station name in string format : T1F,T1E,T2F...
  const int iStation = m_idHelperSvc->tgcIdHelper().stationName(elementID);
  const int stationType = (iStation - 39)/2;

  int iEta = m_idHelperSvc->tgcIdHelper().stationEta(elementID);
  int iPhi = m_idHelperSvc->tgcIdHelper().stationPhi(elementID);
       
  // forward/endcap
  enum {FORWARD, ENDCAP};
  const int regionType = m_idHelperSvc->tgcIdHelper().isForward(elementID) ? FORWARD : ENDCAP;
 
  // SideType
  subdetectorID = (iEta > 0) ? m_AsideId.value() : m_CsideId.value();

  // sector index and module index
  const int max_phi = (regionType==FORWARD||stationType==4) ?
    MuonTGC_Cabling::TGCId::NumberOfForwardSector :
    MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
  const int max_module =  max_phi / MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
  const int sector = ((iPhi-1)+ max_phi/24 + max_phi)%max_phi;
  int readoutSector = static_cast<int>(sector/max_module);  
  // Inner case
  if(stationType==4) {
    readoutSector = (readoutSector /3);
    readoutSector = 3*readoutSector + 1;
  }
  
  // rodID = 1..12
  if(subdetectorID == m_AsideId.value()) {
    rodID = readoutSector +1;
  } else {
    rodID = readoutSector +1;
  }    

  return true;
}

///////////////////////////////////////////////////////////////
// readout IDs -> element ID
bool MuonTGC_CablingSvc::getElementIDfromReadoutID(Identifier & elementID,
						   const int subDetectorID,
						   const int rodID,
						   const int sswID,
						   const int sbLoc,
						   const int channelID,
						   bool orChannel) const
{
  Identifier offlineID;
   
  // get min/max values for ReadoutID parameters
  int maxRodId, maxSswId, maxSbloc, minChannelId ,maxChannelId;
  getReadoutIDRanges(maxRodId, maxSswId, maxSbloc, 
		     minChannelId ,maxChannelId);

  // check sswID and channelID in allowed range
  if((sswID > maxSswId) || 
     (channelID < minChannelId) ||
     (channelID > maxChannelId))  {
      ATH_MSG_DEBUG(" getElementIDfromReadoutID() :"
	    << " Illeagal channel ID"
	    << " subdetectorID=" << subDetectorID
	    << " rodID=" << rodID 
	    << " sswID=" << sswID
	    << " sbLoc=" << sbLoc  
	    << " channel=" << channelID
	    << " [ Or =" << orChannel <<"] ");
    return false; 
  }

  bool status = getOfflineIDfromReadoutID(offlineID,
					  subDetectorID,
					  rodID,
					  sswID,
					  sbLoc,
					  channelID,
					  orChannel);
  if(!status) {
    ATH_MSG_DEBUG(" getElementIDfromReadoutID :"
        << " Cannot get OfflineID ");
    return false;
  }
  
  elementID = m_idHelperSvc->tgcIdHelper().elementID(offlineID);
  return true;
}

///////////////////////////////////////////////////////////////
// readout ID -> SLB ID
bool MuonTGC_CablingSvc::getSLBIDfromReadoutID(int & phi,
					       bool & isAside,
					       bool & isEndcap,
					       int & moduleType,
					       int & id,
					       const int subsectorID,
					       const int rodID,
					       const int sswID,
					       const int sbLoc) const
{
  isAside = (subsectorID==m_AsideId);

  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside : TGCIdBase::Cside;

  const MuonTGC_Cabling::TGCModuleId * slb = m_cabling->getSLBFromReadout(side, rodID, sswID, sbLoc);
  if(!slb) {
    ATH_MSG_VERBOSE(" getSLBIDfromReadoutID :"
	  << " Cannot get SLB of "
	  << " side" << side 
	  << " rodID=" << rodID 
	  << " sswID=" << sswID
	  << " sbLoc=" << sbLoc);
    return false;
  }

  isEndcap = (slb->getRegionType()==TGCIdBase::Endcap);
  moduleType = (int)slb->getModuleType();
  bool isInner = (moduleType==TGCIdBase::WI || moduleType==TGCIdBase::SI);
  int offset, numOfSector;
  if(isInner) {
    numOfSector = MuonTGC_Cabling::TGCId::NumberOfInnerSector;
  } else {
    if(isEndcap) {
      numOfSector = MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
    }else{
      numOfSector = MuonTGC_Cabling::TGCId::NumberOfForwardSector;
    }
  } 
  offset = numOfSector -  numOfSector/24;
  phi = (slb->getSector()+offset)%numOfSector +1;
  id = slb->getId();

  return true;
}

///////////////////////////////////////////////////////////////
// readout ID -> rxID
bool MuonTGC_CablingSvc::getSLBAddressfromReadoutID(int & slbAddr,
						    const int subsectorID,
						    const int rodID,
						    const int sswID,
						    const int sbLoc) const
{
  slbAddr = -1;
  
  bool isAside = (subsectorID==m_AsideId);
  
  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside : TGCIdBase::Cside;

  const MuonTGC_Cabling::TGCModuleId * slb = m_cabling->getSLBFromReadout(side, rodID, sswID, sbLoc);
  if(!slb)   {
    ATH_MSG_WARNING(" getSLBAddressfromReadoutID :"
        << " Cannot get SLB of "
        << " side" << side 
        << " rodID=" << rodID 
        << " sswID=" << sswID
        << " sbLoc=" << sbLoc);
    return false;
  }

  const MuonTGC_Cabling::TGCModuleSLB* modSlb = dynamic_cast<const MuonTGC_Cabling::TGCModuleSLB*>(slb);
  if(!modSlb) {
    ATH_MSG_WARNING("MuonTGC_CablingSvc::getSLBAddressfromReadoutID "
	  << "dynamic_cast<const MuonTGC_Cabling::TGCModuleSLB*>(slb) failed."); 
    return false;
  }

  slbAddr = modSlb->getSlbAddr(); 
  return true;
}

///////////////////////////////////////////////////////////////
// readout ID -> RxID
bool  MuonTGC_CablingSvc::getRxIDfromReadoutID(int &rxId,
					       const int subsectorID,
					       const int rodID,
					       const int sswID,
					       const int sbLoc) const
{
  rxId = -1;
  
  bool isAside = (subsectorID==m_AsideId);
  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside : TGCIdBase::Cside;
  
  rxId  = m_cabling->getRxIdFromReadout(side, rodID, sswID, sbLoc);
  if(rxId<0) {
    ATH_MSG_WARNING(" getRxIDfromReadoutID :"
	  << " Cannot get rxId of "
	  << " side" << side 
	  << " rodID=" << rodID 
	  << " sswID=" << sswID
	  << " sbLoc=" << sbLoc);
    return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////
// ROD_ID / SSW_ID / RX_ID -> SLB ID
bool MuonTGC_CablingSvc::getSLBIDfromRxID(int &phi,
					  bool &isAside,
					  bool &isEndcap,
					  int &moduleType,
					  int &id,
					  const int subsectorID,
					  const int rodID,
					  const int sswID,
					  const int rxId) const
{
  isAside = (subsectorID==m_AsideId);
  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside : TGCIdBase::Cside;

  MuonTGC_Cabling::TGCModuleId * slb = m_cabling->getSLBFromRxId(side, rodID, sswID, rxId);
  if(!slb) {
    ATH_MSG_WARNING("geSLBIDfromRxID :"
	  << " Cannot get SLB of "
	  << " side" << side 
	  << " rodID=" << rodID 
	  << " sswID=" << sswID
	  << " rxId=" << rxId);
    return false;
  }
  
  isEndcap = (slb->getRegionType()==TGCIdBase::Endcap);
  moduleType = (int)slb->getModuleType();
  bool isInner = (moduleType==TGCIdBase::WI || moduleType==TGCIdBase::SI);
  int offset, numOfSector;
  if(isInner) {
    numOfSector = MuonTGC_Cabling::TGCId::NumberOfInnerSector;
  } else {
    if(isEndcap) {
      numOfSector = MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
    } else {
      numOfSector = MuonTGC_Cabling::TGCId::NumberOfForwardSector;
    }
  } 
  offset = numOfSector -  numOfSector/24;
  phi = (slb->getSector()+offset)%numOfSector +1;
  id = slb->getId();
  delete slb; slb = 0; 
  return true;
}

///////////////////////////////////////////////////////////////
// SLB ID -> readout ID
bool MuonTGC_CablingSvc::getReadoutIDfromSLBID(const int phi,
					       const bool isAside,
					       const bool isEndcap,
					       const int moduleType,
					       const int id,
					       int & subsectorID,
					       int & rodID,
					       int & sswID,
					       int & sbLoc) const
{
  TGCIdBase::ModuleType module = static_cast<TGCIdBase::ModuleType>(moduleType);
  TGCIdBase::RegionType region = isEndcap ? TGCIdBase::Endcap : TGCIdBase::Forward;
  bool isInner = (module==TGCIdBase::WI || module==TGCIdBase::SI); 
  int sector = -1;// sector=0-47(EC), 0-23(FWD), 0-23(INNER)
  if(isInner) {
    sector = phi % MuonTGC_Cabling::TGCId::NumberOfInnerSector;
  } else if(isEndcap) {
    sector = (phi+1) % MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
  } else {
    sector = phi  % MuonTGC_Cabling::TGCId::NumberOfForwardSector;
  }
  TGCIdBase::SideType side = isAside ? TGCIdBase::Aside : TGCIdBase::Cside;

  MuonTGC_Cabling::TGCModuleSLB slb(side, module, region, sector, id);
  
   if(!slb.isValid()) {
    ATH_MSG_WARNING("getReadoutIDfromSLBID "
	  << " phi=" << phi
	  << " side=" << ((isAside) ? "A" : "C") 
	  << " region=" << ((isEndcap) ? "Endcap" : "Forward")
	  << " type=" << moduleType
	  << " id=" << id 
	  << " Invalid SLB");
   } 
   if(!slb.isValid()) return false;
  
  subsectorID = (isAside ? m_AsideId : m_CsideId);
   
  bool status = m_cabling->getReadoutFromSLB(&slb, side, rodID, sswID, sbLoc);
  
  if(!status) {
    ATH_MSG_DEBUG(" FAIL  getReadoutIDfromSLBID");
  } else {
      ATH_MSG_DEBUG(" SUCCESS  getReadoutIDfromSLBID");
      ATH_MSG_DEBUG(" phi=" << phi
	    << " side=" << ((isAside) ? "A" : "C") 
	    << " region=" << ((isEndcap) ? "Endcap" : "Forward")
	    << " type=" << moduleType
	    << " id=" << id 
	    << " side" << side 
	    << " rodID=" << rodID 
	    << " sswID=" << sswID
	    << " sbLoc=" << sbLoc);
  }
  if(!status) return false;
  return true;
}


///////////////////////////////////////////////////////////////
// readout ID -> SL ID 
bool MuonTGC_CablingSvc::getSLIDfromReadoutID(int & phi,
					      bool & isAside,
					      bool & isEndcap,
					      const int subsectorID,
					      const int rodID,
					      const int sswID,
					      const int sbLoc) const
{
  isAside = (subsectorID==m_AsideId);
  if(!isAside && (subsectorID!=m_CsideId)) {
    ATH_MSG_WARNING(" getSLIDfromReadoutID : "
	  << " ERROR  illegal subsectorID [=" 
	  << subsectorID  <<"] ");
    return false;
  } 
  int sectorInReadout = (rodID -1); // rodID = 1..12 for both sides
  if(sectorInReadout>= MuonTGC_Cabling::TGCId::NumberOfReadoutSector) return false;
  
  // sswID
  // sswID for SL is fixed to 9
  if(sswID!= 9) {
    ATH_MSG_WARNING(" getSLIDfromReadoutID : "
	  << " ERROR  sswID for SL should be 9 [now =" 
	  << sswID  <<"] ");
    return false;
  }
  
  int offset, numOfSector, sector;
  if(0<=sbLoc && sbLoc <= 3) {
    isEndcap=true;
    numOfSector = MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
    offset = numOfSector -  numOfSector/24;
    sector = numOfSector * sectorInReadout /  MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
    phi = (sector + sbLoc + offset)%numOfSector+1;
  } else if(sbLoc==4 || sbLoc==5) {
    isEndcap=false;
    numOfSector = MuonTGC_Cabling::TGCId::NumberOfForwardSector;
    offset = numOfSector -  numOfSector/24;
    sector = numOfSector * sectorInReadout /  MuonTGC_Cabling::TGCId::NumberOfReadoutSector;
    phi = (sector + (sbLoc-4) + offset)% numOfSector+1;
  } else {
    ATH_MSG_WARNING(" getSLIDfromReadoutID : "
	  << " ERROR  illegal sbLoc for SL [=" 
	  << sbLoc  <<"] ");
    return false;
  }
  return true;  
}

///////////////////////////////////////////////////////////////
// SL ID -> readout ID
bool MuonTGC_CablingSvc::getReadoutIDfromSLID(const int phi,
					      const bool isAside,
					      const bool isEndcap,
					      int & subsectorID,
					      int & rodID,
					      int & sswID,
					      int & sbLoc) const
{
  if(isAside)subsectorID=m_AsideId;
  else subsectorID=m_CsideId;
  
  if(isEndcap) {
    if(phi<1 || phi>MuonTGC_Cabling::TGCId::NumberOfEndcapSector) return false;
  } else {
    if(phi<1 || phi>MuonTGC_Cabling::TGCId::NumberOfForwardSector) return false;
  }

  int sector;
  int sectorInReadout;
  if(isEndcap) {
    sector = (phi+1)% MuonTGC_Cabling::TGCId::NumberOfEndcapSector;
    sectorInReadout = sector %  
      (MuonTGC_Cabling::TGCId::NumberOfEndcapSector / MuonTGC_Cabling::TGCId::NumberOfReadoutSector);
    sbLoc = sectorInReadout;
    rodID = (sector-sectorInReadout)/
      (MuonTGC_Cabling::TGCId::NumberOfEndcapSector / MuonTGC_Cabling::TGCId::NumberOfReadoutSector)
      + 1;
  } else {
    sector = phi % MuonTGC_Cabling::TGCId::NumberOfForwardSector;
    sectorInReadout = sector % 
      (MuonTGC_Cabling::TGCId::NumberOfForwardSector / MuonTGC_Cabling::TGCId::NumberOfReadoutSector);
    sbLoc = sectorInReadout + 4;
    rodID = (sector-sectorInReadout)/
      (MuonTGC_Cabling::TGCId::NumberOfForwardSector / MuonTGC_Cabling::TGCId::NumberOfReadoutSector)
      + 1;
  }
  // Fixed SSWID for SL 
  sswID = 9;
  
  return true;
}

///////////////////////////////////////////////////////////////
// HPT ID -> readout ID
bool MuonTGC_CablingSvc::getReadoutIDfromHPTID(const int phi,
					       const bool isAside,
					       const bool isEndcap,
					       const bool ,
					       const int ,
					       int & subsectorID,
					       int & rodID,
					       int & sswID,
					       int & sbLoc) const
{
  return getReadoutIDfromSLID(phi, isAside, isEndcap,
			      subsectorID,
			      rodID,
			      sswID,
			      sbLoc);
}



///////////////////////////////////////////////////////////////
// channel connection
MuonTGC_Cabling::TGCChannelId* 
MuonTGC_CablingSvc::getChannel(const MuonTGC_Cabling::TGCChannelId* channelId,
			       TGCIdBase::ChannelIdType type,
			       bool orChannel) const 
{
  return m_cabling->getChannel(channelId, type, orChannel);
}


///////////////////////////////////////////////////////////////
// module connection
MuonTGC_Cabling::TGCModuleMap* 
MuonTGC_CablingSvc::getModule(const MuonTGC_Cabling::TGCModuleId* moduleId,
			      MuonTGC_Cabling::TGCModuleId::ModuleIdType type) const
{
  return m_cabling->getModule(moduleId, type);
}


///////////////////////////////////////////////////////////////
// CAUTION!!: return RDO value (not the value for simulation)
bool MuonTGC_CablingSvc::getHighPtIDfromROINumber(int roi,
						  bool isForward,
						  bool isStrip,
						  int & hpb,
						  int & chip,
						  int & hitId,
						  int & sub) const
{
  // for Strip, there is some ambiguity in the relation between hitID and ROI
  bool status = true;
  int RoiRow = static_cast<int>(roi/4);
  int RoiColumn = static_cast<int>(roi%4);
  
  if(!isStrip) {
    if(isForward) {
      switch(RoiRow) {
      case 0: chip=0; hitId=1; sub=0; break;
      case 1: chip=0; hitId=1; sub=1; break;
      case 2: chip=0; hitId=2; sub=0; break;
      case 3: chip=0; hitId=2; sub=1; break;
      case 4: chip=0; hitId=3; sub=0; break;
      case 5: chip=0; hitId=3; sub=1; break;
      case 6: chip=0; hitId=4; sub=0; break;
      case 7: chip=0; hitId=4; sub=1; break;
      case 8: chip=0; hitId=5; sub=0; break;
      case 9: chip=0; hitId=5; sub=1; break;
      case 10: chip=0; hitId=6; sub=0; break;
      case 11: chip=0; hitId=6; sub=1; break;
      case 12: chip=1; hitId=1; sub=0; break;
      case 13: chip=1; hitId=1; sub=1; break;
      case 14: chip=1; hitId=2; sub=0; break;
      case 15: chip=1; hitId=2; sub=1; break;
      default: status=false; break;
      }
    } else {
      switch(RoiRow) {
      case 0: chip=0; hitId=1; sub=1; break;
      case 1: chip=1; hitId=1; sub=0; break;
      case 2: chip=1; hitId=1; sub=1; break;
      case 3: chip=1; hitId=2; sub=0; break;
      case 4: chip=1; hitId=2; sub=1; break;
      case 5: chip=1; hitId=3; sub=0; break;
      case 6: chip=1; hitId=3; sub=1; break;
      case 7: chip=1; hitId=4; sub=0; break;
      case 8: chip=1; hitId=4; sub=1; break;
      case 9: chip=1; hitId=5; sub=0; break;
      case 10: chip=1; hitId=5; sub=1; break;
      case 11: chip=1; hitId=6; sub=0; break;
      case 12: chip=1; hitId=6; sub=1; break;
      case 13: chip=2; hitId=1; sub=0; break;
      case 14: chip=2; hitId=1; sub=1; break;
      case 15: chip=2; hitId=2; sub=0; break;
      case 16: chip=2; hitId=2; sub=1; break;
      case 17: chip=2; hitId=3; sub=0; break;
      case 18: chip=2; hitId=3; sub=1; break;
      case 19: chip=2; hitId=4; sub=0; break;
      case 20: chip=2; hitId=4; sub=1; break;
      case 21: chip=2; hitId=5; sub=0; break;
      case 22: chip=2; hitId=5; sub=1; break;
      case 23: chip=2; hitId=6; sub=0; break;
      case 24: chip=2; hitId=6; sub=1; break;
      case 25: chip=3; hitId=1; sub=0; break;
      case 26: chip=3; hitId=1; sub=1; break;
      case 27: chip=3; hitId=2; sub=0; break;
      case 28: chip=3; hitId=2; sub=1; break;
      case 29: chip=3; hitId=3; sub=0; break;
      case 30: chip=3; hitId=3; sub=1; break;
      case 31: chip=3; hitId=4; sub=0; break;
      case 32: chip=3; hitId=4; sub=1; break;
      case 33: chip=3; hitId=5; sub=0; break;
      case 34: chip=3; hitId=5; sub=1; break;
      case 35: chip=3; hitId=6; sub=0; break;
       case 36: chip=3; hitId=6; sub=1; break;
      default: status=false; break;
      }
    }
  } else if(isStrip) {
    if(isForward) {
      switch(RoiColumn) {
      case 0: chip=0; hitId=1; sub=0; break;
      case 1: chip=0; hitId=1; sub=1; break;
      case 2: chip=0; hitId=2; sub=0; break;
      case 3: chip=0; hitId=2; sub=1; break;
      default: status=false; break;
      }
    } else {
      switch(RoiColumn) {
      case 0: 
        if(RoiRow < 5) { chip=0; hitId=1; sub=0; }
        else if(RoiRow < 8) { chip=0; hitId=3; sub=0; }
        else if(RoiRow < 12) { chip=0; hitId=5; sub=0; }
        else if(RoiRow < 25) { chip=1; hitId=1; sub=0; }
        else if(RoiRow < 37) { chip=1; hitId=5; sub=0; }
        else status=false;
        break;
      case 1:
	if(RoiRow < 5) { chip=0; hitId=1; sub=1; }
	else if(RoiRow < 8) { chip=0; hitId=3; sub=1; }
        else if(RoiRow < 12) { chip=0; hitId=5; sub=1; }
        else if(RoiRow < 25) { chip=1; hitId=1; sub=1; }
        else if(RoiRow < 37) { chip=1; hitId=5; sub=1; }
        else status=false;
        break;
      case 2:
        if(RoiRow < 5) { chip=0; hitId=2; sub=0; }
        else if(RoiRow < 8) { chip=0; hitId=4; sub=0; }
        else if(RoiRow < 12) { chip=0; hitId=6; sub=0; }
        else if(RoiRow < 25) { chip=1; hitId=2; sub=0; }
        else if(RoiRow < 37) { chip=1; hitId=6; sub=0; }
        else status=false;
        break;
      case 3:
        if(RoiRow < 5) { chip=0; hitId=2; sub=1; }
        else if(RoiRow < 8) { chip=0; hitId=4; sub=1; }
        else if(RoiRow < 12) { chip=0; hitId=6; sub=1; }
        else if(RoiRow < 25) { chip=1; hitId=2; sub=1; }
        else if(RoiRow < 37) { chip=1; hitId=6; sub=1; }
        else status=false;
        break;
      default: status=false; break;
      }
    }
  }
  
  hpb = 0;
  
  return status;
}

///////////////////////////////////////////////////////////////
bool MuonTGC_CablingSvc::getROINumberfromHighPtID(int &roi,
						  bool isForward,
						  int , // hpb_wire
						  int chip_wire,
						  int hitId_wire,
						  int sub_wire,
						  int ,  // chip_strip
						  int hitId_strip,
						  int sub_strip) const
{
  // for Strip, there is some ambiguity in the relation between hitID and ROI
  // CAUTION!!: return RDO value (not the value for simulation)
  bool status = true;
  int RoiRow = 0;
  int RoiColumn = 0;
  
  if(isForward) {
    RoiRow = 12*chip_wire + 2*(hitId_wire - 1) + sub_wire;
    RoiColumn = 2*((hitId_strip - 1)%2) + sub_strip;
  } else if(!isForward) {
    RoiColumn = 2*((hitId_strip - 1)%2) + sub_strip;
    switch(chip_wire) {
    case 0: RoiRow=0; break;
    case 1:
    case 2:
    case 3: RoiRow=12*(chip_wire-1) + 2*(hitId_wire - 1) + sub_wire + 1; break;
    default: break;
    }
  }

  roi = 4*RoiRow + RoiColumn;  

  return status;
}


///////////////////////////////////////////////////////////////
// HighPtID used in Simulation -> HighPtID in RDO
bool MuonTGC_CablingSvc::getRDOHighPtIDfromSimHighPtID(const bool isForward,
						       const bool isStrip,
						       int & index,
						       int & chip,
						       int & hitId) const
{
  if(isForward) {
    if(isStrip) {//FS
      index=0; hitId--;
    } else if(!isStrip) {//FW
      index=0; hitId++;
    }
  } else {
    if(isStrip) {//ES
      index=0; hitId++;
    } else if(!isStrip) {//EW
      if(index==0 && chip==0) hitId=1;
      else if(index==0 && chip==1) hitId++;
      else if(index==1 && chip==0) {chip=2; hitId++;}
      else if(index==1 && chip==1) {chip=3; hitId++;}
      }
  }    
  
  return true;
}

///////////////////////////////////////////////////////////////
// HighPtID in RDO -> HighPtID used in Simulation
bool MuonTGC_CablingSvc::getSimHighPtIDfromRDOHighPtID(const bool isForward,
						       const bool isStrip,
						       int & index,
						       int & chip,
						       int & hitId) const
{
  if(isForward) {
    if(isStrip) {//FS
      index=0; hitId++;
    } else if(!isStrip) {//FW
      index=0; hitId--;
    }
  } else {
    if(isStrip) {//ES
      index=0; hitId--;
    } else if(!isStrip) {//EW
      switch(chip) {
      case 0: index=0; hitId=5; break;
      case 1: index=0; hitId--; break;
      case 2: index=1; chip=0; hitId--; break;
      case 3: index=1; chip=1; hitId--; break;
      default: break;
      }
    }    
  }
  
  return true;
}

///////////////////////////////////////////////////////////////
// high pt coincidence IDs -> offline IDs
bool MuonTGC_CablingSvc::getOfflineIDfromHighPtID(Identifier & offlineID,
						  const int subDetectorID,
						  const int rodID,
						  const int sectorInReadout,
						  const bool isStrip,
						  const bool isForward,
						  const int hpb,
						  const int chip,
						  const int hitID,
						  const int pos) const 
{
  // all input is derived from TgcRawData
  int sswID = -1;
  int sbLoc = -1;
  int channelID = -1;
  
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;

  // SignalType, RegionType
  TGCIdBase::SignalType signalType = (isStrip) ? TGCIdBase::Strip : TGCIdBase::Wire;
  TGCIdBase::RegionType regionType = (isForward) ? TGCIdBase::Forward : TGCIdBase::Endcap;

  // ModuleType
  TGCIdBase::ModuleType moduleType = TGCIdBase::NoModuleType;
  if(signalType==TGCIdBase::Wire) { 
    moduleType=TGCIdBase::WD;
  } else {
    moduleType=TGCIdBase::SD;
  }

  // Get ReadoutID for pivot plane 
  bool status = m_cabling->getReadoutFromHighPtID(sideType,
						  rodID,
						  sswID,
						  sbLoc,
						  channelID,
						  signalType,
						  regionType,
						  sectorInReadout,
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
                                   sbLoc,
                                   channelID);
  
}

///////////////////////////////////////////////////////////////
// offline IDs -> high pt coincidence IDs
bool MuonTGC_CablingSvc::getHighPtIDfromOfflineID(const Identifier & offlineID,
						  int & subDetectorID,
						  int & rodID,
						  int & sectorInReadout,
						  bool & isStrip,
						  bool & isForward,
						  int & hpb,
						  int & chip,
						  int & hitID,
						  int & pos) const 
{  
  int sswID = -1;
  int sbLoc = -1;
  int channelID = -1;   

  bool status = getReadoutIDfromOfflineID(offlineID,
					  subDetectorID,
					  rodID,
					  sswID,
					  sbLoc,
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
					     sbLoc,
					     channelID,
					     signalType,
					     regionType,
					     sectorInReadout,
					     hpb,
					     chip,
					     hitID,
					     pos);
  if(!status) return false;
  
  isStrip = (signalType==TGCIdBase::Strip); 
  isForward = (regionType==TGCIdBase::Forward); 

  return true;
}


///////////////////////////////////////////////////////////////
// low pt coincidence IDs -> offline IDs
bool MuonTGC_CablingSvc::getOfflineIDfromLowPtCoincidenceID(Identifier & offlineID,
							    const int subDetectorID,
							    const int rodID,
							    const int sswID,
							    const int sbLoc,
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
							  sbLoc,
							  channelID,
							  block,
							  pos,
							  middle);
  if(!status) return false;
  
  return getOfflineIDfromReadoutID(offlineID,
				   subDetectorID,
				   rodID,
				   sswID,
				   sbLoc,
				   channelID);
}

///////////////////////////////////////////////////////////////
// offline IDs -> low pt coincidence IDs
bool MuonTGC_CablingSvc::getLowPtCoincidenceIDfromOfflineID(const Identifier & offlineID,
							    int & subDetectorID,
							    int & rodID,
							    int & sswID,
							    int & sbLoc,
							    int & block,
							    int & pos,
							    bool middle) const
{
  int channelID = -1;   

  bool status = getReadoutIDfromOfflineID(offlineID,
					  subDetectorID,
					  rodID,
					  sswID,
					  sbLoc,
					  channelID);
  if(!status) return false;    
  
  // SideType
  TGCIdBase::SideType sideType = TGCIdBase::NoSideType;
  if(subDetectorID==m_AsideId.value()) sideType = TGCIdBase::Aside;
  if(subDetectorID==m_CsideId.value()) sideType = TGCIdBase::Cside;
  
  return m_cabling->getLowPtCoincidenceFromReadout(sideType,
						   rodID,
						   sswID,
						   sbLoc,
						   channelID,
						   block,
						   pos,
						   middle);
}
