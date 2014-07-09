# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#import TrigConfigSvc.TriggerPythonConfig


def extendLVL1config(triggerPythonConfig):
    triggerPythonConfig.CTPInfo().setBunchGroupSet("MC")
    triggerPythonConfig.CTPInfo().addBunchGroup( 'BCRVeto',           0, [1] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'Filled',            1, [1] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'EmptyCalib',        2, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'Empty',             3, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'UnpairedBeam1',     4, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'UnpairedBeam2',     5, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'EmptyAfterFilled',  6, [] )
    triggerPythonConfig.CTPInfo().addBunchGroup( 'InTrain',           7, [1] )
    triggerPythonConfig.CTPInfo().setDeadtime('Commissioning', 4, 0, 0, 0, 0)
    triggerPythonConfig.CTPInfo().setRandom('rand01', 5, 0, 1, 1)
    triggerPythonConfig.CTPInfo().setPrescaledClock('psc01', 10, 100)
    triggerPythonConfig.Lvl1CaloInfo().setName('standard')

