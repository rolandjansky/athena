/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <SampleHandler/DiskListEOS.h>
#include <SampleHandler/DiskListLocal.h>
#include <SampleHandler/DiskListSRM.h>
#include <SampleHandler/DiskListXRD.h>
#include <SampleHandler/DiskOutputLocal.h>
#include <SampleHandler/DiskOutputXRD.h>
#include <SampleHandler/DiskWriter.h>
#include <SampleHandler/MetaData.h>
#include <SampleHandler/MetaFields.h>
#include <SampleHandler/MetaNames.h>
#include <SampleHandler/MetaObject.h>
#include <SampleHandler/MetaVector.h>
#include <SampleHandler/Sample.h>
#include <SampleHandler/SampleComposite.h>
#include <SampleHandler/SampleGrid.h>
#include <SampleHandler/SampleHandler.h>
#include <SampleHandler/SampleHist.h>
#include <SampleHandler/SampleLocal.h>
#include <SampleHandler/SampleMeta.h>
#include <SampleHandler/SamplePtr.h>
#include <SampleHandler/ScanDir.h>
#include <SampleHandler/TagList.h>
#include <SampleHandler/ToolsDiscovery.h>
#include <SampleHandler/ToolsDuplicates.h>
#include <SampleHandler/ToolsJoin.h>
#include <SampleHandler/ToolsMeta.h>
#include <SampleHandler/ToolsSplit.h>
#include <SampleHandler/MetaDataQuery.h>
#include <SampleHandler/MetaDataSample.h>
#include <SampleHandler/fetch.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace SH;
#pragma link C++ class SH::DiskList+;
#pragma link C++ class SH::DiskListEOS+;
#pragma link C++ class SH::DiskListLocal+;
#pragma link C++ class SH::DiskListSRM+;
#pragma link C++ class SH::DiskListXRD+;
#pragma link C++ class SH::DiskOutput+;
#pragma link C++ class SH::DiskOutputLocal+;
#pragma link C++ class SH::DiskOutputXRD+;
#pragma link C++ class SH::DiskWriter+;
#pragma link C++ class SH::MetaFields+;
#pragma link C++ class SH::MetaNames+;
#pragma link C++ class SH::Sample+;
#pragma link C++ class SH::SampleComposite+;
#pragma link C++ class SH::SampleGrid+;
#pragma link C++ class SH::SampleHandler-;
#pragma link C++ class SH::SampleHist+;
#pragma link C++ class SH::SampleLocal+;
#pragma link C++ class SH::SampleMeta+;
#pragma link C++ class SH::ScanDir-;
#pragma link C++ class SH::TagList+;
#pragma link C++ class SH::Meta+;
#pragma link C++ class SH::MetaData<float>+;
#pragma link C++ class SH::MetaVector<float>+;
#pragma link C++ class SH::MetaData<double>+;
#pragma link C++ class SH::MetaData<std::string>+;
#pragma link C++ class SH::MetaData<bool>+;
#pragma link C++ class SH::MetaData<int>+;
#pragma link C++ class SH::MetaVector<Long64_t>+;
#pragma link C++ class SH::MetaObject-;
#pragma link C++ class std::vector<SH::Sample*>+;
#pragma link C++ class std::pair<std::string,SH::Sample*>+;
#pragma link C++ class std::map<std::string,SH::Sample*>+;

#pragma link C++ function SH::dbg (const SH::SampleHandler&, unsigned);
#pragma link C++ function SH::dbg (const SH::Sample&, unsigned);
#pragma link C++ function SH::dbg (const SH::TagList&, unsigned);
#pragma link C++ function SH::scanDir (SampleHandler&, DiskList&, const std::string&, const std::string&, const std::string&);
#pragma link C++ function SH::scanDir (SampleHandler&, const std::string&);
#pragma link C++ function SH::scanDir (SampleHandler&, const std::string&, const std::string&);
#pragma link C++ function SH::scanSingleDir (SampleHandler&, const std::string&, DiskList&, const std::string&);
#pragma link C++ function SH::scanFiles (SampleHandler&, DiskList&, const std::string&);
#pragma link C++ function SH::scanNEvents (SH::SampleHandler&);
#pragma link C++ function SH::scanNEvents (SH::Sample&);
#pragma link C++ function SH::makeFromTChain (const std::string&, const TChain&);
#pragma link C++ function SH::scanDQ2 (SampleHandler&, const std::string&);
#pragma link C++ function SH::scanRucio (SampleHandler&, const std::string&, bool);
#pragma link C++ function SH::readFileList (SampleHandler&, const std::string&, const std::string&);
#pragma link C++ function SH::addGrid (SampleHandler&, const std::string&);
#pragma link C++ function SH::addGridCombined (SampleHandler&, const std::string&, const std::vector<std:string>&);
#pragma link C++ function SH::addGridCombinedFromFile (SampleHandler&, const std::string&, const std:string&);
#pragma link C++ function SH::makeGridDirect (SampleHandler&, const std::string&, const std::string&, const std::string&, bool);
#pragma link C++ function SH::printDuplicateEvents (const SH::Sample&);
#pragma link C++ function SH::printDuplicateEventsSplit (const SH::SampleHandler&);
#pragma link C++ function SH::printDuplicateEventsJoint (const SH::SampleHandler&);
#pragma link C++ function SH::mergeSamples (SH::SampleHandler&, const std::string&, const std::string&);
#pragma link C++ function SH::readSusyMeta (const SH::SampleHandler&, const std::string&);
#pragma link C++ function SH::readSusyMetaDir (const SH::SampleHandler&, const std::string&);

#pragma link C++ class SH::MetaDataQuery+;
#pragma link C++ class SH::MetaDataSample+;
#pragma link C++ function SH::fetchMetaData (SH::MetaDataQuery&);
#pragma link C++ function SH::fetchMetaData (SH::SampleHandler&, bool);

#endif
