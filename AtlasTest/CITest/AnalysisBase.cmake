# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# CI test definitions for the AnalysisBase project
# --> README.md before you modify this file
#

atlas_add_citest( EMPFlowData
   LOG_IGNORE_PATTERN "Cannot interpolate outside histogram domain" # ANALYSISTO-1165
   SCRIPT CI_EMPFlowDatatest.py )

atlas_add_citest( EMPFlowMC
   SCRIPT CI_EMPFlowMCtest.py )
