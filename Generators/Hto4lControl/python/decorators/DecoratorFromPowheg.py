# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from PowhegControl.parameters import powheg_atlas_common

class DecoratorFromPowheg(object) :

  ## Define decorator name string
  name = 'fromPowheg'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_H', powheg_atlas_common.mass.H,       desc='Higgs boson mass', parameter='mh' )
    self.decorated.add_parameter( 'gfermi', powheg_atlas_common.EW_parameters.G_F, desc='Fermi constant', parameter='gf' )
    self.decorated.add_parameter( 'mass_Z', powheg_atlas_common.mass.Z,       desc='Z boson mass', parameter='mz' )
    self.decorated.add_parameter( 'mass_W', powheg_atlas_common.mass.W,       desc='W boson mass', parameter='mw' )
    self.decorated.add_parameter( 'width_Z', powheg_atlas_common.width.Z,     desc='on-shell Z-boson width (only to calculate pole mass)', parameter='gammaz' )
    self.decorated.add_parameter( 'width_W', powheg_atlas_common.width.W,     desc='on-shell W-boson width (only to calculate pole mass)', parameter='gammaw' )
    self.decorated.add_parameter( 'mass_e', powheg_atlas_common.mass.e,       desc='electron mass', parameter='me' )
    self.decorated.add_parameter( 'mass_mu', powheg_atlas_common.mass.mu,     desc='muon mass', parameter='mmu' )
    self.decorated.add_parameter( 'mass_tau', powheg_atlas_common.mass.tau,   desc='tau mass', parameter='mtau' )
    self.decorated.add_parameter( 'mass_d', powheg_atlas_common.mass.d,       desc='d-quark mass', parameter='md' )
    self.decorated.add_parameter( 'mass_u', powheg_atlas_common.mass.u,       desc='u-quark mass', parameter='mu' )
    self.decorated.add_parameter( 'mass_s', powheg_atlas_common.mass.s,       desc='s-quark mass', parameter='ms' )
    self.decorated.add_parameter( 'mass_c', powheg_atlas_common.mass.c,       desc='c-quark mass', parameter='mc' )
    self.decorated.add_parameter( 'mass_b', powheg_atlas_common.mass.b,       desc='b-quark mass', parameter='mb' )
    self.decorated.add_parameter( 'mass_t', powheg_atlas_common.mass.t,       desc='t-quark mass', parameter='mt' )

