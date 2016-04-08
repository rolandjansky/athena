################################################################################
# Package: xAODTrigMuonCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMuonCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigMuon
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/DataModel
                          Event/FourMomUtils
                          Trigger/TrigEvent/TrigMuonEvent )

# Component(s) in the package:
atlas_add_component( xAODTrigMuonCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODMuon xAODTracking xAODTrigMuon GaudiKernel AthenaBaseComps AthenaKernel DataModel FourMomUtils TrigMuonEvent )

# Install files from the package:
atlas_install_headers( xAODTrigMuonCnv )
atlas_install_python_modules( python/xAODTrigMuonCnvConfig.py python/xAODTrigMuonCnvConfigDb.py )
atlas_install_joboptions( share/addEFMuonxAODCnv_jobOptions.py share/runEFMuonAODCnvAlg_topOptions.py )

