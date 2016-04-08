#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Script to prepare prescale keys for P1 using the rates rulebook
usage:
python runRuleBook.py
  With no options, will create keys in 25% incremenents (100, 125,
  160,...) with units of 1e30 (actually, the units are arbitrary, as
  long as the target_lumi, online_lumi, and breakpoints in the
  rules.py file are the same, you're ok).

python runRuleBook.py 100, 200
  Create key for the specified comma-separated lumi points

python runRuleBook.py 100,200 myDir
  With a second option, the script will also run rate predictions for
  the specified lumi points and output them to the specified directory

Variables to edit by hand
1) l1_xml and hlt_xml
   The location of your xml files, these can be taken from the
   release, downloaded from P1 using the TT, etc...
   Default is to use files from release, so should only need to change if menu at P1 doesn't match release
2) rulebook
   The name of your rulebook file (removing the trailing '.py').
   Typically this will be 'Physics_pp_v4_rules', but you may
   want to use the Standby keys.
3) filled_bunches, empty_bunches, unpaired_isolated, unpaired_nonisolated, abortgap, bgrp9, bgrp11, bgrp12
   The number of bunches in each of the bunch groups, check the bunch group ID key in the
   https://atlas-trigconf.cern.ch/ interface.
   The getBunchGroups.py script will return the values to fill in for
   a specific bunch group ID
4) list_of_streamers  
    This list is used to identify, from the chain name, which items are streamers.
    The rulebook then set the efficiency to 1. This is especially important if the chains are
    seeded by random triggers    
