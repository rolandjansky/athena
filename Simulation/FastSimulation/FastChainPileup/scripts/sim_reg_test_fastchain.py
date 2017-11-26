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
def get_recent(path,pathsplit):
    print "---------------------------------------------------------------------------------------"
    pathlist=path.split('/')
    pos_index=0
    release_list=[]
    for i,a in enumerate(pathsplit):
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
            if days_tmp>=0 and days_tmp<recent_days:
                recent_days=days_tmp
                recent_index=i
        else:
            print "==> Nightly: "+release_tag+"  Age: N/A"
        print "---------------------------------------------------------------------------------------"
    if recent_days>998:
        print "=====> Could not find another release to compare to. Returning today's nightly ("+todays_release+") as the reference."
        return todays_release
    good_rel_refease="rel_"+str(recent_index)
    print "=====> "+good_rel_refease+" is the most recent nightly to use as reference."
    return good_rel_refease

###Add EOS prefix###
def eoscp(package,filename,jobname,nightly,tag,store_tag,release):
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
    #postfix='/'.join(argv[-3:])
    
    postfix="x86_64-slc6-gcc62-opt/Athena/"+package
    #middle='/'.join(argv[-6:-5])
    #middle=argv[-5]
    middle=nightly
    pathlist=["prod","batch"]#This 2nd option should make the script work on the RTT test instance.
