# $Id: CMakeLists.txt 728095 2016-03-07 12:42:05Z krasznaa $
#
# Package installing files from the Herwig++ generator into the offline
# release.
#

# The name of the package:
atlas_subdir( Herwigpp )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Find the Herwig++ package:
find_package( Herwigpp )

# Make sure that it was found:
if( NOT HERWIGPP_FOUND )
   message( WARNING "Couldn't find the Herwig++ package" )
   return()
endif()

# Install files from the package:
set( herwig_datadir "${HERWIGPP_ROOT}/share/Herwig++" )
if( NOT IS_DIRECTORY ${herwig_datadir} )
   message( WARNING "Can't access directory ${herwig_datadir}" )
   return()
endif()
atlas_install_generic( ${herwig_datadir}/*.rpo ${herwig_datadir}/*.in
   ${herwig_datadir}/MSSM.model
   DESTINATION share
   TYPENAME HerwigData )
