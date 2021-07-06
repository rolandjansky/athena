/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
         ITGCcablingSvc.h

    Author  : H.Kurashige           Aug. 2007
    Email   : Hisaya.Kurashige@cern.ch
    Description : Interface for TGCcavlingSv
                   pure abstract class
***************************************************************************/

#ifndef TGCCABLING_ITGCCABLINGSVC_H
#define TGCCABLING_ITGCCABLINGSVC_H

#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include <string>
#include <vector>
#include <algorithm>


// forward declaration
class Identifier;
class TGCCablingBase;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_TGCcablingSvc("TGCcablingSvc", 1 , 0);

class ITGCcablingSvc : public AthService, virtual public IInterface 
{
 public:
  ITGCcablingSvc(const std::string& name, ISvcLocator* svc)
   : AthService( name, svc ) {}

  // / Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_TGCcablingSvc; }

  

  // //Query interfaces of Interface
  //    @param riid       ID of Interface to be retrieved
  //     @param ppvUnknown Pointer to Location for interface pointer
  //
  virtual StatusCode queryInterface(const InterfaceID& riid,
				    void** ppvInterface) =0;

 public:
  virtual const TGCCablingBase* getTGCCabling() const=0;  

 public:
  //  TGCcablingSvc Interface
    // give max value of ReadoutID parameters
  virtual 
   void getReadoutIDRanges( int& maxRodId,
			    int& maxSswId,
			    int& maxSbloc,
			    int& minChannelId,
			    int& maxChannelId) const  =0;
  
  // give phi-range which a ROD covers  
  virtual 
    bool getCoveragefromRodID(const int rodID,
			      double & startPhi,
			      double & endPhi
			      ) const                 =0;

  virtual
    bool getCoveragefromRodID(const int rodID,
			      int & startEndcapSector,
			      int & coverageOfEndcapSector,
			      int & startForwardSector,
			      int & coverageOfForwardSector
			      ) const                =0;

  
  // Readout ID is ored
  virtual
    bool isOredChannel(const int subDetectorID,
		       const int rodID,
		       const int sswID,
		       const int sbLoc,
		       const int channelID) const =0;
  

  // Offline ID has adjacent Readout ID
  virtual 
    bool hasAdjacentChannel(const Identifier & offlineID) const =0;


  // Online ID has adjacent Readout ID
  virtual
    bool hasAdjacentChannel(const int subsystemNumber,
			    const int octantNumber,
			    const int moduleNumber,
			    const int layerNumber,
			    const int rNumber,
			    const int wireOrStrip,
			    const int channelNumber) const =0 ;


  // readout IDs -> offline IDs
  virtual
    bool getOfflineIDfromReadoutID(Identifier & offlineID,
				   const int subDetectorID,
				   const int rodID,
				   const int sswID,
				   const int sbLoc,
				   const int channelID,
				   bool orChannel=false) const =0;
  
  
  // offline IDs -> readout IDs
  virtual
    bool getReadoutIDfromOfflineID(const Identifier & offlineID,
				   int & subDetectorID,
				   int & rodID,
				   int & sswID,
				   int & sbLoc,
				   int & channelID,
				   bool adChannel=false) const =0;
  
  // offline ID -> online IDs
  virtual 
    bool getOnlineIDfromOfflineID(const Identifier & offlineID,
				  int & subsystemNumber,
				  int & octantNumber,
				  int & moduleNumber,
				  int & layerNumber,
				  int & rNumber,
				  int & wireOrStrip,
				  int & channelNumber) const =0;

  // online IDs -> offline ID
  virtual
    bool getOfflineIDfromOnlineID(Identifier & offlineID,
				  const int subsystemNumber,
				  const int octantNumber,
				  const int moduleNumber,
				  const int layerNumber,
				  const int rNumber,
				  const int wireOrStrip,
				  const int channelNumber) const=0;
  
  // readout IDs -> online IDs
  virtual
    bool getOnlineIDfromReadoutID(const int subDetectorID,
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
				  bool orChannel=false) const =0;

  // online IDs -> readout IDs
  virtual
    bool getReadoutIDfromOnlineID(int & subDetectorID,
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
				  bool adChannel=false) const =0;
  
  // element ID -> readout IDs
  virtual
    bool getReadoutIDfromElementID(const Identifier & elementID,
				   int & subdetectorID,
				   int & rodID) const =0;
  
  // readout IDs -> element ID
  virtual
    bool getElementIDfromReadoutID(Identifier & elementID,
				   const int subDetectorID,
				   const int rodID,
				   const int sswID,
				   const int sbLoc,
				   const int channelID,
				   bool orChannel=false) const =0;
  

  // HPT ID -> readout ID
  virtual
    bool getReadoutIDfromHPTID(const int phi,
			       const bool isAside,
			       const bool isEndcap,
			       const bool isStrip,
			       const int id,
			       int & subsectorID,
			       int & rodID,
			       int & sswID,
			       int & sbLoc) const  =0;

