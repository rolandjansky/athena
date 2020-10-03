# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Files for use in configuration unit tests

from __future__ import print_function

class defaultTestFiles(object):
    import os
    d = os.environ.get ("ATLAS_REFERENCE_DATA",
                        "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art")
    EVNT= [d + "/SimCoreTests/mcatnlojimmy_ttbar_leptonfilter.19.2.5.37.EVNT.pool.root"] #find an official file.
    HITS= [d + "/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"]
    HITS_SPECIAL = [d + "/DigitizationTests/mc16_13TeV.HITS.16965029._000024.pool.root.1"]
    HITS_DATA_OVERLAY = [d + "/OverlayMonitoringRTT/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.OverlaySim/HITS.pool.root"]
    HITS_MINBIAS_HIGH = [
        d + "/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/HITS.10501933._000005.pool.root.1",
        d + "/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/HITS.10501933._000013.pool.root.1",
        d + "/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/HITS.10501933._000018.pool.root.1",
    ]
    HITS_MINBIAS_LOW = [
        d + "/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000003.pool.root.1",
        d + "/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000024.pool.root.1",
        d + "/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000044.pool.root.1",
    ]
    RAW = [d + "/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    RAW_BKG = [d + "/OverlayMonitoringRTT/mc15_valid.00200010.overlay_streamsAll_2016_pp_1.skim.DRAW.r8381/DRAW.09331084._000146.pool.root.1"]
    RDO = [d + "/Tier0ChainTests/q221/21.0/v1/myRDO.pool.root"]
    RDO_BKG = [d + "/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup-MT.100events.pool.root"]
    ESD = [d + "/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]
    AOD = [d + "/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1"]

