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
  With just one option, will create key for the specified
  comma-separated lumi points

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

   The number of bunches in each of the bunch groups. In stable
   running this doesn't need to be edited. If you're unlucky and this
   changes a lot during your shift, you can get these values by
   feeding the appropriate bunch group ID key in the
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


###
### START CONFIGURATION
###

doUseOnline = False
doPhysics = True
doCosmic = False 
doStandby = False
doVdM = False


doDBRuleBookDownload = False # False python rule book is used, True DB rules are used

## Sept 2012, ALFA setting (BGR: 507)
#filled_bunches = 2
#empty_bunches = 3425
#empty_after_filled_bunches = 2
#unp_iso_bunches = 2
#unp_noniso_bunches = 2

##NOTE THAT WE NEED TO UPDATE THIS TO THE EXPECTED BUNCH STRUCTURE THE PS SHOULD BE USED WITH
##otherwise, the PS values may be wrong

# 1003 setting - all fill/all empty (see https://atlas-trigconf.cern.ch/bunchgroups?key=1003) for Cosmic runs
#filled_bunches = 3474
#empty_bunches = 3473
#empty_after_filled_bunches = 1470
#unp_iso_bunches = 18
#unp_noniso_bunches = 80
#abortgapnotcalib_bunches= 0 
#bgrp9_bunches=0
#bgrp11_bunches=0 
#bgrp12_bunches=0 

###Bunch structure for loss map
#filled_bunches = 1
#empty_bunches = 3441
#empty_after_filled_bunches = 5
#unp_iso_bunches = 2
#unp_noniso_bunches = 0
#abortgapnotcalib_bunches= 29
#bgrp9_bunches=29
#bgrp11_bunches=1
#bgrp12_bunches=0 

##Possible bunch structure for 2bunches (same as 1098)
#filled_bunches = 2
#empty_bunches = 3416
#empty_after_filled_bunches = 10
#unp_iso_bunches = 2
#unp_noniso_bunches = 2
#abortgapnotcalib_bunches= 29
#bgrp9_bunches=145
#bgrp11_bunches=2
#bgrp12_bunches=0 


###Possible bunch structure for 38bunches (same as 1181)
#filled_bunches = 38
#empty_bunches = 3317
#empty_after_filled_bunches = 58
#unp_iso_bunches = 1
#unp_noniso_bunches = 24
#abortgapnotcalib_bunches= 29  
#bgrp9_bunches=62 
#bgrp11_bunches=38
#bgrp12_bunches=0 

###Possible bunch structure for 110bunches (same as 1180)
#filled_bunches = 110
#empty_bunches = 3092
#empty_after_filled_bunches = 130
#unp_iso_bunches = 6
#unp_noniso_bunches = 6
#abortgapnotcalib_bunches= 29  
#bgrp9_bunches=62 
#bgrp11_bunches=110
#bgrp12_bunches=0 

###Possible bunch structure for 254bunches (same as 1180, changing only filled and empy)
#filled_bunches = 254 #1
#empty_bunches = 2759 #3
#empty_after_filled_bunches = 290 #6
#unp_iso_bunches = 12 #4
#unp_noniso_bunches = 72 #5
#abortgapnotcalib_bunches= 29  
#bgrp9_bunches=62 
#bgrp11_bunches=110
#bgrp12_bunches=0 
#

###Possible bunch structure for 434 bunches (same as 1180, changing only filled and empy) #BGRP Set 1192
#filled_bunches = 434 #1
#empty_bunches = 2362 #3
#empty_after_filled_bunches = 490 #6
#unp_iso_bunches = 12 #4
#unp_noniso_bunches = 72 #5
#abortgapnotcalib_bunches= 29  
#bgrp9_bunches=62 
#bgrp11_bunches=110
#bgrp12_bunches=0 

####Possible bunch structure for 434 bunches (same as 1180, changing only filled and empy) #BGRP Set 1195                                                     
#filled_bunches = 426 #1                                                                                                     
#empty_bunches = 2330 #3                                                                                                                                      
#empty_after_filled_bunches = 478 #6                                                                                                                  
#unp_iso_bunches = 20 #4                                                                                                                                 
#unp_noniso_bunches = 80 #5                                                                                                                 
#abortgapnotcalib_bunches= 29 #8
#bgrp9_bunches=63   # afterglow so not important for physics 
#bgrp11_bunches=110  # vdm scan
#bgrp12_bunches=0 

