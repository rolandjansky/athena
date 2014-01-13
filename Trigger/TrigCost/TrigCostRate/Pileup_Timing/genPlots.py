#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from ROOT           import *
from ROOT           import gStyle
from optparse       import OptionParser
import os,commands
import sys
import re
import pickle
import copy
###########################################
# How to run this code:
#
# For data:
#   A pickle file can be created if desired. This way the mu values do not repeatable need
#   to be looked up in COOL
#
#   > python genPlots.py -i ../../TrigCostPython/macros/. -l EF  --isData --readPickle ../../TrigCostPython/macros/PileupEF.pickle 
#
# For MC:
#  python genPlots.py -i . -l EF 
#
#
###########################################

#
# Parse command line options
#   
from optparse import OptionParser
p = OptionParser()
p.add_option( '-l',                type="string",       default="L2",         dest="lvl",        help="Trigger level L2 or EF")
p.add_option( '-i', '--input',     type="string",       default="/tmp/schae", dest="input",      help="input directory for cost root files")
p.add_option( '-k',                type="string",       default="cost_mu",    dest="filter",     help="filter input files with this string")
p.add_option( '--readPickle',      type="string",       default=None,         dest="readPickle", help="Read run mu pickle")
                                   
p.add_option( '-s', '--save',      action='store_true', default=False,        dest="save",       help="save the histograms as pdf")
p.add_option( '--local',           action='store_true', default=False,        dest="local",      help="local running")
p.add_option( '--statErr',         action='store_true', default=False,        dest="statErr",    help="Stats only ERROR")
p.add_option( '--lxplus',          action='store_true', default=False,        dest="lxplus",     help="lxplus running")
p.add_option( '--wait',            action='store_true', default=False,        dest="wait",       help="wait on canvas")
p.add_option('-d', '--debug',      action="store_true", default=False,                           help="Run in debug mode")
p.add_option('-q', '--quiet',      action="store_true", default=False,                           help="Run in quiet mode (suppresses chains with zero rates)")
p.add_option( '--isData',          action='store_true', default=False,        dest="isData",     help="This is data so look up the mu in COOL")
p.add_option( '--makePickle',      action='store_true', default=False,        dest="makePickle", help="Make run mu pickle")
p.add_option( '--rmOutliers',      action='store_true', default=False,        dest="rmOutliers", help="Remove the outliers from th plots")
p.add_option( '--NoiseSuppress',   action='store_true', default=False,        dest="NoiseSuppress", help="Noise suppress bins with error greater than 0.5")

(options, args) = p.parse_args()

if options.local:
    ROOT.gROOT.LoadMacro('/Users/schaefer/root/style/atlasstyle-00-03-04/AtlasStyle.C')
    ROOT.gROOT.LoadMacro('/Users/schaefer/root/style/atlasstyle-00-03-04/AtlasUtils.C')
    ROOT.gROOT.LoadMacro('/Users/schaefer/root/style/atlasstyle-00-03-04/AtlasLabels.C')
    SetAtlasStyle();
if options.lxplus:
    ROOT.gROOT.LoadMacro('./style/AtlasStyle.C')
    ROOT.gROOT.LoadMacro('./style/AtlasUtils.C')
    ROOT.gROOT.LoadMacro('./style/AtlasLabels.C')
    SetAtlasStyle();

LOWER_BIN=0.0
UPPER_BIN=40.0
tmu=[]
fmu=[]
mus=[]
# If it is data, we need these variables too
lbSet      ={}
inputRuns  ={}
runsToRead ={}
fLevel=options.lvl

if fLevel=='L2':
    #runsToRead=[186156,189536,189561,190120,190119,189598,189372,189425] # for L2
    #runsToRead={186156 : None}
    #runsToRead={190256  : ['cost_sfo4_190256.root','cost_sfo9_190256.root','cost_sfo10_190256.root','cost_sfo11_190256.root','cost_sfo12_190256.root']}
    runsToRead={201289 : ['cost_BegLB200-EndLB300-SFO-4_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200-SFO-1_0201289_L2CostMonitoring.root',  'cost_BegLB400-EndLB500-SFO-1_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200-SFO-2_0201289_L2CostMonitoring.root',  'cost_BegLB400-EndLB500-SFO-2_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200-SFO-3_0201289_L2CostMonitoring.root',  'cost_BegLB400-EndLB500-SFO-3_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200-SFO-4_0201289_L2CostMonitoring.root',  'cost_BegLB400-EndLB500-SFO-4_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200-SFO-5_0201289_L2CostMonitoring.root',  'cost_BegLB400-EndLB500-SFO-6_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200-SFO-6_0201289_L2CostMonitoring.root',  'cost_BegLB400-EndLB500_0201289_L2CostMonitoring.root',
    'cost_BegLB100-EndLB200_0201289_L2CostMonitoring.root',        'cost_BegLB500-EndLB600-SFO-1_0201289_L2CostMonitoring.root',
    'cost_BegLB200-EndLB300-SFO-1_0201289_L2CostMonitoring.root',  'cost_BegLB500-EndLB600-SFO-3_0201289_L2CostMonitoring.root',
                          #'cost_BegLB200-EndLB300-SFO-4_0201289_L2CostMonitoring.root',
                          'cost_BegLB500-EndLB600-SFO-4_0201289_L2CostMonitoring.root',
    'cost_BegLB200-EndLB300-SFO-5_0201289_L2CostMonitoring.root',  'cost_BegLB500-EndLB600-SFO-6_0201289_L2CostMonitoring.root',
    'cost_BegLB200-EndLB300-SFO-6_0201289_L2CostMonitoring.root',  'cost_BegLB500-EndLB600_0201289_L2CostMonitoring.root',
    'cost_BegLB200-EndLB300_0201289_L2CostMonitoring.root',        'cost_BegLB600-EndLB700-SFO-1_0201289_L2CostMonitoring.root',
    'cost_BegLB300-EndLB400-SFO-1_0201289_L2CostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-2_0201289_L2CostMonitoring.root',
    'cost_BegLB300-EndLB400-SFO-2_0201289_L2CostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-3_0201289_L2CostMonitoring.root',
    'cost_BegLB300-EndLB400-SFO-3_0201289_L2CostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-4_0201289_L2CostMonitoring.root',
    'cost_BegLB300-EndLB400-SFO-5_0201289_L2CostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-5_0201289_L2CostMonitoring.root',
    'cost_BegLB300-EndLB400-SFO-6_0201289_L2CostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-6_0201289_L2CostMonitoring.root',
    'cost_BegLB300-EndLB400_0201289_L2CostMonitoring.root',
    
        ],
                }
