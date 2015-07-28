#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os
import subprocess

"""
Script to compare the xml output of the rulebook
"""

import xml.etree.cElementTree as ET
from TrigMenuRulebook.RuleTools import read_hlt, build_groups_xml
import sys


# the following functions getCutFromPrescale, getPrescaleFromCut are stolen from TriggerMenu/python/l1/PrescaleHelper.py, but are explicitly written here, for not creating dependencies on TriggerMenu package. Maybe better thinking is required...


def getCutFromPrescale(prescale):
    """
    C = 2**24-(2**24-1)/PS
    
    PS =       1 --> C = 1
    PS =       2 --> C = 8388609
    PS =      10 --> C = 15099495
    PS =    1000 --> C = 16760439
    PS =   10000 --> C = 16775539
    PS = 2**24-1 --> C = 2**24-1
    """

    if prescale==0:
        raise RuntimeError("L1 Prescale value of 0 is not allowed")
    
    sign = -1 if prescale<0 else 1
    prescale = abs(prescale)
    cut=sign * ( 0x1000000 - int(0xFFFFFF/prescale) )
    if  prescale > 0xFFFFFF:
      cut=sign * (0x1000000-1)
    return cut



def getPrescaleFromCut(cut):
    """
    PS = (2**24-1)/(2**24-C)

    C =       1 --> PS =     1
    C = 2**24-1 --> PS = 2**24-1
    """
    
    if cut==0:
        raise RuntimeError("L1 Prescale cut of 0 is not allowed, use cut=1 for a prescale value of 1")

    sign = -1 if cut<0 else 1
    cut = abs(cut)
    return (sign * 0xFFFFFF) / ( 0x1000000 - cut )


def mergePrescalesIntoMenu(psfile, l1file, hltfile, rerun_prescale):
  """ Input are the xml files.
  Create new copies of L1 and HLT menu xml files modifiying the PS as from the PS xml file.
  To make a diff between the original and the new ones, use xmllint to reformat both of them.
  """
  
  psfromPS=readPrescalesXML(psfile)
  PSSetName=psfile.replace(".xml","")

  # new L1 replaced file
  
  newL1file="%s.%s_rerun%s.xml" %(l1file.replace(".xml",""),PSSetName,rerun_prescale)
  
  l1tree = ET.parse(l1file)
  l1_xml = l1tree.getroot()

  #l1tree.write("L1original.xml")
  
  ctpids = {}
  for trigger in l1_xml.findall("TriggerMenu/TriggerItem"):
    trigger_name = trigger.get('name')
    trigger_id   = trigger.get('ctpid')
    ctpids[trigger_id] = trigger_name


  for trigger in l1_xml.findall("PrescaleSet/Prescale"):
    trigger_id = trigger.get('ctpid')
    trigger_cut = trigger.get('cut')  
    trigger_ps = int(trigger.get('value')) 
    trigger_name = ctpids.get(trigger_id)
    if trigger_name != None:
      # leave default if not found
      new_trigger_ps = int(psfromPS.get(trigger_name, trigger_ps))
      #      new_trigger_ps=50000
      if trigger_ps != new_trigger_ps:
        trigger.set("value", str(new_trigger_ps))
        # calculate cut
        newcut = getCutFromPrescale(new_trigger_ps)
        cut = str(format(newcut,'x')) #make it hex, without 0x
        trigger.set("cut", cut)

        # read back
        readcut=int(cut,16)
        readPS=getPrescaleFromCut(readcut)
        print "Change L1 PS for chain %s id=%s from %s to %s (written cut=%s from cut=%d READCUT=%d READPS=%d)" %(trigger_name, trigger_id, trigger_ps, new_trigger_ps,trigger.get('cut'), newcut, readcut, readPS)
        if readPS != new_trigger_ps:
          print "      ERRORRRRRRR different PS: readback=%d original=%d"%(readPS, new_trigger_ps)

          #  change em energy scale to 1 as for Run1 data
  caloline=l1_xml.find('CaloInfo')
  caloline.set("global_em_scale",str(1))
  print caloline.get("global_em_scale")
  print "Change L1 PS for chain %s id=%s from %s to %s (value=%s)" %(trigger_name, trigger_id, trigger_ps, new_trigger_ps,trigger.get('value'))

  for trigger in l1_xml.findall("PrescaleSet"):
    oldname=trigger.get('name')
    newname="%s-%s"%(oldname,PSSetName)
    trigger.set("name",newname)
    
  l1tree.write(newL1file)


  # HLT replace
  #  newHLTfile="%s.prescales5000_rerun%s.xml" %(hltfile,rerun_prescale)
  newHLTfile="%s.%s_rerun%s.xml" %(hltfile.replace(".xml",""),PSSetName,rerun_prescale)
  
  HLTtree = ET.parse(hltfile)
  hlt_xml = HLTtree.getroot()
  #HLTtree.write("HLToriginal.xml")
  for trigger in hlt_xml.findall('CHAIN_LIST/CHAIN'):
    trigger_name = trigger.get('chain_name')
    trigger_ps   = int(trigger.get('prescale')) 
    new_trigger_ps = str(int(psfromPS.get(trigger_name, trigger_ps)))
    trigger.set("prescale", new_trigger_ps)
    trigger.set("rerun_prescale", rerun_prescale)
    print "Change HLT PS for chain %s from %s to %s (value=%s)" %(trigger_name, trigger_ps, new_trigger_ps,trigger.get('prescale'))

  HLTtree.write(newHLTfile)
  return



