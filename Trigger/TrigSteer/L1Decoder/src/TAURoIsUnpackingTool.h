/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_TAUROISUNPACKINGTOOL_H
#define L1DECODER_TAUROISUNPACKINGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "IRoIsUnpackingTool.h"

// Forward declaration
class StoreGateSvc;



class TAURoIsUnpackingTool
  : virtual public ::IRoIsUnpackingTool,
            public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TAURoIsUnpackingTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~TAURoIsUnpackingTool(); 
  StatusCode  updateConfiguration() override { return StatusCode::SUCCESS; }
  // Athena algtool's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;

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
  TAURoIsUnpackingTool();

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


#endif //> !L1DECODER_TAUROISUNPACKINGTOOL_H
