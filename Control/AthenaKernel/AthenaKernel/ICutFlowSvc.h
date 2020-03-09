///////////////////////// -*- C++ -*- //////////////////////////////////////////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header file for class ICutFlowSvc                                                                  //
// Authors: Joao Firmino da Costa (joao.costa@cern.ch) and David Cote (david.cote@cern.ch), July 2010 //
////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ATHENAKERNEL_ICUTFLOWSVC_H
#define ATHENAKERNEL_ICUTFLOWSVC_H 1

//<<<<<< INCLUDES                                                       >>>>>>
#include <vector>
#include <string>
#include <map>
#include <stdint.h> // for uint32_t

//<<<<<< FORWARD DECLARATIONS                                           >>>>>>
class INamedInterface;


/// InstanceIdentifier is a unique identifer used for every AthFilterAlgorithm instance
typedef uint32_t CutIdentifier;


/**
 * @class ICutFlowSvc
 * @brief This class provides an interface between event filtering algorithms and athena input/output streams,
 * @brief and returns a comprehensive summary of filters "Cut Flow" by internally using T/P EventBookkeeper objects.
 * @brief ICutFlowSvc also supports flat TTree format for D3PD usage.
 */

class ICutFlowSvc
  : virtual public ::INamedInterface
{

public:

  virtual ~ICutFlowSvc();


public:

  /// Register filter in the CutFlowSvc and returns the CutID of the
  /// corresponding EventBookkeeper.
  /// This method should be used by filters that register themselves.
  virtual CutIdentifier registerFilter( const std::string& name,
                                        const std::string& description ) = 0;

  /// Tells CutFlowSvc that a filter is used directly by an outputStream with
  /// a given logical context. The only foreseen client should the DecisionSvc,
  /// with its Accept/Require/Veto.
  virtual CutIdentifier registerTopFilter( const std::string& name,
                                           const std::string& description,
                                           unsigned int logic,
                                           const std::string& outputStream ) = 0;

  /// Set the description of an existing EventBookkeeper
  virtual void setFilterDescription( CutIdentifier cutID,
                                     const std::string& descr ) = 0;

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter
  virtual void addEvent( CutIdentifier cutID, double weight) = 0;

  /// Get number of accepted events for a cut
  virtual uint64_t getNAcceptedEvents( const CutIdentifier cutID ) const = 0;

  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();

};


//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline
const InterfaceID&
ICutFlowSvc::interfaceID()
{
  static const InterfaceID IID_ICutFlowSvc("ICutFlowSvc", 1, 0);
  return IID_ICutFlowSvc;
}


#endif //> !ATHENAKERNEL_ICUTFLOWSVC_H
