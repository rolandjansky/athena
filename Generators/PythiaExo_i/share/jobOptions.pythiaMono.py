###############################################
#
# JobOptions for Drell-Yan monopole production
# through gamma/Z*
#
# Author: Daniel Goldin
#
###############################################

###############################################
#
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#

from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

# MC8_Pythia_Common.py not included as it messes up
# the monopole generation. It is included in the
# background analysis

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = 3

###############################################
#
# User-defined inputs

# Mono mass
MONOPOLEMASS = 500.
# Number of events to be processed (default is 10)
NumEvts = 100

###############################################
#
# Initialize Pythia
#

theApp.EvtMax = NumEvts

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512","PYTHIA_INIT 820021 2347532"]

from PythiaExo_i.PythiaMono_iConf import PythiaMono
topAlg += PythiaMono()

###################################################
#
# Pythia commands
# gamma/Z* interference must be off but Z*->mu mu
# should still be included
#

topAlg.PythiaMono.PythiaCommand = [

    # Turn on q q_bar -> gamma/Z*
    "pysubs msub 1 1",

    # Virtual gamma only (turn off gamma/Z* interference)
    "pypars mstp 43 1",

    # Kine threshold cut
    "pysubs ckin 1 " + str(2*MONOPOLEMASS+1),

    # Mono mass (in place of monopoles we use "heavy" muons)
    "pydat2 pmas 13 1 " + str(MONOPOLEMASS),

    #     Turn on customized subprocesses
    "pysubs msel 0",

    # turn off some processes

    # no initial state showers
    "pypars mstp 61 0",
    # no final state showers
    "pypars mstp 71 0",
    # no multiple interactions
    "pypars mstp 81 0",
    # no hadronization
    "pypars mstp 111 0",
    
    #     Turn off mu decays
    "pydat3 mdme  83 1 0",
    "pydat3 mdme  84 1 0",
    "pydat3 mdme  85 1 0",
    "pydat3 mdme  86 1 0",   
    # gamma decays
    "pydat3 mdme 162 1 0",
    "pydat3 mdme 163 1 0",
    "pydat3 mdme 164 1 0",
    "pydat3 mdme 165 1 0",  
    "pydat3 mdme 166 1 0",
    "pydat3 mdme 167 1 0",
    "pydat3 mdme 168 1 0",
    "pydat3 mdme 169 1 0",  
    "pydat3 mdme 170 1 0",
    #     gamma -> mu+mu-
    "pydat3 mdme 171 1 1",
    "pydat3 mdme 172 1 0",
    "pydat3 mdme 173 1 0",  
    # Z decays
    "pydat3 mdme 174 1 0",
    "pydat3 mdme 175 1 0",
    "pydat3 mdme 176 1 0",
    "pydat3 mdme 177 1 0",
    "pydat3 mdme 178 1 0",
    "pydat3 mdme 179 1 0",
    "pydat3 mdme 182 1 0",
    "pydat3 mdme 183 1 0", 
    #     Z0 -> mu+mu-
    "pydat3 mdme 184 1 1",
    "pydat3 mdme 185 1 0", 
    "pydat3 mdme 186 1 0",
    "pydat3 mdme 187 1 0",
    "pydat3 mdme 188 1 0",
    "pydat3 mdme 189 1 0",

    # Dump event and initial settings
    "pyinit pylisti 12",
    "pyinit pylistf 1",
    "pystat 1 3 4 5",
    "pyinit dumpr 1 5"
    ]

# ASCII MC Event dump
# from TruthExamples.TruthExamplesConf import DumpMC
# topAlg += DumpMC()

##############################################################
# Translate particles codes
# Assign heavy muon (PDG ID = 13) new ID = 50, which
# corresponds uniquely to a monopole and used
# to propagate in GEANT
#############################################################
theApp.Dlls += [ "HepMCTools" ]
theApp.TopAlg += [ "TranslateMonoID" ]
TranslateMonoID = Algorithm( "TranslateMonoID" )
TranslateMonoID.OutputLevel = 1

####################################################################
#
# POOL/ROOT output
#

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "pythia.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
Stream1.OutputFile = "pythia_MONO_GeV_"+str(MONOPOLEMASS)+"GeV_"+str(NumEvts)+"evts.pool.root"

####################################################################
