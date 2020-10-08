# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JiveXML )

# Component(s) in the package:
atlas_add_library( JiveXMLLib
                   src/*.cxx
                   PUBLIC_HEADERS JiveXML
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES EventInfo xAODEventInfo )

atlas_add_component( JiveXML
                     src/components/*.cxx
                     LINK_LIBRARIES JiveXMLLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml test/*.xsl )

