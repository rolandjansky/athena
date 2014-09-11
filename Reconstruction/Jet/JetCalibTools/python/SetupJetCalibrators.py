# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##-----------------------------------------------------------------------
## \author  P.A. Delsart
## \author  P.O. DeViveiros
## \date    June 7 2009
##
## \file SetupJetCalibrators.py
##
## \brief Functions used to setup the different JetCalibrators
##
## do***:
##    Standard configurations for the JetCalibrators used in calibration sequences.
##    dependent on finder, jet size, input.
##
##----------------------------------------------------------------------------

from JetRec.JetAlgConfiguration import checkAndUpdateOptions, editParm
#import JetCalibTools.MCNumInvConstants as MCNumInvConstants
#import JetCalibTools.EtaMassEnergyFactors as EtaMassEnergyFactors

import JetCalibTools.DBConfig as DBConfig
import JetCalibTools.DBUtils as DBUtils


# set up logger --------------------------- 
from AthenaCommon.Logging import logging
_caliblog = logging.getLogger( 'SetupJetCalib' )

# Here we modify our logger so it also prints the function name
# automatically :
class JetCalibLogFilter(logging.Filter):
    def filter(self,record):
        # just add the calling function name :
        record.name = record.name+':'+record.funcName
        return True
_caliblog.addFilter( JetCalibLogFilter() )
#-----------------------------------------------



def doH1CellCalib(finder, mainParam, input, precedingCalibs = [], **options):
    """ Just forward to the relevan function according to input"""
    from JetRec.JetRecFlags import jetFlags
    if jetFlags.CalibrationDBSuffix() == "AtlfastII":
        _caliblog.info("Using constants for Atlfast II")
        if 'Tower' == input or 'TopoTower' == input:
            return doTowerCellCalibAtlfastII(finder , mainParam, input, **options)
        elif 'Topo'  == input  :
            return doTopoCellCalibAtlfastII(finder , mainParam, input, **options)
        else:
            print " WARNING No H1 Cell calibration known for input",input
            return None
    else:
        if 'Tower' == input or 'TopoTower' == input:
            return doTowerCellCalib(finder , mainParam, input, **options)
        elif 'Topo'  == input  :
            return doTopoCellCalib(finder , mainParam, input, **options)
        else:
            print " WARNING No H1 Cell calibration known for input",input
            return None
    

