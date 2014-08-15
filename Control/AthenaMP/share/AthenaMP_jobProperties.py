# @file: Control/AthenaMP/share/AthenaMP_jobProperties
# @author: Mous Tatarkhanov <tmmous@cern.ch>
# $Id: AthenaMP_jobProperties.py, v 1.1 2010-12-02 tmmous Exp $

#==============================================================
# List of all available AthenaMP job properties explained
#==============================================================

#-------------------IMPORTANT NOTE-----------------------------
# All mp jobs get default job property values
# NO NEED to include this file!
# 
# USE this file and change settings in your job options 
# ONLY when you want to change/play with properties!
#--------------------------------------------------------------

#==============================================================
# HOW TO RUN ATHENAMP:
# 1. Job option runs:   
#   CLI '--nprocs=$n' to be used in command line run of athena.py 
# 2. Job transform runs:
#   CLI  '--athenaopts="--nprocs=$n"' to be used in command 
#==============================================================

#--------------------------------------------------------------
# AthenaMP properties
#--------------------------------------------------------------
from AthenaMP.AthenaMPFlags import jobproperties as jps

#--------------------TmpDir-----------------------------------
#TmpDir property controls where the workers create tmp-dir for output

jps.AthenaMPFlags.TmpDir= "" #default -> "/tmp/$USER/athena-mp-tmp-$USER/$PID"
#jps.AthenaMPFlags.TmpDir = "$HOME" #set to your home folder
#-EXAMPLE-
#for testing different IO setting for MP runs (workers output):
#jps.AthenaMPFlags.TmpDir = "$HOME/scratch0" # on lxplus network drive
#jps.AthenaMPFlags.TmpDir = "/dev/shmm" # standard in memory drive
#--------------------------------------------------------------


#-------------------AffinityCPUList-----------------------------
# AffinityCPUlist option controls the affinity of workers to CPU-cores on 
# multicore machines. default value = [] -> workers float (not pinned)

jps.AthenaMPFlags.AffinityCPUList=[] #default -> workers are floating (not pinned)
#jps.AthenaMPFlags.AffinityCPUList=[0,1,2,3,4,5,6,7,8] 

#-NOTES-
#if you have only 4 workers they are pinned 
#to following order of CPU-cores: 0,1,2,3
#if you want to change the order to 8,7,6,5 
# CPUList should be [8,7,6,5,...]

##-EXAMPLE-
#If you are forking only two workers, and you want to pin them 
# to the first and last CPU-cores available on your machine do:
#import multiprocessing
#jps.AthenaMPFlags.AffinityCPUList=[0, multiprocessing.cpu_count()-1] 
#--------------------------------------------------------------


#----------------- EventsBeforeFork----------------------------
# EventsBeforeFork option controls when to fork Workers from mother process 0-after init
# value n=0-> after init,  n>0 -> after n'th event. default value - 0.

jps.AthenaMPFlags.EventsBeforeFork=0    #default
#jps.AthenaMPFlags.EventsBeforeFork=1   #fork workers after processing 1-evt in mother 

#-NOTES-
#Forking after processing evts in mother has advantage of
#sharing more memory among workers and parent by COW mechanism. (>0.5Gb vs. 0.2Gb)
#But that triggers vetoing algorithm on first events processed in mother
#on each output stream to avoid writing of pool_file in mother  
#This is done for merging compliance reasons of pool files created by workers
# /vetoing algorithm ref: Charles Leggett/
#--------------------------------------------------------------


#--------------------doFastMerge-----------------------------
#doFastMerge is the switch between pool_fast_merge and pool_slow_merge

jps.AthenaMPFlags.doFastMerge = False #default -> use more reliable slow merge
#jps.AthenaMPFlags.doFastMerge = True 

#-NOTES-
#slow merge creates separate Athena merging job /ref: Sebastien Binet/
#fast_merge should be an order of magnitude faster than slow merge
#fast merge uses PoolMerge tool /ref: Peter Van Gemmeren/
#--------------------------------------------------------------


#--------------------doRoundRobin-----------------------------
#doRoundRobing is the switch between RoundRobin and Queue mechanism
#of feeding evts to Workers

jps.AthenaMPFlags.doRoundRobin = False #default -> use MP.Queue 
#--------------------useSingleReader-----------------------------
#useSingleReader is the switch to choose single reader to get events
#into shared memory queue

jps.AthenaMPFlags.useSingleReader = False #default -> worker read from file
#jps.AthenaMPFlags.doFastMerge = True  #use RoundRobin

#-COMMENTS-
#default Mp.Queue is more efficient way of distributing evts to Workers 
#but when EvtMax=-1 (undefined nbr. of evts.) -> RoundRobin scheme is used
#--------------------------------------------------------------


#--------------------------------------------------------------
#--------- more options will be created as needed or requested-
#--------------------------------------------------------------

#==============================================================
#---------------------- THE END -------------------------------
#==============================================================
