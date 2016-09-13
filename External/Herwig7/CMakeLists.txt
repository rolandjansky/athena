# $Id$
#
# Package installing files from the Herwig 7 generator into the offline
# release.
#
      
# The name of the package:
atlas_subdir( Herwig7 )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Find the Herwig7 package. Note that LCG now calls the 7.0.1
# version "herwig3". LCG should call the package "herwig7"
# beginning with 7.0.3.
find_package( Herwig3 )

# Make sure that it was found:
if( NOT HERWIG3_FOUND )
   message( WARNING "Couldn't find the Herwig3 package" )
   return()
endif()

# Install files from the package:
set( herwig7_datadir "${HERWIG3_ROOT}/share/Herwig" )
if( NOT IS_DIRECTORY ${herwig7_datadir} )
   message( WARNING "Can't access directory ${herwig7_datadir}" )
   return()
endif()
atlas_install_generic( ${herwig7_datadir}/*.rpo ${herwig7_datadir}/*.in
   ${herwig7_datadir}/MSSM.model
   DESTINATION share
   TYPENAME Herwig7Data )