if fLevel=='EF':
    #runsToRead=[186156,190120,190119,189372,189242] # for EF - 189598,189561
    runsToRead={201289 : ['cost_BegLB200-EndLB300-SFO-4_0201289_EFCostMonitoring.root',
        'cost_BegLB100-EndLB200-SFO-6_0201289_EFCostMonitoring.root',  'cost_BegLB500-EndLB600-SFO-1_0201289_EFCostMonitoring.root',
        'cost_BegLB100-EndLB200_0201289_EFCostMonitoring.root',       'cost_BegLB500-EndLB600-SFO-2_0201289_EFCostMonitoring.root',
        'cost_BegLB200-EndLB300-SFO-2_0201289_EFCostMonitoring.root',  'cost_BegLB500-EndLB600-SFO-3_0201289_EFCostMonitoring.root',
        #'cost_BegLB200-EndLB300-SFO-4_0201289_EFCostMonitoring.root',
        'cost_BegLB500-EndLB600-SFO-4_0201289_EFCostMonitoring.root',
        'cost_BegLB200-EndLB300_0201289_EFCostMonitoring.root      ',  'cost_BegLB500-EndLB600-SFO-5_0201289_EFCostMonitoring.root',
        'cost_BegLB300-EndLB400-SFO-1_0201289_EFCostMonitoring.root',  'cost_BegLB500-EndLB600_0201289_EFCostMonitoring.root',
        'cost_BegLB300-EndLB400-SFO-2_0201289_EFCostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-1_0201289_EFCostMonitoring.root',
        'cost_BegLB300-EndLB400-SFO-6_0201289_EFCostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-2_0201289_EFCostMonitoring.root',
        'cost_BegLB300-EndLB400_0201289_EFCostMonitoring.root',        'cost_BegLB600-EndLB700-SFO-3_0201289_EFCostMonitoring.root',
        'cost_BegLB400-EndLB500-SFO-1_0201289_EFCostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-4_0201289_EFCostMonitoring.root',
        'cost_BegLB400-EndLB500-SFO-2_0201289_EFCostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-5_0201289_EFCostMonitoring.root',
        'cost_BegLB400-EndLB500-SFO-3_0201289_EFCostMonitoring.root',  'cost_BegLB600-EndLB700-SFO-6_0201289_EFCostMonitoring.root',
        'cost_BegLB400-EndLB500-SFO-5_0201289_EFCostMonitoring.root',  'cost_BegLB600-EndLB700_0201289_EFCostMonitoring.root',
        'cost_BegLB400-EndLB500_0201289_EFCostMonitoring.root',
        ],
                
                } # for EF - 189598,189561


# Finding input files
inputDir=os.listdir(options.input)
for f in inputDir:
    if f.count(options.filter) and f.count(".root"):
        fmu.append(f)
# check that a least one root file exists
if not len(f):
    print "Could not find any input files with filter: "+options.filter
    os.sys.exit(0)
    
if options.isData:
    from TrigCostPython import TrigCostAnalysis,TrigCostCool
    #is data - need to look up in cool for the mu values
    if (options.readPickle==None):

        opts=[""]
        if options.debug: opts.append('debug')
        if options.quiet: opts.append('quiet')
        
        lbList=[]
        # map runs to LBs with mu data
        for run,file_name in runsToRead.iteritems():
            print 'run:',run
            lbList.append(TrigCostCool.GetLumiblocks(run,1,1200,opts));
        # compile the dictionary
        lbSet=dict(zip(runsToRead.keys(), lbList));
    else:
        # Reading in pickle file with mu
        if not os.path.exists(options.readPickle):
            print 'ERROR Pickle file does not exist'
            sys.exit(0)
        print 'Reading Pickle File: ',options.readPickle
        pfile = open(options.readPickle,'r')
        pickleObjects = pickle.load(pfile)
        pfile.close()
        lbSet = pickleObjects
        
    if options.makePickle:
        pickleObjects =  lbSet 
        pfile = open('Pileup'+fLevel+'.pickle','w')
        pickle.dump(pickleObjects,pfile)
        pfile.close()

    #Load Root Files
    for run,file_list in runsToRead.iteritems():
        if file_list == None:
            newFile=TFile(options.input.rstrip('/')+'/'+'cost_0'+str(run)+'_'+fLevel+'CostMonitoring.root');
            if newFile.IsZombie():
                print 'First file must be valid'
                sys.exit(0)
            tmu.append(newFile);
            inputRuns[run] = [newFile]
        else:
            InsertFileList = []
            for file_name in file_list:
                
                if not os.path.exists(options.input.rstrip('/')+'/'+file_name):
                    print 'Could not find file'
                newFile=TFile(options.input.rstrip('/')+'/'+file_name);
                if newFile.IsZombie():
                    print 'ERROR could not load: ',file_name
                    continue
                tmu.append(newFile);
                InsertFileList += [newFile]
            # Load the TFile list into the inputRuns list
            inputRuns[run] = InsertFileList

