# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
_caliblog = logging.getLogger( 'JetRecalibrator' )

def _guessInputFromCollectionName(collName):
    for i in ['LCTopo','Tower','Topo']: # the order matters !
        if i in collName:
            return i
    print "Error could not guess input type in ",collName            
    return

def _guessFinderFromCollectionName(collName):
    for a in [ 'AntiKt','CamKt','Kt', 'Cone','SISCone','CMSCone']: # the order matters !
        if collName.startswith(a):
            return a
    print "Error could not guess jet finder type in ",collName
    return 

def _guessMainParamFromCollectionName(collName):
        # get the 2 chars following finder :
        finder = _guessFinderFromCollectionName(collName)
        mp = collName[len(finder):len(finder)+2]
        mp = mp[0] if not mp[1] in '0123456789' else mp
        try :
            return float(mp)/10.
        except ValueError :
            print "Error could not guess main parameter in ",collName
            return

    
def guessJetParamFromCollectionName(collName, finder = None,input=None, mainParam=None):
    

    if not finder : finder =  _guessFinderFromCollectionName(collName)
    if not input  : input =  _guessMainParamFromCollectionName(collName)
    if not mainParam : mainParam = _guessInputFromCollectionName(collName)
    return finder, input, mainParam
    

def makeJetRecalibrator(  collName, calibSeq, finder = None,input=None, mainParam=None):
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from JetCalibTools.MakeCalibSequences import alternateCalibSequence , getStandardCalibSequence
    from JetCalibTools.JetCalibToolsConf import JetRecalibrator
    from JetRec.JetAlgConfiguration import checkAndUpdateOptions
    #d = {"input" :"Topo"}
    finder , mainParam, input = guessJetParamFromCollectionName( collName, finder=finder, input=input, mainParam=mainParam)
    if not all( (finder , mainParam, input ) ):
        _caliblog.error( " Can not guess what type of jet algorithm is ",collName)
    #calT = getStandardCalibSequence(calibSeq, checkAndUpdateOptions(input = input, finder= finder, mainParam=mainParam) )
    d = checkAndUpdateOptions(input = input, finder= finder, mainParam=mainParam, )
    calT = getStandardCalibSequence(calibSeq, input, finder= finder, mainParam=mainParam )
    #calT.OutputLevel = 2
    recal = JetRecalibrator(collName+calibSeq, JetCollection = collName)
    recal.CalibrationTool = calT


    _caliblog.warning( " Jet Collection "+collName +" will be recalibrated with "+calibSeq+ " !!! " )

    topSequence += recal
    
    return recal
