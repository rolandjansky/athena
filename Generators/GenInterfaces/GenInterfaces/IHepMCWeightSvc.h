/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVGENPRODTOOLS_IHEPMCWEIGHTSVC_H
#define EVGENPRODTOOLS_IHEPMCWEIGHTSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <string>
#include <map>

/// @brief IService to read/write HepMC's WeightContainer key names from/to IOVMetaDataContainers
///  author: will buttinger , NLAA
/// 
///
class IHepMCWeightSvc : virtual public IInterface
{
public:
  DeclareInterfaceID (IHepMCWeightSvc, 1, 0);

  using WeightMap = std::map<std::string, std::size_t>;

  
  /// If no weight names have been set yet, record them to metadata.
  virtual StatusCode setWeightNames (const WeightMap& weightNames,
                                     const EventContext& ctx = Gaudi::Hive::currentContext())= 0;

  /// Return the current weight names.
  virtual WeightMap weightNames (const EventContext& ctx = Gaudi::Hive::currentContext()) = 0;

  /// Return the current weight names.
  virtual std::vector<std::string> weightNameVec (const EventContext& ctx = Gaudi::Hive::currentContext()) = 0;
};

#endif
