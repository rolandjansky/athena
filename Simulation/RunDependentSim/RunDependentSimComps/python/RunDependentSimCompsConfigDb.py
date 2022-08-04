# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addService

addService("RunDependentSimComps.RunDependentConfigLegacy.getLumiProfileSvc"       , "LumiProfileSvc")
addService("RunDependentSimComps.RunDependentConfigLegacy.getNoProfileSvc"         , "NoProfileSvc")
addService("RunDependentSimComps.RunDependentConfigLegacy.getEvtIdModifierSvc"     , "EvtIdModifierSvc")
