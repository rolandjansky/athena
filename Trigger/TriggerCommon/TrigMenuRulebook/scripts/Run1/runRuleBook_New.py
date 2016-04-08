#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, time, re

global rulebookPath, lumi_list

def print_help():
    print """
%(on_green)sScript to prepare prescale keys for P1 using the rates rulebook.%(ec)s

package: %(rb)s

%(bold)sUsages%(ec)s:

%(bold)s%(prog)s%(ec)s
  With no options, will %(ul)screate prescale sets%(ec)s in ~25%% incremenents
  %(lumis)s
  with units of 1e30 (actually, the units are arbitrary, as
  long as the target_lumi, online_lumi, and breakpoints in the
  rules.py file are the same, you're ok).

%(bold)s%(prog)s -l 100,200%(ec)s
  With just one option, will create prescale sets for the %(ul)sspecified%(ec)s
  %(ul)scomma-separated lumi points%(ec)s

%(bold)s%(prog)s -l 100,200 -p myDir%(ec)s
  With a second option, the script will %(ul)salso run rate predictions%(ec)s for
  the specified lumi points and output them to the specified directory

%(bold)sVariables to edit by hand:%(ec)s

1) l1_xml and hlt_xml
   The location of your xml files, these can be taken from the
   release, downloaded from P1 using the TT, etc...
   Default is to use files from release, so should only need to change if menu at P1 doesn't match release

2) rulebook
   The name of your rulebook file (removing the trailing '.py').
   Typically this will be 'Physics_pp_v4_rules', but you may
   want to use the Standby keys.

3) filled_bunches, empty_bunches, unpaired_isolated, unpaired_nonisolated

   The number of bunches in each of the bunch groups. In stable
   running this doesn't need to be edited. If you're unlucky and this
   changes a lot during your shift, you can get these values by
   feeding the appropriate bunch group ID key in the
   https://atlas-trigconf.cern.ch/ interface.

   The getBunchGroups.py script will return the values to fill in for
   a specific bunch group ID
""" % { 'prog': sys.argv[0].split('/')[-1], 'rb' : rulebookPath, 'lumis' : lumi_list, 'bold' : '\033[1m', 'ul' : '\033[4m', 'ec' : '\033[0m', 'on_green' : '\033[42m'}


def main():
    print os.path.realpath(__file__)


    global lumi_list, cmdline_options

    importRulebook()

    bunchStructure, xmlfiles, rulebook, lumi_list = load_settings("hi")

    opts,args = parseOptions()
    
    ### CONFIGURATION

    doPhysics = True
    doCosmic  = False
    doStandby = False

    doUseOnline = True

    # rate prediction directories
    #ebntp="/eos/atlas/atlasdatadisk/data12_8TeV/NTUP_TRIGRATE/r3647/data12_8TeV.00202798.physics_EnhancedBias.recon.NTUP_TRIGRATE.r3647_tid00814396_00/
    # standard
    ebntp="/afs/cern.ch/user/l/lipeles/eos/atlas/atlasdatadisk/data12_8TeV/NTUP_TRIGRATE/r4034_p966/data12_8TeV.00212967.physics_EnhancedBias.merge.NTUP_TRIGRATE.r4034_p966_tid01012172_00"
    # for 25ns
    # ebntp="/afs/cern.ch/user/l/lipeles/eos/atlas/atlasdatadisk/data12_8TeV/NTUP_TRIGRATE/r4147_p966/data12_8TeV.00212967.physics_EnhancedBias.merge.NTUP_TRIGRATE.r4147_p966_tid01085171_00"

    ### END CONFIGURATION

    linkfiles(xmlfiles)

    if opts.lumi_list:
        lumi_list = [int(i) for i in opts.lumi_list.split(",")]

    lumi_list = zip([lumi_list[0]/1.25] + lumi_list, lumi_list)

    rulebook = dict([ (k,"TrigMenuRulebook.%s"%r) for k,r in rulebook.items()])


    if doPhysics and "Physics" in rulebook:
        print "\n=====> Will generate Physics prescales\n"
        doPhysicsPrescales(lumi_list, rulebook["Physics"], bunchStructure, xmlfiles)

    if doCosmic and "Cosmic" in rulebook:
        print "\n=====> Will generate Cosmic prescales\n"
        doCosmicStandbyPrescales("Cosmic", rulebook["Cosmic"], bunchStructure, xmlfiles)

    if doStandby and "Standby" in rulebook:
        print "\n=====> Will generate Standby prescales\n"
        doCosmicStandbyPrescales("Standby", rulebook["Standby"], bunchStructure, xmlfiles)

    if doVdM and "VdM" in rulebook:
        doCosmicStandbyPrescales("VdM", rulebook["VdM"], bunchStructure, xmlfiles)

    if opts.do_rate_pred:
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



    if getPrescaleDir():
        mv_command = "mv Set_*.xml %s" % getPrescaleDir()
        os.system(mv_command)

    if doUseOnline:
        print "\n... tar-ing prescales directory"
        os.system("tar -zcvf %s.tgz %s" % (getPrescaleDir(),getPrescaleDir()))
        print "...rsync to P1"
        os.system("rsync %s.tgz $USER@atlasgw.cern.ch:"  % getPrescaleDir() )
        print

    

