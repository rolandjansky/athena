/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
         TGCcablingSvc.h

    Author  : Tadashi Maeno
              H.Kurashige           Sep. 2007
    Email   : Hisaya.Kurashige@cern.ch
    Description : online-offline ID mapper for TGC

***************************************************************************/

#ifndef TGCCABLING_TGCCABLINGSVC_H
#define TGCCABLING_TGCCABLINGSVC_H

class StatusCode;
class Identifier;
class TgcIdHelper;

#include <string>
#include <vector>
#include <algorithm>

#include "GaudiKernel/Service.h"

#include "GaudiKernel/MsgStream.h"

#include "TGCcablingInterface/ITGCcablingSvc.h"
#include "TGCcabling/TGCCabling.h"

class TGCcablingSvc : public ITGCcablingSvc
{
public:
  TGCcablingSvc (const std::string& name, ISvcLocator* svc);
  virtual ~TGCcablingSvc()=default;

  static const InterfaceID& interfaceID() 
  {
    return  IID_TGCcablingSvc;
  }

  StatusCode queryInterface(const InterfaceID& riid,void** ppvIF); 
  
  StatusCode initialize (void);
  StatusCode finalize (void);

   //////  virtual methods of ITGCcabling12Svc
  
  virtual TGCCablingBase* getTGCCabling() const;  


  // give max value of ReadoutID parameters
  void getReadoutIDRanges( int& maxRodId,
			   int& maxSswId,
			   int& maxSlbId,
			   int& minChannelId,
			   int& maxChannelId) const;

  // give phi-range which a ROD covers  
  bool getCoveragefromRodID(const int rodID,
			    double & startPhi,
			    double & endPhi
			    ) const;

  bool getCoveragefromRodID(const int rodID,
			    int & startEndcapSector,
			    int & coverageOfEndcapSector,
			    int & startForwardSector,
			    int & coverageOfForwardSector
			    ) const;

  
  // Readout ID is ored
  bool isOredChannel(const int subDetectorID,
		     const int rodID,
		     const int sswID,
		     const int slbID,
		     const int channelID) const;


  // Offline ID has adjacent Readout ID
  bool hasAdjacentChannel(const Identifier & offlineID) const;


  // Online ID has adjacent Readout ID
  bool hasAdjacentChannel(const int subsystemNumber,
			  const int octantNumber,
			  const int moduleNumber,
			  const int layerNumber,
			  const int rNumber,
			  const int wireOrStrip,
			  const int channelNumber) const;


  // readout IDs -> offline IDs
  bool getOfflineIDfromReadoutID(Identifier & offlineID,
				 const int subDetectorID,
				 const int rodID,
				 const int sswID,
				 const int slbID,
				 const int channelID,
				 bool orChannel=false) const;

  
  // offline IDs -> readout IDs
  bool getReadoutIDfromOfflineID(const Identifier & offlineID,
				 int & subDetectorID,
				 int & rodID,
				 int & sswID,
				 int & slbID,
				 int & channelID,
				 bool adChannel=false) const;

  // offline ID -> online IDs
  bool getOnlineIDfromOfflineID(const Identifier & offlineID,
				int & subsystemNumber,
				int & octantNumber,
				int & moduleNumber,
				int & layerNumber,
				int & rNumber,
				int & wireOrStrip,
				int & channelNumber) const;

  // online IDs -> offline ID
  bool getOfflineIDfromOnlineID(Identifier & offlineID,
				const int subsystemNumber,
				const int octantNumber,
				const int moduleNumber,
				const int layerNumber,
				const int rNumber,
				const int wireOrStrip,
				const int channelNumber) const;

  // readout IDs -> online IDs
  bool getOnlineIDfromReadoutID(const int subDetectorID,
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
				bool orChannel=false) const;

  // online IDs -> readout IDs
  bool getReadoutIDfromOnlineID(int & subDetectorID,
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
				bool adChannel=false) const;
  
  // element ID -> readout IDs
  bool getReadoutIDfromElementID(const Identifier & elementID,
				 int & subdetectorID,
				 int & rodID) const;
  
  // readout IDs -> element ID
  bool getElementIDfromReadoutID(Identifier & elementID,
				 const int subDetectorID,
				 const int rodID,
				 const int sswID,
				 const int slbID,
				 const int channelID,
				 bool orChannel=false) const;

  // readout ID -> HPT ID  
  bool getHPTIDfromReadoutID(int & phi,
			     bool & isAside,
			     bool & isEndcap,
			     bool & isStrip,
			     int & id,
			     const int subsectorID,
			     const int rodID,
			     const int sswID,
			     const int slbID) const;

  // HPT ID -> readout ID
  bool getReadoutIDfromHPTID(const int phi,
			     const bool isAside,
			     const bool isEndcap,
			     const bool isStrip,
			     const int id,
			     int & subsectorID,
			     int & rodID,
			     int & sswID,
			     int & slbID) const;

  // readout ID -> SLB ID
  bool getSLBIDfromReadoutID(int &phi,
			     bool & isAside,
			     bool & isEndcap,
			     int & moduleType,
			     int & id,
			     const int subsectorID,
			     const int rodID,
			     const int sswID,
			     const int slbID) const;

  // readout ID -> slbAddr
  //  SLBAddressis  Not Valid for TGCcabling for Octant segmentation
  bool getSLBAddressfromReadoutID(int & slbAddr,
                                  const int subsectorID,
                                  const int rodID,
                                  const int sswID,
                                  const int slbID) const;

 
  // readout ID -> RxID
  //  rxId is same as slbID for TGCcabling for Octant segmentation
  bool getRxIDfromReadoutID(int & rxId,
			    const int subsectorID,
			    const int rodID,
			    const int sswID,
			    const int sbLoc) const;
  
 // ROD_ID / SSW_ID / RX_ID -> SLB ID
  //  rxId is same as slbID for TGCcabling for Octant segmentation
  bool getSLBIDfromRxID(int &phi,
                        bool & isAside,
                        bool & isEndcap,
                        int & moduleType,
                        int & id,
                        const int subsectorID,
                        const int rodID,
                        const int sswID,
                        const int rxId) const;
  
  // SLB ID -> readout ID
  bool getReadoutIDfromSLBID(const int phi,
			     const bool isAside,
			     const bool isEndcap,
			     const int moduleType,
			     const int id,
			     int & subsectorID,
			     int & rodID,
			     int & sswID,
			     int & slbID) const;
  
  // readout ID -> SL ID
  bool getSLIDfromReadoutID(int & phi,
			    bool & isAside,
			    bool & isEndcap,
			    const int subsectorID,
			    const int rodID,
			    const int sswID,
			    const int slbID) const;

  // SL ID -> readout ID
  bool getReadoutIDfromSLID(const int phi,
			    const bool isAside,
			    const bool isEndcap,
			    int & subsectorID,
			    int & rodID,
			    int & sswID,
			    int & slbID) const;

  // HighPtID used in Simulation -> HighPtID in RDO
  bool getRDOHighPtIDfromSimHighPtID(const bool isForward,
                                     const bool isStrip,
                                     int & index,
                                     int & chip,
                                     int & hitId) const;
 
  // HighPtID in RDO -> HighPtID used in Simulation
  bool getSimHighPtIDfromRDOHighPtID(const bool isForward,
                                     const bool isStrip,
                                     int & index,
                                     int & chip,
                                     int & hitId) const;

  // high pt coincidence IDs -> offline IDs
  virtual 
    bool getOfflineIDfromHighPtID(Identifier & offlineID,
				  const int subDetectorID,
				  const int rodID,
				  const int sectorInReadout,
				  const bool isStrip,
				  const bool isForward,
				  const int hpb,
				  const int chip,
				  const int hitID,
				  const int pos ) const ;

  // offline IDs -> high pt coincidence IDs
  virtual 
    bool getHighPtIDfromOfflineID(const Identifier & offlineID,
				  int & subDetectorID,
				  int & rodID,
				  int & sectorInReadout,
				  bool & isStrip,
				  bool & isForward,
				  int & hpb,
				  int & chip,
				  int & hitID,
				  int & pos) const;

  // HPT HitID -> ROI Number
  virtual 
    bool getROINumberfromHighPtID(int &roi,
				  bool isForward,
				  int hpb_wire,
				  int chip_wire,
				  int hitId_wire,
				  int sub_wire,
				  int chip_strip,
				  int hitId_strip,
				  int sub_strip) const;
  // HPT HitID -> ROI Number
  virtual
   bool getHighPtIDfromROINumber(int roi,
				 bool isForward,
				 bool isStrip,
				 int & hpb,
				 int & chip,
				 int & hitID,
				 int & sub) const;
  
  // low pt coincidence IDs -> offline IDs
  virtual
   bool getOfflineIDfromLowPtCoincidenceID(Identifier & offlineID,
					   const int subDetectorID,
					   const int rodID,
					   const int sswID,
					   const int sbLoc,
					   const int block,
					   const int pos,
					   bool middle=false) const;
  
  // offline IDs -> low pt coincidence IDs
  virtual 
    bool getLowPtCoincidenceIDfromOfflineID(const Identifier & offlineID,
					    int & subDetectorID,
					    int & rodID,
					    int & sswID,
					    int & sbLoc,
					    int & block,
					    int & pos, 
					    bool middle=false) const;

  /////////////////////////////////////////////////////////////
  // channel connection
  LVL1TGCCabling8::TGCChannelId* 
    getChannel (const LVL1TGCCabling8::TGCChannelId* channelId,
		TGCIdBase::ChannelIdType type,
		bool orChannel=false) const;
  
  // module connection
  LVL1TGCCabling8::TGCModuleMap* 
    getModule (const LVL1TGCCabling8::TGCModuleId* moduleId,
	       LVL1TGCCabling8::TGCModuleId::ModuleIdType type) const;
  

 ///////////////////////  
private:
  const TgcIdHelper * m_idHelper;
  LVL1TGCCabling8::TGCCabling * m_cabling;

};

inline
 TGCCablingBase* TGCcablingSvc::getTGCCabling() const
{
  return m_cabling;
}  

#endif

