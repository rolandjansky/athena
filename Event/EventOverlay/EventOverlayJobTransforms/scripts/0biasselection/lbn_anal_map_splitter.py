# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys

#get the list of special run,lbn with more than 1 file
special_run_lbn = {}
with open("runs_lbn_files.txt") as f:
    for line in f:
        #print line
        if not line.startswith("root"):
            print "WARNING: line doesn't start with 'root'"
            print line
            continue
        filenm=line.rstrip()
        print filenm
        finfo=filenm.split(".")
        run=finfo[3]
        lbn=finfo[7].replace("_lb","")
        print run+"_"+lbn
        if run+"_"+lbn in special_run_lbn:
            if filenm in special_run_lbn[run+"_"+lbn]:
                print filenm, "already in map for ",run,lbn
            else:
                special_run_lbn[run+"_"+lbn].append(filenm)
        else:
            special_run_lbn[run+"_"+lbn]=[filenm]
    for key in special_run_lbn.keys():
        #if len(special_run_lbn[key])>1: print special_run_lbn[key]
        pass
    print "done with special_run_lbn"

#do the work
maxstream = int(sys.argv[1])
print "maxstream=",maxstream
for s in range(0,maxstream+1): #50
    with open("lbn_anal_map_stream"+str(s)+".txt") as f:
        tot=0
        nfile=1
        of=open("output_stream"+str(s)+"/lbn_anal_map_"+str(nfile)+".txt","w")
        of2=open("output_stream"+str(s)+"/filelist_"+str(nfile)+".txt","w")
        for line in f:
            linfo=line.split(" ")
            #print line
            #print linfo[16]
            tot+=int(linfo[16])
            run=linfo[3].replace(",","").zfill(8)
            lbn=linfo[5].replace(",","").zfill(4)
            if tot>100:
                #replace last bit of line and also write it to the new file
                extra=tot-100
                forthisone=int(linfo[16])-extra
                rline=line.replace(linfo[16]+" wanted",str(forthisone)+" wanted")
                of.write(rline)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        #print of2.tell()
                        if of2.tell()>0: of2.write(",")
                        of2.write(f)
                else:
                    print run,lbn," not in files map!"
                #print line, rline
                #print run, lbn

                of.close(); of2.close(); nfile+=1
                of=open("output_stream"+str(s)+"/lbn_anal_map_"+str(nfile)+".txt","w")
                of2=open("output_stream"+str(s)+"/filelist_"+str(nfile)+".txt","w")
                tot=extra #the number left over from the last set
            
                rline=line.replace(linfo[16]+" wanted",str(extra)+" wanted")
                of.write(rline)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        if of2.tell()>0: of2.write(",")
                        of2.write(f)
                else:
                    print run,lbn," not in files map!"

            elif tot==100:
                of.write(line)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        if of2.tell()>0: of2.write(",")
                        of2.write(f)
                else:
                    print run,lbn," not in files map!"            

                of.close(); of2.close(); nfile+=1
                of=open("output_stream"+str(s)+"/lbn_anal_map_"+str(nfile)+".txt","w")
                of2=open("output_stream"+str(s)+"/filelist_"+str(nfile)+".txt","w")
                tot=0
            else:
                of.write(line)
                if run+"_"+lbn in special_run_lbn:
                    for f in special_run_lbn[run+"_"+lbn]:
                        #print of2.tell()
                        if of2.tell()>0: of2.write(",")
                        of2.write(f)
                else:
                    print run,lbn," not in files map!"

        print "done ",s
        of.close(); of2.close()

