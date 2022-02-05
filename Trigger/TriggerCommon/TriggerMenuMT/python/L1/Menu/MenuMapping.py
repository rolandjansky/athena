# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
This map specifies with menu to load from TriggerMenuMT/python/L1/Menu

The mapping takes precedence over the existence of the menu definition file in the above directory
The resolved name is also being used in the L1/Config/ItemDef.py and L1/Config/ThresholdDef*.py files 
"""

menuMap = {
    # pp
    "Physics_pp_run3_v1"   : ["Physics_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "PhysicsP1_pp_run3_v1" : ["Physics_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "MC_pp_run3_v1"        : ["MC_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "Dev_pp_run3_v1"               : ["MC_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "Dev_pp_run3_v1_Primary_prescale"           : ["MC_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "Dev_pp_run3_v1_TriggerValidation_prescale" : ["MC_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "Dev_pp_run3_v1_BulkMCProd_prescale"        : ["MC_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],
    "Dev_pp_run3_v1_CPSampleProd_prescale"      : ["MC_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],

    # low mu
    "PhysicsP1_pp_lowMu_run3_v1"  : ["MC_HI_run3_v1","MC_pp_run3_v1_inputs","MC_HI_run3_v1_inputs_legacy"],

    # cosmics
    "Cosmic_run3_v1"       : ["Physics_pp_run3_v1","MC_pp_run3_v1_inputs","MC_pp_run3_v1_inputs_legacy"],

    #HI
    "PhysicsP1_HI_run3_v1" : ["MC_HI_run3_v1","MC_pp_run3_v1_inputs","MC_HI_run3_v1_inputs_legacy"],
    "Dev_HI_run3_v1"       : ["MC_HI_run3_v1","MC_pp_run3_v1_inputs","MC_HI_run3_v1_inputs_legacy"],
}
