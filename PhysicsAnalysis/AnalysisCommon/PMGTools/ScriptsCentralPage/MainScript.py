#! /usr/bin/python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# This is the script that steers the PMG Central Page creation/updates
import os
import sys
from optparse import OptionParser
import re
import logging
sys.path.append('ScriptsCentralPage/MTscripts/')
import CentralPage

# Init the logger for the script and various modules
# INFO is not too noisy and helps to keep track of what is going on
logging.basicConfig(format='%(asctime)s %(levelname)s %(message)s', datefmt='%H:%M:%S')
log = logging.getLogger("myCentralPageLogger")
log.setLevel(logging.INFO)

# Init the Parser for the command line options
# The default settings will run in the current folder and produce the mc16 page
# DSID set refers to the first n digits after 'mcXX_YYTeV.'
parser = OptionParser()
parser.add_option("-s", "--set", dest='set', default='', help="DSID set")
parser.add_option("-p", "--production", dest='production', default='mc16_13TeV', help="MC production")
parser.add_option("-o", "--output", dest='out', default='/afs/cern.ch/user/m/mcgensvc/www/mc15twiki', help="Output twiki folder")
parser.add_option("-m", "--mainFolder", dest='mainFolder', default=os.getcwd(), help="Main folder")
parser.add_option("-d", "--debug", dest='debug', action='store_true', default=False, help="Enable debug mode")
parser.add_option("--skipMeta", action='store_true', default=False, help="skip Metadata retrieval")
parser.add_option("--skipStatus", action='store_true', default=False, help="skip StatusFile creation")
parser.add_option("--skipTwiki", action='store_true', default=False, help="skip Twiki file creation")
parser.add_option("--skipCopy", action='store_true', default=False, help="skip Twiki copy")
(options, args) = parser.parse_args()

if options.debug:
    log.setLevel(logging.DEBUG)

# Compute here the target folder, depending on mc prod and command line options
stringTargetMCprod = re.split("[a-z,_]+", options.production)
targetFolder = options.out+"/CentralMC"+stringTargetMCprod[1]+"ProductionPage/"

# Printout for the user
log.info('====> Will look for %s.%s*', str(options.production), str(options.set))
log.info('====> Twikis will be copied to %s', str(targetFolder))

# Create the CentralPageMaker class, where everything takes place
# This allows to keep only the configuration in this script
# and hide performance changes.
# In the future pass an extra flag to run only subsets of the full chain
myCentralPage = CentralPage.CentralPageMaker(options.production, options.set, options.mainFolder, options.skipMeta, options.skipStatus, options.skipTwiki)
myCentralPage.run()

# All the core business is done and saved in the mainFolder
# Now copy to the public afs location to update the www pages
if not options.skipCopy:
    log.info('==> Copying MainTwiki files')
    os.system("cp "+options.mainFolder+"/AllSampleInfo_"+str(options.production)+"/twikiPages/MainTwiki_* "+targetFolder)

# Yay!
log.info('====> All done!')
