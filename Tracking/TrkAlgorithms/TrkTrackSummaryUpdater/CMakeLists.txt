################################################################################
# Package: TrkTrackSummaryUpdater
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackSummaryUpdater )
                
# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )
find_package( ROOT COMPONENTS Core )
          
# Component(s) in the package:
atlas_add_library( TrkTrackSummaryUpdaterLib
                   src/*.cxx
                   test/PutTrackCollectionsInSG.cxx
                   test/SummaryToolStub.cxx
                   PUBLIC_HEADERS TrkTrackSummaryUpdater
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrkTrack TrkEventUtils TrkToolInterfaces TrkTrackSummary
                   PRIVATE_LINK_LIBRARIES TrkPrepRawData )
                   
atlas_add_library( TrkTrackSummaryUpdaterTestLib
                   test/PutTrackCollectionsInSG.cxx
                   test/SummaryToolStub.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES TrkTrackSummaryUpdaterLib 
                   PRIVATE_LINK_LIBRARIES TrkPrepRawData )

# Component(s) in the package:
atlas_add_component( TrkTrackSummaryUpdater
                     src/components/*.cxx
                     LINK_LIBRARIES TrkTrackSummaryUpdaterLib)

atlas_add_test(TrackSummaryUpdater_test
                SOURCES test/TrackSummaryUpdater_test.cxx 
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}  TrkTrackSummaryUpdaterTestLib
                POST_EXEC_SCRIPT "nopost.sh" )

# Install files from the package:
atlas_install_joboptions( share/*.txt )

# Install files from the package:
atlas_install_headers( TrkTrackSummaryUpdater )
