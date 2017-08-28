/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page StoreGateTests_page StoreGateTests Package
@author Sebastien Binet

@section StoreGateTests_StoreGateTestsIntro Introduction

This package is part of the ATN tests suite asserting the performances of the @c StoreGateSvc service.
It bundles a couple of algorithms to benchmark @c StoreGateSvc::record() and @c StoreGateSvc::retrieve() method calls to make sure that nothing goes wrong during the maintenance of this key class of the Athena component model.
It can also be used (together with the @c PerfMonSvc) to perform timing measurements of new concrete implementations of the @c StoreGateSvc and analyze the impact of new data structures on @c StoreGateSvc performances.

@section StoreGateTests_StoreGateTestsOverview Class Overview
  The StoreGateTests package contains the following algorithms:

  - SgStressProducer : creates a bunch of @c SgTests::PayLoad objects and puts them into the event store (@c StoreGateSvc). The number and the size of these dummy objects can be modified at the jobOptions level allowing the test to be rather versatile and tweakable.

  - SgStressConsumer : retrieves the previously created @c SgTests::PayLoad objects and reads them back.

A typical test job consists of these 2 algorithms chained together in the topSequence. Using the @c PerfMonSvc and its performance toolbox, one can deduce basic performances of the @c StoreGateSvc class and make sure they satisfy the Atlas requirements.

@section StoreGateTests_StoreGateTestsJobOptions JobOptions
  The tests can be run via the @c share/test_sgProducerConsumer_jobOptions.py file which will run in sequence the @c SgStressProducer and the @c SgStressConsumer algorithms. Performance data will be gathered during the job by the @c PerfMonSvc and saved into a tuple.
This tuple can be analyzed later on by the @c perfmon.py script provided by the performance toolbox (see @ref PerfMonSvc)



*/
