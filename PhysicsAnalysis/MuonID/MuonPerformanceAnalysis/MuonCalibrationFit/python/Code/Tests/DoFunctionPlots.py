# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import FitResult
import ROOT

def main():
  for name in [ 's1_func', 'p1_func', 'p2_func' ]:
    C = ROOT.TCanvas( name, '', 600, 600 )
    func = FitResult.GetCorrespondingTruth( name.replace( '_func', '' ), 'ID' )
    func.Draw()
    C.SaveAs( '.pdf' )

if __name__ == '__main__':
  sys.exit( main() )
