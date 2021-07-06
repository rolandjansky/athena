///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CallGraphBuilderSvc.h 
// Header file for class CallGraphBuilderSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONCOMPS_CALLGRAPHBUILDERSVC_H 
#define PERFMONCOMPS_CALLGRAPHBUILDERSVC_H 

// STL includes
#include <string>
#include <stack>

// boost includes
//Remove in boost > 1.76 when the boost iterator issue
//is solved see ATLASRECTS-6358
#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/graph/adjacency_list.hpp>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/HashMap.h"

// PerfMonKernel includes
#include "PerfMonKernel/ICallGraphBuilderSvc.h"

// Forward declaration
class ISvcLocator;
template <class TYPE> class SvcFactory;

namespace PerfMon {

class CallGraphBuilderSvc : virtual public ICallGraphBuilderSvc,
 	                            public AthService
{ 

protected:
    
  friend class SvcFactory<CallGraphBuilderSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public typedefs: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  typedef unsigned long NodeId_t;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  CallGraphBuilderSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~CallGraphBuilderSvc(); 

  /// Gaudi Service Implementation
  //@{
  StatusCode initialize();
  StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface );
  //@}

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  static const InterfaceID& interfaceID();

  /// open a new node in the call graph tree
  void openNode( const std::string& nodeName );

  /// close an existing node in the call graph tree
  void closeNode( const std::string& nodeName );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  CallGraphBuilderSvc();

  /// a "unique identifier" for node names
  static NodeId_t m_uuid;

  /// Method-name-to-UUID
  GaudiUtils::HashMap<std::string, NodeId_t> m_nameToId;
  /// UUID-to-Method-name
  GaudiUtils::HashMap<NodeId_t, std::string> m_idToName;

  /// stack of method names (in fact their uuid)
  std::stack<NodeId_t> m_stack;

  typedef boost::adjacency_list<> CallGraph_t;
  /// the callgraph
  CallGraph_t m_graph;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& CallGraphBuilderSvc::interfaceID() 
{ 
   return ICallGraphBuilderSvc::interfaceID(); 
}

} // end namespace PerfMon

#endif //> PERFMONCOMPS_CALLGRAPHBUILDERSVC_H
