# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to configure Powheg subprocesses
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#            Daniel Hayden   <danhayden0@googlemail.com>
#            Stephen Bieniek <stephen.paul.bieniek@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import os,time,subprocess,threading #,shutil sys,
from AthenaCommon import Logging

class RepeatingTimer(threading._Timer):
  '''To allow timing output in an additional thread. From http://stackoverflow.com/questions/5429577/'''
  def run(self):
    while True:
      self.finished.wait(self.interval)
      if self.finished.is_set():
        return
      else:
        self.function(*self.args, **self.kwargs)

class PowhegConfig_base() :
  __logger = Logging.logging.getLogger('PowhegControl')
  # This needs to be set so that Generate_trf finds an appropriate file format for showering
  # It should not be changed by end-users!
  __output_events_file_name = 'PowhegOTF._1.events'
  # This must be defined by each derived class - don't change it in the jobOptions!
  _powheg_executable = os.environ['POWHEGPATH']

  # These values can be changed in the jobOptions - defaults are set here.
  # Process-specific options are in the relevant class
  runArgs        = None
  nEvents        = 5200
  energy         = 4000
  randomSeed     = 1
  mu_F, mu_R     = 1.0, 1.0
  PDF            = 10800 # (CT10) - Powheg can set each proton with a different PDF, here we impose the same value for each one

  # Enable PDF information by default
  # 0 - disables PDF information
  # 1 - produces nominal sample
  # 2 - produces reweighted sample
  PDF_info_type = 1

  # Weighted-event generation is disabled by default
  withnegweights = 0
  bornsuppfact   = None
  bornktmin      = None

  # Parameters to allow/disallow use of stored data
  use_old_grid, use_old_ubound = 1, 1

  # Integration parameters (reasonable defaults)
  # These are optimised in each process to ensure:
  #   Upper bound failures      < 1% : to reduce, increase nubound/xupbound, ncall2*itmx2
  #   Cross section uncertainty < 1% : to reduce, increase ncall1*itmx1 / ncall2*itmx2
  #   Negative weight events    < 1% : to reduce, increase fold parameters
  ncall1, ncall2        = 20000, 100000
  nubound, xupbound     = 100000, 2
  itmx1, itmx2          = 5, 5
  ixmax, iymax          = 1, 1
  foldx, foldy, foldphi = 1, 1, 1 # allowed values for folding parameters are 1, 2, 5, 10, 25, 50

  # Optional parameters
  ptsqmin                 = 0.8
  charmthr, charmthrpdf   = 1.5, 1.5
  bottomthr, bottomthrpdf = 5.0, 5.0
  # These may be needed in presence of processes where the Born cross section vanishes in some phase-space region
  withdamp                = 0
  hfact                   = -1
  # Users are asked not to change these, since their invocation is time consuming and/or may cause some conflicts with other settings.
  testsuda                = 0
  testplots               = 0
  bornonly                = 0
  smartsig                = 1
  withsubtr               = 1
  radregion               = -1
  iupperisr, iupperfsr    = 1, 2
  # Vector-boson parameters
  mass_low, mass_high = None, None
  hdamp               = -1
  zerowidth           = 0
  runningscale        = 1
  running_width       = 1
  mllmin              = 4
  tevscale            = None
  # SM parameters
  mass_e      = 0.00051
  mass_mu     = 0.1057
  mass_tau    = 1.777
  mass_d      = 0.320
  mass_u      = 0.320
  mass_s      = 0.5
  mass_c      = 1.55
  mass_b      = 4.95
  mass_t      = 172.5
  mass_W      = 80.399
  mass_Z      = 91.1876
  mass_H      = 125
  width_t     = 1.3200
  width_W     = 2.085
  width_Z     = 2.4952
  width_H     = 0.003605
  sthw2       = 0.23113
  alphaem     = 0.00781653
  sin2cabibbo = 0.051
  CKM_Vud     = 0.97428
  CKM_Vus     = 0.2253
  CKM_Vub     = 0.00347
  CKM_Vcd     = 0.2252
  CKM_Vcs     = 0.97345
  CKM_Vcb     = 0.0410
  CKM_Vtd     = 0.00862
  CKM_Vts     = 0.0403
  CKM_Vtb     = 0.999152
  G_F         = 0.00001166397


  # LHEF filter and directory setting
  filter_name, filter_args = '', ''
  TestArea = os.environ['PWD']

  def __init__( self, runArgs=None ) :
    self.runArgs = runArgs
    if runArgs == None :
      self.__logger.warning( 'No run arguments found! Using defaults.' )
    else :
      # Read values from runArgs
      if hasattr(self.runArgs,'ecmEnergy') :
        self.energy = 0.5 * self.runArgs.ecmEnergy
      if hasattr(self.runArgs,'maxEvents') :
        if self.runArgs.maxEvents > 0 :
          self.nEvents = self.runArgs.maxEvents
      if hasattr(self.runArgs,'randomSeed') :
        self.randomSeed = self.runArgs.randomSeed
      # Set inputGeneratorFile to match output events file. Otherwise Generate_trf check will fail.
      self.runArgs.inputGeneratorFile = self.__output_events_file_name

  def human_readable_time_interval( self, seconds_elapsed ) :
    h = int( seconds_elapsed / (60 * 60) )
    m = int( (seconds_elapsed % (60 * 60)) / 60 )
    s = int( seconds_elapsed % 60. )
    return "{0}h {1:>02}m {2:>02}s".format(h, m, s)

  # All runcard options which are generic here
  def generateRunCardSharedOptions( self ) :
    self.__logger.info( 'Writing Powheg runcard to {0}/powheg.input'.format(self.TestArea) )

    # Perform some sanity checks
    if self.withnegweights != 0 and self.foldx * self.foldy * self.foldphi != 1 :
      # Performing a folded integration reduces the proportion of negative weights
      # Not needed if using (accepting) negative weights
      self.foldx, self.foldy, self.foldphi = 1, 1, 1
      self.__logger.warning( 'Requesting negative weights while using folding parameters! This is suboptimal: setting folding parameters to foldx({0}) foldy({1}) foldphi({2})'.format(self.foldx,self.foldy,self.foldphi) )
    if (self.bornsuppfact is not None and self.bornsuppfact != -1.0) and (self.bornktmin is None or self.bornktmin == 0.0) :
      self.__logger.warning( 'These settings: bornsuppfact({0}) and bornktmin({1}) cannot be used to generate events! Only a fixed-order distribution will be produced!'.format(self.bornsuppfact,self.bornktmin) )

    # Write output to runcard
    with open( str(self.TestArea)+'/powheg.input','w') as f :
      # Beam/event information
      f.write( 'numevts '+str(self.nEvents)+'                ! number of events to be generated\n' )
      f.write( 'ih1 1                                        ! hadron 1 type\n' )
      f.write( 'ih2 1                                        ! hadron 2 type\n' )
      f.write( 'lhans1 '+str(self.PDF)+'                     ! PDF set for hadron 1 (LHAGLUE numbering)\n' )
      f.write( 'lhans2 '+str(self.PDF)+'                     ! PDF set for hadron 2 (LHAGLUE numbering)\n' )
      f.write( 'ebeam1 '+str(self.energy)+'                  ! energy of beam 1 in GeV\n' )
      f.write( 'ebeam2 '+str(self.energy)+'                  ! energy of beam 2 in GeV\n' )
      f.write( 'iseed '+str(self.randomSeed)+'               ! Start the random number generator with seed iseed\n' )
      # Generation setup
      f.write( 'withnegweights '+str(self.withnegweights)+'  ! allow negative weights\n' )
      f.write( 'facscfact '+str(self.mu_F)+'                 ! factorization scale factor: mufact=muref*facscfact\n' )
      f.write( 'renscfact '+str(self.mu_R)+'                 ! renormalization scale factor: muren=muref*renscfact\n' )
      # PDF reweighting information
      if self.PDF_info_type != 0 :
        f.write( 'pdfreweight 1                              ! store PDF information\n' )
        if self.PDF_info_type == 1 :
          f.write( 'storeinfo_rwgt 1                         ! enable new-style PDF information: nominal\n' )
          f.write( 'compute_rwgt 0                           ! enable new-style PDF information: nominal\n' )
        elif self.PDF_info_type == 2 :
          f.write( 'storeinfo_rwgt 0                         ! enable new-style PDF information: reweight to new PDF\n' )
          f.write( 'compute_rwgt 1                           ! enable new-style PDF information: reweight to new PDF\n' )
      # Parameters to allow/disallow use of stored data
      f.write( 'use-old-grid '+str(self.use_old_grid)+'      ! If 1 use old grid if file pwggrids.dat is present (<> 1 regenerate)\n' )
      f.write( 'use-old-ubound '+str(self.use_old_ubound)+'  ! If 1 use norm of upper bounding function stored in pwgubound.dat, if present; <> 1 regenerate\n' )
      # Integration parameters: A typical call uses 1/1400 seconds (1400 calls per second)
      #   NB. the total number of calls is ncall2*itmx2*foldcsi*foldy*foldphi
      #       with the default setting of ncall2*itmx2*foldcsi*foldy*foldphi=5M, 60 minutes
      #       these folding numbers yield a negative fraction of 0.5% with bornktmin=10 GeV
      f.write( 'ncall1 '+str(self.ncall1)+'                  ! number of calls for initializing the integration grid\n' )
      f.write( 'itmx1 '+str(self.itmx1)+'                    ! number of iterations for initializing the integration grid: total 100000 calls ~ 70 seconds\n' )
      f.write( 'ncall2 '+str(self.ncall2)+'                  ! number of calls for computing the integral and finding upper bound\n' )
      f.write( 'itmx2 '+str(self.itmx2)+'                    ! number of iterations for the above\n' )
      f.write( 'foldx '+str(self.foldx)+'                    ! number of folds on x integration\n' )
      f.write( 'foldcsi '+str(self.foldx)+'                  ! (alternate naming in some processes)\n' )
      f.write( 'foldy '+str(self.foldy)+'                    ! number of folds on y integration\n' )
      f.write( 'foldphi '+str(self.foldphi)+'                ! number of folds on phi integration\n' )
      f.write( 'nubound '+str(self.nubound)+'                ! number of calls to setup upper bounds for radiation\n' )
      f.write( 'xupbound '+str(self.xupbound)+'              ! increase upper bound for radiation generation by this factor\n' )
      f.write( 'ixmax '+str(self.ixmax)+'                    ! <= 10, number of intervals in x grid to compute upper bounds\n' )
      f.write( 'icsimax '+str(self.ixmax)+'                  ! (alternate naming in some processes)\n' )
      f.write( 'iymax '+str(self.iymax)+'                    ! <= 10, number of intervals in y grid to compute upper bounds\n' )
      # Other parameters
      f.write( 'ptsqmin '+str(self.ptsqmin)+'                ! (default 0.8 GeV) minimum pt for generation of radiation\n' )
      f.write( 'charmthr '+str(self.charmthr)+'              ! (default 1.5 GeV) charm threshold for gluon splitting\n' )
      f.write( 'bottomthr '+str(self.bottomthr)+'            ! (default 5.0 GeV) bottom threshold for gluon splitting\n' )
      f.write( 'charmthrpdf '+str(self.charmthrpdf)+'        ! (default 1.5 GeV) PDF charm threshold\n' )
      f.write( 'bottomthrpdf '+str(self.bottomthrpdf)+'      ! (default 5.0 GeV) PDF bottom threshold\n' )
      # These may be needed in presence of processes where the Born cross section vanishes in some phase-space region
      f.write( 'withdamp '+str(self.withdamp)+'              ! (default 0, do not use) use Born-zero damping factor\n' )
      f.write( 'hfact '+str(self.hfact)+'                    ! (default no dumping factor) dump factor for high-pt radiation: > 0 dumpfac=h**2/(pt2+h**2)\n' )
      # Users are asked not to change these, since their invocation is time consuming and/or may cause some conflicts with other settings.
      f.write( 'testsuda '+str(self.testsuda)+'              ! (default 0, do not test) test Sudakov form factor\n' )
      f.write( 'testplots '+str(self.testplots)+'            ! (default 0, do not) do NLO and PWHG distributions\n' )
      f.write( 'bornonly '+str(self.bornonly)+'              ! (default 0) if 1 do Born only\n' )
      f.write( 'smartsig '+str(self.smartsig)+'              ! (default 1) remember equal amplitudes (0 do not remember)\n' )
      f.write( 'withsubtr '+str(self.withsubtr)+'            ! (default 1) subtract real counterterms (0 do not subtract)\n' )
      f.write( 'radregion '+str(self.radregion)+'            ! (default all regions) only generate radiation in the selected singular region\n' )
      f.write( 'iupperisr '+str(self.iupperisr)+'            ! (default 1) choice of ISR upper bounding functional form\n' )
      f.write( 'iupperfsr  '+str(self.iupperfsr )+'          ! (default 2) choice of FSR upper bounding functional form\n' )
      # SM parameters
      f.write( 'Zmass '+str(self.mass_Z)+'                   ! Z mass in GeV\n' )
      f.write( 'Zwidth '+str(self.width_Z)+'                 ! Z width in GeV\n' )
      f.write( 'sthw2 '+str(self.sthw2)+'                    ! sin**2 theta w\n' )
      f.write( 'alphaem '+str(self.alphaem)+'                ! em coupling\n' )
      f.write( 'Wmass '+str(self.mass_W)+'                   ! W mass in GeV\n' )
      f.write( 'Wwidth '+str(self.width_W)+'                 ! W width in GeV\n ')
      f.write( 'CKM_Vud '+str(self.CKM_Vud)+'                ! CKM element \n' )
      f.write( 'CKM_Vus '+str(self.CKM_Vus)+'                ! CKM element \n' )
      f.write( 'CKM_Vub '+str(self.CKM_Vub)+'                ! CKM element \n' )
      f.write( 'CKM_Vcd '+str(self.CKM_Vcd)+'                ! CKM element \n' )
      f.write( 'CKM_Vcs '+str(self.CKM_Vcs)+'                ! CKM element \n' )
      f.write( 'CKM_Vcb '+str(self.CKM_Vcb)+'                ! CKM element \n' )
      f.write( 'CKM_Vtd '+str(self.CKM_Vtd)+'                ! CKM element \n' )
      f.write( 'CKM_Vts '+str(self.CKM_Vts)+'                ! CKM element \n' )
      f.write( 'CKM_Vtb '+str(self.CKM_Vtb)+'                ! CKM element \n' )
      # Vector boson process parameters
      f.write( 'runningscale '+str(self.runningscale)+'      ! choice for ren and fac scales in Bbar integration\n' )
      f.write( '                                             ! (0: fixed scale M Z, 1: running scale inv mass Z, 2: running scale transverse mass Z\n' )
      f.write( 'hdamp '+str(self.hdamp)+'                    ! Born-zero damping factor \n' )
      f.write( 'running_width '+str(self.running_width)+'    ! \n' )
      f.write( 'mllmin '+str(self.mllmin)+'                  ! Minimum invariant mass of lepton pairs from Z decay\n' )
      f.write( 'zerowidth '+str(self.zerowidth)+'            ! if 1 (true) use zero width approximatrion (default 0)\n' )
      # Optional parameters
      if self.bornsuppfact is not None  :
        f.write( 'bornsuppfact  '+str(self.bornsuppfact)+'   ! (default 0d0) Mass parameter for Born suppression factor. If < 0 suppfact = 1\n' )
      if self.bornktmin is not None :
        f.write( 'bornktmin '+str(self.bornktmin)+'          ! (default 0d0) Generation cut: minimum kt in underlying Born\n' )
      if self.mass_low is not None :
        f.write( 'mass_low '+str(self.mass_low)+'            ! M V > mass low\n ')
      if self.mass_high is not None :
        f.write( 'mass_high '+str(self.mass_high)+'          ! M V < mass high\n' )
      if self.tevscale is not None :
        f.write( 'tevscale '+str(self.tevscale)+'            ! Form-factor scale, in TeV\n' )


  def generateEvents(self) :
    self.__logger.info( 'Starting Powheg LHEF event generation at '+str(time.asctime()) )

    # Remove any existing .lhe files to avoid repeated events
    if self.PDF_info_type == 2 :
      self.__logger.warning( 'Not attempting to remove existing LHE files due to reweighting settings! This could cause problems.' )
    else :
      try :
        os.remove( '*.lhe' )
      except OSError :
        pass

    # Generate the events in the external Powheg process
    time_start = time.time()
    self.__logger.info( 'Running ' + str(self._powheg_executable) )
    # generate = subprocess.Popen( [self._powheg_executable,''] )
    # generate.wait()

    # Setup heartbeat thread
    heartbeat = RepeatingTimer( 600., lambda: self.__logger.info('Heartbeat: Powheg generation has been running for {0} in total'.format( self.human_readable_time_interval(time.time() - time_start)) ) )
    heartbeat.daemon = True # Allows program to exit if only this thread is alive
    heartbeat.start()

    # Run generation until finished then kill heartbeat thread
    generate = subprocess.Popen( [self._powheg_executable,''], stdout=subprocess.PIPE, stderr=subprocess.STDOUT )
    for output_line in iter( generate.stdout.readline, b'' ) :
      self.__logger.info( output_line.rstrip() )
    generate.stdout.close()
    heartbeat.cancel()

    elapsed_time = time.time() - time_start
    self.__logger.info( 'Running Powheg took {0} for {1} events => {2:4.1f} Hz'.format( self.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time ) )

    # Move output to correctly named file
    os.rename( 'pwgevents.lhe', self.__output_events_file_name )

    # Finish
    self.__logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


  def generateFilteredEvents(self) :
    self.__logger.info( 'Starting Powheg LHEF event generation at {0}'.format( time.asctime() ) )
    filter_executable = os.environ['LHEFPATH']
    filter_executable += '/'+self.filter_name
    filter_args.insert( 0, self.__output_events_file_name )
    filter_args.insert( 0, 'pwgevents.lhe' )
    filter_args.insert( 0, str(filter_executable) )

    # Remove any existing .lhe files to avoid repeated events
    if self.PDF_info_type == 2 :
      self.__logger.warning( 'Not attempting to remove existing LHE files due to reweighting settings! This could cause problems.' )
    else :
      try :
        os.remove( '*.lhe' )
      except OSError :
        pass

    # Generate the events
    self.__logger.info( 'Running ' + str(self._powheg_executable) )

    # Start fifo
    setuppipe = subprocess.Popen( ['mkfifo','pwgevents.lhe'] )
    setuppipe.wait()

    # Start generation process
    generate = subprocess.Popen( [self._powheg_executable,''] )

    # Start filter and wait until quota has been filled
    # LHEF_filter moves output to correctly named file
    self.__logger.info( 'Running filter with commmand: {0}'.format( filter_executable ) )
    LHEF_filter = subprocess.Popen( self.filter_args )
    LHEF_filter.wait()
    outputline = LHEF_filter.communicate()
    self.__logger.info( outputline )

    # Once quota filled, kill generation process
    generate.terminate()

    # Finish
    self.__logger.info( 'Finished at {0}'.format( time.asctime() ) )
    return


