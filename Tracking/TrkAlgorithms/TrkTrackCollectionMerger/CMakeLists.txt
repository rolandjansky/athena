################################################################################
# Package: TrkTrackCollectionMerger
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackCollectionMerger )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Tracking/TrkEvent/TrkPrepRawData )
                          
find_package( Boost COMPONENTS unit_test_framework )
find_package( Eigen )
find_package( GeoModelCore )


# Component(s) in the package:
atlas_add_component( TrkTrackCollectionMerger
                     src/*.cxx
                     test/PutTrackCollectionsInSG.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkTrack TrkToolInterfaces TrkPrepRawData )
                     
atlas_add_test( TrackCollectionMerger_test
                SOURCES test/TrackCollectionMerger_test.cxx test/PutTrackCollectionsInSG.cxx src/TrackCollectionMerger.cxx 
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
                LINK_LIBRARIES TestTools ${Boost_LIBRARIES} AthenaKernel GaudiKernel SGTools StoreGateLib CxxUtils AthenaBaseComps TrkTrack TrkToolInterfaces
                POST_EXEC_SCRIPT "nopost.sh"
)

atlas_add_test( SimpleTrackCollectionMerger_test
                SOURCES test/SimpleTrackCollectionMerger_test.cxx test/PutTrackCollectionsInSG.cxx src/SimpleTrackCollectionMerger.cxx 
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
                LINK_LIBRARIES TestTools ${Boost_LIBRARIES} AthenaKernel GaudiKernel SGTools StoreGateLib CxxUtils AthenaBaseComps TrkTrack TrkToolInterfaces
                POST_EXEC_SCRIPT "nopost.sh"
)


# Install files from the package:
atlas_install_headers( TrkTrackCollectionMerger )
atlas_install_joboptions( share/*.txt )

