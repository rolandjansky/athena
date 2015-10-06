# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags



# ----------------------------------------------------------------------------
# inputs files (in locations of lxpus)
# ----------------------------------------------------------------------------
isData = False
if isData:
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00202740.00204265.00204564/"

    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00202740.txt.v30_ESD.16770327/"
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00204240.txt.v33_ESD.16770897/"
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00204265.txt.v34_ESD.16771185/"
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00204564.txt.v35_ESD.16771782/"
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00204910.txt.v36_ESD.16772030/"
    folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00204910.txt.v36_ESD.16829861/"
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00205112.txt.v37_ESD.16772152/"
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00205112.txt.v37_ESD.16775485/";
    # folder = "/ZIH.fast/users/kirchmeier/high_pt_data/user.dkirchme.data12_8TeV.00205112.txt.v37_ESD.16777014/"

    files = []
    for (dirpath, dirnames, filenames) in os.walk(folder):
        files.extend(filenames)
        break

    athenaCommonFlags.FilesInput = [folder+f for f in files]

    globalflags.DataSource = 'data'

else:
    # r.20 MC (Drell Yan)
    folder = "/ZIH.fast/users/kirchmeier/ESD_r20/mc15_13TeV.301057.PowhegPythia8EvtGen_AZNLOCTEQ6L1_DYtautau_4500M5000.recon.ESD.e3649_s2650_s2183_r6771_tid05570837_00/"

    files = []
    for (dirpath, dirnames, filenames) in os.walk(folder):
        files.extend(filenames)
        break
    athenaCommonFlags.FilesInput += [folder+f for f in files]



# ----------------------------------------------------------------------------
# RecEx Flags
# ----------------------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(True)  # set true just for testing ... default: False
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)  # TODO: remove this options and write AODs as output
rec.doWriteESD.set_Value_and_Lock(False)


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.SkipEvents=4
athenaCommonFlags.EvtMax = 100



from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DetDescrVersion = inputFileSummary['geometry']
include("RecExCond/AllDet_detDescr.py")
include("RecExCommon/RecExCommon_topOptions.py")



# ----------------------------------------------------------------------------
# Athena algorithm sequence
# ----------------------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


# ----------------------------------------------------------------------------
# Tools
# ----------------------------------------------------------------------------
_jet_container = "AntiKt10LCTopoJets"
_R_jet = 1.0                # seed jet radius  ... TODO check correct implementation in tools
_R_subjet = 0.2             # subjet radius  ... TODO check correct implementation in tools
_R_core = 0.1               # subjet core radius  ... TODO check correct implementation in tools
_do_cell_finding = True     # enable cell finder
_write_jet_cells = True     # write seed jet cell information in xAOD
_write_subjet_cells = True  # write subjet cell information in xAOD
_use_cells = True           # use cells in ID variable calculations

import DiTauRec.DiTauAlgorithmsHolder as DiTauAlgs

tools = []
tools.append(DiTauAlgs.getSeedJetBuilder(_jet_container))
tools.append(DiTauAlgs.getElMuFinder())
tools.append(DiTauAlgs.getSubjetBuilder())
tools.append(DiTauAlgs.getVertexFinder())
tools.append(DiTauAlgs.getDiTauTrackFinder())
if _do_cell_finding:
    tools.append(DiTauAlgs.getCellFinder(_write_jet_cells, _write_subjet_cells))
tools.append(DiTauAlgs.getIDVarCalculator(_use_cells))


# ----------------------------------------------------------------------------
# add DiTauBuilder to Algorithm Sequence
# ----------------------------------------------------------------------------
from DiTauRec.DiTauRecConf import DiTauBuilder
DiTauBuilder = DiTauBuilder(
    name="DiTauBuilder",
    DiTauContainer="DiTau",
    DiTauAuxContainer="DiTauAux.",  # the dot is important
    Tools=tools,
    SeedJetName=_jet_container,
    minPt=15000,
    maxEta=2.5,
    OutputLevel=2,
    Rjet=_R_jet,
    Rsubjet=_R_subjet,
    Rcore=_R_core)

topSequence += DiTauBuilder
print topSequence


# ----------------------------------------------------------------------------
# xAOD output stream
# ----------------------------------------------------------------------------
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStream = MSMgr.NewPoolRootStream("DiTauXAODStream", "DiTauXAOD.pool.root")
# outStream.AddItem(['xAOD::JetContainer#GoodJets'])
# outStream.AddItem(['xAOD::JetAuxContainer#GoodJetsAux.'])
outStream.AddItem(['xAOD::DiTauJetContainer#DiTau'])
outStream.AddItem(['xAOD::DiTauJetAuxContainer#DiTauAux.'])
