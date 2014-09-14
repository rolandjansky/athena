/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <iostream>

#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"
#include "TrigInterfaces/AlgoConfig.h"

#include "TestTools/initGaudi.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/TECreateAlgo.h"

int main()
{

  // get Gaudi MsgService:
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi(pSvcLoc)) {
    std::cerr << "problem loading Athena_test::initGaudi !!" << std::endl;
    return 0;
  }
  assert(pSvcLoc);

  MsgStream* log = new MsgStream(Athena::getMessageSvc(), "getMessageSvc_test");
  int logLvl = log->level();

  // Create & set the config object, holding all common variables
  HLT::AlgoConfig* config = new HLT::AlgoConfig();
  config->setMsgStream(log);
  config->setMsgLvl(logLvl);



  // add empty signature
  HLT::Signature* sign1 = new HLT::Signature(config);

  std::vector<HLT::Algo*> algos;
  HLT::TECreateAlgo* firstAlgo=0;
  HLT::Sequence* seq1 = new HLT::Sequence(std::vector<unsigned int>(1), 2, firstAlgo, algos, config);

  sign1->addItem(2, seq1);
  sign1->addItem(4, seq1);

  std::cout << "Signature1: "<< (*sign1) << std::endl;

  // clean up
  delete sign1;
  delete seq1;
  delete log;
  delete config;



  return 0;
}
