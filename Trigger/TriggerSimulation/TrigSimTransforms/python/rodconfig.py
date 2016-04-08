# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# this file provides lookup for the ROD versions of all sub-detectors by release



rodconfig = {}

rodconfig["17.1.3.4"] = {}

rodconfig["17.1.3.4"]["LAR"] = {"DSPRunMode" : 4, "RodBlockVersion" : 9, "IncludeDigits" : True }


def get_rod_config(trigger_release):
    """
    provides ROD version lookup for trigger release

    returns dictionary {"subdetector" -> version}
    """

    return rodconfig[trigger_release]
