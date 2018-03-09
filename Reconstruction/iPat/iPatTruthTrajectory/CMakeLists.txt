################################################################################
# Package: iPatTruthTrajectory
################################################################################

# Declare the package name:
atlas_subdir( iPatTruthTrajectory )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Reconstruction/iPat/iPatInterfaces
                          Reconstruction/iPat/iPatTrackParameters
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Generators/GenAnalysisTools/TruthTools
                          Generators/GenInterfaces
                          Generators/GeneratorObjects
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( iPatTruthTrajectory
                     src/TruthParameters.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel iPatInterfaces iPatTrackParameters TrkExUtils GeneratorObjects TrkSurfaces TrkExInterfaces )

# Install files from the package:
atlas_install_headers( iPatTruthTrajectory )

