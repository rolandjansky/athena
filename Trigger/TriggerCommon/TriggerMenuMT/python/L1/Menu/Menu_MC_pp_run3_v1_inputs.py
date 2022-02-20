# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from ..Base.L1MenuFlags import L1MenuFlags
from ..Base.MenuConfObj import TopoMenuDef
import TriggerMenuMT.L1.Menu.Menu_Physics_pp_run3_v1_inputs as phys_menu_inputs


def defineInputsMenu():
    
    phys_menu_inputs.defineInputsMenu()

    for boardName, boardDef in L1MenuFlags.boards().items():
        if "connectors" in boardDef:
           for conn in boardDef["connectors"]:

              # Add more multiplicity inputs

              # Topo1Opt3
              if conn["name"] == "Topo1Opt3":
                  conn["thresholds"] += [
                       ('jXEPerf100',1),
                  ]              

              # Add more decision algorithms
              if conn["name"] == "Topo2El":
                  for group in conn["algorithmGroups"]:
                      if group["fpga"]==0 and group["clock"]==1:
                         group["algorithms"] += [
                             TopoMenuDef( '0DR04-MU5VFab-CjJ50ab',  outputbits = 11), #Bjet, TODO: not a primary
                         ]
   

    #----------------------------------------------

    def remapThresholds():
        # remap thresholds. TODO: add checks in case the remap does not fulfill HW constraints?
        for boardName, boardDef in L1MenuFlags.boards().items():
            if "connectors" in boardDef:
                for conn in boardDef["connectors"]:
                    if "thresholds" in conn:
                        thresholdsToRemove = []
                        for thrIndex, thrName in enumerate(conn["thresholds"]):
                            nBits = 0
                            if type(thrName)==tuple:
                                (thrName,nBits) = thrName
                            if thrName in L1MenuFlags.ThresholdMap():
                                if (L1MenuFlags.ThresholdMap()[thrName] != ''):
                                    if nBits > 0:
                                        conn["thresholds"][thrIndex] = (L1MenuFlags.ThresholdMap()[thrName],nBits)
                                    else:
                                        conn["thresholds"][thrIndex] = L1MenuFlags.ThresholdMap()[thrName]
                                else:
                                    thresholdsToRemove.append(thrIndex) 
                        for i in reversed(thresholdsToRemove):
                            del conn["thresholds"][i]

    #----------------------------------------------

    remapThresholds()

