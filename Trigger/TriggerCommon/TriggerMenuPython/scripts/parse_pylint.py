#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
""" Summarize pylint findings """

import sys
import re

def prettyString(text, width):
  return text + (" "*(width - len(text)))

if len(sys.argv) < 2:
  print "Usage:"
  print "%s log_file [detail_level]" % sys.argv[0]
  print "   log_file -- The pylint output"
  print "   detail_level -- 0 print just the number of errors by category, 1 prints the errors and their line location too (default = 0)"
  sys.exit(1)

fname = sys.argv[1]
detail = 0
if len(sys.argv) > 2:
  detail = int(sys.argv[2])

check_searches = {"Errors"  : "E[0-9]+:(.*)",
                  "Unused Imports" : "W[0-9]+:(.*Unused import (\w+))$",
                  "Duplicate Keys" : "^W[0-9]+:(.*Duplicate key '(.*))' .*",
                  "Untyped Exceptions" : "^W[0-9]+:(.*No exception type.*)",
                  }

check_results = {"Errors"  : {"" : []},
                 "Unused Imports" : {"" : []},
                 "Duplicate Keys" : {"" : []},
                 "Untyped Exceptions" : {"" : []},
                 }

module_name = ""
for line in open(sys.argv[1]).readlines():
  line = line.strip('\n')

  m = re.search("\*+ Module (.*)", line)
  if m:
    check_results["Unused Imports"][module_name] = set(check_results["Unused Imports"][module_name])
    module_name = m.group(1)
    
    for check in check_results.itervalues():
      check[module_name] = []
    continue

  for check_type in check_searches.keys():
    m = re.search(check_searches[check_type], line)
    if m:
      check_results[check_type][module_name].append(m.group(1))
  
for check in check_results.keys():
  print
  print "-"*50
  print "** %s **" % check
  print 
  for name, value in sorted(check_results[check].items(), key=lambda x: len(x[1]), reverse=True):
    if len(value) > 0:
      print prettyString(name, 30), len(value)
      
      if detail < 1:
        continue
      
      for val in value:
        print "  ", val
      print


  
