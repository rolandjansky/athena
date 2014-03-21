#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#File name: isf_test.py
#Author:Bo Liu <boliu@cern.ch>

import os,sys

from optparse import OptionParser
parser=OptionParser()
parser.add_option("--jobs",dest="jobs",help="Option for compare jobs, Add with full job name,seperate with comma")
parser.add_option("--referencefile=",dest="referencefile",help="Options for referencefiles,seperate with comma",default="")
parser.add_option("--chain",dest="chain",help="Option for chain name")
parser.add_option("--tag",dest="tag",help="tag for stored file",default="")

(options,args)=parser.parse_args()

jobs=options.jobs
referencefile=options.referencefile
chain=options.chain
tag=options.tag

pwd=os.getcwd()
###Add chain directory###
def addchain(current,chain_name):
##   current=os.getcwd()
   print current
   chain_store_prefix="/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/chainstore"
##   chain_store_prefix="/afs/cern.ch/work/b/boliu/testarea/ISF_Test/Results/rtt/chainstore"
   chain_store_postfix="AtlasProduction/Simulation/ISF/"
   argv=current.split('/')
   middle_list=[]
   middle_list.append(argv[-6])
   middle_list.append(argv[-4])
   middle='/'.join(middle_list)
   chain_store_path=chain_store_prefix+'/'+middle+'/'+chain_store_postfix+argv[-2]+'/'+chain_name
   return chain_store_path
###copy file from chainstore###(For monitoring .root file from Reconstruction)
def copy_from_chainstore(current_path,Chain_Name,filename,posttag):
#   current_path=os.getcwd()
   print current_path
   num_file=0
   chain_store_path=addchain(current_path,Chain_Name)
   for f in os.listdir(chain_store_path):
      if f==filename:
         if posttag=="":
            cmd="cp %s/%s %s" % (chain_store_path,filename,filename)
         else:
            cmd="cp %s/%s %s.%s" % (chain_store_path,filename,posttag,filename)
         print cmd
         num_file=num_file+1
         os.system(cmd)
   if num_file==1:
      print "Copy %s from %s successfully." % (filename,chain_store_path)
   else:
      print "Can not find file %s in this store." % (filename,)
###copy file###(For some small .root file)
def copyfile(current,jobname,filename,posttag):
  # current=os.getcwd()

   print current
   argv=current.split('/')
   argv.pop()
   argv.append(jobname)
   store_path='/'.join(argv)
   num_file=0
   for f in os.listdir(store_path):
      if f==filename:
         if posttag=="":
            cmd="cp %s/%s %s" % (store_path,filename,filename)
         else: 
            cmd="cp %s/%s %s.%s" % (store_path,filename,posttag,filename)
         print cmd
         num_file=num_file+1
         os.system(cmd)
   if num_file==1:
      print "Copy file %s successfully." % (filename,)
      return True
   else:
      print "Can not find file %s in this store." % (filename,)
      return False
##main###
if __name__ =="__main__":
   pwd=os.getcwd()
   if os.environ.has_key('LS_SUBCWD'):
      pwd=os.environ['LS_SUBCWD']
   print pwd
   
   referencelist=referencefile.split(',')
   print referencelist
   joblist=jobs.split(',')
   print joblist

### search referencefile in jobname subdirectory at first, if can not find it, copy it from chainstore
   for rf in referencelist:
      status=False
      for job in joblist:
         print "Search referencefile %s in %s subdirectory"%(rf,job)
         status=copyfile(pwd,job,rf,tag)
         if status==True:
            break
      if status==False:
         print "Search referencefile %s in %s chainstore"%(rf,chain)
         copy_from_chainstore(pwd,chain,rf,tag)
### make FileGrepper successful
   print "leaving with code 0"

      
#
