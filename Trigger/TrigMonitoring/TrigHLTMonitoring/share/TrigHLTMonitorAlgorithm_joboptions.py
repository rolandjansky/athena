#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigHLTMonitorAlgorithm_jobOptions.py
@author E. Bergeaas Kuutmann
@date 2019-04-16
@brief Example trigger python configuration for the Run III AthenaMonitoring package (old jobOptions), based on the example by C Burton and P Onyisi
'''

# The following class will make a sequence, configure algorithms, and link
# them to GenericMonitoringTools
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "TrigHLTMonitor")

### STEP 2 ###
# Adding an algorithm to the helper. Here, we will use the example 
# algorithm in the AthenaMonitoring package. Just pass the type to the 
# helper. Then, the helper will instantiate an instance and set up the 
# base class configuration following the inputFlags. The returned object 
# is the algorithm.
from TrigHLTMonitoring.TrigHLTMonitoringConf import TrigHLTMonitorAlgorithm
trigHLTMonAlg = helper.addAlgorithm(TrigHLTMonitorAlgorithm,'TrigHLTMonAlg')

# Examples of setting a trigger, or of running with debug messages on
#trigHLTMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
#trigHLTMonAlg.OutputLevel = DEBUG

myGroup = helper.addGroup( trigHLTMonAlg,
        "TrigHLTMonitor",
        "HLT/HLTMon/Expert"
    )

#myGroup.defineHistogram("lumiPerBCID;lumiPerBCID", title="Luminosity;L/BCID;Events",
#                        path='lumi',xbins=10,xmin=0.0,xmax=10.0)
myGroup.defineHistogram("lb;lb", title="Luminosity Block;lb;Events",
                        path='lbpath',xbins=1000,xmin=-0.5,xmax=999.5)
myGroup.defineHistogram("random;random", title="LB;x;Events",
                        path='randompath',xbins=30,xmin=0,xmax=1)

topSequence += helper.result()
