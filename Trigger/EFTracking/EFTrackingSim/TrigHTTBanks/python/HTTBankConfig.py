# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

'''
@author Riley Xu - rixu@cern.ch
@date Feb 6th 2020
@brief This file declares functions to make and configure the bank service.
'''

from TrigHTTBanks.TrigHTTBanksConf import TrigHTTBankSvc
from TrigHTTConfTools import HTTConfigCompInit

import os

def addBankSvc(mapTag, bankTag):
    '''
    Creates and returns a TrigHTTBankSvc object, configured with the supplied tag.

    This function adds the returned bank service
    instance to SvcMgr.
    '''

    from AthenaCommon.AppMgr import ServiceMgr
    HTTBankSvc = TrigHTTBankSvc()

    _applyTag(HTTBankSvc, mapTag, bankTag)

    if "EOS_MGM_URL_HTT" in os.environ: # Set by the gitlab yaml
        _applyGitlabOverrides(HTTBankSvc)

    ServiceMgr += HTTBankSvc

    return HTTBankSvc


def _applyTag(HTTBankSvc, mapTag, bankTag):
    '''
    Helper function that sets the filepaths of the BankSvc using the supplied tag.
    '''

    bankDir = bankTag['bankDir']

    filepaths = [
            'constants_1st',
            'constants_2nd',
            'sectorBank_1st',
            'sectorBank_2nd',
            'sectorSlices'
    ]
    filelists = [
            'constantsNoGuess_1st',
            'constantsNoGuess_2nd',
    ]

    formats = {
            'region': HTTConfigCompInit.getRegionIndex(mapTag),
            'regionName': mapTag['regionNames'][HTTConfigCompInit.getRegionIndex(mapTag)],            
    }

    for param in filepaths:
        if bankTag['formatted']:
            value = (bankDir + bankTag[param]).format(**formats)
        else:
            value = bankDir + bankTag[param]
        setattr(HTTBankSvc, param, value)

    for param in filelists:
        if bankTag['formatted']:
            value = [(bankDir + path).format(**formats) for path in bankTag[param]]
        else:
            value = [bankDir + path for path in bankTag[param]]
        setattr(HTTBankSvc, param, value)



def _applyGitlabOverrides(HTTBankSvc):
    '''
    Alters the filepaths for running on Gitlab CI
    '''

    # The non-root files need to be copied to the pwd
    for param in ['sectorBank_1st', 'sectorBank_2nd', 'constants_1st', 'constants_2nd']:
        filepath = getattr(HTTBankSvc, param)
        newpath = os.path.basename(filepath)

        # This doesn't work for some reason, gets a file not found
        #import subprocess
        #subprocess.call(['xrdcp', '-f', "'" + os.environ["EOS_MGM_URL_HTT"] + filepath + "'", "'" + newpath + "'"])
        setattr(HTTBankSvc, param, newpath)

    for param in ['sectorSlices']:
        setattr(HTTBankSvc, param, os.environ["EOS_MGM_URL_HTT"] + getattr(HTTBankSvc, param))
