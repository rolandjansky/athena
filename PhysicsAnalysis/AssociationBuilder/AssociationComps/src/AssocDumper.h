///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AssocDumper.h 
// Header file for class AssocDumper
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ASSOCIATIONCOMPS_ASSOCDUMPER_H 
#define ASSOCIATIONCOMPS_ASSOCDUMPER_H 

// STL includes
#include <string>
#include <iosfwd>


// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Forward declaration
class INav4MomAssocs;

class AssocDumper : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  AssocDumper( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~AssocDumper(); 

  // Assignment operator: 
  //AssocDumper &operator=(const AssocDumper &alg); 

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
   *  of an @c INav4MomAssocs object
   */
  StatusCode dump( const INav4MomAssocs* assocs );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  AssocDumper();

  /** Callback method to configure the input location of the
   *  @c INav4MomAssocs we want to print out
   */
  void setupINav4MomAssocs( Gaudi::Details::PropertyBase& inav4MomAssocsName );

  /** Callback method to configure the output stream into which we'll
   *  dump the informations from @c INav4MomAssocs.
   */
  void setupOutputStream( Gaudi::Details::PropertyBase& outputStreamName );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  // Containers
  
  /** StoreGate location of the @c INav4MomAssocs we want to print out
   */
  StringProperty m_inav4MomAssocsName;

  /** Name of the output stream where we'll dump informations from the
   *  @c INav4MomAssocs object.
   *  Default: "MsgStream"
   *  Available streams: ["MsgStream", "stdout", "stderr", "anyfile"]
   */
  StringProperty m_outputStreamName;

  /** pointer to the file descriptor in case the output stream is not
   *  a "MsgStream"
   */
  std::ostream * m_outputStream;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ASSOCIATIONCOMPS_ASSOCDUMPER_H
