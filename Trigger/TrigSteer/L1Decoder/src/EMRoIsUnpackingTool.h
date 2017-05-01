///////////////////////// -*- C++ -*- /////////////////////////////
// EMRoIsUnpackingTool.h 
// Header file for class EMRoIsUnpackingTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef L1DECODER_EMROISUNPACKINGTOOL_H
#define L1DECODER_EMROISUNPACKINGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "./IRoIsUnpackingTool.h"

// Forward declaration
class StoreGateSvc;



class EMRoIsUnpackingTool
  : virtual public ::IRoIsUnpackingTool,
            public ::AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  EMRoIsUnpackingTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~EMRoIsUnpackingTool(); 

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
  EMRoIsUnpackingTool();

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


#endif //> !L1DECODER_EMROISUNPACKINGTOOL_H
