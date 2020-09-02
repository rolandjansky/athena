################################################################################
# Package: AmdcDb
################################################################################

# Declare the package name:
atlas_subdir( AmdcDb )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/RDBAccessSvc
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/Amdcsimrec/AmdcAth
                          MuonSpectrometer/Amdcsimrec/AmdcCore )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AmdcDbLib
                   src/*.cxx
                   PUBLIC_HEADERS AmdcDb
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests AmdcAthLib RDBAccessSvcLib
                   PRIVATE_LINK_LIBRARIES AmdcCore )

atlas_add_component( AmdcDb
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests AmdcAthLib AmdcCore AmdcDbLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( AmdcDbCnvSqlToolKit/* )
