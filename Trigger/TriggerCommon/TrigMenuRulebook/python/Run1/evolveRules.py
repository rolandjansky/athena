#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Produce a prescales XML file based on a rulebook
"""

from copy import deepcopy
import logging
import xml.etree.cElementTree as ET
from RuleTools import read_online_rates, read_online_metadata, build_tree, calc_eff, round_figures
import UserFunctions
import commands, math, sys
import os
import re
import cPickle

from disabledOK import can_be_disabled
import processRules


def dumpTriggers(triggers,lumi):
    enabled={ 'L1': set(),
              'L2': set(),
              'EF': set(),}
    for lvl in enabled:
        for trig in triggers:
            if trig.startswith(lvl):
                enabled[lvl].add(trig)
        fileName='%s_%4d.menu' % (lvl,lumi)
        cPickle.dump( (enabled[lvl], lumi/1000.), open(fileName,'wb'),2)


def main():
    """Call the methods to set up the chains, read the pre-computed rates, etc...
    """
    if 'userArgs=' in sys.argv[-1]:
        if len([word for word in ("import", "os", "sys", "open", "file", "compile", "eval") if word in sys.argv[-1]]):
            sys.exit("ERROR: Cannot handle user argument: %s" % sys.argv[-1])
        exec sys.argv[-1]
        sys.argv=sys.argv[:-1]
    else:
        userArgs={}

    from optparse import OptionParser
    p = OptionParser(usage = "usage: python processRules.py", version = "1.0")

    p.add_option('-r', '--rulebook',       type  = "str",         default = "Physics_pp_v2_rules",   dest = "rulebook",               help = "rulebook to process"                                                                     )
    p.add_option('-x', '--rates-xml',      type  = "str",         default = "../share/TriggerCosts_1e33.xml", dest = "online_xml",    help = "rates xml"                                                                               )
    p.add_option(      '--online_lumi',    type  = "int",                                            dest = "online_lumi",            help = "rates xml lumi (x1e30)"                                                                  )
    p.add_option('-l', '--target_lumi',    type  = "int",         default = 1000,                    dest = "target_lumi",            help = "target lumi (x1e30)"                                                                     )
    p.add_option('-s', '--scale_lumi',     type  = "str",         default = "1/1.15",                dest = "scale_lumi",             help = "scale lumi (1.15 for online data taking based on Brian's proposal)"                      )
    p.add_option('-m', '--trigMenu',       type  = "str",         default = "Physics_pp_v2",         dest = "trigMenu",                                                                                                                )
    p.add_option(      '--rel',            type  = "str",                                            dest = "rel",                    help = "release number"                                                                          )
    p.add_option(      '--AtlasHLT',       type  = "str",                                            dest = "atlasHLT",               help = "P1 or CAF"                                                                               )
    p.add_option(      '--lvl1-xml',       type  = "str",         default = 'None',                  dest = "l1_xml",                 help = "lvl1 menu xml"                                                                           )
    p.add_option(      '--hlt-xml',        type  = "str",         default = 'None',                  dest = "hlt_xml",                help = "hlt menu xml"                                                                            )
    p.add_option('--no-rates-metadata',    action = "store_true", default = False,                   dest = "no_online_metadata",     help = "disable the automatic reading of the rates xml metadata"                                 )
    p.add_option('--force-rates-metadata', action = "store_true", default = False,                   dest = "force_online_metadata",  help = "overwrite any default parameters with the rates xml metadata"                            )
    p.add_option('--no-bgrp-scaling',      action = "store_true", default = False,                   dest = "disable_bunch_scaling",  help = "disable bunch group scaling calculations, scale factor will always be 1"                 )
    p.add_option('--online_filled',        type  = "int",                                            dest = "online_filled",          help = "# filled bunches in rates xml"                                                           )
    p.add_option('--online_empty',         type  = "int",                                            dest = "online_empty",           help = "# empty bunches in rates xml"                                                            )
    p.add_option('--online_unp_iso',       type  = "int",                                            dest = "online_unpaired_iso",    help = "# unpaired_iso bunches in rates xml"                                                     )
    p.add_option('--online_unp_noniso',    type  = "int",                                            dest = "online_unpaired_noniso", help = "# unpaired_noniso bunches in rates xml"                                                  )
    p.add_option('--target_filled',        type  = "int",         default = 700,                     dest = "target_filled",          help = "# target filled bunches"                                                                 )
    p.add_option('--target_empty',         type  = "int",         default = 2000,                    dest = "target_empty",           help = "# target empty bunches"                                                                  )
    p.add_option('--target_unp_iso',       type  = "int",         default = 100,                     dest = "target_unpaired_iso",    help = "# target unpaired_iso bunches"                                                           )
    p.add_option('--target_unp_noniso',    type  = "int",         default = 10,                      dest = "target_unpaired_noniso", help = "# target unpaired_noniso bunches"                                                        )
    p.add_option('--use_lowest_rule',      action = "store_true", default = True,                    dest = "use_lowest_rule",        help = "optionally use the lowest available rule when none has been defined for the target lumi" )
    p.add_option('--dump',                 action = "store_true", default = False,                   dest = "dump",                   help = "optionally dump list of unprescaled triggers for rate GUI "                              )
    p.add_option('--log',                  type  = "str",         default = "",                      dest = "log",                    help = "optionally print the final state of the trigger tree to a log file"                      )
    p.add_option('--debug',                action = "store_true",                                    dest = "debug",                  help = "enable debug output"                                                                     )
    p.add_option('-q', '--quiet',          action = "store_true",                                    dest = "quiet",                  help = "disable most output"                                                                     )
    p.add_option('-v', '--verbosity',      type  = "int",         default = 2,                       dest = "verbosity",              help = "set level of detail on the output (values from 0 to 5)"                                  )

    #*#** Note2: we should make it read the xml from a release too. 
    
    (options, args) = p.parse_args()
    
    if options.quiet:
        options.verbosity = 0

    if options.debug:
        options.verbosity = 5

    logger = processRules.create_logger("RuleReader", options.verbosity)

    if len(args) > 0:
        logger.warning("Unknown arguments: %s" % args)
    
    if options.atlasHLT != None and options.rel == None:
        raise ValueError("P1 or CAF requires release number")
    elif options.rel != None and options.atlasHLT == None:
        raise ValueError("Release number requires P1 or CAF")

    if (options.quiet and options.debug):
        raise ValueError("Cannot specify both --debug and --quiet")

    online_metadata = {}
    if not options.no_online_metadata:
        online_metadata = read_online_metadata(ET.parse(options.online_xml).getroot(),
                                               {1: "filled",
                                                3: "empty",
                                                4: "unpaired_iso",
                                                5: "unpaired_noniso"
                                                }
                                               )
    for key, val in online_metadata.items():
        key = "online_%s" % key
        if getattr(options, key, None) == None or options.force_online_metadata:
            setattr(options, key, val)


    #online_empty           = options.online_empty #*#** this could be calculated from the rest
    #target_lumi_unscaled   = options.target_lumi

    ##Since we're using an eval, we should at least put some
    ##protections around this. Let's limit the possible characters
    ##to numbers and mathematical operators
    if re.search("[^0-9+-./*]", options.scale_lumi):
        logger.error("--scale_lumi can only be handed numbers and mathematical operators")
        return
    else:
        options.scale_lumi             = float(eval(options.scale_lumi))

    #options.target_lumi    = int(options.scale_lumi * target_lumi_unscaled)
    #target_empty           = options.target_empty #*#** this could be calculated from the rest

    if options.rel != None and options.atlasHLT != None:

        if options.atlasHLT == "CAF":

            options.l1_xml = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z/AtlasCAFHLT/"+options.rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v2_*xml")
            options.hlt_xml = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y.Z/AtlasCAFHLT/"+options.rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v2_*xml")

            logger.info("***** CAFHLT is using 15.6.X.Y.Z *****")

        elif options.atlasHLT == "P1":

            options.l1_xml = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y-VAL/AtlasP1HLT/"+options.rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_MC_pp_v2_*xml")
            options.hlt_xml = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/16.1.X.Y-VAL/AtlasP1HLT/"+options.rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_MC_pp_v2_*xml")

            logger.info("***** P1HLT is using 16.1.X.Y *****")
            logger.info("***** Physics_pp_v1 LVL1 Menu *****")
            logger.info("********** MC_pp_v2 HLT1 Menu *****")

    elif  options.trigMenu != 'None' and options.hlt_xml == 'None' and options.l1_xml == 'None'  :

        logger.info(' Will try to find xmls ')
        nightlyPaths = os.environ['XMLPATH'].split(':')

        for p in nightlyPaths:
            hltpath = p+"/"+processRules.HLTname(options.trigMenu)
            if os.path.exists(hltpath) is True:
                options.hlt_xml = hltpath
                break
            else:
                logger.info("Can not find HLT xml %s" % hltpath)
                return
 
        for p in nightlyPaths:
            l1path = p+"/"+processRules.L1name(options.trigMenu)
            if os.path.exists(l1path) is True:
                options.l1_xml = l1path
                break
            else:
                logger.info("Can not find L1 xml %s" % l1path)
                return

    ##User request to have the header w/o the logger preprended text
    config_output = "\n"
    config_output += "="*100 + "\n"
    config_output += "Run with: \n"
    for key, val in vars(options).iteritems():
        config_output += "%s%s = %s \n" % (key, " "*(30 - len(key)), val)
    config_output += "="*100 + "\n"
    logger.info(config_output)


    os.system("cp "+options.l1_xml +" l1_xml.xml")
    os.system("cp "+options.hlt_xml+" hlt_xml.xml")    
    outputFileName = "prescales"+str(options.target_lumi)+".xml"

    #This needs to be changed to import a specific file
    #exec( "from %s import rules" % options.rulebook)
    rulebook = __import__(options.rulebook)
    rules = rulebook.rules
    rulelocation = rulebook.__file__
    if rulelocation[-1] == "c":
        rulelocation = rulelocation[:-1]
       
    processRules.check_rulebook_for_duplicates(rulelocation, logger) 

    UserFunctions.modifyRules(rules, userArgs)

    
    configuration = {"l1_xml"                 : options.l1_xml, 
                     "hlt_xml"                : options.hlt_xml,
                     "scale_lumi"             : options.scale_lumi,
                     "disable_bunch_scaling"  : options.disable_bunch_scaling,
                     "target_lumi"            : options.target_lumi,
                     "target_filled"          : options.target_filled,
                     "target_empty"           : options.target_empty,
                     "target_unpaired_iso"    : options.target_unpaired_iso,
                     "target_unpaired_noniso" : options.target_unpaired_noniso,
                     "online_xml"             : options.online_xml,
                     "online_lumi"            : options.online_lumi,
                     "online_filled"          : options.online_filled,
                     "online_empty"           : options.online_empty,
                     "online_unpaired_iso"    : options.online_unpaired_iso,
                     "online_unpaired_noniso" : options.online_unpaired_noniso,
                     "use_lowest_rule"        : options.use_lowest_rule,
                     "log"                    : options.log,
                     "verbosity"              : options.verbosity,
        }

    reader = processRules.RuleReader(deepcopy(configuration), rules, logger)
    reader.compute_ps()
    UserFunctions.modifyPrescales(reader.all_items)
    prescaled={}
    unprescaled=set()
    for item in reader.all_items:
        if reader.all_items[item].total_ps()!=1 or reader.all_items[item].ps!=1:
            prescaled[item]=reader.all_items[item].rate
        else:
            unprescaled.add(item)
    if options.dump:
        dumpTriggers(unprescaled,configuration["target_lumi"])

    while configuration["target_lumi"]<5000.:
        configuration["target_lumi"]+=100
#        print 'Looking at lumi:',configuration["target_lumi"]
        reader = processRules.RuleReader(deepcopy(configuration), rules, logger)
        reader.compute_ps()
        UserFunctions.modifyPrescales(reader.all_items)
        unprescaled=set()
        newprescaled=[]
        for item in reader.all_items:
            if reader.all_items[item].total_ps()!=1 or reader.all_items[item].ps!=1:
                if not item in prescaled.keys() and item.startswith('EF'):
                    newprescaled.append(item)
                prescaled[item]=reader.all_items[item].rate
            else:
                unprescaled.add(item)
        if len(newprescaled):
            newprescaled.sort()
            print "Newly prescaled at lumi %de30" % configuration["target_lumi"]
            for item in newprescaled:
                print ' %-40s (to %4.1f Hz)' % (item,prescaled[item])
            print
            if options.dump:
                dumpTriggers(unprescaled,configuration["target_lumi"])


if __name__ == "__main__":
    main()

