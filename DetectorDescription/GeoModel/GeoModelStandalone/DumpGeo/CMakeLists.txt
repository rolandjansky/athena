# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( DumpGeo )

# Component(s) in the package.
atlas_add_component( DumpGeo
   DumpGeo/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps CxxUtils GaudiKernel GeoExporter )

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/dump-geo )
