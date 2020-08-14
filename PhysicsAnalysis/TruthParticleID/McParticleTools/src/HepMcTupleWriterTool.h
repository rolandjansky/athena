///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HepMcTupleWriterTool.h 
// Header file for class HepMcTupleWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_HEPMCTUPLEWRITERTOOL_H 
#define MCPARTICLETOOLS_HEPMCTUPLEWRITERTOOL_H 

// STL includes
#include <string>

// Boost includes
#include "boost/array.hpp"

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/IIOHepMcTool.h"

// Forward declaration
class ITHistSvc;
class TTree;
#include "AtlasHepMC/GenEvent_fwd.h"

class HepMcTupleWriterTool : virtual public IIOHepMcTool,
			             public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  HepMcTupleWriterTool( const std::string& type,
			const std::string& name, 
			const IInterface* parent );

  /// Destructor: 
  virtual ~HepMcTupleWriterTool(); 

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
  HepMcTupleWriterTool();

  /** @brief Method to configure the back-end to write out the
   *  @c HepMC::GenEvent.
   */
  void setupBackend( Gaudi::Details::PropertyBase& outputFileName );

  /**
   * @brief book the tuple
   */
  void bookTuple();

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /// maximum number of particles per event
  static const int s_nMax = 2000;

  /// A simple representation of a HepMc particle
  struct HepMcParticles {
    int m_nParticles;
    boost::array<double, s_nMax> m_px;
    boost::array<double, s_nMax> m_py;
    boost::array<double, s_nMax> m_pz;
    boost::array<double, s_nMax> m_m;
    boost::array<double, s_nMax> m_ene;
    boost::array<int,    s_nMax> m_pdgId;
    boost::array<int,    s_nMax> m_status;
    boost::array<int,    s_nMax> m_barcode;
  };
  /// our cached particles
  HepMcParticles m_particles;

  /// Pointer to @ ITHistSvc
  ServiceHandle<ITHistSvc> m_tupleSvc;

  /** Location of the @c McEventCollection to be written out
   *  If there is more than 1 @c HepMC::GenEvent in the @c McEventCollection
   *  we will send warning messages, and just write the first one.
   */
  StringProperty m_mcEventsName;

  /// Name of the output tuple file
  StringProperty m_outputFileName;

  /// Name of the output tuple stream
  StringProperty m_outputStreamName;

  /// cached pointer to the tuple
  TTree* m_tuple;
}; 

/// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> MCPARTICLETOOLS_HEPMCTUPLEWRITERTOOL_H
