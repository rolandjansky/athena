#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
import time,datetime

__author__ = 'FFF'
__doc__    = 'FFF'


dryrun=False


def checkStatus(path,days=1):
    if not os.path.exists(path+"/jobExitCode.txt"):
        print path+"/jobExitCode.txt","missing - skipping this nightly"
        return False,999
    f = open(path+"/jobExitCode.txt")
    print "Checking job status..."
    print "Found "+path+"/jobExitCode.txt"
    file_status=False
    for line in f:
        if line.strip()=='0':
            file_status=True
            #print line,
        if "<jobSuccess>True</jobSuccess>" in line:
            file_status=True
            #print line,
        elif "<jobSuccess>False</jobSuccess>" in line:
            file_status=False
            #print line,
    if file_status: print "Job succeeded in this nightly."
    else:
        print "Job failed in this nightly."
        return False,999
    #check date
    #<timeStamp>1352062929</timeStamp>
    if os.path.exists(path+"/rttjobinfo.xml"):
        import re
        f = open(path+"/rttjobinfo.xml")
        for l in f:
            #<timeStamp>1352062929</timeStamp>#
            m=re.search('<timeStamp>([0-9]*)</timeStamp>',l)
            if m is not None:
                #print m.groups()
                ts=int(m.groups()[0])
                import datetime as dt
                yes=dt.date.fromtimestamp(ts)
                now=dt.date.today()
                days=(now-yes).days
                if days>6:
                    # to old, ignore
                    print 'WARNING results found in '+path+' are more than a week old'
#                    file_status=False
    return file_status,days

###get the recent tag###
def get_recent(path):
    print "####### Get the most recent successful job as reference ######"
    print "---------------------------------------------------------------------------------------"
    pathlist=path.split('/')
    pos_index=0
    release_list=[]
    for i,a in enumerate(argv):
        if a.find("rel_")>=0:
            pos_index=i
            release_list=a.split('_')
    todays_release=pathlist[pos_index]
    recent_days=999;
    recent_index=0;
    for i in xrange(7):
        release_tag=release_list[0]+"_"+str(i)
#        print release_tag
        pathlist[pos_index]=release_tag
        path_refill="/".join(pathlist)
#        print path_refill
        print "Checking for results for "+release_tag+"..."
        status_tmp,days_tmp=checkStatus(path_refill)
        if status_tmp:
            print "==> Nightly: "+release_tag+"  Age: "+str(days_tmp)+" days"
            if days_tmp>0 and days_tmp<recent_days:
                recent_days=days_tmp
                recent_index=i
        else:
            print "==> Nightly: "+release_tag+"  Age: N/A"
        print "---------------------------------------------------------------------------------------"
    if recent_days>998:
        print "=====> Could not find another release to compare to. Returning today's nightly ("+todays_release+") as the reference."
        return todays_release
    recent_release="rel_"+str(recent_index)
    print "=====> "+recent_release+" is the most recent nightly to use as reference."
    return recent_release

###Add EOS prefix###
def eoscp(filename,jobname,tag,store_tag,release):
    ##rel_list=["0","1","2","3","4","5","6"]
    current=os.getcwd()
    if os.environ.has_key('LS_SUBCWD'):
        current=os.environ['LS_SUBCWD']
    #current='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/19.2.X.Y-VAL/build/x86_64-slc6-gcc47-opt/offline/ISF_Validation/ttbar_ISF_G4_sim_Reg'
    argv=current.split('/')
    argv.pop()
    ##for arg in argv:
    ##    if arg.find("rel_")==0:
    ##        arg_list=arg.split("_")
    ##        #print arg_list
    ##        if tag=="today":
    ##            release=arg
    ##            print release
    ##        elif tag=="yesterday":
    ##            release=arg_list[0]+"_"+rel_list[int(arg_list[1])-1]
    ##            print release
    postfix='/'.join(argv[-3:])
    #middle='/'.join(argv[-6:-5])
    middle=argv[-5]
    pathlist=["prod","batch"]#This 2nd option should make the script work on the RTT test instance.
