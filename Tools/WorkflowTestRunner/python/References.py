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
    "s3759": "v10",
    "s3760": "v8",
    "s3779": "v4",
    # Overlay
    "d1590": "v10",
    "d1726": "v6",
    "d1759": "v3",
    # Reco
    "q442": "v4",
    "q445": "v12",
    "q449": "v7",
}
