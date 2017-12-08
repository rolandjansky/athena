/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVGENPRODTOOLS_IHEPMCWEIGHTSVC_H
#define EVGENPRODTOOLS_IHEPMCWEIGHTSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>

/// @brief IService to read/write HepMC's WeightContainer key names from/to IOVMetaDataContainers
///  author: will buttinger , NLAA
/// 
///

static const InterfaceID IID_IHepMCWeightSvc("IHepMCWeightSvc", 1 , 0);

class IHepMCWeightSvc : virtual public IInterface {
public:


   ///checks for any changes to weightnames ... none are allowed. Then records to metadata
   virtual StatusCode setWeightNames(const std::map<std::string, std::size_t>& weightNames)= 0;

   ///returns the current weight names ... will only change at file boudaries, it is assumed all events in a given file will have same weights in SAME ORDER
   virtual const std::map<std::string, std::size_t>& weightNames() = 0;

  static const InterfaceID& interfaceID() { return IID_IHepMCWeightSvc; }


};

#endif
