#!/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys

# get the list of special run,lbn with more than 1 file
special_run_lbn = {}
with open("runs_lbn_files.txt") as f:
    for line in f:
        # print line
        if not line.startswith("root"):
            print("WARNING: line doesn't start with 'root'")
            print(line)
            continue
        fileName = line.rstrip()
        print(fileName)
        finfo = fileName.split(".")
        run = finfo[3]
        lbi = 4  # index of the lbn info
        while "_lb" not in finfo[lbi] and lbi < 20:
            lbi += 1
        if lbi >= 20:
            print(finfo[lbi] + " doesnt contain _lb")
            exit(1)
        lbn = finfo[lbi].replace("_lb", "")
        print(run+"_"+lbn)
        if run+"_"+lbn in special_run_lbn:
            if fileName in special_run_lbn[run+"_"+lbn]:
                print("%s already in map for %s %s" % (fileName, run, lbn))
            else:
                special_run_lbn[run+"_"+lbn].append(fileName)
        else:
            special_run_lbn[run+"_"+lbn] = [fileName]
    for key in special_run_lbn.keys():
        pass
    print("done with special_run_lbn")

# do the work
maxstream = int(sys.argv[1])
print("maxstream=%d" % maxstream)
for s in range(0, maxstream+1):  # 50
    with open("lbn_anal_map_stream"+str(s)+".txt") as f:
        tot = 0
        nfile = 1
        of = open("output_stream"+str(s) +
                  "/lbn_anal_map_"+str(nfile)+".txt", "w")
        of2 = open("output_stream"+str(s)+"/filelist_"+str(nfile)+".txt", "w")
        for line in f:
            linfo = line.split(" ")
            tot += int(linfo[16])
            run = linfo[3].replace(",", "").zfill(8)
            lbn = linfo[5].replace(",", "").zfill(4)
            if tot > 100:
                # replace last bit of line and also write it to the new file
                extra = tot-100
                forthisone = int(linfo[16])-extra
                rline = line.replace(
                    linfo[16]+" wanted", str(forthisone)+" wanted")
                of.write(rline)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        if of2.tell() > 0:
                            of2.write(",")
                        of2.write(f)
                else:
                    print("%s %s not in files map!" % (run, lbn))

                of.close()
                of2.close()
                nfile += 1
                of = open("output_stream"+str(s) +
                          "/lbn_anal_map_"+str(nfile)+".txt", "w")
                of2 = open("output_stream"+str(s) +
                           "/filelist_"+str(nfile)+".txt", "w")
                tot = extra  # the number left over from the last set

                rline = line.replace(linfo[16]+" wanted", str(extra)+" wanted")
                of.write(rline)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        if of2.tell() > 0:
                            of2.write(",")
                        of2.write(f)
                else:
                    print("%s %s not in files map!" % (run, lbn))

            elif tot == 100:
                of.write(line)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        if of2.tell() > 0:
                            of2.write(",")
                        of2.write(f)
                else:
                    print("%s %s not in files map!" % (run, lbn))

                of.close()
                of2.close()
                nfile += 1
                of = open("output_stream"+str(s) +
                          "/lbn_anal_map_"+str(nfile)+".txt", "w")
                of2 = open("output_stream"+str(s) +
                           "/filelist_"+str(nfile)+".txt", "w")
                tot = 0
            else:
                of.write(line)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        if of2.tell() > 0:
                            of2.write(",")
                        of2.write(f)
                else:
                    print("%s %s not in files map!" % (run, lbn))

        print("done %d" % s)
        of.close()
        of2.close()
