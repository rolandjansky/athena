################################################################################
# Package: egammaInterfaces
################################################################################

# Declare the package name:
atlas_subdir( egammaInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          GaudiKernel
			              Tracking/TrkEvent/TrkTrack
		      	          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters 
			              Tracking/TrkEvent/TrkCaloExtension
			              Reconstruction/egamma/egammaRecEvent)

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
