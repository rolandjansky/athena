# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

from RecExConfig.Configured import Configured

# global tauRec config keys
_outputType = "xAOD::DiTauJetContainer"
_outputKey = "DiTauJets"
_outputAuxType = "xAOD::DiTauJetAuxContainer"
_outputAuxKey = "DiTauJetsAux."
_jet_container = "AntiKt10LCTopoJets"
_R_jet = 1.0                # seed jet radius  ... TODO check correct implementation in tools
_R_subjet = 0.2             # subjet radius  ... TODO check correct implementation in tools
_R_core = 0.1               # subjet core radius  ... TODO check correct implementation in tools
_do_cell_finding = True     # enable cell finder
_write_jet_cells = True     # write seed jet cell information in xAOD
_write_subjet_cells = True  # write subjet cell information in xAOD
_use_cells = True           # use cells in ID variable calculations


class DiTauBuilder(Configured):
    _output = {_outputKey: _outputKey, _outputAuxType: _outputAuxKey}

    def __init__(self, name="DiTauBuilder",
                 ignoreExistingDataObject=True,
                 R_jet=1.0,
                 R_subjet=0.2,
                 R_core=0.1,
                 do_cell_finding=True,
                 write_jet_cells=True,
                 write_subjet_cells=True,
                 use_cells=True):
        self.name = name
        self.R_jet = R_jet
        self.R_subjet = R_subjet
        self.R_core = R_core
        self.do_cell_finding = do_cell_finding
        self.write_jet_cells = write_jet_cells
        self.write_subjet_cells = write_subjet_cells
        self.use_cells = use_cells

        Configured.__init__(self, ignoreExistingDataObject=ignoreExistingDataObject)

    def configure(self):
        # objKeyStore stuff needed?

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        # ---------------------------------------------------------------
        # DiTauRec Tools
        # ---------------------------------------------------------------
        import DiTauRec.DiTauAlgorithmsHolder as DiTauAlgs

        tools = []
        tools.append(DiTauAlgs.getSeedJetBuilder(_jet_container))
        tools.append(DiTauAlgs.getElMuFinder())
        tools.append(DiTauAlgs.getSubjetBuilder())
        from InDetRecExample.InDetJobProperties import InDetFlags
        from JetRec.JetRecFlags import jetFlags
        if InDetFlags.doVertexFinding() and jetFlags.useTracks():
            tools.append(DiTauAlgs.getVertexFinder())
            pass
        tools.append(DiTauAlgs.getDiTauTrackFinder())
        if _do_cell_finding:
            tools.append(DiTauAlgs.getCellFinder(_write_jet_cells, _write_subjet_cells))
            pass
        tools.append(DiTauAlgs.getIDVarCalculator(_use_cells))

        # ---------------------------------------------------------------
        # add DiTauBuilder to Algorithm Sequence
        # ---------------------------------------------------------------
        from DiTauRec.DiTauRecFlags import diTauFlags
        from DiTauRec.DiTauRecConf import DiTauBuilder
        DiTauBuilder = DiTauBuilder(
            name=self.name,
            DiTauContainer=_outputKey,
            DiTauAuxContainer=_outputAuxKey,
            Tools=tools,
            SeedJetName=_jet_container,
            minPt=diTauFlags.diTauRecJetSeedPt(),
            maxEta=2.5,
            OutputLevel=3,
            Rjet=_R_jet,
            Rsubjet=_R_subjet,
            Rcore=_R_core)

        topSequence += DiTauBuilder
        #print topSequence
        return True
