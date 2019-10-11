#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigCaloMonitorAlgorithm_jobOptions.py
@author E. Bergeaas Kuutmann
@date 2019-04-16
@brief Example trigger python configuration for the Run III AthenaMonitoring package (old jobOptions), based on the example by C Burton and P Onyisi
'''

# The following class will make a sequence, configure algorithms, and link
# them to GenericMonitoringTools
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "TrigCaloMonitor")

# Adding the TrigCaloMonitor algorithm to the helper
from TrigCaloMonitoring.TrigCaloMonitoringConf import TrigCaloMonitorAlgorithm
trigCaloMonAlg = helper.addAlgorithm(TrigCaloMonitorAlgorithm,'TrigCaloMonAlg')

#trigCaloMonAlg.OutputLevel = DEBUG

topSequence += helper.result()
