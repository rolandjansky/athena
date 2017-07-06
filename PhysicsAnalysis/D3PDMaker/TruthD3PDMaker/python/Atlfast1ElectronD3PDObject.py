# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from egammaD3PDMaker.isem_version          import isem_version
from EventCommonD3PDMaker.DRAssociation    import DRAssociation
from D3PDMakerCoreComps.D3PDObject         import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.SimpleAssociation  import SimpleAssociation
from D3PDMakerConfig.D3PDMakerFlags        import D3PDMakerFlags
from TruthD3PDMaker.Atlfast1D3PDMakerFlags import Atlfast1D3PDMakerFlags
from RecExConfig.RecFlags                  import rec
import egammaD3PDMaker
import EventCommonD3PDMaker
import D3PDMakerCoreComps

from ROOT import egammaParameters
from ROOT import egammaPID

Atlfast1ElectronD3PDObject = \
           make_SGDataVector_D3PDObject ('ElectronContainer',
                                         D3PDMakerFlags.ElectronSGKey(),
                                         'el_', 'Atlfast1ElectronD3PDObject')


Atlfast1ElectronD3PDObject.defineBlock (0, 'Kinematics',
                                        EventCommonD3PDMaker.FourMomFillerTool,
                                        WriteE  = True,
                                        WriteEt = True,
                                        WriteRect = True)
Atlfast1ElectronD3PDObject.defineBlock (0, 'Charge',
                                        EventCommonD3PDMaker.ChargeFillerTool)

if rec.doTruth():
    import TruthD3PDMaker.MCTruthClassifierConfig
    Atlfast1ElectronD3PDObject.defineBlock (1, 'TruthClassification',
                                            egammaD3PDMaker.egammaTruthClassificationFillerTool)
    Atlfast1ElectronGenPartAssoc = SimpleAssociation \
        (Atlfast1ElectronD3PDObject,
         egammaD3PDMaker.egammaGenParticleAssociationTool,
         prefix = 'truth_',
         matched = 'matched',
         blockname = 'Truth')
    Atlfast1ElectronGenPartAssoc.defineBlock (0, 'Truth',
                                              EventCommonD3PDMaker.GenParticleFillerTool,
                                              WriteE = True,
                                              WriteM = False)
    Atlfast1ElectronGenPartAssoc.defineBlock (0, 'TruthBrem',
                                              EventCommonD3PDMaker.GenParticleBremFillerTool)

############################################################################
# Check isolation via associations
#
Atlfast1EleIsoD3PDAssoc = DRAssociation (Atlfast1ElectronD3PDObject,
                                         'ElectronContainer',
                                         Atlfast1D3PDMakerFlags.IsoElectronSGKey(),
                                         0.2,
                                         'iso_',
                                         level = 0,
                                         blockname = 'Isolated')

############################################################################
# Jet associations
#

Atlfast1EleJetD3PDAssoc = DRAssociation (Atlfast1ElectronD3PDObject,
                                         'JetCollection',
                                         D3PDMakerFlags.JetSGKey(),
                                         0.2,
                                         'jet_',
                                         level = 2,
                                         blockname = 'JetMatch')
Atlfast1EleJetD3PDAssoc.defineBlock (2, 'JetKinematics',
                                     EventCommonD3PDMaker.FourMomFillerTool,
                                     WriteE = True)


if rec.doTruth():
    Atlfast1JetTruthJetD3PDAssoc = DRAssociation (Atlfast1EleJetD3PDAssoc,
                                                  'JetCollection',
                                                  D3PDMakerFlags.TruthJetSGKey(),
                                                  0.2,
                                                  'truth_',
                                                  level = 2,
                                                  blockname = 'TrueJetMatch')
    Atlfast1JetTruthJetD3PDAssoc.defineBlock (2, 'TrueJetKinematics',
                                              EventCommonD3PDMaker.FourMomFillerTool,
                                              WriteE = True)
