///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ThinningOutputTool.h 
// Header file for class ThinningOutputTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_THINNINGOUTPUTTOOL_H 
#define ATHENASERVICES_THINNINGOUTPUTTOOL_H 

// STL includes
#include <string>
#include <vector>
#include <utility> // for std::pair

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// AthenaKernel includes
#include "AthenaKernel/IAthenaOutputTool.h"

// Forward declaration
class IThinningSvc;

class ThinningOutputTool : virtual public IAthenaOutputTool,
                                   public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ThinningOutputTool( const std::string& type,
		      const std::string& name, 
		      const IInterface* parent );

  /// Destructor: 
  virtual ~ThinningOutputTool(); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  /// Called at the beginning of @c AthenaOutputStream::execute()
  /// This will trigger the 'commit' of the @c IThinningSvc indices
  StatusCode preExecute();
  
  /// Called at the end of @c AthenaOutputStream::execute()
  /// This will trigger the 'rollback' of the @c IThinningSvc indices
  StatusCode postExecute();

  /// Called at the end of @c AthenaOutputStream::initialize()
  /// No-op (for now ?)
  StatusCode postInitialize() { return StatusCode::SUCCESS; }

  /// Called at the beginning of @c AthenaOutputStream::finalize()
  /// No-op (for now ?)
  StatusCode preFinalize() { return StatusCode::SUCCESS; }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  ThinningOutputTool();

  /** property callback to ensure correct format of 'Proxies'
   */
  void propertyHandler(Property& p);
  
  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  typedef ServiceHandle<IThinningSvc> ThinningSvc_t;
  /// Pointer to the @c IThinningSvc service
  ThinningSvc_t m_thinningSvc;

  /// Pointer to the active @c IThinningSvc service
  IThinningSvc* m_activeSvc;

  typedef std::vector<std::string> Proxy_t;
  typedef std::vector<Proxy_t> Proxies_t;
  /** list of pairs (clid,key) the tool will forcingly read from input
   *  to ensure a sound thinning state (ie: all element-links will be correctly
   *  massaged by the thinning machinery)
   */
  Proxies_t m_proxies;

  // Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENASERVICES_THINNINGOUTPUTTOOL_H
