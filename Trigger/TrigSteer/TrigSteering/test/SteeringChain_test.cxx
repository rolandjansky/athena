/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <iostream>

#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/PeriodicScaler.h"
#include "TrigSteering/ISequenceProvider.h"
#include "TrigSteering/IScalerSvc.h"
#include "TrigInterfaces/AlgoConfig.h"

#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"


using namespace HLT;
class TestSequenceProvider : public ISequenceProvider {
public:
  HLT::Sequence* findSeqForOutputTeType( const unsigned int ) override {
    return 0;
  }
};

class TestScalerSvc : public IScalerSvc, public AthService {
public:
  TestScalerSvc( ISvcLocator* pSvc ) : AthService( "TestScalerSvc", pSvc ) {}
  IScaler* get( const std::string& ) const {
    return new PeriodicScaler;
  } 
private:
};

int main()
{

  // get Gaudi MsgService:
  ISvcLocator* pSvcLoc;
  if ( !Athena_test::initGaudi( pSvcLoc ) ) {
    std::cerr << "problem loading Athena_test::initGaudi !!" << std::endl;
    return 0;
  }
  assert( pSvcLoc );

  MsgStream* log = new MsgStream( Athena::getMessageSvc(), "getMessageSvc_test" );
  int logLvl = log->level();

  // Create & set the config object, holding all common variables
  HLT::AlgoConfig* config = new HLT::AlgoConfig();
  config->setMsgStream( log );
  config->setMsgLvl( logLvl );
  const std::vector<TrigConf::HLTSignature*> noSteps;
  TrigConf::HLTChain chainConf( "HLT_multiSeeded", 1, 0, "HLT", "L1_A L1_B", 0, noSteps );
  chainConf.set_prescale( 10 );
  chainConf.addStream( new TrigConf::HLTStreamTag("Main", "physics", true, 1.) );
  chainConf.addStream( new TrigConf::HLTStreamTag("express", "express", true, 1.) );
  chainConf.set_groupList( std::set<std::string>( {"grA", "grB"} ) );


  TestSequenceProvider seqProvider;
  TestScalerSvc scalerSvc( pSvcLoc );
  HLT::SteeringChain steeringChain( &chainConf, &seqProvider, config, &scalerSvc );
  std::cout << "After creation" << std::endl;
  steeringChain.print(std::cout);
  std::cout << std::endl;

  std::cout << "reset:" << std::endl;
  for ( int i = 0; i < 20; ++i ) {
    steeringChain.reset();
    steeringChain.setActive();

    steeringChain.setPrescaleState();
    steeringChain.print(std::cout);
    std::cout << std::endl;  
  }

  std::cout << "reseetChain:" << std::endl;
  for ( int i = 0; i < 20; ++i ) {
    steeringChain.resetChain();
    steeringChain.setActive();

    steeringChain.setPrescaleState();
    steeringChain.print(std::cout);
    std::cout << std::endl;  
  }

  

  
  
  delete log;
  delete config;



  return 0;
}
