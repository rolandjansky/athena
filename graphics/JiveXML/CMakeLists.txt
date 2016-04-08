################################################################################
# Package: JiveXML
################################################################################

# Declare the package name:
atlas_subdir( JiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          TestPolicy )

# Component(s) in the package:
atlas_add_library( JiveXMLLib
                   src/*.cxx
                   PUBLIC_HEADERS JiveXML
                   LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES EventInfo xAODEventInfo )

atlas_add_component( JiveXML
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests EventInfo xAODEventInfo JiveXMLLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml test/*.xsl )

