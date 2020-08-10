################################################################################
# Package: egammaInterfaces
################################################################################

# Declare the package name:
atlas_subdir( egammaInterfaces )


# Component(s) in the package:
atlas_add_library( egammaInterfacesLib
                   egammaInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS egammaInterfaces
                   LINK_LIBRARIES GaudiKernel TrkCaloExtension TrkEventPrimitives TrkNeutralParameters TrkParameters
                   TrkTrack egammaRecEvent xAODCaloEvent xAODEgamma xAODTracking)

atlas_add_dictionary( egammaInterfacesDict
                      egammaInterfaces/egammaInterfacesDict.h
                      egammaInterfaces/selection.xml
                      LINK_LIBRARIES egammaInterfacesLib )