def doTowerCellCalib(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard H1 Cell Calibration '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    from JetCalibTools.JetCalibToolsConf import JetCellCalibTool
    
    calibT = JetCellCalibTool("H1Cell")

    cellcalibtool = getCellWeightTool(finder, mainParam, input, onlyCellWeight=True)

    calibT.CellCalibrator = cellcalibtool
    calibT.ApplyScaleFactor = False
    calibT.CalibrationTag = 'GCW'
    if options['GCWScheme'] != None:
        calibT.TowerMethod = options['GCWScheme']
    
    return calibT

def doTopoCellCalib(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Retrieve H1 Signal State for TopoClusters '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
    from JetCalibTools.JetCalibToolsConf import JetH1StateCryoCorr
    from JetRec.JetGetterBase import editParm    

    calibT = JetH1StateCryoCorr("RestoreH1Scale")
    calibT.CalibrationTag = 'GCW'
    if input=="Topo" : calibT.ConstituentState = 2 # constituents are cluster (2 is H1)
    else:                calibT.ConstituentState = 1 # constituents are jets (1 is final hence H1)
    calibT.CellCalibrator = None # should be re-set below in case of Topo

    if input=="Topo" :
        # cryostat correction is mandatory
        calibT.CorrectCryostat = True
        tname =  finder + editParm(mainParam) + input
        
        cellcalibtool = getCellWeightTool(finder, mainParam, input, onlyCellWeight=True)        
        calibT.CellCalibrator = cellcalibtool

    return calibT
        
def doH1ScaleFactor(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard H1 Scale Factor '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic
    from CaloClusterCorrection.StandardCellWeightCalib import H1Calibration
    from JetCalibTools.JetCalibToolsConf import MCNumInversionResponse
    
    (key, folder, tag ) = H1Calibration.getCalibDBParams(finder, mainParam, input)
    H1Calibration.loadCaloFolder(folder,tag)
    
    scaleTool = MCNumInversionResponse("H1Factor")
    scaleTool.EtaBins = [0]
    scaleTool.UseH1Calculation = True
    scaleTool.WriteMoments   = True
    scaleTool.CalibrationTag = 'H1f'
    scaleTool.detStoreKey = key # For now, use same key as H1weights

    return scaleTool

def doEtaJESCorrection(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard EtaJES Correction '''
    
    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)


    # test  the current calibration state.
    # start with higher calibration first, else, assume EM case
    if "GCW" in precedingCalibs:
        preCalibState , flag , calibTag = 'GCW' , '', 'GCWJES'
    elif "LC" in precedingCalibs:
        preCalibState , flag , calibTag = 'LC' , '', 'LCJES'
    else:
        preCalibState , flag , calibTag = 'EM' , '_EM', 'EMJES'        

    
    from JetCalibTools.JetCalibToolsConf import EtaMassEnergyCorr
    from JetRec.JetGetterBase import editParm
    
    
    #dbinput = input
    
    from JetRec.JetRecFlags import jetFlags
    
    # Check whether we have TowerGhostJets or not
    replaceNegEnergy = options["replaceNegEnergy"]
    if 'Tower' in input and replaceNegEnergy == True:
        # If so, add Ghost to the name
        #dbinput = dbinput + "Ghost"
        input = input + "Ghost"

    #jetname =  finder + editParm(mainParam) + input + "Jets" + flag + jetFlags.CalibrationDBSuffix()
    jetname =  finder + editParm(mainParam) + input + "Jets" + flag 
    dbkey = DBUtils.getNameFromJetname(jetname)

    #mcitool = EtaMassEnergyCorr("NumInv"+preCalibState)
    mcitool = EtaMassEnergyCorr("EtaMassEnergyCorr"+"_"+dbkey)
    #dbfolder = 'NumInv'+finder+editParm(mainParam)+dbinput+ jetFlags.CalibrationDBSuffix()
    if not DBUtils.configFolder(mcitool):
        _caliblog.error(' Could not setup folder for tool: '+mcitool.getFullName())
        return ''

    mcitool.detStoreKey      = "EtaMassEnergyCorr_"+dbkey
    #mcitool.DBstring         = dbkey
    mcitool.COOLFolder       = DBUtils.getFolder(mcitool)
    _caliblog.info(' Performing *%s* EtaJES for %s! (constants from DB)' %(preCalibState, dbkey) )
        
    # Eta bins for EtaMassEnergy correction - 90 of them
    #etaBins=[]
    #for i in range(-44, 45):
    #    etaBins.append(i*0.1)
    #    
    #mcitool.EtaBins          = [] # clear first to fix imcompatibility error
    #mcitool.EtaBins          = etaBins
    mcitool.PtCut            = 10.
    mcitool.WriteMoments     = True
    mcitool.CalibrationTag   = calibTag
    #mcitool.OutputLevel      = 2
    
    return mcitool
    
def doMCNumInversion(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard MCNumericalInversion Response Correction '''
            
    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    # test  the current calibration state.
    # start with higher calibration first, else, assume EM case
    if "GCW" in precedingCalibs:
        preCalibState , flag , calibTag = 'GCW' , '', 'GCWJES'
    elif "LC" in precedingCalibs:
        preCalibState , flag , calibTag = 'LC' , '', 'LCJES'
    else:
        preCalibState , flag , calibTag = 'EM' , '_EM', 'EMJES'        
    
    from JetCalibTools.JetCalibToolsConf import MCNumInversionResponse
    from JetRec.JetGetterBase import editParm
    
    mcitool =  MCNumInversionResponse("NumInv"+preCalibState)
    

    #dbinput = input



    from JetRec.JetRecFlags import jetFlags

    # Check whether we have TowerGhostJets or not
    replaceNegEnergy = options["replaceNegEnergy"]    
    if 'Tower' in input and replaceNegEnergy == True:
        # If so, add Ghost to the name
        #dbinput = dbinput + "Ghost"
        input = input + "Ghost"

    #jetname =  finder + editParm(mainParam) + input + "Jets" + flag + jetFlags.CalibrationDBSuffix()
    jetname =  finder + editParm(mainParam) + input + "Jets" + flag 

    if not DBUtils.configFolder(mcitool):
        _caliblog.error(' Could not setup folder for tool: '+mcitool.getFullName())
        return ''

    dbkey = DBUtils.getNameFromJetname(jetname)
    # Check for asking the nonexisting constamnts (no tag resolution yet
    print "SetupJetCalibrators, going to check ",jetname      
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource()=='data':
       myl=DBUtils.GetFolderChanList(DBUtils.getFolder(mcitool),'',False)
    else:   
       myl=DBUtils.GetFolderChanList(DBUtils.getFolder(mcitool),'',True)
    if not "MCNumInversionResponse_"+dbkey in myl:
       dbkey = DBUtils.getNameFromJetname('AntiKt4TopoJets')
    mcitool.detStoreKey      = "MCNumInversionResponse_"+dbkey
    #mcitool.DBstring         = dbkey
    mcitool.COOLFolder       = DBUtils.getFolder(mcitool)
    _caliblog.info(' Performing *%s* MCNumInv for %s! (constants from DB)' %(preCalibState, dbkey) )

    # check if dbkey is know to be in DB
    #if dbfolder in MCNumInvConstants.existingKeysInDB :            
        # then we use the db
        #dbkey = DBConfig.loadJetFolder( dbfolder  )
        #mcitool.detStoreKey      = dbkey
        #_caliblog.info(' Performing *%s* NumInv for %s! (constants from DB)' %(preCalibState, dbkey) )        
    #else:
        # get constants from python file
        
        #jetname =  finder + editParm(mainParam) + input + "Jets" + flag + jetFlags.CalibrationDBSuffix()

        #_caliblog.info(' Performing *%s* NumInv for %s! (constants from file)' %(preCalibState,jetname) )

        #etaFit=[]

        #if jetname in MCNumInvConstants.etaFitDict:
            #_caliblog.info(' Input type %s found in dictionary!' %(jetname) )
            #etaFit = MCNumInvConstants.etaFitDict[jetname]
        #else:
            #_caliblog.warning(' NumInversion: Input type %s not found in dictionary!'%(jetname))
            #etaFit = MCNumInvConstants.etaFitDict['AntiKt4TopoJets']
        #mcitool.etaFit           = str(etaFit)
            
    # Enter Eta Bins... This is the correct format (don't start with zero, and don't insert 4.5 at the end)
    #etabins=[]
    #for i in range(1, 45):
         #etabins.append(i*0.1)

    mcitool.UseH1Calculation = False
    #mcitool.EtaBins          = [] # clear first to fix imcompatibility error
    #mcitool.EtaBins          = etabins
    mcitool.PtCut            = 10.
    mcitool.WriteMoments     = True
    mcitool.CalibrationTag   = calibTag
    mcitool.WriteMoments     = True

    return mcitool

def doJetOriginCorrection(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Correction for Jet Origin '''

    from JetCalibTools.JetCalibToolsConf import JetCaloOrigin

    # get base sig state on which num inv should be applied
    preCalibState = options["preCalibState"]
    
    myVertexCorr = JetCaloOrigin("JetCaloOrigin")

    # Write moments by default
    myVertexCorr.WriteMoments = True
    myVertexCorr.CalibrationTag = "Origin" # should not be hardcoded here ?

    # A lot of if statements for the moment!
    if 'Topo' in input and 'Tower' not in input:
        # Cluster based approach
        myVertexCorr.UseClusters = True
        myVertexCorr.UseGCW = False
        if preCalibState == 'GCW':
            myVertexCorr.UseSignalState = 2
        else: 
            myVertexCorr.UseSignalState = 0
        # Special case for LCTopo    
        if input == 'LCTopo':
            myVertexCorr.UseSignalState = 1
            myVertexCorr.UseLC = True
    else:
        # Cell/Tower based approach
        myVertexCorr.UseClusters = False
        myVertexCorr.UseSignalState = -1
        if preCalibState == 'GCW':
            myVertexCorr.UseGCW = True
        else:
            myVertexCorr.UseGCW = False

    # Not really implemented yet...
    myVertexCorr.UseJVA = False

    # Try primary vertex first, then attempt Beam Spot if the first doesn't work
    myVertexCorr.UsePrimaryVertex = True
    myVertexCorr.UseBeamSpot = True

    # H1-Calibrated Clusters
    if myVertexCorr.UseGCW == True:
        from CaloClusterCorrection.StandardCellWeightCalib import getCellWeightTool
        myVertexCorr.CellCalibrator = getCellWeightTool(finder, mainParam, input, onlyCellWeight=True)
    else:
        myVertexCorr.CellCalibrator = None

    # A default
    myVertexCorr.VertexContainerKey = "VxPrimaryCandidate"

    # This is *probably* not needed...
    # myVertexCorr.BeamConditionsSvc = None

    #myVertexCorr.OutputLevel = 2
    return myVertexCorr

def doJetSamplingCalibration(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard Sampling Layer correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
    
    from JetCalibTools.JetCalibToolsConf import JetSamplingCalibrator
    from JetRec.JetGetterBase import editParm
    
    jetname =  finder + editParm(mainParam) + input

    jetcalibrationtool2 = JetSamplingCalibrator("Layer")
    jetcalibrationtool2.CalibrationTag   = 'Layer'

    
    return jetcalibrationtool2

##This needs to be updated to use the 2011 tower based corrections. ##
def doEMOffset(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up 2011 Pile-up Tower-Based Offset Response Correction at EM scale '''

    # This tool can't work if trigger is off or if reading MC bytestream :
    from RecExConfig.RecFlags import rec
    from AthenaCommon.GlobalFlags import globalflags
    if not rec.doTrigger() or ( rec.readRDO() and globalflags.InputFormat()=='bytestream' and globalflags.DataSource()=='geant4' ) :
        _caliblog.warning("Trigger is off or reading MC bytestream. Can't apply Offset correction")
        return None
    
    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
    
    preCalibState = options["preCalibState"]
    #Import the LumiBlockMuTool
    from JetMomentTools.JetMomentToolsConf import NumTowerInJetTool
    numTowerTool = NumTowerInJetTool("PreCalibNumTower")
    options['precalibTools'].append( numTowerTool )


    from AthenaCommon.AppMgr import ToolSvc
    from IOVDbSvc.CondDB import conddb

    #import the BunchCrossingTool, does "theJob" need to be something else? 
    from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
    theBCTool = BunchCrossingTool()
    ToolSvc += theBCTool
    
#from JetCalibTools.TowerOffsetConstants import retrieveTowerConstants
#from JetCalibTools.TruthOffsetConstants import retrieveTruthConstants
    
    from JetCalibTools.JetCalibToolsConf import OffsetCorrection 
    from JetRec.JetGetterBase import editParm
    from JetRec.JetRecFlags import jetFlags
    from AthenaCommon.BeamFlags import jobproperties

    if conddb.isMC:
        isMCtmp=True
    else:
        isMCtmp=False
    ## MC10B, MC11A, MC11B, MC11C 
    ## This is used for the truth based correction, which is derived from MC. 


    mcTAG="MC11C"

    if(mcTAG=="MC10B"): 
      mcType = 0 
    if (mcTAG=="MC11A"): 
      mcType = 1 
    if (mcTAG=="MC11B"): 
      mcType = 2 
    if (mcTAG=="MC11C"):
      mcType = 3
    else:
      mcType = 2 

    #towerConstantsOOT, towerConstantsIT, towerConstantsOOT50  = retrieveTowerConstants(finder, mainParam, input, isMCtmp)
    #truthConstantsOOT, truthConstantsIT  = retrieveTruthConstants(finder, mainParam, input, preCalibState, isMCtmp, mcTAG)


    numTowerMoment = "" if "Tower" in input else "NumTowers"    

    offsetTool = OffsetCorrection("OffsetTool") 
    offsetTool.CalibrationTag        = "Offset"            # String specifying JetMoment name
    offsetTool.VertexContainerKey    = "VxPrimaryCandidate" # StoreGate key for the PV container
    offsetTool.MinimumPVTracks       = 2                    # Min. number of tracks for counting PV's                                     
#offsetTool.TowerBasedConstantsOOT = str(towerConstantsOOT) 
#offsetTool.TowerBasedConstantsIT = str(towerConstantsIT)  
#offsetTool.TowerBasedConstantsOOT50 = str(towerConstantsOOT50)

#offsetTool.TruthBasedConstantsOOT = str(truthConstantsOOT)   
#offsetTool.TruthBasedConstantsIT = str(truthConstantsIT)  

    # Hack, because there is only "Tower" and 0.4 input for the moment in constants
    #jetname =  finder + editParm(mainParam) + input + "Jets" + jetFlags.CalibrationDBSuffix()
    #jetname =  finder + "_4_" + "Tower" + "Jets" + jetFlags.CalibrationDBSuffix()
    if input == "TopoTower":
       input = "Tower" # not TopoTower, only Tower for TowerConstants
    if input == "Topo":
       if "LC" in precedingCalibs:
          input = 'LC'+input # adding correct prefix   
       else:   
          input = input+'_EM'
       isMCtmp = False    # no MC switch for TruthConstants
    jetname =  finder + editParm(mainParam) + input + "Jets"
    #jetname =  finder + "_4_" + "Tower" + "Jets" 

    #dbfolder = 'NumInv'+finder+editParm(mainParam)+dbinput+ jetFlags.CalibrationDBSuffix()
    if not DBUtils.configFolder(offsetTool):
        _caliblog.error(' Could not setup folder for tool: '+offsetTool.getFullName())
        return ''

    dbkey = DBUtils.getNameFromJetname(jetname)
    offsetTool.detStoreKey      = "OffsetCorrection_"+dbkey
    offsetTool.COOLFolder       = DBUtils.getFolder(offsetTool)
    _caliblog.info(' Performing OffsetCorrection for %s! (constants from DB)' %(dbkey) )




    offsetTool.BCTool = theBCTool
    offsetTool.WriteMoments   = True
    # New parameters for the 2011 offsets
    offsetTool.R              = mainParam
    offsetTool.DoTruthOffset  = True
    offsetTool.DoTowerOffset  = False
    offsetTool.IsMC           = isMCtmp
    offsetTool.MCType         = mcType
    offsetTool.NPVref         = 1.0
    offsetTool.MUref          = 0.0
    offsetTool.UseNumTowers   = False
    offsetTool.NumTowersMoment    = numTowerMoment
    offsetTool.DoOffsetComparison = False
    return offsetTool


# def doEMOffset(finder, mainParam, input, **options):
#     ''' Set-up standard Pile-up Tower-Based Offset Response Correction at EM scale '''
#     
#     options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
#     
#     from JetCalibTools.TowerOffsetConstants import retrieveConstants, isDummyConstants
#     from JetCalibTools.JetCalibToolsConf import OffsetCorrection 
#     from JetRec.JetGetterBase import editParm
#     from AthenaCommon.BeamFlags import jobproperties 
# 
#     useTower = ("Tower" in input) or ("Topo" in input)
#     numTowerMoment = "" if "Tower" in input else "NumTowers"
# 
#     constants , etaBins = retrieveConstants(finder, mainParam, input)
# 
#     if isDummyConstants(constants):
#         _caliblog.warning("""The configuration : %s
#         is not supported. NO OFFSET CORRECTION WILL BE APPLIED
#         """% str( (finder, mainParam, input) ))
# 
#     ## # Luminosity
#     bs   = jobproperties.Beam.bunchSpacing()
#     lumi = jobproperties.Beam.estimatedLuminosity()
#     ## if (bs==25  and lumi>=1.9E33 and lumi<=2.1E33):
#     ##     constants = towerOffsetDict['2E33_25ns'] 
#     ## elif (bs==450 and lumi>=0.9E32 and lumi<=1.1E32):
#     ##     constants = towerOffsetDict['1E32_450ns'] 
# 
#  
#     # Setup tool
#     offsetTool = OffsetCorrection("OffsetTool") 
#     offsetTool.CalibrationTag        = "Offset"            # String specifying JetMoment name
#     offsetTool.VertexContainerKey    = "VxPrimaryCandidate" # StoreGate key for the PV container
#     offsetTool.MinimumPVTracks       = 2                    # Min. number of tracks for counting PV's                                     
#     offsetTool.UseTowersPerJet       = useTower             # Use the per-jet tower multiplicity for calc. offset
#     offsetTool.BunchSpacing          = bs                   # Bunch spacing for this dataset
#     offsetTool.Luminosity            = lumi                 # Approximate lumi for this dataset
#     offsetTool.PresetConstituents    = False                # Turn ON / OFF an override in the per-jet tower multiplicity 
#     offsetTool.NumPresetConstituents = 42.0 if useTower else 1.0    # Specify per-jet constituent mult. override
# 
#     offsetTool.CalibConstants = str(constants)          # Calibration constants vs Npv and eta: [ eta1[Npv=1,Npv=2,Npv=3, ...], eta2[Npv=1,Npv=2,Npv=3, ...], ... ] 
#     offsetTool.EtaBins        = etaBins                 # Eta bins expected in the calibration constants array
#     offsetTool.NumTowerMoment = numTowerMoment          # the moment name containing the number of towers in this jet. if "", jet::size is used.
#     offsetTool.WriteMoments   = True
#     # New parameters for the 2011 offsets
#     offsetTool.R              = mainParam
#     offsetTool.DoNewOffset    = True
#     offsetTool.IsMC           = True
#     return offsetTool




def doTrackBasedJetCorrection(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Track-Based Jet Response Correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from JetCalibTools.JetCalibToolsConf import TrackBasedResponseCorrection
    #from JetCalibTools.TrackBasedConstants import TrackFitDict     # Calibration Constants
    from JetRec.JetGetterBase import editParm
    
    jetname =  finder + editParm(mainParam) + input + "Jets"


    #nTrackFit=[]

    #if jetname in TrackFitDict:
    #    nTrackFit = TrackFitDict[jetname]
    #else:
    #    print "WARNING: Jet algorithm %s is not supported in the current track-based response correction" % jetname

    # Eta Bins
    #etaBins = [0.8,1.4]

    # Setup the tool
    TrackCorrTool = TrackBasedResponseCorrection("TrackCorr")
    #TrackCorrTool.nTrackFit = str(nTrackFit)
    #TrackCorrTool.EtaBins = []
    #TrackCorrTool.EtaBins = str(etaBins)
    TrackCorrTool.CalibrationTag = "TrackCorr"
    if not DBUtils.configFolder(TrackCorrTool):
        _caliblog.error(' Could not setup folder for tool: '+offsetTool.getFullName())
        return ''

    dbkey = DBUtils.getNameFromJetname(jetname)
    TrackCorrTool.detStoreKey      = "TrackBasedResponseCorrection_"+dbkey
    TrackCorrTool.COOLFolder       = DBUtils.getFolder(TrackCorrTool)
    _caliblog.info(' Performing TrackBasedResponseCorrection for %s! (constants from DB)' %(dbkey) )
        
    return TrackCorrTool


def doMomentBasedCorrection(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Moment Based Correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    _caliblog.info("Configuring "+finder+" "+str(mainParam)+ " "+input)
    # prepare special jet moment :
    from JetMomentTools.JetMomentToolsConf import LowEtConstituentsFracTool
    ConstMomTool = LowEtConstituentsFracTool("LowEtConstituentsFrac")
    options['precalibTools'].append( ConstMomTool )


    ### setup conditions database folder ###

    from IOVDbSvc.CondDB import conddb

      
    DBConfig.loadJetFolder('HadCalibration2/CaloJetEnergyScale',baseFolder='/CALO',schema='CALO')


    ### setup jet calibration tool ###

    from JetCalibTools.JetCalibToolsConf import ConstituentsBasedJetCalibDBTool

    ConstCalibTool                   = ConstituentsBasedJetCalibDBTool()
    ConstCalibTool.JetMoment         = "LowEtConstituentsFrac"
    ConstCalibTool.CalibrationTag    = "CONST"
    ConstCalibTool.CorrectionKey     = 'JESCorrection'
    ConstCalibTool.WriteMoments      = True

    ### select jet collection ###

    if   finder == 'AntiKt' and mainParam == 0.4 and 'LCTopo' in input :
      ConstCalibTool.JetCollectionType = "AntiKt4LCTopo"
    elif finder == 'AntiKt' and mainParam == 0.6 and 'LCTopo' in input :
      ConstCalibTool.JetCollectionType = "AntiKt6LCTopo"
    else :
      _caliblog.warning(" No constants available: Using default constants for AntiKt4LCTopoJets!")


    return ConstCalibTool


def doTrackAngularCorrection(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard TrackAngularCorrection '''
    
    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
    
    # default constants
    #jetname =  finder + editParm(mainParam) + input + "Jets"
    # Only "AntiKt4TrackJets" exists so far
    jetname = "AntiKt4TrackJets"

    from JetCalibTools.JetCalibToolsConf import TrackAngularCorrection
    from JetRec.JetGetterBase import editParm

    trackangulartool = TrackAngularCorrection("TrackAngularCorrection"+finder+input)
    trackangulartool.CalibrationTag = "TrackAngularCorrection"
    
    # Track jet collection and delta R
    trackangulartool.DeltaR = 0.4
    trackangulartool.TrackJetCollectionKey = jetname
    
    
    _caliblog.info(' Performing *%s* TrackAngularCorrection for %s! ' %("AntiKt4TrackJets",jetname) )
    
    # Write moments/correct 4-momentum?
    trackangulartool.WriteMoments = True
    trackangulartool.OutputLevel = 3
    
    dbkey = DBUtils.getNameFromJetname(jetname)
    trackangulartool.detStoreKey      = "TrackAngularCorrection_"+dbkey
    trackangulartool.COOLFolder       = DBUtils.getFolder(trackangulartool)
    _caliblog.info(' Performing TrackAngularCorrection for %s! (constants from DB)' %(dbkey) )
        

    return trackangulartool



def doTowerCellCalibAtlfastII(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up standard H1 Cell Calibration '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)


    from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic
    from JetCalibTools.JetCalibToolsConf import JetCellCalibTool
    from JetRec.JetGetterBase import editParm

    from JetRec.JetRecFlags import jetFlags
    
    calibT = JetCellCalibTool("H1Cell")
    tname =  finder + editParm(mainParam) + input + jetFlags.CalibrationDBSuffix()
    cellcalibtool = H1WeightToolCSC12Generic("H1Weight"+tname)
    cellcalibtool.etaFit = str('[[]]')

    from JetCalibTools.AtlfastIICalibrationConstants import H1Coefficients01

    AtlfastIIH1 = H1Coefficients01()

    cellcalibtool.wtEMB0 = AtlfastIIH1.wtEMB0
    cellcalibtool.wtEMB1 = AtlfastIIH1.wtEMB1
    cellcalibtool.wtEMB2 = AtlfastIIH1.wtEMB2
    cellcalibtool.wtEME0 = AtlfastIIH1.wtEME0
    cellcalibtool.wtEME1 = AtlfastIIH1.wtEME1
    cellcalibtool.wtEME2 = AtlfastIIH1.wtEME2
    cellcalibtool.wtFCal1 = AtlfastIIH1.wtFCal1
    cellcalibtool.wtFCal2 = AtlfastIIH1.wtFCal2
    cellcalibtool.wtHec1 = AtlfastIIH1.wtHec1
    cellcalibtool.wtHec2 = AtlfastIIH1.wtHec2
    cellcalibtool.wtTile1 = AtlfastIIH1.wtTile1
    cellcalibtool.wtTile2 = AtlfastIIH1.wtTile2
    cellcalibtool.wtGap = AtlfastIIH1.wtGap
    cellcalibtool.wtCryo = AtlfastIIH1.wtCryo
    cellcalibtool.wtScint = AtlfastIIH1.wtScint
    
    calibT.CellCalibrator = cellcalibtool
    calibT.ApplyScaleFactor = False
    calibT.CalibrationTag = 'GCW'
    if options['GCWScheme'] != None:
        calibT.TowerMethod = options['GCWScheme']
    
    return calibT

def doTopoCellCalibAtlfastII(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Retrieve H1 Signal State for TopoClusters '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from CaloClusterCorrection.CaloClusterCorrectionConf import H1WeightToolCSC12Generic
    from CaloClusterCorrection.StandardCellWeightCalib import H1Calibration
    from JetCalibTools.JetCalibToolsConf import JetH1StateCryoCorr
    from JetRec.JetGetterBase import editParm    

    #if input == "TopoTower" or input == "Topo" :
    calibT = JetH1StateCryoCorr("RestoreH1Scale")
    calibT.CalibrationTag = 'GCW'
    if input=="Topo" : calibT.ConstituentState = 2 # constituents are cluster (2 is H1)
    else:                calibT.ConstituentState = 1 # constituents are jets (1 is final hence H1)
    calibT.CellCalibrator = None # should be re-set below in case of Topo

    if input=="Topo" :
        # cryostat correction is mandatory
        calibT.CorrectCryostat = True
        from JetRec.JetRecFlags import jetFlags
        tname =  finder + editParm(mainParam) + input + jetFlags.CalibrationDBSuffix()

        cellcalibtool = H1WeightToolCSC12Generic("H1Weight"+tname)
        cellcalibtool.etaFit = str('[[]]')

        from JetCalibTools.AtlfastIICalibrationConstants import H1Coefficients01

        AtlfastIIH1 = H1Coefficients01()

        cellcalibtool.wtEMB0 = AtlfastIIH1.wtEMB0
        cellcalibtool.wtEMB1 = AtlfastIIH1.wtEMB1
        cellcalibtool.wtEMB2 = AtlfastIIH1.wtEMB2
        cellcalibtool.wtEME0 = AtlfastIIH1.wtEME0
        cellcalibtool.wtEME1 = AtlfastIIH1.wtEME1
        cellcalibtool.wtEME2 = AtlfastIIH1.wtEME2
        cellcalibtool.wtFCal1 = AtlfastIIH1.wtFCal1
        cellcalibtool.wtFCal2 = AtlfastIIH1.wtFCal2
        cellcalibtool.wtHec1 = AtlfastIIH1.wtHec1
        cellcalibtool.wtHec2 = AtlfastIIH1.wtHec2
        cellcalibtool.wtTile1 = AtlfastIIH1.wtTile1
        cellcalibtool.wtTile2 = AtlfastIIH1.wtTile2
        cellcalibtool.wtGap = AtlfastIIH1.wtGap
        cellcalibtool.wtCryo = AtlfastIIH1.wtCryo
        cellcalibtool.wtScint = AtlfastIIH1.wtScint
    
        calibT.CellCalibrator = cellcalibtool




    return calibT
