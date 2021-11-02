#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ActsInterop import UnitConstants

units = [
    "fm",
    "pm",
    "um",
    "nm",
    "mm",
    "cm",
    "m",
    "km",
    "mm2",
    "cm2",
    "m2",
    "mm3",
    "cm3",
    "m3",
    "s",
    "fs",
    "ps",
    "ns",
    "us",
    "ms",
    "min",
    "h",
    "mrad",
    "rad",
    "degree",
    "eV",
    "keV",
    "MeV",
    "GeV",
    "TeV",
    "J",
    "u",
    "g",
    "kg",
    "e",
    "C",
    "T",
    "Gauss",
    "kGauss",
    "mol",
]

for unit in units:
    assert hasattr(UnitConstants, unit), (
        "%s not in ActsInterop.Units.UnitConstants" % unit
    )

    assert getattr(UnitConstants, unit) != 0.0


