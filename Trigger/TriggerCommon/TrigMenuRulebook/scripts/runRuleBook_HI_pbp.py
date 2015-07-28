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

if "userArgs=" in sys.argv[-1]:
    userArgs=sys.argv[-1]
    sys.argv = sys.argv[:-1]

lumi_list  = []
npoints = 10


doUseOnline = True
doCosmicStandby = False
doDBRuleBookDownload = False # False python rule book is used, True DB rules are used

if len(sys.argv) > 1:
    lumi_list = sys.argv[1].split(",")
else:
    #lumi_list = [int(round(11 * (1.25**i), -1)) for i in range(npoints)]
    lumi_list = [int(round(20 * (1.4**i),-1)) for i in range(npoints)]  #was 20 and 1.25 before
#    doCosmicStandby = True

print "lumi_list = %s" % lumi_list
    
### Things to edit by hand
if doDBRuleBookDownload:
    rulebook = ["Physics_pp_v2_RULES","Cosmic_pp_v2_RULES","Standby_pp_v2_RULES"]
    for i in rulebook:
        ii=i.split("_RULES")
        command="python download_rulebook_from_DB.py %s" % (ii[0])
        os.system(command)
else:
    #rulebook = ["Physics_HI_v1_rules", "Cosmic_HI_v1_rules", "Standby_HI_v1_rules"]
    rulebook = ["Physics_HI_v1_pbp_rules", "Cosmic_HI_v1_rules", "Standby_HI_v1_rules"]


filled_bunches = 4 #356 #168  
empty_bunches = 2070
unp_iso_bunches = 4
unp_noniso_bunches = 0

#l1_xml = "/afs/cern.ch/atlas/software/releases/16.1.3/AtlasCAFHLT/16.1.3.20.3/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_HI_v1.xml"
#hlt_xml = "/afs/cern.ch/atlas/software/releases/16.1.3/AtlasCAFHLT/16.1.3.20.3/InstallArea/XML/TriggerMenuXML/HLTconfig_MC_HI_v1_16.1.3.20.3.xml"
l1_xml = "L1Menu-1276.xml"
hlt_xml = "HLTMenu-1276.xml"
rates_xml="../share/TriggerCosts_3e25_HI.xml"
online_lumi=30

if doCosmicStandby == True:
    print
    print "=====> Will also generate Cosmic and Standby prescales"
    print

# OI Apr 13 ntuples from https://savannah.cern.ch/task/?20015#comment2
#2010 data
#"/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.00169175.physics_bulk.daq.RAW_19-11-2010_InitialBeamHIv1_AtlasCAFHLT_15.6.9.36.2_TMP-00-13-41-00"
# below are new ntuples for 16.1.3.6.2 for 2011
#ebntp=" /castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.27-07-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.6.2.rerunL1"
# 16.1.3.7.1
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.02-08-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.7.1.rerunL1"
# 16.1.3.7.2
#ebntp="/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.00169751.physics_bulk.daq.RAW.11-08-2011_MCHIV1_AtlasCAFHLT-16.1.3.7.2"
# 16.1.3.13.1
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.22-09-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.13.1.rerunL1"
# 16.1.3.14.1
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.27-09-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.14.1.rerunL1.HIHypo"
# 16.1.3.16.1 This release is not good!
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.08-10-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.16.1.rerunL1"
# 16.1.3.18.2
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.00169751.physics_bulk.daq.RAW.21-10-2011_MCHIV1_AtlasCAFHLT-16.1.3.18.2"
# 16.1.3.18.4
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.00169751.physics_bulk.daq.RAW.26-10-2011_MCHIV1_CAFHLT-16.1.3.18.4"
# 16.1.3.19.1
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.02-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.19.1.rerunL1"
# 16.1.3.19.2
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.04-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.19.2.rerunL1"
# 16.1.3.20.1
#ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.00169751.physics_bulk.daq.RAW.7-11-2011_testMCHIV1_AtlasCAFHLT-16.1.3.20.1"
# 16.1.3.20.3
ebntp="/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_hi.169751.physics_bulk.daq.RAW.13-11-2011_MC_HI_v1_AtlasCAFHLT-16.1.3.20.3.rerunL1"

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
    command = "python ../python/processRules.py --rulebook=%s --log=rulebook_%s.log --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s --online_lumi=%s --rates-xml=%s" % (rulebook[0], lumi_point, hlt_xml, l1_xml, lumi_point, online_lumi, rates_xml)
    command += " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,unp_iso_bunches,unp_noniso_bunches)
    if "userArgs" in dir():
        command += ' "%s"' % userArgs
    print 
    print "*** Will execute:  %s ***" % command
    print
    status = os.system(command)

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
        #ebxml=findFileInXMLPATH("$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_179938_l1pp2.xml");
        ebxml=findFileInXMLPATH("TrigCostRate/enhanced_bias_run_HI.xml")
        dqxml=findFileInXMLPATH("TrigCostRate/dq_179938.xml")

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
        --pred-lumi="+lumi_point+"e24"
        #           --dq-xml="+dqxml
        
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
    command = "python ../python/processRules.py --rulebook=%s --log=rulebook_%s.log --use_lowest_rule --hlt-xml=%s --lvl1-xml=%s --target_lumi=%s" % (tryRulebook, what, hlt_xml, l1_xml, lumi_ref)
    if what == "Cosmic":
        command += " --target_empty=3000"
    elif what == "Standby":
        command += " --target_filled=%s --target_empty=%s --target_unp_iso=%s --target_unp_noniso=%s" % (filled_bunches,empty_bunches,unp_iso_bunches,unp_noniso_bunches)
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



if doUseOnline == True:
    print
    print "... Tarring prescales directory"
    os.system("tar -zcvf %s.tgz %s" % (prescale_dir,prescale_dir))
    print "...rsync to P1"
    os.system("rsync %s.tgz $USER@atlasgw.cern.ch:"  % prescale_dir )
    print
    
