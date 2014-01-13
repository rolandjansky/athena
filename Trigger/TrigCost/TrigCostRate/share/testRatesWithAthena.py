
#
# Parse command line options
#    
import TrigCostBase.TrigCostBaseConfig as baseConfig
import TrigCostRate.TrigCostRateConfig as rateConfig

baseConfig.loadTrigCostBase()

import ROOT, PyCintex

ROOT.gSystem.Load('libTrigCostRate')
ROOT.gSystem.Load('libTrigCostExec')

run = rateConfig.RunModule()
run.Execute('RunModule.xml')

import sys
sys.exit(0)
