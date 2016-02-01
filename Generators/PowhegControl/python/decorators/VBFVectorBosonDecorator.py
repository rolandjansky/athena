# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl VBFVectorBosonDecorator
#  Powheg runcard decorator for VBF vector boson processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class VBFVectorBosonDecorator(object) :

  ## Define decorator name string
  name = 'VBF vector boson decay'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    # For additional run card
    self.decorated.allowed_decay_modes = []
    self.decorated.add_phantom( 'decay_mode', None,                                      desc='Vector boson decay mode' )
    self.decorated.add_phantom( 'mass_H', ATLASCommonParameters.mass_H,                  desc='mass of Higgs boson in GeV' )
    self.decorated.add_phantom( 'width_H', ATLASCommonParameters.width_H,                desc='width of Higgs boson in GeV' )
    self.decorated.add_phantom( 'mass_t', ATLASCommonParameters.mass_t,                  desc='mass of top quark in GeV' )
    self.decorated.add_phantom( 'mass_tau', ATLASCommonParameters.mass_tau,              desc='mass of tau in GeV' )
    self.decorated.add_phantom( 'mass_b', ATLASCommonParameters.mass_b,                  desc='mass of bottom quark in GeV' )
    self.decorated.add_phantom( 'mass_c', ATLASCommonParameters.mass_c,                  desc='mass of charm quark in GeV' )
    self.decorated.add_phantom( 'gfermi', ATLASCommonParameters.G_F,                     desc='Fermi constant' )
    self.decorated.add_phantom( 'alphaem_inv', 1.0/float(ATLASCommonParameters.alphaem), desc='EM coupling' )
    self.decorated.add_phantom( 'sthw2', ATLASCommonParameters.sin2thW_eff,              desc='sin**2 theta w' )
    self.decorated.add_phantom( 'mass_W', ATLASCommonParameters.mass_W,                  desc='mass of W-boson in GeV' )
    self.decorated.add_phantom( 'mass_Z', ATLASCommonParameters.mass_Z,                  desc='mass of Z-boson in GeV' )
    # Normal decorations
    self.decorated.add_parameter( 'mll_gencut', 20., desc='(minimum 15) cut on the invariant dilepton mass in GeV. Needed to avoid singularities from virtual photon decays to two massless leptons' )
    self.decorated.fix_parameter( 'Phasespace', 1,   desc='(1:standard phasespace; 2:use separate unweighted events as input' )
    self.decorated.add_parameter( 'ptj_gencut', 20,  desc='Generation cut on the jets pT in the phase space generator. Should be used when bornsuppfact is set to 0' )
    self.decorated.add_parameter( 'ptsuppfact', -1,  desc='(-1:use Powheg default) Born pT suppression factor' )


  def finalise( self ) :
    VBF_runcard_entries = []
    # Convert allowed decay mode into PROC_ID/DECAYMODE
    if self.decorated.decay_mode not in self.decorated.allowed_decay_modes :
      self.decorated.logger.warning( 'Decay mode {0} not recognised!'.format( self.decorated.decay_mode ) )
    vector_boson_type = self.decorated.decay_mode[0] if self.decorated.decay_mode[0] != 'W' else self.decorated.decay_mode[0:2]
    vector_boson_decay = self.decorated.decay_mode.replace(vector_boson_type,'').replace('v','').replace('ee','e').replace('mumu','mu')
    # Add runcard entries
    VBF_runcard_entries.append(( 'PROC_ID', { 'Z':120, 'Wp':130, 'Wm':140 }[vector_boson_type], vector_boson_type.replace('p','+').replace('m','-') ))
    VBF_runcard_entries.append(( 'DECAYMODE', { 'e':11, 'mu':13 }[vector_boson_decay], vector_boson_decay ))
    VBF_runcard_entries.append(( 'HMASS', self.decorated.mass_H, self.decorated.phantom_parameters['mass_H'][2] ))
    VBF_runcard_entries.append(( 'HWIDTH', self.decorated.width_H, self.decorated.phantom_parameters['width_H'][2] ))
    VBF_runcard_entries.append(( 'EWSCHEME', 4, 'Explicitly specify all EW parameters' ))
    VBF_runcard_entries.append(( 'ANOM_CPL', 0, 'Disable anomalous couplings as these are not supported by the authors' ))
    VBF_runcard_entries.append(( 'TOPMASS', self.decorated.mass_t, self.decorated.phantom_parameters['mass_t'][2] ))
    VBF_runcard_entries.append(( 'TAU_MASS', self.decorated.mass_tau, self.decorated.phantom_parameters['mass_tau'][2] ))
    VBF_runcard_entries.append(( 'BOTTOMMASS', self.decorated.mass_b, self.decorated.phantom_parameters['mass_b'][2] ))
    VBF_runcard_entries.append(( 'CHARMMASS', self.decorated.mass_c, self.decorated.phantom_parameters['mass_c'][2] ))
    VBF_runcard_entries.append(( 'FERMI_CONST', self.decorated.gfermi, self.decorated.phantom_parameters['gfermi'][2] ))
    VBF_runcard_entries.append(( 'INVALFA', self.decorated.alphaem_inv, self.decorated.phantom_parameters['alphaem_inv'][2] ))
    VBF_runcard_entries.append(( 'SIN2W', self.decorated.sthw2, self.decorated.phantom_parameters['sthw2'][2] ))
    VBF_runcard_entries.append(( 'WMASS', self.decorated.mass_W, self.decorated.phantom_parameters['mass_W'][2] ))
    VBF_runcard_entries.append(( 'ZMASS', self.decorated.mass_Z, self.decorated.phantom_parameters['mass_Z'][2] ))
    # Write special VBF runcard
    with open( 'vbfnlo.input', 'w' ) as f :
      [ f.write( '{0:<15} {1:<15} ! {2}\n'.format(*runcard_entry) ) for runcard_entry in VBF_runcard_entries ]