else:
    # is mc
    for f in fmu:
        print f
        mu=re.search("\d\d*",f).group(0)
        if mu.isdigit():
            mus.append(float(mu))
        # open root files        
        tmu.append(TFile(options.input.rstrip('/')+'/'+f))

# Set limits for plots
muMax=0.0
muMin=1.0e5
if options.isData:
    for run,lbs in lbSet.iteritems():
        for i in range(lbs.FirstLB(),lbs.LastLB()):
            if False:
                print 'muRead:',lbs.GetMeanPileUp(i)
                sys.stdout.flush()
                
            if muMax<lbs.GetMeanPileUp(i):
                muMax=lbs.GetMeanPileUp(i)
    muMax+=1
    muMax=float(int(muMax))
else:
    muMin=min(mus)
    muMax=max(mus)

print 'Max mu:',muMax
if len(mus) != len(fmu) and not options.isData:
    print "ERROR could not find mu for all files!"
    sys.exit(0)
#print "mus: ",mus
#for i in range(0,len(mus)):
#    if not fmu[i].count(str(mus[i])):
#        print "ERROR mu does not match the file!!!"
#        sys.exit(0)

c1 = TCanvas("c1", "c1", 1000, 500);

timingPlotsL2=["TrigIDSCAN",
               "TrigSiTrack",
               "Hypo",
               "L2BMuMuFex",
               "Dummy",
               "L2TrkMassFex",
               "LArL2ROBListWriter",
               "TrigTRTSegFinder",
               "muComb",
               "Cosmic",
               "nroi_event",
               "nroi_call",
               "evt_time",
               "alg_time_per_roi"]

#timingPlotsL2=["TrigIDSCAN_Muon",
#               "TrigIDSCAN_muonIso",
#               "TrigIDSCAN_Electron",
#               "TrigIDSCAN_eGamma",
#               "TrigIDSCAN_Bphysics",
#               "TrigIDSCAN_FullScan", #should remove
#               "TrigIDSCAN_Jet", # should remove
#               "TrigIDSCAN_Tau", # should remove
#               "TrigSiTrack_Tau_robust",
#               "TrigSiTrack_Tau",
#               "TrigSiTrack_Jet",
#               "nroi_event",
#               "nroi_call",
#               "evt_time",
#               "alg_time_per_roi"]

timingPlotsEF=["SiTrigTrackFinder",
               "TrigAmbiguitySolver",
               "SiTrigSpacePointFinder",
               "TRTTrackExtAlg",
               "TrigExtProcessor",
               "TrigVxPrimary",
               "TRTDriftCircleMaker",
               "SCTClustering",
               "PixuleClustering",
               "nroi_event",
               "nroi_call",
               "evt_time",
               "alg_time_per_roi"]

def getPlot(rfile,name,level=options.lvl,table='tableRoI'+options.lvl):
    folder = rfile.Get('exec'+level).Get(table)
    if folder:
        plot=folder.Get(name)
        return plot
    return 0

def getPlotMean(rfile,name,level=options.lvl,table='tableRoI'+options.lvl):
    folder=rfile.Get('exec'+level).Get(table)
    if folder:
        plot=folder.Get(name)
        if plot:
            return plot.GetMean(1)
    print "Could not find histogram ",name
    return 0

def getPlotErr(rfile,name,level=options.lvl,table='tableRoI'+options.lvl):
    folder=rfile.Get('exec'+level).Get(table)
    if folder:
        plot=folder.Get(name)
        if plot:
            if plot.Integral():
                return plot.GetMean(1)*1.0/TMath.Sqrt(plot.Integral())
    return 0

def getEvtMean(rfile,name,level):
    plot=rfile.Get('exec'+level).Get('tableEventPlot'+level).Get(name)
    return plot.GetMean(1)

def getEvtErr(rfile,name,level):
    plot=rfile.Get('exec'+level).Get('tableEventPlot'+level).Get(name)
    return plot.GetMean(1)*1.0/TMath.Sqrt(plot.Integral())

def getKey(rfile,name):
    key=rfile.Get(name)
    return key

def fillPlot(name,title,level=options.lvl,table='tableRoI'+options.lvl,algname=""):

    newname=name

    if name.count('_roi'): # and not name.count('time'):
        if getPlot(tmu[0],name,table=table).GetTitle():
            newname='mc_roi_algtime_'+getPlot(tmu[0],name,table=table).GetTitle()
        else:
            newname='mc_roi_algtime_'+algname
        print "alg name",newname

    else:
        newname='mc_'+name+'_'+algname

    plot = TH1F( newname+'_vs_mu', title, int(muMax-muMin)+1 , muMin-0.001, muMax+1)
    for i in range(0,len(tmu)):
        plot.SetBinContent(int(mus[i])+1,getPlotMean(tmu[i],  name,table=table))
        plot.SetBinError(int(mus[i])+1,getPlotErr(tmu[i],  name,table=table))

    plot.Draw()
    plot.Fit('pol2','W')
    aplt = getPlot(tmu[0],name,table=table)
    plot.SetTitle(algname)
    plot.SetYTitle('Algorithm Time (ms)/ROI')
    plot.SetXTitle('Mean Number of Pileup Events #mu')

    for chn in timingPlotsL2:
        if newname.count(chn):
            if options.save:
                c1.SaveAs('del_'+level+newname+'.pdf')

    for chn in timingPlotsEF:
        if newname.count(chn):
            if options.save:
                c1.SaveAs('del_'+level+newname+'.pdf')

    return plot