def readPrescalesXML(filename):
  """ Input is the root level of an xml file. Will return a flat
  dictionary with the prescales for all trigger items. """

  prescales_xml = ET.parse(filename).getroot()

  output = {}

  for trigger in prescales_xml.findall("level/signature"):
    trigger_name = trigger.find("sig_name").text
    prescale     = trigger.find("chain_prescale").text
    output[trigger_name] = float(prescale)

  output["L1_multiseed"] = 1
  output["L1_unseeded"] = 1
  
  return output

def readPrescalesMenu(l1_filename, hlt_filename):
  """ This function will pull the default prescale values stored in a
  set of L1 and HLT menu XMLs"""
  print l1_filename
  print hlt_filename
  l1_xml = ET.parse(l1_filename).getroot()
  hlt_xml = ET.parse(hlt_filename).getroot()

  output = {}

  ctpids = {}
   
  for trigger in l1_xml.findall("TriggerMenu/TriggerItem"):
    trigger_name = trigger.get('name')
    trigger_id   = trigger.get('ctpid')
    ctpids[trigger_id] = trigger_name


  for trigger in l1_xml.findall('PrescaleSet/Prescale'):
    trigger_id = trigger.get('ctpid')
    trigger_ps = trigger.get('value') 
    output[ctpids.get(trigger_id, "None")] = trigger_ps

  for trigger in hlt_xml.findall('CHAIN_LIST/CHAIN'):
    trigger_name = trigger.get('chain_name')
    trigger_ps   = float(trigger.get('prescale'))
    output[trigger_name] = trigger_ps

  output["L1_multiseed"] = 1
  output["L1_unseeded"] = 1

  return output

def getTriggerToolMenu(smk, l1psk, hltpsk):
  """ Pull the menu XMLs from the TriggerTool"""

  curdir = os.getcwd()

  tmpdir = "tmp_%d_%d_%d" % (smk, l1psk, hltpsk)

  try:
    os.mkdir(tmpdir)
  except OSError:
    print "Trying to over-write existing TT menu files with SMK %d, L1PSK %s, and HLTPSK %d."% (smk, l1psk, hltpsk)
    print "Please move the existing directory (%s)"  % tmpdir
    sys.exit()
  
  os.chdir(tmpdir)

  cmd = "/afs/cern.ch/user/a/attrgcnf/TriggerTool/run_TriggerTool.sh" \
        " -db TRIGGERDB -down -k %d -l1psk %d -hltpsk %d" \
        % (smk, l1psk, hltpsk)

  #cmd = cmd.split(" ")
  print cmd

  #status = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
  status = os.system(cmd + "&> TT.log")

  os.chdir(curdir)

  return "%s/LVL1Menu_%d_%s.xml" % (tmpdir, smk, l1psk), "%s/HLTMenu_%d_%d.xml" % (tmpdir, smk, hltpsk)


