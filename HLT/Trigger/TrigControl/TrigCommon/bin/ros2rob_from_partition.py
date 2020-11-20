#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import eformat
from pm.project import Project
from argparse import ArgumentParser
from types import MethodType
from sys import stdout, stderr

# get the arg parser
def argparser():
  parser = ArgumentParser(description='Produce a ros2rob map, as a python '
                                      'dictionary, from a given partition.')
  parser.add_argument('--database_file', '-d', required=True, 
                      help='A partition filename (e.g. ATLAS.data.xml).')
  parser.add_argument('--partition', '-p', required=True,
                      help='A partition filename. The name of the partition '
                           'that is read from pfile (e.g. ATLAS).')
  parser.add_argument('--output_file', '-o',
                      help='The output filename. The name of the file to which '
                           'the ros2rob map is written. If omitted, stdout is '
                           'used (e.g. myros2rob.py).')
  make_parser_print_help_on_error(parser)
  return parser

def make_parser_print_help_on_error(parser):
  """
  Alter an ArgumentParser so that it shows a help msg whenever there is an 
  error in the command line
  """
  def error(self, msg):
    print('error: %s\n' % msg, file=stderr)
    self.print_help()
    exit(2)
  parser.error = MethodType(error, parser)

def get_roses(pfname, pname):
  """
  Get all the ROSes in the partition
  """
  return Project(pfname).getObject('Partition', pname).get('ROS')

def get_ros2rob(roses):
  """
  Get the ros2rob map from the ROS list
  """
  ros2rob = {}
  for ros in roses:
    if ros.id in ros2rob:
      print("WARNING: %s is repeated in the partition: ignoring "
            "second occurrence", file=stderr)
    else:
      ros2rob[ros.id] = get_robs(ros)
  return ros2rob


def get_robs(ros):
  """
  Get the list of ROBs that correspond to a ROS
  """
  return [eformat.helper.SourceIdentifier(rol.Id).code() 
          for robin in ros.Contains for rol in robin.Contains]

def print_ros2rob(ros2rob, out):
  """
  Print the ros2rob map as an easily readable/editable python dictionary
  """
  print("ros2rob = {", file=out)
  count = 0
  for k, v in ros2rob.items():
    count += 1
    print("\t'%s': \n\t[" % k, file=out)
    for i in range(len(v)):
      print("\t\t%s" % hex(v[i]), end=' ', file=out) 
      if i+1 != len(v):
        print(",", file=out)
      else:
        print("\n\t]", end=' ', file=out)
    if count != len(ros2rob):
      print(",", file=out)
  print("\n}", file=out)

# main
if __name__ == '__main__':
  args = argparser().parse_args()
  out = open(args.output_file, 'w') if args.output_file else stdout
  print("# Extracting ROS2ROB map", file=stderr)
  print("# ROS2ROB map extracted from %s:" % args.database_file, file=out)
  print_ros2rob(get_ros2rob(get_roses(args.database_file, args.partition)), out)
