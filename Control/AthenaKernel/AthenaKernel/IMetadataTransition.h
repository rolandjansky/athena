/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IMETADATATRANSITION_H
#define ATHENAKERNEL_IMETADATATRANSITION_H

/** @file IMetadataTransition.h
 *  @brief This file contains the class definition for the IMetadataTransition class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: IMetadataTransition.h,v 1.2 2007-07-30 19:06:50 gemmeren Exp $
 **/

#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/Incident.h"
#include <string>

/** @class IMetadataTransition
 *  @brief This class provides the interface for MetadataTransitions.
 **/
class IMetadataTransition : virtual public ::INamedInterface {

public: // Non-static members

  /// Function called when a new input file is opened
  virtual StatusCode newMetadataSource(const Incident&) = 0;

  /// Function called when the currently open input file got completely
  /// processed
  virtual StatusCode retireMetadataSource(const Incident&) = 0;

  /// Function called when the tool should write out its metadata
  virtual StatusCode prepareOutput() = 0;

  virtual StatusCode shmProxy(const std::string& filename) = 0;

  /// Gaudi boilerplate
   static const InterfaceID& interfaceID();
private: // Data
};

inline const InterfaceID& IMetadataTransition::interfaceID() {
   static const InterfaceID IID("IMetadataTransition", 1, 0);
   return(IID);
}

#endif