#
#Calculate mean and standard deviation of data x[]:
#    mean = {\sum_i x_i \over n}
#    std = sqrt(\sum_i (x_i - mean)^2 \over n-1)
#
def meanstdv(x):
    n, mean, std = len(x), 0.0, 0.0
    if n==0:
        return mean, std
    
    for a in x:
        mean = mean + a
        
    mean = mean / float(n)
    if n<1.5:
        std  = 0.2*mean
        return mean, std
    
    for a in x:
        std = std + (a - mean)**2
    std = TMath.Sqrt(std / float(n-1))
    return mean, std

def median(x):
    xs = sorted(x)
    if len(x)==0:
        return 0.0
    if len(x)%2==1:
        return xs[(len(xs)+1)/2-1]
    else:
        lower = xs[len(xs)/2-1]
        upper = xs[len(xs)/2]

        return (float(lower + upper)) / 2.0

def GetOnlinePlot(fFile,hist,alg_name,table='tableAlgL2'):
    if alg_name=='':
        return fFile.Get('exec'+fLevel).Get(table).Get(hist)
    return fFile.Get('exec'+fLevel).Get(table).Get(alg_name).Get(hist)
    
def FillOnlinePlot(fFile,hist,alg_name,title,vsMu,run,nEntry,table):

    if False:
        print 'algName: ',alg_name
    

    # Collect histogram for Algorithm alg_name and trigger level
    mu_plot=GetOnlinePlot(fFile,hist,alg_name,table);

    # Create timing and error variables
    lb   = 0;
    time = 0.0;
    err  = 0.0;
    err_stat = 0.0;
    ent_stat = [];
    times=[]
    if not mu_plot:
        return vsMu

    rms_err = mu_plot.GetRMS()/TMath.Sqrt(mu_plot.GetNbinsX())
    lb=0
    if False:
        print "number of X bins:",mu_plot.GetNbinsX()
        print "entries: ",len(nEntry)
        sys.stdout.flush()

    NLBS_avg=15
    if run==190728:
        NLBS_avg=15
    while lb<mu_plot.GetNbinsX():
        
        time_lb=mu_plot.GetBinContent(lb)
        if mu_plot.GetBinError(lb)!=0.0:
            err_stat+=TMath.pow(time_lb/mu_plot.GetBinError(lb),2.0)
            #ent_stat+=TMath.pow(time_lb/mu_plot.GetBinError(lb),2.0)
            ent_stat+=[[time_lb,mu_plot.GetBinError(lb)]]
        if time_lb==0.0:
            lb=1+lb;
            continue
        times+=[time_lb]
        if lb%NLBS_avg !=0:
            if time_lb < (.6*time/float(lb%NLBS_avg)):
                time_lb=time/float(lb%NLBS_avg)
        time=time_lb+time;
        if False:
            print 'Time: ',time
            sys.stdout.flush()
        # Averaging over 5 lbs
        nbins_time=0.0
        if (lb%NLBS_avg==0 and lb != 0): # or lb+1==mu_plot.GetNbinsX():

            if time==0:
                lb=1+lb;
                continue
            nbins_time+=1.0

            #Collect the mu
            mu=lbSet[run].GetMeanPileUp(lb+int(mu_plot.GetBinLowEdge(0)))
            #
            # Hack for this run because the mu was miscalculated by a factor of 2
            #
            if run==190728:
                mu*=2.0
            if False:
                print 'mu:',mu
                sys.stdout.flush()
            
            bin=int(800.0/(UPPER_BIN-LOWER_BIN)*mu)
            old_time=vsMu.GetBinContent(bin);
            old_err=vsMu.GetBinError(bin)*TMath.Sqrt(nEntry[lb]);
            old_err_stat=vsMu.GetBinError(bin)

            if old_err_stat!=0.0:
                #err_stat+=TMath.pow(old_time/old_err_stat,2.0)
                err_stat+=old_err_stat*old_err_stat*nEntry[lb]
            time,err=meanstdv(times)
            err=rms_err
            if old_time>0.0:
                time=(old_time*nEntry[lb]+time)/(nEntry[lb]+1.0)
                if err ==0.0:
                    err = old_err
                elif old_err == 0.0:
                    err=err
                else:
                    err=1.0/(TMath.Sqrt(nEntry[lb])*1.0/err+1.0/old_err)/TMath.Sqrt(nEntry[lb]+1.0)
                    #err=TMath.Sqrt((err*err+old_err*old_err*nEntry[lb]*nEntry[lb])/(nEntry[lb]+1.0)/(nEntry[lb]+1.0))
                #err=err/(TMath.Sqrt(nEntry[lb]+1.0))
            vsMu.SetBinContent(bin,time/nbins_time);

            # compute stat error
            if err_stat==0.0:
                lb=1+lb;
                print 'ERROR entries equal to zero'
                continue
            if options.statErr:
                thistot=0.0
                thiserr=0.0
                thisn=0.0
                for li in ent_stat:
                    if len(li)!=2:
                        continue
                    tval=li[0]
                    terr=li[1]
                    tn=0.
                    if terr!=0.0:
                        tn = tval/terr
                    thistot+=tn*tval
                    thiserr+=tn*terr*terr
                    thisn  +=tn

                if old_err_stat!=0.0:
                    old_tn=old_time/old_err_stat
                    if thisn+old_tn!=0.0:
                        time=(old_time*old_tn+thistot)/(thisn+old_tn)
                        err=TMath.Sqrt((thiserr+old_tn*old_err_stat*old_err_stat)/(thisn+old_tn))
                    else:
                        time=0.0
                        err=0.0
                else:
                    if thisn!=0.0:
                        time=thistot/thisn
                        err=TMath.Sqrt(thiserr/thisn)
                    else:
                        time=0.0
                        err=0.0
                #err = time/nbins_time/TMath.Sqrt(err_stat)
            
            vsMu.SetBinError(bin,err);
            if lb<len(nEntry):
                nEntry[lb]=1.0+nEntry[lb];
            else:
                print "Exceeded entry size ",lb
            # Reset variables
            time = 0.0;
            err  = 0.0;
            err_stat=0.0
            times= [];
            nbins_time=0.0;
            
        lb=1+lb;

    return vsMu;

