#
# Package setting up Pythia8 to be used in the ATLAS offline software.
#

# The name of the package:
atlas_subdir( Pythia8 )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Find Pythia8:
find_package( Pythia8 )

# If Pythia8 is not found, stop here:
if( NOT PYTHIA8_FOUND )
   message( WARNING "Pythia8 not found. Can't install its data files." )
   return()
endif()

# Install the files from the xmldoc directory into the offline release.
set( xmldoc_dir "${PYTHIA8_LCGROOT}/share/Pythia8/xmldoc" )
if( NOT IS_DIRECTORY ${xmldoc_dir} )
   message( WARNING "Can't access ${xmldoc_dir}" )
   return()
endif()
atlas_install_generic( ${xmldoc_dir}/*.xml ${xmldoc_dir}/*.dat
   ${xmldoc_dir}/*.data ${xmldoc_dir}/*.pds ${xmldoc_dir}/*.tex
   ${xmldoc_dir}/*.tbl ${xmldoc_dir}/*.eps
   DESTINATION share/Pythia8/xmldoc
   TYPENAME XmlDoc )
