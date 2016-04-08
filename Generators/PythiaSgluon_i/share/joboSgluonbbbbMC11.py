
-- 
#########################################
# Adrien Renaud
# Laboratoire de l'Accelerateur Lineaire
# Universite Paris-Sud XI
# Batiment 200
# 91898 Orsay France
# Tel :  +33 1 64 46 89 85
#########################################

from AthenaCommon.AlgSequence import AlgSequence 
topAlg = AlgSequence("TopAlg") 

include ( "MC11JobOptions/MC11_Pythia_Common.py" )


#use external process : qqbar->sgluon sgluon  and gg->sgluon sgluon 
Pythia.PythiaCommand += ["pyinit user pythiasgluon"]

#adding particle to pythia ../share/sgluons.dat
Pythia.addParticle = True

#set the sgluon mass
Pythia.PythiaCommand += ["pydat2 pmas 5100021 1 100"]


#switch off all the sgluon decay channels
pydat3_mdme = "pydat3 mdme "
switch_off=" 1 0"
for IDC in range(5065,5067):
   c = pydat3_mdme + str(IDC)
   c += switch_off
   Pythia.PythiaCommand += [c]
   c = ""

# and turn on the good one
Pythia.PythiaCommand += ["pydat3 mdme 5065 1 1"]

# Tauola
include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
# Photos
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )


from MC11JobOptions.PythiaEvgenConfig import evgenConfig
evgenConfig.efficiency = 0.9





# ###############################################################
# #
# # JOB OPTIONS FILE TO USE PYTHIASGLUON_i
# #==============================================================

# #--------------------------------------------------------------
# # Configuration for EvgenJobTransforms
# #--------------------------------------------------------------
# from MC11JobOptions.EvgenConfig import evgenConfig
# evgenConfig.generators += [ "Pythia" ]
# evgenConfig.minevents = 100


# #--------------------------------------------------------------
# # General Application Configuration options
# #--------------------------------------------------------------
# from AthenaCommon.AppMgr import ServiceMgr
# from AthenaCommon.AppMgr import theApp
# from AthenaCommon.AlgSequence import AlgSequence
# from AthenaCommon import CfgMgr
# topAlg = AlgSequence("TopAlg")


# #--------------------------------------------------------------
# # SVC Configuration options
# #--------------------------------------------------------------
# if not hasattr(ServiceMgr, "MessageSvc"):
   # ServiceMgr += CfgMgr.MessageSvc()
# ServiceMgr.MessageSvc.OutputLevel = INFO

# from AthenaServices.AthenaServicesConf import AtRndmGenSvc
# ServiceMgr += AtRndmGenSvc()
# ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA 872025 1702946", "PYTHIA_INIT 9691 890732", "EVTGEN 732814 39893651"]


# #--------------------------------------------------------------
# # Event related parameters
# #--------------------------------------------------------------
# theApp.EvtMax = 100


# #--------------------------------------------------------------
# # Algorithms Private Options
# #--------------------------------------------------------------
# from Pythia_i.Pythia_iConf import Pythia
# topAlg += Pythia()
# Pythia = topAlg.Pythia

# # mSUGRA and his parameters via pythia approximate RGE
# Pythia.PythiaCommand += ["pymssm imss 1 2"]
# Pythia.PythiaCommand += [
   # "pymssm rmss 8 100.",         #m0
   # "pymssm rmss 1 300.",         #m1/2
   # "pymssm rmss 16 -300.",       #A0
   # "pymssm rmss 5 6.",           #tanBeta
   # "pymssm rmss 4 1",            #signmu
   # ]

# #use external process : qqbar->sgluon sgluon  and gg->sgluon sgluon 
# Pythia.PythiaCommand += ["pyinit user pythiasgluon"]

# #adding particle to pythia ../share/sgluons.dat
# Pythia.addParticle = True

# #set the sgluon mass
# Pythia.PythiaCommand += ["pydat2 pmas 5100021 1 225"]

# #pylist(1) for event from -1 to -1 
# Pythia.PythiaCommand += ["pyinit pylistf 1","pyinit dumpr 1 1"]

# #pylist(x) ie: dump particle data base, x = 0 for no list, x =12 for complete list 
# Pythia.PythiaCommand += ["pyinit pylisti 12"]

# #switch off all the sgluon decay channels
# pydat3_mdme = "pydat3 mdme "
# switch_off=" 1 0"
# for IDC in range(5065,5067):
   # c = pydat3_mdme + str(IDC)
   # c += switch_off
   # Pythia.PythiaCommand += [c]
   # c = ""

# # and turn on the good one
# Pythia.PythiaCommand += ["pydat3 mdme 5067 1 1"]
# Pythia.useAtlasPythiaRecomm=True


# #----------------------------------------------------------------
# # Add POOL persistency
# #----------------------------------------------------------------
# from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
# Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
# Stream1.ItemList += [ "2101#*", "133273#*" ]
# #Stream1.OutputFile = "./sgluon.PYTHIA_SGLUON__gluglu_M225_7tev.evgen.EVNT._0001.pool.root"

# #==============================================================
# #
# # End of job options file
# #
# ###############################################################
