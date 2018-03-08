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
import os,sys,time,subprocess,shutil
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('PowhegUtils')

class PowhegConfig_base():
  # As this is a dummy file now it might as well be generic - don't change this
  outputEventsBaseName = 'PowhegOutput._1'
  outputEventsName = 'events.lhe' 
  # This must be defined by each derived class - don't change it in the jobOptions
  executablePath = os.environ['POWHEGPATH']
  # These can be changed in the jobOptions - default values are set here.
  # Put process-specific options in the appropriate class instead
  runArgs = None
  nEvents = 5200
  energy = 4000
  randomSeed = 1
  muF, muR = 1.0, 1.0
  pdf = 10800 #(CT10) - POWHEG can set each proton with a different PDF, for now sticking to using the same value for each one
  storePdfInfo, use_old_grid, use_old_ubound = 1, 1, 1
  ncall1, ncall2, nubound = 20000, 100000, 500000
  itmx1, itmx2 = 5, 5
  foldcsi, foldy, foldphi = 5, 10, 2
  withnegweights = 1
  filter, filterRunArgs = "", ""
  TestArea = os.environ['PWD']

  def __init__(self,runArgs=None):
    self.runArgs = runArgs
    if runArgs == None :
      mglog.warning('No run arguments found! Using defaults.')
    else :
      # Add inputGeneratorFile to runArgs
      # Read values from runArgs
      if hasattr(self.runArgs,'ecmEnergy'):
        self.energy = 0.5 * self.runArgs.ecmEnergy
      if hasattr(self.runArgs,'maxEvents'):
        if self.runArgs.maxEvents > 0 :
          self.nEvents = self.runArgs.maxEvents
      if hasattr(self.runArgs,'randomSeed'):
        self.randomSeed = self.runArgs.randomSeed
      if hasattr(runArgs,'inputGeneratorFile'):
        genFileList = self.runArgs.inputGeneratorFile.split(".")
        self.outputEventsName = genFileList[0]+"."+genFileList[1]+".events"
  
  # All runcard options which are generic here
  def generateRunCardSharedOptions(self):
    mglog.info("Writing POWHEG runcard to "+str(self.TestArea)+"/powheg.input")

    with open( str(self.TestArea)+'/powheg.input','w') as f:        
      f.write( "iseed "+str(self.randomSeed)+"      ! Start the random number generator with seed iseed\n" )
      f.write( "numevts "+str(self.nEvents)+"       ! number of events to be generated\n" )
      f.write( "ih1 1                               ! hadron 1 type\n" )
      f.write( "ih2 1                               ! hadron 2 type\n" )
      f.write( "lhans1 "+str(self.pdf)+"            ! pdf set for hadron 1 (LHA numbering)\n" )
      f.write( "lhans2 "+str(self.pdf)+"            ! pdf set for hadron 2 (LHA numbering)\n" )
      f.write( "ebeam1 "+str(self.energy)+"         ! energy of beam 1\n" )
      f.write( "ebeam2 "+str(self.energy)+"         ! energy of beam 1\n" )
      f.write( "facscfact "+str(self.muF)+"         ! factorization scale factor: mufact=muref*facscfact\n" )
      f.write( "renscfact "+str(self.muR)+"         ! renormalization scale factor: muren=muref*renscfact\n" )
      f.write( "pdfreweight "+str(self.storePdfInfo)+"\n" )
      f.write( "! Parameters to allow/disallow use of stored data\n" )
      f.write( "use-old-grid "+str(self.use_old_grid)+"      ! If 1 use old grid if file pwggrids.dat is present (<> 1 regenerate)\n" )
      f.write( "use-old-ubound "+str(self.use_old_ubound)+"  ! If 1 use norm of upper bounding function stored in pwgubound.dat, if present; <> 1 regenerate\n" )
      f.write( "! A typical call uses 1/1400 seconds (1400 calls per second)\n" )
      f.write( "ncall1 "+str(self.ncall1)+"           ! number of calls for initializing the integration grid\n" )
      f.write( "itmx1 "+str(self.itmx1)+"             ! No. iterations for grid: total 100000 calls ~ 70 seconds\n" )
      f.write( "ncall2 "+str(self.ncall2)+"           ! number of calls for computing the integral and finding upper bound\n" )
      f.write( "itmx2 "+str(self.itmx2)+"             ! No. iterations for the above\n" )
      f.write( "! Notice: the total number of calls is ncall2*itmx2*foldcsi*foldy*foldphi\n" )
      f.write( "! these folding numbers yield a negative fraction of 0.5% with bornktmin=10 GeV.\n" )
      f.write( "! With these settings: ncall2*itmx2*foldcsi*foldy*foldphi=5M, 60 minutes\n" )
      f.write( "foldcsi "+str(self.foldcsi)+"         ! number of folds on csi integration\n" )
      f.write( "!foldx 2                              ! number of folds on  x  integration\n" )
      f.write( "foldy   "+str(self.foldy)+"           ! number of folds on  y  integration\n" )
      f.write( "foldphi "+str(self.foldphi)+"         ! number of folds on phi integration\n" )
      f.write( "nubound "+str(self.nubound)+"         ! number of bbarra calls to setup norm of upper bounding function\n" )
      f.write( "withnegweights "+str(self.withnegweights)+"  ! Ending\n" )

  def generateEvents(self):
    # Start
    mglog.info('Starting POWHEG LHEF event generation at '+str(time.asctime()))
       
    # try:
    #   with open('filename'): pass
    # except IOError:
    #   print 'Oh dear.'

    # Generate the events and move output to correctly named file
    mglog.info('Running ' + str(self.executablePath))
    generate = subprocess.Popen( [self.executablePath,''])
    generate.wait()
    os.rename('pwgevents.lhe',self.outputEventsName)

    # Finish
    mglog.info('Finished at '+str(time.asctime()))
    return 0

  def generateFilteredEvents(self):
    # Start
    mglog.info('Starting POWHEG LHEF event generation at '+str(time.asctime()))
    filterPath = os.environ['LHEFPATH']
    filterPath += '/'+self.filter
    filterRunArgs.insert(0,'PowhegOutput._1.events')
    filterRunArgs.insert(0,'pwgevents.lhe')
    filterRunArgs.insert(0,str(filterPath))
    
    # Generate the events and move output to correctly named file
    print "Running " + str(self.executablePath)
    
    # Start Fifo
    setuppipe = subprocess.Popen( ['mkfifo','pwgevents.lhe'])
    setuppipe.wait()
    
    # Start Generation Process
    generate = subprocess.Popen( [self.executablePath,''])
    
    # Start Filter and wait until quota has been filled
    mglog.info("Running filter with the following commmand:")
    mglog.info(str(filterPath))
    filter = subprocess.Popen( self.filterRunArgs )
    filter.wait()
    outputline = filter.communicate()
    mglog.info(outputline)
    
    # Once quota filled, kill generation process
    generate.terminate()
    
    # Finish
    mglog.info('Finished at '+str(time.asctime()))
    return 0

