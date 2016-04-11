################################################################################
# Package: ByteStreamTest
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          GaudiKernel
                          TestPolicy )

# Component(s) in the package:
atlas_add_component( ByteStreamTest
                     src/EvenEventsSelectorTool.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )

