///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// GenEventAsciiReader.h 
// Header file for class GenEventAsciiReader
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEALGS_GENEVENTASCIIREADER_H 
#define MCPARTICLEALGS_GENEVENTASCIIREADER_H 

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declaration
class IIOHepMcTool;

class GenEventAsciiReader : public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  GenEventAsciiReader( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~GenEventAsciiReader(); 

  // Assignment operator: 
  //GenEventAsciiReader &operator=(const GenEventAsciiReader &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// Default constructor: 
  GenEventAsciiReader();

  /** @brief Method to configure the front-end to read-back the
   *  @c HepMC::GenEvent.
   */
  void setupGenEventReaderTool();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  typedef ToolHandle<IIOHepMcTool> IIOHepMcTool_t;
  /** Tool to read the reference HepMC::GenEvent from a dedicated file
   */
  IIOHepMcTool_t m_genEventReader;
}; 


#endif //> MCPARTICLEALGS_GENEVENTASCIIREADER_H
