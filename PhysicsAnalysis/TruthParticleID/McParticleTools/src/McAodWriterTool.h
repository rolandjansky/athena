///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodWriterTool.h 
// Header file for class McAodWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_MCAODWRITERTOOL_H 
#define MCPARTICLETOOLS_MCAODWRITERTOOL_H 

// STL includes
#include <string>
#include <fstream>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/IIOMcAodTool.h"

// Forward declaration
class TruthParticleContainer;

class McAodWriterTool : virtual public IIOMcAodTool,
			        public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McAodWriterTool( const std::string& type,
		   const std::string& name, 
		   const IInterface* parent );

  /// Destructor: 
  virtual ~McAodWriterTool(); 

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

  /** Process the @c TruthParticleContainer through the I/O backend.
   */
  StatusCode write( const TruthParticleContainer* mcParts );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  McAodWriterTool();

  /** @brief Method to configure the back-end to write out the
   *  @c TruthParticleContainer.
   */
  void setupBackend( Gaudi::Details::PropertyBase& ioBackendURL );

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** URL of the I/O back-end (only "ASCII" for now...) glued with
   *  the name of the output file name.
   *  Ex: \"ascii:/home/foo/mcaod.txt\"
   *  If no protocol separator ':' is found, fallback is "ASCII"
   */
  StringProperty m_ioBackendURL;

  /** Location of the @c TruthParticleContainer to be written out
   */
  StringProperty m_truthParticlesName;

  /** Abstract base class for the back-end
   */
  std::ofstream* m_ioBackend;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_MCAODWRITERTOOL_H
