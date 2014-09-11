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
  from ParticleJetTools.ParticleJetToolsConf import CopyTruthParticles
  for ptype in jetFlags.truthFlavorTags():
    ctp = CopyTruthParticles("CopyTruthTag" + ptype + "Alg")
    print myname + "Scheduling " + ctp.name()
    ctp.OutputName = "TruthLabel" + ptype
    ctp.ParticleType = ptype
    ctp.PtMin = 50
    theJob += ctp 
    print ctp
