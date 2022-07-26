#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

'''@file JetValidationConfig.py
@author N.Pettersson
@date 2022-07-04
@brief Main CA-based python configuration for JetValidation
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValJetCfg(flags, **kwargs):
	acc = ComponentAccumulator()

	kwargs.setdefault("EnableLumi", False)
	from AthenaCommon.Constants import WARNING
	kwargs.setdefault("OutputLevel", WARNING)
	kwargs.setdefault("DetailLevel", 10)

	from JetValidation.JetValidationTools import PhysValJetToolCfg
	acc.setPrivateTools(acc.popToolsAndMerge(PhysValJetToolCfg(flags, **kwargs)))
	
	return acc

def JetValidationCfg(flags, **kwargs):
	acc = ComponentAccumulator()

	kwargs.setdefault("FileKey", "PhysVal")
	kwargs.setdefault("Environment", "altprod")
	kwargs.setdefault("ManualDataTypeSetup", True)
	kwargs.setdefault("DataType", "monteCarlo")
	kwargs.setdefault("ManualRunLBSetup", True)
	kwargs.setdefault("Run", 1)
	kwargs.setdefault("LumiBlock", 1)
	kwargs.setdefault("AthenaMonTools", [acc.popToolsAndMerge(PhysValJetCfg(flags))])
	acc.addEventAlgo(CompFactory.AthenaMonManager( "PhysValMonManager", **kwargs), primary = True)

	return acc
