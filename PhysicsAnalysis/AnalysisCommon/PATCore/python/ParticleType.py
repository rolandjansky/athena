# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ParticleType.py
# Author: Karsten Koeneke <karsten.koeneke@cern.ch>
# 
# Purpose: This file defines the values of the enum PATCore::ParticleType::Type
#          so that they can be used in Python in a fairly transparent way
#
# Usage: 1) import 2) use
#  1) import PATCore.ParticleType
#  2) myTool.dataType = PATCore.ParticleType.ConvertedPhoton

Unknown           = 0
Photon            = 10
UnconvertedPhoton = 11
ConvertedPhoton   = 12
Electron          = 20
Muon              = 30
Tau               = 40
OneProngTau       = 41
ThreeProngTau     = 42
Jet               = 50
GluonJet          = 51
LightQuarkJet     = 52
CharmJet          = 53
BJet              = 54

