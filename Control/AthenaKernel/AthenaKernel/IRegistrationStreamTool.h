/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IREGISTRATIONSTREAMTOOL_H
#define ATHENAKERNEL_IREGISTRATIONSTREAMTOOL_H 1

/**
 * @file IRegistrationStreamTool.h
 *
 * @brief AlgTool which takes references provided by RegStream 
 * finds the appropriate CORAL object in storegate and stores 
 * them in a POOL collection. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * @author Jack Cranshaw <Jack.Cranshaw@cern.ch>
 *
 * $Id: IRegistrationStreamTool.h,v 1.4 2009-04-28 18:57:40 cranshaw Exp $
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/IAlgTool.h"

#include <map>

//<<<<<< PUBLIC TYPES                                                   >>>>>>


/**
 * @class IRegistrationStreamTool
 *
 * @brief AlgTool which takes references provided by RegStream 
 * finds the appropriate CORAL object in storegate and stores 
 * them in a POOL collection. 
 *
 */

class IRegistrationStreamTool : virtual public IAlgTool {
public:
   /// Working entry point
   virtual StatusCode fill(std::vector<std::pair<std::string, std::string> >& refs, std::string key) = 0;

   virtual StatusCode commit() = 0;

   virtual const CLID& listID() = 0;

   virtual void setCollMetadataKeys(const std::vector<std::string>& keys) = 0;

   /// Gaudi boilerplate
   static const InterfaceID& interfaceID();
};

//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline const InterfaceID& IRegistrationStreamTool::interfaceID() {
  static const InterfaceID _IID("IRegistrationStreamTool", 1, 0);
  return _IID;
}

#endif // ATHENAKERNEL_IREGISTRATIONSTREAMTOOL_H
