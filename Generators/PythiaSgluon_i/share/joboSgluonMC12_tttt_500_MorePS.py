include ( "MC12JobOptions/Pythia_AUET2B_CTEQ6L1_Common.py" )
include ( "MC12JobOptions/Pythia_CTEQ6L1_MorePS_Common.py")

#-----------------------------------------------------------------------------
# Configuration for Process: gg,qqbar->sgluon sgluon and sgluon -> tt
#-----------------------------------------------------------------------------
ecm4sgluon=open('./sgluonecm.dat', 'w')
print >> ecm4sgluon, runArgs.ecmEnergy
ecm4sgluon.close()

topAlg.Pythia.PythiaCommand += ["pyinit user pythiasgluon"]

#adding particle to pythia Generators/PythiaSgluon_i/share/sgluons.dat
topAlg.Pythia.addParticle = True

#set the sgluon mass
topAlg.Pythia.PythiaCommand += ["pydat2 pmas 5100021 1 500"]

#switch off all the sgluon decay channels
pydat3_mdme = "pydat3 mdme "
switch_off=" 1 0"
for IDC in range(5065,5068):
    c = pydat3_mdme + str(IDC)
    c += switch_off
    topAlg.Pythia.PythiaCommand += [c]
    c = ""
    
# and turn on the good one
topAlg.Pythia.PythiaCommand += ["pydat3 mdme 5066 1 1"]
#--------------------------------------------------------------------------
# end configuration for Process gg,qqbar->sgluon sgluon and sgluon -> tt
#--------------------------------------------------------------------------

# ... Tauola
include ( "MC12JobOptions/Pythia_Tauola.py" )

# ... Photos
#include ( "MC12JobOptions/Photos_Fragment.py" )

evgenConfig.description = "process qqbar and gg -> sgluon sgluon with sgluon -> tt"

