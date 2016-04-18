# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetFlavorAlgs.py
#
# David Adams
# September 2014

# Import the jet reconstruction control flags.
from JetRecFlags import jetFlags

print str(jetFlags.truthFlavorTags())

def scheduleCopyTruthParticles():
  myname = "scheduleCopyTruthParticles: "
  from JetRecStandardToolManager import jtm
  if not jtm.haveParticleJetTools: return
  from ParticleJetTools.ParticleJetToolsConf import CopyFlavorLabelTruthParticles
  from ParticleJetTools.ParticleJetToolsConf import CopyBosonTopLabelTruthParticles
  from ParticleJetTools.ParticleJetToolsConf import CopyTruthPartons
  from ParticleJetTools.CopyTruthParticlesAlg import CopyTruthParticlesAlg

  tools = []
  for ptype in jetFlags.truthFlavorTags():
    toolname = "CopyTruthTag" + ptype
    if toolname in jtm.tools:
      print myname + "Skipping previously-defined tool: " + toolname
      print jtm.tools[toolname]
    else:
      print myname + "Scheduling " + toolname
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
      print ctp
      tools.append( ctp )
  return tools
