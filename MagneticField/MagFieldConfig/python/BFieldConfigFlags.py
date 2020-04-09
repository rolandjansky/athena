# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createBFieldConfigFlags(): 
    bcf=AthConfigFlags()
    # True when magnetic filed is taken from these flags rather than cond db (when available)
    bcf.addFlag("BField.override", True)
    # True when solenoid is on
    bcf.addFlag("BField.solenoidOn", True)
    # True when barrel toroid is on
    bcf.addFlag("BField.barrelToroidOn", True)
    # True when endcap toroid is on
    bcf.addFlag("BField.endcapToroidOn", True)
    return bcf
