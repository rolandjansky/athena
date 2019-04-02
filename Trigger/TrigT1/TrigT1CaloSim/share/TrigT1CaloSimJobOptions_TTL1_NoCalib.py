if not 'topSequence' in dir():
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

if not hasattr( topSequence, 'xAODMaker::EventInfoCnvAlg' ):
    topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()

include('TrigT1CaloSim/TrigT1CaloSimJobOptions_Run2.py')
