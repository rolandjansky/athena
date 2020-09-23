#############################################################################
#
# Generation of Polarized Lambda_b events   Lambda_b->J/psi(mu+mu-)Lambda(p pi)
#    Prepared by: Eduard De La Cruz Burelo,  17/04/2006
##############################################################################
#
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
theApp.setup( MONTECARLO )
include( "PartPropSvc/PartPropSvc.py" )
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls += [ "GaudiAlg" ]
theApp.Dlls += [ "TruthExamples", "PythiaB" ]
theApp.Dlls += [ "GaudiAud" ]
theApp.Dlls += [ "HbookCnv" ]
theApp.Dlls += [ "GeneratorFilters" ,"EvtGen_i" ]
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  = [ "ChronoAuditor" ]
#--------------------------------------------------------------
# Algorithms
#--------------------------------------------------------------
theApp.TopAlg = ["PythiaB" , "EvtLbPolDecay",  "LambdabFilter"]
theApp.ExtSvc += ["AtRndmGenSvc"]
#--------------------------------------------------------------
# Number of events to be accepted !! (default is 10)
# re-written if use B job submition script
# RunNumber, FirstEvent re-written if use B job submition script
#--------------------------------------------------------------
theApp.EvtMax = 1000
EventSelector.RunNumber   = 1
EventSelector.FirstEvent  = 1
#--------------------------------------------------------------
# User random number seeds - re-written if use B job submition script
# EvtGen now uses the "PYTHIA" value as the random seed
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 250053", "PYTHIA_INIT 820021 2347532", "EVTGEN 4789899 821000366"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------                                                                                                                     #              PARAMETERS  SPECIFIC  TO   PYTHIAB
#--------------------------------------------------------------
include( "EvtGen_i/StopPytWeakBdecays.py" )
#--------------------------------------------------------------
# --------  PYTHIA PARAMETERS OPTIMAL FOR BEAUTY PRODUCTION  --
#--------------------------------------------------------------
#  'msel 5' is only for fast tests!
#  for correct b-producion you should use 'msel 1'
include( "PythiaB/Btune.py" )
PythiaB = Algorithm( "PythiaB" )
PythiaB.PythiaCommand += ["pysubs ckin 3 9.",
                               "pysubs ckin 9 -3.5",
                               "pysubs ckin 10 3.5",
                               "pysubs ckin 11 -3.5",
                               "pysubs ckin 12 3.5",
                               "pysubs msel 1"]
#  ------------- Selections on b  quarks   -------------
PythiaB.cutbq = ["0. 100. or 10. 3.5"]
#  ------------- Number of repeated hadronization mhadr -------------
PythiaB.mhadr =  15
#  ------------- Control flags  on the userFinsel file    -------------  
PythiaB.ForceDecayChannel = "LambdabJpsimumuLambda"
PythiaB.DecayChannelParameters = [7.,3.]  # Minimum pt of Lambdab = 7, and maximum abs(etha)=3.
#  ------------- For how many events store B-chain in NTUPLE -------------
LambdabFilter = Algorithm( "LambdabFilter" )
LambdabFilter.SignaltoNtup = 20000
LambdabFilter.EtaCut = 2.7;
LambdabFilter.PtCut = 500.;
LambdabFilter.LowPtMuCut = 2500.;
LambdabFilter.HighPtMuCut = 4000.;
LambdabFilter.WriteNtuple = False
#--------------------------------------------------------------
# YOUR CONTROL OF EVTGEN
#--------------------------------------------------------------
# If you do not define a name to your Decay-table file here
# then EvtGen will work with a default decay table DECAY.DEC
EvtLbPolDecay = Algorithm( "EvtLbPolDecay" )
EvtLbPolDecay.userDecayTableName = "LAMBDAB.DEC"
# Here you set the polarization value
EvtLbPolDecay.EvtGenPol=0.4
# This control the production of histograms for certification
EvtLbPolDecay.MakeHisto=False
#--------------------------------------------------------------
# Histogram & Ntuple Persistency
#--------------------------------------------------------------
#Change the following line to "RootHistCnv" for ROOT persistency
theApp.Dlls                += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NtupleSvc = Service( "NtupleSvc" )
NtupleSvc.Output = ["FILE1 DATAFILE='ntuple_LbPol.root' TYP='ROOT' OPT='NEW'"]
#----------------------------------------------------------------                    
# Pool persistency
#----------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
Stream1 = Algorithm( "Stream1" )
Stream1.ItemList += ["2101#*", "133273#*" ]
Stream1.AcceptAlgs += ["LambdabFilter"]
PoolSvc = Service( "PoolSvc" )
Stream1.OutputFile = "LbPol.root.pool"
#==============================================================
#
# End of job options file
#
###############################################################

