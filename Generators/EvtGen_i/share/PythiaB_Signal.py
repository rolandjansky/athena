###############################################################
#
# Job options file for generation of B-events 
#  in user selected exclusive channel
#
#==============================================================
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
# RunNumber, FirstEvent re-written if use B job submition script
#--------------------------------------------------------------
theApp.EvtMax = 5
EventSelector.RunNumber   = 1
EventSelector.FirstEvent  = 1
#--------------------------------------------------------------
#User random number seeds - re-written if use B job submition script
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532"]
# AtRndmGenSvc.ReadFromFile = true;
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------				 
#              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
#PythiaB.ForceBDecay = "no";
PythiaB = Algorithm( "PythiaB" )
PythiaB.ForceCDecay = "no"
#--------------------------------------------------------------				 
# -------------  FORCE   YOUR  B CHANNEL  HERE -------------
#--------------------------------------------------------------
# To force your B-decay channels decomment following 2 lines:
include( "CloseAntibQuark.py" )

PythiaB.ForceBDecay = "yes"
#   T H R E E   E X A M P L E S    T O   O P E N    Y O U R    C H A N N E L 				 
#   Decomment one of following examples or create your analogically
# open your exclusive channel here  Bs -> J/psi(mumu) phi 				 
PythiaB.PythiaCommand += ["pydat3 mdme 982 1 1",    
				"pydat3 mdme 858 1 0",
				"pydat3 mdme 860 1 0"        ]
# open your exclusive channel here  Bs -> Ds pi with Ds->phi pi
##include "Dsphipi.txt" 				 
#PythiaB.PythiaCommand += {"pydat3 mdme 967 1 1",
#                                "pydat3 mdme 831 1 1"      };
#PythiaB.ForceCDecay = "yes";                                
# open your exclusive channel here  Bs -> J/psi(mumu) K0 
#PythiaB.PythiaCommand += {"pydat3 mdme 889 1 1",    
#				"pydat3 mdme 858 1 0",
#				"pydat3 mdme 860 1 0"        };
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------
#  'msel 5' is only for fast tests! 
#  for correct b-producion you should use 'msel 1'
# 'mstj 26 0' = no mixing was defined in Btune as default
# 'mstj 22 2' = no K0S, Lambda0 decays in Pythia - defined in Btune as default

include( "Btune.py" )

PythiaB.PythiaCommand += ["pysubs ckin 3 15.",
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
PythiaB.cutbq = ["0. 102.5 and 10. 2.5"]
#  ------------- LVL1 muon cuts 0=OFF 1=ON -------------
PythiaB.lvl1cut = [ 1.,  6., 2.5]
#  ------------- LVL2 muon/electron cuts  0=OFF 1=ON-------------
PythiaB.lvl2cut = [ 0.,  13.,     6.,   2.5]
#PythiaB.lvl2cut = { 0.,  11.,     6.,   2.5};
#  ------------- Offline cuts 0=OFF 1=ON -------------
PythiaB.offcut = [ 1., 0.5, 2.5, 3., 2.5, 0.5, 2.5]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  14. 
#  ------------- For how many events store B-chain in NTUPLE -------------
BSignalFilter = Algorithm( "BSignalFilter" )
BSignalFilter.SignaltoNtup = 50
#  ------------- For how many events store B-chain in ascii -------------
PythiaB.SignalDumptoAscii =  50.



###############################################################

#--------------------------------------------------------------
# Histogram & Ntuple Persistency
#--------------------------------------------------------------
# Change the following line to "RootHistCnv" for ROOT persistency
#theApp.Dlls                += [ "RootHistCnv" ]
## Change the following line to "ROOT" for ROOT persistency
#theApp.HistogramPersistency = "ROOT"
#--------------------------------------------------------------
# NTuple output file
#--------------------------------------------------------------
#RootHistSvc = Service( "RootHistSvc" )
##HbookHistSvc.NPAWC = 1500000
##HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
##HistogramPersistencySvc.OutputFile  = "histo.root"
#NTupleSvc = Service( "NTupleSvc" )
#NTupleSvc.Output = [ "FILE1  DATAFILE='pythiaB.root'  OPT='NEW'  TYP='ROOT'" ]

# Change since 12.0.3
theApp.Dlls += [ "AnalysisTools" ]
THistSvc = Service ( "THistSvc" )
THistSvc.Output = ["AANT DATAFILE='pythiaB.root' OPT='NEW'"]
theApp.TopAlg += [ "AANTupleStream" ]
AANTupleStream = Algorithm( "AANTupleStream" )
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = 'pythiaB.root'
AANTupleStream.ExistDataHeader = False
AANTupleStream.OutputLevel = VERBOSE




###############################################################
# Add POOL persistency
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
# 2101 = EventInfo
# 133273 = MCTruth (HepMC)
Stream1.ItemList += [ "2101#*", "133273#*" ]
Stream1.OutputFile = "pythiaB.pool.root"

          

#==============================================================
#
# End of job options file
#
###############################################################
