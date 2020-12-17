############################################################
#
# to use it  
# athena FastCaloSimFromTruthv13WithTrigT1Calo &> athena.log
#
# Date: 2008.04.10
# Created by Marius Groll and Stefan Rieke fo
#
# Use this file if you like to run only the lvl1 calo trigger
# together with the FastCaloSim
#
# So this means that all switches and input and output
# files/streams are defined
# in the python script FastCaloSimFromTruthv14.py
# and in this file are only the trigger relevant quantities set 

doTrigger = True # for example do not run trigger simulation
# if needed to configure trigger
# see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerFlags
include ( "TriggerJobOpts/TriggerFlags.py" )

# run only the TrigT1Calo simulation

TriggerFlags.doLVL1 = True

# the following line is a work arround,
# to fix a problem in the Lvl1TriggerGetter.py, where decision take place,
# which kind of Alogrithm is choosen, to generate the TTs

TriggerFlags.useCaloTTL = False

TriggerFlags.doID   = False
TriggerFlags.doCalo = True
TriggerFlags.doMuon = False

TriggerFlags.doHLT   = False

include ( "FastCaloSim/FastCaloSimFromTruthv14.py" )
