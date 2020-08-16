///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// I4MomDumper.h 
// Header file for class I4MomDumper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONALGS_I4MOMDUMPER_H 
#define EVENTCOMMONALGS_I4MOMDUMPER_H 

// STL includes
#include <string>
#include <iosfwd>


// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class I4Momentum;

class I4MomDumper : public AthAlgorithm
{ 
 public:
  struct Display {
    enum Type {
      EEtaPhiM = 0,
      IPtCotThPhiM,
      PtEtaPhiM,
      PxPyPzE,

      Size
    };
  };
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  I4MomDumper( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~I4MomDumper(); 

  // Assignment operator: 
  //I4MomDumper &operator=(const I4MomDumper &alg); 

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
  I4MomDumper();

  /// Callback method to configure the display property which will tell how to
  /// dump the informations from @c I4MomentumCollection
  void setupDisplay( Gaudi::Details::PropertyBase& display );

  /// Callback method to configure the output stream into which we'll
  /// dump the informations from @c I4MomentumCollection
  void setupOutputStream( Gaudi::Details::PropertyBase& outputStreamName );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  // Containers
  
  /// List of @c INavigable4Momentum containers one wants to dump
  StringArrayProperty m_i4momContainersName;

  /// switch for the type of printout (pxpypze/eetaphim/...)
  StringProperty m_displayName;

  /// its value translated into integer
  Display::Type m_display;

  ///  Name of the output stream where we'll dump informations from the
  /// @c I4Assocs object.
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

#endif //> EVENTCOMMONALGS_I4MOMDUMPER_H
