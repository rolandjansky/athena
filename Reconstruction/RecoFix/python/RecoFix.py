# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""@package RecoFix 

This package is the only one meant to be used externally (mainly by
RecExCommon). Only the RecoFix_* functions should be called.

When adding a new RecoFix for a release family (where a release family
is defined by the first two numbers in a release, e.g., 20.1) that
does not yet have RecoFix, the user needs to create a new
RecoFix_rXXY.py file with an RecoFix_rXXY class inheriting from the
RecoFix_base class and overwrite the appropriate member
functions. Furthermore, the RecoFix_Init() function below needs to set
_aodFixInstance approprately, as explained in the comments below.

When modifying the RecoFix for a release family for which
RecoFix_rXXY.py exists, most likely the changes would be restricted to
the RecoFix_rXXY.py class. The only exception is if the release family
definition changes (e.g., add 17.3 to the 17.2 family).

"""

from AthenaCommon.Logging import logging

logRecoFix = logging.getLogger( 'RecoFix' )

from RecoFix_base import RecoFix_base
from RecoFix_r210 import RecoFix_r210

_recoFixInstance = RecoFix_base(False)

def run_once(f):
    def wrapper(*args, **kwargs):
        if not wrapper.has_run:
            wrapper.has_run = True
            return f(*args, **kwargs)
    wrapper.has_run = False
    return wrapper

@run_once
def RecoFix_Init():
    ''' This function is called to correctly setup RecoFix.'''

    global _recoFixInstance

    from RecExConfig.RecFlags import rec

    ##################
    # exit if locked to false
    ##################
    if rec.doApplyRecoFix.is_locked() and rec.doApplyRecoFix.get_Value() == False:
        logRecoFix.info("doApplyRecoFix is locked to False; not applying anything")
        return


    ##################
    # deterimine in what release we are running (and fill some rec variables
    ##################

    import os
    curRelease=os.getenv("AtlasVersion") or ''

    logRecoFix.debug("curRelease set to " + curRelease)
    
    
    ##################
    # determine athena with which input file was created and previous RecoFix
    ##################
   
    # RDO doesn't have MetaData
    prevAODFix = ''
    if not rec.readRDO():

        ##################
        # determine which is the previous verion of the AODFix
        ##################
        try:
            prevAODFix=inputFileSummary['tag_info']['AODFixVersion']
            if prevAODFix.startswith("AODFix_"):
                prevAODFix=prevAODFix[7:]
                prevAODFix=prevAODFix.split("_")[0]
        except Exception:
            logRecoFix.debug("no AODFixVersion found in input file, setting to <none>.")
            prevAODFix=''

    logRecoFix.info("Summary of MetaData for RecoFix:")
    logRecoFix.info("AODFix version <%s> was previously applied." % prevRecoFix)

    ##################
    # determine which RecoFix to run (if actually running--to be determined later)
    ##################

    doRecoFix = False
    if rec.doESD():
        # running from RAW or ESD, set recofix metadata
        logRecoFix.debug("detected doESD=True, applying RecoFix metadata")
        doRecoFix = True

    #exit if not doing RecoFix
    if not doRecoFix:
        return  

    curReleaseSplit = curRelease.split('.')
    if len(curReleaseSplit) >= 2:
        ### If adding an RecoFix for a release family that does not have it,
        ### please add it to the if-elif... statement below
        if (curReleaseSplit[0] == '21' and (curReleaseSplit[1] == '0' or curReleaseSplit[1] == '2'):
            _recoFixInstance = RecoFix_r210()
        else:
            logRecoFix.info("No RecoFix scheduled for this release.")

    else:
        logRecoFix.info("Current release is of strange form: %s" % curRelease)

# The interface routines
def RecoFix_addMetaData():
    '''This adds metadata both when AODFix is run or when "fixed" reco is performed'''
    _recoFixInstance.addMetaData()
