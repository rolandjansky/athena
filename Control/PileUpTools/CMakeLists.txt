################################################################################
# Package: PileUpTools
################################################################################

# Declare the package name:
atlas_subdir( PileUpTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/SGTools
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCnvInterfaces
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( PileUpToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS PileUpTools
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel
                   AthLinks SGTools
                   EventInfo xAODEventInfo xAODCnvInterfaces
                   GaudiKernel StoreGateLib SGtests ) 

atlas_add_component( PileUpTools
                     src/components/*.cxx
                     LINK_LIBRARIES PileUpToolsLib )

