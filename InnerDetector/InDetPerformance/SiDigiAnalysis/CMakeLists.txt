################################################################################
# Package: SiDigiAnalysis
################################################################################

# Declare the package name:
atlas_subdir( SiDigiAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/ReadoutGeometryBase
                          InnerDetector/InDetRawEvent/InDetRawData
			  InnerDetector/InDetDetDesct/InDetIdentifier
			  InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/PixelReadoutGeometry
			  InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
			  Generators/GeneratorObjects
			   )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Eigen )
find_package( HepMC )

# Component(s) in the package:
atlas_add_component( SiDigiAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps GeneratorObjects
                     PRIVATE_LINK_LIBRARIES ReadoutGeometryBase InDetRawData InDetIdentifier InDetReadoutGeometry SCT_ReadoutGeometry PixelReadoutGeometry)

# Install files from the package:
atlas_install_headers( SiDigiAnalysis )
atlas_install_joboptions( share/jobOptions_SiDigiAnalysis.py )
