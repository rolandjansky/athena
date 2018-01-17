################################################################################
# Package: JetTagInfo
################################################################################

# Declare the package name:
atlas_subdir( JetTagInfo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Event/EventPrimitives
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JetTagInfo
                   src/*.cxx
                   PUBLIC_HEADERS JetTagInfo
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks EventPrimitives JetEvent muonEvent Particle egammaEvent VxVertex
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( JetTagInfoDict
                      JetTagInfo/JetTagInfoDict.h
                      JetTagInfo/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthLinks EventPrimitives JetEvent muonEvent Particle egammaEvent VxVertex GaudiKernel JetTagInfo )

