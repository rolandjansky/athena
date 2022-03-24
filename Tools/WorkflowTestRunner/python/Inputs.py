# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# corresponding EOS path: /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences
references_CVMFS_path = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/WorkflowReferences"

#####
# CI special input files
#####
from .Test import WorkflowRun
# common
input_HITS = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3681/HITS.25836812._004813.pool.root.1",
}
input_HITS_minbias_low = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/*",
}
input_HITS_minbias_high = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/*",
}
input_HITS_neutrino = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.900149.PG_single_nu_Pt50.simul.HITS.e8307_s3482/HITS.24078104._234467.pool.root.1",
}

# simulation
input_EVNT = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1",
    WorkflowRun.Run3: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1",
    WorkflowRun.Run4: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/EVNT/mc15_14TeV.422036.ParticleGun_single_mu_Pt100.evgen.EVNT.e5286/EVNT.09244578._000001.pool.root.1",
}
input_EVNT_AF3 = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc15_13TeV.ParticleGun.EVNT.pool.root",
}

# overlay
input_HITS_MC_overlay = {
     WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.424000.ParticleGun_single_mu_Pt100.simul.HITS.e3580_s3126/HITS.11330296._000376.pool.root.1",
}
input_RDO_BKG = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20_13TeV.900149.PG_single_nu_Pt50.digit.RDO.e8307_s3482_s3136_d1715/RDO.26811908._031801.pool.root.1",
}
input_HITS_data_overlay = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/22.0/v1/HITS.pool.root",
}
input_BS_SKIM = {
    WorkflowRun.Run2: "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1",
}
