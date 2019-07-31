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
			  egamma/egammaRecEvent)

atlas_add_dictionary( egammaInterfacesDict
                      egammaInterfaces/egammaInterfacesDict.h
                      egammaInterfaces/selection.xml
                      INCLUDE_DIRS 
		      LINK_LIBRARIES  xAODCaloEvent xAODEgamma xAODTracking GaudiKernel TrkTrack 
		      TrkEventPrimitives TrkNeutralParameters TrkParameters TrkCaloExtension egammaRecEvent)

# Install files from the package:
atlas_install_headers( egammaInterfaces )

