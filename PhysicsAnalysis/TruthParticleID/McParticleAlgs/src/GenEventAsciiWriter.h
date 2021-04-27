///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenEventAsciiWriter.h 
// Header file for class GenEventAsciiWriter
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_GENEVENTASCIIWRITER_H 
#define MCPARTICLEALGS_GENEVENTASCIIWRITER_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class IIOHepMcTool;

class GenEventAsciiWriter : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  GenEventAsciiWriter( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~GenEventAsciiWriter(); 

  // Assignment operator: 
  //GenEventAsciiWriter &operator=(const GenEventAsciiWriter &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  GenEventAsciiWriter();

  /** @brief Method to configure the back-end to write out the
   *  @c HepMC::GenEvent.
   */
  void setupGenEventWriterTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandle<IIOHepMcTool> IIOHepMcTool_t;
  /** Tool to write the reference HepMC::GenEvent into a dedicated file
   */
  IIOHepMcTool_t m_genEventWriter;
}; 

#endif //> MCPARTICLEALGS_GENEVENTASCIIWRITER_H