def LoopOverDataRuns(execLevel='execL2',tableType='surveyLBL2',plt_name='',type_name=''):
     nEntry=[]
     type_plt=TH1F('data_'+plt_name+type_name,'Algorithm Time '+type_name,800,LOWER_BIN,UPPER_BIN);
     i=0
     while i<1250:
         nEntry.append(0.0)
         i=i+1
     for run,file_list in inputRuns.iteritems():
         for tfile in file_list:

             if tfile == None:
                 print 'Missing root file for run: ',run
                 continue
             #Check that plot is in file
             n_algPlots=tfile.Get(execLevel).Get(tableType);
             if type_name!='' and (type_name in n_algPlots.GetListOfKeys()):
                 n_algPlots=n_algPlots.Get(type_name)
             found=False
             for n_alg in n_algPlots.GetListOfKeys():
                 n_alg_name=str(n_alg.GetName()).strip();
                 if n_alg_name == plt_name:
                     found=True
             if found:
                 type_plt  = FillOnlinePlot(tfile,plt_name,type_name,'Event Time (ms)',type_plt,run,nEntry,tableType)
             if type_plt and options.rmOutliers:
                 RemoveOutliers(type_plt)
             if type_plt and options.NoiseSuppress:
                 NoiseSuppress(type_plt)
             
     return type_plt

def AddToSumPlot(sum_all,plt,name,hname):

    if not hname in sum_all:
        return
    sum_map = sum_all[hname]
    print 'hname:',hname
    print 'alg_name:',name

    for key,p in sum_map.iteritems():

        if re.search(key,name) and re.search('alg_avg_roi',hname):

            print '--------------------------'
            print 'key:   ',key
            print 'Sum:   ',name
            print 'hname: ',hname
            print '--------------------------'
            if p!=None:
                #p.Add(plt)
                for i in range(0,plt.GetNbinsX()):
                    if plt.GetBinContent(i)>0.0: # and p.GetBinContent(i)>0.0:
                        p.SetBinContent(i,plt.GetBinContent(i)+p.GetBinContent(i))
                        p.SetBinError(i,TMath.Sqrt(plt.GetBinError(i)*plt.GetBinError(i)+p.GetBinError(i)*p.GetBinError(i)))
                    elif plt.GetBinContent(i)==0.0 and p.GetBinContent(i)==0.0:
                        #p.Add(plt)
                        p.SetTitle('Sum of Algorithm Time for '+key)
                        p.SetName('sum_'+hname+'_'+key)
                        p.SetXTitle('Sum '+key+' - Mean PileUp #mu')
                        if hname.count('_roi_'):
                            p.SetYTitle('Mean Algorithm Time/RoI (ms)')
                        else:
                            p.SetYTitle('Mean Algorithm Time/Event (ms)')
            else:
                p=copy.deepcopy(plt.Clone())
                p=TH1F('sum_'+hname+'_'+key,'Algorithm Time '+key,800,LOWER_BIN,UPPER_BIN);
                p.Add(plt)
                p.SetTitle('Sum of Algorithm Time for '+key)
                p.SetName('sum_'+hname+'_'+key)

def NoiseSuppress(plt):
    #
    # Suppress events with >50 % error
    #
    for i in range(0,plt.GetNbinsX()):

        content = plt.GetBinContent(i)
        err     = plt.GetBinError(i)
        if content ==0.0:
            continue
        elif err/content>0.5:
            plt.SetBinContent(i,0.0)
            plt.SetBinError(i,0.0)
    
def RemoveOutliers(plt):
    #
    # Remove the outliers
    #
    avgOver=20 # bins
    avg=[]
    for i in range(0,plt.GetNbinsX()):

        avg+=[plt.GetBinContent(i)]
        
        if (i%avgOver)==0:
            
            mean,std=meanstdv(avg)
            medn    =median(avg)
            ratio   =1000.0

            if mean>0.0:
                ratio=std/mean
                if ratio<0.1:
                    std=0.1*mean
            bMax=medn+2.5*std
            bMin=medn-2.5*std
            if len(avg)<13:
                continue

            for j in range(i-avgOver,i):
                
                value=plt.GetBinContent(j)
                
                if value>bMax or bMin>value:
                    plt.SetBinContent(j,0.0)
                    plt.SetBinError(j,0.0)

            # Reset vars
            avg=[]
            