###Possible bunch structure for 704 bunches (same as 1180, changing only filled and empy) #BGRP Set 1195                                                     
filled_bunches = 704 #1                                                                                                     
empty_bunches = 1642 #3                                                                                                                                 
empty_after_filled_bunches = 786 #6                                                                                                                  
unp_iso_bunches = 90 #4                                                                                                                                 
unp_noniso_bunches = 30 #5                                                                                                                 
abortgapnotcalib_bunches= 29 #8
bgrp9_bunches=63   # afterglow so not important for physics 
bgrp11_bunches=110  # vdm scan
bgrp12_bunches=0 



###Possible bunch structure for 220bunches (same as 1180, changing only filled and empy)
#filled_bunches = 220
#empty_bunches = 2872
#empty_after_filled_bunches = 130
#unp_iso_bunches = 6
#unp_noniso_bunches = 6
#abortgapnotcalib_bunches= 29  
#bgrp9_bunches=62 
#bgrp11_bunches=110
#bgrp12_bunches=0 
#


#By default we pickup menu xml files from release
#l1_xml = "/afs/cern.ch/atlas/software/releases/17.1.6/AtlasP1HLT/17.1.6.5/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_pp_v5.xml"
#hlt_xml = "/afs/cern.ch/atlas/software/releases/17.1.6/AtlasP1HLT/17.1.6.5/InstallArea/XML/TriggerMenuXML/HLTconfig_Physics_pp_v5_20.2.1.3.xml"
l1_xml = "l1.xml"
hlt_xml = "hlt.xml"
#cost_xml ="/afs/cern.ch/work/y/yygao/public/Trigger/rates/TrigRate_mcv5-20.2.1.2.2-13TeV-nops-L2.0e34_HLT_SMK_72_L1_27_HLT_50.xml"
cost_xml="/afs/cern.ch/user/n/nagano/public/SMK_2162/TrigRate_ppv5-20.2.1.5-13TeV-nops-L2.0e34_HLT_SMK_119_L1_33_HLT_86.xml"


## List of identifier in HLT streamers name, for which efficiency is 1, but has to be set by hand 
## This should matter only if the streamer is seeded by random triggers
list_of_streamers="noalg,sct_noise,HLT_lumipeb"

###
### END CONFIGURATION
###



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




if "userArgs=" in sys.argv[-1]:
    userArgs=sys.argv[-1]
    sys.argv = sys.argv[:-1]


lumi_list  = []
npoints = 4

if len(sys.argv) > 1:
    lumi_list = [int(i) for i in sys.argv[1].split(",")]
else:
    #lumi_list = [1000, 1500,2000] 
    #lumi_list = [120, 160, 200, 240] 
    #lumi_list = [ 7400]
    #lumi_list = sorted([int(round_figures(100 *(1.25**i), 2)) for i in range(npoints)])
    ##insted of 500, use the maxLumi*1.25, just to be sure
    #lumi_list = sorted([int(round_figures(500 /(1.25**i), 2)) for i in range(npoints)])
    #lumi_list = sorted([int(round_figures(800 /(1.25**i), 2)) for i in range(npoints)] +[1000,1250,1500,2000])
    lumi_list = sorted([int(round_figures(1000 /(1.25**i), 2)) for i in range(npoints)] +[1250,1500,1750,2000])
    #lumi_list = sorted([int(round_figures(500 /(1.25**i), 2)) for i in range(npoints)])
    # lumi_list = sorted([int(round_figures(961 * (1.25**i), 2)) for i in range(npoints)] + [9000])  # this gives [960, 1200, 1500, 1900, 2300, 2900, 3700, 4600, 5700, 7200, 9000, 9000]

    ##96945
    ##lumi_list = sorted(list(set([int(round_figures(961 * (1.25**i), 2)) for i in range(npoints)] + [9000])))

lumi_list = zip([lumi_list[0]/1.25] + lumi_list,lumi_list)

print lumi_list
#sleep 10

### Things to edit by hand
if doDBRuleBookDownload:
    rulebook = ["Cosmic_pp_v4_RULES"]
    for i in rulebook:
        ii=i.split("_RULES")
        command="python download_rulebook_from_DB.py %s" % (ii[0])
        os.system(command)
