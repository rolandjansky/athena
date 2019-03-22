#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#####
# CI Reference Files Map
#####

# The top-level directory for the files is /eos/atlas/atlascerngroupdisk/data-art/grid-input/Tier0ChainTests/
# Then the subfolders follow the format test/branch/version, i.e. for q221 in 21.0 the reference files are under
# /eos/atlas/atlascerngroupdisk/data-art/grid-input/Tier0ChainTests/q221/21.0/v1 for v1 version

# Format is "test-branch" : "version"
ciRefFileMap = {
                # qTestsTier0_required-test
                'q221-21.0'            : 'v1',
                'q431-21.0'            : 'v1',
                'q221-21.3'            : 'v1',
                'q431-21.3'            : 'v1',
                # SimulationTier0Test_required-test
                's3126-21.0'           : 'v1',
                's3126-21.3'           : 'v1',
                's3126-21.9'           : 'v1',
                's3126-22.0'           : 'v3',
                # OverlayTier0Test_required-test
                'overlay-d1498-21.0'   : 'v2',
                'overlay-d1498-22.0'   : 'v12',
                'overlay-bkg-21.0'     : 'v1',
                'overlay-bkg-22.0'     : 'v1',
               }


#####
# CI special input files
#####
# Simulation tests
SimInput = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/ttbar_muplusjets-pythia6-7000.evgen.pool.root"

# Overlay tests
OverlayInputHits = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/mc16_13TeV.424000.ParticleGun_single_mu_Pt100.simul.HITS.e3580_s3126/HITS.11330296._000376.pool.root.1"
OverlayInputBkg = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/{}/{}/RDO.merged-pileup.100events.pool.root"
