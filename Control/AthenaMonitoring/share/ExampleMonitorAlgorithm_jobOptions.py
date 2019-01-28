#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file ExampleMonitorAlgorithm_jobOptions.py
@author C. D. Burton
@author P. Onyisi
@date 2018-01-11
@brief Example python configuration for the Run III AthenaMonitoring package (old jobOptions)
'''

# The following class will make a sequence, configure algorithms, and link
# them to GenericMonitoringTools
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "ExampleMonitor")

### STEP 2 ###
# Adding an algorithm to the helper. Here, we will use the example 
# algorithm in the AthenaMonitoring package. Just pass the type to the 
# helper. Then, the helper will instantiate an instance and set up the 
# base class configuration following the inputFlags. The returned object 
# is the algorithm.
from AthenaMonitoring.AthenaMonitoringConf import ExampleMonitorAlgorithm
exampleMonAlg = helper.AddAlgorithm(ExampleMonitorAlgorithm, "ExampleMonAlg")

myGroup = helper.AddGroup( exampleMonAlg,
        "ExampleMonitor",
        "OneRing/"
    )

myGroup.defineHistogram("lumiPerBCID;lumiPerBCID", title="Luminosity;L/BCID;Events",
                        path='ToRuleThemAll',xbins=10,xmin=0.0,xmax=10.0)
myGroup.defineHistogram("lb;lb", title="Luminosity Block;lb;Events",
                        path='ToFindThem',xbins=1000,xmin=-0.5,xmax=999.5)
myGroup.defineHistogram("random;random", title="LB;x;Events",
                        path='ToBringThemAll',xbins=30,xmin=0,xmax=1)

topSequence += helper.result()
