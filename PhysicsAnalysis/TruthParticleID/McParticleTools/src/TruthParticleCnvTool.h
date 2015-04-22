///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleCnvTool.h 
// Header file for class TruthParticleCnvTool
// Author: S.Binet<binet@cern.ch>
// Purpose: Create a TruthParticleContainer from a HepMC::GenEvent
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLETOOLS_TRUTHPARTICLECNVTOOL_H
#define MCPARTICLETOOLS_TRUTHPARTICLECNVTOOL_H

/** \class TruthParticleCnvTool is an AlgTool that converts, on the fly, on 
 *  demand, each GenParticle to a TruthParticle and builds the 
 *  TruthParticleContainer. 
 *  So the GEN_AOD and TruthParticleContainer are identical in contents, only 
 *  the interfaces are different: the TruthParticle class has the same 
 *  interface as other AOD particles.
 *  Hence the analysis tools can operate on all the AOD particles (ie: 
 *  including Mc Truth).
 *  When the AOD is created, the TruthParticleContainer is empty. 
 *  When the user requests the TruthParticleContainer in his/her analysis code,
 *  a custom converter is triggered where by the GEN_AOD is convertered into 
 *  the TruthParticleContainer using the TruthParticleCnvTool. 
 *
 *
 *  In pile-up events, the many GenEvent are grouped into 5 TruthParticleContainer
 *  according to the classification of :
 *  https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PileupDigitization#Arrangement_of_Truth_Information 
 */

#include "HepPDT/ParticleDataTable.hh"

// Framework includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "EventKernel/IParticle.h" //> for ParticleDataType

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

#include "McParticleEvent/PileUpClassification.h"

// Forward declarations
class McEventCollection;
class TruthParticle;
class TruthParticleContainer;
class ITruthIsolationTool;

class TruthParticleCnvTool :  virtual public ITruthParticleCnvTool, 
			              public AthAlgTool   
{
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public:

  /// Constructor with parameters: 
  TruthParticleCnvTool( const std::string& type,
			const std::string& name, 
			const IInterface* parent );
  
  /// Destructor: 
  virtual ~TruthParticleCnvTool ();
  
  // Athena's Hooks
  virtual StatusCode initialize();
  virtual StatusCode execute();
  
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Converts a @c McEventCollection into an @c TruthParticleContainer (ie:
   *  converts it into an AOD compliant collection).
   *  @in  mcEvts the @c McEventCollection holding the @c HepMC::GenEvent we
   *      want to convert into a @c TruthParticleContainer
   *  @in  genEvtIndex the index to the @c HepMC::GenEvent to be converted
   *  @out mcParts a valid pointer to a @c TruthParticleContainer which will
   *       be filled with adaptors to @c HepMC::GenParticles.
   */
  StatusCode convert( const McEventCollection* mcEvts,
		      const unsigned int genEvtIndex,
		      TruthParticleContainer* mcParts,
		      const ITruthParticleVisitor* visitor ) const;

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 
  
  void setDataType( const int type );
  void setVxCandidateName( const std::string& name );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private:

  /** Type of data we are dealing with (Full/Fast/Truth/...)
   */
  ParticleDataType::DataType m_dataType;
  int m_dataType_int;
  
  std::string m_vxCandidatesName;

  /** Location of the McEventCollection the TruthParticles will be made from
   */
  StringProperty m_mcEventsName;

  /** Output TruthParticle location (built from the McEventCollection)
   */
  StringProperty m_mcPartsOutputName;

  /** Particle Property service
   */
  const HepPDT::ParticleDataTable * m_pdt;

  /** Type of truth particle we want to create
   */
  PileUpClassification::PileuType_t m_selectSignalType;

  /** Type of truth particle we want to create (property to be set by jobO)
   */
  int m_selectSignalTypeProp;


  
  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /**
   * Switch to compute or not the Et-isolations for @c TruthParticle (and
   * their underlying @c HepMC::GenParticle).
   * Default is to not compute these Et-isolations (and save CPU)
   */
  BooleanProperty m_doEtIsolation;

  typedef ToolHandle<ITruthIsolationTool> IsolTool_t;

  /** Pointer to the ITruthIsolationTool to be able to retrieve the
   *  previously computed transverse energy isolations.
   */
   IsolTool_t m_isolationTool;

};

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 
inline void TruthParticleCnvTool::setDataType( const int type ) 
{ 
  m_dataType = ParticleDataType::DataType(type); 
}

inline void TruthParticleCnvTool::setVxCandidateName( const std::string& name ) 
{ 
  m_vxCandidatesName = name; 
}

#endif // MCPARTICLETOOLS_TRUTHPARTICLECNVTOOL_H
