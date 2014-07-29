/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////
// ================================================
// InDetCalibStream - Author: Belmiro Pinto
// ================================================
//
// CalibStream.h
//
// Namespace InDet
//
// Started: September 2006

#ifndef INDETCALIBSTREAM_CALIBSTREAM_H
#define INDETCALIBSTREAM_CALIBSTREAM_H

#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ByteStreamData/RawEvent.h"

class Identifier;

class IPixelCablingSvc; 
class ISCT_CablingSvc;
class ITRT_CablingSvc;
class IRegSelSvc;
class AtlasDetectorID;

namespace InDetDD {
class PixelDetectorManager;
class SCT_DetectorManager;
}

class PixelID;
class SCT_ID;

class IROBDataProviderSvc;

namespace EventStorage {
class DataWriter;
}

class TTree;
class ITHistSvc;
class TrigInDetTrackCollection;

namespace HepMC {
  class GenParticle;
}


namespace Trig {
  class TrigDecisionTool;
}

namespace Rec {
class TrackParticleContainer;
}

class EventInfo;

////////////////////////////////////////////////////////////////////////////

namespace InDet {

class SiCluster;

class CalibStream:public AthAlgorithm {
  
 public:
  
  CalibStream(const std::string& name, ISvcLocator* pSvcLocator);
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  // **** properties ****

  /** Name of the TrackParticleCollection */
  std::string        m_trackParticleName;  
  /** Directory for the data files */
  std::string        m_inputDir;         
  /** Application Name */ 
  std::string        m_appName;        
  /** File Tag */
  std::string        m_fileTag;   
  /** stream type */
  std::string        m_streamType;
  /** stream name */
  std::string        m_streamName;      
  /** luminosity block number */ 
  long               m_lumiBlockNumber;   
  /** run number */
  long               m_run;      

  bool m_makeBS; // make bytestream file
  bool m_makeNtuple; // make ntuple
  bool m_useTrackColl; // use offline track collection as source
  bool m_useL2Muon; // use L2 muon trigger as source
  bool m_useL2Tau; // use L2 tau trigger as source
  bool m_useTRT; // include TRT information when running from trigger
  std::string m_ntupdir; // ntuple directory
  std::vector<std::string> m_triggerlist; // triggers to use
  std::vector<std::string> m_level1list; // L1 source triggers to identify
  ToolHandle<Trig::TrigDecisionTool> m_trigDec;
  ServiceHandle<IRegSelSvc> m_regselsvc;
  float m_trtdeleta; // extra width added to road around SI tracks
  float m_trtdelphi; // when extrapolating to TRT


  // ********************
  
  ServiceHandle<IToolSvc>               m_toolSvc; //!< Pointer to the tool service
  
  AtlasDetectorID* 			m_idHelper;

  ServiceHandle<IPixelCablingSvc>     m_pixIdMapping;  
  ServiceHandle<ISCT_CablingSvc>      m_sctCablingSvc;
  ServiceHandle<ITRT_CablingSvc>      m_trtIdMapping;

  // ID helpers
  const PixelID* m_pixid;
  const SCT_ID* m_sctid;
  
  ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;
  
  EventStorage::DataWriter * 		m_dw; /** pointer to DataWriter */         
  int m_crun; // current run
  int m_cevent; // current event
  TTree* m_trknt;

  ServiceHandle<ITHistSvc> p_thistsvc;

  std::vector<uint32_t> m_robIDs;

  std::vector<unsigned int> m_seentrig;
  std::vector<unsigned int> m_seenl1;

  /**  Construct a list of selected ROBs 
   This method makes a list of selected ( criteria written bellow ) robs (robIds) starting
   from TrackParticleContainer (tpContainer).
   Return: the status code of the method. */

  StatusCode getROBList(const Rec::TrackParticleContainer *tpContainer, std::vector<uint32_t> & robIDs);
 
  /** Make a BS from a list of ROBs 
   This method makes a memory ByteStream (dest) from user proveded list of ROBs (robIds)
   Return : length of changed pointer (dest) in units of sizeof(uint32_t). */

  uint32_t robSelector(const RawEvent* re, uint32_t* &dest, std::vector<uint32_t> & robIDs);
  
  /** Get subdetector id */
//  uint8_t subId(uint32_t id);
  
  /** Test if subId belongs to Inner Detector */
  bool testId(uint32_t sd);

  // write the bytestream data for the found list of ROBs
  unsigned long writeByteStream();
  // setup the ROB list from a TrackParticleContainer
  StatusCode seedFromTrackColl();
  // setup the ROB list from LVL2 trigger TrigInDetTrackCollection
  StatusCode seedFromL2Trigger();
  // book the optional calibration ntuple
  bool bookCalibNtuple();
  // setup truth information for ntuple
  void setTruth(const HepMC::GenParticle* particle, const int itrk);
  // fill the calibraton ntuple for a given RoI
  void fillCalibNtuple(const TrigInDetTrackCollection* tracks, 
		       const int roi,const int trigmask);
  // check if the RoI is selected
  bool isSelected();
  // fill the ROB list for LVL2 trigger mode, based on isSelected
  void getROBListTrigIndet(const TrigInDetTrackCollection* tracks);
  // helpers for getting wafers and identifying overlaps
  const Identifier waferId(const InDet::SiCluster* cluster, bool& edge);
  bool pixelEdge(const Identifier& ident);
};

}

#endif // MYNEWPACKAGE_MYALG_H
