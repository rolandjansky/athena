/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////
// ================================================
// InDetCalibStream - Author: Belmiro Pinto
// ================================================
//
// CalibStream.cxx
//
// Namespace InDet
//
// Started: September 2006

#include "InDetCalibStream/CalibStream.h"

#include <stdint.h>
#include <map>
#include "GaudiKernel/SvcFactory.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
                     
#include "InDetReadoutGeometry/SiDetectorManager.h"          
#include "PixelCabling/IPixelCablingSvc.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "TRT_Cabling/ITRT_CablingSvc.h"
#include "InDetIdentifier/PixelID.h"   
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
 
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "Particle/TrackParticleContainer.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "EventStorage/DataWriter.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"
#include "eformat/Issue.h"

using namespace EventStorage; 

namespace InDet {

/////////////////////////////////////////////////////////////////////////////

CalibStream::CalibStream(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_regselsvc("RegSelSvc",name),
  m_toolSvc ("ToolSvc",name),
  m_pixIdMapping ("PixelCablingSvc",name),
  m_sctCablingSvc("SCT_CablingSvc",name),
  m_trtIdMapping ("TRT_IdMapping",name),  
  m_robDataProvider ("ROBDataProviderSvc",name),
  m_dw(0),
  p_thistsvc ("THistSvc",name)
{
  declareProperty("TrackParticlesName",m_trackParticleName);
  declareProperty("IToolSvc",m_toolSvc);
  declareProperty("ITHistSvc",p_thistsvc);
  declareProperty("IROBDataProviderSvc",m_robDataProvider);
  declareProperty("IRegSelSvc",m_regselsvc);
  declareProperty("SCT_CablingSvc",m_sctCablingSvc);
  //new for files name
  declareProperty("OutputDirectory", m_inputDir="./");
  declareProperty("AppName",         m_appName="Athena");
  declareProperty("FileTag",         m_fileTag="csc13");
  declareProperty("RunNumber",       m_run=0);
  declareProperty("StreamType",      m_streamType="Single");
  declareProperty("StreamName",      m_streamName="Stream");
  declareProperty("LumiBlockNumber", m_lumiBlockNumber=0);

  declareProperty("MakeByteStream",m_makeBS=true);
  declareProperty("MakeNtuple",m_makeNtuple=false);
  declareProperty("UseTrackCollection",m_useTrackColl=true);
  declareProperty("UseL2Muon",m_useL2Muon=false);
  declareProperty("UseL2Tau",m_useL2Tau=false);
  declareProperty("UseTRT",m_useTRT=false);
  declareProperty("NtupleDirectory",m_ntupdir="/IDCALIB");
  declareProperty("TriggerList",m_triggerlist);
  declareProperty("Level1List",m_level1list);
  declareProperty("TrigDecisionTool",m_trigDec);
  declareProperty("TRTDelEta",m_trtdeleta=0.05);
  declareProperty("TRTDelPhi",m_trtdelphi=0.05);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode CalibStream::initialize(){
                                                                                                                         
  if (m_toolSvc.retrieve().isFailure())
    {
      msg(MSG::ERROR) << "Unable to get ToolSvc!" << endreq;
      return StatusCode::FAILURE;
    };
  
const AtlasDetectorID * m_idHelper;
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    msg(MSG::FATAL) << "Could not get AtlasDetectorID helper" << endreq;
    return StatusCode::FAILURE;
  }

 // Pixel
if (detStore()->retrieve(m_pixid, "PixelID").isFailure()) {
     msg(MSG::FATAL) << "Could not get Pixel ID helper" << endreq;
     return StatusCode::FAILURE;
}
 
// SCT
if (detStore()->retrieve(m_sctid, "SCT_ID").isFailure()) {
     msg(MSG::FATAL) << "Could not get SCT ID helper" << endreq;
     return StatusCode::FAILURE;
}

  std::string toolType;
  
  // Retrieve pixel id mapping 
  if (m_pixIdMapping.retrieve().isFailure()) {
     msg(MSG::FATAL) << "Failed to retrieve tool " << m_pixIdMapping << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved tool " << m_pixIdMapping << endreq;

  // Retrieve SCT cabling svc
  if (m_sctCablingSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_sctCablingSvc << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved service " << m_sctCablingSvc << endreq;   

  // Retrieve TRT id mapping 
  if (m_trtIdMapping.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve tool " << m_trtIdMapping << endreq;
    return StatusCode::FAILURE;
  } else 
    msg(MSG::INFO) << "Retrieved tool " << m_trtIdMapping << endreq;

  if (m_robDataProvider.retrieve().isFailure()) {
    msg(MSG::FATAL) << " Cant get ROBDataProviderSvc " << m_robDataProvider << endreq;
    return StatusCode::FAILURE;
  } else
    msg(MSG::INFO) << "Retrieved service " << m_robDataProvider << endreq;

  if (m_useL2Muon || m_useL2Tau) {
    if (StatusCode::SUCCESS!=m_trigDec.retrieve()) {
      msg(MSG::FATAL) << "Cannot retrieve TrigDecisionTool" << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "Trigger decision tool OK at " << m_trigDec << endreq;

    // dump out bit assignments and reset counters
    m_seentrig.resize(m_triggerlist.size());
    for (unsigned int ibit=0;ibit<m_triggerlist.size();++ibit) {
      msg(MSG::INFO) << "TrigMask " << ibit << " " << m_triggerlist[ibit] << endreq;
      m_seentrig[ibit]=0;
    }
    m_seenl1.resize(m_level1list.size());
    for (unsigned int ibit=0;ibit<m_level1list.size();++ibit) {
      msg(MSG::INFO) << "LV1Mask " << ibit << " " << m_level1list[ibit] << endreq;
      m_seenl1[ibit]=0;
    }
    if (m_regselsvc.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve RegSelSvc (RegionSelector)" << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Got RegionSelector for adding TRT information" << endreq;
      msg(MSG::INFO) << "Window half size deta=" << m_trtdeleta << " dphi=" << m_trtdelphi << endreq;
    }
  }

  if (m_makeNtuple) {
    if (p_thistsvc.retrieve().isFailure()) {
      msg(MSG::FATAL) << "Could not find THistSvc" << endreq;
      return StatusCode::FAILURE;
    }
    if (bookCalibNtuple()) {
      msg(MSG::INFO) << "Booked calibration ntuple" << endreq;
    } else {
      msg(MSG::ERROR) << "Unable to book calibration ntuple" << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode CalibStream::execute() {
  
  StatusCode sc;
  
  //Retrieve EventInfo to get run number
  const EventInfo * eventInfo=0;                      
  
  sc = sgSvc()->retrieve(eventInfo);
  if (sc.isFailure() || eventInfo == 0) {
    msg(MSG::ERROR) << "Unable to retrieve EventInfo. Exiting !" << endreq;
    return StatusCode::SUCCESS;
  } else {
    m_crun=eventInfo->event_ID()->run_number();
    m_cevent=eventInfo->event_ID()->event_number();
  }
  
  m_robIDs.clear();

  if (m_useTrackColl) {
    sc=seedFromTrackColl();
  } else if (m_useL2Muon || m_useL2Tau) {
    sc=seedFromL2Trigger();
  }
  if (sc==StatusCode::SUCCESS) {
    // only write out if we found a RoB list
    if (m_makeBS && m_robIDs.size()>0) {
      unsigned long eventSize=writeByteStream();
      if (msgLvl(MSG::DEBUG)) msg() << "Written event of size " << eventSize << " ("
          << m_robIDs.size() << " ROBs)" << endreq;                          
    }
  }
  return sc;
}

StatusCode CalibStream::seedFromTrackColl() {
  // start from offline track collection
  const Rec::TrackParticleContainer * tpContainer(0);

  // Retrieve TrackParticles from StoreGate 
  StatusCode sc;
  sc = sgSvc()->retrieve(tpContainer, m_trackParticleName);
  if (sc.isFailure() || tpContainer == 0) {
    msg(MSG::ERROR) << "Unable to retrieve " << m_trackParticleName << endreq;
    return StatusCode::FAILURE;
  }
  
  getROBList(tpContainer, m_robIDs);
  return StatusCode::SUCCESS;
}

unsigned long CalibStream::writeByteStream() {
  // write the bytestream specified by the existing RobList
  // returns event size (in 32-bit words) or zero in case of failure

  //MsgStream log(msgSvc(), name());

  unsigned long eventSize=0; 
  uint32_t *buf=0;
  
  if (m_dw==0) {
    
    run_parameters_record rPar;
    rPar.marker = run_parameters_pattern.marker; 
    rPar.record_size = run_parameters_pattern.record_size; 
    rPar.run_number = run_parameters_pattern.run_number; 
    rPar.max_events = run_parameters_pattern.max_events;
    rPar.rec_enable = run_parameters_pattern.rec_enable;
    rPar.trigger_type = run_parameters_pattern.trigger_type;
    rPar.detector_mask =  ( ((uint64_t)run_parameters_pattern.detector_mask_2of2) << 32) + run_parameters_pattern.detector_mask_1of2;
    rPar.beam_type = run_parameters_pattern.beam_type;
    rPar.beam_energy = run_parameters_pattern.beam_energy;  
    
    rPar.run_number = m_crun;
    
    // construct file name
    // std::string fileName = daq::AgreedFileName(m_fileTag,m_run,m_streamType,
    //                                          m_streamName,m_lumiBlockNumber,
    //                                         m_appName);
    
    EventStorage::freeMetaDataStrings metaStrings;
    m_dw = new DataWriter(m_inputDir,"fileName",rPar, metaStrings); 
    
    if(! m_dw->good() ) { 
      msg(MSG::ERROR) <<" Unable to initialize file "<< endreq;
      return 0;
    }     
  }  
  
  const RawEvent * re =  m_robDataProvider->getEvent();
  if (!re)
     {
       msg(MSG::ERROR) << " Can not get RawEvent " << endreq;
       return 0;
     }
  
  //uint32_t size = RE->fragment_size_word();
  
  eventSize = robSelector(re,buf,m_robIDs);
  
  if ( m_dw->putData(eventSize*sizeof(uint32_t),static_cast<void *>(buf))==DWNOOK ){
      msg(MSG::ERROR) << " Can not putData " << endreq;
      return 0;
  }
  
  delete  buf;
  buf = 0;
  
  return eventSize;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode CalibStream::finalize() {
  
  if (m_dw) m_dw->closeFile();

  // can not delete m_dw. If deleted we get a second empty file. 
  // delete m_dw;

  delete m_idHelper;
  m_idHelper = 0;
  

  //TODO: change code to new EventStorage version
  //log << MSG::INFO  << "Events in run " << std::dec << m_dw->eventsInRun() << endreq;
  //log << MSG::INFO  << "MB in run " << std::dec << m_dw->dataMB_InRun() << endreq;
  // if running from trigger, list out statistics
  if (m_useL2Muon || m_useL2Tau) {
    for (unsigned int ibit=0;ibit<m_triggerlist.size();++ibit) {
      msg(MSG::INFO) << "Saw " << m_seentrig[ibit] << " RoIs from "
          << m_triggerlist[ibit] << "(" << ibit << ")" << endreq;

    }
    for (unsigned int ibit=0;ibit<m_level1list.size();++ibit) {
      if (m_seenl1[ibit]>0) 
        msg(MSG::INFO) << "Saw " << m_seenl1[ibit] << " RoIs from "
            << m_level1list[ibit] << "(" << ibit << ")" << endreq;

    }
  }

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------------------------------

/*
   This method makes a list of selected ( criteria written bellow ) robs (robIds) starting 
   from TrackParticleContainer (tpContainer).
   Return: the status code of the method.

   Work Method:
	Iterates through: TrackParticle->TrackStateOnSurface. The surfaces to be considered must
        contain simultaneously a MeasurementBase and Trk::TrackParameters, or in other words, hits 
        and parameters that are grouped together by the same Trk::Surface they exist on. Get ROB ids 
        from the considered surfaces and stored them in the a list (robIds).          
*/



StatusCode CalibStream::getROBList(const Rec::TrackParticleContainer *tpContainer, std::vector<uint32_t> & robIDs) 
{
  StatusCode sc;  
  
  //MsgStream log(msgSvc(), name());
  
  // Get the total numbers of tracks
  //log << MSG::INFO << "Total event track number:"<< tpContainer->size() << endreq;
  
  typedef DataVector<Rec::TrackParticle>::const_iterator TPIter;
  
    for ( TPIter fz=tpContainer->begin(); fz!=tpContainer->end() ; ++fz) {

    const Trk::Track* ptrTrk = (*fz)->originalTrack();
    
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it = ptrTrk->trackStateOnSurfaces()->begin();
    
    //long num=0;

    //log.setLevel(MSG::INFO-1);
    //if (log.level()<MSG::INFO) log << "Level provisoriamente DEBUG\n";

    for (;it != ptrTrk->trackStateOnSurfaces()->end();++it) {
      //log<< MSG::DEBUG <<" --------- Start of TrackStateOnSurface \t"<<num<<"\t-------"<<endreq;
      //log<< MSG::DEBUG <<(**it);
      
      //std::string className;
      
      if ((static_cast<bool>((*it)->measurementOnTrack()))&&(static_cast<bool>((*it)->trackParameters()))) {
	  
	  const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>((*it)->measurementOnTrack());
	  
	  if (rio != 0) {
	    
	    const Identifier& idPRD = rio->prepRawData()->identify();
	   
	    uint32_t robs=0;
	    vector<uint32_t>::iterator result;
	    
	      if (m_idHelper->is_pixel(idPRD)) {
		
		//log << MSG::INFO << " This is a PIXEL hit " << endreq;

                const Identifier& idW  = m_pixid->wafer_id(idPRD);

		robs = m_pixIdMapping->getRobID(idW);

		result = find( robIDs.begin(), robIDs.end(), robs);
		
		if (result == robIDs.end()) {
		  robIDs.push_back(robs);
		}
		else {
		  //log << MSG::INFO << "Found a matching element: " << *result << endreq;
		}
	      }
	      else if (m_idHelper->is_sct(idPRD)) {

		//log << MSG::INFO << " This is a SCT hit " << endreq;

                const Identifier& idW  = m_sctid->wafer_id(idPRD);
		
		robs = m_sctCablingSvc->getRobIdFromOfflineId(idW);

		result = find( robIDs.begin(), robIDs.end(), robs);
		if (result == robIDs.end()) {
		  robIDs.push_back(robs);
		}
		else {
		  //log << MSG::INFO << "Found a matching element: " << *result << endreq;
		}
		
	      }
	      else if (m_idHelper->is_trt(idPRD)) {

		//log << MSG::INFO << " This is a TRT hit " << endreq;

		std::vector< uint32_t > robTRTId;
		const Identifier idE = rio->detectorElement()->identify();
		const Identifier& tests =  idE;
                  
		robTRTId = m_trtIdMapping->getRobID((Identifier&)(tests));
		
		std::vector<uint32_t>::const_iterator it_iT = robTRTId.begin();
		std::vector<uint32_t>::const_iterator it_fT = robTRTId.end();
		
		for(; it_iT!=it_fT;it_iT++) {
		  
		  robs = *it_iT;

		  result = find( robIDs.begin(), robIDs.end(), robs);
		  
		  if (result == robIDs.end()) {
		    robIDs.push_back(robs);
		  }
		  else {
		    //log << MSG::INFO << "Found a matching element: " << *result << endreq;
		  }
		}
	      }
	      else {
                msg(MSG::ERROR) << " One module is neither PIXEL nor SCT nor TRT! " << endreq;
	      }
	  }
      }
      //log<< MSG::DEBUG<< " ---------   End of TrackStateOnSurface \t"<<num++<<"\t-------"<<endreq;
    }
    //log.setLevel(MSG::INFO);
  }
  if (sc.isFailure()) { msg(MSG::WARNING) << "One or more track indices could not be recorded!" << endreq; }
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------------------------------

/*
   This method makes a memory ByteStream (dest) from user proveded list of ROBs (robIds)
   Return : length of changed pointer (dest) in units of sizeof(uint32_t) 
	
   Work Method:
       Extract fragment objects from RawEvent
       Iterates through: FullEventFragment -> SubDetectorFragment -> ROSFragment -> ROBFragment.
       Select SubDetectorFragments belonging to Inner Detector, selected by testId() method, 
       and select the ROBFragments provided by the (robIds) list  	  
       Create new OFFLINE_FRAGMENTS_NAMESPACE_WRITE objects to each of the select objects
       Appends all OFFLINE_FRAGMENTS_NAMESPACE_WRITE objects to each other in hierarchical order 
       Use the eformat::write::copy to perform a memcpy like operation of the 'new' event
       from created OFFLINE_FRAGMENTS_NAMESPACE_WRITE objects to the (dest) buffer   	 
*/

uint32_t CalibStream::robSelector(const RawEvent * fe, uint32_t * &dest, std::vector<uint32_t> & robIDs)
{
  //long nros_counter=0, nsd_counter=0; 
  
  // NEW
  fe->check();
 
  OFFLINE_FRAGMENTS_NAMESPACE_WRITE::FullEventFragment nfe(fe->source_id(), fe->bc_time_seconds(),                                                                                    fe->bc_time_nanoseconds(), fe->global_id(),                                                                                fe->run_type(), fe->run_no(), fe->lumi_block(), 
                                                           fe->lvl1_id(), fe->bc_id(), fe->lvl1_trigger_type());

  const OFFLINE_FRAGMENTS_NAMESPACE::DataType * d;
  fe->lvl1_trigger_info(d);
  nfe.lvl1_trigger_info(fe->nlvl1_trigger_info(),d);
  fe->lvl2_trigger_info(d);
  nfe.lvl2_trigger_info(fe->nlvl2_trigger_info(), d);
  fe->event_filter_info(d);
  nfe.event_filter_info(fe->nevent_filter_info(), d);
  fe->stream_tag(d);
  nfe.stream_tag(fe->nstream_tag(), d);
  fe->status(d);
  nfe.status(fe->nstatus(), d);
  //FIXME??
  //nfe.minor_version(version.minor_version());
  //nfe.checksum_type(event_checksum);
  
  std::vector<OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment*> acc_rob;
  for (size_t i=0; i<fe->nchildren(); ++i) {
     fe->child(d,i);		
     try {
       OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment rob(d);
       rob.check();  
       
       uint32_t source_id = rob.source_id();
       
       if (!testId(source_id)) continue;
       
       vector<uint32_t>::iterator selectROB;
       selectROB = find( robIDs.begin(), robIDs.end(), source_id);
       
       if(selectROB == robIDs.end()) {continue;}
       
       d = 0;
       rob.rod_data(d);
       long ndata=rob.rod_ndata();
       
       OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment* nrob = 
	 new OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment(rob.source_id(), rob.rod_run_no(), rob.rod_lvl1_id(),                                                                      rob.rod_bc_id(), rob.rod_lvl1_trigger_type(),                                                                              rob.rod_detev_type(), ndata, d, rob.rod_status_position());
 
       rob.status(d);
       nrob->status(rob.nstatus(), d);rob.rod_status(d);		
       nrob->rod_status(rob.rod_nstatus(), d);
       //helper::Version rob_version(rob.rod_version());
       //nrob->minor_version(rob_version.minor_version());
       //helper::Version rod_version(rob.rod_version());
       //nrob->rod_minor_version(rod_version.minor_version());
       //nrob->checksum_type(rob_checksum);
       
       //make this new ROB part of the new ROS
       nfe.append(nrob);
       //make sure we don't forget to delete this guy
       acc_rob.push_back(nrob);
     }
     catch (eformat::Issue& e) {
       //ers::warning(e);
       //ers::warning(EFORMAT_SKIPPED_FRAGMENT("ROB", "ROS", ros.source_id())); 
       //we skip this fragment, but do not loose the whole event
       continue;
     }
  }
  

  uint32_t rawsize = nfe.size_word();
  
  dest = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[rawsize];
  
  uint32_t retval = eformat::write::copy(*(nfe.bind()),dest,rawsize);
  
  //delete the allocated stuff
  for (size_t i=0; i<acc_rob.size(); ++i) delete acc_rob[i];

  acc_rob.clear();

  return retval;
}
//---------------------------------------------------------------------------------------------------------------

/*
  Test if subdetector id is a Inner Detector's id    
*/
bool CalibStream::testId(uint32_t id)
{  
  eformat::helper::SourceIdentifier sid(id);
  eformat::SubDetectorGroup sd = sid.subdetector_group();
  using namespace eformat;
  switch(sd) {
  case  eformat::PIXEL:
  case  eformat::SCT:
  case  eformat::TRT:
     return 1; break;
  default:
    return 0;
    break;
  }
  return 0;
}

StatusCode CalibStream::seedFromL2Trigger() {
  // start from LVL2 trigger information using list of signatures to look at
  if (msgLvl(MSG::DEBUG)) msg() << "In seedFromL2Trigger for event " << m_cevent << endreq;
  // keep a map of track collections to look at, and the triggers they pass
  std::map<const TrigInDetTrackCollection*,int> trigmap;
  for (unsigned int i=0;i<m_triggerlist.size();++i) {
    if (m_trigDec->isPassed(m_triggerlist[i])) {
      if (msgLvl(MSG::DEBUG)) msg() << "Trigger " << m_triggerlist[i] << " was passed" <<
        endreq;
	//endreq;
//       const std::vector<const HLT::TriggerElement*>* TEs=
// 	m_trigDec->getChainTEs(m_triggerlist[i]);
//       if (TEs!=0) {
// 	for (std::vector<const HLT::TriggerElement*>::const_iterator iTE=
// 	       TEs->begin();iTE!=TEs->end();++iTE) {
// 	  // for release 14
// 	  //std::vector<const TrigInDetTrackCollection*> setsOfTracks;
// 	  //m_trigDec->getFeatures(iTE,setsOfTracks);
// 	  //log << MSG::INFO << "TE has " << setsOfTracks.size() << " trackcol"
// 	  //    << endreq;
// 	  //for (std::vector<const TrigInDetTrackCollection*>::const_iterator
// 	  //   itcol=setsOfTracks.begin();itcol!=setsOfTracks.end();++itcol) {
// 	  const TrigInDetTrackCollection* tcol;
// 	  if (HLT::OK==m_trigDec->getFeature(*iTE,tcol,"TrigIDSCAN_Tau")) {
// 	    int mask=0;
// 	    if (trigmap.find(tcol)!=trigmap.end()) mask=trigmap[tcol];
// 	    mask=mask | (1 << i);
// 	    trigmap[tcol]=mask;
// 	    ++m_seentrig[i];
// 	  }
// 	}
//       } else {
//         if (msgLvl(MSG::DEBUG)) msg() << "TE pointer is zero!" << endreq; 
//       }

      // the same for the new TDT

      Trig::FeatureContainer fc = m_trigDec->features(m_triggerlist[i]);
      const std::vector<Trig::Feature<TrigInDetTrackCollection> >
        trkColls = fc.get<TrigInDetTrackCollection>("TrigIDSCAN_Tau");
        
      if (!trkColls.empty()) {
        for (std::vector<Trig::Feature<TrigInDetTrackCollection> >::const_iterator featIt=trkColls.begin();
             featIt!=trkColls.end(); ++featIt) {
          // for release 15
          const TrigInDetTrackCollection* tcol = *featIt;
          if (tcol) {
            int mask=0;
            if (trigmap.find(tcol)!=trigmap.end()) mask=trigmap[tcol];
            mask=mask | (1 << i);
            trigmap[tcol]=mask;
            ++m_seentrig[i];
          }
        }
      } else {
        if (msgLvl(MSG::DEBUG)) msg() << "TE pointer is zero!" << endreq; 
      }

    }
  }
  if (msgLvl(MSG::DEBUG)) msg() << "Total of " << trigmap.size() <<
    " collections to fill in ntuple" << endreq;
  int roi=0;
  for (std::map<const TrigInDetTrackCollection*,int>::const_iterator itr=
	   trigmap.begin();itr!=trigmap.end();++itr,++roi) 
    fillCalibNtuple(itr->first,roi,itr->second);
  return StatusCode::SUCCESS;
}

// RJH -  calibration ntuple definition
#define MAXTRK 256
int trknt_run;
int trknt_event;
int trknt_roi;
unsigned int trknt_trigmask;
unsigned int trknt_l1mask1;
unsigned int trknt_l1mask2;
int trknt_nsirob;
int trknt_ntrtrob;
float trknt_deleta;
float trknt_delphi;
int trknt_ntrk;
float trknt_a0[MAXTRK];
float trknt_phi0[MAXTRK];
float trknt_z0[MAXTRK];
float trknt_eta[MAXTRK];
float trknt_pt[MAXTRK];
float trknt_chi2[MAXTRK];
int trknt_author[MAXTRK];
int trknt_nsihit[MAXTRK];
int trknt_ntrthit[MAXTRK];
int trknt_noverlap[MAXTRK];
int trknt_overlap1[MAXTRK];
int trknt_overlap2[MAXTRK];
int trknt_trksel[MAXTRK];
// truth information
int trknt_pdgcode[MAXTRK];
int trknt_ppdgcode[MAXTRK];
int trknt_tnpar[MAXTRK];
float trknt_truept[MAXTRK];
float trknt_pvtx[MAXTRK];
float trknt_pvty[MAXTRK];
float trknt_pvtz[MAXTRK];
int trknt_truensi[MAXTRK];

bool CalibStream::bookCalibNtuple() {
  // book calibration ntuple
  const std::string ntid=m_ntupdir+"/TrigTrack";
  m_trknt=new TTree("TrigTrack","Tracks in RoI");

  m_trknt->Branch("Run",&trknt_run,"Run/I");
  m_trknt->Branch("Event",&trknt_event,"Event/I");

  m_trknt->Branch("TrkROI",&trknt_roi,"TrkROI/I");
  m_trknt->Branch("TrkTrigMask",&trknt_trigmask,"TrkTrigMask/i");
  m_trknt->Branch("TrkL1Mask1",&trknt_l1mask1,"TrkL1Mask1/i");
  m_trknt->Branch("TrkL1Mask2",&trknt_l1mask2,"TrkL1Mask2/i");
  m_trknt->Branch("NSiROB",&trknt_nsirob,"NSiROB/I");
  m_trknt->Branch("NTRTROB",&trknt_ntrtrob,"NTRTROB/I");
  m_trknt->Branch("DelEta",&trknt_deleta,"DelEta/F");
  m_trknt->Branch("DelPhi",&trknt_delphi,"DelPhi/F");
  m_trknt->Branch("NTrk",&trknt_ntrk,"NTrk/I");
  m_trknt->Branch("TrkA0",trknt_a0,"TrkA0[NTrk]/F");
  m_trknt->Branch("TrkPhi0",trknt_phi0,"TrkPhi0[NTrk]/F");
  m_trknt->Branch("TrkZ0",trknt_z0,"TrkZ0[NTrk]/F");
  m_trknt->Branch("TrkEta",trknt_eta,"TrkEta[NTrk]/F");
  m_trknt->Branch("TrkPt",trknt_pt,"TrkPt[NTrk]/F");
  m_trknt->Branch("TrkChi2",trknt_chi2,"TrkChi2[NTrk]/F");
  m_trknt->Branch("TrkAuthor",trknt_author,"TrkAuthor[NTrk]/I");
  m_trknt->Branch("TrkNSiHit",trknt_nsihit,"TrkNSiHit[NTrk]/I");
  m_trknt->Branch("TrkNTRTHit",trknt_ntrthit,"TrkNTRTHit[NTrk]/I");
  m_trknt->Branch("TrkNOverlap",trknt_noverlap,"TrkNOverlap[NTrk]/I");
  m_trknt->Branch("TrkOverlap1",trknt_overlap1,"TrkOverlap1[NTrk]/I");
  m_trknt->Branch("TrkOverlap2",trknt_overlap2,"TrkOverlap2[NTrk]/I");
  m_trknt->Branch("TrkSel",trknt_trksel,"TrkSelected[NTrk]/I");

  m_trknt->Branch("TrkPDGCode",trknt_pdgcode,"TrkPDGCode[NTrk]/I");
  m_trknt->Branch("TrkPPDGCode",trknt_ppdgcode,"TrkPPDGCode[NTrk]/I");
  m_trknt->Branch("TrkNParent",trknt_tnpar,"TrkNParent[NTrk]/I");
  m_trknt->Branch("TrkTruePt",trknt_truept,"TrkTruePt[NTrk]/F");
  m_trknt->Branch("TrkTPVtx",trknt_pvtx,"TrkTPVtx[NTrk]/F");
  m_trknt->Branch("TrkTPVty",trknt_pvty,"TrkTPVty[NTrk]/F");
  m_trknt->Branch("TrkTPVtz",trknt_pvtz,"TrkTPVtz[NTrk]/F");
  m_trknt->Branch("TrkTrueNSI",trknt_truensi,"TrkTrueNSI[NTrk]/I");

  // register ntuple with THistSvc
  if (StatusCode::SUCCESS==p_thistsvc->regTree(ntid,m_trknt)) {
    return true;
  } else {
    return false;
  }
}

void CalibStream::fillCalibNtuple(const TrigInDetTrackCollection* tracks, 
				  const int roi,const int trigmask) {
  // fill the calibration ntuple for an RoI

  if (msgLvl(MSG::DEBUG)) msg() << "In fillCalibNtuple for " << tracks->size()
      << " tracks, ROI=" << roi << " mask=" << trigmask << endreq; 

  trknt_run=m_crun;
  trknt_event=m_cevent;

  // get truth map
  const TrigInDetTrackTruthMap* tracktruthmap;
  if (StatusCode::FAILURE==sgSvc()->retrieve(tracktruthmap,
				    "TrigInDetTrackTruthMap")) {
    msg(MSG::WARNING) << "Cannot find track-truth map" << endreq;
    tracktruthmap=0;
  }
  trknt_roi=roi;
  trknt_trigmask=trigmask;

  // get LVL1 information
  trknt_l1mask1=0;
  trknt_l1mask2=0;
  std::vector<std::string> lv1items=m_trigDec->getListOfTriggers("L1_.*");
  for (std::vector<std::string>::const_iterator itr=lv1items.begin();
       itr!=lv1items.end();++itr) {
    if (m_trigDec->isPassed(*itr)) {
      // L1 passed - see if it is one bit we are monitoring
      const std::string l1name=*itr;
      for (unsigned int ibit=0;ibit<m_level1list.size();++ibit) {
        if (l1name==m_level1list[ibit]) {
          if (ibit<32) {
            trknt_l1mask1=trknt_l1mask1 | (1 << ibit);
          } else {
            trknt_l1mask2=trknt_l1mask2 | (1 << (ibit-32));
          }
          ++m_seenl1[ibit];
          break;
        }
      }
    }
  }
  if (msgLvl(MSG::DEBUG)) msg() << "L1 masks are " << trknt_l1mask1 << " " << trknt_l1mask2 << endreq;

  trknt_ntrk=0;
  for (TrigInDetTrackCollection::const_iterator trkitr=tracks->begin();
       trkitr!=tracks->end();++trkitr) {
    trknt_author[trknt_ntrk]=(*trkitr)->algorithmId();
    const TrigInDetTrackFitPar* pars=(*trkitr)->param();
    if (pars!=0) {
      trknt_a0[trknt_ntrk]=pars->a0();
      trknt_phi0[trknt_ntrk]=pars->phi0();
      trknt_z0[trknt_ntrk]=pars->z0();
      trknt_eta[trknt_ntrk]=pars->eta();
      trknt_pt[trknt_ntrk]=pars->pT();
    } else {
      trknt_a0[trknt_ntrk]=-999.;
      trknt_phi0[trknt_ntrk]=-999.;
      trknt_z0[trknt_ntrk]=-999.;
      trknt_eta[trknt_ntrk]=-999.;
      trknt_pt[trknt_ntrk]=-999.;
    }
    trknt_chi2[trknt_ntrk]=(*trkitr)->chi2();
    // now analyse silicon hits and count overlaps
    std::vector<const TrigSiSpacePoint*>* sivec=(*trkitr)->siSpacePoints();
    if (sivec) {
      trknt_nsihit[trknt_ntrk]=sivec->size();
      // count the overlaps using layer identifiers
      std::vector<int> overlaps;
      std::vector<int> layers;
      for (std::vector<const TrigSiSpacePoint*>::const_iterator hitr=
	sivec->begin();hitr!=sivec->end();++hitr) {
        if (*hitr!=0) {
          int layer=(*hitr)->layer();
          if (std::find(layers.begin(),layers.end(),layer)==layers.end()) {
	    // layer code not seen before, add to list
	    layers.push_back(layer);
          } else {
	    // layer has been seen before - this is an overlap
	    overlaps.push_back(layer);
  	  }
        } else {
          if (msgLvl(MSG::DEBUG)) msg() << "SiHit vector pointer is zero " << endreq;
        }
        // count overlaps and store the first two
        trknt_noverlap[trknt_ntrk]=overlaps.size();
        if (overlaps.size()>0) {
          trknt_overlap1[trknt_ntrk]=overlaps[0];
        } else {
          trknt_overlap1[trknt_ntrk]=-1;
        }
        if (overlaps.size()>1) {
          trknt_overlap2[trknt_ntrk]=overlaps[1];
        } else {
         trknt_overlap2[trknt_ntrk]=-1;
	}
      }
    } else {
      trknt_nsihit[trknt_ntrk]=0;
    }
    
    std::vector<const InDet::TRT_DriftCircle*>* 
       trtvec=(*trkitr)->trtDriftCircles();
    if (trtvec!=0) {
      trknt_ntrthit[trknt_ntrk]=trtvec->size();
    } else {
      trknt_ntrthit[trknt_ntrk]=0;
    }
    // look at the truth information - first set to zero
    trknt_pdgcode[trknt_ntrk]=0;
    trknt_ppdgcode[trknt_ntrk]=0;
    trknt_tnpar[trknt_ntrk]=-1;
    trknt_truept[trknt_ntrk]=0.;
    trknt_pvtx[trknt_ntrk]=-999.;
    trknt_pvty[trknt_ntrk]=-999.;
    trknt_pvtz[trknt_ntrk]=-999.;
    trknt_truensi[trknt_ntrk]=-1;
    if (tracktruthmap!=0 && tracktruthmap->hasTruth(*trkitr)) {
      trknt_truensi[trknt_ntrk]=tracktruthmap->bestMatchSiHits(*trkitr);
      const HepMcParticleLink* tlink=tracktruthmap->bestMatchSi(*trkitr);
      if (msgLvl(MSG::DEBUG)) msg() << "Truth: Best match to " << tlink << " with " <<
         trknt_truensi[trknt_ntrk] << " hits" << endreq;
      const HepMC::GenParticle* particle=*tlink;
      if (particle!=0) setTruth(particle,trknt_ntrk);
    }
    ++trknt_ntrk;
    if (trknt_ntrk>=MAXTRK) {
      msg(MSG::WARNING) << "Exceeded maximum number of tracks" << endreq;
      trknt_ntrk=MAXTRK-1;
    }
  }
  trknt_nsirob=0;
  // check if RoI passes cuts, if so, write list of RoBs
  if (isSelected()) getROBListTrigIndet(tracks);
  if (m_makeNtuple) m_trknt->Fill();
}

void CalibStream::setTruth(const HepMC::GenParticle* particle, 
			   const int itrk) {
  // set the truth information in the ntuple from the given particle

  trknt_pdgcode[itrk]=particle->pdg_id();
  trknt_truept[itrk]=particle->momentum().perp();
  const HepMC::GenVertex* vert=particle->production_vertex();
  if (vert!=0) {
    trknt_pvtx[itrk]=vert->position().x();
    trknt_pvty[itrk]=vert->position().y();
    trknt_pvtz[itrk]=vert->position().z();
    // store number of parent particles, and PDG code of first one
    trknt_tnpar[itrk]=vert->particles_in_size();
    const HepMC::GenParticle* parent=*(vert->particles_in_const_begin());
    if (parent!=0) {
      trknt_ppdgcode[itrk]=parent->pdg_id();
    }
  }
}

bool CalibStream::isSelected() {
  // decide if RoI should be written out, based on trknt_ information
  // dummy for now - select all tracks and return true
  for (int itrk=0;itrk<trknt_ntrk;++itrk) trknt_trksel[itrk]=1;
  return true;
}

void CalibStream::getROBListTrigIndet(const TrigInDetTrackCollection* 
					    tracks) {
  // fill in the RoBlist for the collection of selected tracks
  // loop over the tracks, get their RoBs if selected
  unsigned int itrk=0;
  float etamin=10.;
  float etamax=-10.;
  float phimin=4.;
  float phimax=-4.;
  for (TrigInDetTrackCollection::const_iterator trkitr=tracks->begin();
       trkitr!=tracks->end();++trkitr,++itrk) {
    if (trknt_trksel[itrk]>0) {
      std::vector<const TrigSiSpacePoint*>* sivec=(*trkitr)->siSpacePoints();
      if (sivec!=0) {
        std::vector<Identifier> clusterids;
        for (std::vector<const TrigSiSpacePoint*>::const_iterator hitr=
	    sivec->begin();hitr!=sivec->end();++hitr) {
          if (*hitr!=0) {
	    // get the two clusters
	    int nclus=0;
	    bool edge=false;
	    std::pair<const InDet::SiCluster*, const InDet::SiCluster*> 
	      cluster=(*hitr)->clusters();
	    if (cluster.first!=0) {
	      nclus++;
	      clusterids.push_back(waferId(cluster.first,edge));
	    }
	    if (cluster.second!=0) {
	      nclus++;
	      clusterids.push_back(waferId(cluster.second,edge));
 	    }
	    // nothing is done with 'edges' at the moment
	  } else {
            if (msgLvl(MSG::DEBUG)) msg() << "Silicon Hit pointer is zero " << endreq;
	  }
	}
	// loop over the clusterIDs and make a list of RoBs
        for (std::vector<Identifier>::const_iterator itr=clusterids.begin();
	     itr!=clusterids.end();++itr) {
	  uint32_t robid=0;
          // identify the hit as pixel or SCT
          if (m_idHelper->is_pixel(*itr)) {
	    robid=m_pixIdMapping->getRobID(*itr);
	  } else if (m_idHelper->is_sct(*itr)) {
	    robid=m_sctCablingSvc->getRobIdFromOfflineId(*itr);
	  } else {
            msg(MSG::ERROR)  << "Cluster of undefined type (not Pixel/SCT)" << endreq;
	  }
	  if (robid!=0 && find(m_robIDs.begin(),m_robIDs.end(),robid)==
	      m_robIDs.end()) {
	    m_robIDs.push_back(robid);
	    ++trknt_nsirob;
	  }
	}
      } else {
       if (msgLvl(MSG::DEBUG)) msg() << "SiHit vector pointer is zero " << endreq;
      }
      if (msgLvl(MSG::DEBUG)) msg() << "Track " << itrk << " selected for RoBlist, new size " << m_robIDs.size() << endreq; 
      // accumulate eta and phi for getting TRT RoBs
      if (trknt_phi0[itrk]<phimin) phimin=trknt_phi0[itrk];
      if (trknt_phi0[itrk]>phimax) phimax=trknt_phi0[itrk];
      if (trknt_eta[itrk]<etamin) etamin=trknt_eta[itrk];
      if (trknt_eta[itrk]>etamax) etamax=trknt_eta[itrk];
    }
  }
  // now add TRT RoBs to list
  // add tolerance to eta-phi region
  const float PIVAL=3.14159265;
  trknt_deleta=etamax-etamin;
  trknt_delphi=phimax-phimin;
  if (trknt_delphi<0) trknt_delphi+=2*PIVAL;
  if (trknt_delphi>2*PIVAL) trknt_delphi-=2*PIVAL;
  phimin-=m_trtdelphi;
  if (phimin<-PIVAL) phimin+=2*PIVAL;
  phimax+=m_trtdelphi;
  if (phimax>PIVAL) phimax-=2*PIVAL;
  etamin-=m_trtdeleta;
  etamax+=m_trtdeleta;
  std::vector<uint32_t> trtROBs;

  double _eta = 0.5*(etamin+etamax);
  double _phi = HLT::phiMean(phimin,phimax);
  
  TrigRoiDescriptor _roi( _eta, etamin, etamax, _phi, phimin, phimax );

  m_regselsvc->DetROBIDListUint(TRT, _roi, trtROBs);
  trknt_ntrtrob=trtROBs.size();
  if (m_useTRT) {
    // add the TRT ROBs to the main list if required
    for (std::vector<uint32_t>::const_iterator irob=trtROBs.begin();
	 irob!=trtROBs.end();++irob) m_robIDs.push_back(*irob);
  }
}

const Identifier CalibStream::waferId(const InDet::SiCluster* cluster,
				      bool& edge) {
  // extract the identifier of the cluster
  // additionally, set edge to true if the cluster is tocuhing the edge 
  // of the wafer (for SCT only)
  const Identifier& rdoid=cluster->identify();
  if (m_idHelper->is_pixel(rdoid)) {
    if (pixelEdge(rdoid)) edge=true;
    for (std::vector<Identifier>::const_iterator itr=cluster->rdoList().begin();itr!=cluster->rdoList().end();++itr) {
      if (pixelEdge(*itr)) edge=true;
    }
    return m_pixid->wafer_id(rdoid);
  } else if (m_idHelper->is_sct(rdoid)) {
    int strip=m_sctid->strip(rdoid);
    if (strip==0 || strip==767) edge=true;
    // also loop over secondary ones in the list
    for (std::vector<Identifier>::const_iterator itr=cluster->rdoList().begin();itr!=cluster->rdoList().end();++itr) {
      int strip2=m_sctid->strip(*itr);
      if (strip2==0 || strip2==767) edge=true;
    }
    return m_sctid->wafer_id(rdoid);
  } else {
    return Identifier();
  }
}

bool CalibStream::pixelEdge(const Identifier& ident) {
  // check if a pixel hit is on the edge of a module
  bool edge=false;
  int phip=m_pixid->phi_index(ident);
  int etap=m_pixid->eta_index(ident);
   if (phip==0 || etap==0) edge=true;
  // different maximum edges for barrel and endcap modules
  if (m_pixid->is_barrel(ident)) {
    if (phip==327 || etap==205) edge=true;
  } else {
    if (phip==205 || etap==164) edge=true;
  }
  return edge;
}

} //end of InDet namespace
