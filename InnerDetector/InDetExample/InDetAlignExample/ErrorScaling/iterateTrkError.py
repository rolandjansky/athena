#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# usage: iterateTrkError.py [options] [file1.py [file2.py [...]]]
#
# options:
#   -h, --help            show this help message and exit
#   -p FILE, --param=FILE
#                         parameter file
#   -c                    tune parameter c
#   -a                    tune parameter a
#   -n NUM, --numiter=NUM
#                         max number of iterations
#   -i NUM, --iter=NUM    first iteration number
#   -r, --rms             use rms rather than pull width
#   -t FILE, --target=FILE
#                         converge to pull width or rms in FILE.
#   -u FILE, --usentuple=FILE
#                         Use ntuple FILE for first iteration
#   -d NUM, --damping=NUM
#                         Damping factor used in C iterations, Default = 0.5
#   -s NUM, --strategy=NUM
#                         Select different strategies for convergence,
#                         0(default): Damping only applied if c2 < 0, 1: damping
#                         applied to all C iterations
#   -e OPTION, --rpoptions=OPTION
#                         Extra options for ResidualAndPulls program (eg
#                         Drifttime or NoDrifttime)
#   -k SIMULATION         input dataset is simulation

# if iter is not supplied the default will depend on the file name param 
# if it ends in iterA## or iterC## and A or C is selected it will use ## as the
# first iteration number

# TODO:
# - Check that athena and residuals program ran successfully and take appropriate action
# - More intellegent use of previous iterations to improve convergence.


# Defaults

# Set true to do a quick test that does not run the athena job. A file TrkValidation.root
# must exist
quickTest = False

nominalTag = 'IndetTrkErrorScaling_nominal'
defaultJobOptions = 'jobOptionsErrorTuning.py'
foldername = "/Indet/TrkErrorScaling"

entries = [
    ('PixPhi Barrel', 'pixbarx'),
    ('PixEta Barrel', 'pixbary'),
    ('PixPhi Endcap', 'pixecx'),
    ('PixEta Endcap', 'pixecy'),
    ('PixPhi IBL'   , 'iblx'),
    ('PixEta IBL'   , 'ibly'),
    ('SCT Barrel',    'sctbar'),
    ('SCT Endcap',    'sctec'),
    ('TRT Barrel',    'trtbar'),
    ('TRT Endcap',    'trtec'),
]


micron = 0.001

#########################################################
# intrinsic resolutions

# nominal values
pixX_sigma0 = 14 * micron
pixY_sigma0 = 115 * micron
iblX_sigma0 = 14 * micron
iblY_sigma0 = 72 * micron
sct_sigma0 = 23 * micron
trt_sigma0 = 170 * micron

#pixbarX_sigma0 = pixX_sigma0
#pixbarY_sigma0 = pixY_sigma0
#pixecX_sigma0  = pixX_sigma0
#pixecY_sigma0  = pixY_sigma0
#sctbar_sigma0  = sct_sigma0
#sctec_sigma0   = sct_sigma0
#trtbar_sigma0  = trt_sigma0
#trtec_sigma0   = trt_sigma0


# From 200GeV muon residuals with perfect alignment
pixbarX_sigma0 = 11.1 * micron
pixbarY_sigma0 = 117 * micron
pixecX_sigma0  = 13.5 * micron
pixecY_sigma0  = 118 * micron
#iblbarX_sigma0 = 14 * micron
#iblbarY_sigma0 = 72 * micron
sctbar_sigma0  = 21.0 * micron
sctec_sigma0   = 24.5 * micron
trtbar_sigma0  = 163 * micron
trtec_sigma0   = 145 * micron


sigma0 ={
    'PixPhi Barrel':  pixbarX_sigma0,
    'PixEta Barrel':  pixbarY_sigma0,
    'PixPhi Endcap':  pixecX_sigma0,
    'PixEta Endcap':  pixecY_sigma0,
    'PixPhi IBL'   :  iblX_sigma0,
    'PixEta IBL'   :  iblY_sigma0,
    'SCT Barrel':     sctbar_sigma0,
    'SCT Endcap':     sctec_sigma0,
    'TRT Barrel':     trtbar_sigma0,
    'TRT Endcap':     trtec_sigma0
}
##########################################################