###############################################################################
#
#  bb
#
###############################################################################
class PowhegConfig_bb(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  bbscalevar = 1
  bornktmin = 5.0
  fixspikes = True

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/hvq/pwhg_main'

    # Set optimised integration parameters
    self.foldx, self.foldy, self.foldphi = 5, 5, 2


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'qmass '+str(self.mass_b)+'           ! mass of heavy quark in GeV\n' )
      f.write( 'bbscalevar '+str(self.bbscalevar)+'  ! use variable re. and fct. scales\n' )
      if self.fixspikes == True :
        f.write( 'doublefsr 1                        ! fix problem with spikes in final observables\n' )
        f.write( 'par_diexp 4                        ! recommended additional options to be used with doublefsr\n' )
        f.write( 'par_dijexp 4                       ! (by Paolo Nason, private communication)\n' )
        f.write( 'par_2gsupp 4                       ! not sure of what happens if these are omitted!\n' )


###############################################################################
#
#  Dijet
#
###############################################################################
class PowhegConfig_Dijet(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  fixspikes = True
  bornktmin = 5.0

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/Dijet/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2 = 20000, 10000
    self.nubound, self.xupbound = 30000, 3
    self.foldx, self.foldy, self.foldphi = 10, 10, 10

  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      if self.fixspikes == True :
        f.write( 'doublefsr 1                        ! fix problem with spikes in final observables\n' )
        f.write( 'par_diexp 4                        ! recommended additional options to be used with doublefsr\n' )
        f.write( 'par_dijexp 4                       ! (by Paolo Nason, private communication)\n' )
        f.write( 'par_2gsupp 4                       ! not sure of what happens if these are omitted!\n' )


###############################################################################
#
#  ggH
#
###############################################################################
class PowhegConfig_ggH(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  model = 0
  massren = 0
  ew  = 0
  hdecaymode = 0
  use_massive_b = False
  use_massive_c = False
  hdecay = 0

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/gg_H_quark-mass-effects/pwhg_main'


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'model '+str(self.model)+'           ! 0 = SM\n' )
      f.write( 'massren '+str(self.massren)+'       ! 0 = OS, 1 = MSBAR, 2 = DRBAR\n' )
      f.write( 'gfermi '+str(self.G_F)+'            ! Fermi constant\n' )
      f.write( 'hmass '+str(self.mass_H)+'          ! mass of Higgs boson in GeV\n' )
      f.write( 'hwidth '+str(self.width_H)+'        ! width of Higgs boson in GeV\n' )
      f.write( 'topmass '+str(self.mass_t)+'        ! top quark mass in GeV\n' )
      f.write( 'ew '+str(self.ew)+'                 ! If 1 enable EW corrections\n' )
      f.write( 'hdecaymode '+str(self.hdecaymode)+' ! Higgs boson decay (-1:stable; 0:all; 1-12:single channel)\n' )
      if self.use_massive_b :
        f.write( 'bottomass '+str(self.mass_b)+'    ! bottom quark mass in GeV (enabled if defined)\n' )
      if self.use_massive_c :
        f.write( 'charmass '+str(self.mass_c)+'     ! charm quark mass in GeV (enabled if defined)\n' )
      f.write( 'hdecay '+str(self.hdecay)+'         ! If 1 read the total decay width from br.sm2 produced by HDECAY\n' )


###############################################################################
#
#  trijet
#
###############################################################################
class PowhegConfig_trijet(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  bornktmin = 5.0

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/trijet/pwhg_main'


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()


###############################################################################
#
#  tt
#
###############################################################################
class PowhegConfig_tt(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  topdecaymode     = 22222
  tdec_elbranching = 0.108

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/hvq/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2, self.nubound = 1000000, 1000000, 1000000
    self.itmx1, self.itmx2, self.xupbound = 4, 6, 4


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'qmass '+str(self.mass_t)+'                      ! mass of heavy quark in GeV\n' )
      f.write( 'topdecaymode '+str(self.topdecaymode)+'         ! an integer of 5 digits that are either 0, or 2, representing in\n' )
      f.write( '                                                !   the order the maximum number of the following particles(antiparticles)\n' )
      f.write( '                                                !   in the final state: e mu tau up charm\n' )
      f.write( '                                                !   22222    All decays (up to 2 units of everything)\n' )
      f.write( '                                                !   20000    both top go into b l nu (with the appropriate signs)\n' )
      f.write( '                                                !   10011    one top goes into electron (or positron), the other into (any) hadron\n' )
      f.write( '                                                !            or one top goes into charm, the other into up\n' )
      f.write( '                                                !   00022    Fully hadronic\n' )
      f.write( '                                                !   00002    Fully hadronic with two charms\n' )
      f.write( '                                                !   00011    Fully hadronic with a single charm\n' )
      f.write( '                                                !   00012    Fully hadronic with at least one charm\n' )
      f.write( 'tdec/wmass '+str(self.mass_W)+'                 ! W mass for top decay\n' )
      f.write( 'tdec/wwidth '+str(self.width_W)+'               ! W width\n' )
      f.write( 'tdec/bmass '+str(self.mass_b)+'                 ! b quark mass in t decay\n' )
      f.write( 'tdec/twidth '+str(self.width_t)+'               ! top width\n' )
      f.write( 'tdec/elbranching '+str(self.tdec_elbranching)+' ! W electronic branching fraction\n' )
      f.write( 'tdec/emass '+str(self.mass_e)+'                 ! electron mass\n' )
      f.write( 'tdec/mumass '+str(self.mass_mu)+'               ! mu mass\n' )
      f.write( 'tdec/taumass '+str(self.mass_tau)+'             ! tau mass\n' )
      f.write( 'tdec/dmass '+str(self.mass_d)+'                 ! d mass\n' )
      f.write( 'tdec/umass '+str(self.mass_u)+'                 ! u mass\n' )
      f.write( 'tdec/smass '+str(self.mass_s)+'                 ! s mass\n' )
      f.write( 'tdec/cmass '+str(self.mass_c)+'                 ! c mass\n' )
      f.write( 'tdec/sin2cabibbo '+str(self.sin2cabibbo)+'      ! sine of Cabibbo angle squared\n' )

###############################################################################
#
#  W
#
###############################################################################
class PowhegConfig_W(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  idvecbos = 24
  vdecaymode = 1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/W/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2, self.nubound = 120000, 250000, 20000

    # Override defaults
    self.withsubtr = 1
    self.withnegweights = 1
    self.mass_low  = 2.5
    self.mass_high = 2.0 * self.energy


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'idvecbos '+str(self.idvecbos)+'               ! PDG code for vector boson to be produced (W:24)\n' )
      f.write( 'vdecaymode ' +str(self.vdecaymode)+'          ! PDG code for first decay product of the vector boson\n' )
      f.write( '                                              !   (11:e-; 12:ve; 13:mu-; 14:vmu; 15:tau; 16:vtau)\n' )


###############################################################################
#
#  Wj
#
###############################################################################
class PowhegConfig_Wj(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  idvecbos = 24
  vdecaymode = 1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/Wj/pwhg_main'


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'idvecbos '+str(self.idvecbos)+'         ! PDG code for vector boson to be produced (W:24)\n' )
      f.write( 'vdecaymode ' +str(self.vdecaymode)+'    ! PDG code for first decay product of the vector boson\n' )
      f.write( '                                        !   (11:e-; 12:ve; 13:mu-; 14:vmu; 15:tau; 16:vtau)\n' )


###############################################################################
#
#  WW
#
###############################################################################
class PowhegConfig_WW(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymodeWp = -11
  vdecaymodeWm = 13
  dronly = 0
  delg1_g = 0
  delg1_z = 0
  lambda_g = 0
  lambda_z = 0
  delk_g = 0
  delk_z = 0
  wllmin = None

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/WW/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2, self.nubound = 300000, 1000000, 500000
    self.itmx1, self.itmx2 = 3, 5


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'vdecaymodeWp '+str(self.vdecaymodeWp)+' ! PDG code for charged decay product of the vector boson (1:dbar; 3: sbar; 7: dbar or sbar; -11:e+; -13:mu+; -15:tau+)\n' )
      f.write( 'vdecaymodeWm '+str(self.vdecaymodeWm)+' ! PDG code for charged decay product of the vector boson (1:d; 3: s; 7: d or s; 11:e-; 13:mu-; 15:tau-)\n' )
      f.write( 'dronly '+str(self.dronly)+'             ! if 1 (true) only include double resonant diagrams (default 0)\n' )
      # Anomalous couplings
      f.write( 'delg1_g '+str(self.delg1_g)+'           ! Delta_g1(Gamma)\n' )
      f.write( 'delg1_z '+str(self.delg1_z)+'           ! Delta_g1(Z)\n' )
      f.write( 'lambda_g '+str(self.lambda_g)+'         ! Lambda(gamma)\n' )
      f.write( 'lambda_z '+str(self.lambda_z)+'         ! Lambda(Z)\n' )
      f.write( 'delk_g '+str(self.delk_g)+'             ! Delta_K(Gamma)\n' )
      f.write( 'delk_z '+str(self.delk_z)+'             ! Delta_K(Z)\n' )
      if self.wllmin is not None :
        f.write( 'wllmin '+str(self.wllmin)+'           ! default 1d-5 GeV this is min invar mass for W leptons\n' )


###############################################################################
#
#  WZ
#
###############################################################################
class PowhegConfig_WZ(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymodeW = -11
  vdecaymodeZ = 13
  withinterference = 1
  dronly = 0
  diagCKM = 0
  delg1_g = 0
  delg1_z = 0
  lambda_g = 0
  lambda_z = 0
  delk_g = 0
  delk_z = 0
  wllmin = None

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/WZ/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2, self.nubound = 1500000, 1000000, 3000000
    self.itmx1, self.itmx2, self.xupbound = 4, 5, 6

    # Override defaults
    self.mllmin = 10


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'vdecaymodeW '+str(self.vdecaymodeW)+'           ! PDG code for charged decay product of the W boson (11:e-;-11:e+;...)\n' )
      f.write( 'vdecaymodeZ '+str(self.vdecaymodeZ)+'           ! PDG code for lepton Z boson (11:e-; 12: ve; ...)\n' )
      f.write( 'withinterference '+str(self.withinterference)+' ! if 1 (true) include interference for like flavour charged leptons\n' )
      f.write( 'dronly '+str(self.dronly)+'                     ! if 1 (true) only include double resonant diagrams (default 0)\n' )
      # CKM matrix
      f.write( 'diagCKM '+str(self.diagCKM)+'                   ! if 1 (true) then diagonal CKM (default 0)\n' )
      # Anomalous couplings
      f.write( 'delg1_g '+str(self.delg1_g)+'                   ! Delta_g1(Gamma)\n' )
      f.write( 'delg1_z '+str(self.delg1_z)+'                   ! Delta_g1(Z)\n' )
      f.write( 'lambda_g '+str(self.lambda_g)+'                 ! Lambda(gamma)\n' )
      f.write( 'lambda_z '+str(self.lambda_z)+'                 ! Lambda(Z)\n' )
      f.write( 'delk_g '+str(self.delk_g)+'                     ! Delta_K(Gamma)\n' )
      f.write( 'delk_z '+str(self.delk_z)+'                     ! Delta_K(Z)\n' )
      if self.wllmin is not None :
        f.write( 'wllmin '+str(self.wllmin)+'                   ! default 1d-5 GeV this is min invar mass for W leptons\n' )


###############################################################################
#
#  Z
#
###############################################################################
class PowhegConfig_Z(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymode = 1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/Z/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2, self.nubound = 120000, 250000, 20000

    # Override defaults
    self.withsubtr = 1
    self.withnegweights = 1
    self.mass_low  = 60.
    self.mass_high = 2.0 * self.energy


  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'vdecaymode '+str(self.vdecaymode)+'     ! (1:leptonic decay, 2:muonic decay, 3: tauonic decay,...)\n' )


###############################################################################
#
#  Zj
#
###############################################################################
class PowhegConfig_Zj(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymode = 1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/Zj/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2 = 100000, 100000
    self.nubound, self.xupbound = 20000, 3
    self.foldx, self.foldy, self.foldphi = 5, 5, 5

  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'vdecaymode '+str(self.vdecaymode)+'     ! Z decay products (default 2) : 1 for electronic, 2 for muonic and 3 for tauonic decay\n' )


###############################################################################
#
#  ZZ
#
###############################################################################
class PowhegConfig_ZZ(PowhegConfig_base) :
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymodeZ1 = 11
  vdecaymodeZ2 = 11

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None) :
    PowhegConfig_base.__init__(self,runArgs)
    self._powheg_executable += '/ZZ/pwhg_main'

    # Set optimised integration parameters
    self.ncall1, self.ncall2 = 100000, 120000
    self.nubound, self.xupbound = 60000, 3
    self.foldx, self.foldy, self.foldphi = 2, 2, 2

  def generateRunCard(self) :
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f :
      f.write( 'vdecaymodeZ1 '+str(self.vdecaymodeZ1)+' ! PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)\n' )
      f.write( 'vdecaymodeZ2 '+str(self.vdecaymodeZ2)+' ! PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)\n' )
