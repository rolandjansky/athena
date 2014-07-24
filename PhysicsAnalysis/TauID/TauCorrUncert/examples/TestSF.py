# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

def main():
  ROOT.gSystem.Load( "libTauCorrUncert" )
  from ROOT import TauCorrUncert as TCU

  # Step 1. Instantiate tool
  #############/
  tc = ROOT.TauCorrUncert.TauSF("../share")
  # Step 2. Retrieving tau ID scale factors
  # Note: those SF must be only applied to truth matched taus
  # Note: currently only inclusive scale factors are provided, hence dummy variables are passed as pT, and eta
  ##########################
  """
  The cast of enum to ROOT.Long is a temprary bug in ROOT and will be fixed soon in patched 5.34
  """
  print "SF for BDT loose 1-prong taus: ", tc.GetIDSF(ROOT.Long(TCU.BDTLOOSE) , 2.2, 1), "  +-  ", tc.GetIDStatUnc(ROOT.Long(TCU.BDTLOOSE) , 2.2, 1)
  print "SF for BDT loose 3-prong taus: ", tc.GetIDSF(ROOT.Long(TCU.BDTLOOSE) , 2.2, 3), "  +-  ", tc.GetIDStatUnc(ROOT.Long(TCU.BDTLOOSE) , 2.2, 3)
  print "SF for BDT medium 1-prong taus: ", tc.GetIDSF(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 1), "  +-  ", tc.GetIDStatUnc(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 1)
  print "SF for BDT medium 3-prong taus: ", tc.GetIDSF(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 3), "  +-  ", tc.GetIDStatUnc(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 3)
  print "SF for BDT tight 1-prong taus: ", tc.GetIDSF(ROOT.Long(TCU.BDTTIGHT) , 2.2, 1), "  +-  ", tc.GetIDStatUnc(ROOT.Long(TCU.BDTTIGHT) , 2.2, 1)
  print "SF for BDT tight 3-prong taus: ", tc.GetIDSF(ROOT.Long(TCU.BDTTIGHT) , 2.2, 3), "  +-  ", tc.GetIDStatUnc(ROOT.Long(TCU.BDTTIGHT) , 2.2, 3)

  """
  Exclusive scale factors are NOT recommended!!!
  Check README
  """
  
  """
  print "SF for BDT loose 1-prong taus: ", tc.GetExclIDSF(ROOT.Long(TCU.BDTLOOSE) , 2.2, 1), "  +-  ", tc.GetExclIDStatUnc(ROOT.Long(TCU.BDTLOOSE) , 2.2, 1)
  print "SF for BDT loose 3-prong taus: ", tc.GetExclIDSF(ROOT.Long(TCU.BDTLOOSE) , 2.2, 3), "  +-  ", tc.GetExclIDStatUnc(ROOT.Long(TCU.BDTLOOSE) , 2.2, 3)
  print "SF for BDT medium 1-prong taus: ", tc.GetExclIDSF(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 1), "  +-  ", tc.GetExclIDStatUnc(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 1)
  print "SF for BDT medium 3-prong taus: ", tc.GetExclIDSF(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 3), "  +-  ", tc.GetExclIDStatUnc(ROOT.Long(TCU.BDTMEDIUM) , 2.2, 3)
  print "SF for BDT tight 1-prong taus: ", tc.GetExclIDSF(ROOT.Long(TCU.BDTTIGHT) , 2.2, 1), "  +-  ", tc.GetExclIDStatUnc(ROOT.Long(TCU.BDTTIGHT) , 2.2, 1)
  print "SF for BDT tight 3-prong taus: ", tc.GetExclIDSF(ROOT.Long(TCU.BDTTIGHT) , 2.2, 3), "  +-  ", tc.GetExclIDStatUnc(ROOT.Long(TCU.BDTTIGHT) , 2.2, 3)
  """

if __name__ == '__main__':
  main()
