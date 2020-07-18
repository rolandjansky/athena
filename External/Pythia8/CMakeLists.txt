# $Id: CMakeLists.txt 728071 2016-03-07 10:51:32Z krasznaa $
#
# Package setting up Pythia8 to be used in the ATLAS offline software.
#

# The name of the package:
atlas_subdir( Pythia8 )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

#set( PYTHIA8_VERSION 230 )
#set( PYTHIA8_ROOT    
# /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/pythia8/${PYTHIA8_VERSION}/${LCG_PLATFORM} )

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
   message( WARNING " Can't access ${xmldoc_dir}" )
   return()
endif()
atlas_install_generic( ${xmldoc_dir}/*.xml ${xmldoc_dir}/*.dat
   ${xmldoc_dir}/*.data ${xmldoc_dir}/*.pds ${xmldoc_dir}/*.tex
   ${xmldoc_dir}/*.tbl ${xmldoc_dir}/*.eps
   DESTINATION share/Pythia8/xmldoc
   TYPENAME XmlDoc )

# Install the files from the pdfdata directory into the offline release. New in Pythia8.3
set( pdfdata_dir "${PYTHIA8_LCGROOT}/share/Pythia8/pdfdata" )
if( NOT IS_DIRECTORY ${pdfdata_dir} )
   message( INFO " Can't access ${pdfdata_dir} (Needed only for Pythia8.3)" )
   return()
endif()

atlas_install_generic( ${pdfdata_dir}/*.*
   DESTINATION share/Pythia8/pdfdata
   TYPENAME XmlDoc )
