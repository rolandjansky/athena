# $Id: CMakeLists.txt 769086 2016-08-22 12:03:50Z krasznaa $
################################################################################
# Package: TruthConverters
################################################################################

# Set the name of the package:
atlas_subdir( TruthConverters )


# Component(s) in the package:
atlas_add_library( TruthConvertersLib
   TruthConverters/*.h Root/*.cxx
   SHARED
   PUBLIC_HEADERS TruthConverters
   LINK_LIBRARIES AsgTools  xAODEventInfo xAODTruth GenInterfacesLib )

atlas_add_Component( TruthConverters
   src/components/*.cxx
   LINK_LIBRARIES TruthConvertersLib )

atlas_add_dictionary( TruthConvertersDict
   TruthConverters/xAODtoHepMCDict.h TruthConverters/selection.xml
   LINK_LIBRARIES TruthConvertersLib )
