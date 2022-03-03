# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure TrkG4UserActions"""

def MaterialStepRecorderCfg(configFlags, name="G4UA::UserActionSvc.MaterialStepRecorderTool", **kwargs):
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from AthenaConfiguration.ComponentFactory import CompFactory
  result = ComponentAccumulator()  
  result.setPrivateTools(CompFactory.G4UA.MaterialStepRecorderTool(name, **kwargs))
  return result
  

def MaterialStepRecorderUserActionSvcCfg(configFlags, name="G4UA::MaterialStepRecorderUserActionSvc", **kwargs):
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from AthenaConfiguration.ComponentFactory import CompFactory
  
  result = ComponentAccumulator()
  
  #Setting up the CA for the MaterialStepRecorder
  actionAcc = ComponentAccumulator()
  actions = []
  actions += [actionAcc.popToolsAndMerge(MaterialStepRecorderCfg(configFlags))]
  actionAcc.setPrivateTools(actions)
  MaterialStepRecorderAction = result.popToolsAndMerge(actionAcc)
  
  #Retrieving the default action list
  from G4AtlasServices.G4AtlasUserActionConfigNew import getDefaultActions
  defaultActions = result.popToolsAndMerge(getDefaultActions(configFlags))
    
  #Adding LengthIntegrator to defaults
  actionList = (defaultActions + MaterialStepRecorderAction)

  #Setting up UserActionsService
  ##We clear it here because UserActionsTools also wants kwargs, different
  ##from the tool above - probably this can be improved...
  kwargs_UATools = {}
  kwargs_UATools.setdefault("UserActionTools",actionList)
  result.addService(CompFactory.G4UA.UserActionSvc(name,**kwargs_UATools))    
    
  return result

def MaterialStepRecorder(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
  
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from AthenaConfiguration.ComponentFactory import CompFactory

  result = ComponentAccumulator()
  #Setting up the CA
  actionAcc = ComponentAccumulator()
  actions = []
  actions += [actionAcc.popToolsAndMerge(MaterialStepRecorderCfg(configFlags,**kwargs))]
  actionAcc.setPrivateTools(actions)
  MaterialStepRecorderAction = result.popToolsAndMerge(actionAcc)
  
  actionList = MaterialStepRecorderAction
  #We clear it here because UserActionsTools also wants kwargs, different
  #from the tool above - probably this caq be improved...
  kwargs_UATools = {}
  kwargs_UATools.setdefault("UserActionTools",actionList)
  result.addService(CompFactory.G4UA.UserActionSvc(name,**kwargs_UATools))
  
  AthenaOutputStream=CompFactory.AthenaOutputStream
  AthenaOutputStreamTool=CompFactory.AthenaOutputStreamTool
  writingTool = AthenaOutputStreamTool( "MaterialStepCollectionStreamTool" )
  
  outputStream = AthenaOutputStream(name = "MaterialStepCollectionStream",
                                    WritingTool = writingTool,
                                    ItemList=['EventInfo#*', 'Trk::MaterialStepCollection#*'],
                                    MetadataItemList = [ "EventStreamInfo#MaterialStepCollectionStream", "IOVMetaDataContainer#*" ],
                                    OutputFile = "MaterialStepCollection.root")
  
  StoreGateSvc=CompFactory.StoreGateSvc
  result.addService(StoreGateSvc("MetaDataStore"))
  outputStream.MetadataStore = result.getService("MetaDataStore")
  
  MakeEventStreamInfo=CompFactory.MakeEventStreamInfo
  streamInfoTool = MakeEventStreamInfo( "MaterialStepCollectionStream_MakeEventStreamInfo" )
  streamInfoTool.Key = "MaterialStepCollectionStream"
  streamInfoTool.EventInfoKey = "EventInfo"
  outputStream.HelperTools.append(streamInfoTool)
      
  result.addEventAlgo(outputStream)
  
  return result
