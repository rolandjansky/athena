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
//class EventBookkeeper;
//class EventBookkeeperCollection;
//class TTree;
// namespace xAOD{
//   class CutBookkeeper;
// }


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

  /// Register filter in the CutFlowSvc and returns the CutID of the
  /// corresponding EventBookkeeper.
  /// This method should be used by filters that register themselves.
  virtual CutIdentifier registerFilter( const std::string& name,
                                        const std::string& description ) = 0;

  /// Register cut as child of a filter in the CutFlowSvc and returns the CutID
  /// of the corresponding EventBookkeeper. This method should be used by
  /// filters to register their internal cuts that are not the Algs themselves.
  virtual CutIdentifier registerCut( const std::string& name,
                                     const std::string& description,
                                     CutIdentifier parentCutID ) = 0;

  /// Tells CutFlowSvc that a filter is used directly by an outputStream with
  /// a given logical context. The only foreseen client should the DecisionSvc,
  /// with its Accept/Require/Veto.
  virtual CutIdentifier registerTopFilter( const std::string& name,
                                           const std::string& description,
                                           unsigned int logic,
                                           const std::string& outputStream ) = 0;

  /// Tells CutFlowSvc that a filter should not be treated as as being used by
  /// another filter. This should be used by filters that use other filter Algs
  /// internally, e.g., like the LogicalFilterCombiner
  virtual CutIdentifier declareUsedOtherFilter( const std::string& name,
                                                CutIdentifier originCutID ) = 0;

  /// Set the description of an existing EventBookkeeper
  virtual void setFilterDescription( CutIdentifier cutID,
                                     const std::string& descr ) = 0;

  /// Tells CutFlowSvc to update the event counter of a CutIdentifier cutID,
  /// using the CutIdentifier returned by selfRegisterFilter or registerCut
  /// Internally, the Monte Carlo event weight will be retrieved from the
  /// xAOD::EventInfo object as evtWeight = evtInfo->mcEventWeight();
  virtual void addEvent( CutIdentifier cutID ) = 0;

  /// Tells CutFlowSvc to update the weighted event counter of a CutIdentifier cutID,
  /// using CutIdentifier returned by selfRegisterFilter or registerCut
  virtual void addEvent( CutIdentifier cutID, double weight) = 0;

  /// Get a CutBookkeeper given a CutID
  // virtual xAOD::CutBookkeeper* getCutBookkeeper( const CutIdentifier cutID ) = 0;

  /// Helper function for D3PDs, dumps the CutFlowSvc content into a flat TTree.
  /// The returned TTree is owned by the caller and can be eventually written by the caller in its favorite output TFile.
  //virtual TTree* dumpCutFlowToTTree( const char* treeName="CutFlowTree" ) = 0;

  /// Inverse of above DumpCutFlowToTTree: when reading a D3PD, re-interpret the flat TTree to a usual transient EventBookkeeperCollection.
  /// This should be the only method able to read flat TTrees, the other CutFlowSvc functionalities always work with EventBookkeepers.
  /// The produced EventBookkeeperCollection remains internal to the CutFlowSvc, users can manipulate it using the usual ICutFlowSvc interface.
  //virtual void loadCutFlowFromTTree( TTree* t ) = 0;

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
