///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcWriterTool.h 
// Header file for class HepMcWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_HEPMCWRITERTOOL_H 
#define MCPARTICLETOOLS_HEPMCWRITERTOOL_H 

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

class HepMcWriterTool : virtual public IIOHepMcTool,
			        public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  HepMcWriterTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

  /// Destructor: 
  virtual ~HepMcWriterTool(); 

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

  /** Process the @c HepMC::GenEvent through the I/O backend.
   */
  StatusCode write( const HepMC::GenEvent* evt );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  HepMcWriterTool();

  /** @brief Method to configure the back-end to write out the
   *  @c HepMC::GenEvent.
   */
  void setupBackend( Gaudi::Details::PropertyBase& ioBackendURL );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** URL of the I/O back-end (only "ASCII" for now...) glued with
   *  the name of the output file name.
   *  Ex: \"ascii:/home/foo/hepmc.txt\"
   *  If no protocol separator ':' is found, fallback is "ASCII"
   */
  StringProperty m_ioBackendURL;

  /** Location of the @c McEventCollection to be written out
   *  If there is more than 1 @c HepMC::GenEvent in the @c McEventCollection
   *  we will send warning messages, and just write the first one.
   */
  StringProperty m_mcEventsName;

  /** Abstract base class for the back-end
   */
  HepMC::IO_BaseClass* m_ioBackend;

}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_HEPMCWRITERTOOL_H
