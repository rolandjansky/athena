///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomDumper.h 
// Header file for class INav4MomDumper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONALGS_INAV4MOMDUMPER_H 
#define EVENTCOMMONALGS_INAV4MOMDUMPER_H 

// STL includes
#include <string>
#include <iosfwd>


// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

class INav4MomDumper : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  INav4MomDumper( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~INav4MomDumper(); 

  // Assignment operator: 
  //INav4MomDumper &operator=(const INav4MomDumper &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Prints out (on the configured output stream) the content
   *  of an @c INavigable4MomentumCollection object, given its @c StoreGate
   *  key.
   */
  StatusCode dump( const std::string& collName );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  INav4MomDumper();

  /// Callback method to configure the output stream into which we'll
  /// dump the informations from @c INavigable4MomentumCollection
  void setupOutputStream( Gaudi::Details::PropertyBase& outputStreamName );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  // Containers
  
  /// List of @c INavigable4Momentum containers one wants to dump
  StringArrayProperty m_inav4momContainersName;

  ///  Name of the output stream where we'll dump informations from the
  /// @c INav4MomAssocs object.
  /// Default: "MsgStream"
  /// Available streams: ["MsgStream", "stdout", "stderr", "anyfile"]
  StringProperty m_outputStreamName;

  /// pointer to the file descriptor in case the output stream is not
  /// a "MsgStream"
  std::ostream * m_outputStream;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> EVENTCOMMONALGS_INAV4MOMDUMPER_H
