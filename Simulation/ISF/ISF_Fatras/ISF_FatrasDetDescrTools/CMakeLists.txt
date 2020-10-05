################################################################################
# Package: ISF_FatrasDetDescrTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasDetDescrTools )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( ISF_FatrasDetDescrToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ISF_FatrasDetDescrTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel ISF_FatrasDetDescrInterfaces ISF_FatrasDetDescrModel TrkDetDescrInterfaces TrkDetDescrUtils TrkSurfaces StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES InDetIdentifier InDetReadoutGeometry TrkGeometry TrkVolumes )

atlas_add_component( ISF_FatrasDetDescrTools
                     src/components/*.cxx
                     LINK_LIBRARIES ISF_FatrasDetDescrToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
