#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
import time,datetime

__author__ = 'FFF'
__doc__    = 'FFF'


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
#                    file_status=False
    return file_status,days

###get the recent tag###
def get_recent(path):
    print "####### Get the most recent successful job as reference ######"
    pathlist=path.split('/')
    pos_index=0
    release_list=[]
    for i,a in enumerate(argv):
        if a.find("rel_")>=0:
            pos_index=i
            release_list=a.split('_')

    recent_index=0;
    recent_days=0;
    for i in xrange(7): 
        release_tag=release_list[0]+"_"+str(i)
#        print release_tag
        pathlist[pos_index]=release_tag
        path_refill="/".join(pathlist)
#        print path_refill
        status_tmp,days_tmp=checkStatus(path_refill)
        print "===>Release: "+release_tag+"  Days: "+str(days_tmp)
        if i==0:
            recent_days=days_tmp
            recent_index=i
        elif status_tmp==True and days_tmp!=0 and days_tmp<recent_days:
            recent_days=days_tmp
            recent_index=i

    recent_release="rel_"+str(recent_index)
    print "=====> Most recent release: "+recent_release
    return recent_release








###Add EOS prefix###
def eoscp(filename,jobname,tag,store_tag):
    rel_list=["0","1","2","3","4","5","6"]
    current=os.getcwd()
    if os.environ.has_key('LS_SUBCWD'):
        current=os.environ['LS_SUBCWD']
    argv=current.split('/')
    argv.pop()
    for arg in argv:
        if arg.find("rel_")==0:
            arg_list=arg.split("_")
            print arg_list
            if tag=="today":
                release=arg
                print release
            elif tag=="yesterday":
                release=arg_list[0]+"_"+rel_list[int(arg_list[1])-1]
                print release
    postfix='/'.join(argv[-3:])
    #middle='/'.join(argv[-6:-5])
    middle=argv[-5]
    pathlist=["prod","batch"]#This 2nd option should make the script work on the RTT test instance.
#    status=0
    for path in pathlist:
        prefix="root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/"+path+"/rtt"
        print "postfix: "+postfix
        print "middle: "+middle
        print "prefix: "+prefix
        print "jobname: "+jobname
        print "filename: "+filename
        path=prefix+'/'+release+'/'+middle+'/'+postfix+'/'+jobname+'/'+filename
        print path
        if store_tag!='':
            command_cp="xrdcp %s %s.%s.root"%(path,tag,store_tag)
        else:
            command_cp="xrdcp %s %s.%s"%(path,tag,filename)
        print command_cp
        status=os.system(command_cp)
        if status==0: return status
    return status

## main
if __name__ == "__main__":
    pwd=os.getcwd()
    if os.environ.has_key('LS_SUBCWD'):
        pwd= os.environ['LS_SUBCWD']

    print pwd
    argv=pwd.split('/')
    argv.pop()
    argv.append(sys.argv[1])
    corr_path='/'.join(argv)
    recent_rel=get_recent(corr_path)

    pos=9

    for i,a in enumerate(argv):
        if a.find("rel_")>=0 :
            print "found at ",i
            pos=i
##    if argv[pos] == 'rel_0':
##        prevrelease = 'rel_6'
##    else:
##        prevrelease = 'rel_%d' % (int(argv[pos].split('_')[1])-1)
##    pass

    refPath='/'.join(argv)
#    argv[pos]=prevrelease
    argv[pos]=recent_rel

    valPath='/'.join(argv)

    print "Releases to compare:"
    print refPath
    print valPath

    # look for suffix
    file_name='.root'
    store_tag=''

    # make sure ISFBroker.root is not confused with truth*.root
    compfiles=['ISFBroker.root']
    if len(sys.argv)>2:
        # got extended argument list create file list
        for f in sys.argv[2:]:
            if f.endswith(file_name):
                print "Add file %s in searching list."%(f)
                compfiles.append(f)
            else:
                print "Find tag set, use common tag: %s."%(f)
                store_tag=f

    print 'compfiles:',compfiles
    file_list_ref=compfiles[:]
    print file_list_ref
    file_list_val=compfiles[:]
    try:
       
        file_status_ref,days=checkStatus(refPath)
                
        if file_status_ref==True:
            print "Success"
            file_root=False
            for f in  os.listdir(refPath):
                if f.endswith(file_name) and f != 'yesterday.root' and f!= 'today.root':
                    print f
                    name,suffix=f.split('.',1)
                    if f in compfiles:
                        suffix=f
                        print 'f1'
                    elif len(compfiles)>=2:
                        continue
                    if store_tag!='':
                        cmd = 'cp'+' '+refPath+'/'+f+' '+'today.'+store_tag+".root"
                    else:
                        cmd = 'cp'+' '+refPath+'/'+f+' '+'today.'+suffix
                    print cmd
                    if not dryrun:
                        os.system(cmd)
                    file_root=True
                    if f in file_list_ref:
                        file_list_ref.remove(f)
                        print file_list_ref

#            if file_root==False:
            file_list_ref.remove('ISFBroker.root')
            if len(file_list_ref)!=0:
                print("Several .root file can not been found in reference path!, try to copy from EOS.")
                for ref_f in file_list_ref:
                    if eoscp(ref_f,sys.argv[1],"today",store_tag)!=0 :
                        print "File does not exist in EOS."


            file_status_val=False
            for i in range(1,5):
                file_status_val,days_val=checkStatus(valPath)
                        
                if file_status_val==True:
                    print "Success"
                    print("Looking for .root files to compare...")
                    file_root=False
                    for f in os.listdir(valPath):
                        if f.endswith(file_name) and f != 'yesterday.root' and f!= 'today.root':
                            print f
                            name,suffix=f.split('.',1)
                            #if name in compfiles:
                            if f in compfiles:
                                suffix=f
                                print 'f2'
                            elif len(compfiles)>=2:
                                continue
                            if store_tag!='':
                                cmd = 'cp'+' '+valPath+'/'+f+' '+'yesterday.'+store_tag+'.root'
                            else:
                                cmd = 'cp'+' '+valPath+'/'+f+' '+'yesterday.'+suffix
                            print cmd
                            if not dryrun:
                                os.system(cmd)
                            file_root=True
                            if f in file_list_val:
                                file_list_val.remove(f)
                                print file_list_val

#                    if file_root==False:
                    file_list_val.remove('ISFBroker.root')
                    if len(file_list_val)!=0:
                        print("No .root file found in validation path!, try to copy from EOS")
                        for val_f in file_list_val :
                            if eoscp(val_f,sys.argv[1],"yesterday",store_tag)!=0:
                                print "File does not exist in EOS."

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
                    print "The previous release has failed. New releases to compare:"
                    print refPath
                    print valPath

            if not file_status_val==True:
                print("Release to compare has failed. No validation release was found!")


            # make fileGrepper happy
            print "leaving with code 0"

        elif file_status_ref==False:
            print "Job has failed in validation path"

    except (OSError,IOError):
        print "No running in this release!"
