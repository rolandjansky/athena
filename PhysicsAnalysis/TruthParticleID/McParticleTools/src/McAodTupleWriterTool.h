///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodTupleWriterTool.h 
// Header file for class McAodTupleWriterTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_MCAODTUPLEWRITERTOOL_H 
#define MCPARTICLETOOLS_MCAODTUPLEWRITERTOOL_H 

// STL includes
#include <string>

// Boost includes
#include "boost/array.hpp"

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// McParticleKernel includes
#include "McParticleKernel/IIOMcAodTool.h"

// Forward declaration
class ITHistSvc;
class TTree;
class TruthParticleContainer;

class McAodTupleWriterTool : virtual public IIOMcAodTool,
			             public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  McAodTupleWriterTool( const std::string& type,
			const std::string& name, 
			const IInterface* parent );

  /// Destructor: 
  virtual ~McAodTupleWriterTool(); 

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
  StatusCode write( const TruthParticleContainer* mcAod );

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Default constructor: 
   */
  McAodTupleWriterTool();

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

  /// A simple representation of a @c TruthParticle
  struct McAodParticles {
    unsigned int m_nParticles;
    boost::array<double, s_nMax> m_px;
    boost::array<double, s_nMax> m_py;
    boost::array<double, s_nMax> m_pz;
    boost::array<double, s_nMax> m_m;
    boost::array<double, s_nMax> m_ene;
    boost::array<int,    s_nMax> m_pdgId;
    boost::array<int,    s_nMax> m_status;
    boost::array<int,    s_nMax> m_barcode;
    // et isolation stuff
    boost::array<double, s_nMax> m_etcone10;
    boost::array<double, s_nMax> m_etcone20;
    boost::array<double, s_nMax> m_etcone30;
    boost::array<double, s_nMax> m_etcone40;
    boost::array<double, s_nMax> m_etcone45;
    boost::array<double, s_nMax> m_etcone50;
    boost::array<double, s_nMax> m_etcone60;
    boost::array<double, s_nMax> m_etcone70;
  };
  /// our cached particles
  McAodParticles m_particles;

  /// Pointer to @ ITHistSvc
  ServiceHandle<ITHistSvc> m_tupleSvc;

  /** Location of the @c TruthParticleContainer to be written out.
   */
  StringProperty m_truthParticlesName;

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

#endif //> MCPARTICLETOOLS_MCAODTUPLEWRITERTOOL_H
