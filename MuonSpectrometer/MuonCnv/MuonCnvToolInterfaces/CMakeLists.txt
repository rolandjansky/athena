################################################################################
# Package: MuonCnvToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonCnvToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel )

# External dependencies:
find_package( tdaq-common )

# Install files from the package:
atlas_install_headers( MuonCnvToolInterfaces )