  // readout ID -> SLB ID
  virtual
    bool getSLBIDfromReadoutID(int &phi,
			       bool & isAside,
			       bool & isEndcap,
			       int & moduleType,
			       int & id,
			       const int subsectorID,
			       const int rodID,
			       const int sswID,
			       const int sbLoc) const   =0;

  // readout ID -> slbAddr
  virtual
    bool getSLBAddressfromReadoutID(int & slbAddr,
				    const int subsectorID,
				    const int rodID,
				    const int sswID,
				    const int sbLoc) const =0;

  // readout ID -> RxID
  virtual
    bool getRxIDfromReadoutID(int & rxId,
			      const int subsectorID,
			      const int rodID,
			      const int sswID,
			      const int sbLoc) const =0;
  
  // ROD_ID / SSW_ID / RX_ID -> SLB ID
  virtual
    bool getSLBIDfromRxID(int &phi,
			  bool & isAside,
			  bool & isEndcap,
			  int & moduleType,
			  int & id,
			  const int subsectorID,
			  const int rodID,
			  const int sswID,
			  const int rxId) const = 0;
  

  // SLB ID -> readout ID
  virtual
    bool getReadoutIDfromSLBID(const int phi,
			       const bool isAside,
			       const bool isEndcap,
			       const int moduleType,
			       const int id,
			       int & subsectorID,
			       int & rodID,
			       int & sswID,
			       int & sbLoc) const = 0;
  
  // readout ID -> SL ID
  virtual
    bool getSLIDfromReadoutID(int & phi,
			      bool & isAside,
			      bool & isEndcap,
			      const int subsectorID,
			      const int rodID,
			      const int sswID,
			      const int sbLoc) const  =0;
  
  // SL ID -> readout ID
  virtual
    bool getReadoutIDfromSLID(const int phi,
			      const bool isAside,
			      const bool isEndcap,
			      int & subsectorID,
			      int & rodID,
			      int & sswID,
			      int & sbLoc) const  =0;
  

  // HighPtID used in Simulation -> HighPtID in RDO
  virtual
    bool getRDOHighPtIDfromSimHighPtID(const bool isForward,
				       const bool isStrip,
				       int & index,
				       int & chip,
				       int & hitId) const =0;
  
  // HighPtID in RDO -> HighPtID used in Simulation
  virtual
    bool getSimHighPtIDfromRDOHighPtID(const bool isForward,
				       const bool isStrip,
				       int & index,
				       int & chip,
				       int & hitId) const =0;

  // high pt coincidence IDs -> offline IDs
  virtual 
    bool getOfflineIDfromHighPtID(Identifier & offlineID,
				  const int subDetectorId,
				  const int rodId,
				  const int sectorInReadout,
				  const bool isStrip,
				  const bool isForward,
				  const int index,
				  const int chip,
				  const int hitId,
				  const int hsub ) const =0;

  // offline IDs -> high pt coincidence IDs
  virtual 
    bool getHighPtIDfromOfflineID(const Identifier & offlineID,
				  int & subDetectorId,
				  int & rodId,
				  int & sectorInReadout,
				  bool & isStrip,
				  bool & isForward,
				  int & index, 
				  int & chip,
				  int & hitId,
				  int & hsub) const  = 0;

  // HPT HitID -> ROI Number
  virtual 
    bool getROINumberfromHighPtID(int &roi,
				  bool isForward,
				  int index_wire,
				  int chip_wire,
				  int hitId_wire,
				  int hsub_wire,
				  int chip_strip,
				  int hitId_strip,
				  int hsub_strip) const = 0;

  // HPT HitID -> ROI Number
  virtual
     bool getHighPtIDfromROINumber(int roi,
				   bool isForward,
				   bool isStrip,
				   int & index,
				   int & chip,
				   int & hitId,
				   int & hsub) const = 0;
  
 
  // low pt coincidence IDs -> offline IDs
  virtual
   bool getOfflineIDfromLowPtCoincidenceID(Identifier & offlineID,
					   const int subDetectorID,
					   const int rodID,
					   const int sswID,
					   const int sbLoc,
					   const int block,
					   const int pos,
					   bool middle=false) const =0;
  
  // offline IDs -> low pt coincidence IDs
  virtual 
    bool getLowPtCoincidenceIDfromOfflineID(const Identifier & offlineID,
					    int & subDetectorID,
					    int & rodID,
					    int & sswID,
					    int & sbLoc,
					    int & block,
					    int & pos,
					    bool middle=false) const =0;

 protected:  
  IntegerProperty m_AsideId;
  IntegerProperty m_CsideId;
  IntegerArrayProperty m_rodId;

  StringProperty m_databaseASDToPP;
  StringProperty m_databaseInPP;
  StringProperty m_databasePPToSL;
  StringProperty m_databaseSLBToROD;
  
  StringProperty m_databaseASDToPPdiff;
   
};

#endif
