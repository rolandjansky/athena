# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##-----------------------------------------------------------------------
## \author  P.A. Delsart
## \author  P.O. DeViveiros
## \date    Summer 2009: Lay the framework for calibSequences
## \date    Summer 2010: Switch to parser based approach
##
## \file MakeCalibSequences.py
##
## \brief Functions used to manage the Standard Jet Calibration Sequences
##
## getStandardCalibSequence:
##    Uses a parser to recognize a sequence of JetCalibrators.  The parser
##    expects something of the format:
##      'SCALE:TOOL_TOOL'
##    Where SCALE is the name of the sigState one applies the correction to
##    And   TOOL  is a calibrator from the calibTags lit
##
## makeCalibSequence:
##    Generates a JetCalibSequence from a list of JetCalibTools.
##
##----------------------------------------------------------------------------

from JetRec.JetAlgConfiguration import checkAndUpdateOptions,nameBuilder
from JetCalibTools.SetupJetCalibrators import *
from JetCalibTools.SetupCaloFractionCalibrators import *
from JetCalibTools.SetupWidthCalibrators import *

# What the statenames mean
stateNames = { 'EM' : 0, 'CAL' : 1, 'LC' : 2, 'GCW' : 1}

# Which tool each calibTag refers to
calibTags = {

    # Cell-weighting and MC-based
    'GCW'     : doH1CellCalib,      # Global Cell Weighting
    'JES'     : doMCNumInversion,   # MC-based Numerical Inversion
    'H1SCALE' : doH1ScaleFactor,    # Old-Style H1 Scale Factor
    'ETAJES'  : doEtaJESCorrection, # MC-based Scale Factor + Eta Correction

    # Various Layer Fraction Set-Ups
    'LAYER'   : [doTile1Fraction, doHec0Fraction, doEm3FractionForward, doFcal1Fraction, doEm3FractionBarrel, doPresamplerFractionBarrel],
    'LAYER1'  : [doTile1Fraction, doHec0FractionEndcap, doEm3FractionForward, doFcal1Fraction],
    'LAYER2'  : [doTile1Fraction, doHec0Fraction, doEm3FractionForward, doFcal1Fraction, doEm3FractionBarrel],
    'LAYER3'  : [doTile1Fraction, doHec0Fraction, doEm3FractionForward, doFcal1Fraction, doEm3FractionBarrel, doEm3FractionEndcap,
                doPresamplerFraction, doEm2FractionTransition, doEm2FractionEndcap],

    # Jet Structure Set-Ups
    'STRUCT'  : [doTile1Fraction, doHec0Fraction, doEm3FractionForward, doFcal1Fraction, doEm3FractionBarrel, doEm3FractionEndcap,
                 doPresamplerFraction, doEm2FractionTransition, doEm2FractionEndcap, doWidth],
    'STRUCT1' : [doTile1Fraction, doHec0Fraction, doEm3FractionForward, doFcal1Fraction, doEm3FractionBarrel, doPresamplerFractionBarrel,
                 doWidth ],

    # Origin Correction
    'ORIGIN'  : doJetOriginCorrection,

    # Offset Correction
    'OFFSET'   : doEMOffset,

    # Track Angular Correction
    'TRANG'   : doTrackAngularCorrection,

    # Track Based JES Correction
    'TRK'     : doTrackBasedJetCorrection,

    # Constituent Moment Based
    'CONST'   : doMomentBasedCorrection
    }

_caliblog = logging.getLogger( 'JetCalibSequence' )


def getStandardCalibSequence(calibName, input,**options):
    options=checkAndUpdateOptions(calibName=calibName,input=input,**options)
    return parseCalibSequence(**options)

def makeCalibSequence(name, toollist, writeMoments=False, baseSigState=1, momentTag="", writeSeqMoments=False, **options):
    ''' Creates a JetCalibSequence from the given tool list '''

    options=checkAndUpdateOptions(**options)

    from JetCalibTools.JetCalibToolsConf import JetCalibSequence
    myJetSequence = JetCalibSequence(name)
    # Write moments for all subtools, if requested
    if writeMoments :
        for calibrator in toollist:
            calibrator.WriteMoments = True
    myJetSequence.CalibToolSequence = toollist
    myJetSequence.BaseSignalState = baseSigState
    myJetSequence.MomentTag = name if momentTag=="" else momentTag
    myJetSequence.WriteMoments = writeSeqMoments
    return myJetSequence

def parseCalibSequence(calibName, input, **options):
    ''' Parse a set of calib tags and transform into a calibSequence '''

    options=checkAndUpdateOptions( **options)

    seqTools = []

    options['input']=input
    options['calibName']=calibName
    
    
    preCalibState, calibName = calibName.split(':')
    baseSigState  = stateNames.get(preCalibState, 1 )

    options['preCalibState'] = preCalibState

    calibList = calibName.split('_')
        
    # Error!!
    if len(calibList) == 0:
        return None

    finalCalibList = [ preCalibState ]
    rejectedCalib  = []


    # loop over calibration tags
    for calibTag in calibList:
        # Copy options for this particular calibTag
        myOptions = dict(options)
        myOptions['precedingCalibs'] = finalCalibList

        calibTool = None
        
        if calibTag  in calibTags:
            calibrator = calibTags[calibTag]
            if calibrator is None :
              pass  
            elif isinstance(calibrator, list):
                subtools = [ individualTool(**myOptions)  for individualTool in calibrator ]
                subsequence  = makeCalibSequence(calibTag+'SubSequence', subtools, **myOptions)
                subsequence.BaseSignalState = 1
                calibTool = subsequence
            else:
                calibTool = calibrator(**myOptions)
        else :
            _caliblog.warning("Requested jet calibration tag %s unknown ! "%(calibTag,))

        # check this calib tool has been properly set-up
        if calibTool is None :
            rejectedCalib.append( calibTag  )
        else:
            seqTools += [ calibTool ]
            finalCalibList.append( calibTag  )


    # Issue warning for all rejected calibs :
    for tag in rejectedCalib:
        _caliblog.warning("Set-up error for calibration tag %s. Corresponding jet calib ignored" %(tag,))

    # rebuild the calib name 
    options['calibName']= preCalibState+':'+'_'.join(finalCalibList[1:]) 
    
    options['baseSigState'] = baseSigState            
    # create the sequence by passing the list of JetCalibTag tools
    return makeCalibSequence(options['calibName'], seqTools, **options)



def alternateCalibSequence(seq, optionDic):
    """ Create a calib sequence, preparing it to not apply correction, but store moments in jets.
    seq is a string describing a calib sequence.
    The optionDic is a dictionnary of option, it is modified by this functions.
    
    returns a JetCalibSequence
    """
    optionDic['writeMoments'] = True # make sure each tool moments are written out.        
    optionDic['calibName']    = seq
    optionDic['inPlaceCorr']  = False
    return getStandardCalibSequence( **optionDic)
    


def indexOfCalibTool(sequenceStr, tag):
    """ return the index of tag within the calibration sequence sequenceStr"""
    n, tags = sequenceStr.split(':')
    tagList = tags.split('_')
    return tagList.index(tag)
