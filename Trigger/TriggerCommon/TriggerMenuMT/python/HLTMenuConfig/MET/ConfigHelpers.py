# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Helper functions for configuring MET chains
"""

from ..Menu.SignatureDicts import METChainParts_Default, JetChainParts_Default

# The keys from the MET chain dict that directly affect reconstruction
# The order here is important as it also controls the dict -> string conversion
recoKeys = ["EFrecoAlg", "calib", "addInfo"]

def extractMetRecoDict(chainDict, fillDefaults=True):
    """ Extract the keys relevant to reconstruction from a provided dictionary

    If fillDefaults is True then any missing keys will be taken from the
    METChainParts_Default dictionary.
    """
    if fillDefaults:
        return {k: chainDict.get(k, METChainParts_Default[k])
                for k in recoKeys}
    else:
        return {k: chainDict[k] for k in recoKeys if k in chainDict}

def metRecoDictToString(recoDict, skipDefaults=True):
    """ Convert a dictionary containing reconstruction keys to a string

    Any key (from recoKeys) missing will just be skipped.
    If skipDefaults is True then any key whose value is the default one will
    also be skipped.
    """
    return "_".join(
            recoDict[k] for k in recoKeys if 
            k in recoDict and 
            (not skipDefaults or recoDict[k] != METChainParts_Default[k]) )

def jetRecoDictForMET(**recoDict):
    """ Get a jet reco dict that's usable for the MET slice """
    keys = ["recoAlg", "dataType", "calib", "jetCalib", "trkopt", "cleaning"]
    return {k: recoDict.get(k, JetChainParts_Default[k]) for k in keys}
