# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to decorate Powheg runcard with common parameter sets
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import SMParams

def decorate( powheg_controller, decorator ) :
  if decorator == 'CKM' :
    CKMDecorator(powheg_controller)
  elif decorator == 'diboson' :
    DibosonDecorator(powheg_controller)
  elif decorator == 'fixed scale' :
    FixedScaleDecorator(powheg_controller)
  elif decorator == 'heavy quark' :
    HeavyQuarkDecorator(powheg_controller)
  elif decorator == 'Higgs' :
    HiggsDecorator(powheg_controller)
  elif decorator == 'Higgs v2' :
    HiggsV2Decorator(powheg_controller)
  elif decorator == 'HJ' :
    HJDecorator(powheg_controller)
  elif decorator == 'HV' :
    HVDecorator(powheg_controller)
  elif decorator == 'on-shell' :
    OnShellDecorator(powheg_controller)
  elif decorator == 'running scale' :
    RunningScaleDecorator(powheg_controller)
  elif decorator == 'single boson' :
    SingleBosonDecorator(powheg_controller)
  elif decorator == 'top' :
    TopDecorator(powheg_controller)
  elif decorator == 'v2' :
    V2Decorator(powheg_controller)
  elif decorator == 'vector boson decay' :
    VectorBosonDecayDecorator(powheg_controller)
  else :
    powheg_controller._logger.warning( 'Unknown decorator: {0}'.format(decorator) )


###############################################################################
#
#  CKM decorator
#
###############################################################################
class CKMDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'CKM_Vud '+str(SMParams.CKM_Vud)+' ! CKM element \n' )
      f.write( 'CKM_Vus '+str(SMParams.CKM_Vus)+' ! CKM element \n' )
      f.write( 'CKM_Vub '+str(SMParams.CKM_Vub)+' ! CKM element \n' )
      f.write( 'CKM_Vcd '+str(SMParams.CKM_Vcd)+' ! CKM element \n' )
      f.write( 'CKM_Vcs '+str(SMParams.CKM_Vcs)+' ! CKM element \n' )
      f.write( 'CKM_Vcb '+str(SMParams.CKM_Vcb)+' ! CKM element \n' )
      f.write( 'CKM_Vtd '+str(SMParams.CKM_Vtd)+' ! CKM element \n' )
      f.write( 'CKM_Vts '+str(SMParams.CKM_Vts)+' ! CKM element \n' )
      f.write( 'CKM_Vtb '+str(SMParams.CKM_Vtb)+' ! CKM element \n' )

###############################################################################
#
#  Diboson decorator
#
###############################################################################
class DibosonDecorator :
  def __init__( self, decorated ) :
    decorate( decorated, 'on-shell' )
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.allowed_decay_modes = []
    self.decorated.decay_mode           = None

    self.decorated.delg1_g           = 0
    self.decorated.delg1_z           = 0
    self.decorated.delk_g            = 0
    self.decorated.delk_z            = 0
    self.decorated.dronly            = 0
    self.decorated.lambda_g          = 0
    self.decorated.lambda_z          = 0
    self.decorated.runningwidth      = 1
    self.decorated.withinterference  = 1
    self.decorated.tevscale          = -1
    self.decorated._use_delta_lambda = True

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      if self.decorated._use_delta_lambda :
        f.write( 'delg1_g '+str(self.decorated.delg1_g)+'                 ! Delta_g1(Gamma)\n' )
        f.write( 'delg1_z '+str(self.decorated.delg1_z)+'                 ! Delta_g1(Z)\n' )
        f.write( 'delk_g '+str(self.decorated.delk_g)+'                   ! Delta_K(Gamma)\n' )
        f.write( 'delk_z '+str(self.decorated.delk_z)+'                   ! Delta_K(Z)\n' )
        f.write( 'lambda_g '+str(self.decorated.lambda_g)+'               ! Lambda(gamma)\n' )
        f.write( 'lambda_z '+str(self.decorated.lambda_z)+'               ! Lambda(Z)\n' )
        f.write( 'tevscale '+str(self.decorated.tevscale)+'               ! W form-factor scale, in TeV\n' )
      # Global diboson parameters
      f.write( 'dronly '+str(self.decorated.dronly)+'                     ! (default 0) if 1 include only double resonant diagrams \n' )
      f.write( 'runningwidth '+str(self.decorated.runningwidth)+'         ! (default 0) if 1 using running width\n' )
      f.write( 'withinterference '+str(self.decorated.withinterference)+' ! if 1 (true) include interference for like flavour charged leptons\n' )
      # Add all decay modes
      if self.decorated.decay_mode is not None :
        if self.decorated.decay_mode in self.decorated.allowed_decay_modes :
          for decay_mode in self.decorated.allowed_decay_modes :
            f.write( decay_mode+' {0}                                     ! Diboson decay mode\n'.format([-1,1][decay_mode==self.decorated.decay_mode]) )
        else :
          self.decorated_logger.warning( 'Decay mode {0} not recognised!'.format( self.decorated.decay_mode) )

