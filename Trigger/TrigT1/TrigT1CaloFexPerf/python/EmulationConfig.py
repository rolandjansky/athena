# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as perfFlags
from AthenaCommon.Include import include
import AthenaCommon.CfgMgr as CfgMgr
def emulateSC(sCell_sequence, CellsIn="SeedLessFS", SCOut="EmulatedSCell"):
    # Conversion Service instance 
    from AthenaCommon.GlobalFlags import globalflags,jobproperties

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from RecExConfig.ObjKeyStore import objKeyStore
    if not objKeyStore.isInInput( "xAOD::EventInfo" ):
        if not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ):
            from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
            topSequence += xAODMaker__EventInfoCnvAlg()
        else:
            if not hasattr( topSequence, "xAODMaker::EventInfoNonConstCnvAlg" ):
                topSequence += CfgMgr.xAODMaker__EventInfoNonConstCnvAlg()


    if  jobproperties.Global.InputFormat() == 'bytestream':
        #If we are running on the bytestream, we need to enable the Run 2 bytestream converters to get xAOD::TriggerTower. 
        #Not techincally needed for emulateSC but needed for other things... 
        include('TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py')

    handle_transBS=None
    if ( globalflags.DataSource == "geant4" ): 
        from LArCabling.LArCablingAccess import LArFebRodMapping
        LArFebRodMapping()
        from TrigT1CaloFexPerf.TransBS_forL1_config import configure_transBS
        handle_transBS=configure_transBS()
    from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellSeedLessMaker
    
    from LArROD.LArSCSimpleMakerDefault import LArSCSimpleMaker
    from LArROD.LArSCSimpleMakerDefault import LArSuperCellBCIDEmAlg
    larscsm = LArSCSimpleMaker()
    #The output container of LArSCSimpleMaker is called SCellContainer 
    larscsm.CellContainer=CellsIn
    #The default input to LARSuperCellBCIDEmAlg (which applies the BCID correction) is the same: SCellContainer
    larscbea = LArSuperCellBCIDEmAlg()
    larscbea.SCellContainerOut=SCOut #Input to other algorithms
    
    #sCell_sequence = AlgSequence()
    if (handle_transBS is not None):
        sCell_sequence+=handle_transBS
    sCell_sequence+=HLTCaloCellSeedLessMaker()
    if(perfFlags.Calo.UseSimpleEmulation() ):
        #Use simple emulation tool, no timing information, so really only works for SC with E > 1 GeV 
        sCell_sequence+=larscsm
    else:
        #Use SCEmulation tool that randomly samples time histograms to estimate time for low energy and negative cells forming a supercell 
        sCell_sequence+= CfgMgr.LVL1__SCEmulation(InputCells=CellsIn, OutputSuperCells = "EmulatedSCellNoBCID")
        larscbea.SCellContainerIn="EmulatedSCellNoBCID"

    if(perfFlags.Calo.ApplyEmulatedPedestal()):
        #Apply the pedestal correction. There may be cases we do not want this. 
        sCell_sequence+=larscbea

def emulateSC_Cfg(flags, CellsIn="SeedLessFS", SCOut="EmulatedSCell"):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()
    if flags.Input.isMC: 
        from LArCabling.LArCablingConfig import LArFebRodMappingCfg
        acc.merge(LArFebRodMappingCfg(flags))

    from TrigCaloRec.TrigCaloRecConfig import hltCaloCellSeedlessMakerCfg
    acc.merge(hltCaloCellSeedlessMakerCfg(flags, roisKey = ""))


    #Use SCEmulation tool that randomly samples time histograms to estimate time for low energy and negative cells forming a supercell 
    acc.addEventAlgo(CompFactory.LVL1.SCEmulation(InputCells=CellsIn, OutputSuperCells = "EmulatedSCellNoBCID"))
    
    from LArROD.LArSCSimpleMakerConfig import LArSuperCellBCIDEmAlgCfg

    larSCargs = {}
    larSCargs["SCellContainerIn"] = "EmulatedSCellNoBCID"
    larSCargs["SCellContainerOut"] = SCOut

    if(perfFlags.Calo.ApplyEmulatedPedestal()):
        #Apply the pedestal correction. There may be cases we do not want this. 
    #The default input to LARSuperCellBCIDEmAlg (which applies the BCID correction) is the same: SCellContainer
        acc.merge(LArSuperCellBCIDEmAlgCfg(flags, **larSCargs))

    return acc
