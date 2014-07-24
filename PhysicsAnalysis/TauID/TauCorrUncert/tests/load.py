# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

SHARE_DIR = os.getenv('TCU_SHARE_DIR', None)
if SHARE_DIR is None:
    raise RuntimeError("Please source setup.sh first.")

import ROOT
ROOT.gSystem.Load("libTauCorrUncert.so")
from ROOT import TauCorrUncert