#    status=0
    for path in pathlist:
        prefix="root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/rtt/"+path+"/rtt"
        ##print "postfix: "+postfix
        ##print "middle: "+middle
        ##print "prefix: "+prefix
        ##print "jobname: "+jobname
        ##print "filename: "+filename
        path=prefix+'/'+release+'/'+middle+'/'+postfix+'/'+jobname+'/'+filename
        print "Path to check on eos: "+path
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
    #pwd='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/19.2.X.Y-VAL/build/x86_64-slc6-gcc47-opt/offline/ISF_Validation/ttbar_ISF_G4_sim_Reg'
    print pwd
    argv=pwd.split('/')
    argv.pop()
    argv.append(sys.argv[1])
    corr_path='/'.join(argv)
    ## Find a release to use as a reference
    recent_rel=get_recent(corr_path)
    this_rel="rel_X"
    ## Construct the reference directory path
    pos=9
    for i,a in enumerate(argv):
        if a.find("rel_")>=0 :
            #print "found at ",i
            this_rel = a
            pos = i

    valPath='/'.join(argv)
    argv[pos]=recent_rel

    refPath='/'.join(argv)

    print "Releases to compare: "+recent_rel+", "+this_rel
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
        print "Confirming today's job status..."
        file_status_val,days=checkStatus(valPath)

        if file_status_val==True:
            #print "Success"
            file_root=False
            for f in  os.listdir(valPath):
                if f.endswith(file_name) and f != 'yesterday.root' and f!= 'today.root':
                    print f
                    name,suffix=f.split('.',1)
                    if f in compfiles:
                        suffix=f
                        print 'f1'
                    elif len(compfiles)>=2:
                        continue
                    if store_tag!='':
                        cmd = 'cp'+' '+valPath+'/'+f+' '+'today.'+store_tag+".root"
                    else:
                        cmd = 'cp'+' '+valPath+'/'+f+' '+'today.'+suffix
                    print cmd
                    if not dryrun:
                        os.system(cmd)
                    file_root=True
                    if f in file_list_val:
                        file_list_val.remove(f)
                        print file_list_val
            if 'ISFBroker.root' in file_list_val:
                file_list_val.remove('ISFBroker.root')
            if len(file_list_val)!=0:
                print "The following files were not found in the output directory of today's job: "+str(file_list_val)+", will try to copy them from EOS instead."
                for val_f in file_list_val:
                    if eoscp(val_f,sys.argv[1],"today",store_tag,this_rel)!=0 :
                        print "File does not exist in EOS."
                    else:
                        file_list_val.remove(val_f)


            file_status_ref=False
            file_list_ref_orig = file_list_ref
            for i in range(1,5):
                print "Confirming "+recent_rel+"'s job status..."
                file_status_ref,days_ref=checkStatus(refPath)
                if file_status_ref==True:
                    #print("Looking for .root files to compare...")
                    file_root=False
                    for f in os.listdir(refPath):
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
                                cmd = 'cp'+' '+refPath+'/'+f+' '+'yesterday.'+store_tag+'.root'
                            else:
                                cmd = 'cp'+' '+refPath+'/'+f+' '+'yesterday.'+suffix
                            print cmd
                            if not dryrun:
                                os.system(cmd)
                            file_root=True
                            if f in file_list_ref:
                                file_list_ref.remove(f)
                                print file_list_ref
                    if 'ISFBroker.root' in file_list_ref:
                        file_list_ref.remove('ISFBroker.root')
                    if len(file_list_ref)!=0:
                        print "The following files were not found in the output directory of "+recent_rel+"'s job: "+str(file_list_ref)+", will try to copy them from EOS instead."
                        for ref_f in file_list_ref :
                            if eoscp(ref_f,sys.argv[1],"yesterday",store_tag,recent_rel)!=0:
                                print "File does not exist in EOS."
                            else:
                                file_list_ref.remove(ref_f)

                if len(file_list_ref)==0:
                    break
                else:
                    ##clean up files
                    print "Could not find a full set of files for "+recent_rel+", so removing any we did find."
                    if(len(file_list_ref)+1) < len(file_list_ref_orig): os.system('rm yesterday.*')
                    file_list_ref = file_list_ref_orig
                    file_status_ref=False
                    previous_rel = recent_rel
                    if recent_rel == 'rel_0':
                        recent_rel = 'rel_6'
                    else:
                        recent_rel = 'rel_%d' % (int(recent_rel.split('_')[1])-1)

                    argv[pos]=recent_rel
                    refPath='/'.join(argv)
                    print "As we failed to find all required files in "+previous_rel+", trying again with "+recent_rel
                    print valPath
                    print refPath

            if not file_status_ref==True:
                print("Failed to find a usable reference nightly!")
            else:
                # make fileGrepper happy
                print "leaving with code 0"

        elif file_status_val==False:
            print "Failed to find expected files in the path for today's nightly!"

    except (OSError,IOError):
        print "No running in this release!"
