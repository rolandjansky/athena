#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = "Convert TMVA BDT weight files to ROOT format used by egamma::BDT"

import os

if __name__ == "__main__":

  from optparse import OptionParser
  parser = OptionParser("%prog inputXMLPath outputPath")
  parser.description = __doc__
  parser.epilog = "\n"

  parser.add_option("-t", "--particleType", default='0', choices=['0', '1'],
                    help="Type of particle (0=photon, 1=electron) (default: %default)")

  options, (inputXMLPath, outputPath) = parser.parse_args()
  options.particleType = int(options.particleType)

  if not os.path.isdir(inputXMLPath):
    raise IOError('Input path %s is not valid!' % inputXMLPath)

  if not os.path.isdir(outputPath):
    print 'Creating output path for ROOT files: %s' % outputPath
    os.mkdir(outputPath)

  import ROOT
  ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")

  m = ROOT.egammaMVACalib(options.particleType, False, inputXMLPath)
  m.InitTree(0)  # needed to create the formulae for the shifts
  m.writeROOTfile(outputPath)
  del m

