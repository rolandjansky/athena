/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigLArNoiseBurstRecoAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "hltinterface/IInfoRegister.h"
#include "hltinterface/ContainerFactory.h"

#include <boost/property_tree/ptree.hpp>

using namespace TrigCompositeUtils;

TrigLArNoiseBurstRecoAlg::TrigLArNoiseBurstRecoAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) :
  AthReentrantAlgorithm( name, pSvcLocator ), m_IsObject(nullptr),  m_timeTagPosToClear(240), m_publishTime(180), m_name_of_is("LArISInfo_NoiseBurstAlg"), m_timeTagPosRec(0) {
}


StatusCode TrigLArNoiseBurstRecoAlg::initialize() {

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_knownBadFEBsVecKey.initialize() );
  ATH_CHECK( m_knownMNBFEBsVecKey.initialize() );
  ATH_CHECK( m_noiseBurstDetected.initialize() );
  ATH_CHECK(m_noisyROTool.retrieve());


  if ( not m_monTool.name().empty() )
    CHECK( m_monTool.retrieve() );

  // prepare mask
  m_mask = 0x0;
  if ( m_badFEBFlaggedPartitions ) m_mask|=0x1;
  if ( m_satTightFlaggedPartitions ) m_mask|=0x2;
  if ( m_mNBLooseFlaggedPartitions ) m_mask|=0x10;
  if ( m_mNBTightFlaggedPartitions ) m_mask|=0x20;
  if ( m_mNBTight_PsVetoFlaggedPartitions ) m_mask|=0x40;

  // No TDAQ structure, no publishing anyway
  try {
   auto cfact = hltinterface::ContainerFactory::getInstance();
    if ( cfact ) {
      // construct the LAr noise burst container and register it
      ATH_MSG_DEBUG ( "Got the factory for TDAQ interface, will try to register vectors" );
      m_IsObject = cfact->constructContainer(m_name_of_is,"LArNoiseBurstCandidates");
      m_evntPos = cfact->addIntVector(m_IsObject,"Flag",hltinterface::GenericHLTContainer::LASTVALUE);
      m_timeTagPos = cfact->addIntVector(m_IsObject,"TimeStamp",hltinterface::GenericHLTContainer::LASTVALUE);
      m_timeTagPosns = cfact->addIntVector(m_IsObject,"TimeStamp_ns",hltinterface::GenericHLTContainer::LASTVALUE);
      ATH_MSG_DEBUG ( "Registering container in IS with name /HLTObjects/" << m_name_of_is );
      hltinterface::IInfoRegister::instance()->registerObject("/HLTObjects/",m_IsObject);
    } // if cfact
   } 
    catch (std::exception& ex ) {
      ATH_MSG_WARNING ( "Cannot use ISInfo publication. got exception " << ex.what() );
      m_IsObject = nullptr;
      m_timeTagPosRec=0;
  }
  // some initial value to clean up stuff
  
  ATH_MSG_INFO("TrigLArNoiseBurstRecoAlg initialization completed successfully.");
  return StatusCode::SUCCESS;
}