if __name__=="__main__":
    #
    # Downloading the rob-ros map
    #
    print "Opening root files"

    gROOT.Reset();
    gStyle.SetOptFit(1111)

    fLevel=options.lvl
    sf=TFile(fLevel+'algs.root','recreate');

    # Making Directories to save the information
    if not sf.Get(fLevel):
        sf.mkdir(fLevel)
    if fLevel=='L2':
        for chn in timingPlotsL2:
            sf.cd('L2')
            if not sf.Get(chn):
                sf.mkdir(chn)
    if fLevel=='EF':
        for chn in timingPlotsEF:
            sf.cd('EF')
            if not sf.Get(chn):
                sf.mkdir(chn)

    execL2=tmu[0].Get('exec'+fLevel)

    tableRoIL2=execL2.Get('tableRoI'+fLevel)

    #############################################################################
    # Check in the tableRoI** for any plots. This will find the total alg time per roi
    #    -- this is mainly for older root files
    #
    for hist in tableRoIL2.GetListOfKeys():
        if options.isData:
            continue
        #print "hists: ",str(hist.GetName())
        if str(hist.GetName()).count('alg_') and not str(hist.GetName()).count('time'):
            print str(hist.GetName()).strip()
            hname=str(hist.GetName()).strip()
            
            # find hist in all files
            plt = None
            if options.isData:
                plt = LoopOverDataRuns('exec'+fLevel,'tableRoI'+fLevel,hname,'')
            else:
                plt = fillPlot(hname,'Event Time',level=fLevel)

            if not plt:
                print 'ERROR plot does not exist: ',hname
            else:
                if fLevel=='L2':
                    for chn in timingPlotsL2:
                        if hname.count(chn):
                            sf.cd(fLevel+'/'+chn)
                            plt.Write()
                if fLevel=='EF':
                    for chn in timingPlotsEF:
                        if hname.count(chn):
                            sf.cd(fLevel+'/'+chn)
                            plt.Write()
    sf.Close()
    sf=TFile(fLevel+'algs.root','update');
    #############################################################################                    #
    # loop over algorithms in tableAlg**
    #   -- newer approach
    #
    tableAlgL2=execL2.Get('tableAlg'+fLevel)

    sum_all={'alg_time_vs_lb'         : {'FullScan_EFID' : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN), #.*_
                                         'Electron_EFID' : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN),
                                         'Tau_EFID'      : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN),
                                         'Muon_EFID'     : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN),
                                         'Bphysics_EFID' : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN) },
             'alg_avg_roi_time_vs_lb' : {'FullScan_EFID' : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN), #.*_
                                         'Electron_EFID' : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN),
                                         'Tau_EFID'      : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN),
                                         'Muon_EFID'     : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN),
                                         'Bphysics_EFID' : TH1F('sum_','Algorithm Time ',800,LOWER_BIN,UPPER_BIN) }
             }

    alg_plots=['alg_time_per_roi','event_alg_time','event_ros_size_retrv','calls_alg_time']
    if options.isData:
        alg_plots=['alg_avg_roi_time_vs_lb','alg_time_vs_lb']

    save_algs = ['TrigIDSCAN_FullScan',
                 'TrigIDSCAN_eGamma',
                 'TrigIDSCAN_Muon',
                 'TrigSiTrack_Jet',
                 'TrigSiTrack_Tau_robust',
                 'TrigSiTrack_BeamSpot',
                 'T2CaloJet_Jet_noise',
                 'muFast_Muon',
                 'T2CaloEgamma_eGamma']
    
    for alg in tableAlgL2.GetListOfKeys():
        alg_name=str(alg.GetName())
        print "Algorithm: ",alg_name
        
        alg_dir=tableAlgL2.Get(alg_name)

        for hist in alg_dir.GetListOfKeys():
            hname=str(hist.GetName()).strip()
            if hname in alg_plots:      #str(hist.GetName()).count('_roi'):
                print "hist: ",hname
                written=False

                plt = None
                if options.isData:
                    plt = LoopOverDataRuns('exec'+fLevel,'tableAlg'+fLevel,hname,alg_name)
                else:
                    plt  = fillPlot(hname,'Event Time',level=fLevel,table='tableAlg'+fLevel+'/'+alg_name,algname=alg_name)

                if not plt:
                    print 'ERROR plot does not exist: ',hname
                else:
                    plt.SetXTitle('Mean PileUp #mu')
                    if hname.count('event_alg_time'):
                        plt.SetYTitle('Event Algorithm Time (ms)')
                    elif hname.count('event_ros_size_retrv'):
                        plt.SetYTitle('ROB Size Retrieved (kB)/ROI')
                    elif hname.count('calls_alg_time'):
                        plt.SetYTitle('Alg Time/Call (ms)')
                    elif hname.count('alg_avg_roi_time_vs_lb'):
                        plt.SetYTitle(alg_name+' Alg Time/RoI (ms)')
                    elif hname.count('alg_time_vs_lb'):
                        plt.SetYTitle(alg_name+' Alg Time/Event (ms)')
                    if fLevel=='L2':
                        for chn in timingPlotsL2:
                            if alg_name.count(chn):
                                sf.cd(fLevel)
                                sf.cd(chn)
                                plt.Write()
                                written=True
                    if fLevel=='EF':
                        for chn in timingPlotsEF:
                            if alg_name.count(chn):
                                sf.cd(fLevel)
                                sf.cd(chn)
                                plt.Write()
                                written=True
                    # Check if plot needs to be summed
                    AddToSumPlot(sum_all,plt,alg_name,hname)
                    # Write to file
                    if not written:
                        sf.cd(fLevel)
                        plt.Write()

                if (alg_name in save_algs) and hname =='alg_avg_roi_time_vs_lb':
                    if plt==None:
                        continue
                    plt.Draw()
                    
                    c1.Update()
                    if options.wait:
                        c1.WaitPrimitive()
                    if options.save:
                        c1.SaveAs('alg_'+plt.GetName()+'.pdf')
    sf.Close()
    sf=TFile(fLevel+'algs.root','update');
    
    # Write Sum plots
    if not sf.Get('sums'):
        sf.mkdir('sums')
    sf.cd('sums')

    for k,sum_m in sum_all.iteritems():
        for key,splot in sum_m.iteritems():
            if fLevel=='L2':
                continue
            if splot!=None:
                print'Plot: ',splot.GetName()
                sf.cd('sums')
                splot.Write()
                c1.cd()
                splot.Draw()
                c1.Update()
                if options.wait:
                    c1.WaitPrimitive()
                if options.save:
                    c1.SaveAs(splot.GetName()+'.pdf')
                
            else:
                print 'Logic ERROR - '+key+' '+k
                continue
                sys.exit(0)

    #############################################################################                    
    # loop over algorithms in tableChn**
    #   -- newer approach
    #
    tableChnL2=execL2.Get('tableChn'+fLevel)
    if not sf.Get('tableChn'+fLevel):
        sf.mkdir('tableChn'+fLevel)
    chn_plots=['alg_time_per_roi','event_alg_time','event_time_frac','event_ros_size_retrv','event_cpu_time','calls_alg_time']
    if options.isData:
        chn_plots = ['chn_avg_roi_time_vs_lb','chn_time_vs_lb']
    
    save_chains=['EF_mu15_mu10_EFFS','EF_e20_medium1','EF_ht350_a4tc_EFFS_L2je255',
                 'EF_j240_a4tc_EFFS_l2cleanph','EF_tau16_loose_e15_medium', 'EF_mu10_Upsimumu_FS',
                 'L2_2b10_medium_L1_2J10J50', 'L2_2b20_medium_3L1J20', 'L2_tau16_loose_e15_medium']
    if options.isData:
        save_chains = ['EFMissingET_Fex',
                       'TrigCaloClusterMaker_topo_fullscan']

    for chn in tableChnL2.GetListOfKeys():
        chn_name=str(chn.GetName())
        chn_dir=tableChnL2.Get(chn_name)

        print "Chain: ",chn_name

        # make output directory
        if not sf.Get('tableChn'+fLevel+'/'+chn_name):
            sf.cd('tableChn'+fLevel)
            sf.Get('tableChn'+fLevel).mkdir(chn_name)
        sf.cd('tableChn'+fLevel+'/'+chn_name)

        for hist in chn_dir.GetListOfKeys():
            hname=str(hist.GetName()).strip()
            if hname in chn_plots:
                print "hist: ",hname
                # find hist in all files
                plt=None
                if options.isData:
                    plt = LoopOverDataRuns('exec'+fLevel,'tableChn'+fLevel,hname,chn_name)
                else:
                    plt  = fillPlot(hname,'Event Time',level=fLevel,table='tableChn'+fLevel+'/'+chn_name,algname=chn_name)
                    
                if not plt:
                    print 'ERROR plot does not exist: ',hname
                else:
                    if hname.count('event_alg_time'):
                        plt.SetYTitle('Event Algorithm Time (ms)')
                    elif hname.count('event_time_frac'):
                        plt.SetYTitle('Fraction of Event (%)')
                    elif hname.count('event_cpu_time'):
                        plt.SetYTitle('Event CPU Time (ms)')
                    elif hname.count('event_ros_size_retrv'):
                        plt.SetYTitle('ROB Size Retrieved (kB)/ROI')
                    elif hname.count('calls_alg_time'):
                        plt.SetYTitle('Alg Time/Call (ms)')
                    plt.Write()

                if (chn_name in save_chains) and (not options.isData or hname=='chn_avg_roi_time_vs_lb'):
                    plt.Draw()
                    c1.Update()
                    if options.wait:
                        c1.WaitPrimitive()
                    if options.save:
                        c1.SaveAs('chn_'+plt.GetName()+'.pdf')
    sf.Close()
    sf=TFile(fLevel+'algs.root','update');
    #############################################################################                    
    # loop over ROS in tableROSL2
    #   -- get ROS: roi_ros_size, roi_ros_nreq, roi_ros_nreq_retrv, roi_ros_robids, roi_ros_size_retrv
    #
    ros_plots = ['hreq_rate_vs_lb','hreq_size_vs_lb',
                 'hret_rate_vs_lb','hret_size_vs_lb']
    if fLevel=="L2":
        tableROSL2=execL2.Get('tableROSL2')
        if not sf.Get('tableROSL2'):
            sf.mkdir('tableROSL2')
        
        for ros in tableROSL2.GetListOfKeys():
            print "ROS: ",str(ros.GetName())
            ros_dir=tableROSL2.Get(str(ros.GetName()))
            ros_name=str(ros.GetName())

            if not sf.Get('tableROSL2/'+ros_name):
                sf.cd('tableROSL2')
                sf.Get('tableROSL2').mkdir(ros_name)
            sf.cd('tableROSL2/'+ros_name)
            for hist in ros_dir.GetListOfKeys():
                #Selecting the plots to write
                hname=str(hist.GetName()).strip()
                print 'hist:',hname
                if (str(hist.GetName()).count('roi_') and not options.isData) or (hname in ros_plots and options.isData):
                    print "hist: ",hname

                    # find hist in all files
                    plt=None
                    if options.isData:
                        plt = LoopOverDataRuns('exec'+fLevel,'tableROS'+fLevel,hname,ros_name)
                    else:
                        plt  = fillPlot(hname,'Event Time',table='tableROSL2/'+ros_name,algname=ros_name)
                    if not plt:
                        print 'ERROR plot does not exist: ',hname
                    else:
                        plt.SetName('mc_'+hname+'_vs_mu')
                        if hname.count('roi_ros_size'):
                            plt.SetYTitle('ROS Size Requested (kB)/ROI')
                        elif hname.count('roi_ros_nreq_retrv'):
                            plt.SetYTitle('ROS Retrievals/ROI')
                        elif hname.count('roi_ros_nreq'):
                            plt.SetYTitle('ROS Requests/ROI')
                        elif hname.count('roi_ros_size_retrv'):
                            plt.SetYTitle('ROS Size Retrieved (kB)/ROI')
                        plt.Write()

    sf.Close()
    sf=TFile(fLevel+'algs.root','update');
    #############################################################################                    
    # Write Event Level Algorithm timing
    #
    print 'About to look at survey algs'
    sys.stdout.flush()
    
    if options.isData:
        #
        # SurveyLB
        #
        evt_plots = ['htime_alg_vs_lb','htime_cpu_vs_lb','htime_rob_vs_lb','hn_roi_vs_lb']
        surveyLBL2=execL2.Get('surveyLB'+fLevel)

        # Making output Directory
        if not sf.Get('surveyLB'+fLevel):
            sf.mkdir('surveyLB'+fLevel)
        sf.cd('surveyLB'+fLevel)
        print 'Starting loop'
        # Looping over histograms
        for lbs in surveyLBL2.GetListOfKeys():
            hname=str(lbs.GetName())
            print "surveyLB: ",hname
            sys.stdout.flush()
            
            if hname in evt_plots:
                
                print "hist: ",hname
                plt=None
                if options.isData:
                    plt = LoopOverDataRuns('exec'+fLevel,'surveyLB'+fLevel,hname,'')
                else:
                    plt  = fillPlot(hname,'Event Time',level=fLevel,table='surveyLB'+fLevel+'/'+chn_name,algname=hname)
                if not plt:
                    print 'ERROR plot does not exist: ',hname
                else:
                    plt.Write()
                    plt.SetXTitle('Mean PileUp (#mu)')
                    if hname == 'htime_alg_vs_lb':
                        plt.SetYTitle('Algorithm Time/RoI (ms)')
                    elif hname == 'htime_cpu_vs_lb':
                        plt.SetYTitle('(Algorithm Time-ROB Time)/RoI (ms)')
                    elif hname == 'htime_rob_vs_lb':
                        plt.SetYTitle('ROB Time/RoI (ms)')
                    elif hname == 'hn_roi_vs_lb':
                        plt.SetYTitle('Number of RoIs')                        
                    plt.Draw()
                    c1.Update()
                    if options.wait:
                        c1.WaitPrimitive()
                    if options.save:
                        c1.SaveAs(fLevel+'time_'+plt.GetName()+'.pdf')

    # Exit if this is data
    if options.isData:
        print 'Closing files'
        sys.stdout.flush()
        sf.Close()
        print 'output Closed'
        sys.stdout.flush()
        
        del inputRuns
        del tmu
        #u=1
        #for fi in tmu:
        #
        #    print 'File Closing: ',u
        #    sys.stdout.flush()
        #    fi.Close()
        #    u+=1
        
        print 'Done!'
        sys.stdout.flush()
        sys.exit(0)
                    
    print "Mean number of roi per call0: ",getPlotMean(tmu[0],'alg_time_per_roi',fLevel)

    evtTime = TH1F( 'evt_time_vs_mu', 'Event Time',int(muMax-muMin)+1 , muMin-0.001, muMax+1)
    for i in range(0,len(tmu)):
        evtTime.SetBinContent(int(mus[i])+1,getEvtMean(tmu[i], 'alg_evt_time',fLevel))
        evtTime.SetBinError(int(mus[i])+1,  getEvtErr(tmu[i],  'alg_evt_time',fLevel))

    evtTime.SetXTitle('Mean Pileup')
    evtTime.SetYTitle('Mean Event Time (ms)')
    evtTime.Fit('pol1')
    evtTime.Draw()
    if options.save:
        c1.SaveAs('del_'+fLevel+'alg_evt_time_vs_mu.pdf')
    sf.cd('evt_time')
    evtTime.Write()

    roiTime = fillPlot('alg_time_per_roi','Event Time',level=fLevel)
    roiTime.SetXTitle('Mean Pileup')
    roiTime.SetYTitle('Mean RoI Time (ms)')
    sf.cd('alg_time_per_roi')
    roiTime.Write()
    
    nroi = fillPlot('nroi_event','Number of RoI',level=fLevel)
    nroi.SetXTitle('Mean Pileup')
    nroi.SetYTitle('# RoI')
    sf.cd('nroi_event')
    nroi.Write()
    
    anroi = fillPlot('nroi_call','Algorithm Calls per RoI',level=fLevel)
    anroi.SetXTitle('Mean Pileup')
    anroi.SetYTitle('Algorithm Calls')
    sf.cd('nroi_call')
    anroi.Write()

    sf.Close()