#    status=0
    for path in pathlist:
        prefix="root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/"+path+"/rtt"
        #print "postfix: "+postfix
        #print "middle: "+middle
        #print "prefix: "+prefix
        #print "jobname: "+jobname
        #print "filename: "+filename
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
    print '------------------usage-----------------'
    print 'sim_reg_test_fastchain.py [ref package] [ref rel] [ref job name] [ref filename] [mon package] [mon rel] [mon job name] [mon filename]'
    print 'packages supported are ISF_Validation and FastChainPileup'
    print 'to use current release, put "current" as a release argument'
    print 'if job fails for today`s ref/mon, the latest succeeded job will be taken'
    print '----------------------------------------'
    
    print 'Got input arguments:'
    print 'reference:'
    print 'package: ', sys.argv[1]
    print 'release: ', sys.argv[2]
    print 'job: ', sys.argv[3]
    print 'filename: ', sys.argv[4]
    print 'monitored:'
    print 'package: ', sys.argv[5]
    print 'release: ', sys.argv[6]
    print 'job: ', sys.argv[7]
    print 'filename: ', sys.argv[8]
    if sys.argv[1]!='ISF_Validation' and sys.argv[1]!='FastChainPileup' :
      print 'ERROR: unsupported reference package.'
    if sys.argv[5]!='ISF_Validation' and sys.argv[5]!='FastChainPileup' :
      print 'ERROR: unsupported monitored package.'
    #establish path to monitored release, rel num is just dummy at this stage
    pwdmon=''
    prepath='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_0/'
    #TODO: make postpath configurable
    postpath='/build/x86_64-slc6-gcc62-opt/Athena/'
    currel='NOREL'
    if sys.argv[6] == 'current' or sys.argv[2] == 'current' :
      curdir=os.getcwd()
      if os.environ.has_key('LS_SUBCWD'): 
	curdir= os.environ['LS_SUBCWD']
      #!TEST
      #curdir='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/mig13/build/x86_64-slc6-gcc49-opt/offline/FastChainPileup/'
      #!END TEST
      folders=curdir.split('/')
      print folders
      for idx, folder in enumerate(folders):
	if 'rel_' in folder :
	  try:
	    currel = folders[idx+1]
	  except IndexError:
	    print 'ERROR: Could not find current release in the path after /rel_*/'
      if currel == 'NOREL' :
	print 'ERROR: Could not find current release in the path. Setting release to "NOREL" and going to crash.'
	    
    relmon=sys.argv[6]
    if sys.argv[6] == 'current' : 
      relmon = currel  
    pwdmon=prepath + relmon + postpath
    #establish path to reference release, rel num is just dummy at this stage  
    pwdref=''
    prepath='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_0/'
    postpath='/build/x86_64-slc6-gcc62-opt/Athena/'
    relref=sys.argv[2]
    if sys.argv[2] == 'current' : 
      relref=currel
    pwdref=prepath + relref + postpath
   
    argvref=pwdref.split('/')
    argvref.pop()
    argvref.append(sys.argv[1])
    argvref.append(sys.argv[3])
    print 'argvref ',argvref 
    corr_path_ref='/'.join(argvref)
    print 'reference path:', corr_path_ref
    ## Find a release to use as a reference
    print '###### Get the most recent successful reference job ######'
    good_rel_ref=get_recent(corr_path_ref,argvref)
    
    print ''
    print ''
    print ''
    print ''
    
    argvmon=pwdmon.split('/')
    argvmon.pop()
    argvmon.append(sys.argv[5])
    argvmon.append(sys.argv[7])
    print 'argvmon ',argvmon 
    corr_path_mon='/'.join(argvmon)
    print 'monitored path:', corr_path_mon
    print '###### Get the most recent successful monitored job ######'
    good_rel_mon=get_recent(corr_path_mon,argvmon)  

    posref=9
    for i,a in enumerate(argvref):
        if a.find("rel_")>=0 :
            posref = i
    
    posmon=9
    for i,a in enumerate(argvmon):
        if a.find("rel_")>=0 :
            posmon = i

    argvmon[posmon]=good_rel_mon
    monPath='/'.join(argvmon)
    
    argvref[posref]=good_rel_ref
    refPath='/'.join(argvref)

    print "Releases to compare: "+good_rel_ref+", "+good_rel_mon
    print refPath
    print monPath

    # look for suffix
    file_name='.root'
    store_tag=''

    # make sure ISFBroker.root is not confused with truth*.root
    compfilesref=['ISFBroker.root']
    compfilesmon=['ISFBroker.root']
    #if len(sys.argv)>3:
        ## got extended argument list create file list
        #for f in sys.argv[3:]:
            #if f.endswith(file_name):
                #print "Add file %s in searching list."%(f)
                #compfiles.append(f)
            #else:
                #print "Find tag set, use common tag: %s."%(f)
                #store_tag=f
                
    compfilesref.append(sys.argv[4])
    compfilesmon.append(sys.argv[8])
    print 'compfilesref:',compfilesref
    print 'compfilesref:',compfilesmon
    file_list_ref=compfilesref[:]
    print file_list_ref
    file_list_val=compfilesmon[:]
    try:
        print "Confirming monitored job status..."
        file_status_val,days=checkStatus(monPath)

        if file_status_val==True:
            #print "Success"
            file_root=False
            for f in  os.listdir(monPath):
                if f.endswith(file_name) and f != 'yesterday.root' and f!= 'today.root':
                    print f
                    name,suffix=f.split('.',1)
                    if f in compfilesmon:
                        suffix=f
                        print 'f1'
                    elif len(compfilesmon)>=2:
                        continue
                    if store_tag!='':
                        cmd = 'cp'+' '+monPath+'/'+f+' '+'today.'+store_tag+".root"
                    else:
                        cmd = 'cp'+' '+monPath+'/'+f+' '+'today.'+suffix
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
                    if eoscp(sys.argv[5],val_f,sys.argv[7],sys.argv[6],"today",store_tag,good_rel_mon)!=0 :
                        print "File does not exist in EOS."
                    else:
                        file_list_val.remove(val_f)


            file_status_ref=False
            file_list_ref_orig = file_list_ref
            for i in range(1,5):
                print "Confirming reference job status..."
                file_status_ref,days_ref=checkStatus(refPath)
                if file_status_ref==True:
                    #print("Looking for .root files to compare...")
                    file_root=False
                    for f in os.listdir(refPath):
                        if f.endswith(file_name) and f != 'yesterday.root' and f!= 'today.root':
                            print f
                            print compfilesref
                            name,suffix=f.split('.',1)
                            #if name in compfiles:
                            if f in compfilesref:
                                suffix=f
                                print 'f2'
                            elif len(compfilesref)>=2:
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
                        print "The following files were not found in the output directory of "+good_rel_ref+"'s job: "+str(file_list_ref)+", will try to copy them from EOS instead."
                        for ref_f in file_list_ref :
                            if eoscp(sys.argv[1],ref_f,sys.argv[3],sys.argv[2],"yesterday",store_tag,good_rel_ref)!=0:
                                print "File does not exist in EOS."
                            else:
                                file_list_ref.remove(ref_f)

                if len(file_list_ref)==0:
                    break
                else:
                    ##clean up files
                    print "Could not find a full set of files for "+good_rel_ref+", so removing any we did find."
                    if(len(file_list_ref)+1) < len(file_list_ref_orig): os.system('rm yesterday.*')
                    file_list_ref = file_list_ref_orig
                    file_status_ref=False
                    previous_rel = good_rel_ref
                    if good_rel_ref == 'rel_0':
                        good_rel_ref = 'rel_6'
                    else:
                        good_rel_ref = 'rel_%d' % (int(good_rel_ref.split('_')[1])-1)

                    argvref[pos]=good_rel_ref
                    refPath='/'.join(argvref)
                    print "As we failed to find all required files in "+previous_rel+", trying again with "+good_rel_ref
                    print monPath
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