###############################################################################
#
#  Fixed-scale decorator
#
###############################################################################
class FixedScaleDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.fixedscale = -1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'fixedscale '+str(self.decorated.fixedscale)+' ! use reference renormalisation and factorisation scales \n' )

###############################################################################
#
#  Running-scale decorator
#
###############################################################################
class RunningScaleDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.runningscale = 1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'runningscale '+str(self.decorated.runningscale)+' ! choice for renormalisation and factorisation scales in Bbar integration\n' )

###############################################################################
#
#  Heavy quark decorator
#
###############################################################################
class HeavyQuarkDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'bmass_lhe '+str(SMParams.mass_b)+'                        ! \n' )
      f.write( 'cmass_lhe '+str(SMParams.mass_c)+'                        ! \n' )

###############################################################################
#
#  Higgs decorator
#
###############################################################################
class HiggsDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.hdecaymode      = -1
    self.decorated.higgsfixedwidth = -1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'hdecaymode '+str(self.decorated.hdecaymode)+'           ! Higgs boson decay (-1:no decay; 0:all; 1-6:dd, uu etc.; 7-9:e+e-, etc.; 10:WW; 11:ZZ; 12:gammagamma)\n' )
      f.write( 'higgsfixedwidth '+str(self.decorated.higgsfixedwidth)+' ! 0 = OS, 1 = MSBAR, 2 = DRBAR. (default 0), If 1 uses standard, fixed width Breit-Wigner, if 0 it uses the running width Breit-Wigner\n' )
      f.write( 'hmass '+str(SMParams.mass_H)+'                          ! mass of Higgs boson in GeV\n' )
      f.write( 'hwidth '+str(SMParams.width_H)+'                        ! width of Higgs boson in GeV\n' )

###############################################################################
#
#  Higgs v2 decorator
#
###############################################################################
class HiggsV2Decorator :
  def __init__( self, decorated ) :
    decorate( decorated, 'Higgs' )
    decorate( decorated, 'v2' )

    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.mass_H_low    = 10.
    self.decorated.mass_H_high   = 1000.
    self.decorated.runningscales = -1

    # Override default minlo setting
    self.decorated.minlo         = 1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'max_h_mass '+str(self.decorated.mass_H_high)+'      ! M H < mass high\n' )
      f.write( 'min_h_mass '+str(self.decorated.mass_H_low)+'       ! M H > mass low\n' )
      f.write( 'runningscales '+str(self.decorated.runningscales)+' ! (default 0), if 0 use hmass as central factorization and renormalisation scale; if 1 use the Ht/2\n' )

