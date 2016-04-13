#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import glob
import commands
import os
import subprocess
import string
import locale
import fnmatch

myfile=""
for file in os.listdir('.'):
    if fnmatch.fnmatch(file, 'Valgrind_*_log_1'):
        f=open(file,"r")
        myfile=file
#        exit()


if not os.path.isfile(myfile):
    print "Did not find a log file matching to Valgrind_*_log_1"
    quit() 

# Find top 20 instances of memory leaks as reported by valgrind

#f = open("/atlas/scratch0/limosani/valgrind_memcheck_devval_Jan11_esdtoaod/valgrind.out","r")
#f = open("Valgrind_*_2","r")
#f = open("/afs/cern.ch/user/l/limosani/atpc2/trigger_memory_tests/Valgrind_MC_pp_v5_log")

searchlines = f.readlines()
f.close()

newfile = open('compact_valgrind_log', 'w+')

for i,line in enumerate(searchlines):
   if "definitely lost" in line:
       #decompose line to check the size of the memory leak
       words = searchlines[i].split()
       #convert memory leak size to a number
       locale.setlocale( locale.LC_ALL, 'en_US.UTF-8' )
       memleakSize = locale.atoi(words[1])
       if memleakSize > 500 :
           print >> newfile
           print >> newfile, "-------------------------------------------------------------------------------------------------------------"
           print >> newfile, "//// Memory leak ", memleakSize
#            if "execute" in searchlines[i:i+30]:
           for l in searchlines[i:i+30]:
               print >> newfile, l,
           print >> newfile
           print >> newfile
