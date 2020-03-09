################################################################################
# Package: EventInfoMgt
################################################################################

# Declare the package name:
atlas_subdir( EventInfoMgt )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
			  Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/IOVDbMetaDataTools
                          Event/EventInfo
                          Event/EventInfoUtils
                          )

# Component(s) in the package:
atlas_add_component( EventInfoMgt
                     src/TagInfoMgr.cxx
                     src/EventInfoMgt_entries.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel 
                     SGTools StoreGateLib SGtests AthenaBaseComps
                     AthenaPoolUtilities EventInfo EventInfoUtils )

# Install files from the package:
atlas_install_headers( EventInfoMgt )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/EventInfoMgt_jobOptions.py )
atlas_install_scripts( share/dumpRunNumber.py )

atlas_add_test( TagInfoMgrCfg
                SCRIPT python -m EventInfoMgt.TagInfoMgrConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
