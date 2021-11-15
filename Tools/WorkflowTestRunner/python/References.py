# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#####
# CI Reference Files Map
#####

# The top-level directory for the files is /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/
# Then the subfolders follow the format test/branch/version, i.e. for q221 in 21.0 the reference files are under
# /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/q221/21.0/v1 for v1 version

# Format is "test-branch" : "version"
references_map = {
    # qTestsTier0_required-test
    'q221-21.0': 'v4',
    'q431-21.0': 'v2',
    'q221-21.3': 'v1',
    'q431-21.3': 'v1',
    'q221-22.0': 'v1',
    'q431-22.0': 'v1',
    # SimulationTier0Test_required-test
    's3126-21.0': 'v1',
    's3126-21.3': 'v1',
    's3126-21.9': 'v1',
    's3126-22.0': 'v8',
    's3505-21.0': 'v2',
    's3505-21.3': 'v1',
    's3505-21.9': 'v1',
    's3505-22.0': 'v13',
    # OverlayTier0Test_required-test
    'overlay-d1498-21.0': 'v2',
    'overlay-d1498-22.0': 'v38',
    'overlay-d1592-22.0': 'v14',
    'overlay-bkg-21.0': 'v1',
    'overlay-bkg-22.0': 'v4',
    'dataoverlay-d1590-22.0': 'v14',
    'dataoverlay-hits-22.0': 'v1',
}
