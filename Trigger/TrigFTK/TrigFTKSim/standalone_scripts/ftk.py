#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# see if psyco is available
try:
    import psyco
    psyco.full()
except ImportError:
    pass

import os,sys,getopt
from FTKCore import *

# try to open an old command history file
try:
    history_file = open(".ftk_history","r")
    hcontent = history_file.readlines()
    history_file.close()
except:
    hcontent = []
    
HISTLEN=1000
if len(hcontent)>=HISTLEN :
    del hcontent[0:len(hcontent)-HISTLEN+1] # delete at least the 1st element
# reencode the command-line
sep = " "
hcontent.append(sep.join(sys.argv)+"\n")

# write the history file
try:
    history_file = open(".ftk_history","w")
    history_file.writelines(hcontent)
    history_file.close()
except:
    print "Cannot write to .ftk_history"
    
if __name__=='__main__':

    ###############################################################
    # EDIT THESE SETTINGS
    ###############################################################

    # Create a base configuration object
    o = SimpleOptions()
    o.SetCommonPars(N=8,M=4,L=11,nevents=200000)
    o.SetConfigDir('../config')
    o.SetFtksimDir('../')
    o.SetFtkanaDir('../../TrigFTKAna') # optional
    o.SetFtklibDir('../../TrigFTKLib') # optional
    o.SetBankDir('../test/banks')
    o.SetOutputDir('./output')
    o.SetRunstring('raw_11L_50x64x144_mydataset')
    # Add/override any additional settings:
    o.Add('MAXPATTERNS',-1)
    o.Add('FWO_NSECTORS',0)
    o.Add('READ_CLUSTERS',0)
    o.Add('qsub_opts','-l mem=3700MB')

    ###############################################################
    # DO NOT EDIT THE STUFF BELOW
    ###############################################################
    # Override the above settings with command line arguments (if any)
    shortopts='s:cbhe:f:g:j:r:t:q:pn:d:o:a:m:i:I:'
    longopts=['mode=','site=','username=','preloadjob=','official=']
    longopts+=['inDS=','match=','nJobs=','nFilesPerJob=','nSkipFiles=']
    longopts+=['gridtest','maxNJobs=']
    longopts+=['express','unlimitNumOutputs','maxCpuCount=','memory=']
    longopts+=['inputs=','regions=','subregions=','loops=',]

    try:
        opts, args = getopt.getopt(sys.argv[1:],shortopts,longopts)
    except getopt.GetoptError, err:
        print str(err)
        o.help()

    # if it is a grid job, intercept and process it outright
    o.CheckIfGrid(opts)

    # parse command line options
    o.ParseOptions(opts)

    # expand certain command shortcuts, such as fullchain or effchain
    o.PostProcessOptions()

    # create job managers
    managers = [JobManager(),]

    # if "preloadjob" is given, use the pickled job object
    job = o.UnPickleJob()
    if job != None:
        managers[-1].AddJob(job)
    bankprodmode = False

    # Loop over actions and create corresponding jobs
    for action in o.actions:
        deps = o.QueueOpts(action)

        if deps:
            print 'Applied inline PBS queue dependencies:',deps
            managers[-1].deps += deps
            continue

        if o.InlineOpts(action)==True:
            print 'Applied inline changes to run settings!'
            continue

        if action.find('makedc')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'makedc',range(o.Geti('N')),range(o.Geti('M')))
            for i in regions:
                for j in subregions:
                    c=MakeDCJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action.find('roadfinder711')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'roadfinder711',range(o.Geti('N')),range(o.Geti('M')))
            for i in regions:
                for j in subregions:
                    c=RoadFinderJob()
                    c.customtpl = "roadfinder711.tpl"
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action.find('roadfindertsp')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'roadfindertsp',range(o.Geti('N')),range(o.Geti('M')))
            for i in regions:
                for j in subregions:
                    c=RoadFinderJob()
                    c.customtpl = "roadfindertsp.tpl"
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action.find('roadfinder')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'roadfinder',range(o.Geti('N')),range(o.Geti('M')))
            for i in regions:
                for j in subregions:
                    c=RoadFinderJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action.find('globalroadmerger')!=-1:
            # This option merges roads across the entire detector.
            # If MERGED_ROADS=1, it merges the results of roadmerger from 8 regions
            # If MERGED_ROADS=0, it merged the results of roadfinder from all regions and subregions
            # This is ONLY needed for computing effcurve coverage over the entire detector
            # TODO - not coded yet!
            regions,d = o.ParseA_A_B_C(action,'roadmerger',range(o.Geti('N')))
            if o.Get('MERGED_ROADS')!='1':
                print 'ERROR: requested roadmerger, but MERGED_ROADS was set to zero'
                sys.exit(3)
            for i in regions:
                c=RoadMergerJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('roadmerger')!=-1:
            regions,d = o.ParseA_A_B_C(action,'roadmerger',range(o.Geti('N')))
            if o.Get('MERGED_ROADS')!='1':
                print 'ERROR: requested roadmerger, but MERGED_ROADS was set to zero'
                sys.exit(3)
            for i in regions:
                c=RoadMergerJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('checkroads')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'checkroads',range(o.Geti('N')),range(o.Geti('M')))
            for i in regions:
                c=CheckRoadsJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                c.subregions = subregions
                managers[-1].AddJob(c)

        elif action.find('hitfilter')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'hitfilter',range(o.Geti('N')),range(o.Geti('M')))
            if o.Get('MERGED_ROADS')=='1':
                subregions = [0,] # if running over merged roads, ignore subregions!
            if o.Get('SHIFTROADS')==None:
                print 'Skipping hitfilter in all regions.'
                print 'Missing -o SHIFTROADS=run_string_for_shifted_ss argument'
                continue
            for i in regions:
                for j in subregions:
                    c=HitFilterJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    c.Add('NGROUPS',2)
                    managers[-1].AddJob(c)

        elif action.find('trackfitter711')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'trackfitter711',range(o.Geti('N')),range(o.Geti('M')))
            if o.Get('MERGED_ROADS')=='1':
                subregions = [0,] # if running over merged roads, ignore subregions!
            for i in regions:
                for j in subregions:
                    c=TrackFitter711Job()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action.find('trackfitter')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'trackfitter',range(o.Geti('N')),range(o.Geti('M')))
            if o.Get('MERGED_ROADS')=='1':
                subregions = [0,] # if running over merged roads, ignore subregions!
            for i in regions:
                for j in subregions:
                    c=TrackFitterJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action.find('checktracks')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'checktracks',range(o.Geti('N')),range(o.Geti('M')))
            if o.Get('MERGED_ROADS')=='1':
                subregions = [None,] # if running over merged roads, ignore subregions!
            for i in regions:
                c=CheckTracksJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                c.subregions = subregions
                managers[-1].AddJob(c)

        elif action=='alltrackmerger':
            # special mode for final global trackmerger:
            # merges 8 regions of tracks_merge/$reg/*.root and saves output to tracks_merge/8/*.root
            regions=range(o.Geti('N'))
            subregions=[None,]
            c=TrackMergerJob()
            c.Set(o.GetOptions())
            c.Add('INPUT_TREE_MERGED',1)
            # if this is a whole-detector trackmerger, save it under region # 8
            c.Add('i',o.Get('N'))
            c.regions = regions
            c.subregions = subregions
            managers[-1].AddJob(c)

        elif action.find('trackmerger')!=-1:
            regions,subregions = o.ParseA_A_B_C(action,'trackmerger',range(o.Geti('N')),range(o.Geti('M')))
            c=TrackMergerJob()
            c.Set(o.GetOptions())
            # if this is a whole-detector trackmerger, save it under region # 8
            c.Add('i',o.Get('N'))
            # but if this is a single-region trackmerger, save it under this regions's subfolder
            if len(regions)==1:
                c.Add('i',regions[0])
            c.regions = regions
            c.subregions = subregions
            managers[-1].AddJob(c)

        elif action.find('makelists')!=-1:
            # makes road and track lists SCTtrk mode
            regions,subregions = o.ParseA_A_B_C(action,'makelists',range(o.Geti('N')),range(o.Geti('M')))
            if o.Get('MERGED_ROADS')=='1':
                subregions = [0,] # if running over merged roads, ignore subregions!
            for i in regions:
                for j in subregions:
                    c=MakelistsJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    managers[-1].AddJob(c)

        elif action=='makelinks':
            # symlinks 4L core folders into 11L for the last SCTtrk step
            if o.Geti('L')!=4:
                print 'Warning: makelinks must be run in the 4L configuration ( => 11L )'
                continue
            c=ShellJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            old = c.Get('runstring')
            new = old.replace('4L','11L')
            c.append('cd %s && if [ ! -s %s ]; then ln -s %s %s; fi;'%(c.Get('outdir'),new,old,new))
            c.noexec = False
            managers[-1].AddJob(c)

        elif action.find('relabelsplit')!=-1:
            # makes road and track lists SCTtrk mode
            regions,subregions = o.ParseA_A_B_C(action,'relabelsplit',range(o.Geti('N')),range(o.Geti('M')))
            for i in regions:
                for j in subregions:
                    c=ShellJob()
                    c.Set(o.GetOptions())
                    c.Add('sleep_time',0.1)
                    old = c.Get('runstring')
                    new = old.replace('4L','11L')
                    c.append('cd '+c.Get('outdir')+' && '+'mkdir -p %s/{roads,tracks}/%d/%d'%(new,i,j))
                    c.append('cd '+c.Get('outdir')+' && '+'cd %s/tracks/%d/%d/'%(new,i,j)+' && '+'ln -s ../../../../%s_tag%d/tracks_merge/%d/*root* .'%(old,j,i))
                    c.append('cd '+c.Get('outdir')+' && '+'cd %s/roads/%d/%d/'%(new,i,j)+' && '+'ln -s ../../../../%s_tag%d/merged/%d/*root* .'%(old,j,i))
                    managers[-1].AddJob(c)

        elif action == 'anaone' or action=='ana':
            # large ana job that dumps ALL regions together
            c=AnaJob();
            c.Set(o.GetOptions())
            c.Add('i',o.Get('N'))
            c.regions = [o.Geti('N'),]
            managers[-1].AddJob(c)

        elif action=='effstudy': 
            # effchain job for all regions
            c=EffstudyJob();
            c.Set(o.GetOptions())
            c.Add('i',o.Get('N'))
            c.regions = [o.Geti('N'),]
            managers[-1].AddJob(c)

        elif action.find('effstudy')!=-1:
            regions,d = o.ParseA_A_B_C(action,'effstudy',range(o.Geti('N')))
            i=regions[0]
            c=EffstudyJob();
            c.Set(o.GetOptions())
            c.Add('i',i)
            c.regions = [i,]
            managers[-1].AddJob(c)

        elif action.find('ana')!=-1:
            regions,d = o.ParseA_A_B_C(action,'ana',range(o.Geti('N')))
            i=regions[0]
            c=AnaJob()
            c.Set(o.GetOptions())
            c.Add('i',i)
            c.regions = [i,]
            managers[-1].AddJob(c)

        elif action=='S' or action=='STOP':
            # start a new group of jobs
            managers.append(JobManager())

        elif action.find('plot')!=-1:
            regions = o.Parse_A_B_C(action,'plot',range(o.Geti('N')))
            c=PlotterJob()
            c.Set(o.GetOptions())
            c.regions = regions
            managers[-1].AddJob(c)

        elif action.find('effcurve')!=-1:
            regions,d = o.ParseA_A_B_C(action,'effcurve',range(o.Geti('N')))
            c=EffCurveJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            c.regions = regions
            managers[-1].AddJob(c)

        elif action.find('neweffcurve')!=-1:
            regions,d = o.ParseA_A_B_C(action,'neweffcurve',range(o.Geti('N')))
            c=NewEffCurveJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            c.regions = regions
            managers[-1].AddJob(c)

        elif action=='log' or action=='check' or action=='summary':
            c=SummaryJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            managers[-1].AddJob(c)

        elif action=='list':
            c=ListJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            managers[-1].AddJob(c)

        elif action=='make' or action=='gmake':
            c=ShellJob();
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            c.append('echo "Executing in shell:"')
            if o.Get('standalone_dir'):
                c.append('make -C '+o.Get('standalone_dir'))
            managers[-1].AddJob(c)

        # pattern production actions:
        elif action=='allsectors':
            # makes sectors in all regions in one job
            bankprodmode=True
            c=AllSectorJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            managers[-1].AddJob(c)

        elif action=='quicksectors':
            # beta: make sectors in all regions using TrigFTKSim quickfit (reads/writes PNFS and bz2)
            bankprodmode=True
            c=QuickSectorJob()
            c.Set(o.GetOptions())
            managers[-1].AddJob(c)

        elif action.find('sectors')!=-1:
            # one sector job per region - useful when we need a different trklookup in each region
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'sectors',range(o.Geti('N')))
            for i in regions:
                c=SectorJob()
                c.Set(o.GetOptions())
                c.Add('sleep_time',0.1)
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action=='convertlookup':
            bankprodmode=True
            c=ConvertLookupJob()
            c.Set(o.GetOptions())
            c.Add('sleep_time',0.1)
            c.noexec = True
            managers[-1].AddJob(c)

        elif action.find('filter')!=-1:
            bankprodmode=True
            nfilt,regions = o.ParseA_A_B_C(action,'filter',[15,],range(o.Geti('N')))
            for i in regions:
                c=FilterJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                c.nfilt = nfilt[0]
                c.noexec = False
                managers[-1].AddJob(c)

        elif action.find('corrgen')!=-1:
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'corrgen',range(o.Geti('N')))
            for i in regions:
                c=CorrgenJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('cleanup')!=-1:
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'cleanup',range(o.Geti('N')))
            for i in regions:
                c=CleanupJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('pattgen')!=-1:
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'pattgen',range(o.Geti('N')))
            for i in regions:
                c=PattgenJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('promote')!=-1:
            bankprodmode=True
            regions,subregions,run = o.ParseA_A_B_C__A(action,'promote',a=range(o.Geti('N')),b=range(o.Geti('M')),c=(0,))
            for i in regions:
                for j in subregions:
                    c=PromotePattgenJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    c.Add('r',run[0])
                    managers[-1].AddJob(c)

        elif action.find('patterns')!=-1:
            bankprodmode=True
            regions,subregions,run = o.ParseA_A_B_C__A(action,'patterns',a=range(o.Geti('N')),b=range(o.Geti('M')),c=(0,))
            for i in regions:
                for j in subregions:
                    c=PatternsJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    c.Add('r',run[0])
                    managers[-1].AddJob(c)

        elif action.find('oldpatmerge')!=-1:
            # original (old) patmerge utility that runs TrigFTKLib pattgen
            # It can only read unzipped banks, and can only merge 7 banks at a time
            bankprodmode=True
            regions,subregions,runs = o.ParseA_A_B_C__A(action,'oldpatmerge',a=range(o.Geti('N')),b=range(o.Geti('M')),c=(0,1))
            for i in regions:
                for j in subregions:
                    c=OldPatmergeJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    c.runs = runs
                    managers[-1].AddJob(c)

        elif action.find('patmerge')!=-1:
            # new patmerge utility implemented in TrigFTKSim
            # Can read/write bzip'ed files, and has no limits on # of files merged at the same time
            bankprodmode=True
            regions,subregions,runs = o.ParseA_A_B_C__A(action,'patmerge',a=range(o.Geti('N')),b=range(o.Geti('M')),c=(0,1))
            for i in regions:
                for j in subregions:
                    c=PatmergeJob()
                    c.Set(o.GetOptions())
                    c.Add('i',i)
                    c.Add('j',j)
                    c.runs = runs
                    managers[-1].AddJob(c)

        elif action.find('split')!=-1:
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'split',range(o.Geti('N')))
            for i in regions:
                c=SplitJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('makelookup')!=-1:
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'makelookup',range(o.Geti('N')))
            for i in regions:
                c=MakeLookupJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('dumptracks')!=-1:
            bankprodmode=True
            regions,d = o.ParseA_A_B_C(action,'dumptracks',range(o.Geti('N')))
            for i in regions:
                c=QuickDumptracksJob()
                c.Set(o.GetOptions())
                c.Add('i',i)
                managers[-1].AddJob(c)

        elif action.find('printOptions')!=-1:
            o.printOptions_help()

        elif action=="":
            print "Empty action"

        else:
            print 'Unknown action:',action
            o.help()

    # check the directories
    o.CheckConfigDir()
    o.CheckInputListDir()
    o.CheckFtksimDir()
    o.CheckFtkanaDir()
    if bankprodmode==True:
        o.CheckFtklibDir()
        o.CheckBankDir(create=True)
    else:
        o.CheckBankDir(create=False)
    o.CheckOutputDir()

    # Process the jobs
    if o.Get('run_mode').find('threaded')!=-1:
        (nthreads,ncpus) = o.GetThreadingInfo()
        for i,manager in enumerate(managers):
            print "Thread Processing %d/%d job groups containing %d jobs"%(i+1,len(managers),manager.GetNJobs())
            manager.ProcessJobsThreaded(nthreads)

    else:
        jobIDs = []
        condorSubFiles = []

        for i,manager in enumerate(managers):
            print "Processing %d/%d job groups containing %d jobs"%(i+1,len(managers),manager.GetNJobs())

            if o.Get('run_mode').find('pbs')!=-1:
               # if explicit dependencies are given, enforce them:
               if manager.deps:
                   for idx in manager.deps:
                       if idx<0 and i+idx>=0:
                           manager.reqIDs += managers[i+idx].jobIDs
                       if idx>=0 and idx<len(managers):
                           manager.reqIDs += managers[idx].jobIDs
   
               else:
                   # if dependencies are not given explicitly, use jobs from the prior group
                   if i>0:
                       manager.reqIDs = managers[i-1].jobIDs


                    
            manager.ProcessJobs()

            if o.Get('run_mode').find('condor')!=-1:
                condorSubFiles.append(manager.GetCondorSubmitFiles())
            else:
                # summarize PBS id's of submitted jobs (if in PBS queue)
                jobIDs += manager.jobIDs
                manager.StatusPBS()

        if o.Get('run_mode').find('condor')!=-1:

            print
            print "Building DAGMAN file"
            print 
            dagFilePath = managers[0].jobs[0].path
            timeStamp   = managers[0].jobs[0].Timestamp()
            f = open(os.path.join(dagFilePath,"submit."+"_"+timeStamp+".dag"),'w')
            print os.path.join(dagFilePath,"submit."+"_"+timeStamp+".dag")
            # counter for each job number
            currentJob = 1

            # To build the tree 
            jobNumbers = []

            #
            #  ok so condorSubFiles is a list of list
            #     The top level list gives the dependencies
            for procStep in condorSubFiles:
                
                for subJob in  procStep:
                    thisStepNumbers = []
                    line = "Job "+str(currentJob)+" "+subJob 
                    print line
                    f.write(line+"\n")
                    thisStepNumbers.append(currentJob)
                    currentJob += 1
                    
                jobNumbers.append(thisStepNumbers)

            for depItr in range(len(jobNumbers) - 1):
                line = "PARENT "
                for jn in jobNumbers[depItr]:
                    line += str(jn)+" "
                line += "CHILD "
                for jn in jobNumbers[depItr+1]:
                    line += str(jn)+" "
                print line
                f.write(line+"\n")

            f.write("\n")
            f.close()

            print
            print "Submitting DAGMAN "
            print
            os.system('condor_submit_dag '+os.path.join(dagFilePath,"submit."+"_"+timeStamp+".dag"))


        else:
            # all jobs are submitted in "HOLD" state. In the end, we release them:
            if len(jobIDs)>0:
                print 'Releasing hold from PBS jobs. Please wait...'
                for j in jobIDs:
                    os.system('qrls %s'%j)
