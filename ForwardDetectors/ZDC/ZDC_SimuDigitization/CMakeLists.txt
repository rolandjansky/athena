################################################################################
# Package: ZDC_SimuDigitization
################################################################################

# Declare the package name:
atlas_subdir( ZDC_SimuDigitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          ForwardDetectors/ZDC/ZdcEvent
                          GaudiKernel
                          Simulation/HitManagement
                          PRIVATE
                          DetectorDescription/Identifier )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( ZDC_SimuDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib ZDC_SimEvent ZdcEvent GaudiKernel HitManagement Identifier )

# Install files from the package:
atlas_install_headers( ZDC_SimuDigitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

