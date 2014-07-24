# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

def main():
  ROOT.gSystem.Load( "libTauCorrUncert" )
  from ROOT import TauCorrUncert as TCU

  # Step 1. Instantiate tool
  #############/
  tc = ROOT.TauCorrUncert.TESUncertainty("../share/TES/mc12_p1344_medium.root")
  # Step 2. 
  ##########################
  print "Unc pt 41819, 1p, eta 1.91013: ", tc.GetTESUncertainty(41819, 1.91013, 1)
  print "Unc pt 10, 1p, eta 0.4: ", tc.GetTESUncertainty(10000., 0.4, 1, ROOT.Long(tc.SYSREST))
  print "Unc pt 60, 1p, eta 0.4: ", tc.GetTESUncertainty(60000., 0.4, 1, ROOT.Long(tc.SYSREST))
  print "Unc pt 80, 1p, eta 0.4: ", tc.GetTESUncertainty(80000., 0.4, 1, ROOT.Long(tc.SYSREST))
  print "Unc pt 100, 1p, eta 0.4: ", tc.GetTESUncertainty(100000., 0.4, 1, ROOT.Long(tc.SYSREST))

  print "Unc pt 10, 1p, eta 0.4: ", tc.GetTESUncertainty(10000., 0.4, 1, ROOT.Long(tc.INSITUSTAT))
  print "Unc pt 60, 1p, eta 0.4: ", tc.GetTESUncertainty(60000., 0.4, 1, ROOT.Long(tc.INSITUSTAT))
  print "Unc pt 80, 1p, eta 0.4: ", tc.GetTESUncertainty(80000., 0.4, 1, ROOT.Long(tc.INSITUSTAT))

  print "Unc pt 10, 1p, eta 0.4: ", tc.GetTESUncertainty(10000., 0.4, 1, ROOT.Long(tc.TOTAL))
  print "Unc pt 60, 1p, eta 0.4: ", tc.GetTESUncertainty(60000., 0.4, 1, ROOT.Long(tc.TOTAL))
  print "Unc pt 80, 1p, eta 0.4: ", tc.GetTESUncertainty(80000., 0.4, 1, ROOT.Long(tc.TOTAL))

  print "In-situ syst Unc pt 80, 1p, eta 0.4: ", tc.GetTESUncertainty(80000., 0.4, 1, ROOT.Long(tc.INSITUSYST))
  print "In-situ syst Unc pt 40, 1p, eta 0.4: ", tc.GetTESUncertainty(40000., 0.4, 1, ROOT.Long(tc.INSITUSYST))
  print "In-situ syst Unc pt 80, 3p, eta 0.4: ", tc.GetTESUncertainty(80000., 0.4, 3, ROOT.Long(tc.INSITUSYST))
  print "In-situ syst Unc pt 40, 3p, eta 0.4: ", tc.GetTESUncertainty(40000., 0.4, 3, ROOT.Long(tc.INSITUSYST))
  
  print "alphy pt 40, 1p, eta 0.4: ", tc.GetTESShift(40000., 1)
  print "alphy pt 60, 1p, eta 0.4: ", tc.GetTESShift(60000., 1)
  print "alphy pt 80, 1p, eta 0.4: ", tc.GetTESShift(80000., 1)

  h_total = ROOT.TGraph(100)
  h_insitustat = ROOT.TGraph(100)
  h_insitusys = ROOT.TGraph(100)
  h_insitutotal = ROOT.TGraph(100)
  h_alpha = ROOT.TGraph(100)
  pt_range = [40. + 0.2*i for i in range(0, 200)]
  point = 0
  for pt in pt_range:
    h_total.SetPoint(point, pt, tc.GetTESUncertainty(pt * 1000., 0.4, 1, ROOT.Long(tc.TOTAL)))
    h_insitustat.SetPoint(point, pt, tc.GetTESUncertainty(pt * 1000., 0.4, 1, ROOT.Long(tc.INSITUSTAT)))
    h_insitusys.SetPoint(point, pt, tc.GetTESUncertainty(pt * 1000., 0.4, 1, ROOT.Long(tc.INSITUSYST)))
    h_insitutotal.SetPoint(point, pt, tc.GetTESUncertainty(pt * 1000., 0.4, 1, ROOT.Long(tc.INSITUINTERPOL)))
    h_alpha.SetPoint(point, pt, tc.GetTESShift(pt * 1000., 1))
    point += 1
  c = ROOT.TCanvas('c','c')
  c.cd()
  h_total.Draw('ap')
  h_total.SetMarkerStyle(21)
  c.SaveAs('TESTotal.eps')
  h_insitustat.Draw('ap')
  h_insitustat.SetMarkerStyle(21)
  c.SaveAs('TESInSituStat.eps')
  h_insitusys.Draw('ap')
  h_insitusys.SetMarkerStyle(21)
  c.SaveAs('TESInSituSyst.eps')
  h_insitutotal.Draw('ap')
  h_insitutotal.SetMarkerStyle(21)
  c.SaveAs('TESInSituTotal.eps')

  h_alpha.Draw('ap')
  h_alpha.SetMarkerStyle(21)
  c.SaveAs('TESAlphaShift.eps')
  
if __name__ == '__main__':
  main()
