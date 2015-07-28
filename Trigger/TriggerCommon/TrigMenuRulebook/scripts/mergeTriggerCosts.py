#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Given two TriggerCost XMLs and a list of seeds, copy the rates for
all chains descending from the seeds from the second XML into the
first XML
"""

#example to merge the randoms items (this takes all the random items and merges them into the main (-m file)
#where -m is the new TriggerCost xml file
#-f is the current TriggerCost_1e33.xml (this file will be used by default)
#-o is the output
#python mergeTriggerCosts.py -m new.xml -f TriggerCosts_1e33.xml -o TriggerCosts_merged.xml -s L1_RD0_FILLED,L1_RD1_FILLED,L1_RD0_EMPTY,L1_RD1_EMPTY

import xml.etree.ElementTree as ET
from xml.dom import minidom

def findFixedValues(xml, seeds):
  fixed_values = {}

  local_seeds = set(seeds)

  for levels in fixes.getroot().findall('level'):
    level = levels.find("lvl_name").text
    for sig in levels.findall('signature'):
      try:
        trigger_name = sig.find("sig_name").text
        lower_name = sig.find("lower_chain_name").text
      except AttributeError:
        continue

      if lower_name in local_seeds:
        local_seeds.update([trigger_name])

      if trigger_name in local_seeds:
        #fixed_values[trigger_name] = str(float(sig.find("rate").text)*25000.)
        fixed_values[trigger_name] = sig.find("rate").text
        print "Store %s for %s" % (fixed_values[trigger_name], trigger_name)

  return fixed_values

def applyFixedValues(xml, fixed_values):
  for levels in master.getroot().findall('level'):
    level = levels.find("lvl_name").text
    for sig in levels.findall('signature'):
      try:
        trigger_name = sig.find("sig_name").text
      except AttributeError:
        continue

      if trigger_name in fixed_values:
        print "Apply %s to %s instead of %s" % (fixed_values[trigger_name], trigger_name, sig.find("rate").text)
        rate = sig.find("rate")
        rate.text = fixed_values[trigger_name]

  return xml

if __name__ == "__main__":
  from optparse import OptionParser
  p = OptionParser(usage = "usage: python mergeXML.py", version = "1.0")
  p.add_option('-m', '--master',   type   = "str",   dest = "master",   help = "The main xml file")   
  p.add_option('-f', '--fixes',    type   = "str",   dest = "fixes",    help = "The xml file from which to merged fixed chains")
  p.add_option('-o', '--output',   type   = "str",   dest = "output",   help = "The xml file to write the merged output")
  p.add_option('-s', '--seeds',    type   = "str",   dest = "seeds",    help = "A comma seperated list of seeds whose values should be taken from the fix file")
  
  (opts, args) = p.parse_args()

  if None in [opts.master, opts.fixes, opts.output, opts.seeds]:
    p.print_help()
    exit(-1)

  master = ET.parse(opts.master)
  fixes = ET.parse(opts.fixes)
  seeds = opts.seeds.split(",")

  fixed_values = findFixedValues(fixes, seeds)
  output = applyFixedValues(master, fixed_values)

  output.write(opts.output)