###############################################################################
#
#  Higgs+jets decorator
#
###############################################################################
class HJDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.bwcutoff    = 15
    self.decorated.ckkwscalup  = 0
    self.decorated.factsc2min  = -1
    self.decorated.frensc2min  = -1
    self.decorated.minlo_nnll  = 1
    self.decorated.sudscalevar = 1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'bwcutoff '+str(self.decorated.bwcutoff)+'       ! Mass window is hmass +- bwcutoff*hwidth\n' )
      f.write( 'ckkwscalup '+str(self.decorated.ckkwscalup)+'   ! (default 0) 0:do not use the CKKW scalup, use the normal Powheg one;  1:compute the scalup scale for subsequent shower using the smallest kt in the final state;\n' )
      f.write( 'factsc2min '+str(self.decorated.factsc2min)+'   ! at this value the factorization scale is frozen (needed with minlo)\n' )
      f.write( 'frensc2min '+str(self.decorated.frensc2min)+'   ! at this value the renormalisation scale is frozen (neede with minlo)\n' )
      f.write( 'minlo_nnll '+str(self.decorated.minlo_nnll)+'   ! \n' )
      f.write( 'sudscalevar '+str(self.decorated.sudscalevar)+' ! (default 1) scale variation also in Sudakov form factors in minlo\n' )

###############################################################################
#
#  H+V decorator
#
###############################################################################
class HVDecorator :
  def __init__( self, decorated ) :
    decorate( decorated, 'vector boson decay' )

    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.bornsuppfactV = -1
    self.decorated.ptVhigh       = -1
    self.decorated.ptVlow        = -1
    self.decorated.Vstep         = -1

    self.decorated.use_massive_t = True

  def append_to_run_card( self ) :
    self.massivetop = 1 if self.decorated.use_massive_t else -1
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'bornsuppfactV '+str(self.decorated.bornsuppfactV)+' ! \n' )
      f.write( 'doublefst 1                                         ! \n' )
      f.write( 'massivetop '+str(self.massivetop)+'                 ! \n' )
      f.write( 'nohad 0                                             ! no hadronization and U.E. in pythia\n' )
      f.write( 'ptVhigh '+str(self.decorated.ptVhigh)+'             ! \n' )
      f.write( 'ptVlow '+str(self.decorated.ptVlow)+'               ! \n' )
      f.write( 'Vstep '+str(self.decorated.Vstep)+'                 ! \n' )

###############################################################################
#
#  On-shell decorator
#
###############################################################################
class OnShellDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.zerowidth = 0

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'zerowidth '+str(self.decorated.zerowidth)+' ! (default 0) if 1 use on-shell bosons only \n' )

###############################################################################
#
#  Single vector boson decorator
#
###############################################################################
class SingleBosonDecorator :
  def __init__( self, decorated ) :
    decorate( decorated, 'vector boson decay' )
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.masswindow_low  = -1
    self.decorated.masswindow_high = -1
    self.decorated.mass_low  = -1
    self.decorated.mass_high = -1
    self.decorated.running_width  = -1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'alphaem '+str(SMParams.alphaem)+'                       ! EM coupling\n' )
      f.write( 'sthw2 '+str(SMParams.sthw2)+'                           ! sin**2 theta w\n' )
      f.write( 'Wmass '+str(SMParams.mass_W)+'                          ! W mass in GeV\n' )
      f.write( 'Wwidth '+str(SMParams.width_W)+'                        ! W width in GeV\n')
      f.write( 'Zmass '+str(SMParams.mass_Z)+'                          ! Z mass in GeV\n' )
      f.write( 'Zwidth '+str(SMParams.width_Z)+'                        ! Z width in GeV\n' )
      f.write( 'masswindow_low '+str(self.decorated.masswindow_low)+'   ! M_V > mass low\n' )
      f.write( 'masswindow_high '+str(self.decorated.masswindow_high)+' ! M_V < mass high\n' )
      f.write( 'mass_low '+str(self.decorated.masswindow_low)+'         ! M_V > mass low\n' )
      f.write( 'mass_high '+str(self.decorated.masswindow_high)+'       ! M_V < mass high\n' )
      f.write( 'running_width '+str(self.decorated.running_width)+'     ! \n' )

