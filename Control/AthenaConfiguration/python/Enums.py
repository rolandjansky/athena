# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from enum import Enum


class FlagEnum(Enum):
    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            raise TypeError(f"Invalid comparison of {self.__class__} with {type(other)}")
        return self is other

    __hash__ = Enum.__hash__


class Format(FlagEnum):
    BS = 'BS'
    POOL = 'POOL'


class ProductionStep(FlagEnum):
    # steps should be added when needed
    Default = 'Default'
    Simulation = 'Simulation'
    PileUpPresampling = 'PileUpPresampling'
    Overlay = 'Overlay'
    FastChain = 'FastChain'
    Digitization = 'Digitization'
    Reconstruction = 'Reconstruction'


def validrunformat(lhcperiod):
    import re
    for item in lhcperiod.__members__.values():
        if not re.match("^RUN[0-9]$", item.value):
            raise ValueError("Value not in a format RUN+single digit %s", item.value)
    return lhcperiod


@validrunformat
class LHCPeriod(FlagEnum):
    def __lt__(self, other):      #operator specific to validrunformat
        if not isinstance(other, self.__class__):
            raise TypeError(f"Invalid comparison of {self.__class__} with {type(other)}")
        else:
            return self.value < other.value
    def __le__(self, other):
        if not isinstance(other, self.__class__):
            raise TypeError(f"Invalid comparison of {self.__class__} with {type(other)}")
        else:
            return self.value <= other.value

    Run1 = 'RUN1'
    Run2 = 'RUN2'
    Run3 = 'RUN3'
    Run4 = 'RUN4'


class BeamType(FlagEnum):
    Collisions = 'collisions'
    SingleBeam = 'singlebeam'
    Cosmics = 'cosmics'
