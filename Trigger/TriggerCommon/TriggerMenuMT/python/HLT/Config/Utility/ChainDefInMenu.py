# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import typing
from typing import List
import dataclasses
from dataclasses import dataclass, field

@dataclass(frozen=True)
class ChainProp:
    """Class to hold chain properties with (limited) type-checking"""
    name: str                   # mandatory
    groups: List[str]           # mandatory
    monGroups: List[str]        = field(default_factory = list)
    l1SeedThresholds: List[str] = field(default_factory = list)
    stream: List[str]           = field(default_factory = lambda : ['Main'])
    mergingStrategy: str        = 'auto'
    mergingOrder: List[str]     = field(default_factory = list)
    mergingOffset: int          = -1
    topoStartFrom: bool         = False  # might be obsolete

    # currently we don't have a type-checker in LCG so we
    # implement a limited version ourselves:
    def _check_types(self):
        """Check field types"""
        for f in dataclasses.fields(self):
            actual_type = typing.get_origin(f.type) or f.type
            value = getattr(self, f.name)
            if actual_type == list:
                if value:
                    for el in value:
                        if not isinstance(el, typing.get_args(f.type)):
                            return f
            elif not isinstance(value, actual_type):
                return f
        return None

    def __post_init__(self):
        f = self._check_types()
        if f is not None:
            raise TypeError(f"Expected type of '{f.name}' to be {f.type}, "
                            f"got {repr(getattr(self, f.name))}")

if __name__=='__main__':
    ch = ChainProp(name='abc', groups=['g1'])
