# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##-----------------------------------------------------------------------
## \author  David Lopez Mateos
## \date    Auguest 13 2009
## \updated Pavol Strizenec, 21. October 2011 for DB setup
##
## \file SetupCaloFractionCalibrators.py
##
## \brief Functions used to setup the different JetCalibrators from LayerFractionTool
##
## do***:
##    Standard configurations for the JetCalibrators used in the Sequences,
##    dependent on finder, cone, input.
##
##----------------------------------------------------------------------------

from JetRec.JetAlgConfiguration import checkAndUpdateOptions
import JetCalibTools.DBUtils as DBUtils


def doFraction(name, pName, cTag, finder, mainParam, input):
    ''' General setup function for Fraction corrections '''
    from JetCalibTools.Tile1FractionConstants import tile1EtaFitDict
    from JetCalibTools.Tile1FractionConstants import tile1EtaBinsDict
    from JetCalibTools.Tile1FractionConstants import tile1PtBinsDict
    from JetCalibTools.Tile1FractionConstants import tile1LayerFracBinsDict

    #options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from JetCalibTools.JetCalibToolsConf import JetPropertyInvertResponseTool
    from JetRec.JetGetterBase import editParm

    jetname =  finder + editParm(mainParam) + input + "Jets"

    layerFractionTool =  JetPropertyInvertResponseTool(name)
    layerFractionTool.PropertyName      = pName
    layerFractionTool.CalibrationTag    = cTag

    if not DBUtils.configFolder(layerFractionTool):
        _caliblog.error(' Could not setup folder for tool: '+layerFractionTool.getFullName())
        return ''

    layerFractionTool.detStoreKey      = layerFractionTool.name()+"_"+DBUtils.getNameFromJetname(jetname)
    layerFractionTool.COOLFolder       = DBUtils.getFolder(layerFractionTool)

    return layerFractionTool


def doTile1Fraction(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Tile1 fraction correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
            
    return doFraction("Tile1FractionTool", "tile1", "Tile1Fraction", finder, mainParam, input)


def doHec0FractionEndcap(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up HEC0 fraction correction for endcap only '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
            
    return doFraction("HEC0EndcFractionTool", "hec0", "HEC0FractionEndc", finder, mainParam, input)
    

def doHec0Fraction(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up HEC0 fraction correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("HEC0FractionTool", "hec0", "HEC0Fraction", finder, mainParam, input)


def doEm3FractionBarrel(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up EM3 fraction correction for the barrel '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
            
    return doFraction("EM3BarrFractionTool", "em3", "EM3FractionBarr", finder, mainParam, input)


def doEm3FractionEndcap(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up EM3 fraction correction for the endcap '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
            
    return doFraction("EM3EndcFractionTool", "em3", "EM3FractionEndc", finder, mainParam, input)
    

def doEm3FractionForward(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up EM3 fraction correction for the forward region '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("EM3FwdFractionTool", "em3", "EM3FractionFwd", finder, mainParam, input)


def doFcal1Fraction(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up FCAL1 fraction correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("FCAL1FractionTool", "fcal1", "FCAL1Fraction", finder, mainParam, input)


def doPresamplerFractionTransition(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Presampler fraction correction in transition region'''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("PresamplerTransFractionTool", "pres", "PresamplerFractionTrans", finder, mainParam, input)


def doPresamplerFraction(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Presampler fraction correction'''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("PresamplerFractionTool", "pres", "PresamplerFraction", finder, mainParam, input)


def doPresamplerFractionBarrel(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Presampler fraction correction'''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("PresamplerFractionTool", "pres", "PresamplerFractionBarr", finder, mainParam, input)


def doEm2FractionTransition(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Em2 fraction correction in transition region '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("EM2TransitionFractionTool", "em2", "EM2FractionTrans", finder, mainParam, input)


def doEm2FractionSmallTransition(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Em2 fraction correction in small transition region '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("EM2SmallTransitionFractionTool", "em2", "EM2FractionSmallTrans", finder, mainParam, input)


def doEm2FractionEndcap(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Em2 fraction correction in end-cap region '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    return doFraction("EM2EndcapFractionTool", "em2", "EM2FractionEndc", finder, mainParam, input)