###############################################################################
#
#  Dijet
#
###############################################################################
class PowhegConfig_Dijet(PowhegConfig_base):
  # These are user configurable - put generic properties in PowhegConfig_base
  bornktmin = 30
  bornsuppfact = -1
  fixspikes = True
  
  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None):
    PowhegConfig_base.__init__(self,runArgs)
    self.executablePath += '/Dijet/pwhg_main'
    mglog.info('In PowhegConfig_Dijet __init__')
    #os.system( 'pwd; ls -alh' )
    
  def generateRunCard(self):
    mglog.info('In PowhegConfig_Dijet generateRunCard')
    #os.system( 'pwd; ls -alh' )
    self.generateRunCardSharedOptions()
    mglog.info('In PowhegConfig_Dijet generateRunCard after shared options')
    #os.system( 'pwd; ls -alh' )

    with open( str(self.TestArea)+'/powheg.input','a') as f:
      f.write( "bornktmin "+str(self.bornktmin)+"      ! (default 0d0) Generation cut: minimum kt in underlying Born\n" )
      f.write( "bornsuppfact "+str(self.bornsuppfact)+"     ! (default 0d0) Mass parameter for Born suppression factor. If < 0 suppfact = 1\n" )
      if self.fixspikes == True :
        f.write( "doublefsr 1          ! fix problem with spikes in final observables\n" )
        f.write( "par_diexp 4          ! recommended additional options to be used with doublefsr\n" )
        f.write( "par_dijexp 4         ! (by Paolo Nason, private communication)\n" )
        f.write( "par_2gsupp 4         ! not sure of what happens if these are omitted!\n" )

###############################################################################
#
#  hvq
#
###############################################################################
class PowhegConfig_hvq(PowhegConfig_base):
  # These are user configurable - put generic properties in PowhegConfig_base
  bbscalevar = 1
  quarkMass = 4.95
  iymax = 1
  ixmax = 1
  xupbound = 2
  bornsuppfact = -1

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None):
    PowhegConfig_base.__init__(self,runArgs)
    self.executablePath += '/hvq/pwhg_main'

  def generateRunCard(self):
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f:
      f.write( "bornsuppfact  "+str(self.bornsuppfact)+"                   ! (default 0d0) Mass parameter for Born suppression factor. If < 0 suppfact = 1\n" )
      f.write( "! Heavy flavour production parameters\n" )
      f.write( "qmass  "+str(self.quarkMass)+"                         ! mass of heavy quark in GeV\n" )
      f.write( "bbscalevar "+str(self.bbscalevar)+" ! use variable re. and fct. scales\n" )
      f.write( "iymax "+str(self.iymax)+"           ! <= 10, normalization of upper bounding function in iunorm X iunorm square in y, log\n" )
      f.write( "ixmax "+str(self.ixmax)+"           ! <= 10, normalization of upper bounding function in iunorm X iunorm square in y, log\n" )
      f.write( "xupbound  "+str(self.xupbound)+"       ! increase upper bound for radiation generation\n" )

