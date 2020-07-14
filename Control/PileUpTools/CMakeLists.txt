################################################################################
# Package: PileUpTools
################################################################################

# Declare the package name:
atlas_subdir( PileUpTools )

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

