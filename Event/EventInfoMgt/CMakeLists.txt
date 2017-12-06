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
                          Control/CxxUtils
                          Control/SGTools
                          Control/StoreGate
			  Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/IOVDbMetaDataTools
                          Event/EventInfo )

# Component(s) in the package:
atlas_add_component( EventInfoMgt
                     src/TagInfoMgr.cxx
                     src/EventInfoMgt_entries.cxx
                     LINK_LIBRARIES AthenaKernel GaudiKernel CxxUtils SGTools StoreGateLib SGtests AthenaBaseComps AthenaPoolUtilities EventInfo )

# Install files from the package:
atlas_install_headers( EventInfoMgt )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/EventInfoMgt_jobOptions.py share/TagInfoMgr_jobOptions.py )
atlas_install_scripts( share/dumpRunNumber.py )