def fillMenuTree(hlt_name, prescales, max_level = "HLT"):
  """ Input is the root level of an xml file and a dict of triggers
  and their prescales. Will return a nested map representation of the
  trigger tree with the appropriate prescales. """

  hlt_xml = ET.parse(hlt_name).getroot()
  
  hlt_menu = read_hlt(hlt_xml, "HLT")

  (hlt_groups) = build_groups_xml(hlt_menu)

  menu_tree = {}
  for l1_name in hlt_groups:
    menu_tree[l1_name] = (prescales.get(l1_name, -1)
                          , {}
                          )
    if max_level == "L1":
      continue
    
    for hlt_name in hlt_groups[l1_name]:
      menu_tree[l1_name][1][hlt_name] = (prescales.get(hlt_name, -1)
                                        , {}
                                        )
  return menu_tree

def prettyString(string, length):
  """ Given some input that has a string representation, return a
  fixed length string. """
  output = str(string)
  output += " "*(length - len(output))
  return output

failures = []

def multiplyPS(a, b):
  """ Prescales don't follow real arithmetic, if you multiply two
  negative PS, they should stay negative. """
  output = abs(a) * abs(b)
  if a < 0 or b < 0:
    output *= -1

  return output

def comparePrescales(left_tree, right_tree, name = "", left_total_ps = 1, right_total_ps = 1):
  """ Walk through nested levels of two dictionary representations of
  a menu. If the cummulative prescale (taking into account lower
  levels) is not the same, print out a message. """

  all_names = set(left_tree.keys()).union(right_tree.keys())

  if left_total_ps < 0:
    left_total_ps = -1.0
  if right_total_ps < 0:
    right_total_ps = -1.0

  if len(all_names) == 0 and left_total_ps != right_total_ps:
    failures.append(
      (name[5:], left_total_ps, right_total_ps)
      )
    
  for sub_name in all_names:
    left_ps, left_items = left_tree.get(sub_name, (-1, {}))
    right_ps, right_items = right_tree.get(sub_name, (-1, {}))

    comparePrescales(left_items, right_items, "%s --> %s" % (name, sub_name), multiplyPS(float(left_total_ps), float(left_ps)), multiplyPS(float(right_total_ps), float(right_ps)))

def printFailures(failures, mode = None):
  if mode == None:
    mode = "sort_names"
    
  if mode == "sort_names":
    failures = sorted(failures, key = lambda x: x[0])
  elif mode == "sort_left":    
    failures = sorted(failures, key = lambda x: x[1])
  elif mode == "sort_right":
    failures = sorted(failures, key = lambda x: x[2])
  elif mode == "sort_fraction":
    failures = sorted(failures, key = lambda x: x[2] / x[1])
  else:
    print "Unknown sorting mode, will sort by names"
    failures = sorted(failures, key = lambda x: x[0])

  print "-"*130
  print "%s: %s vs %s (%s )" % (
    prettyString("trigger chain", 90),
    prettyString("left PS", 10),
    prettyString("right PS", 10),
    prettyString("right/left", 10)
    )
  print "-"*131
    
  for name, left_total_ps, right_total_ps in failures:

    ps_ratio = 1
    if left_total_ps != 0: 
      ps_ratio  = right_total_ps / left_total_ps 
    print "%s: %s vs %s (%s )" % (
      prettyString(name, 90),
      prettyString(left_total_ps, 10),
      prettyString(right_total_ps, 10),
      prettyString( round(ps_ratio, 4), 10)
      )
  print "-"*131

