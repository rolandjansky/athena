# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetJvtEfficiencyToolConfig: A helper module for configuring jet jvt  #
# efficiency configurations. This way the derivation framework can     #
# easily be kept up to date with the latest recommendations.           #
# Author: mswiatlo
#                                                                      #
########################################################################

from AnaAlgorithm import Logging
jetjvttoollog = Logging.logging.getLogger('JetJvtEfficiencyToolConfig')

def getJvtEffTool( jetalg , toolname = ''):
  from AthenaCommon import CfgMgr
  if not toolname:
    toolname = getJvtEffToolName( jetalg )

  configs = {
    "AntiKt4EMTopo": "JetJvtEfficiency/Moriond2018/JvtSFFile_EMTopoJets.root",
    "AntiKt4EMPFlow": "JetJvtEfficiency/Moriond2018/JvtSFFile_EMPFlow.root"
  }

  jvtefftool = CfgMgr.CP__JetJvtEfficiency(toolname)
  jvtefftool.SFFile=configs[ jetalg ] 

  jetjvttoollog.info("Configured JetJvtEfficiencyTool {} for jetalg {}".format(toolname, jetalg))

  return jvtefftool

# return the default toolname for the specified alg
def getJvtEffToolName( jetalg ):
  toolname = "JVTEff_{0}".format(jetalg)
  return toolname
