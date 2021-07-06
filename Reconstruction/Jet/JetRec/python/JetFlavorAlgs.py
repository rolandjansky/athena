# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# JetFlavorAlgs.py
#
# David Adams
# September 2014

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

jetlog.info( str(jetFlags.truthFlavorTags()) )

def scheduleCopyTruthParticles():
  myname = "scheduleCopyTruthParticles: "
  from JetRec.JetRecStandardToolManager import jtm
  if not jtm.haveParticleJetTools: return
  from ParticleJetTools.ParticleJetToolsConf import CopyFlavorLabelTruthParticles
  from ParticleJetTools.ParticleJetToolsConf import CopyBosonTopLabelTruthParticles
  from ParticleJetTools.ParticleJetToolsConf import CopyTruthPartons

  tools = []
  for ptype in jetFlags.truthFlavorTags():
    toolname = "CopyTruthTag" + ptype
    if toolname in jtm.tools:
      jetlog.info( myname + "Skipping previously-defined tool: " + toolname )
      jetlog.info( jtm.tools[toolname] )
    else:
      jetlog.info( myname + "Scheduling " + toolname )
      ptmin = 5000
      if ptype == "Partons":
        ctp = CopyTruthPartons(toolname)
      elif ptype in ["WBosons", "ZBosons", "HBosons", "TQuarksFinal"]:
        ctp = CopyBosonTopLabelTruthParticles(toolname)
        ctp.ParticleType = ptype
        ptmin = 100000
      else:
        ctp = CopyFlavorLabelTruthParticles(toolname)
        ctp.ParticleType = ptype
      ctp.OutputName = "TruthLabel" + ptype
      ctp.PtMin = ptmin
      jtm += ctp
      #theJob += CopyTruthParticlesAlg(ctp, toolname + "Alg")
      jetlog.info( ctp )
      tools.append( ctp )
  return tools
