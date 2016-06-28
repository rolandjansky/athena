################################################################################
# Package: TrigFTKByteStream
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKByteStream )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Tools/PyJobTransforms
                          GaudiKernel
                          Trigger/TrigFTK/TrigFTK_RawData
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/StoreGate
                          TestPolicy )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( TrigFTKByteStream
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TrigFTK_RawData TestTools AthenaKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( TrigFTKByteStream )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
