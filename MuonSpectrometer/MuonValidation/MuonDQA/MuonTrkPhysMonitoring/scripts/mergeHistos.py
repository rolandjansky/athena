# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################
#                              #
#     Nektarios Chr. Benekos   #
#     Jochen Meyer             #
#     April, 2011              #
#     send prun job            #
#                              #
###############################

#!/usr/bin/env python

import sys 
import fileinput
import os
import commands
import string
from os import popen
from time import strftime
import glob
 
##############################################################
##              to be defined
##############################################################

## defined your site @BNL :  ANALY_BNL_ATLAS_2 ANALY_BNL_LOCAL ANALY_LONG_BNL_LOCAL
 
site_name="AUTO"
 
## defined your tag
tag="m_1"

## athena release for ROOT
athena="17.2.1.4"

## Panda lastname.firsname 
user_name="user.nectar"

##############################################################
##              source Athena/Pathena/Dq2
##############################################################
#sCommand = 'dummy'
sCommand = 'source setup-All.sh; echo "===block=="; env; echo "===block==="'
status,output = commands.getstatusoutput(sCommand)

print status
print output

command=output.strip().split('\n')
#print command
indice=0
for line in command :
    if '===block==' in line :
        indice=indice+1
    
    if indice==1 :
        #print line
        env=line.strip().split('=')
        if(len(env))>0:
             #print env
             os.environ[env[0]] = env[1]
             
##############################################################
##############################################################
inDSs = [
'user.nectar.data12_8TeV.periodA1.physics_Muons.PhysCont.DESD_SGLMU.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA3.physics_Muons.PhysCont.DESD_SGLMU.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_SGLMU.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA1.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA3.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_ZMUMU.t0pro12_v01.120421/', 
'user.nectar.data12_8TeV.periodA4.physics_Muons.PhysCont.ESD.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA3.physics_Muons.PhysCont.ESD.t0pro12_v01.120421/',
'user.nectar.data12_8TeV.periodA1.physics_Muons.PhysCont.ESD.t0pro12_v01.120421/'
]

#######################################################
##                    Submit job                     ##
## retry(jobID,retryBuild=True,newSite=True)         ##
#######################################################

### put the jobOption and option'
print "Define pathena command ..."

## for all site
#commandExec0 = ' --outputs newoutput.root --nFilesPerJob=9999  --maxNFilesPerJob=10000 --useOldStyleOutput --match "CombinedMonitoring*.root*" '
#CommonPart=" hadd newoutput.root \`echo %%IN | sed 's/,/ /g'\`"
#comFirst = 'prun --exec "'+ CommonPart +'" ' + commandExec0 + ' --athenaTag %s --outDS %s --inDS %s --site %s '

commandExec0 = ' --outputs mergeHist.root --nFilesPerJob=9999  --maxNFilesPerJob=10000 --useOldStyleOutput --match "CombinedMonitoring*.root*" '
CommonPart=" python aho.py %%IN mergeHist.root "
comFirst = 'prun --exec "'+ CommonPart +'" ' + commandExec0 + ' --athenaTag %s --outDS %s --inDS %s --site %s '

# print command
print comFirst
 
print "Loop over input DataSet .. "
for i,inDS in enumerate(inDSs):
    print 'inDS = ',inDS
    print 'i = ',i
    #### create the output name
    position = string.find(inDS, ".t0pro12_v01")
    outDS=inDS[:position]
    outDS+=".mergeHist."+tag
    outDS+="/"
    print "   ",outDS
    print "   Final output name = ",outDS
    print 'dataset name =',inDS 
    ## build the command line
    sCommand=comFirst % (athena,outDS,inDS,site_name)
    print "sCommand ",sCommand
    print "execute the command ... "
    status = os.system(sCommand)

print "################ summary of the pathena command ############################"

