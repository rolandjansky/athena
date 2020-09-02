################################################################################
# Package: ForwardRegion_EventTPCnv
################################################################################

# Declare the package name:
atlas_subdir( ForwardRegion_EventTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          ForwardDetectors/ForwardSimulation/ForwardRegion_SimEv
                          PRIVATE
                          AtlasTest/TestTools
                          DetectorDescription/Identifier
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ForwardRegion_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegion_EventTPCnv
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ForwardRegion_SimEv AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_dictionary( ForwardRegion_EventTPCnvDict
                      ForwardRegion_EventTPCnv/SimulationHitTPCnvDict.h
                      ForwardRegion_EventTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolCnvSvcLib ForwardRegion_SimEv Identifier GaudiKernel ForwardRegion_EventTPCnv )



atlas_add_test( SimulationHitCnv_p1_test
                SOURCES
                test/SimulationHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ForwardRegion_EventTPCnv TestTools )

                    
