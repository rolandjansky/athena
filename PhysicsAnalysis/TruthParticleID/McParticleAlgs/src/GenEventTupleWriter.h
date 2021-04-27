///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenEventTupleWriter.h 
// Header file for class GenEventTupleWriter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_GENEVENTTUPLEWRITER_H 
#define MCPARTICLEALGS_GENEVENTTUPLEWRITER_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class IIOHepMcTool;

class GenEventTupleWriter : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  GenEventTupleWriter( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~GenEventTupleWriter(); 

  // Assignment operator: 
  //GenEventTupleWriter &operator=(const GenEventTupleWriter &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  GenEventTupleWriter();

  /** @brief Method to configure the back-end to write out the
   *  @c HepMC::GenEvent.
   */
  void setupGenEventWriterTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandle<IIOHepMcTool> IIOHepMcTool_t;
  /** Tool to write the reference HepMC::GenEvent into a dedicated tuple
   */
  IIOHepMcTool_t m_genEventWriter;
}; 

#endif //> MCPARTICLEALGS_GENEVENTTUPLEWRITER_H
