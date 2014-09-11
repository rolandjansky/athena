# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##-----------------------------------------------------------------------
## \author  David Lopez Mateos
## \date    November 17 2009
## \updated Pavol Strizenec, 21. October 2011 for DB setup
##
## \file SetupWidthCalibrators.py
##
## \brief Functions used to setup the different JetCalibrators from width
##
## do***:
##    Standard configurations for the JetCalibrators used in the Sequences,
##    dependent on finder, cone, input.
##
##----------------------------------------------------------------------------

from JetRec.JetAlgConfiguration import checkAndUpdateOptions
import JetCalibTools.DBUtils as DBUtils

def doWidth(finder, mainParam, input, precedingCalibs = [], **options):
    ''' Set-up Width correction '''

    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)

    from JetCalibTools.JetCalibToolsConf import JetPropertyInvertResponseTool
    from JetRec.JetGetterBase import editParm
    
    jetname =  finder + editParm(mainParam) + input + "Jets"

            
    widthTool =  JetPropertyInvertResponseTool("WidthTool")
    widthTool.PropertyName      = "width"
    widthTool.CalibrationTag    = 'WidthFraction'

    if not DBUtils.configFolder(widthTool):
        _caliblog.error(' Could not setup folder for tool: '+widthTool.getFullName())
        return ''

    widthTool.detStoreKey      = widthTool.name()+"_"+DBUtils.getNameFromJetname(jetname)
    widthTool.COOLFolder       = DBUtils.getFolder(widthTool)

    return widthTool


