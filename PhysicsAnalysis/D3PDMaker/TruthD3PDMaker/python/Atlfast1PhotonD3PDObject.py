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


Atlfast1PhotonD3PDObject = \
           make_SGDataVector_D3PDObject ('PhotonContainer',
                                         D3PDMakerFlags.PhotonSGKey(),
                                         'ph_', 'Atlfast1PhotonD3PDObject')


Atlfast1PhotonD3PDObject.defineBlock (0, 'Kinematics',
                                      EventCommonD3PDMaker.FourMomFillerTool,
                                      WriteE  = True,
                                      WriteEt = True,
                                      WriteRect = True)

if rec.doTruth():
    import TruthD3PDMaker.MCTruthClassifierConfig
    Atlfast1PhotonD3PDObject.defineBlock (1, 'TruthClassification',
                                          egammaD3PDMaker.egammaTruthClassificationFillerTool)
    Atlfast1PhotonGenPartAssoc = SimpleAssociation \
        (Atlfast1PhotonD3PDObject,
         egammaD3PDMaker.egammaGenParticleAssociationTool,
         prefix = 'truth_',
         matched = 'matched',
         blockname = 'Truth',
         DRVar = 'deltaRRecPhoton')
    Atlfast1PhotonGenPartAssoc.defineBlock (0, 'Truth',
                                            EventCommonD3PDMaker.GenParticleFillerTool,
                                            WriteE = True,
                                            WriteM = False)

############################################################################
# Check isolation via associations
#
Atlfast1PhoIsoD3PDAssoc = DRAssociation (Atlfast1PhotonD3PDObject,
                                         'PhotonContainer',
                                         Atlfast1D3PDMakerFlags.IsoPhotonSGKey(),
                                         0.2,
                                         'iso_',
                                         level = 0,
                                         blockname = 'Isolated')

############################################################################
# Jet associations
#

Atlfast1PhotonJetD3PDAssoc = DRAssociation (Atlfast1PhotonD3PDObject,
                                            'JetCollection',
                                            D3PDMakerFlags.JetSGKey(),
                                            0.2,
                                            'jet_',
                                            level = 2,
                                            blockname = 'JetMatch')
Atlfast1PhotonJetD3PDAssoc.defineBlock (2, 'JetKinematics',
                                        EventCommonD3PDMaker.FourMomFillerTool,
                                        WriteE = True)


if rec.doTruth():
    Atlfast1JetTruthJetD3PDAssoc = DRAssociation (Atlfast1PhotonJetD3PDAssoc,
                                                  'JetCollection',
                                                  D3PDMakerFlags.TruthJetSGKey(),
                                                  0.2,
                                                  'truth_',
                                                  level = 2,
                                                  blockname = 'TrueJetMatch')
    Atlfast1JetTruthJetD3PDAssoc.defineBlock (2, 'TrueJetKinematics',
                                              EventCommonD3PDMaker.FourMomFillerTool,
                                              WriteE = True)
