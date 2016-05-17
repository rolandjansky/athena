/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_MERGEPACKS
#define TRIGSTORAGEDEF_MERGEPACKS

#include "TrigStorageDefinitions/EDM_TypeInformation.h"
#include "TrigStorageDefinitions/TrigInDetEvent.h"
#include "TrigStorageDefinitions/TrigSteeringEvent.h"
#include "TrigStorageDefinitions/TrigMuonEvent.h"
#include "TrigStorageDefinitions/TrigMissingEtEvent.h"
#include "TrigStorageDefinitions/TrigParticle.h"
#include "TrigStorageDefinitions/TrigTopoEvent.h"
#include "TrigStorageDefinitions/TrigCaloEvent.h"
#include "TrigStorageDefinitions/TrigCombinedEvent.h"
#include "TrigStorageDefinitions/TrigMonitoringEvent.h"
#include "TrigStorageDefinitions/TrigBphysicsEvent.h"


#define TYPEMAPCLASS(name)\
struct class_##name{\
  typedef TypeInfo_##name map;\
  static const std::string package_name;\
};

TYPEMAPCLASS(TrigBphysicsEvent)
TYPEMAPCLASS(TrigMonitoringEvent)
TYPEMAPCLASS(TrigTopoEvent)
TYPEMAPCLASS(TrigCombinedEvent)
TYPEMAPCLASS(TrigCaloEvent)
TYPEMAPCLASS(TrigParticle)
TYPEMAPCLASS(TrigMissingEtEvent)
TYPEMAPCLASS(TrigMuonEvent) 
TYPEMAPCLASS(TrigInDetEvent)
TYPEMAPCLASS(TrigSteeringEvent)

   
struct TypeInfo_EDM {
typedef  HLT::TypeInformation::newlist
::add<class_TrigBphysicsEvent>    ::go
::add<class_TrigMonitoringEvent>  ::go
::add<class_TrigCombinedEvent>    ::go
::add<class_TrigCaloEvent>        ::go
::add<class_TrigTopoEvent>        ::go
::add<class_TrigParticle>         ::go
::add<class_TrigMissingEtEvent>   ::go
::add<class_TrigMuonEvent>        ::go
::add<class_TrigInDetEvent>       ::go
::add<class_TrigSteeringEvent>    ::go
::done map;
};

#endif
