include ( "MC12JobOptions/Pythia_AUET2B_CTEQ6L1_Common.py" )

# ABC For PDF tests only
# ABC include ( "MC12JobOptions/Pythia_Base_Fragment.py" )
# ABC include ( "MC12JobOptions/Pythia_AUET2B_LOxx_Common.py" )

#-----------------------------------------------------------------------------
# Configuration for Process: qqbar->sgluon sgluon and gg->sgluon sgluon

evgenConfig.description = "process qqbar and gg -> sgluon sgluon -> gggg"
evgenConfig.generators=['Pythia']
evgenConfig.keywords=['SUSY','sgluons','jets']
evgenConfig.contact=['claire.bourdarios@cern.ch']

#-----------------------------------------------------------------------------

ecm4sgluon=open('./sgluonecm.dat', 'w')
print >> ecm4sgluon, runArgs.ecmEnergy
ecm4sgluon.close()

topAlg.Pythia.PythiaCommand += ["pyinit user pythiasgluon"]

#adding particle to pythia Generators/PythiaSgluon_i/share/sgluons.dat
topAlg.Pythia.addParticle = True

#set the sgluon mass
topAlg.Pythia.PythiaCommand += ["pydat2 pmas 5100021 1 300"]

#switch off all the sgluon decay channels
pydat3_mdme = "pydat3 mdme "
switch_off=" 1 0"
for IDC in range(5065,5068):
    c = pydat3_mdme + str(IDC)
    c += switch_off
    topAlg.Pythia.PythiaCommand += [c]
    c = ""
    
# and turn on the good one
topAlg.Pythia.PythiaCommand += ["pydat3 mdme 5065 1 1"]

# ... Tauola
include ( "MC12JobOptions/Pythia_Tauola.py" )