import sys
import TrkErrorScalingDB

#--------------------------------------------------------------
# Get next scaling parameters.
# c_i = sqrt(a^2 * s_0^2 * (p_obs^2 - 1) + c_{i-1}^2 * p_obs^2)
# if targetp != 1 then scale p_obs by 1/targetp. This prevents it
# from trying to improve the pull more than was achieved with
# tuning parameter a.
def getNextScalingC(s, p_obs, sigma_0, targetp, damping, strategy):
    from math import sqrt

    if (targetp <= 0): targetp = 1
    p = p_obs/targetp
    a = s[0]
    c = s[1]
    cnext2 = a*a * sigma_0*sigma_0 * (p*p - 1) + c*c * p*p;
    if  (cnext2 < 0):
        cnext = damping * c
    else:
        cnext = sqrt(cnext2)
        if (strategy == 1):
            cnext = c + (cnext - c) * damping
    #print cnext
    return (a,cnext)

#--------------------------------------------------------------
# Get next parameter a. Assume c = 0.
def getNextScalingA(s, p_obs):
    a = s[0]
    anext = a * p_obs
    #print anext
    return (anext,0.)

#--------------------------------------------------------------
# cool dataset -> scaling set
def makeScaling(dataset):
    scaling = {}
    for data in dataset:
        key = data[0]
        scaling[key] = (data[1],data[2])
    return scaling

# scaling set -> cool dataset
def makeDataset(scalingset):
    dataset = []
    for entry in entries:
        key = entry[0]
        data = scalingset[key]
        dataset.append((key,data[0],data[1]))
    return dataset

#--------------------------------------------------------------
def createNominalScaling():
    dataset = []
    for entry in entries:
        key = entry[0]
        a = 1.0
        c = 0.0
        dataset.append((key,a,c))
    return (nominalTag,dataset)

#--------------------------------------------------------------
def printDataset(file,dataset):
    file.write('dataset = [')
    first = True
    for data in dataset:
        if (not first):
            file.write(',\n           ')
        file.write(str(data))
        first = False
    file.write('\n           ]\n')
    
#--------------------------------------------------------------
def addToDatabase(dataset, tag, mc):
    #if (not quickTest):
    #comp with data, ofl with MC
    
    if not mc:
        connect="sqlite://X;schema=mycool.db;dbname=CONDBR2"  #COMP200
    else:
        connect="sqlite://X;schema=mycool.db;dbname=OFLP200"
    TrkErrorScalingDB.make(dataset, tag, foldername, connect)
    #TrkErrorScalingDB.make(dataset, tag, foldername)
    
#--------------------------------------------------------------
def printResults(outfile, tag, sigma0, targetpull, pulls, scalingset, extraRPOptions=None, rmsoption=False):
    outfile.write('Tag: '+tag+'\n')
    if (extraRPOptions == ""): extraRPOptions="None"
    if (extraRPOptions is None): extraRPOptions="None"
    outfile.write('Use RMS: '+str(rmsoption)+'\n')
    outfile.write('Extra options: '+extraRPOptions+'\n')
    outfile.write("%-15s %10s %10s %10s %10s %10s %10s %10s %10s\n" % ('Det', 'sigma0', 'target', 'Pull','Pull_err','RMS','RMS_err','A','C'))
    for entry in entries:
        key = entry[0]
        keypull = entry[1]
        targetp = getTargetPull(keypull, targetpull, rmsoption)           
        outfile.write("%-15s %10g %10g %10g %10g %10g %10g %10g %10g\n" %
                      (key, sigma0[key], targetp,
                       pulls[keypull][0], pulls[keypull][1],
                       pulls[keypull][2], pulls[keypull][3],
                       scalingset[key][0], scalingset[key][1]))

def printScaling(outfile, tag, scalingset):
    outfile.write('Tag: '+tag+'\n')
    outfile.write("%-15s %10s %10s\n" % ('Det','A','C'))
    for entry in entries:
        key = entry[0]
        keypull = entry[1]
        outfile.write("%-15s %10g %10g\n" %
                      (key, scalingset[key][0], scalingset[key][1]))


