# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetFlavorAlgs.py
#
# David Adams
# September 2014
#

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags

print str(jetFlags.truthFlavorTags())

def scheduleCopyTruthParticles(theJob):
  myname = "scheduleCopyTruthParticles: "
  from ParticleJetTools.ParticleJetToolsConf import CopyFlavorLabelTruthParticles
  from ParticleJetTools.ParticleJetToolsConf import CopyTruthPartons

  for ptype in jetFlags.truthFlavorTags():
    algname = "CopyTruthTag" + ptype + "Alg"
    print myname + "Scheduling " + algname
    if ptype == "Partons":
      ctp = CopyTruthPartons(algname)
    else:
      ctp = CopyFlavorLabelTruthParticles(algname)
      ctp.ParticleType = ptype
    ctp.OutputName = "TruthLabel" + ptype
    ctp.PtMin = 5000
    theJob += ctp 
    print ctp
