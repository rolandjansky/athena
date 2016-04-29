# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Utility functions for DPD transforms

from RecExConfig.RecoFunctions import AddValidItemToList

def SetupOutputDPDs(runArgs,flagContainerList):
    DPDMakerScripts=[]
    for flagContainer in flagContainerList:
        for flagName in flagContainer.__dict__.keys():
            flag=getattr(flagContainer,flagName)
            if hasattr(flag,"StreamName"):
                dpdName=flag.StreamName.lstrip("Stream")
                argName='output'+dpdName+'File'
                if hasattr(runArgs,argName):
                    if hasattr(flag,"FileName"):
                        flag.FileName=getattr(runArgs,argName)
                        flag.set_Value_and_Lock( True )
                        if hasattr(flag,"DPDMakerScript"):
                            AddValidItemToList(flag.DPDMakerScript,DPDMakerScripts)
                        elif hasattr (flag, 'configure'):
                            DPDMakerScripts.append(flag.configure)
                        else:
                            raise RuntimeError("No athena script attached to argument '%s'"%argName)
                    else:
                        raise RuntimeError("%s is not conform with the job transform convention, hence unusable. Please fix this."%dpdName)

    return DPDMakerScripts