###############################################################################
#
#  WW
#
###############################################################################
class PowhegConfig_WW(PowhegConfig_base):
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymodeWp = -11 
  vdecaymodeWm = 13 
  mllmin = 20      

  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None):
    PowhegConfig_base.__init__(self,runArgs)
    self.executablePath += '/WW/pwhg_main'

  def generateRunCard(self):
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f:
      f.write( "vdecaymodeWp "+str(self.vdecaymodeWp)+" ! PDG code for charged decay product of the vector boson (1:dbar; 3: sbar; 7: dbar or sbar; -11:e+; -13:mu+; -15:tau+)\n" ) 
      f.write( "vdecaymodeWm "+str(self.vdecaymodeWm)+" ! PDG code for charged decay product of the vector boson (1:d; 3: s; 7: d or s; 11:e-; 13:mu-; 15:tau-)\n" )
      f.write( "mllmin "+str(self.mllmin)+"      ! Minimum invariant mass of lepton pairs from Z decay\n" )

###############################################################################
#
#  WZ
#
###############################################################################
class PowhegConfig_WZ(PowhegConfig_base):
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymodeW = -11
  vdecaymodeZ = 13
  mllmin = 20
  zerowidth = 0
  withinterference = 1
  dronly = 0
  diagCKM = 0
  delg1_z = 0
  delg1_g = 0
  lambda_z = 0
  lambda_g = 0
  delk_g = 0
  delk_z = 0
  tevscale = 2
  bornonly = 0
  wllmin = 0.01
  
  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None):
    PowhegConfig_base.__init__(self,runArgs)
    self.executablePath += '/WZ/pwhg_main'

  def generateRunCard(self):
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f:
      f.write( "vdecaymodeW "+str(self.vdecaymodeW)+"      ! PDG code for charged decay product of the W boson (11:e-;-11:e+;...)\n" )
      f.write( "vdecaymodeZ "+str(self.vdecaymodeZ)+"    ! PDG code for lepton Z boson (11:e-; 12: ve; ...)\n" )
      f.write( "mllmin "+str(self.mllmin)+"         ! default 0.1 GeV this is minimum invar mass for Z leptons\n" )
      f.write( "zerowidth "+str(self.zerowidth)+"         ! if 1 (true) use zero width approximatrion (default 0)\n" )
      f.write( "withinterference "+str(self.withinterference)+"  ! if 1 (true) include interference for like flavour charged leptons\n" )
      f.write( "dronly "+str(self.dronly)+"     ! if 1 (true) only include double resonant diagrams (default 0)\n" )
      f.write( "! CKM matrix\n" )
      f.write( "diagCKM "+str(self.diagCKM)+"     ! if 1 (true) then diagonal CKM (default 0)\n" )
      f.write( "! anom couplings\n" )
      f.write( "delg1_z    "+str(self.delg1_z)+"     ! Delta_g1(Z)\n" )
      f.write( "delg1_g    "+str(self.delg1_g)+"     ! Delta_g1(Gamma)\n" )
      f.write( "lambda_z   "+str(self.lambda_z)+"    ! Lambda(Z)\n" )
      f.write( "lambda_g   "+str(self.lambda_g)+"    ! Lambda(gamma)\n" )
      f.write( "delk_g     "+str(self.delk_g)+"      ! Delta_K(Gamma)\n" )
      f.write( "delk_z     "+str(self.delk_z)+"      ! Delta_K(Z)\n" )
      f.write( "tevscale   "+str(self.tevscale)+"    ! Form-factor scale, in TeV\n" )
      f.write( "bornonly   "+str(self.bornonly)+"    ! (default 0) if 1 do Born only\n" )
      f.write( "wllmin  "+str(self.wllmin)+"        ! default 1d-5 GeV this is min invar mass for W leptons\n" )

###############################################################################
#
#  ZZ
#
###############################################################################
class PowhegConfig_ZZ(PowhegConfig_base):
  # These are user configurable - put generic properties in PowhegConfig_base
  vdecaymodeZ1 = 11
  vdecaymodeZ2 = 11
  mllmin = 20
  
  # Set process-dependent paths in the constructor
  def __init__(self,runArgs=None):
    PowhegConfig_base.__init__(self,runArgs)
    self.executablePath += '/ZZ/pwhg_main'

  def generateRunCard(self):
    self.generateRunCardSharedOptions()

    with open( str(self.TestArea)+'/powheg.input','a') as f:
      f.write( "vdecaymodeZ1 "+str(self.vdecaymodeZ1)+"  ! PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)\n" )  
      f.write( "vdecaymodeZ2 "+str(self.vdecaymodeZ2)+"  ! PDG code for charged decay product of the vector boson (11:e-; -11:e+; ...)\n" )
      f.write( "mllmin  "+str(self.mllmin)+"    ! Minimum invariant mass of lepton pairs from Z decay\n" )
