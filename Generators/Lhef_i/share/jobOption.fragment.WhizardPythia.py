###############################################################
# Pythia Commands
# Modeled after DC3.005166.MadGraphZ4jQCD4QED1.py
# by
# 06/2006, G. Azuelos, P.A. Delsart, J. Idarraga
# Modified for Whizard fragmentation by J. Schumacher

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls  += [ "Pythia_i" ]
theApp.TopAlg = ["Pythia"]#,"DumpMC"]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
# Channel switches
Wtojj = "0"
Wtoln = "1"
Ztojj = "0"
Ztoll = "1"
Pythia = Algorithm( "Pythia" )
#---------------------------------------------------------------
Pythia.PythiaCommand = [
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

#--------------------------------------------------------------
# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
from EvgenJobOptions.PythiaEvgenConfig import evgenConfig
evgenConfig.efficiency = 0.9
evgenConfig.generators = ["Pythia","Lhef"]
evgenConfig.inputfilebase = "whizard.lhe"
evgenConfig.minevents  = 1  # Don't crash when testing
evgenConfig.maxeventsfactor = 100000000  # dito
