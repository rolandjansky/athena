###############################################################
# Pythia Commands
#
# !!! WARNING !!! 
# ChL model, using modified pythia code through PythiaChL_i
# pysghg_ChL.f (Georges Azuelos)
#
# 06/2006, G. Azuelos, P.A. Delsart, J. Idarraga
###############################################################
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO

MyNevts = 10
theApp.EvtMax = MyNevts

# dijet production with pythia
from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

from PythiaChL_i.PythiaChL_iConf import PythiaChL
topAlg += PythiaChL()

###############################################################
# Channel switches
Ztojj = "0"
Ztoll = "4"
Ztonunu = "5"
# close tau channels
# for jjll both mdme 186 and 187 are "0"
# for llnn mdme186=0 and mdme187=5
Ztotau186 = "0"
Ztotau187 = "0"

if Ztonunu == "0":
    Ztotau187 = "0"
elif Ztonunu == "4" or Ztonunu == "5":
    Ztotau187 = "5"
else:
    Ztotau187 = "0"
    
print "[INFO] Optios for tau: "
print "[INFO] mdme 186 = ", Ztotau186
print "[INFO] mdme 187 = ", Ztotau187
    
Pythia = topAlg.PythiaChL
#---------------------------------------------------------------
Pythia.PythiaCommand = [

    #######################
    # using madgraph
    # "pyinit user madgraph",

    # Set process type and W' mass
    "pysubs msel 0",

    #######################
    # if using madgraph
    # comment out the next
    # msub, mstp and parp options
    #######################
    "pysubs msub 71 1",
    "pysubs msub 76 1",
    "pypars mstp 46 5",            #
    "pypars parp 200 0.009",
    "pypars parp 199 0.009",

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
    "pydat3 mdme 183 1 "+Ztonunu,     # to nu_e
    "pydat3 mdme 184 1 "+Ztoll,
    "pydat3 mdme 185 1 "+Ztonunu,     # to nu_mu
    
    # Close Z -> tau channels
    "pydat3 mdme 186 1 "+Ztotau186,
    "pydat3 mdme 187 1 "+Ztotau187,    # IMPORTANT !!! 5 for the case llnn
    # "pydat3 mdme 188 1 0",
    # "pydat3 mdme 189 1 0",
    
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
#from EvgenJobOptions.PythiaEvgenConfig import evgenConfig
#evgenConfig.efficiency = 0.9
