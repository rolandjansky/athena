# $Id: CMakeLists.txt 732153 2016-03-24 12:45:45Z krasznaa $
################################################################################
# Package: VP1BanksSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1BanksSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   graphics/VP1/VP1Base
   PRIVATE
   Control/AthenaKernel
   GaudiKernel
   graphics/VP1/VP1Utils )

# External package(s):
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1BanksSystems
   VP1BanksSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1BanksSystems
   PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES VP1Base
   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel VP1Utils )
