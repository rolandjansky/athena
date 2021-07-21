################################################################################
#Package : HGTD_FastDigitization
################################################################################

#Declare the package name:
atlas_subdir( HGTD_FastDigitization )

find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_component( HGTD_FastDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS HGTD_FastDigitization ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}
                     ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel
                     HGTD_Identifier HGTD_ReadoutGeometry TrkTruthData
                     PileUpToolsLib HitManagement InDetSimEvent InDetPrepRawData
                     HGTD_PrepRawData AthenaKernel )

atlas_install_headers( HGTD_FastDigitization )
atlas_install_python_modules( python/*.py)
atlas_install_joboptions( share/*.py )
