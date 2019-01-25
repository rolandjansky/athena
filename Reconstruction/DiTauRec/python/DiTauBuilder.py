# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import os

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

from RecExConfig.Configured import Configured

# global tauRec config keys
_outputKey = "DiTauJets"
_outputAuxType = "xAOD::DiTauJetAuxContainer"
_outputAuxKey = "DiTauJetsAux."
_jet_container = "AntiKt10LCTopoJets"


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
        from DiTauRec.DiTauRecFlags import diTauFlags
        from DiTauRec.DiTauRecConf import DiTauBuilder

        tools = []
        tools.append(DiTauAlgs.getSeedJetBuilder(_jet_container))
        tools.append(DiTauAlgs.getElMuFinder())
        tools.append(DiTauAlgs.getSubjetBuilder())
        from InDetRecExample.InDetJobProperties import InDetFlags
        from JetRec.JetRecFlags import jetFlags
        if (InDetFlags.doVertexFinding() and jetFlags.useTracks()) or diTauFlags.doVtxFinding:
            tools.append(DiTauAlgs.getVertexFinder())
            pass
        tools.append(DiTauAlgs.getDiTauTrackFinder())
        if diTauFlags.doCellFinding:
            tools.append(DiTauAlgs.getCellFinder(self.write_jet_cells, self.write_subjet_cells))
            pass

        if not diTauFlags.doCellFinding:
            self.use_cells = False
        tools.append(DiTauAlgs.getIDVarCalculator(self.use_cells))

        # ---------------------------------------------------------------
        # add DiTauBuilder to Algorithm Sequence
        # ---------------------------------------------------------------
        DiTauBuilder = DiTauBuilder(
            name=self.name,
            DiTauContainer=_outputKey,
            Tools=tools,
            SeedJetName=_jet_container,
            minPt=diTauFlags.diTauRecJetSeedPt(),
            maxEta=2.5,
            OutputLevel=3,
            Rjet=self.R_jet,
            Rsubjet=self.R_subjet,
            Rcore=self.R_core)

        topSequence += DiTauBuilder
        #print topSequence
        return True
