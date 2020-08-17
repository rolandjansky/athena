///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcReaderTool.h 
// Header file for class HepMcReaderTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_HEPMCREADERTOOL_H 
#define MCPARTICLETOOLS_HEPMCREADERTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/IIOHepMcTool.h"

// Forward declaration
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/IO_BaseClass_fwd.h"

class HepMcReaderTool : virtual public IIOHepMcTool,
			        public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  HepMcReaderTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

  /// Destructor: 
  virtual ~HepMcReaderTool(); 

  // Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /** Process the @c HepMC::GenEvent through the I/O frontend.
   */
  StatusCode read( HepMC::GenEvent* evt );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  HepMcReaderTool();

  /** @brief Method to configure the front-end to read out the
   *  @c HepMC::GenEvent.
   */
  void setupFrontend( Gaudi::Details::PropertyBase& ioFrontendURL );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** URL of the I/O front-end (only "ASCII" for now...) glued with
   *  the name of the input file name.
   *  Ex: \"ascii:/home/foo/hepmc.txt\"
   *  If no protocol separator ':' is found, fallback is "ASCII"
   */
  StringProperty m_ioFrontendURL;

  /** Location of the @c McEventCollection to be read out
   */
  StringProperty m_mcEventsOutputName;

  /** Abstract base class for the back-end
   */
  HepMC::IO_BaseClass* m_ioFrontend;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_HEPMCREADERTOOL_H
