# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from load import SHARE_DIR, TauCorrUncert
import ROOT


def get_file(fname):
    """
    Retrieve a file from the share directory.
    """
    full_path = os.path.join(SHARE_DIR, fname)
    if not os.path.isfile(full_path):
        raise IOError("%s is not a file" % full_path)
    root_file = ROOT.TFile.Open(full_path)
    if not root_file:
        raise IOError("Could not open %s" % full_path)
    return root_file
