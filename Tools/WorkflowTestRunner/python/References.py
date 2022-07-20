# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#####
# CI Reference Files Map
#####

# The top-level directory for the files is /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/
# Then the subfolders follow the format branch/test/version, i.e. for s3760 in master the reference files are under
# /eos/atlas/atlascerngroupdisk/data-art/grid-input/WorkflowReferences/master/s3760/v1 for v1 version

# Format is "test" : "version"
references_map = {
    # Simulation
    "s3759": "v2",
    "s3760": "v2",
    "s3779": "v1",
    # Overlay
    "d1590": "v2",
    "d1726": "v3",
    "d1759": "v2",
    # Reco
    "q442": "v4",
    "q445": "v12",
    "q449": "v6",
}
