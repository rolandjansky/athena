#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Script to prepare prescale keys for P1 using the rates rulebook

p+Pb luminosity points:
8 bunches - 2000

usage:
python runRuleBook_HI.py
  With no options, will create keys in 25% incremenents (100, 125,
  160,...) with units of 1e24 (actually, the units are arbitrary, as
  long as the target_lumi, online_lumi, and breakpoints in the
  rules.py file are the same, you're ok).

python runRuleBook_HI.py 100,200
  With just one option, will create key for the specified
  comma-separated lumi points

python runRuleBook_HI.py 100,200 myDir
  With a second option, the script will also run rate predictions for
  the specified lumi points and output them to the specified directory

Variables to edit by hand
1) l1_xml and hlt_xml
   The location of your xml files, these can be taken from the
   release, downloaded from P1 using the TT, etc...

2) rulebook
   The name of your rulebook file (removing the trailing '.py').
   Typically this will be 'Physics_HI_v2_rules', but you may
   want to use the Standby keys.

3) filled_bunches, empty_bunches, unpaired_isolated, unpaired_nonisolated

   The number of bunches in each of the bunch groups. In stable
   running this doesn't need to be edited. If you're unlucky and this
   changes a lot during your shift, you can get these values by
   feeding the appropriate bunch group ID key in the
   https://atlas-trigconf.cern.ch/ interface.

   The getBunchGroups.py script will return the values to fill in for
   a specific bunch group ID
   
