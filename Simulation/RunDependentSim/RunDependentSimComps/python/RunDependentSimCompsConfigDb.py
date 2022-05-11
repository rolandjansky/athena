# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("RunDependentSimComps.RunDependentConfig.getLumiProfileSvc"       , "LumiProfileSvc")
addService("RunDependentSimComps.RunDependentConfig.getNoProfileSvc"         , "NoProfileSvc")
addService("RunDependentSimComps.RunDependentConfig.getEvtIdModifierSvc"     , "EvtIdModifierSvc")
