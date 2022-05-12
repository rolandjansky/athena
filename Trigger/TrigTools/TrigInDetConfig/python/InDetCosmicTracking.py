# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from ViewAlgs.ViewAlgsConf import EventViewCreatorAlgorithm
from DecisionHandling.DecisionHandlingConf import ViewCreatorInitialROITool
from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

def getTrigCosmicInDetTracking(flags):
    dataPrepInputMaker = EventViewCreatorAlgorithm("IMCosmicTrkEventViewCreator")
    dataPrepInputMaker.ViewFallThrough = True
    dataPrepInputMaker.RoITool = ViewCreatorInitialROITool()
    dataPrepInputMaker.InViewRoIs = "InputRoI" # contract with the consumer
    dataPrepInputMaker.Views = "CosmicViewRoIs"
    dataPrepInputMaker.RequireParentView = False

    idTrigConfig = getInDetTrigConfig('cosmics')
    from TrigInDetConfig.InDetTrigFastTracking import makeInDetTrigFastTracking
    dataPrepAlgs, verifier = makeInDetTrigFastTracking(config=idTrigConfig, 
                                     rois=dataPrepInputMaker.InViewRoIs, 
                                     viewVerifier='VDVCosmicsIDTracking', 
                                     doFTF=False) # no fast tracking, just data prep
    verifier.DataObjects += [('TrigRoiDescriptorCollection', 'StoreGateSvc+InputRoI')]

    from TrigInDetConfig.EFIDTracking import makeInDetPatternRecognition
    efidAlgs, verifierForEF = makeInDetPatternRecognition(idTrigConfig, verifier='VDVCosmicsIDTracking')
    return   dataPrepInputMaker, [verifier,verifierForEF] + dataPrepAlgs + efidAlgs
