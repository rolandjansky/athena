#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigMETMonitorAlgorithm_jobOptions.py
@author K. Hamano
@author G. Gallardo
@date 2019-06-13
@brief MET Trigger python configuration for Run 3 AthenaMonitoring package (with old job options)
'''

# The following class will make a sequence, configure algorithms, and link
# them to GenericMonitoringTools
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "TrigMETMonitor")

### STEP 2 ###
# Adding an algorithm to the helper. Here, we will use the example 
# algorithm in the AthenaMonitoring package. Just pass the type to the 
# helper. Then, the helper will instantiate an instance and set up the 
# base class configuration following the inputFlags. The returned object 
# is the algorithm.
from TrigMETMonitoring.TrigMETMonitoringConf import TrigMETMonitorAlgorithm
trigMETMonAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'TrigMETMonAlg')

# Examples of setting a trigger, or of running with debug messages on
#trigMETMonAlg.TriggerChain = 'HLT_xe30_cell_L1XE10'
#trigMETMonAlg.OutputLevel = DEBUG

myGroup = helper.addGroup( trigMETMonAlg,"TrigMETMonitor","HLT/METMon/Expert")

myGroup.defineHistogram("lumiPerBCID;lumiPerBCID", title="Luminosity;L/BCID;Events",
                        path='lumi',xbins=10,xmin=0.0,xmax=10.0)
myGroup.defineHistogram("lb;lb", title="Luminosity Block;lb;Events",
                        path='lbpath',xbins=1000,xmin=-0.5,xmax=999.5)
myGroup.defineHistogram("random;random", title="LB;x;Events",
                        path='randompath',xbins=30,xmin=0,xmax=1)

topSequence += helper.result()
