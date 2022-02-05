# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Files for use in configuration unit tests

class defaultTestFiles:
    import os
    d = os.environ.get ("ATLAS_REFERENCE_DATA",
                        "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art")
    EVNT = [f"{d}/SimCoreTests/mcatnlojimmy_ttbar_leptonfilter.19.2.5.37.EVNT.pool.root"] #find an official file.
    HITS_RUN2 = [f"{d}/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3681/HITS.25836812._004813.pool.root.1"]
    HITS_RUN2_MINBIAS_HIGH = [
        f"{d}/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106512._000149.pool.root.1",
        f"{d}/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106512._000581.pool.root.1",
        f"{d}/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106512._000717.pool.root.1",
    ]
    HITS_RUN2_MINBIAS_LOW = [
        f"{d}/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106626._000068.pool.root.1",
        f"{d}/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106626._000480.pool.root.1",
        f"{d}/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/HITS_FILT.26106626._000574.pool.root.1",
    ]
    HITS_DATA_OVERLAY = [f"{d}/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root"]
    RAW = [f"{d}/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    RAW_BKG = [f"{d}/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1"]
    RDO_RUN2 = [f"{d}/WorkflowReferences/master/q443/v1/myRDO.pool.root"]
    RDO_BKG_RUN2 = [f"{d}/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20_13TeV.900149.PG_single_nu_Pt50.digit.RDO.e8307_s3482_s3136_d1715/RDO.26811908._031801.pool.root.1"]
    ESD = [f"{d}/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    AOD = [f"{d}/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1"]
    AOD_MC = [f"{d}/CommonInputs/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1"]