###############################################################################
#
#  Top decorator
#
###############################################################################
class TopDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.topdecaymode     = 10000
    self.decorated.tdec_elbranching = SMParams.tdec_elbranching

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'topdecaymode '+str(self.decorated.topdecaymode)+'         ! an integer of 5 digits that are either 0, 1 or 2, representing\n' )
      f.write( '                                                          !   the maximum number of the following particles(antiparticles)\n' )
      f.write( '                                                          !   in the final state: e mu tau up charm\n' )
      f.write( 'tdec/elbranching '+str(self.decorated.tdec_elbranching)+' ! W electronic branching fraction\n' )
      f.write( 'tdec/emass '+str(SMParams.mass_e)+'             ! electron mass\n' )
      f.write( 'tdec/mumass '+str(SMParams.mass_mu)+'           ! mu mass\n' )
      f.write( 'tdec/taumass '+str(SMParams.mass_tau)+'         ! tau mass\n' )
      f.write( 'tdec/wmass '+str(SMParams.mass_W)+'             ! W mass for top decay\n' )
      f.write( 'tdec/wwidth '+str(SMParams.width_W)+'           ! W width\n' )

###############################################################################
#
#  Powheg v2 decorator
#
###############################################################################
class V2Decorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.itmx1rm       = -1
    self.decorated.itmx2rm       = -1
    self.decorated.minlo         = -1
    self.decorated.ncall1rm      = -1
    self.decorated.ncall2rm      = -1
    self.decorated.olddij        = -1
    self.decorated.parallelstage = -1
    self.decorated.stage2init    = -1

    self.decorated.doublefsr     = 1
    self.decorated.evenmaxrat    = -1
    self.decorated.fastbtlbound  = 1
    self.decorated.storemintupb  = 1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      # Specialisation of baseclass PDF switch
      if self.decorated.PDF_info_type == 1 :
        f.write( 'storeinfo_rwgt 1                                  ! enable new-style PDF information: nominal\n' )
        f.write( 'compute_rwgt 0                                    ! enable new-style PDF information: nominal\n' )
      elif self.decorated.PDF_info_type == 2 :
        f.write( 'storeinfo_rwgt 0                                  ! enable new-style PDF information: reweight to new PDF\n' )
        f.write( 'compute_rwgt 1                                    ! enable new-style PDF information: reweight to new PDF\n' )
      f.write( 'doublefsr '+str(self.decorated.doublefsr)+'         ! fix problem with spikes in final observables, see arXiv:1303.3922\n' )
      f.write( 'evenmaxrat '+str(self.decorated.evenmaxrat)+'       ! \n')
      # factsc2min 2       ! at this value the factorization scale is frozen (neede with minlo)
      f.write( 'fastbtlbound '+str(self.decorated.fastbtlbound)+'   ! (default 0) if 1 use fast btilde bound\n')
      f.write( 'itmx1rm '+str(self.decorated.itmx1rm)+'             ! \n')
      f.write( 'itmx2rm '+str(self.decorated.itmx2rm)+'             ! \n')
      f.write( 'LOevents -1                                         ! with LOevents==1 bornonly must be set equal to 1\n' )
      f.write( 'minlo '+str(self.decorated.minlo)+'                 ! (default 0) set to 1 to use minlo\n' )
      f.write( 'ncall1rm '+str(self.decorated.ncall1rm)+'           ! \n')
      f.write( 'ncall2rm '+str(self.decorated.ncall2rm)+'           ! \n')
      f.write( 'olddij '+str(self.decorated.olddij)+'               ! \n')
      f.write( 'parallelstage '+str(self.decorated.parallelstage)+' ! \n')
      f.write( 'stage2init '+str(self.decorated.stage2init)+'       ! \n')
      f.write( 'storemintupb '+str(self.decorated.storemintupb)+'   ! \n')

###############################################################################
#
#  Vector boson decay decorator
#
###############################################################################
class VectorBosonDecayDecorator :
  def __init__( self, decorated ) :
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.vdecaymode = 1

  def append_to_run_card( self ) :
    with open( str(self.decorated.TestArea)+'/powheg.input', 'a' ) as f :
      f.write( 'vdecaymode '+str(self.decorated.vdecaymode)+' ! (1:leptonic decay, 2:muonic decay, 3: tauonic decay,...)\n' )
