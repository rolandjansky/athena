################################################################################
# Package: MuonCondCool
################################################################################

# Declare the package name:
atlas_subdir( MuonCondCool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/MinimalRunTime
                          Control/StoreGate
                          GaudiKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondData
                          PRIVATE
                          Control/AthenaKernel
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondInterface
                          MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_component( MuonCondCool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel MuonCondData AthenaKernel MuonCondInterface MuonCondSvcLib MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( MuonCondCool )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
