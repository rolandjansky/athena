/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <EventLoop/Algorithm.h>
#include <EventLoop/BackgroundDriver.h>
#include <EventLoop/BackgroundJob.h>
#include <EventLoop/BackgroundTSelector.h>
#include <EventLoop/BackgroundWorker.h>
#include <EventLoop/BatchDriver.h>
#include <EventLoop/BatchJob.h>
#include <EventLoop/BatchSegment.h>
#include <EventLoop/BatchSample.h>
#include <EventLoop/BatchWorker.h>
#include <EventLoop/CondorDriver.h>
#include <EventLoop/DirectDriver.h>
#include <EventLoop/D3PDReaderSvc.h>
#include <EventLoop/GEDriver.h>
#include <EventLoop/Job.h>
#include <EventLoop/LLDriver.h>
#include <EventLoop/LSFDriver.h>
#include <EventLoop/LocalDriver.h>
#include <EventLoop/OutputStream.h>
#include <EventLoop/MetricsSvc.h>
#include <EventLoop/ProofArgs.h>
#include <EventLoop/ProofDriver.h>
#include <EventLoop/ProofTSelector.h>
#include <EventLoop/SoGEDriver.h>
#include <EventLoop/SlurmDriver.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/TEventSvc.h>
#include <EventLoop/TorqueDriver.h>
#include <EventLoop/UnitTestAlg.h>
#include <EventLoop/UnitTestAlg1.h>
#include <EventLoop/VomsProxySvc.h>
#include <EventLoop/Worker.h>
#include <EventLoop/ProofWorker.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class EL::Algorithm+;
#pragma link C++ class EL::BackgroundDriver+;
#pragma link C++ class EL::BackgroundJob+;
#pragma link C++ class EL::BackgroundTSelector+;
#pragma link C++ class EL::BackgroundWorker+;
#pragma link C++ class EL::BatchDriver+;
#pragma link C++ class EL::BatchJob+;
#pragma link C++ class EL::BatchSample+;
#pragma link C++ class EL::BatchSegment+;
#pragma link C++ class EL::BatchWorker+;
#pragma link C++ class EL::CondorDriver+;
#pragma link C++ class EL::DirectDriver+;
#pragma link C++ class EL::Driver+;
#pragma link C++ class EL::Job+;
#pragma link C++ class EL::GEDriver+;
#pragma link C++ class EL::LSFDriver+;
#pragma link C++ class EL::LLDriver+;
#pragma link C++ class EL::LocalDriver+;
#pragma link C++ class EL::OutputStream+;
#pragma link C++ class EL::MetricsSvc+;
#pragma link C++ class EL::ProofArgs+;
#pragma link C++ class EL::ProofDriver+;
#pragma link C++ class EL::ProofTSelector+;
#pragma link C++ class EL::ProofWorker+;
#pragma link C++ class EL::SlurmDriver+;
#pragma link C++ class EL::SoGEDriver+;
#pragma link C++ class EL::StatusCode+;
#pragma link C++ class EL::TorqueDriver+;
#pragma link C++ class EL::UnitTestAlg+;
#pragma link C++ class EL::UnitTestAlg1+;
#pragma link C++ class EL::VomsProxySvc+;
#pragma link C++ class EL::Worker+;

#pragma link C++ class std::pair<Long64_t,Long64_t>+;
#pragma link C++ class std::vector<std::pair<Long64_t,Long64_t> >+;

#ifdef ROOTCORE_PACKAGE_D3PDReader
#pragma link C++ class EL::D3PDReaderSvc+;
#endif

#ifdef ROOTCORE_PACKAGE_xAODRootAccess
#pragma link C++ class EL::TEventSvc+;
#endif

#endif
