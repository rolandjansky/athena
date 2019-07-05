################################################################################
# Package: MuonCnvToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonCnvToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          DetectorDescription/Identifier
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel )

# External dependencies:
find_package( tdaq-common )
find_package( ROOT )

# Install files from the package:
atlas_install_headers( MuonCnvToolInterfaces )

atlas_add_dictionary( MuonCnvToolInterfacesDict
                      MuonCnvToolInterfaces/MuonCnvToolInterfacesDict.h
                      MuonCnvToolInterfaces/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )

