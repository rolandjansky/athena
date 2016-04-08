################################################################################
# Package: SiDigitization
################################################################################

# Declare the package name:
atlas_subdir( SiDigitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/DataModel
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetSimEvent )

# Component(s) in the package:
atlas_add_library( SiDigitization
                   src/SiChargedDiode.cxx
                   src/SiChargedDiodeCollection.cxx
                   src/SiSurfaceCharge.cxx
                   PUBLIC_HEADERS SiDigitization
                   LINK_LIBRARIES AthenaKernel CxxUtils DataModel Identifier GaudiKernel InDetReadoutGeometry InDetSimEvent )