StatusCode TrigLArNoiseBurstRecoAlg::execute( const EventContext& context ) const {  

  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  bool monitor = !m_monTool.name().empty();
  auto timer = Monitored::Timer("TIME_larnoisetool");
  auto mon = Monitored::Group(m_monTool,timer);
  std::string bitWise_flags("bitWise_flags");

  // get cells
  SG::ReadHandle<CaloCellContainer> cellsHandle(m_cellContainerKey, context);
  ATH_CHECK( cellsHandle.isValid() );
  ATH_MSG_DEBUG ( "Cells handle size: " << cellsHandle->size() << "..." );

  // necessary conditions
  std::set<unsigned int> bf;
  std::vector<HWIdentifier> MNBfeb;
  SG::ReadCondHandle<LArBadFebCont> badHdl(m_knownBadFEBsVecKey, context);
  const LArBadFebCont* badCont=*badHdl;
  if(badCont) {
    for(LArBadFebCont::BadChanVec::const_iterator i = badCont->begin(); i!=badCont->end(); i++) {
      bf.insert(i->first);
    }
  }
    
  SG::ReadCondHandle<LArBadFebCont> MNBHdl(m_knownMNBFEBsVecKey, context);
  const LArBadFebCont* MNBCont=*MNBHdl;
  if(MNBCont) {
    for(LArBadFebCont::BadChanVec::const_iterator i = MNBCont->begin(); i!=MNBCont->end(); i++) {
      MNBfeb.push_back(HWIdentifier(i->first));
    } 
  }

  // no cells, no discussion
  // should record, before amything, the answer
  if ( !(cellsHandle.cptr()) ) return StatusCode::FAILURE;
  unsigned int flag = 0;
  bool pass=false;
  ATH_MSG_DEBUG ("Got cell container, will process it");
  std::unique_ptr<LArNoisyROSummary> noisyRO = m_noisyROTool->process(cellsHandle.cptr(), &bf, &MNBfeb);
  ATH_MSG_DEBUG("processed it");
  if ( monitor ) { // input
    auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"Input");
    fill(m_monTool,bitWise);
  }
  if ( noisyRO->BadFEBFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : BadFEBFlaggedPartitions");
        flag |= 0x1;
        if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"BadFEBFlaggedPartitions");
          fill(m_monTool,bitWise);
        }
  }
  if ( noisyRO->BadFEB_WFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : BadFEB_WFlaggedPartitions");
        flag |= 0x8;
        if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"BadFEB_WFlaggedPartitions");
          fill(m_monTool,bitWise);
        }
  }
  if ( noisyRO->SatTightFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : SatTightFlaggedPartitions");
        flag |= 0x2;
        if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"SatTightFlaggedPartitions");
          fill(m_monTool,bitWise);
        }
  }
  if ( noisyRO->MNBLooseFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : MNBLooseFlaggedPartions");
        flag |= 0x10;
        if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"MNBLooseFlaggedPartions");
          fill(m_monTool,bitWise);
        }
  }
  if ( noisyRO->MNBTightFlaggedPartitions() ) {
        ATH_MSG_DEBUG("Passed : MNBTightFlaggedPartions");
        flag |= 0x20;
        if ( monitor ) {
          auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"MNBTightFlaggedPartions");
          fill(m_monTool,bitWise);
        }
  }

  if ( (flag & m_mask) != 0x0 ) {
     ATH_MSG_DEBUG("LAr Noise detected : ");
     pass = true;
     if ( monitor ) {
        auto bitWise = Monitored::Scalar<std::string>(bitWise_flags,"Output");
        fill(m_monTool,bitWise);
     }
  }
  else {
     ATH_MSG_DEBUG("LAr Noise not detected!");
  }
  
  // publish the found values in IS
  if ( pass && (m_IsObject != nullptr) ) {
    // lock the IS publishing
    std::lock_guard<std::mutex> lock( m_pubIS_mtx );
    const EventIDBase& EIHandle = context.eventID();
    long int thisTimeStamp    = (EIHandle).time_stamp();
    long int thisTimeStampns  = (EIHandle).time_stamp_ns_offset();
    uint32_t thisLB           = (EIHandle).lumi_block();
    unsigned long long thisEv = (EIHandle).event_number();
    boost::property_tree::ptree event_tree;
    event_tree.put("eventNumber",thisEv);
    event_tree.put("LBNumber",thisLB);
    try{
      hltinterface::IInfoRegister::instance()->beginEvent(event_tree);

      if ( ( thisTimeStamp - m_timeTagPosRec) > m_timeTagPosToClear ) {
    
        std::vector<long>& ee = m_IsObject->getIntVecField(m_evntPos);
        std::vector<long>& tt = m_IsObject->getIntVecField(m_timeTagPos);
        std::vector<long>& ttn = m_IsObject->getIntVecField(m_timeTagPosns);

        int toBeCleared = 0;
        for(unsigned int dd=0;dd<tt.size();dd++){
	  if ( ( thisTimeStamp - tt[dd] )  < m_publishTime ) {
	    toBeCleared = (int)dd-1;
	    break;
	  }
        }

        if ( toBeCleared > 0 ) {
	  ee.erase(ee.begin(),ee.begin()+(toBeCleared) );
	  tt.erase(tt.begin(),tt.begin()+(toBeCleared) );
	  ttn.erase(ttn.begin(),ttn.begin()+(toBeCleared) );
        }
        m_timeTagPosRec = thisTimeStamp ; // records to remember when things were cleared
      } // thisTimeStamp if check

      m_IsObject->appendField(m_evntPos,std::vector<long>{flag});
      m_IsObject->appendField(m_timeTagPos,std::vector<long>{(long int)thisTimeStamp});
      m_IsObject->appendField(m_timeTagPosns,std::vector<long>{(long int)thisTimeStampns});
      hltinterface::IInfoRegister::instance()->endEvent(event_tree);
    }catch(const std::exception &ex){
      ATH_MSG_INFO("Caught exception while calling hltinterface::IInfoRegister::instance()->endEvent for event " << thisEv
		   <<" at LB " << thisLB
		   <<". Exception was "<<ex.what());
    }
  } // if m_IsObject
  SG::WriteHandle<bool> outputHandle (m_noiseBurstDetected, context );
  auto output = std::make_unique<bool>(pass);
  ATH_CHECK( outputHandle.record(std::move(output) ) );
 
  return StatusCode::SUCCESS;
}
