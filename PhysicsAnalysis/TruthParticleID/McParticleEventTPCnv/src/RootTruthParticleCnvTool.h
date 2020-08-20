///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECNVTOOL_H
#define MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECNVTOOL_H

#include "McParticleKernel/ITruthParticleCnvTool.h"
#include "Gaudi/Property.h"

class RootTruthParticleCnvTool
  : public ITruthParticleCnvTool
{
public:
  /// @name Function(s) inherited from ITruthParticleCnvTool
  /// @{

  /// A hook for Athena algorithms
  virtual StatusCode execute() const;
  virtual StatusCode execute (const EventContext& ctx) const;

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

  /// @}

  /** Helper method to get the charge of a particle given its PDG Id.
   */
  double chargeFromPdgId( int pdgId ) const;


  /// @name Function(s) inherited from IInterface
  /// @{
  virtual StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvInterface );
  virtual unsigned long addRef();
  virtual unsigned long release();
  virtual unsigned long refCount() const;
  /// @}

  /// @name Function(s) inherited from IProperty
  /// @{
  virtual StatusCode setProperty( const std::string& s );
  virtual StatusCode setProperty( const std::string& name, const Gaudi::Details::PropertyBase& p );
  virtual StatusCode setPropertyRepr( const std::string& n, const std::string& r );

  virtual StatusCode getProperty( Gaudi::Details::PropertyBase* p ) const;
  virtual const Gaudi::Details::PropertyBase& getProperty( const std::string& name ) const;
  virtual StatusCode getProperty( const std::string& n, std::string& v ) const;
  virtual const std::vector<Gaudi::Details::PropertyBase*>& getProperties( ) const;
  virtual bool hasProperty(const std::string& name) const;
  /// @}

  /// @name Function(s) inherited from IAlgTool
  /// @{
  virtual const std::string& type() const;
  virtual const IInterface* parent() const;
  virtual StatusCode configure();
  virtual StatusCode initialize();
  virtual StatusCode sysInitialize();
  virtual StatusCode reinitialize();
  virtual StatusCode sysReinitialize();
  virtual StatusCode start();
  virtual StatusCode sysStart();
  virtual StatusCode restart();
  virtual StatusCode sysRestart();
  virtual StatusCode stop();
  virtual StatusCode sysStop();
  virtual StatusCode finalize();
  virtual StatusCode sysFinalize();
  virtual StatusCode terminate();
  virtual Gaudi::StateMachine::State FSMState() const;
  /// @}

  /// @name Function(s) inherited from INamedInterface
  /// @{
  virtual const std::string& name() const;
  /// @}

};

#endif // not MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECNVTOOL_H
