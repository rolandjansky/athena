################################################################################
# Package: DetDescrCondExample
################################################################################

# Declare the package name:
atlas_subdir( DetDescrCondExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrCondTools
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread )

# Component(s) in the package:
atlas_add_component( DetDescrCondExample
                     src/ReadAlignTrans.cxx
                     src/ReadTopTrans.cxx
                     src/ReadWriteCoolStrFile.cxx
                     src/CoolHistExample.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives Identifier GaudiKernel StoreGateLib SGtests AthenaPoolUtilities DetDescrConditions InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( DetDescrCondExample )
atlas_install_joboptions( share/*.py )

