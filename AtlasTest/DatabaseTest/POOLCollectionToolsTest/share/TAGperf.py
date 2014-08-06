#!/usr/bin/env python

import sys, os, subprocess, commands

if (len(sys.argv) < 5):
  print "Too few arguments"
  print "Usage: perf.py file ref tol name"
else:
  cmd = "grep 'ChronoStatSvc        INFO Time User' "+sys.argv[1]

  (retc_cmd, o) = commands.getstatusoutput(cmd)

  pieces = o.split()

  try:
    f=open('stat.perf','a')
  except:
    f=open('stat.perf','w')

  if (len(pieces)>6):
    val = float(pieces[6])
    ref = float(sys.argv[2])
    tol = float(sys.argv[3])
    name= sys.argv[4]
    f.write(name+"\n"+pieces[6]+"\n")
    if (abs(val-ref)>tol) :
      print name,"ERROR",val,"outside tolerance (",tol,")"
    else:
      print name,"SUCCESS",val,"within tolerance(",tol,")"
  else:
    print "ERROR timing not available"
    print o
  f.close()
