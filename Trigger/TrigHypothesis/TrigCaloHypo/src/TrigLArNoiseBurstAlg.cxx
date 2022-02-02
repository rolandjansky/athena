/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigLArNoiseBurstAlg.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "hltinterface/IInfoRegister.h"
#include "hltinterface/ContainerFactory.h"

#include <boost/property_tree/ptree.hpp>

using namespace TrigCompositeUtils;

TrigLArNoiseBurstAlg::TrigLArNoiseBurstAlg( const std::string& name, 
                      ISvcLocator* pSvcLocator ) :
  ::HypoBase( name, pSvcLocator ), m_IsObject(nullptr),  m_timeTagPosToClear(240), m_publishTime(180), m_name_of_is("LArISInfo_NoiseBurstAlg"), m_timeTagPosRec(0) {}


StatusCode TrigLArNoiseBurstAlg::initialize() {

  ATH_CHECK( m_cellContainerKey.initialize() );
  ATH_CHECK( m_knownBadFEBsVecKey.initialize() );
  ATH_CHECK( m_knownMNBFEBsVecKey.initialize() );
  ATH_CHECK( m_hypoTools.retrieve() );

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
  
  return StatusCode::SUCCESS;
}

StatusCode TrigLArNoiseBurstAlg::execute( const EventContext& context ) const {  
  ATH_MSG_DEBUG ( "Executing " << name() << "..." );

  auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

  // new output decisions
  SG::WriteHandle<DecisionContainer> outputHandle = createAndStore(decisionOutput(), context ); 
  auto decisions = outputHandle.ptr();

  // input for decision
  std::vector<ITrigLArNoiseBurstHypoTool::CaloCellNoiseInfo> toolInput;

  ATH_CHECK( previousDecisionsHandle->size() == 1 );
  const auto previousDecision = previousDecisionsHandle->at(0);
  //get RoI  
  auto roiELInfo = findLink<TrigRoiDescriptorCollection>( previousDecision, initialRoIString() );
    
  ATH_CHECK( roiELInfo.isValid() );
  const TrigRoiDescriptor* roi = *(roiELInfo.link);

    // get cells
  SG::ReadHandle<CaloCellContainer> cellsHandle(m_cellContainerKey, context);
  ATH_CHECK( cellsHandle.isValid() );
  ATH_MSG_DEBUG ( "Cluster handle size: " << cellsHandle->size() << "..." );

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

  // Get new output decision, child of previousDecision
  auto d = newDecisionIn (decisions, previousDecision, hypoAlgNodeName(), context );
    
  unsigned int flag(0); // to get information outside
  // create summary struct
  toolInput.emplace_back( d, flag, roi, cellsHandle.cptr(), &bf, &MNBfeb, previousDecision );
  
  // link the cluster
  { 
    auto cell = ElementLink<CaloCellContainer>( *cellsHandle, 0 );
    ATH_CHECK( cell.isValid() );
    d->setObjectLink( featureString(),  cell );
  }
    
  ATH_MSG_DEBUG( "Found "<<toolInput.size()<<" inputs to tools");
  for ( auto& tool: m_hypoTools ) {
    ATH_CHECK( tool->decide( toolInput ) );
  }
  
  bool any_passed = not d->decisions().empty();

  // publish the found values in IS
  if ( any_passed && (m_IsObject != nullptr) ) {
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
 
  ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

  return StatusCode::SUCCESS;
}
