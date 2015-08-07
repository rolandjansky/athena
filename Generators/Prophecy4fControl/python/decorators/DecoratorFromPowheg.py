#! /usr/bin/env python

from PowhegControl import ATLASCommonParameters

class DecoratorFromPowheg(object) :

  ## Define decorator name string
  name = 'fromPowheg'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_H', ATLASCommonParameters.mass_H,       desc='Higgs boson mass', parameter='mh' )
    self.decorated.add_parameter( 'gfermi', ATLASCommonParameters.G_F,             desc='Fermi constant', parameter='gf' )
    self.decorated.add_parameter( 'mass_Z', ATLASCommonParameters.mass_Z,       desc='Z boson mass', parameter='mz' )
    self.decorated.add_parameter( 'mass_W', ATLASCommonParameters.mass_W,       desc='W boson mass', parameter='mw' )
    self.decorated.add_parameter( 'width_Z', ATLASCommonParameters.width_Z,     desc='on-shell Z-boson width (only to calculate pole mass)', parameter='gammaz' )
    self.decorated.add_parameter( 'width_W', ATLASCommonParameters.width_W,     desc='on-shell W-boson width (only to calculate pole mass)', parameter='gammaw' )
    self.decorated.add_parameter( 'mass_e', ATLASCommonParameters.mass_e,       desc='electron mass', parameter='me' )
    self.decorated.add_parameter( 'mass_mu', ATLASCommonParameters.mass_mu,     desc='muon mass', parameter='mmu' )
    self.decorated.add_parameter( 'mass_tau', ATLASCommonParameters.mass_tau,   desc='tau mass', parameter='mtau' )
    self.decorated.add_parameter( 'mass_d', ATLASCommonParameters.mass_d,       desc='d-quark mass', parameter='md' )
    self.decorated.add_parameter( 'mass_u', ATLASCommonParameters.mass_u,       desc='u-quark mass', parameter='mu' )
    self.decorated.add_parameter( 'mass_s', ATLASCommonParameters.mass_s,       desc='s-quark mass', parameter='ms' )
    self.decorated.add_parameter( 'mass_c', ATLASCommonParameters.mass_c,       desc='c-quark mass', parameter='mc' )
    self.decorated.add_parameter( 'mass_b', ATLASCommonParameters.mass_b,       desc='b-quark mass', parameter='mb' )
    self.decorated.add_parameter( 'mass_t', ATLASCommonParameters.mass_t,       desc='t-quark mass', parameter='mt' )

