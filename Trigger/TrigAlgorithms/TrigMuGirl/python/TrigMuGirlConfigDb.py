# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm,addTool,addService

addTool("TrigMuGirl.TrigMuGirlConfig.TrigMuGirlToolConfig", "TrigMuGirlTool")

addAlgorithm("TrigMuGirl.TrigMuGirlConfig.TrigMuGirlConfig", "TrigMuGirl")
