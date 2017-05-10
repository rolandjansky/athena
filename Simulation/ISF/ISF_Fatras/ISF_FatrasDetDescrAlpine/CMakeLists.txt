################################################################################
# Package: ISF_FatrasDetDescrAlpine
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasDetDescrAlpine )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetTrackingGeometryXML
                          Simulation/ISF/ISF_Fatras/ISF_FatrasDetDescrModel
                          PRIVATE
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkGeometryAlpine )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( ISF_FatrasDetDescrAlpine
                     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS}
		     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} ${XERCESC_LIBRARIES} GaudiKernel InDetTrackingGeometryXMLLib ISF_FatrasDetDescrModel TrkGeometry TrkGeometryAlpine )

# Install files from the package:
atlas_install_headers( ISF_FatrasDetDescrAlpine )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