if __name__ == "__main__":

  from optparse import OptionParser
  p = OptionParser(usage = "usage: python comparePrescales.py", version = "1.0")  

  p.add_option("-s", "--sort", type = "str", default = "sort_names", dest = "sort_mode",
               help = "How to sort the results: sort_names (default)"
               ", sort_left, sort_right, sort_fraction")
  
  p.add_option("-m", "--max_level", type = "str", default = "HLT", dest = "max_level",
               help = "How far along the tree the comparision should be made:"
               " L1, HLT (default, total L1*HLT prescale)")
  
  p.add_option("--source", type = "str", default = "rulebook", dest = "source",
               help = "What type of input to expect:                                                                           "
               "    - rulebook (default) - two rulebookXXX.xml files produced by runRulebook.py                                "
               "    - TTXML - two each of the L1 and HLT xmls produced by TriggerTool's write command                          "
               "    - MERGE - merge PS from  xml file to menu xml files, and sets rerun_prescales (args=psfile, l1file,hltfile, rerun_prescale)                              "
               "    - TT - two each of a SMK, L1 key, and HLT key for reading directly from the TriggerTool."
               )

  (opts, args) = p.parse_args()


 

  if opts.source == "rulebook":
    if len(args) != 3:
      print "Expected 3 arguments, got %d" % len(args)
      print "Arguements are <hlt menu xml> <xml1> <xml2>"
      p.print_help()
      sys.exit()
      
    menu_name = args[0]
    left_name = args[1]
    right_name = args[2]

    left_prescales = readPrescalesXML(left_name)
    right_prescales = readPrescalesXML(right_name)
    
    left_tree = fillMenuTree(menu_name, left_prescales, opts.max_level)
    right_tree = fillMenuTree(menu_name, right_prescales, opts.max_level)

  elif opts.source == "TT":
    if len(args) != 6:
      print "Expected 6 arguments, got %d" % len(args)
      p.print_help()
      sys.exit()

    left_smk = int(args[0])
    left_l1key = int(args[1])
    left_hltkey = int(args[2])
    right_smk = int(args[3])
    right_l1key = int(args[4])
    right_hltkey = int(args[5])

    left_l1, left_hlt = getTriggerToolMenu(left_smk, left_l1key, left_hltkey)
    right_l1, right_hlt = getTriggerToolMenu(right_smk, right_l1key, right_hltkey)

    left_prescales  = readPrescalesMenu(left_l1,  left_hlt)
    right_prescales = readPrescalesMenu(right_l1, right_hlt)
    
    left_tree = fillMenuTree(left_hlt, left_prescales, opts.max_level)
    right_tree = fillMenuTree(right_hlt, right_prescales, opts.max_level)
    
  elif opts.source == "TTXML":
    if len(args) != 4:
      print "Expected 4 arguments, got %d" % len(args)
      p.print_help()
      sys.exit()

    left_l1 = args[0]
    left_hlt = args[1]
    right_l1 = args[2]
    right_hlt = args[3]
 
    print left_l1, left_hlt
    left_prescales  = readPrescalesMenu(left_l1,  left_hlt)
    print right_l1, right_hlt
    right_prescales = readPrescalesMenu(right_l1, right_hlt)

    left_tree = fillMenuTree(left_hlt, left_prescales, opts.max_level)
    right_tree = fillMenuTree(right_hlt, right_prescales, opts.max_level)

  elif opts.source =="MERGE":
    if len(args) != 4:
      print "Expected 4 arguments, got %d" % len(args)
      p.print_help()
      sys.exit()
    psfile=args[0]
    l1file=args[1]
    hltfile=args[2]
    rerun_prescale = args[3]
    print psfile, l1file, hltfile
    mergePrescalesIntoMenu(psfile, l1file,hltfile, rerun_prescale)
    sys.exit()

  else:
    print "Unrecognized source option: %s" % opts.source
    p.print_help()
    sys.exit()

  print ""
#  print "Comparing total PS for %s and %s" % (left_name, right_name)
  comparePrescales(left_tree, right_tree)
  printFailures(failures, opts.sort_mode)
  print ""