"""

import sys
import commands
import os
import time
import math
from collections import namedtuple
SummaryEntry = namedtuple('SummaryEntry', 'l1file hltfile lowlumi highlumi comment')
import xml.etree.cElementTree as ET

from runOptions import *

#--------------------------
l1_xml = "l1.xml"
hlt_xml = "hlt.xml"

#HI old data
#cost_xml="/afs/cern.ch/user/i/igrabows/public/TrigRate_hiv3-20.2.3.8.1-data11-nops-L3.7e27_HLT_SMK_225_L1_61_HLT_159.xml"
#cost_xml="/afs/cern.ch/user/t/tamartin/public/TrigRate_ppv5-20.2.3.9.1-5TeV-nops-L4.0e32_HLT_SMK_230_L1_62_HLT_161.xml"
cost_xml="/afs/cern.ch/work/n/nakahama/public/v6menu/TrigRate_rate-prediction-ATR-13260-noPS_HLT_SMK_273_L1_86_HLT_198.xml"

#--------------------------
# List of identifier in HLT streamers name, for which efficiency is 1, has to be set by hand 
# This should matter only if the streamer is seeded by random triggers
list_of_streamers="noalg,sct_noise,HLT_lumipeb"
#--------------------------


#---------------------------------------------
#In case the user hasn't run cmt make
def round_figures(value, precision):
    """Returns x rounded to n significant figures."""
    if value == 0:
        return 0

    abs_value = abs(value)
    round_value =  round(abs_value,
                       int(precision - math.ceil(math.log10(abs(abs_value))))
                       )
    return math.copysign(round_value, value)
#---------------------------------------------
def run_command(command, useimports):
    """Convert the command string to arguments for the imported processRules module"""
    if useimports:
        import shlex
        command = "processRules " + command
        thisargs = sys.argv
        sys.argv = shlex.split(command)
        import TrigMenuRulebook.processRules
        status = TrigMenuRulebook.processRules.main()
        sys.argv = thisargs
    else:
        command = "python ../python/processRules.py " + command
        status = os.system(command)

    return status
#---------------------------------------------
def PrintSummaryXML(alias, summary):
    def indent(elem, level=0):
        i = "\n" + level*"  "
        if len(elem):
            if not elem.text or not elem.text.strip():
                elem.text = i + "  "
            if not elem.tail or not elem.tail.strip():
                elem.tail = i
            for elem in elem:
                indent(elem, level+1)
            if not elem.tail or not elem.tail.strip():
                elem.tail = i
        else:
            if level and (not elem.tail or not elem.tail.strip()):
                elem.tail = i

    #treeroot = ET.ElementTree(ET.Element('ALIAS_DEFINITION', {'alias':"%s" % alias, 'bunches':'%i' % filled_bunches} ))
    # Change according to ATR-9675
    treeroot = ET.ElementTree(ET.Element('ALIAS_DEFINITION', {'alias':"%s" % alias, 'description':"%s" % alias, 'comment':"%s" %alias} ))
    for entry in PrescaleSetSummary[alias]:
        ET.SubElement(treeroot.getroot(),'SET',entry._asdict())
    indent(treeroot.getroot())
    treeroot.write("Set_%s.xml" % alias)
#-----------------------------------------------

if "userArgs=" in sys.argv[-1]:
    userArgs=sys.argv[-1]
    sys.argv = sys.argv[:-1]

lumi_list  = []
npoints = 4

if len(sys.argv) > 1:
    lumi_list = [float(i) for i in sys.argv[1].split(",")]
elif lumi_points:
    from copy import deepcopy
    lumi_list = deepcopy(lumi_points)
    print "Using user input list"
else:
    lumi_list = [1000, 1500, 2000, 2500, 3000,3500,4000, 4500, 5000, 5500, 6000] 
    print "No user defined lumi points given"

lumi_list_sorted = list(lumi_list)    
lumi_list_sorted.sort()
if lumi_list != lumi_list_sorted:
  sys.exit("runRulebook failed, lumi points are not in increasing order, please fix this!")

lumi_list = zip([lumi_list[0]/1.25] + lumi_list,lumi_list)

print 'Lumi list that is used to generate keys: ', lumi_list

#---------------------------------------------
if doDBRuleBookDownload:
    rulebook = ["Cosmic_pp_v4_RULES"]
    for i in rulebook:
        ii=i.split("_RULES")
        command="python download_rulebook_from_DB.py %s" % (ii[0])
        os.system(command)
else:
#    rulebook =  ["Physics_pp_v6_rules", "Cosmic_pp_v5_rules", "Standby_HI_v3_rules"]
    rulebook =  ["Physics_HI_v3_rules", "Cosmic_pp_v5_rules","Standby_HI_v3_rules"]    
    if doVdM == True:
        #if dolowmu == True:
        #    rulebook =  ["Physics_pp_v5_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_LossMap_rules","Physics_pp_v5_VdM_LowMu_rules"]
        #else:
        print "---------------------------------------"
        print "Running Physics_HI_v3_VdM__rules"
        print "---------------------------------------"
        rulebook =  ["Physics_HI_v3_rules", "Cosmic_pp_v5_rules","Standby_HI_v3_rules","Physics_HI_v3_VdM_rules"]
    elif doEB == True:
        print "RUNNING EB PHYSCIS RuleBooK"
        rulebook =  ["Physics_pp_v5_EB_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_rules","Physics_pp_v5_VdM_rules"]
#---------------------------------------------
rulebook = ["TrigMenuRulebook.%s" % r for r in rulebook]
#---------------------------------------------
useimports = True

if "TrigMenuRulebook/scripts" in os.getcwd():
    os.system("ln -s ../python TrigMenuRulebook &> /dev/null")
    os.system("ln -s ../share/%s" % l1_xml)
    os.system("ln -s ../share/%s" % hlt_xml)
    os.system("ln -s %s" % cost_xml)
    #os.system("ln -s ../share/%s" % cost_xml)
else:
    os.system('get_files -xmls %s' % cost_xml)
    os.system('get_files -xmls %s' % l1_xml)
    os.system('get_files -xmls %s' % hlt_xml)
    os.system('get_files -scripts cnvXML.py')


if doCosmic or doStandby:
    print
    print "=====> Will also generate Cosmic and Standby prescales"
    print

ebntp="/afs/cern.ch/user/l/lipeles/eos/atlas/atlasdatadisk/data12_8TeV/NTUP_TRIGRATE/r4034_p966/data12_8TeV.00212967.physics_EnhancedBias.merge.NTUP_TRIGRATE.r4034_p966_tid01012172_00"

os.system("echo $PWD")

make_prescale_dir = False
prescale_dir = "prescales_%.f"%time.time()
try:
    os.mkdir(prescale_dir)
    make_prescale_dir = True
except OSError:
    print OSError, "but continue..."

if len(sys.argv)>2:
    output_dir = sys.argv[2]

    try:
        os.mkdir(output_dir)
    except OSError:
        print OSError, "but continue..."

PrescaleSetSummary = {}
PrescaleSetSummary['PHYSICS'] = []

#---------------------------------------------
if doPhysics:
    for (lower_lumi_point, lumi_point) in lumi_list:

        print "*** Running:", lumi_point
        logname="log_"+str(lumi_point)
        
        command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point)
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s --target_abortgapnotcalib=%s --target_BGRP9=%s --target_BGRP10=%s --target_BGRP11=%s --target_BGRP12=%s" % (filled_bunches,empty_bunches, empty_after_filled_bunches, unp_iso_bunches,unp_noniso_bunches,abortgapnotcalib_bunches, bgrp9_bunches,bgrp10_bunches,bgrp11_bunches, bgrp12_bunches)
        command += " --rates-xml=%s" % cost_xml
        command += " --streamers=%s" %list_of_streamers
        if ignoreErrors:
            command += "  --ignore-errors"
        if "userArgs" in dir():
            command += ' "%s"' % userArgs
        print
        print "*** Will execute:  %s ***" % command
        print
        status = run_command(command, useimports)

        if status != 0:
            sys.exit("processRule failed")

        print " *** Converting XMLs to uploadable XMLs"
        print lower_lumi_point, lumi_point
        short_rulebookname = rulebook[0].split('.')[1].split('_rules')[0]
        prescale_name = "%s_%04.10f_%04.10fe32"%(short_rulebookname, (float(lower_lumi_point)/100.),float(lumi_point)/100.)
        prescale_name += "_%sb" % filled_bunches
        if prescale_name.startswith('TrigMenuRulebook.'):
            prescale_name = prescale_name[17:]
        command_cnv   = "python %scnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %("" if useimports else "../scripts/", prescale_name, lumi_point)
        status = os.system(command_cnv)
        if status != 0:
            sys.exit("XML conversion failed")

        if make_prescale_dir == True:
            mv_command = "mv RuleBook_*PS_%s* %s"%(prescale_name,prescale_dir)
            status = os.system(mv_command)
            if status != 0:
                sys.exit("Move failed") 


        PrescaleSetSummary['PHYSICS'] += [SummaryEntry(l1file='RuleBook_LV1PS_%s.xml' % prescale_name,
                                                       hltfile='RuleBook_HLTPS_%s.xml' % prescale_name,
                                                       lowlumi='%.10fe32' % (float(lower_lumi_point)/100.),
                                                       highlumi='%.10fe32' % (float(lumi_point)/100.),
                                                       comment='Physics')]

        if len(sys.argv)>2:
            from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
            cmd = "runEB.py \
            -i "+ebntp+" \
            -o "+output_dir+" \
            --pred-lumi="+str(lumi_point)+"e30 \
            --joblist rulebook \
            --forwardArgs='-k TRIGRATE --prescaleXML=prescales"+str(lumi_point)+".xml'"


            print 'Execute : %s' % cmd

            status = os.system(cmd)
            if status != 0:
                sys.exit("Rate prediction failed")

        PrintSummaryXML('PHYSICS', PrescaleSetSummary)


#---------------------------------------------
def doCosmicStandbyPrescales(what):
    lumi_ref = 1.0
    extra_command = ""
    if what == "Cosmic":
        tryRulebook = rulebook[1]
    elif what == "Standby":
        tryRulebook = rulebook[2]
    elif what == "VdM":
        lumi_ref = 10.0
        tryRulebook = rulebook[3]

    print "*** Running "+what

    command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (tryRulebook, what, hlt_xml, l1_xml, lumi_ref)
    command += " --streamers=%s" %list_of_streamers
    if ignoreErrors:
        command += "  --ignore-errors"
    if what == "Cosmic":
        print "*** Cosmic Run, set target_empty to 3000 ***"        
        command +=  " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s --target_abortgapnotcalib=%s --target_BGRP9=%s --target_BGRP10=%s --target_BGRP11=%s --target_BGRP12=%s" % (filled_bunches,3000, empty_after_filled_bunches, unp_iso_bunches,unp_noniso_bunches,abortgapnotcalib_bunches, bgrp9_bunches,bgrp10_bunches,bgrp11_bunches,bgrp12_bunches)

    elif what == "Standby" or what=="VdM":
        command +=  " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s --target_abortgapnotcalib=%s --target_BGRP9=%s --target_BGRP10=%s --target_BGRP11=%s --target_BGRP12=%s" % (filled_bunches,empty_bunches, empty_after_filled_bunches, unp_iso_bunches,unp_noniso_bunches,abortgapnotcalib_bunches, bgrp9_bunches, bgrp10_bunches,  bgrp11_bunches,bgrp12_bunches)
    command += " --rates-xml=%s" % cost_xml

    print
    print "*** Will execute:  %s ***" % command
    print
    status = run_command(command, useimports)

    print " *** Converting XMLs to uploadable XMLs"
    prescale_name = what
    command_cnv   = "python %scnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %("" if useimports else "../scripts/", prescale_name, lumi_ref)

    os.system(command_cnv)

    PrescaleSetSummary[what.upper()] = []
    PrescaleSetSummary[what.upper()] += [SummaryEntry(l1file='RuleBook_LV1PS_%s.xml' % prescale_name,
                                                      hltfile='RuleBook_HLTPS_%s.xml' % prescale_name,
                                                      lowlumi='0', highlumi='0', comment='Cosmic')]

    PrintSummaryXML(what.upper(), PrescaleSetSummary)

    if make_prescale_dir == True:
        mv_command = "mv RuleBook_*PS_%s* %s"%(prescale_name,prescale_dir)
        os.system(mv_command)




#---------------------------------------------
if doCosmic:
    doCosmicStandbyPrescales("Cosmic")

#---------------------------------------------
if doStandby:
    doCosmicStandbyPrescales("Standby")

#---------------------------------------------
if doVdM:
    doCosmicStandbyPrescales("VdM")

#---------------------------------------------
if make_prescale_dir == True:
    mv_command = "mv Set_*.xml %s" % prescale_dir
    os.system(mv_command)

#---------------------------------------------
if doUseOnline == True:
    print
    print "... Tarring prescales directory"
    os.system("tar -zcvf %s.tgz %s" % (prescale_dir,prescale_dir))
    print "...rsync to P1"
    os.system("rsync %s.tgz $USER@atlasgw.cern.ch:"  % prescale_dir )
    print

