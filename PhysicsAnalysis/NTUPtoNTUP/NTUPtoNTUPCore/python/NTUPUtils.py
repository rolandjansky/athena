# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecoFunctions import AddValidItemToList
def SetupOutputNTUPs(runArgs,flagContainerList):
     NTUPScripts=[]
     for flagContainer in flagContainerList:
         for flagName in flagContainer.__dict__.keys():
             flag=getattr(flagContainer,flagName)
             if hasattr(flag,"StreamName"):
                 ntupName=flag.StreamName.lstrip("Stream")
                 argName='output'+ntupName+'File'
                 if hasattr(runArgs,argName):
                     if hasattr(flag,"FileName"):
                         flag.FileName=getattr(runArgs,argName)
                         flag.set_Value_and_Lock( True )
                         if hasattr(flag,"NTUPScript"):
                             AddValidItemToList(flag.NTUPScript,NTUPScripts)
                         elif hasattr (flag, 'configure'):
                             NTUPScripts.append(flag.configure)
                         else:
                             raise RuntimeError("No athena script attached to argument '%s'"%argName)
                     else:
                         raise RuntimeError("%s is not conform with the job transform convention, hence unusable. Please fix this."%ntupName)

     return NTUPScripts

def SetupOutputSkimNTUPs(runArgs,flagContainerList):
     SkimNTUPScripts=[]
     for flagContainer in flagContainerList:
         for flagName in flagContainer.__dict__.keys():
             flag=getattr(flagContainer,flagName)
             if hasattr(flag,"StreamName"):
                 ntupName=flag.StreamName.lstrip("Stream")
                 argName='output'+ntupName+'File'
                 if hasattr(runArgs,argName):
                     if hasattr(flag,"FileName"):
                         flag.FileName=getattr(runArgs,argName)
                         flag.set_Value_and_Lock( True )
                         if hasattr(flag,"SkimNTUPScript"):
                             AddValidItemToList(flag.SkimNTUPScript,SkimNTUPScripts)
                         elif hasattr (flag, 'configure'):
                             SkimNTUPScripts.append(flag.configure)
                         else:
                             raise RuntimeError("No athena script attached to argument '%s'"%argName)
                     else:
                         raise RuntimeError("%s is not conform with the job transform convention, hence unusable. Please fix this."%ntupName)

     return SkimNTUPScripts

def NTUPtoNTUPProdFlags():
    from NTUPtoNTUPCore.NTUPtoNTUPProdFlags import prodFlags
    return prodFlags

def SkimProdFlags():
    from NTUPtoNTUPCore.SkimProdFlags import prodFlags
    return prodFlags

def NTUPtoNTUPTopOptionsInclude():
    include( "NTUPtoNTUPCore/NTUPtoNTUP_topOptions.py" )

def MNSMgr():
    from NTUPtoNTUPCore.MultipleNTUPStreamManager import MNSMgr
    return MNSMgr
