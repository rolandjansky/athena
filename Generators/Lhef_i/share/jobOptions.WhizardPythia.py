###############################################################
# Pythia Commands
# Modeled after DC3.005166.MadGraphZ4jQCD4QED1.py
# by
# 06/2006, G. Azuelos, P.A. Delsart, J. Idarraga
# Modified for Whizard fragmentation by J. Schumacher
# Last modification: M. Rammes, 24/08/2010
#--------------------------------------------------------------
# Instructions: In order to use this, you need to have an Les Houches Event
# File with the name events.lhe in your current working directory.
# Alternatively you can use a symbolic link.
#

# IMPORTANT!!! The following line adds the correct meta data to Pythia output!
import AthenaCommon.AtlasUnixGeneratorJob
#--------------------------------------------------------------

# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512",
"PYTHIA_INIT 820021 2347532"]
# ServiceManager.AtRndmGenSvc.ReadFromFile = true;

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Pythia_i.Pythia_iConf import Pythia
job +=Pythia()

# Channel switches
Wtojj = "0"
Wtoln = "1"
Ztojj = "0"
Ztoll = "1"
#---------------------------------------------------------------
job.Pythia.PythiaCommand = [
    # Use Madgraph interface
    "pyinit user lhef",

    # Set process type and W' mass
    "pysubs msel 0",

    "pydat2 pmas 25 1 1.E9",         # Higgs mass

    #####################
    # Z -> ee or mu mu (apago los jets)
    # the last parameter turns on(1) or turns off(0) the desired channel
    # but he will take in to account for the width ! ... watch out !

    # Z -> to quarks
    "pydat3 mdme 174 1 "+Ztojj,
    "pydat3 mdme 175 1 "+Ztojj,
    "pydat3 mdme 176 1 "+Ztojj,
    "pydat3 mdme 177 1 "+Ztojj,
    "pydat3 mdme 178 1 "+Ztojj,
    "pydat3 mdme 179 1 "+Ztojj,

    # Z -> ll
    "pydat3 mdme 182 1 "+Ztoll,
    "pydat3 mdme 183 1 0",      # to nu_e
    "pydat3 mdme 184 1 "+Ztoll,
    "pydat3 mdme 185 1 0",      # to nu_mu

    # Close Z -> tau channels
    "pydat3 mdme 186 1 0",
    "pydat3 mdme 187 1 0",
    "pydat3 mdme 188 1 0",
    "pydat3 mdme 189 1 0",


    ######################
    # W -> leptons chanels  -------------
    "pydat3 mdme 206 1 "+Wtoln,
    "pydat3 mdme 207 1 "+Wtoln,

    # Close W -> tau channels
    "pydat3 mdme 208 1 0", # no tau
    "pydat3 mdme 209 1 0",


    # W -> quarks channels -------------------
    "pydat3 mdme 190 1 "+Wtojj,
    "pydat3 mdme 191 1 "+Wtojj,
    "pydat3 mdme 192 1 "+Wtojj,
    # "pydat3 mdme 193 1 "+Wtojj,  # to t' : keep default value
    "pydat3 mdme 194 1 "+Wtojj,
    "pydat3 mdme 195 1 "+Wtojj,
    "pydat3 mdme 196 1 "+Wtojj,
    #"pydat3 mdme 197 1 "+Wtojj, # to t' : keep default value
    "pydat3 mdme 198 1 "+Wtojj,
    "pydat3 mdme 199 1 "+Wtojj,
    "pydat3 mdme 200 1 "+Wtojj,
    ##     "pydat3 mdme 201 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 202 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 203 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 204 1 "+Wtojj,  # to t' : keep default value
    ##     "pydat3 mdme 205 1 "+Wtojj,  # to t' : keep default value

    # No tau decays (Tauola)
    "pydat3 mdcy 15 1 0",
    # No FSR (Photos)
    "pydat1 parj 90 20000",

    ######################
    "pysubs ckin 1 300",   # minimun for the invariant mass
                           # of WZ
    "pyinit pylistf 1",

    # list 1st 10 events
    "pyinit dumpr 1 10",

    # print the decay info
    "pyinit pylisti 12"

]

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "pythia.pool.root"
Stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*' ]
###############################################################
