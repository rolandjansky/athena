# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigSteeringTest.TrigSteeringTestConf import DummyComboUsingTopo
from TrigSteeringTest.TrigSteeringTestMonitoring import TrigSteeringTestValMonitoring



class DummyComboUsingTopoConfig( DummyComboUsingTopo ):

    __slots__ = []
    def __init__ (self, name="DummyComboUsingTopoConfig"):

        super(DummyComboUsingTopoConfig, self).__init__(name)

        self.AthenaMonTools += [ TrigSteeringTestValMonitoring() ]
