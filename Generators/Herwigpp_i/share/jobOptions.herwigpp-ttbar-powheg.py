## Job option script for POWHEG with Herwig++
## Riccardo Di Sipio
##
## See bottom of this file for an example POWHEG config

include("GeneratorUtils/StdEvgenSetup.py")

# ## Control log levels
# svcMgr.MessageSvc.OutputLevel = DEBUG

## Add H++ to the job list
from Herwigpp_i.Herwigpp_iConf import Herwigpp
topAlg += Herwigpp()

from Herwigpp_i import config as hw
## According to Paolo Nason, using NLO PDFs for ME and LO for PS/UE is okay
cmds = hw.energy_cmds(7000) + hw.base_cmds() + hw.lo_pdf_cmds("cteq6ll.LHpdf") + hw.ue_tune_cmds("CTEQ6L1-UE-EE-7000-3")

# TODO: Check that mass resets (to match POWHEG) are approved.
cmds += """
setup /Herwig/Particles/t     6 t    172.5 1.4 14 0  2  3 2 0
#setup /Herwig/Particles/tbar -6 tbar 174.2 1.4 14 0 -2 -3 2 0
set /Herwig/Particles/u:NominalMass 0.320
set /Herwig/Particles/ubar:NominalMass 0.320

set /Herwig/Particles/d:NominalMass 0.320
set /Herwig/Particles/dbar:NominalMass 0.320

set /Herwig/Particles/s:NominalMass 0.5
set /Herwig/Particles/sbar:NominalMass 0.5

set /Herwig/Particles/c:NominalMass 1.55
set /Herwig/Particles/cbar:NominalMass 1.55

set /Herwig/Particles/b:NominalMass 4.95
set /Herwig/Particles/bbar:NominalMass 4.95

set /Herwig/Particles/W+:NominalMass 80.398
set /Herwig/Particles/W+:Width 2.141
set /Herwig/Particles/W-:NominalMass 80.398
set /Herwig/Particles/W-:Width 2.141

set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption General
## create Herwig::PowhegEvolver /Herwig/Shower/PowhegEvolver HwPowhegShower.so
## set /Herwig/Shower/ShowerHandler:Evolver  /Herwig/Shower/PowhegEvolver

## set /Herwig/Shower/PowhegEvolver:ShowerModel /Herwig/Shower/ShowerModel
## set /Herwig/Shower/PowhegEvolver:SplittingGenerator /Herwig/Shower/SplittingGenerator
## set /Herwig/Shower/PowhegEvolver:MECorrMode No
## set /Herwig/Shower/PowhegEvolver:IntrinsicPtGaussian 0.0*GeV
## #set /Herwig/Shower/PowhegEvolver:HardOnly Yes
## #set /Herwig/Shower/PowhegEvolver:TruncatedShower No
set /Herwig/Shower/Evolver:HardVetoScaleSource Read
set /Herwig/Shower/Evolver:IntrinsicPtGaussian 2.2*GeV
#set /Herwig/Shower/Evolver:MECorrMode No

library LesHouches.so
#library MadGraphReader.so

## Create LesHouchesReader object
#create ThePEG::MadGraphReader /Herwig/EventHandlers/myReader
create ThePEG::LesHouchesFileReader /Herwig/EventHandlers/myReader
set /Herwig/EventHandlers/myReader:FileName events.lhe
set /Herwig/EventHandlers/myReader:CacheFileName cache.tmp

## Attach PDFs to reader
set /Herwig/EventHandlers/myReader:InitPDFs 0
#set /Herwig/EventHandlers/myReader:PDFA /Herwig/Partons/AtlasPDFsetNLO
#set /Herwig/EventHandlers/myReader:PDFB /Herwig/Partons/AtlasPDFsetNLO
set /Herwig/EventHandlers/myReader:PDFA /Herwig/Partons/AtlasPDFsetLO
set /Herwig/EventHandlers/myReader:PDFB /Herwig/Partons/AtlasPDFsetLO

## Apply kinematic cuts
#set /Herwig/EventHandlers/myReader:InitCuts 0
set /Herwig/EventHandlers/myReader:Cuts /Herwig/Cuts/QCDCuts
set /Herwig/Cuts/JetKtCut:MinKT 0.0*GeV

## Create LesHouchesEventHandler object
create ThePEG::LesHouchesEventHandler /Herwig/EventHandlers/myLesHouchesHandler
set /Herwig/EventHandlers/myLesHouchesHandler:CascadeHandler /Herwig/Shower/ShowerHandler
set /Herwig/EventHandlers/myLesHouchesHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler
set /Herwig/EventHandlers/myLesHouchesHandler:DecayHandler /Herwig/Decays/DecayHandler
set /Herwig/EventHandlers/myLesHouchesHandler:PartonExtractor /Herwig/Partons/QCDExtractor
set /Herwig/Generators/LHCGenerator:EventHandler /Herwig/EventHandlers/myLesHouchesHandler

insert /Herwig/EventHandlers/myLesHouchesHandler:LesHouchesReaders[0] /Herwig/EventHandlers/myReader
set /Herwig/Generators/LHCGenerator:EventHandler /Herwig/EventHandlers/myLesHouchesHandler

set LHCGenerator:DebugLevel 1
set LHCGenerator:PrintEvent 1
set /Herwig/Generators/LHCGenerator:MaxErrors 70000

erase  LHCGenerator:AnalysisHandlers 0 /Herwig/Analysis/Basics

## cp /Herwig/Generators/LHCGenerator /Herwig/Generators/myLesHouchesGenerator
## set /Herwig/Generators/myLesHouchesGenerator:EventHandler /Herwig/EventHandlers/myLesHouchesHandler
## saverun myLesHouches /Herwig/Generators/myLesHouchesGenerator
"""

