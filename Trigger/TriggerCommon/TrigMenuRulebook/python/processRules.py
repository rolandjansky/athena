#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Produce a prescales XML file based on a rulebook
"""

from RuleTools import read_online_metadata, create_logger
from RulebookProcessors import *
import UserFunctions
import commands, sys
import os
import re

### Maximum prescale the Trigger Tool will allow

menuMap = {
    #2011 menus:          menu name                      L1 xml file
    'Physics_pp_v2': ('Physics_pp_v2',                  'TriggerMenuXML/LVL1config_Physics_pp_v2.xml'),
    'MC_pp_v2':      ('MC_pp_v2',                       'TriggerMenuXML/LVL1config_MC_pp_v2.xml'),
    'L1_pp_v2':      ('L1_pp_v2',                       'TriggerMenuXML/LVL1config_L1_pp_v2.xml'),
    
    #2010 menus:          menu name                      L1 xml file
    'CosmicV2':      ('Cosmic_v2',                      'TriggerMenuXML/LVL1config_InitialBeam_v2.xml'),
    'CosmicV3':      ('Cosmic_v3',                      'TriggerMenuXML/LVL1config_InitialBeam_v3.xml'),
    
    'HIV1':          ('InitialBeam_HI_v1',              'TriggerMenuXML/LVL1config_InitialBeam_HI_v1.xml'),
    'MCHIV1':        ('MC_InitialBeam_HI_v1',           'TriggerMenuXML/LVL1config_InitialBeam_HI_v1.xml'),
    'PhysicsV1':     ('Physics_pp_v1',                  'TriggerMenuXML/LVL1config_Physics_pp_v1.xml'),
    'MCV1':          ('MC_pp_v1',                       'TriggerMenuXML/LVL1config_Physics_pp_v1.xml'),
    'CosmicPPV1':    ('Physics_pp_v1_cosmics_prescale', 'TriggerMenuXML/LVL1config_Physics_pp_v1.xml'),
    }

   
def L1name( name) :
    return menuMap[name][1]

def HLTname( name) :
    from AthenaCommon.AppMgr import release_metadata
    return "TriggerMenuXML/HLTconfig_"+menuMap[name][0]+"_"+release_metadata()['release']+".xml"

def main():
    """Call the methods to set up the chains, read the pre-computed rates, etc...
    """

    if 'userArgs=' in sys.argv[-1]:
        if len([word for word in ("import", "os", "sys", "open", "file", "compile", "eval") if word in sys.argv[-1]]):
            sys.exit("ERROR: Cannot handle user argument: %s" % sys.argv[-1])
        #exec sys.argv[-1]
        userArgs=sys.argv[-1]
        sys.argv=sys.argv[:-1]
    else:
        userArgs={}

    from optparse import OptionParser
    p = OptionParser(usage = "usage: python processRules.py", version = "1.0")

    p.add_option('-r', '--rulebook',       type  = "str",         default = "Physics_pp_v2_rules",   dest = "rulebook",               help = "rulebook to process"                                                                     )
    p.add_option('-x', '--rates-xml',      type  = "str",         default = "../share/TriggerCosts_1e33.xml", dest = "online_xml",    help = "rates xml"                                                                               )
    p.add_option(      '--online_lumi',    type  = "int",                                            dest = "online_lumi",            help = "rates xml lumi (the online_lumi, target_lumi, and breakpoints in the rules file must all"\
                                                                                                                                             " be in the same units, typically 1e30)"                                                  )
    p.add_option('-l', '--target_lumi',    type  = "float",         default = 1,                     dest = "target_lumi",            help = "target lumi (the online_lumi, target_lumi, and breakpoints in the rules file must all be"\
                                                                                                                                             " in the same units, typically 1e30)"                                                     )
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
    p.add_option('--online_empty_after_filled', type  = "int",                                       dest = "online_empty_after_filled", help = "# empty_after_filled bunches in rates xml"                                            )
    p.add_option('--online_unp_iso',       type  = "int",                                            dest = "online_unpaired_iso",    help = "# unpaired_iso bunches in rates xml"                                                     )
    p.add_option('--online_unp_noniso',    type  = "int",                                            dest = "online_unpaired_noniso", help = "# unpaired_noniso bunches in rates xml"                                                  )
    p.add_option('--online_abortgapnotcalib',    type  = "int",                                      dest = "online_abortgapnotcalib", help = "# bunches in abortgapnotcalib rates xml"                                                  )
    p.add_option('--online_BGRP9',           type  = "int",                                          dest = "online_bgrp9", help = "# bunches in BGRP9 rates xml"                                                  )
    p.add_option('--online_BGRP10',           type  = "int",                                          dest = "online_bgrp10", help = "# bunches in BGRP10 rates xml"                                                  )
    p.add_option('--online_BGRP11',           type  = "int",                                          dest = "online_bgrp11", help = "# bunches in BGRP11 rates xml"                                                  )
    p.add_option('--online_BGRP12',           type  = "int",                                          dest = "online_bgrp12", help = "# bunches in BGRP12 rates xml"                                                  )
    p.add_option('--target_filled',        type  = "int",         default = 700,                     dest = "target_filled",          help = "# target filled bunches"                                                                 )
    p.add_option('--target_empty',         type  = "int",         default = 2000,                    dest = "target_empty",           help = "# target empty bunches"                                                                  )
    p.add_option('--target_empty_after_filled', type  = "int",         default = 100,                dest = "target_empty_after_filled", help = "# target empty_after_filled bunches"                                                  )
    p.add_option('--target_unp_iso',       type  = "int",         default = 100,                     dest = "target_unpaired_iso",    help = "# target unpaired_iso bunches"                                                           )
    p.add_option('--target_unp_noniso',    type  = "int",         default = 10,                      dest = "target_unpaired_noniso", help = "# target unpaired_noniso bunches"                                                        )
    p.add_option('--target_abortgapnotcalib',    type  = "int",                                      dest = "target_abortgapnotcalib", help = "# bunches in abortgapnotcalib rates xml"                                                  )
    p.add_option('--target_BGRP9',           type  = "int",                                          dest = "target_bgrp9", help = "# bunches in BGRP9 rates xml"                                                  )
    p.add_option('--target_BGRP10',           type  = "int",                                          dest = "target_bgrp10", help = "# bunches in BGRP10 rates xml"                                                  )
    p.add_option('--target_BGRP11',           type  = "int",                                          dest = "target_bgrp11", help = "# bunches in BGRP11 rates xml"                                                  )
    p.add_option('--target_BGRP12',           type  = "int",                                          dest = "target_bgrp12", help = "# bunches in BGRP12 rates xml"                                                  )
    p.add_option('--use_lowest_rule',      action = "store_true", default = True,                    dest = "use_lowest_rule",        help = "optionally use the lowest available rule when none has been defined for the target lumi" )
    p.add_option('--log',                  type  = "str",         default = "",                      dest = "log",                    help = "optionally print the final state of the trigger tree to a log file"                      )
    p.add_option('--debug',                action = "store_true",                                    dest = "debug",                  help = "enable debug output"                                                                     )
    p.add_option('-q', '--quiet',          action = "store_true",                                    dest = "quiet",                  help = "disable most output"                                                                     )
    p.add_option('-v', '--verbosity',      type  = "int",         default = 2,                       dest = "verbosity",              help = "set level of detail on the output (values from 0 to 5)"                                                           )
    p.add_option( '--streamers',      type  = "str",         default = "noalg,",   dest = "list_of_streamers",      help = "List of streamers for which the efficiency is set to 1"
                 )
    p.add_option( '--ignore-errors',      action = "store_true",         default = False,   dest = "ignore_errors",      help = "Do not terminate on ERROR/FATAL"
                 )
    p.add_option( '--store-comments',     action = "store_true",         default = False,   dest = "store_comments",     help = "Store comments (in rule definitions) in ps xmls")
    
    #*#** Note2: we should make it read the xml from a release too. 
    
    (options, args) = p.parse_args()
    
    if options.quiet:
        options.verbosity = 0

    if options.debug:
        options.verbosity = 5

    logger = create_logger("RuleReader", options.verbosity)

    if len(args) > 0:
        logger.warning("Unknown arguments: %s" % args)
    
    if options.atlasHLT != None and options.rel == None:
        raise ValueError("P1 or CAF requires release number")
    elif options.rel != None and options.atlasHLT == None:
        raise ValueError("Release number requires P1 or CAF")

    if (options.quiet and options.debug):
        raise ValueError("Cannot specify both --debug and --quiet")

    online_metadata = {}
    metadata_mapping = {0: "bcr_veto",
                        1: "filled",
                        2: "calreq",
                        3: "empty",
                        4: "unpaired_iso",
                        5: "unpaired_noniso",
                        6: "empty_after_filled",
                        7: "bgrp7",
                        10: "bgrp10",

                        }

    if not options.no_online_metadata:
        import xml.etree.cElementTree as ET
        online_metadata = read_online_metadata(ET.parse(options.online_xml).getroot(),
                                               metadata_mapping
                                               )
    for key, val in online_metadata.items():
        key = "online_%s" % key
        if getattr(options, key, None) == None or options.force_online_metadata:
            setattr(options, key, val)
            print "Setting: "+str(key)+"="+str(val)

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

            options.l1_xml = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/15.6.X.Y.Z/AtlasCAFHLT/"+options.rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v2_*xml")
            options.hlt_xml = commands.getoutput("ls /afs/cern.ch/atlas/software/builds/nightlies/15.6.X.Y.Z/AtlasCAFHLT/"+options.rel+"/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v2_*xml")

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
            hltpath = p+"/"+HLTname(options.trigMenu)
            if os.path.exists(hltpath) is True:
                options.hlt_xml = hltpath
                break
            else:
                logger.info("Can not find HLT xml %s" % hltpath)
                return
 
        for p in nightlyPaths:
            l1path = p+"/"+L1name(options.trigMenu)
            if os.path.exists(l1path) is True:
                options.l1_xml = l1path
                break
            else:
                logger.info("Can not find L1 xml %s" % l1path)
                return

    ##User request to have the header w/o the logger preprended text

    os.system("cp "+options.l1_xml +" l1_xml.xml")
    os.system("cp "+options.hlt_xml+" hlt_xml.xml")    
    outputFileName = "prescales"+str(options.target_lumi)+".xml"

    #This needs to be changed to import a specific file
    #exec( "from %s import rules" % options.rulebook)
    rulebook = __import__(options.rulebook, fromlist=[""]) 
    rules = rulebook.rules
    rulelocation = rulebook.__file__
    if rulelocation[-1] == "c":
        rulelocation = rulelocation[:-1]

    config_output = "\n"
    config_output += "="*100 + "\n"
    config_output += "Run with: \n"
    for key, val in vars(options).iteritems():
        config_output += "%s%s = %s \n" % (key, " "*(30 - len(key)), val)
    config_output += "Rulebook location:             = %s\n" % rulelocation
    config_output += "="*100 + "\n"
    logger.info(config_output)    

    check_rulebook_for_duplicates(rulelocation, logger) 

    UserFunctions.modifyRules(rules, userArgs)
    configuration = {"l1_xml"                    : options.l1_xml, 
                     "hlt_xml"                   : options.hlt_xml,
                     "scale_lumi"                : options.scale_lumi,
                     "disable_bunch_scaling"     : options.disable_bunch_scaling,
                     "target_lumi"               : options.target_lumi,
                     "target_filled"             : options.target_filled,
                     "target_empty"              : options.target_empty,
                     "target_empty_after_filled" : options.target_empty_after_filled,
                     "target_unpaired_iso"       : options.target_unpaired_iso,
                     "target_unpaired_noniso"    : options.target_unpaired_noniso,
                     "target_abortgapnotcalib"   : options.target_abortgapnotcalib,
                     "target_BGRP9"              : options.target_bgrp9,
                     "target_BGRP10"             : options.target_bgrp10,
                     "target_BGRP11"             : options.target_bgrp11,
                     "target_BGRP12"             : options.target_bgrp12,
                     "online_xml"                : options.online_xml,
                     "online_lumi"               : options.online_lumi,
                     "online_filled"             : options.online_filled,
                     "online_empty"              : options.online_empty,
                     "online_empty_after_filled" : options.online_empty_after_filled,
                     "online_unpaired_iso"       : options.online_unpaired_iso,
                     "online_unpaired_noniso"    : options.online_unpaired_noniso,                     
                     "online_abortgapnotcalib"   : options.online_abortgapnotcalib,
                     "online_BGRP9"              : options.online_bgrp9,
                     "online_BGRP10"             : options.online_bgrp10,
                     "online_BGRP11"             : options.online_bgrp11,
                     "online_BGRP12"             : options.online_bgrp12,
                     "use_lowest_rule"           : options.use_lowest_rule,
                     "log"                       : options.log,
                     "verbosity"                 : options.verbosity,
                     "list_of_streamers"         : options.list_of_streamers, 
                     "ignore_errors"             : options.ignore_errors,
                     "store_comments"            : options.store_comments,
        }

    reader = RuleReader(configuration, rules, logger)
    reader.compute_ps()
    UserFunctions.modifyPrescales(reader.all_items,userArgs)
    checker = OutputChecker(l1_items = reader.l1_items,
                            all_items = reader.all_items,
                            rules = rules,
                            l1_xml = options.l1_xml,
                            hlt_xml = options.hlt_xml
                            )
    
    checker.check_all()    

    target_metadata = {"lumi" : options.target_lumi}
    for key, value in metadata_mapping.iteritems():
        if value in online_metadata:
            target_metadata[key] = getattr(options, "target_%s" % value,
                                           online_metadata[value])

    writer = RuleWriter(reader.all_items.values(),
                        target_metadata,
                        outputFileName)

    writer.write_xml()

    logger.info("="*100)
    logger.info("===> Prescales xml (%s) generated for lumi %s and lumi scale factor %.3f" %(outputFileName, options.target_lumi, options.scale_lumi))
    logger.info("="*100)
    
    return 0

if __name__ == "__main__":
    main()
