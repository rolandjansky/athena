 ###############################################################
#
#  Job options file for generation of B-events 
#  in user selected exclusive channel
#
#  Decay channel: B0->D(Phi(K+K-)Pi)A1(Rho(Pi+Pi-)Pi)
#
#  Author : W. Walkowiak, 2006-12-11
#           (adjusted after a template provided by PythiaB)
#  changed:
#
#  $Id: PythiaB_B0_DPhiA1_Signal3.py,v 1.2 2007-03-20 14:21:33 msmizans Exp $
#
#==============================================================
#
varInit = dir()

# default options (can be overwritten by command line options)
PythiaBDecayFlags = {
    'BtuneOptionsFile'     : "Btune.py"
    }

# set variable to default or from personalized JobOptions
# if not already set (code taken from RecExCommon)
for o in [ o for o in PythiaBDecayFlags.keys() if not o in varInit ]:
        exec '%s = PythiaBDecayFlags["%s"]' % (o,o)

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
#theApp.setup( MONTECARLO )
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" ) 

include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "GaudiAlg" ]
theApp.Dlls += [ "TruthExamples", "PythiaB" ]
theApp.Dlls += [ "GaudiAud" ]
theApp.Dlls += [ "HbookCnv" ]
theApp.Dlls += [ "GeneratorFilters" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  = [ "ChronoAuditor" ]
#--------------------------------------------------------------
# Algorithms 
#--------------------------------------------------------------
theApp.TopAlg = ["PythiaB" ,  "BSignalFilter"]
theApp.ExtSvc += ["AtRndmGenSvc"]
#--------------------------------------------------------------
# Number of events to be accepted !! (default is 10)
# re-written if use B job submition script
# RunNumber, FirstEvent re-written if use B job submission script
#--------------------------------------------------------------
theApp.EvtMax = 100
EventSelector.RunNumber   = 1
EventSelector.FirstEvent  = 1
#--------------------------------------------------------------
#User random number seeds - re-written if use B job submission script
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
AtRndmGenSvc.ReadFromFile = True;
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
PythiaB = Algorithm( "PythiaB" )
PythiaB.ForceCDecay = "no"
#--------------------------------------------------------------				 
# -------------  FORCE   YOUR  B CHANNEL  HERE -------------
#--------------------------------------------------------------
# To force your B-decay channels decomment following 2 lines:
include( "CloseAntibQuark.py" )
PythiaB.ForceBDecay = "yes"
#
# open your exclusive channel here  B0 -> D- a1+ with D->phi pi
#
# DXChannel to be set here (close all D- -> phi pi- channels)
DXChannel = "Dphipi"
include( "DXphipi.py" )
#
# 879: B0 -> D- a1+
# 715: D- -> Phi Pi-
#  				 
PythiaB.PythiaCommand += [ "pydat3 mdme 879 1 1",
                           "pydat3 mdme 715 1 1"   ];
PythiaB.ForceCDecay = "yes";                               

#
# w.w., 2006-12-11 revised:
# 
# user_finsel.F decay particle channel
#
PythiaB.ForceDecayChannel = "DsPhiX";
#
# Decay particle selection for DsPhiX: 
#   0. (off)/ 1. (on) -- ordered as follows:
#     1 : PhiKK   decay -- Phi -> K+K- 
#                          ((from B0s, B0bar or B-) or (from B0sbar, B0 or B+)
#                          or (from B0->D-X) or (from B0bar->D+X))
#     2 : DsPhi   decay -- Ds*- -> Ds-X, Ds- -> Phi pi-, Phi -> K+K- 
#                          (from B0s, B0bar or B-) or (from B0sbar, B0 or B+))
#     3 : A1RhoPi decay -- a1 -> RhoPi   (from B0s)
#     4 : RhoPiPi decay -- Rho -> Pi+Pi- (from B0s)
#
PythiaB.DecayChannelParameters = [1., 0., 1., 1.];
 
#
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------
# 'msel 5' is only for fast tests! 
#  for correct b-production you should use 'msel 1'
# 'mstj 26 0' = no mixing was defined in Btune as default
# 'mstj 22 2' = no K0S, Lambda0 decays in Pythia - defined in Btune as default

include( BtuneOptionsFile )

# PythiaB.PythiaCommand += ["pysubs ckin 3 6.",
PythiaB.PythiaCommand += ["pysubs ckin 3 10.",
                          "pysubs ckin 9 -3.5",
                          "pysubs ckin 10 3.5",
                          "pysubs ckin 11 -3.5",
                          "pysubs ckin 12 3.5",
                          "pysubs msel 1"]

#--------------------------------------------------------------
# -------------  DEFINE SELECTION CUTS  -------------
#--------------------------------------------------------------				
#  ------------- Selections on b  quarks   -------------
# simulate  only b-flavour events
PythiaB.flavour =  5.
# PythiaB force exclusive decay channels only on b=-5 side 
# ------------------- b=5  --- and/or ---  b=-5 --------
PythiaB.cutbq = ["6.0 2.5 and 6.0 2.5"]
# PythiaB.cutbq = ["6.0 4.5 and 6.0 4.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 1., 0.5, 2.5, 3., 2.5, 0.5, 2.5]
# 
#  ------------- Maximum number of tries for hard process -------------
PythiaB.maxTriesHard = 100000.
# 
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  100.
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter = Algorithm( "BSignalFilter" )
BSignalFilter.SignaltoNtup = 400
#  ------------- For how many events store B-chain in ascii -------------
PythiaB.SignalDumptoAscii =  10.

#--------------------------------------------------------------
# CBNT output
#--------------------------------------------------------------
CBNTAthenaAware=True
include( "CBNT_Athena/CBNT_AthenaAware_jobOptions.py" )
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )
# include( "RecExCommon/CBNT_Truth_jobOptions.py" )
include( "CBNT_Truth_jobOptions.py" )
CBNT_Truth.MaxNbParticles = 6000    # maximum number of particles in the ntuple
CBNT_Truth.MaxNbVertices  = 6000    # maximum number of vertices in the ntuple
All.Enable = True                   # save ALL particles

#--------------------------------------------------------------
# Histogram & Ntuple Persistency 
#--------------------------------------------------------------
theApp.Dlls                += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"

#--------------------------------------------------------------
# NTuple output file 
#--------------------------------------------------------------
RootHistSvc = Service( "RootHistSvc" )
NtupleSvc = Service( "NtupleSvc" )
NtupleSvc.Output = ["FILE1 DATAFILE='pythiaB.root' TYP='ROOT' OPT='NEW'"]

# Change since 12.0.3
theApp.Dlls += [ "AnalysisTools" ]
THistSvc = Service ( "THistSvc" )
THistSvc.Output = ["AANT DATAFILE='pythiaB.root' OPT='NEW'"]
theApp.TopAlg += [ "AANTupleStream" ]
AANTupleStream = Algorithm( "AANTupleStream" )
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = 'pythiaB.root'
AANTupleStream.ExistDataHeader = False
AANTupleStream.OutputLevel = INFO

#----------------------------------------------------------------              # Pool persistency
#----------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)
Stream1.ItemList += ["2101#*", "133273#*" ]
Stream1.OutputFile = "pythiaB.pool.root"

#==============================================================
#
# End of job options file
#
###############################################################
