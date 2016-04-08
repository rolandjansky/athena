################################################################################
# Package: TrigExMTHelloWorldLvl1
################################################################################

# Declare the package name:
atlas_subdir( TrigExMTHelloWorldLvl1 )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          Control/StoreGate
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          GaudiKernel
                          Trigger/TrigT1/TrigT1Result )

# Component(s) in the package:
atlas_add_component( TrigExMTHelloWorldLvl1
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel TrigT1Result )

# Install files from the package:
atlas_install_headers( TrigExMTHelloWorldLvl1 )
atlas_install_joboptions( share/*.py )

