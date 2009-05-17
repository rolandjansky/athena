///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AnotherConcreteSvc.h 
// Header file for class AnotherConcreteSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHEXJOBOPTIONS_ANOTHERCONCRETESVC_H 
#define ATHEXJOBOPTIONS_ANOTHERCONCRETESVC_H  1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// AthExJobOptions includes
#include "AthExJobOptions/IVirtualSvc.h"

// Forward declaration
template <class TYPE> class SvcFactory;

class AnotherConcreteSvc : virtual public IVirtualSvc,
                            public AthService
{ 

protected:
    
  friend class SvcFactory<AnotherConcreteSvc>;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AnotherConcreteSvc( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AnotherConcreteSvc(); 

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
  AnotherConcreteSvc();

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

inline const InterfaceID& AnotherConcreteSvc::interfaceID() 
{ 
  return IVirtualSvc::interfaceID(); 
}

#endif //> ATHEXJOBOPTIONS_ANOTHERCONCRETESVC_H
