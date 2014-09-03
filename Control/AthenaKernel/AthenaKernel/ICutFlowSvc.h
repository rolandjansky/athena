///////////////////////// -*- C++ -*- //////////////////////////////////////////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
class EventBookkeeper;
class EventBookkeeperCollection;
class TTree;


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

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

public:

  /// Register filter in the CutFlowSvc and returns a pointer to the corresponding EventBookkeeper.
  /// This method should be used by filters that register themselves.
  virtual CutIdentifier selfRegisterFilter( const std::string& name,
                                            const std::string& description ) = 0;

  /// Register cut as child of a filter in the CutFlowSvc and returns a pointer to the corresponding EventBookkeeper.
  /// This method should be filters to register their internal cuts that are not Algs themselves. 
  /// The pointed EventBookkeeper is owned by the CutFlowSvc (i.e. must NOT be deleted by the CutFlowSvc clients). 
  virtual CutIdentifier registerCut( const std::string& name, const std::string& description, CutIdentifier parentCutID ) = 0;

  virtual void setFilterDescription( CutIdentifier cutID, const std::string& descr ) = 0;

  /// Tells CutFlowSvc that a filter should not be treated as standalone but as a child of another filter.
  /// This should be used by filters that use other filter Algs internally, like e.g. the LogicalFilterCombiner
  virtual void declareChildFilter(const std::string& name, CutIdentifier parentCutID ) = 0;

  /// Tells CutFlowSvc that a filter is used standalone by an outputStream with a given logical context.
  /// The only foreseen client should the DecisionSvc, with its Accept/Require/Veto.
  virtual void declareTopFilter(const std::string& name, const std::string& logic, const std::string& outputStream ) = 0;

  /// Tells CutFlowSvc to update event counter of a CutIdentifier cutID, using CutIdentifier returned by selfRegisterFilter or registerCut
  virtual void addEvent( CutIdentifier cutID ) = 0;

  /// Tells CutFlowSvc to update weighted event counter of a CutIdentifier cutID, using CutIdentifier returned by selfRegisterFilter or registerCut
  virtual void addEvent( CutIdentifier cutID, double weight) = 0;

  /// Helper function for D3PDs, dumps the CutFlowSvc content into a flat TTree.
  /// The returned TTree is owned by the caller and can be eventually written by the caller in its favorite output TFile. 
  virtual TTree* dumpCutFlowToTTree( const char* treeName="CutFlowTree" ) = 0;

  /// Inverse of above DumpCutFlowToTTree: when reading a D3PD, re-interpret the flat TTree to a usual transient EventBookkeeperCollection.
  /// This should be the only method able to read flat TTrees, the other CutFlowSvc functionalities always work with EventBookkeepers.
  /// The produced EventBookkeeperCollection remains internal to the CutFlowSvc, users can manipulate it using the usual ICutFlowSvc interface.
  virtual void loadCutFlowFromTTree( TTree* t ) = 0;

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

