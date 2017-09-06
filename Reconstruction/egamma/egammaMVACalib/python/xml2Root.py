#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = "Convert TMVA BDT weight files to ROOT format used by egamma::BDT"

import os
from xml.dom.minidom import parse
from glob import glob
from multiprocessing.dummy import Pool
import re
import time

# regex is not the best option, but better than minidom
regex = re.compile(r'<VariablesName><Info Name="(.*?)"/></VariablesName>')
def get_variables_from_xml(xml):
  xml_content = open(xml).read()
  m = regex.search(xml_content)
  return m.group(1).split(',')
#  top = parse(xml).documentElement
#  print "parsing done"
#  return top.getElementsByTagName('VariablesName')[0].getElementsByTagName("Info")[0].getAttribute("Name").split(",")


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
    raise IOError('Input path %s is not valid: it must be a directory containing all the xmls' % inputXMLPath)

  if not os.path.isdir(outputPath):
    print 'Creating output path for ROOT files: %s' % outputPath
    os.mkdir(outputPath)

  all_xml = glob("%s/*.xml" % inputXMLPath)
  if not all_xml:
    print "cannot find xml file in %s" % inputXMLPath

  all_variables = set()
  pool = Pool(20)
  variables_per_xml = pool.map_async(get_variables_from_xml, all_xml, chunksize=1)
  pool.close()
  while not variables_per_xml.ready():
    time.sleep(0.5)
  variables_per_xml = variables_per_xml.get()
  for v in variables_per_xml:
      all_variables.update(v)
  all_variables = list(all_variables)
  all_variables.append('el_cl_E')
  print "creating root file using these variables (%d): %s" % (len(all_variables), ', '.join(all_variables))

  import ROOT
  ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")

  m = ROOT.egammaMVACalib(options.particleType, False, inputXMLPath)

  from array import array
  all_arrays = []
  tree = ROOT.TTree("dummy", "dummy")
  for v in all_variables:
    d = array('f', [0.])
    all_arrays.append(d)
    tree.Branch(v, d, v + "/F")
  tree.Fill()
  tree.Print()

  m.InitTree(tree)  # needed to create the formulae for the shifts
  m.writeROOTfile(outputPath)
  del m
