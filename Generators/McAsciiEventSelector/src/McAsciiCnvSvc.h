///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAsciiCnvSvc.h 
// Header file for class McAsciiCnvSvc
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCASCIIEVENTSELECTOR_MCASCIICNVSVC_H 
#define MCASCIIEVENTSELECTOR_MCASCIICNVSVC_H 

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/MsgStream.h"

// Forward declaration
class IIOHepMcTool;

class McAsciiCnvSvc : public ConversionSvc
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  
  /// Constructor with parameters: 
  McAsciiCnvSvc( const std::string& name, ISvcLocator* svcLoc );

  /// Destructor: 
  virtual ~McAsciiCnvSvc(); 

  //@{
  /// Initialize the service.
  virtual StatusCode initialize();

  /// Update state of the service
  virtual StatusCode updateServiceState(IOpaqueAddress* pAddress);
  //@}

  /// storage type
  virtual long repSvcType() const;
  static long storageType() ;

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  void setInputFile( const std::string& asciiFileName );

  /// Main entry point to read an ASCII file and create a @c McEventCollection
  /// out of it.
  StatusCode cnv();

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// configure the underlying converter tool
  void setupCnvTool();

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// MsgStream instance (a std::cout like with print-out levels)
  mutable MsgStream m_msg;

  /// pointer to the @c McEventCollection tool converter
  IIOHepMcTool* m_cnvTool;

  /// Output location of the @c McEventCollection
  Gaudi::Property<std::string> m_mcEventsOutputName;
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCASCIIEVENTSELECTOR_MCASCIICNVSVC_H
