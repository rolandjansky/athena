/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_MUROISUNPACKINGTOOL_H
#define L1DECODER_MUROISUNPACKINGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "IRoIsUnpackingTool.h"

// Forward declaration
class StoreGateSvc;



class MURoIsUnpackingTool
  : virtual public ::IRoIsUnpackingTool,
            public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MURoIsUnpackingTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~MURoIsUnpackingTool(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  MURoIsUnpackingTool();

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  /// Pointer to the StoreGate service
  StoreGateSvc_t m_storeGate;

  // Containers
  

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !L1DECODER_MUROISUNPACKINGTOOL_H
