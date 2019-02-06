///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// IDecisionSvc.h 
// Header file for class IDecisionSvc
// Author: S.Binet<binet@cern.ch>
//         B.Radics<radbal@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_IDECISIONSVC_H 
#define ATHENAKERNEL_IDECISIONSVC_H 1

/**
 * @class IDecisionSvc
 * @brief This class defines a protocol to register boolean decisions and 
 * @brief and retrieve them and their combined result 
 */

// STL includes
#include <vector>
#include <string>

// fwd declares
class INamedInterface;
class EventContext;


class IDecisionSvc  
  : virtual public ::INamedInterface
{ 

public:
  DeclareInterfaceID( IDecisionSvc, 1, 0);
  virtual ~IDecisionSvc();

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

public:

  // Add Stream
  virtual StatusCode addStream(const std::string& stream) = 0;
  // Add methods of Accept/Require/Veto algorithms for a given stream
  virtual StatusCode addAcceptAlg(const std::string& name, 
				  const std::string& stream) = 0;
  virtual StatusCode addRequireAlg(const std::string& name,
				   const std::string& stream) = 0;
  virtual StatusCode addVetoAlg(const std::string& name,
				const std::string& stream) = 0;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  // Get methods of Accept/Require/Veto algorithms for a given stream
  virtual 
  const std::vector<std::string> getAcceptAlgs(const std::string& stream) const = 0;

  virtual 
  const std::vector<std::string> getRequireAlgs(const std::string& stream) const = 0;
  
  virtual 
  const std::vector<std::string> getVetoAlgs(const std::string& stream) const = 0;

  // Get list of streams
  virtual const std::vector<std::string> getStreams() const = 0;

  /// Test whether this event should be output, of a given stream
  virtual bool isEventAccepted(const std::string& stream) const = 0;
  virtual bool isEventAccepted(const std::string& stream, const EventContext&) const = 0;
}; 

#endif //> !ATHENAKERNEL_IDECISIONSVC_H
