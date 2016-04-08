################################################################################
# Package: OutputStreamAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( OutputStreamAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/PersistentDataModel
                          Event/EventInfo
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( OutputStreamAthenaPool
                     src/AthenaPoolOutputStreamTool.cxx
                     src/MakeEventStreamInfo.cxx
                     src/CopyEventStreamInfo.cxx
                     src/MakeInputDataHeader.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGTools StoreGateLib SGtests AthenaPoolCnvSvcLib AthenaPoolUtilities PersistentDataModel EventInfo GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

