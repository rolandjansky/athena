# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


atlas_subdir( SelectionHelpers )

atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PRIVATE
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhyaicsAnalysis/AnalysisCommon/PATInterfaces )

atlas_add_library( SelectionHelpersLib
   SelectionHelpers/*.h SelectionHelpers/*.icc Root/*.cxx
   PUBLIC_HEADERS SelectionHelpers
   LINK_LIBRARIES AthContainers AsgTools xAODBase AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES PATCoreLib PATInterfaces )

atlas_add_dictionary( SelectionHelpersDict
   SelectionHelpers/SelectionHelpersDict.h
   SelectionHelpers/selection.xml
   LINK_LIBRARIES SelectionHelpersLib )
