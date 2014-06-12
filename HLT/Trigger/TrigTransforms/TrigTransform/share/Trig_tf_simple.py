#!/usr/bin/env python

from optparse import OptionParser
import subprocess
import shlex

parser = OptionParser()

parser.add_option("--inputBSFile",
                  dest    = "inputfile",
                  metavar = "FILE",
                  help    = "Set the input file name")

parser.add_option("--outputRAWFile",
                  dest    = "outputfile",
                  metavar = "FILE",
                  help    = "Set the output file name")

parser.add_option("--connection",
                  dest    = "connection",
                  metavar = "STR",
                  help    = "Set the connection to the configuration database")

parser.add_option("--smk",
                  dest    = "smk",
                  metavar = "SMK",
                  help    = "Set the super master key for the database configuration")

parser.add_option("--lvl1ps",
                  dest    = "lvl1ps",
                  metavar = "L1PS",
                  help    = "Set the prescale key for the LVL1")

parser.add_option("--hltps",
                  dest    = "hltps",
                  metavar = "HLTPS",
                  help    = "Set the prescale key for the HLT")

(options, args) = parser.parse_args()

com = "athenaMT.py"
#com=""
com += " -f %s" % options.inputfile
com += " -o %s" % options.outputfile
com += " -J TrigConf::HLTJobOptionsSvc"
database = "DBServer=%s:DBSMKey=%s:DBHLTPSKey=%s:DBLV1PSKey=%s" % (options.connection, options.smk, options.hltps, options.lvl1ps)
com += " -b \"%s\"" % database

args = shlex.split(com)

temp = subprocess.Popen(["which", "athenaMT.py"],stdout=subprocess.PIPE )
path = temp.communicate()[0].strip()

temp = ["python", path]
temp.extend(args[1:])

print com
print args
print temp

#this works not
process = subprocess.Popen(args)

#this works
process = subprocess.Popen(temp)
process.wait()
print process.returncode
