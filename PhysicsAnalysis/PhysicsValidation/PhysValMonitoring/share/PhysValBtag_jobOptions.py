from PhysValMonitoring.PhysValUtils import getHistogramDefinitions
import os
from RecExConfig.RecFlags import rec as recFlags
import ROOT

from JetTagDQA.JetTagDQAConf import JetTagDQA__PhysValBTag
tool1 = JetTagDQA__PhysValBTag()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10
tool1.isData = not recFlags.doTruth()
tool1.JetPtCutTtbar = 20000
tool1.JetPtCutZprime = 500000
tool1.JVTCutAntiKt4EMTopoJets = 0.59
tool1.JVTCutLargerEtaAntiKt4EMTopoJets = 0.11
tool1.JVTCutAntiKt4EMPFlowJets = 0.2
tool1.truthMatchProbabilityCut = 0.75

path = ROOT.PathResolver.find_file( 'JetTagDQA/PhysValBtag_VariablesMenu.json', 'DATAPATH' )
tool1.HistogramDefinitionsVector = getHistogramDefinitions(path, 'PHYSVAL', 'ALL')

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.AthenaMonTools += [ tool1 ]
