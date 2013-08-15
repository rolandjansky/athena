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
   Typically this will be 'Physics_pp_v3_rules', but you may
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
import sys

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
npoints = 15

doUseOnline = False
doCosmicStandby = False
doDBRuleBookDownload = False # False python rule book is used, True DB rules are used

if len(sys.argv) > 1:
    lumi_list = sys.argv[1].split(",")
else:
    lumi_list = [int(round_figures(201 * (1.25**i), 2)) for i in range(npoints)]
#    doCosmicStandby = True

### Things to edit by hand
if doDBRuleBookDownload:
    rulebook = ["Physics_pp_v3_RULES","Cosmic_pp_v3_RULES","Standby_pp_v3_RULES"]
    for i in rulebook:
        ii=i.split("_RULES")
        command="python download_rulebook_from_DB.py %s" % (ii[0])
        os.system(command)
else:
    rulebook = ["Physics_pp_v3_25ns_rules"]

rulebook = ["TrigMenuRulebook.%s" % r for r in rulebook]

#25ns #lumi points 50,100,200
filled_bunches = 61
empty_bunches = 3466
unp_iso_bunches = 3
unp_noniso_bunches = 3


#By default we pickup menu xml files from release
#from TrigMenuRulebook.processRules import menuVersion
l1_xml = "LVL1config_Physics_pp_v3.xml"
hlt_xml = "HLTconfig_Physics_pp_v3.xml"
cost_xml = "TriggerCosts_1e33_v3.xml"
#override xml files here
#l1_xml = "LVL1config_Physics_pp_v3_16.1.3.9.xml"
#hlt_xml = "HLTconfig_Physics_pp_v3_16.1.3.9.xml"

useimports = True

if "TrigMenuRulebook/scripts" in os.getcwd():
    os.system("ln -s ../python TrigMenuRulebook &> /dev/null")
    os.system("ln -s ../share/%s" % l1_xml)
    os.system("ln -s ../share/%s" % hlt_xml)
    os.system("ln -s ../share/%s" % cost_xml)
else:
    os.system(' get_files -xmls %s' % cost_xml)
    os.system('get_files -xmls %s' % l1_xml)
    os.system('get_files -xmls %s' % hlt_xml)
    os.system('get_files -scripts cnvXML.py')

if doCosmicStandby == True:
    print
    print "=====> Will also generate Cosmic and Standby prescales"
    print

# OI Apr 13 ntuples from https://savannah.cern.ch/task/?20015#comment2
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.00179938.00179939.physics_EnhancedBias.merge.RAW.12-05-2011_mcppv2_AtlasCAFHLT-16.1.2.6.3_Tags"
##"/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.00178044.physics_EnhancedBias.merge.RAW.04-12-2011_Physics_pp_v2_AtlasCAFHLT-16.1.2.2.1.rerunLV1"
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.00179938.179939.physics_EnhancedBias.merge.RAW.08-05-2011_mcppv2_AtlasCAFHLT-16.1.2.6.3.egam.rerunL1 "
# below are new ntuples for 16.1.2.10
#/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.00179939.physics_EnhancedBias.merge.RAW.08-18-2011_physicsppv2_AtlasCAFHLT-16.1.X.Y.Z.val2.rel3"
# OI Jul 12, 2011   16.1.3.4
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.179939.physics_EnhancedBias.merge.RAW.11-07-2011_Physicsppv2_AtlasP1HLT-16.1.3.4.rerunL1"
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.179939.physics_EnhancedBias.merge.RAW.27-07-2011_Physicsppv2_AtlasCAFHLT-16.1.3.6.2.rerunL1"
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.186923.physics_EnhancedBias.merge.RAW.03-09-2011_Physicsppv3_AtlasP1HLT-16.1.3.10.rerunL1"
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.189421.physics_EnhancedBias.merge.RAW.16-09-2011_MCppv3_AtlasCAFHLT-16.1.3.11.4"
ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.189421.physics_EnhancedBias.merge.RAW.29-09-2011_MCppv3_AtlasP1HLT-16.1.3.14.1"
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

for lumi_point in lumi_list:

    print "*** Running:", lumi_point
    logname="log_"+str(lumi_point)
    command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point)
    command += " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,unp_iso_bunches,unp_noniso_bunches)
    command += " --rates-xml=%s" % cost_xml
    if "userArgs" in dir():
        command += ' "%s"' % userArgs
    print 
    print "*** Will execute:  %s ***" % command
    print
    
    status =run_command(command, useimports)

    if status != 0:
        sys.exit("processRule failed")

    print " *** Converting XMLs to uploadable XMLs"

    short_rulebookname = rulebook[0].split('.')[1].split('_rules')[0] 
    prescale_name = "%s_%.1f_%.1fe32"%(short_rulebookname, (float(lumi_point)/125.),float(lumi_point)/100.)
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
        #ebxml=findFileInXMLPATH("$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_179938_l1pp2.xml");
        ebxml=findFileInXMLPATH("TrigCostRate/enhanced_bias_run_189421.xml")
        dqxml=findFileInXMLPATH("TrigCostRate/dq_189421.xml")

        cmd = "runRates.py \
        "+ebntp+" \
        -o "+output_dir+"/testRatesNew_"+lumi_point+" \
        --prescaleXML=prescales"+lumi_point+".xml \
        --default-ps=-1 \
        --default-pt=-1 \
        --data \
        --do-bg \
        --bg-scaleXML="+ebxml+" \
        --predict-rates \
        --pred-lumi="+lumi_point+"e30 \
        --dq-xml="+ dqxml
        #--rate-scale=1.08 \
        # --rate-scale for old EB run
        
        print 'Execute : %s' % cmd
        
        status = os.system(cmd)
        if status != 0:
            sys.exit("Rate prediction failed")




def doCosmicStandbyPrescales(what):

    
    lumi_ref = 1000

    extra_command = ""

    if what == "Cosmic":
        tryRulebook = rulebook[1]
        extra_command = " --target_empty=3000"        
    elif what == "Standby":
        tryRulebook = rulebook[2]
        extra_command = " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,unp_iso_bunches,unp_noniso_bunches)



    print "*** Running "+what
    command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (tryRulebook, what, hlt_xml, l1_xml, lumi_ref)
    if what == "Cosmic":
        command += " --target_empty=3000"
    elif what == "Standby":
        command += " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,unp_iso_bunches,unp_noniso_bunches)
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


if doCosmicStandby == True:


    doCosmicStandbyPrescales("Cosmic")

    doCosmicStandbyPrescales("Standby")



if doUseOnline == True:
    print
    print "... Tarring prescales directory"
    os.system("tar -zcvf %s.tgz %s" % (prescale_dir,prescale_dir))
    print "...rsync to P1"
    os.system("rsync %s.tgz $USER@atlasgw.cern.ch:"  % prescale_dir )
    print
    
