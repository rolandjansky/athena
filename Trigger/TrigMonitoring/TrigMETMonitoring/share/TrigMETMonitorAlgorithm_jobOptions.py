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
#from AthenaMonitoring import AthMonitorCfgHelper
#helper = AthMonitorCfgHelper(DQMonFlags, "TrigMETMonitor")

### STEP 2 ###
# Adding an algorithm to the helper. Here, we will use the example 
# algorithm in the AthenaMonitoring package. Just pass the type to the 
# helper. Then, the helper will instantiate an instance and set up the 
# base class configuration following the inputFlags. The returned object 
# is the algorithm.
from TrigMETMonitoring.TrigMETMonitoringConf import TrigMETMonitorAlgorithm
expertTrigMETMonAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'TrigMETMonAlg')
shifterTrigMETMonAlg = helper.addAlgorithm(TrigMETMonitorAlgorithm,'ShifterTrigMETMonAlg')

# Examples of setting a trigger, or of running with debug messages on
#expertTrigMETMonAlg.TriggerChain = 'HLT_xe30_cell_L1XE10'
#expertTrigMETMonAlg.OutputLevel = DEBUG

expertGroup = helper.addGroup(expertTrigMETMonAlg,'TrigMETMonitor','HLT/METMon/Expert/')
shifterGroup = helper.addGroup(shifterTrigMETMonAlg,'TrigMETMonitor','HLT/METMon/Shifter/')

shifterGroup.defineHistogram('L1_Ex',title='L1 Missing E_{x};E_{x} (GeV);Events',
                             path='L1',xbins=199,xmin=-298.5,xmax=298.5)
shifterGroup.defineHistogram('L1_Ey',title='L1 Missing E_{y};E_{y} (GeV);Events',
                             path='L1',xbins=199,xmin=-298.5,xmax=298.5)
shifterGroup.defineHistogram('L1_Et',title='L1 Missing E_{T};E_{T} (GeV);Events',
                             path='L1',xbins=205,xmin=-13.5,xmax=401.5)
shifterGroup.defineHistogram('cell_Ex',title='cell Missing E_{x};E_{x} (GeV);Events',
                             path='cell',xbins=199,xmin=-298.5,xmax=298.5)
shifterGroup.defineHistogram('cell_Ey',title='cell Missing E_{y};E_{y} (GeV);Events',
                             path='cell',xbins=199,xmin=-298.5,xmax=298.5)
shifterGroup.defineHistogram('cell_Et',title='cell Missing E_{T};E_{T} (GeV);Events',
                             path='cell',xbins=205,xmin=-13.5,xmax=401.5)
expertGroup.defineHistogram('mht_Ex',title='mht Missing E_{x};E_{x} (GeV);Events',
                            path='mht',xbins=199,xmin=-298.5,xmax=298.5)
expertGroup.defineHistogram('mht_Ey',title='mht Missing E_{y};E_{y} (GeV);Events',
                            path='mht',xbins=199,xmin=-298.5,xmax=298.5)
expertGroup.defineHistogram('mht_Et', title='mht E_{T};E_{T} (GeV);Events',
                            path='mht',xbins=205,xmin=-13.5,xmax=401.5)

topSequence += helper.result()