"""

import sys
import commands
import os
import time
import math

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

if "userArgs=" in sys.argv[-1]:
    userArgs=sys.argv[-1]
    sys.argv = sys.argv[:-1]

lumi_list  = []
npoints = 10


doUseOnline = True
doPhysics = True
doCosmic = False
doStandby = True
doVdM = False
doDBRuleBookDownload = False # False python rule book is used, True DB rules are used

if len(sys.argv) > 1:
    lumi_list = sys.argv[1].split(",")
else:
    #lumi_list = [int(round(11 * (1.25**i), -1)) for i in range(npoints)]
    lumi_list = [int(round(20000 * (1.25**i),-4)) for i in range(npoints)]  #was 20 and 1.25 before
#    doCosmicStandby = True

print "lumi_list = %s" % lumi_list
    
### Things to edit by hand
if doDBRuleBookDownload:
    rulebook = ["Physics_pp_v4_rules","Cosmic_pp_v4_rules","Standby_pp_v4_rules"]
    for i in rulebook:
        ii=i.split("_rules")
        command="python download_rulebook_from_DB.py %s" % (ii[0])
        os.system(command)
else:
    rulebook = ["Physics_HI_v2_pp_2p76TeV_rules", "Cosmic_HI_v1_rules", "Standby_HI_v2_rules", "VdM_HI_v2_rules"]
    

rulebook = ["TrigMenuRulebook.%s" % r for r in rulebook]

filled_bunches = 1278 #29 #1368 #471 #46 
empty_bunches = 375 #3240 #390 #2356 #3223 
empty_after_filled_bunches = 1420 #29 #1520 #531 #46
unp_iso_bunches = 48 #10 #6 #28
unp_noniso_bunches = 108 #10 #6 #12 #28

#l1_xml = "LVL1config_Physics_HI_v2.xml"
#hlt_xml = "HLTconfig_Physics_HI_v2_17.1.6.6.xml"
#
l1_xml = "/afs/cern.ch/atlas/software/releases/17.1.6/AtlasP1HLT/17.1.6.8/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_HI_v2_17.1.6.8.xml"
hlt_xml = "/afs/cern.ch/atlas/software/releases/17.1.6/AtlasP1HLT/17.1.6.8/InstallArea/XML/TriggerMenuXML/HLTconfig_Physics_HI_v2_17.1.6.8.xml"
#AT l1_xml = "/afs/cern.ch/atlas/software/releases/17.1.6/AtlasCAFHLT/17.1.6.6.3/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_HI_v2.xml"
#AT hlt_xml = "/afs/cern.ch/atlas/software/releases/17.1.6/AtlasCAFHLT/17.1.6.6.3/InstallArea/XML/TriggerMenuXML/HLTconfig_MC_HI_v2_17.1.6.6.3.xml"
#hlt_xml = "./HLTconfig_MC_HI_v2_17.1.5.20.1.xml"
#l1_xml = "../share/L1Menu-1278.xml"
#hlt_xml = "../share/HLTMenu-1278.xml"
cost_xml="TriggerCosts_1e32_pp_2p76TeV.xml"
online_lumi=100000 #AT 1e32


useimports = True

if "TrigMenuRulebook/scripts" in os.getcwd():
    os.system("ln -s ../python TrigMenuRulebook &> /dev/null")
    os.system("ln -s %s" % l1_xml)
    os.system("ln -s %s" % hlt_xml)
    os.system("ln -s ../share/%s" % cost_xml)
else:
    os.system(' get_files -xmls %s' % cost_xml)
    os.system('get_files -xmls %s' % l1_xml)
    os.system('get_files -xmls %s' % hlt_xml)
    os.system('get_files -scripts cnvXML.py')

    
if doCosmic or doStandby:
    print
    print "=====> Will also generate Cosmic and Standby prescales"
    print

# OI Apr 13 ntuples from https://savannah.cern.ch/task/?20015#comment2
#2010 data
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.19-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.22.2.rerunL1"
# 16.1.3.24.1
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.22-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.24.1.rerunL1"
#2011 data
#17.1.6.1.8 (muon problem)
#17.1.6.4
#ebntp="/eos/atlas/atlasdatadisk/data12_hip/NTUP_TRIGRATE/r4224_p966/data12_hip.00210184.physics_MinBiasOverlay.merge.NTUP_TRIGRATE.r4224_p966_tid01180678_00/"
#17.1.6.5
#ebntp="/eos/atlas/atlasdatadisk/data12_hip/NTUP_TRIGRATE/r4227_p966/data12_hip.00210184.physics_MinBiasOverlay.merge.NTUP_TRIGRATE.r4227_p966_tid01181658_00/"
#17.1.6.4.1 run 217999 from 2013
#ebntp="/eos/atlas/atlasdatadisk/data13_hip/NTUP_TRIGRATE/r4230_p966/data13_hip.00217999.physics_MinBiasOverlay.merge.NTUP_TRIGRATE.r4230_p966_tid01183423_00/"
#17.1.6.5.1
#ebntp="/eos/atlas/atlasdatadisk/data13_hip/NTUP_TRIGRATE/r4252_p966/data13_hip.00217999.physics_MinBiasOverlay.merge.NTUP_TRIGRATE.r4252_p966_tid01186990_00/"

#17.1.6.5.2 pp at 2.76TeV
#ebntp="/eos/atlas/atlasdatadisk/data11_2p76TeV/NTUP_TRIGRATE/r4268_p966/data11_2p76TeV.00178211.physics_MinBias.merge.NTUP_TRIGRATE.r4268_p966_tid01190372_00/"
#17.1.6.6.1 pp at 2.76TeV
#ebntp="/afs/cern.ch/user/i/igrabows/eos/atlas/atlasdatadisk/data11_2p76TeV/NTUP_TRIGRATE/r4275_p966/data11_2p76TeV.00178211.physics_MinBias.merge.NTUP_TRIGRATE.r4275_p966_tid01194516_00/"
#17.1.6.6.2 pp at 2.76TeV
#ebntp="/afs/cern.ch/user/i/igrabows/eos/atlas/atlasdatadisk/data11_2p76TeV/NTUP_TRIGRATE/r4281_p966/data11_2p76TeV.00178211.physics_MinBias.merge.NTUP_TRIGRATE.r4281_p966_tid01195719_00/"
#17.1.6.6.3 pp at 2.76TeV
ebntp="/afs/cern.ch/user/i/igrabows/eos/atlas/atlasdatadisk/data11_2p76TeV/NTUP_TRIGRATE/r4285_p966/data11_2p76TeV.00178211.physics_MinBias.merge.NTUP_TRIGRATE.r4285_p966_tid01196398_00/"

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

if doPhysics:
    for lumi_point in lumi_list:

        print "*** Running:", lumi_point
        logname="log_"+str(lumi_point)

#        command = "--rulebook=%s --log=rulebook_%s.log --use_lowest_rule --no-bgrp-scaling --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point)
        command = "--rulebook=%s --log=rulebook_%s.log --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point)        
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
        command += " --rates-xml=%s --online_lumi=%s" % (cost_xml, online_lumi)
        if "userArgs" in dir():
            command += ' "%s"' % userArgs
        print 
        print "*** Will execute:  %s ***" % command
        print
        status = run_command(command, useimports)

        if status != 0:
            sys.exit("processRule failed")

        print " *** Converting XMLs to uploadable XMLs"
        short_rulebookname = rulebook[0].split('.')[1].split('_rules')[0]

        #prescale_name = "%s_%04.1f_%04.1fe32"%(short_rulebookname, (float(lower_lumi_point)/100.),float(lumi_point)/100.)

        prescale_name = "%s_%04.1f_%04.1fe29"%(short_rulebookname, (float(lumi_point)/125.),float(lumi_point)/100.)

        prescale_name += "_%sb" % filled_bunches
        command_cnv   = "python %scnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %("" if useimports else "../scripts/", prescale_name, lumi_point)

        status = os.system(command_cnv)
        if status != 0:
            sys.exit("XML conversion failed")

        if make_prescale_dir == True:
            mv_command = "mv RuleBook_*PS_%s* %s"%(prescale_name,prescale_dir) 
            status = os.system(mv_command)
            if status != 0:
                sys.exit("Move failed")


        if len(sys.argv)>2:
            from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
            cmd = "runEB.py \
            -i "+ebntp+" \
            -o "+output_dir+" \
            --pred-lumi="+lumi_point+"e27 \
            --joblist default_tiny \
            --max-num-files -1 \
            --forwardArgs='-k TRIGRATE --prescaleXML=prescales"+lumi_point+".xml --rate-scale 2.0'"

            print 'Execute : %s' % cmd

            status = os.system(cmd)
            if status != 0:
                sys.exit("Rate prediction failed")




def doCosmicStandbyPrescales(what):
    lumi_ref = 80000

    extra_command = ""

    if what == "Cosmic":
        tryRulebook = rulebook[1]
        extra_command = " --target_empty=3000"
    elif what == "Standby":
        tryRulebook = rulebook[2]
        extra_command = " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
    elif what == "VdM":
        tryRulebook = rulebook[3]
        extra_command = " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)

    print "*** Running "+what
    command = "--rulebook=%s --log=rulebook_%s.log --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (tryRulebook, what, hlt_xml, l1_xml, lumi_ref)
    if what == "Cosmic":
        command += " --target_empty=3000"
        command += " --rates-xml=%s" % cost_xml
    elif what == "Standby":
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
        command += " --rates-xml=%s" % cost_xml
    elif what == "VdM":
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
        command += " --rates-xml=%s --online_lumi=%s" % (cost_xml, online_lumi)

    print 
    print "*** Will execute:  %s ***" % command
    print
    status = run_command(command, useimports)

    print " *** Converting XMLs to uploadable XMLs"
    prescale_name = what
    command_cnv   = "python %scnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %("" if useimports else "../scripts/", prescale_name, lumi_ref)
                                                                             
    os.system(command_cnv)

    if make_prescale_dir == True:
        mv_command = "mv RuleBook_*PS_%s* %s"%(prescale_name,prescale_dir) 
        os.system(mv_command)


if doCosmic:
    doCosmicStandbyPrescales("Cosmic")

if doStandby:
    doCosmicStandbyPrescales("Standby")

if doVdM:
    doCosmicStandbyPrescales("VdM")


if doUseOnline == True:
    print
    print "... Tarring prescales directory"
    os.system("tar -zcvf %s.tgz %s" % (prescale_dir,prescale_dir))
    print "...rsync to P1"
    os.system("rsync %s.tgz $USER@atlasgw.cern.ch:"  % prescale_dir )
    print
    
