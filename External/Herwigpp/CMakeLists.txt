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

# Find the Herwig++ package. Note that LCG now calls the 7.0.1
# version "herwig3".
find_package( Herwig3 )

# Make sure that it was found:
if( NOT HERWIG3_FOUND )
   message( WARNING "Couldn't find the Herwig3 package" )
   return()
endif()

# Install files from the package:
set( herwig_datadir "${HERWIG3_LCGROOT}/share/Herwig" )
if( NOT IS_DIRECTORY ${herwig_datadir} )
   message( WARNING "Can't access directory ${herwig_datadir}" )
   return()
endif()
atlas_install_generic( ${herwig_datadir}/*.rpo ${herwig_datadir}/*.in
   ${herwig_datadir}/MSSM.model
   DESTINATION share
   TYPENAME HerwigData )
