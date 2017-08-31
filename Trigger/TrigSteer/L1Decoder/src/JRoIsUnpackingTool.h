/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_JROISUNPACKINGTOOL_H
#define L1DECODER_JROISUNPACKINGTOOL_H 1


#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// L1Decoder includes
#include "./IRoIsUnpackingTool.h"

// Forward declaration
class StoreGateSvc;



class JRoIsUnpackingTool
  : virtual public ::IRoIsUnpackingTool,
            public ::AthAlgTool
{ 


 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  JRoIsUnpackingTool( const std::string& type,
	     const std::string& name, 
	     const IInterface* parent );

  /// Destructor: 
  virtual ~JRoIsUnpackingTool(); 
  StatusCode  updateConfiguration() override { return StatusCode::SUCCESS; }
  // Athena algtool's Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;

 private: 

  /// Default constructor: 
  JRoIsUnpackingTool();


  

}; 


#endif //> !L1DECODER_JROISUNPACKINGTOOL_H