#--------------------------------------------------------------
def runAthena(jobOptions,tag, logfile, batchmode, itertag):
    import os
    import sys
    from subprocess import Popen

    if not batchmode:
        commandstr = ('athena.py -c "runmode=\'local\'; errorScalingOverride = \''
                      + tag + '\'" ' + jobOptions + ' | tee ' + logfile)
        print "Executing:",commandstr
        sys.stdout.flush()
        if (not quickTest):
            p = Popen(commandstr, shell=True)
            rc = os.waitpid(p.pid, 0)[1]
        else:
            rc = 0
            return rc
    else:
        #-n is number of segments, -t is the iter tag
        # the errorScalingOverride='tag' line must NOT have any spaces or extra quotes 
        # and must come before the jobOptions
        commandstr = ( 'python run_RecEx.py -n40 -t %s ' % itertag +
                       ("--estag %s" % tag)
                       + ' ' + jobOptions )
                       

        print "ExecutingBen:",commandstr
        sys.stdout.flush()
        p = Popen(commandstr, shell=True)
        rc = os.waitpid(p.pid, 0)[1]
        #print rc

#--------------------------------------------------------------
def readPullsFile(filename):                              
   infile = open(filename)
   lines = infile.readlines()
   pulls = {}
   for l in lines:
       words = l.split()
       pulls[words[0]] = [float(words[1]),float(words[2]),float(words[3]),float(words[4])]
   #print pulls
   infile.close()
    
   return pulls

#--------------------------------------------------------------
# Get the target pull width or rms for convergence 
def getTargetPull(key, targetpull, rmsoption):
    targetp = 1
    if (targetpull != None and key in targetpull) :
        targetp = targetpull[key][0]
        if (rmsoption): targetp = targetpull[key][2]
    #if (targetp < 1): targetp = 1
    return targetp

#--------------------------------------------------------------
def iterStr(num):
    return str(num).zfill(2)

