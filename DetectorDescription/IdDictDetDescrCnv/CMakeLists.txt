################################################################################
# Package: IdDictDetDescrCnv
################################################################################

# Declare the package name:
atlas_subdir( IdDictDetDescrCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/RDBAccessSvc
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/DetDescrCnvSvc
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/IdDictParser
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( IdDictDetDescrCnv
                     src/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps CxxUtils SGTools StoreGateLib SGtests AtlasDetDescr DetDescrCnvSvcLib GeoModelUtilities IdDictDetDescr IdDictParser Identifier GaudiKernel )

# Install files from the package:
atlas_install_headers( IdDictDetDescrCnv )
atlas_install_joboptions( share/*.py )

