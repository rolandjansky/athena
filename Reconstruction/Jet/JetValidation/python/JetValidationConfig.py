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

	kwargs.setdefault("EnableLum", False)
	from AthenaCommon.Constants import WARNING
	kwargs.setdefault("OutputLevel", WARNING)
	kwargs.setdefault("DetailLevel", 10)

	from JetValidation.JetValidationTools import PhysValJetToolCfg
	acc.popToolsAndMerge(PhysValJetToolCfg(flags, **kwargs))
	
	return acc

def JetValidationCfg(flags):
	acc = ComponentAccumulator()

	monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
	monMan.FileKey = "PhysVal"
	monMan.Environment = "altprod"
	monMan.ManualDataTypeSetup = True
	monMan.DataType = "monteCarlo"
	monMan.ManualRunLBSetup = True
	monMan.Run = 1
	monMan.LumiBlock = 1
	from JetValidation.JetValidationTools import PhysValJetToolCfg
	monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValJetToolCfg(flags)) ]

	acc.addEventAlgo(monMan, primary = True)
	return acc