#--------------------------------------------------------------
def doIteration(jobOptions, tag, prevDataset, itercount, doAorC, targetpull, rms, usentuple, damping, strategy, extraRPOptions, batchmode,mc):    
    print "-----------------------------------------------------------------"
    print "Running iteration:", itercount
    print " tag =", tag
    print " Tuning parameter", doAorC
    print 
    
    pullsfilename = 'pulls_iter'+doAorC+iterStr(itercount)+'.txt'
    #pullsfile = 'pulls.txt'
    monitoringfile = 'monitoring_iter'+doAorC+iterStr(itercount)+'.root'
    paramfile = 'param_iter'+doAorC+iterStr(itercount+1)+'.py'
    nexttag = 'IndetTrkErrorScaling_iter'+doAorC+iterStr(itercount+1)
    logfile = 'out_iter'+doAorC+iterStr(itercount)+'.log'
    resultsfilename = 'results_iter'+doAorC+iterStr(itercount)+'.txt'

    import os
    import os.path
    from subprocess import Popen

    if not (doAorC == 'A' or doAorC == 'C') :
        doAorC = 'C'

        
    resultsfile = open(resultsfilename,'w')
    resultsfile.write('---------------------------------------------------------------------------------------------------------\n')
    resultsfile.write("Iteration: "+str(itercount)+'\n')
    resultsfile.write("Tuning parameter "+doAorC+'\n')
    resultsfile.write("\n")

    prevScalingSet = makeScaling(prevDataset)
    printScaling(sys.stdout, tag, prevScalingSet)
    printScaling(resultsfile, tag, prevScalingSet)
    #print prevScalingSet


    print "Is  this mc ? : ", mc
    addToDatabase(prevDataset, tag, mc) 


    if (not usentuple):
        runAthena(jobOptions, tag, logfile, batchmode,
                  itertag=('iter' + doAorC + iterStr(itercount)))
    else:
        print "Athena not run. Using ntuple file:", usentuple
    
    if (usentuple):
        monitoringfile = usentuple 
    elif (quickTest):
        monitoringfile = "monitoring.root"
    
    
    # rename ntuple output file
    if (not quickTest and not usentuple):
        os.rename('monitoring.root',monitoringfile)

    #commandstr = './AllResidualsAndPulls -b '+ntuplebasename+' Validation Tracks 65 2000 250 ' + extraRPOptions
    #commandstr = './AllResidualsAndPulls -b '+ntuplebasename+' Validation CombinedInDetTracks 65 2000 250 Drifttime ' + extraRPOptions
    commandstr = 'python generatePulls.py ' + monitoringfile + ' iter' + doAorC+iterStr(itercount) + ' "Fully Unbiased"'
    print "Executing:",commandstr
    sys.stdout.flush()
    p = Popen(commandstr, shell=True)
    rc = os.waitpid(p.pid, 0)[1]

    os.rename('pulls.txt',pullsfilename)
        
    #rc = 0
    #print 'return status: ',rc

    pulls = {}
    pulls = readPullsFile(pullsfilename)

    nextScalingSet = {}
    for entry in entries:
        key = entry[0]
        pullkey = entry[1]
        p = pulls[pullkey]
        p_obs = p[0]
        p_err = p[1]
        if (rms): # use rms instead
            p_obs = p[2]
            p_err = p[3]
            
        nextScaling  = prevScalingSet[key]

        targetp = getTargetPull(pullkey, targetpull, rms)           

        if (doAorC == 'A'):
            nextScaling = getNextScalingA(nextScaling, p_obs)
        else:
            nextScaling = getNextScalingC(nextScaling, p_obs, sigma0[key], targetp, damping, strategy)
        #print nextScaling
        nextScalingSet[key] = nextScaling

    printResults(sys.stdout,  tag, sigma0, targetpull, pulls, nextScalingSet, extraRPOptions, rms)
    printResults(resultsfile, tag, sigma0, targetpull, pulls, nextScalingSet, extraRPOptions, rms)
    print 'Next tag:',nexttag
    resultsfile.write('Next tag: '+nexttag+'\n')
    resultsfile.write('---------------------------------------------------------------------------------------------------------\n')
    resultsfile.write('\n')

    #print nextScalingSet
    dataset = makeDataset(nextScalingSet)
    print 'Writing parameters for next iteration:',paramfile
    outfile = open(paramfile,'w')
    outfile.write('tag = \''+nexttag+'\'\n')
    printDataset(outfile,dataset)
    outfile.close()
    
    resultsfile.close()

    # signal to continue iterations.
    # Placeholder to allow the possibility to abort iterations
    more = True

    return (more, nexttag, dataset)
#--------------------------------------------------------------
def parseOptions():

    from optparse import OptionParser

    usage = "usage: %prog [options] [file1.py [file2.py [...]]] "
    parser = OptionParser(usage=usage)
    parser.add_option("-p", "--param", dest="paramfile",
                      help="parameter file", metavar="FILE")
    parser.add_option("-c", "",
                      action="store_true", dest="tunec", default=False,
                      help="tune parameter c")
    parser.add_option("-a", "",
                      action="store_true", dest="tunea", default=False,
                      help="tune parameter a")
    parser.add_option("-n", "--numiter", dest="numIter", type="int",
                      default = 1,
                      help="max number of iterations", metavar="NUM")
    parser.add_option("-i", "--iter", dest="firstIter", type="int",
                      help="first iteration number", metavar="NUM")
    parser.add_option("-r", "--rms",
                      action="store_true", dest="rms", default=False,
                      help="use rms rather than pull width")
    parser.add_option("-t", "--target", dest="pullsfile", 
                      help="converge to pull width or rms in FILE.",
                      metavar="FILE")
    parser.add_option("-u", "--usentuple", dest="ntuplefile", 
                      help="Use ntuple FILE for first iteration",
                      metavar="FILE")   
    parser.add_option("-d", "--damping", dest="damping", type="float",
                      default = 0.5,
                      help="Damping factor used in C iterations, Default = %default",
                      metavar="NUM")
    parser.add_option("-s", "--strategy", dest="strategy",  type="int",
                      default = 0,
                      help="Select different strategies for convergence, "
                      "0(default): Damping only applied if c2 < 0, "
                      "1: damping applied to all C iterations", 
                      metavar="NUM")
    parser.add_option("-e", "--rpoptions", dest="rpoptions",
                      help="Extra options for ResidualAndPulls program (eg Drifttime or NoDrifttime)",
                      metavar="OPTION")
    parser.add_option('-b', '--batch', dest='batch', help='run on the batch system',
                      action='store_true', default=False)
    parser.add_option("-k", "",
                      action="store_true", dest="simul", default=False,
                      help="input dataset is simulation")
    parser.add_option("-q", "--queue", dest="queue", default="atlasb1",
                      help="queue for job submission")
    

   
    (options, args) = parser.parse_args()

    if options.tunea and options.tunec:
        parser.error("options -a and -c are mutually exclusive")
    if (options.strategy <0 or  options.strategy > 1):
        parser.error("-s,--strategy option: value out of range")
    return (options, args)


