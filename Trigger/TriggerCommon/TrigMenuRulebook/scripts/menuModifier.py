#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# This code replaces the prescale and rerun_prescsale in the l1 and hlt menu by the input prescaleXXXXX.xml
# not the ones to upload to the Trigger tool
#
# === README ====
#  You need to get the input prescaleXXXX.xml first by running the runRulebook.py with the SAME lumi blocks
#  run this script by
#  python menuModifier.py l1.xml hlt.xml 10000,10000
# === README ====

from TriggerMenu.l1.PrescaleHelper import getCutFromPrescale 
import xml.etree.cElementTree as ET
import sys
from sys import argv
import string
import os.path


if len(argv) < 3:
    print "Error, you need to provide inputs l1.xml, hlt xml"
    print "--l1.xml        : l1 xml"
    print "--hlt.xml       : hlt xml"
    sys.exit()

l1_filename = sys.argv[1]
hlt_filename = sys.argv[2]

lumi_list  = []

if len(sys.argv) > 3:
    lumi_list = [int(i) for i in sys.argv[3].split(",")]
else:
    lumi_list = [5000, 10000, 15000, 20000]

lumi_list = zip([lumi_list[0]/1.25] + lumi_list,lumi_list)

for (lower_lumi_point, lumi_point) in lumi_list:

  # get the prescale information from the prescaleXXXX.xml inputs
  prescale_filename = "prescales" + str(lumi_point) + ".xml"
  # check if the prescale file exisits, if not move to the next point
  if not os.path.exists(prescale_filename):
      print "ERROR, prescale file %s does not exisit, need to do runRulebook.py first to create them!" % prescale_file
      continue
  # file name of the menu with correct prescales
  prescale_appendix = "_withps_" + str(lumi_point) + ".xml"

  #
  # 1. figure out a map of prescale for a given chain
  #
  prescale_file = open(prescale_filename, "r")
  prescale_XML = ET.parse(prescale_file)
  prescale_INFO = prescale_XML.getroot()

  list_prescales_l1 = {}
  list_prescales_hlt = {}
  list_prescales_hlt_rerun = {}

  for e in prescale_INFO.findall('level'):
      level = e.find('lvl_name').text
      for sig in e.findall('signature'):
        chain_name = sig.find('sig_name').text
        prescale = float(sig.find('chain_prescale').text)
        rerun_prescale = 0
        if sig.find('rerun_prescale'):
            rerun_prescale = float(sig.find('rerun_prescale').text)
        if 'L1' in level:
           list_prescales_l1[chain_name] = prescale
        if 'HLT' in level:
           list_prescales_hlt[chain_name] = prescale
           list_prescales_hlt_rerun[chain_name] = rerun_prescale

  #
  # 2. replace l1 ouptut
  #
  output_l1_filename = l1_filename.replace(".xml", prescale_appendix) 
  output_l1 = open(output_l1_filename, "w")
  print '====Replacing the menu %s for lumi %.0f ' % (l1_filename, lumi_point)

  l1_file_fordict = open(l1_filename, "r")
  l1_file = open(l1_filename, "r")

  # == Get the L1 ctpid and item name correspondance
  L1_XML = ET.parse(l1_file_fordict)
  L1_INFO = L1_XML.getroot()

  list_of_l1chains = {}
  for e in L1_INFO.findall('TriggerMenu/TriggerItem'):
    l1_name = '"%s"' % e.get('ctpid')
    list_of_l1chains[l1_name] = e.get('name')

  # replace the prescale in the L1 file
  for line in l1_file:
      prescale_l1 = 1
      if 'Prescale ' in line:
          mylist = line.split(' ')
          for item in mylist:
              if 'ctpid' in item:
                  temp, ctpid = item.split("=", 1)
          if ctpid in list_of_l1chains:
              l1_string = list_of_l1chains[ctpid]

          if l1_string in list_prescales_l1:
              prescale_l1 = list_prescales_l1[l1_string]
              cutvalue_l1 = int(getCutFromPrescale(int(prescale_l1)))
              # figure out the cut value
              l1ps_string = 'value="%.0f"' % prescale_l1
              line = line.replace('value="1"', l1ps_string )
              l1cut_string = 'cut="%i"' % cutvalue_l1
              line = line.replace('cut="000001"', l1cut_string )

      output_l1.writelines(line)

  #
  # 3. replace hlt output
  #
  output_hlt_filename = hlt_filename.replace(".xml", prescale_appendix) 
  output_hlt = open(output_hlt_filename, "w")
  print '====Replacing the menu %s for lumi %.0f ' % (hlt_filename, lumi_point)
  hlt_file = open(hlt_filename, "r")

  # replace the prescale in the HLT file
  for line in hlt_file:
    if 'chain_counter=' in line:
        mylist = line.split(' ')
        for item in mylist:
            if 'chain_name=' in item and "lower" not in item:
                temp, chain_name = item.split("=", 1)

        if chain_name in list_prescales_hlt:
            prescale = list_prescales_hlt[chain_name]

        if chain_name in list_prescales_hlt_rerun:
            rerun_prescale = list_prescales_hlt_rerun[chain_name] 

        str_rerun_prescale = 'rerun_prescale="%.1f"' % rerun_prescale
        str_prescale = ' prescale="%.1f"' % prescale

        line = line.replace('rerun_prescale="-1"', str_rerun_prescale) 
        line = line.replace(' prescale="1"', str_prescale) 

    output_hlt.writelines(line)




