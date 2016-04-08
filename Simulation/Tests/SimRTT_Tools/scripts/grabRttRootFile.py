#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys

__author__ = 'FFF'
__doc__    = 'FFF'


## grabRttRootFile.py devval x86_64-slc5-gcc43-opt SimCoreTests TTbarSim [list of root files to copy]

dryrun=False


def checkStatus(path,days=1):
    if not os.path.exists(path+"/jobExitCode.txt"):
        print path+"/jobExitCode.txt","does not exist"
        return False,0
    f = open(path+"/jobExitCode.txt")
    print "Checking job status..."
    print path+"/jobExitCode.txt"
    file_status=False
    for line in f:
        if line.strip()=='0':
            file_status=True
            print line,
        if "<jobSuccess>True</jobSuccess>" in line:
            file_status=True
            print line,
        elif "<jobSuccess>False</jobSuccess>" in line:
            file_status=False
            print line,

    #check date
    #<timeStamp>1352062929</timeStamp>
    if os.path.exists(path+"/rttjobinfo.xml"):
        import re
        f = open(path+"/rttjobinfo.xml")
        for l in f:
            #<timeStamp>1352062929</timeStamp>#
            m=re.search('<timeStamp>([0-9]*)</timeStamp>',l)
            if m is not None:
                print m.groups()
                ts=int(m.groups()[0])
                import datetime as dt
                yes=dt.date.fromtimestamp(ts)
                now=dt.date.today()
                days=(now-yes).days
                if days>6:
                    # to old, ignore
                    print 'WARNING found too old results in ',path
                    file_status=False
    return file_status,days

## main
if __name__ == "__main__":

    pwd=os.getcwd()
    if os.environ.has_key('LS_SUBCWD'):
        pwd= os.environ['LS_SUBCWD']

    print pwd
    argv=pwd.split('/')
    pos=4 # for debugging, but really pos=9

    for i,a in enumerate(argv):
        if a.find("rel_")>=0 :
            print "found at ",i
            pos=i
    if argv[pos] == 'rel_0':
        prevrelease = 'rel_6'
    else:
        prevrelease = 'rel_%d' % (int(argv[pos].split('_')[1])-1)
    pass

    argv[pos]=prevrelease   #nightly release (rel_1 etc.)
    argv[pos+1]=sys.argv[1] #nightly build (dev,devval etc.)
    argv[pos+3]=sys.argv[2] #platform (x86_64-slc5-gcc43-opt)
    argv.pop()
    argv.pop()
    argv.append(sys.argv[3]) #package
    argv.append(sys.argv[4]) #job
    valPath='/'.join(argv)
    print "Directory for input file:"
    print valPath

    # look for suffix
    file_name='.root'

    # make sure other root files are not confused with target file.
    compfiles=['test.HITS.pool.root']
    if len(sys.argv)>5:
        # got extended argument list create file list
        for f in sys.argv[5:]:
            compfiles.append(f)
    print 'compfiles:',compfiles
    try:
       
        file_status_val=False
        for i in range(1,5):
            file_status_val,days_val=checkStatus(valPath)

            if file_status_val==True:
                print "Success"
                print("Looking for .root files to compare...")
                file_root=False
                for f in os.listdir(valPath):
                    if f.endswith(file_name) and f != 'yesterday.root' and f!= 'today.root':
                        #print f
                        name,suffix=f.split('.',1)
                        #if name in compfiles:
                        if f in compfiles:
                            suffix=f
                            print 'will copy '+f
                        else:
                            print 'rejected '+f
                            continue
                        cmd = 'cp'+' '+valPath+'/'+f+' '+sys.argv[1]+'.'+suffix
                        print cmd
                        if not dryrun:
                            os.system(cmd)
                        file_root=True

                if file_root==False:
                    print("No .root file found in validation path!")

                # found a release!
                break
                #elif(i==2):
                #print("Release to compare has failed. No validation release was found!")

            elif file_status_val==False:
                if argv[pos] == 'rel_0':
                    prevrelease = 'rel_6'
                else:
                    prevrelease = 'rel_%d' % (int(argv[pos].split('_')[1])-1)

                argv[pos]=prevrelease
                valPath='/'.join(argv)
                print "The previous nightly has failed. New nightly to check:"
                print valPath

        if not file_status_val==True:
            print("Search for input file has failed. No valid nightly run was found!")


        # make fileGrepper happy
        print "leaving with code 0"

    except (OSError,IOError):
        print "No running in this release!"
