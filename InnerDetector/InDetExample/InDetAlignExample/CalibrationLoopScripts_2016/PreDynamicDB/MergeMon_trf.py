#!/usr/bin/env python

#########################################################################
##
## MergeMon trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##
##     1) 'inputMonitoringFiles': python list
##          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names)
##        or list of file dictionaries
##          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...},  
##           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...]
##     2) 'outputMonitoringMergeFile': string 'datasetname#filename'
##        (merged output dataset name + file)
##     3) 'iteration': integer 
##        (number of current iteration; optional -- if not given, will be extracted from input dataset name)
##
##  - assembles custom jobOptions file, runs athena
##
##########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib

#########################################################################

# Utility function

def getFileMap(fname, dsname, nevts=0) :
    if os.path.isfile(fname) :
        sz = os.path.getsize(fname)
        map = { 'lfn': fname,
                'dataset': dsname,
                'size': sz,
                'events': nevts
              }
    else : 
        map = {}
    return map

#########################################################################

        
def runAthena(picklefile) :
    t0 = time.time()
  
    print "\n##################################################################"
    print   "##             ATLAS Tier-0 Alignment Processing                ##"
    print   "##################################################################\n"

    # extract parameters from pickle file
    print "Using pickled file ", picklefile, " for input parameters"
    f = open(picklefile, 'r')
    parmap = pickle.load(f)
    f.close()

    print "\nAlignment Processing run options:\n"
    pprint.pprint(parmap)

    inputfilelist = parmap.get('inputMonitoringFiles', [])
    nfiles = len(inputfilelist) 
    if not nfiles :   # problem with job definition or reading pickle file
        dt = int(time.time() - t0) 
        retcode = 1
        acronym = 'TRF_NOINPUT'
        txt = 'empty input file list'  
        outmap = { 'prodsys': { 'trfCode': retcode,
                                'trfAcronym': acronym,  
                                'jobOutputs': [],
                                'jobInputs': [],
                                'nevents': 0,
                                'more': { 'num1': 0, 'num2': dt, 'txt1': txt }
                              }
                 }
    else : 
        # assemble list of ['filename1', 'filename2', ...]
        inputdsname = ''
        svcclass = 't0atlas'

        #JAVI: Making first an auxiliary input file name list, ordering it and then removing first nLBtoSkip LB
        inputfilenamesAUX = []
        
        if isinstance(inputfilelist[0], str) :  
            inputdsname = (inputfilelist[0]).split('#')[0]
            for val in inputfilelist :
                inputfilenamesAUX.append(val.split('#')[1])

        elif isinstance(inputfilelist[0], dict) :
            inputdsname = inputfilelist[0]['dsn']
            svcclass = inputfilelist[0].get('svcclass', 't0atlas')
            for fdict in inputfilelist :
                if fdict.has_key('pfn') : 
                    # full remote path, for direct access on storage
                    if fdict['pfn'].startswith('/castor') :
                        inputfilenamesAUX.append('root://castoratlas/' + fdict['pfn'])                        
                    elif fdict['pfn'].startswith('/eos') :
                        inputfilenamesAUX.append('root://eosatlas/' + fdict['pfn'])
                    else :
                        inputfilenamesAUX.append(fdict['pfn'])
                else :
                    inputfilenamesAUX.append(fdict['lfn'])    

        inputfilenamesAUX.sort()

        nLBtoSkip = parmap.get('nLBtoSkip', 60)
        blockLBsize = parmap.get('blockLBSize', 100)
        
        # fall-back: extract iteration from input dataset name, e.g. data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0
        iterAUX = int((inputdsname.split('.')[5]).replace('Iter', ''))
        iteration = parmap.get('iteration', iterAUX) 

        #JAVI: Next lines determine how many LB have been processed per job
        #JAVI: They can come in bunches of 5 LB each
        #JAVI: The format can be something as ._lb0540 in the case of only one lb per job
        #JAVI: or as ._lb0540-lb0544 in the case of multiple lb per job, 5 in this case
        infoLB = inputfilenamesAUX[0].split('._')[1]
        jobFirstLB = int(infoLB[2:6]) #JAVI: as it is already sorted by LB
        if len(infoLB) == 6 :  
            jobLBsize = 1
        else : 
            jobLBsize = int(infoLB[-4:]) - int(infoLB[2:6]) + 1

        #JAVI: Checks if number of LB of the run (number of files * number of LB per file/job) is smaller than the threshold 
        if nfiles*jobLBsize <= nLBtoSkip :
            #JAVI: If only nLBtoSkip LB, skip run
	    dt = int(time.time() - t0) 
            retcode = 0
            acronym = 'OK'
            txt = 'only %d lb or less. too short' % (nLBtoSkip)  
            outmap = { 'prodsys': { 'trfCode': retcode,
                                    'trfAcronym': acronym,  
                                    'jobOutputs': [],
                                    'jobInputs': [],
                                    'nevents': 0,
                                    'more': { 'num1': 0, 'num2': dt, 'txt1': txt }
                                  }
                      }

        else :
            if iteration <= 1 :
                #JAVI: inputfilenames is the proper list to use, without first nLBtoSkip LB
                inputfilenames = []
                count = 0
                for val in inputfilenamesAUX :
                    if count < nLBtoSkip :
                        #JAVI: Skipping first nLBtoSkip LB
                        pass
                    else :
                        inputfilenames.append(val)
                    count += jobLBsize
            else: 
                #JAVI: iteration is 2 or 3, we need the list of lists:
                #JAVI: inputfilenames is the proper list to use, without first nLBtoSkip LB, a list of list in this case.
                nBlocks = (nfiles*jobLBsize - nLBtoSkip) // blockLBsize
                if nBlocks == 0 : nBlocks += 1
                inputfilenames = [[] for x in xrange(nBlocks)]

                count = 0
                usedLB = 0 
                for val in inputfilenamesAUX :
                    if count < nLBtoSkip :
                        #JAVI: Skipping first nLBtoSkip LB
                        pass
                    else :
                        usedblock = usedLB // blockLBsize
                        #JAVI: divide in groups of blockLBsize LB
                        if usedblock == nBlocks : usedblock -= 1
                        #JAVI: Last list solves for blockLBsize LB plus the lasting ones. 
                        inputfilenames[usedblock].append(val)
                        usedLB += jobLBsize
                    count += jobLBsize

            # output merged Monitoring file                                                                                           
            outputdsname = (parmap['outputMonitoringMergedFile']).split('#')[0]
            outputfilename = (parmap['outputMonitoringMergedFile']).split('#')[1]

            if iteration >= 2 :
                #JAVI: For the last two iterations multiple outputs are needed.
                outputfilenameBlock = []
                for x in xrange(nBlocks) : 
                    outputfilenameBlock.append(outputfilename.split('.Iter')[0] + '.Iter%d.Block%02d' % (iteration, x) + outputfilename.split('.Iter')[1][1:])

            if iteration <= 1 :
                #JAVI: Same as before, only that 60 first LB have been skipped.
                # assemble jobOptions fragment
                (s,o) = commands.getstatusoutput('rm -f filelist.txt')
                fileList = open('filelist.txt', 'w')
                for fname in inputfilenames :
        	    fileList.write(fname + '\n')
                fileList.close()

                # run athena
                cmd = ""
                if svcclass :
                    cmd += "export STAGE_SVCCLASS=%s ; " % svcclass
                cmd += "DQHistogramMerge.py filelist.txt %s True" % outputfilename
                
                print "\nRun command:"
                print cmd
                print "\nLogfile:"
                print "------------------------------------------------------------------"
                retcode = os.system(cmd)
                print "------------------------------------------------------------------"
                dt = int(time.time() - t0)
                print "\n## DQHistogramMerge.py finished with retcode = %s" % retcode
                print   "## ... elapsed time: ", dt, " sec"

                # assemble report pickle file
                outfiles = []
                infiles = []
                nevts = 0
                acronym = 'OK'
                txt = 'trf finished OK'  

                # get info for report gpickle file
                if retcode == 0 :
                    # one could determine the number of processed events here, e.g. by grep'ping in the logfile
                    fmap = getFileMap(outputfilename, outputdsname, nevts=nevts)
                    outfiles = [fmap]
                else :
                    print "ERROR: DQHistogramMerge execution problem!"
                    acronym = 'TRF_DQMERGE_EXE'
                    txt = "DQHistogramMerge.py execution problem"

                # assemble job report map
                outmap = { 'prodsys': { 'trfCode': retcode,
                                        'trfAcronym': acronym,
                                        'jobOutputs': outfiles,
                                        'jobInputs': infiles,
                                        'more': { 'num1': int(nevts), 'num2': int(dt), 'txt1': txt }
                                      }
                         }
            else:
                retcode = {}
                outfiles = []
                infiles = []
                nevts = 0
                retcode_all = 0
            
                for x in xrange(nBlocks): 
                    # assemble jobOptions fragment
                    (s,o) = commands.getstatusoutput('rm -f filelist.txt')
                    fileList = open('filelist.txt', 'w')
                    for fname in inputfilenames[x] :
            	        fileList.write(fname + '\n')
                    fileList.close()

                    # run athena
                    cmd = ""
                    if svcclass :
                        cmd += "export STAGE_SVCCLASS=%s ; " % svcclass
                    cmd += "DQHistogramMerge.py filelist.txt %s True" % outputfilenameBlock[x]
                    
                    print "\nRun command:"
                    print cmd
                    print "\nLogfile:"
                    print "------------------------------------------------------------------"
                    retcode[x] = os.system(cmd)
                    retcode_all += retcode[x]
                    print "------------------------------------------------------------------"
                    dt = int(time.time() - t0)
                    print "\n## DQHistogramMerge.py finished with retcode = %s" % retcode[x]
                    print   "## ... elapsed time: ", dt, " sec"

                    # get info for report gpickle file
                    if retcode[x] == 0 :
                        # one could determine the number of processed events here, e.g. by grep'ping in the logfile
                        fmap = getFileMap(outputfilenameBlock[x], outputdsname, nevts=nevts)
                        outfiles.append(fmap)
                if retcode_all == 0 :
                    # assemble report pickle file
                    acronym = 'OK'
                    txt = 'trf finished OK'  
                else :
                    print "ERROR: DQHistogramMerge execution problem!"
                    acronym = 'TRF_DQMERGE_EXE'
                    txt = "DQHistogramMerge.py execution problem"

                # assemble job report map
                outmap = { 'prodsys': { 'trfCode': retcode_all,
                                        'trfAcronym': acronym,
                                        'jobOutputs': outfiles,
                                        'jobInputs': infiles,
                                        'more': { 'num1': int(nevts), 'num2': int(dt), 'txt1': txt }
                                      }
                         }
 
    # pickle report map
    f = open('jobReport.gpickle', 'w')
    pickle.dump(outmap, f)
    f.close()

    print "\n##################################################################"
    print   "## End of job."
    print   "##################################################################\n"


########################################
## main()
########################################

if __name__ == "__main__" :

    if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argdict=')) :
        print "Input format wrong --- use "
        print "   --argdict=<pickled dictionary containing input info> "
        print "   with key/value pairs: "
        print "     1) 'inputMonitoringFiles': python list "
        print "          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) "
        print "        or list of file dictionaries "
        print "          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...}, "  
        print "           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...] "
        print "     2) 'outputMonitoringMergeFile': string 'datasetname#filename' "
        print "        (merged output dataset name + file) "
        print "     3) 'iteration': integer " 
        print "        (number of current iteration; optional -- if not given, will be extracted from input dataset name) "
        print " "
        sys.exit(-1)
  
    else :
      picklefile = sys.argv[1][len('--argdict='):]
      runAthena(picklefile)
