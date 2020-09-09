################################################################################
# Package: MuonAssociationTools
################################################################################

# Declare the package name:
atlas_subdir( MuonAssociationTools )

# Component(s) in the package:
atlas_add_component( MuonAssociationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonRecToolInterfaces Identifier MuonIdHelpersLib MuonPattern MuonPrepRawData MuonSegment TrkPrepRawData TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonAssociationTools )

