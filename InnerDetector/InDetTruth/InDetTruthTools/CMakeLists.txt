################################################################################
# Package: InDetTruthTools
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          InnerDetector/InDetTruth/InDetTruthInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkTruthTracks/TrkTruthTrackInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRawEvent/InDetSimData
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkTruthData 
                          Generators/AtlasHepMC )

# External dependencies:

# Component(s) in the package:
atlas_add_component( InDetTruthTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps TrkEventPrimitives AtlasDetDescr GaudiKernel InDetIdentifier InDetReadoutGeometry InDetSimData InDetPrepRawData TrkPrepRawData TrkTruthData InDetTruthInterfaces TrkTruthTrackInterfaces )

# Install files from the package:
atlas_install_headers( InDetTruthTools )
atlas_install_joboptions( share/*.py )

