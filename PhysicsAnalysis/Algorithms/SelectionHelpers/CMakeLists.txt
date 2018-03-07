# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


# The name of the package:
atlas_subdir( SelectionHelpers )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   Control/AthToolSupport/AsgTools )

atlas_add_library( SelectionHelpersLib
   SelectionHelpers/*.h SelectionHelpers/*.icc Root/*.cxx
   PUBLIC_HEADERS SelectionHelpers
   LINK_LIBRARIES PATCoreLib xAODBase PATInterfaces
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

atlas_add_dictionary( SelectionHelpersDict
   SelectionHelpers/SelectionHelpersDict.h
   SelectionHelpers/selection.xml
   LINK_LIBRARIES SelectionHelpersLib )