else:
    #rulebook =  ["Physics_pp_v5_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_rules","Physics_pp_v5_VdM_rules"]
    rulebook =  ["Physics_pp_v5_wL1Calo_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_rules","Physics_pp_v5_VdM_rules"]
    #rulebook =  ["Physics_pp_v5_EB_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_rules","Physics_pp_v5_VdM_rules"] 
    #rulebook =  ["Physics_pp_v5_ToroidOff_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_rules","Physics_pp_v5_VdM_rules"]
    #rulebook =  ["Physics_pp_v5_LossMap_rules", "Cosmic_pp_v5_rules","Standby_pp_v5_LossMap_rules","Physics_pp_v5_VdM_rules"]

rulebook = ["TrigMenuRulebook.%s" % r for r in rulebook]

useimports = True


if "TrigMenuRulebook/scripts" in os.getcwd():
    os.system("ln -s ../python TrigMenuRulebook &> /dev/null")
    os.system("ln -s ../share/%s" % l1_xml)
    os.system("ln -s ../share/%s" % hlt_xml)
    os.system("ln -s ../share/%s" % cost_xml)

else:
    os.system('get_files -xmls %s' % cost_xml)
    os.system('get_files -xmls %s' % l1_xml)
    os.system('get_files -xmls %s' % hlt_xml)
    os.system('get_files -scripts cnvXML.py')


if doCosmic or doStandby:
    print
    print "=====> Will also generate Cosmic and Standby prescales"
    print

# Latest ntuples - based on 2012 data
#ebntp="/eos/atlas/atlasdatadisk/data12_8TeV/NTUP_TRIGRATE/r3647/data12_8TeV.00202798.physics_EnhancedBias.recon.NTUP_TRIGRATE.r3647_tid00814396_00/

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


if doPhysics:
    for (lower_lumi_point, lumi_point) in lumi_list:

        print "*** Running:", lumi_point
        logname="log_"+str(lumi_point)
        
        command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point)
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s --target_abortgapnotcalib=%s --target_BGRP9=%s --target_BGRP11=%s --target_BGRP12=%s" % (filled_bunches,empty_bunches, empty_after_filled_bunches, unp_iso_bunches,unp_noniso_bunches,abortgapnotcalib_bunches, bgrp9_bunches,bgrp11_bunches, bgrp12_bunches)
        command += " --rates-xml=%s" % cost_xml
        command += " --streamers=%s" %list_of_streamers
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
        prescale_name = "%s_%04.1f_%04.1fe32"%(short_rulebookname, (float(lower_lumi_point)/100.),float(lumi_point)/100.)
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
                                                       lowlumi='%.1fe32' % (float(lower_lumi_point)/100.),
                                                       highlumi='%.1fe32' % (float(lumi_point)/100.),
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


def doCosmicStandbyPrescales(what):


    lumi_ref = 1

    extra_command = ""

    if what == "Cosmic":
        tryRulebook = rulebook[1]
    elif what == "Standby":
        tryRulebook = rulebook[2]
    elif what == "VdM":
        tryRulebook = rulebook[3]

    print "*** Running "+what

    command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (tryRulebook, what, hlt_xml, l1_xml, lumi_ref)
    command += " --streamers=%s" %list_of_streamers
    if what == "Cosmic":
        print "*** Cosmic Run, set target_empty to 3000 ***"        
        command +=  " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s --target_abortgapnotcalib=%s --target_BGRP9=%s --target_BGRP11=%s --target_BGRP12=%s" % (filled_bunches,3000, empty_after_filled_bunches, unp_iso_bunches,unp_noniso_bunches,abortgapnotcalib_bunches, bgrp9_bunches,bgrp11_bunches,bgrp12_bunches)

    elif what == "Standby" or what=="VdM":
        command +=  " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s --target_abortgapnotcalib=%s --target_BGRP9=%s --target_BGRP11=%s --target_BGRP12=%s" % (filled_bunches,empty_bunches, empty_after_filled_bunches, unp_iso_bunches,unp_noniso_bunches,abortgapnotcalib_bunches, bgrp9_bunches,bgrp11_bunches,bgrp12_bunches)
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




if doCosmic:
    doCosmicStandbyPrescales("Cosmic")

if doStandby:
    doCosmicStandbyPrescales("Standby")

if doVdM:
    doCosmicStandbyPrescales("VdM")




if make_prescale_dir == True:
    mv_command = "mv Set_*.xml %s" % prescale_dir
    os.system(mv_command)


if doUseOnline == True:
    print
    print "... Tarring prescales directory"
    os.system("tar -zcvf %s.tgz %s" % (prescale_dir,prescale_dir))
    print "...rsync to P1"
    os.system("rsync %s.tgz $USER@atlasgw.cern.ch:"  % prescale_dir )
    print

