# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( METRecoInterface )

# Component(s) in the package:
atlas_add_library( METRecoInterface
                   METRecoInterface/*.h
                   INTERFACE
                   PUBLIC_HEADERS METRecoInterface
                   LINK_LIBRARIES AsgTools xAODMissingET )

atlas_add_dictionary( METRecoInterfaceDict
                      METRecoInterface/METRecoInterfaceDict.h
	                  METRecoInterface/selection.xml
	                  LINK_LIBRARIES METRecoInterface )
