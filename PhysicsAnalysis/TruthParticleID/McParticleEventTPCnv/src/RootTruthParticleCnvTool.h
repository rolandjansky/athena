///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  virtual StatusCode execute() const override;
  virtual StatusCode execute (const EventContext& ctx) const override;

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
                                     void** ppvInterface ) override;
  virtual unsigned long addRef() override;
  virtual unsigned long release() override;
  virtual unsigned long refCount() const override;
  /// @}

  /// @name Function(s) inherited from IProperty
  /// @{
  virtual StatusCode setProperty( const std::string& s ) override;
  virtual StatusCode setProperty( const std::string& name, const Gaudi::Details::PropertyBase& p ) override;
  virtual StatusCode setPropertyRepr( const std::string& n, const std::string& r ) override;

  virtual StatusCode getProperty( Gaudi::Details::PropertyBase* p ) const override;
  virtual const Gaudi::Details::PropertyBase& getProperty( std::string_view name ) const override;
  virtual StatusCode getProperty( std::string_view n, std::string& v ) const override;
  virtual const std::vector<Gaudi::Details::PropertyBase*>& getProperties( ) const override;
  virtual bool hasProperty(std::string_view name) const override;
  /// @}

  /// @name Function(s) inherited from IAlgTool
  /// @{
  virtual const std::string& type() const override;
  virtual const IInterface* parent() const override;
  virtual StatusCode configure() override;
  virtual StatusCode initialize() override;
  virtual StatusCode sysInitialize() override;
  virtual StatusCode reinitialize() override;
  virtual StatusCode sysReinitialize() override;
  virtual StatusCode start() override;
  virtual StatusCode sysStart() override;
  virtual StatusCode restart() override;
  virtual StatusCode sysRestart() override;
  virtual StatusCode stop() override;
  virtual StatusCode sysStop() override;
  virtual StatusCode finalize() override;
  virtual StatusCode sysFinalize() override;
  virtual StatusCode terminate() override;
  virtual Gaudi::StateMachine::State FSMState() const override;
  /// @}

  /// @name Function(s) inherited from INamedInterface
  /// @{
  virtual const std::string& name() const override;
  /// @}

};

#endif // not MCPARTICLEEVENTTPCNV_ROOTTRUTHPARTICLECNVTOOL_H
