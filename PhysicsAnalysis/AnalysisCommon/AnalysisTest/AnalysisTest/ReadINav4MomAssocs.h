/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// ReadINav4MomAssocs.h 
// Header file for class ReadINav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISTEST_READINAV4MOMASSOCS_H 
#define ANALYSISTEST_READINAV4MOMASSOCS_H 

/** Algorithm to test persistency of INav4MomAssocs class
 */

// STL includes

// HepMC / CLHEP includes

// FrameWork includes
#include "GaudiKernel/Algorithm.h"

// DataModel includes
#include "DataModel/ClassName.h"

// Forward declaration
class StoreGateSvc;

class ReadINav4MomAssocs : public Algorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  ReadINav4MomAssocs( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~ReadINav4MomAssocs(); 

  // Assignment operator: 
  //ReadINav4MomAssocs &operator=(const ReadINav4MomAssocs &alg); 

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

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  template <typename ASSOCS>
  StatusCode readAssocs( const std::string assocsName ) const;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  ReadINav4MomAssocs();

  /** Pointer to StoreGate
   */
  StoreGateSvc *m_storeGate;

  // Containers
  
  /** Input location of INav4MomAssocs container
   */
  StringProperty m_inavAssocsName;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
template <typename ASSOCS>
StatusCode ReadINav4MomAssocs::readAssocs( const std::string assocsName ) const
{
  StatusCode sc = StatusCode::SUCCESS;
  MsgStream log( msgSvc(), name() );


  const ASSOCS * assocs = 0;
  if ( m_storeGate->retrieve( assocs, assocsName ).isFailure() ||
       0 == assocs ) {

    log << MSG::ERROR
	<< "Could not retrieve " << ClassName<ASSOCS>::name() << " at : "
	<< assocsName
	<< endreq;
    return sc;
  }

  typename ASSOCS::object_iterator objEnd = assocs->endObject();
  for ( typename ASSOCS::object_iterator objItr = assocs->beginObject();
	objItr != objEnd;
	++objItr ) {
    log << MSG::INFO
	<< "--> e= " << (*objItr)->e()
	<< "\tnAssocs= " << assocs->getNumberOfAssociations(objItr)
	<< endreq;
  }

  return sc;
}

#endif //> ANALYSISTEST_READINAV4MOMASSOCS_H
