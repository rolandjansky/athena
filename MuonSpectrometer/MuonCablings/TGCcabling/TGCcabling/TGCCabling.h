/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLING_HH
#define TGCCABLING_HH

#include "TGCcablingInterface/TGCCablingBase.h"
 
#include "TGCcabling/TGCCableInASD.h"
#include "TGCcabling/TGCCableASDToPP.h"
#include "TGCcabling/TGCCableInPP.h"
#include "TGCcabling/TGCCablePPToSLB.h"
#include "TGCcabling/TGCCableInSLB.h"
#include "TGCcabling/TGCCableSLBToHPB.h"
#include "TGCcabling/TGCCableHPBToSL.h"
#include "TGCcabling/TGCCableSLBToSSW.h"
#include "TGCcabling/TGCCableSSWToROD.h"

namespace LVL1TGCCabling8
{

class TGCCabling: public TGCCablingBase
{
public:
  // Constructor & Destructor
  TGCCabling (std::string filenameASDToPP,
	      std::string filenameInPP,
	      std::string filenamePPToSL,
	      std::string filenameSLBToROD);

  virtual ~TGCCabling (void);

  enum MAXMINREADOUTIDS { 
    MAXRODID = 7, 
    MINRODID = 0, 
    MAXSSWID = 15, 
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
					int slbId) const;
  
  
  // SLB Module -> readout ID
  bool getReadoutFromSLB (const TGCModuleSLB* slb,
			  TGCIdBase::SideType & side,
			  int & rodId,
			  int & sswId,
			  int & slbId) const;
  
  
  // readout channel -> chamber channel
  TGCChannelId* getASDOutFromReadout(TGCIdBase::SideType side,
				     int rodId,
				     int sswId,
				     int slbId,
				     int channel,
				     bool orChannel=false) const;
  

  // chamber channel -> readout channel
  bool getReadoutFromASDOut(const TGCChannelASDOut* asdout,
			    TGCIdBase::SideType & side,
			    int & rodId,
			    int & sswId,
			    int & slbId,
			    int & channel,
			    bool orChannel=false) const;
  

  // readout channel -> coincidence channel
  bool getHighPtIDFromReadout(TGCIdBase::SideType side,
			       int rodId,
			       int sswId,
			       int slbId,
			       int channel,
			       TGCIdBase::SignalType & signal,
			       TGCIdBase::RegionType & region,
			       int & sectorInOctant,
			       int & hpbId,
			       int & block,
			       int & hitId,
			       int & pos) const;
  

  // coincidence channel -> readout channel
  bool getReadoutFromHighPtID(TGCIdBase::SideType side,
			       int rodId,
			       int & sswId,
			       int & slbId,
			       int & channel,
			       TGCIdBase::SignalType signal,
			       TGCIdBase::RegionType region,
			       int sectorInOctant,
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
				      int slbId,
				      int channel,
				      int & block,
				      int & pos,
				      bool middle=false) const;
  

  // coincidence channel -> readout channel
  bool getReadoutFromLowPtCoincidence(TGCIdBase::SideType side,
				      int rodId,
				      int sswId,
				      int slbId,
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

private:
  TGCCableInASD*    m_cableInASD;
  TGCCableASDToPP*  m_cableASDToPP;
  TGCCableInPP*     m_cableInPP;
  TGCCablePPToSLB*  m_cablePPToSLB;
  TGCCableInSLB*    m_cableInSLB;
  TGCCableSLBToHPB* m_cableSLBToHPB;
  TGCCableHPBToSL*  m_cableHPBToSL;
  TGCCableSLBToSSW* m_cableSLBToSSW;
  TGCCableSSWToROD* m_cableSSWToROD;
  TGCCabling (void) {}

  mutable std::map<int, TGCModuleId*> m_slbModuleIdMap; 
  int getIndexFromReadoutWithoutChannel(const TGCIdBase::SideType side, 
					const int rodId, 
					const int sswId, 
					const int sbLoc) const;

  // hidden assignment operator and copy constructor
  TGCCabling & operator=(const TGCCabling &right);
  TGCCabling(const TGCCabling&);

};

} // end of namespace

#endif
