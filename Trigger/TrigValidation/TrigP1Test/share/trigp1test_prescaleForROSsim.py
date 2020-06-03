#!/usr/bin/env python
#Script to run athenaHLT from DB with physics prescales and ROS simulation

from subprocess import Popen, PIPE, STDOUT, check_call

def main():

  from optparse import OptionParser
  parser = OptionParser()
  parser.add_option("--file", help="Input file", default='')
  parser.add_option("--nevents", help="Number of max events",
        type='int', default=-1)
  parser.add_option("--ros2rob", help="ros2rob map", default='')
  parser.add_option("--keys", help="file to export keys", default='')
  parser.add_option("--db", help="database", default="TRIGGERDBART")
  (opts, args) = parser.parse_args()

  file_handle = open(opts.keys, 'r')
  lines_list = file_handle.readlines()
  smk = -1
  l1ps = -1
  hltps = -1
  for line in lines_list:
     if "smk=" in line:
        smk = int(line.split("=")[1])
     elif "l1psk=" in line:
        l1ps = int(line.split("=")[1])
     elif "hltpsk=" in line:
        hltps = int(line.split("=")[1])

  if smk == -1 or l1ps == -1 or hltps == -1:
     print "keys not found"
     exit()

  trigCmd = "unset FRONTIER_SERVER; athenaHLT.py -n " + str(opts.nevents) + " -f " + str(opts.file) + " -J TrigConf::HLTJobOptionsSvc -L1 -m \"" + str(opts.ros2rob) + "\" --use-database --db-type \"Coral\" --db-server \"" +  str(opts.db) + "\" --db-smkey " + str(smk) + " --db-hltpskey " + str(hltps) + " --db-extra \"{\'lvl1key\': " + str(l1ps) + "}\""

  trigCmdEsc = trigCmd.replace("\\","\\\\").replace("\"","\\\"")#For output to echo
  check_call("echo \"" + trigCmdEsc + "\"", shell=True)#Call echo rather than print so that it completes first
  check_call(trigCmd, shell=True)

if __name__ == "__main__":
  main()

