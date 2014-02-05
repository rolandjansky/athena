///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodTupleWriter.h 
// Header file for class McAodTupleWriter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_MCAODTUPLEWRITER_H 
#define MCPARTICLEALGS_MCAODTUPLEWRITER_H 

// STL includes
#include <string>

// HepMC / CLHEP includes

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class IIOMcAodTool;

class McAodTupleWriter : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McAodTupleWriter( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~McAodTupleWriter(); 

  // Assignment operator: 
  //McAodTupleWriter &operator=(const McAodTupleWriter &alg); 

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

  /// Default constructor: 
  McAodTupleWriter();

  /** @brief Method to configure the back-end to write out the
   *  @c HepMC::GenEvent.
   */
  void setupMcAodWriterTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandle<IIOMcAodTool> IIOMcAodTool_t;
  /** Tool to write the reference @c TruthParticleContainer into a dedicated 
   *  tuple
   */
  IIOMcAodTool_t m_mcAodWriter;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLEALGS_MCAODTUPLEWRITER_H
