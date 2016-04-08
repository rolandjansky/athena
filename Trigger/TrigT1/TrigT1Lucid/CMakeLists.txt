################################################################################
# Package: TrigT1Lucid
################################################################################

# Declare the package name:
atlas_subdir( TrigT1Lucid )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          ForwardDetectors/LUCID/LUCID_RawEvent
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_component( TrigT1Lucid
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps LUCID_RawEvent GaudiKernel TrigConfL1Data TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigT1Lucid )
atlas_install_joboptions( share/*.py )

