from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.AlgSequence import AthSequencer

def MainServicesMiniCfg():
    #Mininmal basic config, just good enough for HelloWorld and alike
    cfg=ComponentAccumulator()
    cfg.setAppProperty('TopAlg',['AthSequencer/AthAlgSeq']) #Just one sequence, no nesting
    cfg.setAppProperty('MessageSvcType', 'MessageSvc')
    cfg.setAppProperty('EventLoop', 'AthenaEventLoopMgr') 
    cfg.setAppProperty('ExtSvcCreates', 'False')
    cfg.setAppProperty('JobOptionsSvcType', 'JobOptionsSvc')
    cfg.setAppProperty('JobOptionsType', 'NONE')
    cfg.setAppProperty('JobOptionsPostAction', '')
    cfg.setAppProperty('JobOptionsPreAction', '')
    return cfg


def MainServicesSerialCfg():
    cfg=ComponentAccumulator("AthMasterSeq")
    cfg.merge(MainServicesMiniCfg())
    cfg.setAppProperty('TopAlg',['AthSequencer/AthMasterSeq'],overwrite=True)
    cfg.setAppProperty('OutStreamType', 'AthenaOutputStream')    
    

    #Build standard sequences:
    #cfg.addSequence(AthSequencer("AthMasterSeq"))
    cfg.addSequence(AthSequencer('AthAlgEvtSeq'),parentName="AthMasterSeq") 
    cfg.addSequence(AthSequencer('AthOutSeq'),parentName="AthMasterSeq")
    cfg.addSequence(AthSequencer('AthRegSeq'),parentName="AthMasterSeq")

    cfg.addSequence(AthSequencer('AthBeginSeq'),parentName='AthAlgEvtSeq')
    cfg.addSequence(AthSequencer('AthAllAlgSeq'),parentName='AthAlgEvtSeq') 
    cfg.addSequence(AthSequencer('AthEndSeq'),parentName='AthAlgEvtSeq') 
    cfg.addSequence(AthSequencer('AthCondSeq'),parentName='AthAllAlgSeq')
    cfg.addSequence(AthSequencer('AthAlgSeq'),parentName='AthAllAlgSeq')

    #Set up incident firing:
    from AthenaServices.AthenaServicesConf import AthIncFirerAlg
    from GaudiCoreSvc.GaudiCoreSvcConf import IncidentProcAlg

    cfg.addEventAlgo(AthIncFirerAlg("BeginIncFiringAlg",Incidents=['BeginEvent']),sequenceName='AthBeginSeq')
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg1'),sequenceName='AthBeginSeq')

    cfg.addEventAlgo(AthIncFirerAlg('EndIncFiringAlg',Incidents=['EndEvent']), sequenceName="AthEndSeq")
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg2'),sequenceName="AthEndSeq")

    #Basic services:
    from CLIDComps.CLIDCompsConf import ClassIDSvc
    cfg.addService(ClassIDSvc(CLIDDBFiles= ['clid.db',"Gaudi_clid.db" ]))


    from StoreGate.StoreGateConf import StoreGateSvc
    cfg.addService(StoreGateSvc())
    cfg.addService(StoreGateSvc("DetectorStore"))
    cfg.addService(StoreGateSvc("ConditionStore"))
    
    cfg.setAppProperty('InitializationLoopCheck',False)
    return cfg
    
