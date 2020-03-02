///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ConcreteSvc.h 
// Header file for class ConcreteSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXJOBOPTIONS_CONCRETESVC_H 
#define ATHEXJOBOPTIONS_CONCRETESVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// AthExJobOptions includes
#include "AthExJobOptions/IVirtualSvc.h"

// Forward declaration
template <class TYPE> class SvcFactory;

class ConcreteSvc : virtual public IVirtualSvc,
                            public AthService
{ 

protected:
    
  friend class SvcFactory<ConcreteSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ConcreteSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ConcreteSvc(); 

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

  /** The very important message of the day
   */
  StatusCode qotd( std::string& quote );

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  ConcreteSvc();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// The quote of the day
  StringProperty m_qotd;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline const InterfaceID& ConcreteSvc::interfaceID() 
{ 
  return IVirtualSvc::interfaceID(); 
}

#endif //> ATHEXJOBOPTIONS_CONCRETESVC_H