#--------------------------------------------------------------
def getIterAndParam(s):
    #Gets iteration number and tuning parameter from filename assuming it is
    #of the form param_iter[A,C]##.py

    #Look for the string "iter"
    tuneParam = None
    iter = None

    if (s != None):
        i = s.find('iter')
        if (i >= 0):
            i += 4
            if (i < len(s)):
                nextChar = s[i]
                if (s[i] == 'A' or s[i] == 'C'):
                    tuneParam = s[i] 
                    i += 1
                ifirst = i
                while (i < len(s) and s[i].isdigit()): i += 1
                if (i - ifirst > 0): iter = int(s[ifirst:i])

    return (tuneParam, iter)

############
##MainBody##
############ 

(options, args) = parseOptions()

print "Running Errorscaling iterations"

if (options.paramfile == None) :
    tag,dataset = createNominalScaling()
else:
    print "Reading parameter file:", options.paramfile
    execfile(options.paramfile)
    
nextDataset = dataset
nextTag = tag


# use the paramfile name to get tuning parameter and iteration number
(tuneParam, firstIter) = getIterAndParam(options.paramfile)

jobOptions = ''
if (len(args) == 0):
    jobOptions = defaultJobOptions
else :
    for j in args:
        jobOptions += j + ' '

        
doA = 'A'
doC = 'C'

# Set tuneParam. If requested tuning paramater differs from
# that determined from paramfile then reset firstIter to 0.
if (options.tunea):
    if (tuneParam == 'C'): firstIter = 0
    tuneParam = doA
if (options.tunec): 
    if (tuneParam == 'A'): firstIter = 0
    tuneParam = doC

if (tuneParam == None):
    print "Tuning parameter not specified. Tuning parameter A" 
    tuneParam = doA

targetpull = None
if (tuneParam == doA and options.pullsfile != None):
    print "WARNING pullsfile is ignored if tuning parameter A" 
elif (options.pullsfile):
    targetpull = readPullsFile(options.pullsfile)
 
if (options.firstIter != None): firstIter = options.firstIter 
if (firstIter == None): firstIter = 0 
numIter = options.numIter

usentuple = options.ntuplefile
rpoptions = options.rpoptions
batch = options.batch
if (rpoptions is None): rpoptions = ""

print "First Iteration:", firstIter
print "Number of iterations:", numIter 
print "JobOptions file(s):", jobOptions
print "Tuning parameter:", tuneParam
print "Use RMS:", options.rms
print "Use ntuple in first iteration:", usentuple
print "Target pullsfile:", options.pullsfile
print "Damping factor:",  options.damping
print "Strategy:",options.strategy
print "Extra Residual and Pulls options:", options.rpoptions
print "Is MC : ", options.simul
print "queue: ",options.queue


for i in range(firstIter,firstIter+numIter):
    print 'Iteration:', i
    if (i != firstIter): usentuple = None
    more,nextTag,nextDataset = doIteration(jobOptions = jobOptions,
                                           tag = nextTag,
                                           prevDataset = nextDataset,
                                           itercount = i,
                                           doAorC = tuneParam,
                                           targetpull = targetpull,
                                           rms = options.rms,
                                           usentuple = usentuple,
                                           damping = options.damping,
                                           strategy = options.strategy,
                                           extraRPOptions = rpoptions,
                                           batchmode = options.batch,
                                           mc=options.simul)
                                           #queue = options.queue)


    if (not more):
        print "Remaining iterations canceled."
        break


