# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#####
# CI Reference Files Map
#####

# The top-level directory for the files is /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/
# Then the subfolders follow the format branch/test/version, i.e. for s3760 in master the reference files are under
# /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/master/s3760/v1 for v1 version

# Format is "test-branch" : "version"
references_map = {
    # Simulation
    "s3759": "v3",
    "s3760": "v1",
    "s3779": "v2",
    # Overlay
    "d1590": "v1",
    "d1726": "v1",
}
