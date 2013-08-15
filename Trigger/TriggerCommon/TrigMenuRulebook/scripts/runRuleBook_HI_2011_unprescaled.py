# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Script to prepare prescale keys for P1 using the rates rulebook

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
   Typically this will be 'Physics_pp_v2_rules', but you may
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
npoints = 20


doUseOnline = True
doCosmicStandby = False
doVdM = False
doDBRuleBookDownload = False # False python rule book is used, True DB rules are used

if len(sys.argv) > 1:
    lumi_list = sys.argv[1].split(",")
else:
    #lumi_list = [int(round(11 * (1.25**i), -1)) for i in range(npoints)]
    lumi_list = [int(round(10 * (1.3**i),-1)) for i in range(npoints)]  #was 20 and 1.25 before
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
    rulebook = ["Physics_HI_v1_rules", "Cosmic_HI_v1_rules", "Standby_HI_v1_rules", "VdM_HI_v1_rules"]

rulebook = ["TrigMenuRulebook.%s" % r for r in rulebook]

filled_bunches = 356 #168  
empty_bunches = 2070
empty_after_filled_bunches = 10
unp_iso_bunches = 4
unp_noniso_bunches = 0

#l1_xml = "/afs/cern.ch/atlas/software/releases/16.1.3/AtlasCAFHLT/16.1.3.24.1/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_HI_v1.xml"
#hlt_xml = "/afs/cern.ch/atlas/software/releases/16.1.3/AtlasCAFHLT/16.1.3.24.1/InstallArea/XML/TriggerMenuXML/HLTconfig_MC_HI_v1_16.1.3.24.1.xml"
l1_xml = "/afs/cern.ch/atlas/software/releases/17.1.5/AtlasCAFHLT/17.1.5.12.1/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_HI_v2.xml"
hlt_xml = "/afs/cern.ch/atlas/software/releases/17.1.5/AtlasCAFHLT/17.1.5.12.1/InstallArea/XML/TriggerMenuXML/HLTconfig_Physics_HI_v2_17.1.5.12.1.xml"
#l1_xml = "../share/L1Menu-1278.xml"
#hlt_xml = "../share/HLTMenu-1278.xml"
cost_xml="TriggerCosts_1e25_HI.xml"
online_lumi=10


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

    
if doCosmicStandby == True:
    print
    print "=====> Will also generate Cosmic and Standby prescales"
    print

# OI Apr 13 ntuples from https://savannah.cern.ch/task/?20015#comment2
#2010 data
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.19-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.22.2.rerunL1"
# 16.1.3.24.1
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.22-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.24.1.rerunL1"
#2011 data
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_hi.193270.physics_bulk.daq.RAW.17-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.20.3"
#17.1.5.10.2
#ebntp="/eos/atlas/atlasdatadisk/data11_hi/NTUP_TRIGRATE/r3811/data11_hi.00193270.physics_MinBias.recon.NTUP_TRIGRATE.r3811_tid00905988_00/"
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_hi.00193270.physics_MinBias.merge.RAW.10-07-2012_testMCHIV2_AtlasP1HLT-17.1.5.10.2/"
#17.1.5.12.1
ebntp="/eos/atlas/atlasdatadisk/data11_hi/NTUP_TRIGRATE/r3847/data11_hi.00193270.physics_MinBias.recon.NTUP_TRIGRATE.r3847_tid00925948_00/"

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
    
    command = "--rulebook=%s --log=rulebook_%s.log --force-rates-metadata --use_lowest_rule --no-bgrp-scaling --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s " % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point)
    command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
    command += " --rates-xml=%s" % cost_xml
    if "userArgs" in dir():
        command += ' "%s"' % userArgs
    print 
    print "*** Will execute:  %s ***" % command
    print
    status = run_command(command, useimports)

    if status != 0:
        sys.exit("processRule failed")

    print " *** Converting XMLs to uploadable XMLs"
    prescale_name = "%s_%.1fe26_%.1fe26"%(rulebook[0], (float(lumi_point)/125.),float(lumi_point)/100.)
    prescale_name += "_%sbunches" % filled_bunches
    command_cnv   = "python ../scripts/cnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %(prescale_name, lumi_point)
                                                                             
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
        --pred-lumi="+lumi_point+"e24 \
        --joblist default \
        --forwardArgs='-k TRIGRATE '"
        
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
        extra_command = " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
    elif what == "VdM":
        tryRulebook = rulebook[3]
        extra_command = " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)



    print "*** Running "+what
    command = "python ../python/processRules.py --rulebook=%s --log=rulebook_%s.log --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (tryRulebook, what, hlt_xml, l1_xml, lumi_ref)
    if what == "Cosmic":
        command += " --target_empty=3000"
    elif what == "Standby":
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
    elif what == "VdM":
        command += " --target_filled=%s --target_empty=%s --target_empty_after_filled=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,empty_after_filled_bunches,unp_iso_bunches,unp_noniso_bunches)
    print 
    print "*** Will execute:  %s ***" % command
    print
    os.system(command)

    print " *** Converting XMLs to uploadable XMLs"
    prescale_name = what
    command_cnv   = "python cnvXML.py --ps_name=%s --ps_xml=prescales%s.xml " %(prescale_name, lumi_ref)
                                                                             
    os.system(command_cnv)

    if make_prescale_dir == True:
        mv_command = "mv RuleBook_*PS_%s* %s"%(prescale_name,prescale_dir) 
        os.system(mv_command)


if doCosmicStandby == True:


    doCosmicStandbyPrescales("Cosmic")

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
    
