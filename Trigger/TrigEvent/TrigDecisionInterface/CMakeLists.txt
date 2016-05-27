# $Id: CMakeLists.txt 750298 2016-05-27 13:03:48Z krasznaa $
################################################################################
# Package: TrigDecisionInterface
################################################################################

# Declare the package name:
atlas_subdir( TrigDecisionInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools )

# Define an interface library for the header(s):
atlas_add_library( TrigDecisionInterface
   TrigDecisionInterface/*.h
   INTERFACE
   PUBLIC_HEADERS TrigDecisionInterface
   LINK_LIBRARIES AsgTools )
