/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// WriteINav4MomAssocs.h 
// Header file for class WriteINav4MomAssocs
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ANALYSISTEST_WRITEINAV4MOMASSOCS_H 
#define ANALYSISTEST_WRITEINAV4MOMASSOCS_H 

/** Algorithm to test persistency of INav4MomAssocs class
 */

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// DataModel includes
#include "DataModel/ClassName.h"

// NqvFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"

class WriteINav4MomAssocs : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  WriteINav4MomAssocs( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~WriteINav4MomAssocs(); 

  // Assignment operator: 
  //WriteINav4MomAssocs &operator=(const WriteINav4MomAssocs &alg); 

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
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  template < typename INCOLL, typename OUTCOLL >
    StatusCode symLink( const std::string& collName ) const;

  StatusCode buildAssocs( const INavigable4MomentumCollection * coll1,
			  const INavigable4MomentumCollection * coll2 ) const;

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  WriteINav4MomAssocs();

  // Containers
  
  /** Input location for ParticleJet container
   */
  std::string m_jetsName;

  /** Input location for Electron container
   */
  std::string m_electronsName;

  /** Output location for INav4MomAssocs container
   */
  std::string m_inavAssocsOutputName;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

template < typename INCOLL, typename OUTCOLL >
StatusCode WriteINav4MomAssocs::symLink( const std::string& collName ) const
{
  const INCOLL * inColl = 0;
  ATH_CHECK ( evtStore()->retrieve( inColl, collName ) );

  const OUTCOLL * outColl = 0;
  ATH_CHECK ( evtStore()->symLink( inColl, outColl ) );

  return StatusCode::SUCCESS;
}

#endif //> ANALYSISTEST_WRITEINAV4MOMASSOCS_H
