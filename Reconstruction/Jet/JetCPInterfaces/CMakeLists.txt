# $Id: CMakeLists.txt 779385 2016-10-20 11:16:26Z krasznaa $
################################################################################
# Package: JetCPInterfaces
################################################################################

# Declare the package name:
atlas_subdir( JetCPInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Reconstruction/Jet/JetInterface
   Event/xAOD/xAODJet )	

# Declare an interface library for the package's header(s):
atlas_add_library( JetCPInterfaces
   JetCPInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS JetCPInterfaces
   LINK_LIBRARIES PATInterfaces JetInterface xAODJet )
