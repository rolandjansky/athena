################################################################################
# Package: SGAudSvc
################################################################################

# Declare the package name:
atlas_subdir( SGAudSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/SGMon/SGAudCore
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_component( SGAudSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel SGAudCore GaudiKernel StoreGateLib SGtests )

# Install files from the package:
atlas_install_headers( SGAudSvc )
atlas_install_scripts( share/SGout2dot.py )

# Aliases:
atlas_add_alias( SGout2dot "SGout2dot.py" )

