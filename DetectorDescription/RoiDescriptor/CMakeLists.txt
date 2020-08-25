# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RoiDescriptor )

# Component(s) in the package:
atlas_add_library( RoiDescriptor
                   Root/RoiDescriptor.cxx
                   Root/RoiUtil.cxx
                   PUBLIC_HEADERS RoiDescriptor
                   LINK_LIBRARIES IRegionSelector )

atlas_add_dictionary( RoiDescriptorDict
                      RoiDescriptor/RoiDescriptorDict.h
                      RoiDescriptor/selection.xml
                      LINK_LIBRARIES RoiDescriptor )