## Set commands
topAlg.Herwigpp.Commands = cmds.splitlines()


###############################################################
#
# EXAMPLE FOR POWHEG INPUT FILE
#
#! Heavy flavour production parameters
#maxev 25000  ! number of events to be generated
#randomseed 1379331196 !!!!!
#seed 0    ! random seed
#seedn1 0       ! seed counter 1
#seedn2 0       ! seed counter 2
#ih1   1        ! hadron 1
#ih2   1        ! hadron 2
#ndns1 131      ! pdf for hadron 1
#ndns2 131      ! pdf for hadron 2
#lhans1 7000   ! LHAPDF for hadron 1
#lhans2 7000   ! LHAPDF for hadron 2
#ebeam1 5000    ! energy of beam 1
#ebeam2 5000    ! energy of beam 2
#qmass 172.5   ! mass of heavy quark in GeV
#facscfact 1    ! factorization scale factor: mufact=muref*facscfact
#renscfact 1    ! renormalization scale factor: muren=muref*renscfact
#underlyingevent 1 ! UH: had to add this...
#bbscalevar 1    ! use variable re. and fct. scales
#
#
#topdecaymode 22222   ! an integer of 5 digits that are either 0, or 2, representing in
#                     ! the order the maximum number of the following particles(antiparticles)
#                     ! in the final state: e  mu tau up charm
#                     ! For example
#                     ! 22222    All decays (up to 2 units of everything)
#                     ! 20000    both top go into b l nu (with the appropriate signs)
#                     ! 10011    one top goes into electron (or positron), the other into (any) hadrons
#                     ! 00022    Fully hadronic
#                     ! 00002    Fully hadronic with two charms
#                     ! 00011    Fully hadronic with a single charm
#                     ! 00012    Fully hadronic with at least one charm
#
#! Parameters for the generation of spin correlations in t tbar decays
#tdec/wmass 80.403  ! W mass for top decay
#tdec/wwidth 2.141
#tdec/bmass 4.95
#tdec/twidth 1.7000
#tdec/elbranching 0.108
#tdec/emass 0.00051
#tdec/mumass 0.1057
#tdec/taumass 1.777
#tdec/dmass   0.320
#tdec/umass   0.320
#tdec/smass   0.5
#tdec/cmass   1.55
#tdec/sin2cabibbo 0.051
#
#! Parameters to allow-disallow use of stored data
#use-old-grid -1    ! if 0 use old grid if file pwggrids.dat is present (# 0: regenerate)
#use-old-ubound -1  ! if 0 use norm of upper bounding function stored in pwgubound.dat, if present; # 0: regenerate
#
#ncall1 7000   ! number of calls for initializing the integration grid
#itmx1 5        ! number of iterations for initializing the integration grid
#ncall2 70000  ! number of calls for computing the integral and finding upper bound
#itmx2 5        ! number of iterations for computing the integral and finding upper bound
#foldx   1      ! number of folds on x integration
#foldy   1      ! number of folds on y integration
#foldphi 1      ! number of folds on phi integration
#nubound 70000  ! number of bbarra calls to setup norm of upper bounding function
#iymax 1        ! <= 10, normalization of upper bounding function in iunorm X iunorm square in y, log(m2qq)
#ixmax 1        ! <= 10, normalization of upper bounding function in iunorm X iunorm square in y, log(m2qq)
#xupbound 2      ! increase upper bound for radiation generation
