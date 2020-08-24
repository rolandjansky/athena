# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
This map specifies with menu to load from TriggerMenuMT/python/L1/Menu

The mapping takes precedence over the existence of the menu definition file in the above directory
The resolved name is also being used in the L1/Config/ItemDef.py and L1/Config/ThresholdDef*.py files 
"""

menuMap = {
    "LS2_v1" : "MC_pp_v8",
    "Physics_pp_run3_v1" : "MC_pp_v8",
    "MC_pp_run3_v1" : "MC_pp_v8",
    "Physics_pp_v8" : "MC_pp_v8",
    "Cosmic_run3_v1" : "MC_pp_v8",
    "Physics_HI_v4" : "MC_pp_v8",
    "MC_HI_v4" : "MC_pp_v8",
    "PhysicsP1_pp_run3_v1" : "MC_pp_v8",
    "PhysicsP1_HI_run3_v1" : "MC_pp_v8",
    "Dev_HI_run3_v1" : "MC_pp_v8"
}
