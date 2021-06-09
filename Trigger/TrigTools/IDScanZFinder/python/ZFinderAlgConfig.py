# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigEDMConfig.TriggerEDMRun3 import recordable
from IDScanZFinder.IDScanZFinderConf import TrigZFinderAlg
MinBiasZFinderAlg = TrigZFinderAlg("TrigZFinderAlg", zFinderKey=recordable("HLT_vtx_z"))
# TODO add monitoring tool and two histograms for weights and the z