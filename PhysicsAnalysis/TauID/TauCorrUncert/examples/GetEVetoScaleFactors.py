# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

def main():
  ROOT.gSystem.Load( "libTauCorrUncert" )
  from ROOT import TauCorrUncert as TCU
  # Step 1. Instantiate tool
  #############/
  tc = TCU.TauSF("../share/")
  # Step 2. Retrieving tau ID scale factors
  # Note: those SF must be only applied to truth matched taus
  ##########################
  """
  The cast of enum to ROOT.Long is a temprary bug in ROOT and will be fixed soon in patched 5.34
  """
  print "SF for BDT medium medium eveto loose olr: ", tc.GetEVetoSF(1.2, TCU.BDTMEDIUM, TCU.MEDIUM, TCU.LOOSEPP), "  +  ", tc.GetEVetoSFUnc(1.2, TCU.BDTMEDIUM, TCU.MEDIUM, TCU.LOOSEPP, 1)
  print "SF for BDT medium medium eveto loose olr 3P: ", tc.GetEVetoSF(1.2, 3, TCU.BDTMEDIUM, TCU.MEDIUM, TCU.LOOSEPP), "  +  ", tc.GetEVetoSFUnc(1.2, 3, TCU.BDTMEDIUM, TCU.MEDIUM, TCU.LOOSEPP, 1)
  
if __name__=='__main__':
  main()


