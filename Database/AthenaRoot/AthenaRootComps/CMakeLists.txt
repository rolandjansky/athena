################################################################################
# Package: AthenaRootComps
################################################################################

# Declare the package name:
atlas_subdir( AthenaRootComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaPython
                          Control/DataModelRoot
                          Control/RootUtils
                          Control/StoreGate
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/APR/StorageSvc
                          Database/AthenaRoot/AthenaRootKernel
                          Database/PersistentDataModel
                          Database/AthenaPOOL/PoolSvc
                          Event/EventInfo
			  Event/xAOD/xAODEventInfo
                          GaudiKernel
                          TestPolicy 
			  Control/xAODRootAccess)

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Graf Tree MathCore Hist pthread )
find_package( Xrootd )

# Component(s) in the package:
atlas_add_component( AthenaRootComps
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools AthenaBaseComps AthenaKernel DataModelRoot RootUtils RootUtilsPyROOT StoreGateLib SGtests CollectionBase CollectionUtilities POOLCore PersistencySvc StorageSvc AthenaRootKernel PersistentDataModel EventInfo GaudiKernel xAODRootAccess xAODEventInfo)

# Install files from the package:
atlas_install_headers( AthenaRootComps )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/tests/*.py )
atlas_install_scripts( test/*.sh test/ref.* )