def parseOptions():
    from optparse import OptionParser as op
    parser = op(usage="usage: %prog options")
    parser.add_option( "-l", "--lumi", dest = "lumi_list", help = "provide comma separated list of lumipoints to overwrite default" )
    parser.add_option( "-p", "--predict", dest = "rate_pred_dir", metavar="DIR", help = "runs rate predictions into output directory DIR" )
    parser.add_option( "-s", "--setting", dest = "setting", help = "the settings for which to generate the prescales and rates" )
    parser.print_help = print_help
    return parser.parse_args()



def importRulebook():
    global rulebookPath
    rulebookPath=None
    global BunchStructure, XMLFiles, linkfiles, doPhysicsPrescales, doCosmicStandbyPrescales, PrintSummaryXML, doRatePredictions
    try:
        from TrigMenuRulebook.runRuleBookTools import BunchStructure, XMLFiles, linkfiles, doPhysicsPrescales, doCosmicStandbyPrescales, PrintSummaryXML, doRatePredictions
        rulebookPath = None
    except:
        m = re.match("(.*/TrigMenuRulebook/)scripts/.*", os.path.realpath(__file__) )
        if m:
            rulebookPath = m.group(1)
            sys.path.insert(0,os.path.realpath(rulebookPath + "/python"))
            try:
                from runRuleBookTools import BunchStructure, XMLFiles, linkfiles, doPhysicsPrescales, doCosmicStandbyPrescales, PrintSummaryXML, doRatePredictions
            except ImportError:
                print "Could not import tools from %s/python" % rulebookPath
                sys.exit(1)
        else:
            print "Could not derive where to import tools from"
            sys.exit(1)


def load_settings(setting):
    """ Definition of
    - Bunch group structures
    - XML menu and cost files
    - Rulebook collections
    """

    bs = {}; xml = {}; rb = {}
    
    ## Bunch group structures
    
    # Sept 2012, ALFA setting (BGR: 507)
    bs['alfa']   = BunchStructure(FILLED =    2, EMPTY = 3425, EMPTY_AFTER_FILLED =    2, UNP_ISO =   2, UNP_NONISO =  2)

    # Cosmic
    bs['cosmic'] = BunchStructure(FILLED =    2, EMPTY = 3560, EMPTY_AFTER_FILLED =    0, UNP_ISO =   0, UNP_NONISO =  0)

    # hi
    bs['hi']     = BunchStructure(FILLED =  296, EMPTY = 1000, EMPTY_AFTER_FILLED = 1000, UNP_ISO = 100, UNP_NONISO = 60)

    # 480b setting (https://atlas-trigconf.cern.ch/bunchgroups?key=479)
    bs['480b']   = BunchStructure(FILLED =  471, EMPTY = 2356, EMPTY_AFTER_FILLED =  531, UNP_ISO = 6, UNP_NONISO = 12)

    # 1368b setting
    bs['1368b']  = BunchStructure(FILLED = 1368, EMPTY =  390, EMPTY_AFTER_FILLED = 1520, UNP_ISO = 6, UNP_NONISO =  6)

    ## XML menu and cost file collections

    ## By default we pickup menu xml files from release
    xml['hi']   = XMLFiles(l1="LVL1config_Physics_HI_v2_17.1.6.3.xml",    hlt="HLTconfig_Physics_HI_v2.xml",    cost="TriggerCosts_1e29_pPb.xml")

    xml['25ns'] = XMLFiles(l1="LVL1config_Physics_pp_v4_17.1.5.22.1.xml", hlt="HLTconfig_Physics_pp_v4_17.1.5.22.1.xml", cost="TriggerCosts_Physics_pp_v4_25ns.xml")


    ## rulebook collections
    rb['hi'] = {
        "Physics" : "Physics_HI_v2_rules",
        "Standby" : "Standby_HI_v2_rules",
        "Cosmic"  : "Cosmic_pp_v4_rules",
        "VdM"     : "VdM_HI_v1_rules"
        }

    rb['25ns'] = {
        "Physics" : "Physics_pp_v4_25ns_rules",
        }

    lumi_list = [960, 1200, 1500, 1900, 2300, 2900, 3700, 4600, 5700, 6300, 6900, 7400, 8000, 8500, 9000, 10000]
    if setting=='hi':
        npoints = 10
        lumi_list = [int(round(20000 * (1.25**i),-4)) for i in range(npoints)]


    return bs[setting], xml[setting], rb[setting], lumi_list



if __name__=="__main__":
    sys.exit(main())
