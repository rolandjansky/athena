/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLING_HH
#define TGCCABLING_HH
  
#include "TGCcablingInterface/TGCCablingBase.h"

#include "TGCcabling12/TGCCableInASD.h"
#include "TGCcabling12/TGCCableASDToPP.h"
#include "TGCcabling12/TGCCableInPP.h"
#include "TGCcabling12/TGCCablePPToSLB.h"
#include "TGCcabling12/TGCCableInSLB.h"
#include "TGCcabling12/TGCCableSLBToHPB.h"
#include "TGCcabling12/TGCCableHPBToSL.h"
#include "TGCcabling12/TGCCableSLBToSSW.h"
#include "TGCcabling12/TGCCableSSWToROD.h"

namespace LVL1TGCCabling12
{

class TGCCabling : public TGCCablingBase
{
public:
  // Constructor & Destructor
  TGCCabling (std::string filenameASDToPP,
	      std::string filenameInPP,
	      std::string filenamePPToSL,
	      std::string filenameSLBToROD);

  virtual ~TGCCabling (void);

  enum MAXMINREADOUTIDS {
    MAXRODID = 12,
    MINRODID = 1,
    MAXSSWID = 9,
    MINSSWID = 0,
    MAXSBLOC = 31,
    MINSBLOC = 0,
    MINCHANNELID = 40,
    MAXCHANNELID = 199
  };

  // virtual method  of TGCCabligBase   
  // slbIn --> AsdOut
  virtual TGCIdBase* getASDOutChannel(const TGCIdBase* slb_in ) const ; 


  /////////////////////////////////////////////////////
  // readout ID -> SLB Module
  const TGCModuleId* getSLBFromReadout (TGCIdBase::SideType side,
					int rodId,
					int sswId,
					int sbLoc) const;

  // readoutID -> RxID
  int getRxIdFromReadout (TGCIdBase::SideType side,
			  int rodId,
			  int sswId,
			  int sbLoc) const;

  // SSW ID/RX ID-> SLB Module
  TGCModuleId* getSLBFromRxId (TGCIdBase::SideType side,
				  int rodId,
				  int sswId,
				  int rxId) const;
  
  
  // SLB Module -> readout ID
  bool getReadoutFromSLB (const TGCModuleSLB* slb,
			  TGCIdBase::SideType & side,
			  int & rodId,
			  int & sswId,
			  int & sbLoc) const;
  
  
  // readout channel -> chamber channel
  TGCChannelId* getASDOutFromReadout(TGCIdBase::SideType side,
				     int rodId,
				     int sswId,
				     int sbLoc,
				     int channel,
				     bool orChannel=false) const;
  

  // chamber channel -> readout channel
  bool getReadoutFromASDOut(const TGCChannelASDOut* asdout,
			    TGCIdBase::SideType & side,
			    int & rodId,
			    int & sswId,
			    int & sbLoc,
			    int & channel,
			    bool orChannel=false) const;
  

  // readout channel -> coincidence channel
  bool getHighPtIDFromReadout(TGCIdBase::SideType side,
			       int rodId,
			       int sswId,
			       int sbLoc,
			       int channel,
			       TGCIdBase::SignalType & signal,
			       TGCIdBase::RegionType & region,
			       int & sectorInReadout,
			       int & hpbId,
			       int & block,
			       int & hitId,
			       int & pos) const;
  

  // coincidence channel -> readout channel
  bool getReadoutFromHighPtID(TGCIdBase::SideType side,
			       int rodId,
			       int & sswId,
			       int & sbLoc,
			       int & channel,
			       TGCIdBase::SignalType signal,
			       TGCIdBase::RegionType region,
			       int sectorInReadout,
			       int hpbId,
			       int block,
			       int hitId,
			       int pos,
			       TGCIdBase::ModuleType moduleType,
			       bool orChannel) const;

  // readout channel -> coincidence channel
  bool getLowPtCoincidenceFromReadout(TGCIdBase::SideType side,
				      int rodId,
				      int sswId,
				      int sbLoc,
				      int channel,
				      int & block,
				      int & pos,
				      bool middle=false) const;
  

  // coincidence channel -> readout channel
  bool getReadoutFromLowPtCoincidence(TGCIdBase::SideType side,
				      int rodId,
				      int sswId,
				      int sbLoc,
				      int & channel,
				      int block,
				      int pos,
				      bool middle=false) const;  
  
  // channel connection
  TGCChannelId* getChannel (const TGCChannelId* channelId,
			    TGCIdBase::ChannelIdType type,
			    bool orChannel=false) const;
  // module connection
  TGCModuleMap* getModule (const TGCModuleId* moduleId,
			   TGCModuleId::ModuleIdType type) const;

public:
  // readin database which describes difference from ASDToPP.db
  void updateCableASDToPP(std::string filename);  

private:
  TGCCableInASD*    cableInASD;
  TGCCableASDToPP*  cableASDToPP;
  TGCCableInPP*     cableInPP;
  TGCCablePPToSLB*  cablePPToSLB;
  TGCCableInSLB*    cableInSLB;
  TGCCableSLBToHPB* cableSLBToHPB;
  TGCCableHPBToSL*  cableHPBToSL;
  TGCCableSLBToSSW* cableSLBToSSW;
  TGCCableSSWToROD* cableSSWToROD;
  TGCCabling (void) {}

  mutable std::map<int, TGCModuleId*> m_slbModuleIdMap;
  int getIndexFromReadoutWithoutChannel(const TGCIdBase::SideType side, 
					const int rodId,
					const int sswId,
					const int sbLoc) const;
};

} // end of namespace

#endif
