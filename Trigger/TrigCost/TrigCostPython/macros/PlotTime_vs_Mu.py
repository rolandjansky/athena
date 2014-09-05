#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from ROOT import *
from ROOT import gStyle
from   optparse    import OptionParser
import os,commands
import sys
from TrigCostPython import TrigCostAnalysis,TrigCostCool

#
# Parse command line options
#   
from optparse import OptionParser
p = OptionParser()
p.add_option( '-l', type="string", default="L2", dest="lvl",            help="Trigger level L2 or EF")
p.add_option( '-s', '--save',   action='store_true', default=False,     dest="save",    help="save the histograms as pdf")
p.add_option( '-o', type="string", default="algs_mu_", dest="ouput",    help="output root file name")
p.add_option( '-r', type="string", default="", dest="runs",             help="comma separated list of runs")
p.add_option('-v', '--verbose',    action="store_true", default=False,  help="Run in verbose mode")
p.add_option('-d', '--debug',      action="store_true", default=False,  help="Run in debug mode")
p.add_option('-q', '--quiet',      action="store_true", default=False,  help="Run in quiet mode (suppresses chains with zero rates)")

(options, args) = p.parse_args()

fmu30=TFile('cost_mu30.root');
fmu25=TFile('cost_mu25.root');
fmu20=TFile('cost_mu20.root');
fmu15=TFile('cost_mu15.root');
fmu10=TFile('cost_mu10.root');
fmu5=TFile('cost_mu5.root');
fmu0=TFile('cost_mu0.root');

print "Adjusting ROOT settings!"

gROOT.Reset();
gStyle.SetOptFit(1111)
fLevel=options.lvl

# OPENING output root file
sf=TFile(fLevel+options.ouput+'.root','recreate');

opts=[""]
if options.debug: opts.append('debug')
if options.quiet: opts.append('quiet')

# Open input root file 
MuOnl=[]
#=TFile('cost_0180710_'+fLevel+'CostMonitoring.root');
#MuOnline2=TFile('cost_0178211_L2CostMonitoring.root');

# MC Plots
fMC=TFile(fLevel+'algs.root');
#189481,189813, 189483, 186156
#189598,- mu 13 40ms little ragged
#190300 - mu 13 - 35ms
# 186965 -mu13.7 - 33ms
# 189781 -mu 12.5 - 33ms - little ragged
#189719 - mu 13.4 - 30ms
#189425 - mu 14.5 - 40ms
#189822 - mu 14.5 - 33ms
#189536 - mu 16   - 55ms
# 189561- mu 15,8
#189751 - mu 13.2 - 30ms
#189483 - mu 14   - 45ms - has spike
#189372 - mu 12.4 - 38ms
#189207 - mu 14.2 - 45ms
#inputRuns=[186156,189536,189561,190120,190119,189598,189372,189425] # for L2
inputRuns=[186156,190120,190119,189372,189242] # for EF - 189598,189561
#180636]   #160879,166466,167607,167776,180710] #178163 #178211,177540,#,180400, 180710,180124,177540,180124,177540,178109,  180124,180400,
lbse=[]
for run in inputRuns:
    lbse.append(TrigCostCool.GetLumiblocks(run,1,1400,opts));
    MuOnl.append(TFile('cost_0'+str(run)+'_'+fLevel+'CostMonitoring.root'));

MuOnline=dict(zip(inputRuns, MuOnl))
lbset=dict(zip(inputRuns, lbse))

c1 = TCanvas("c1", "c1", 1000, 500);

timingPlotsL2=["TrigIDSCAN_Muon",
               "TrigIDSCAN_muonIso",
               "TrigIDSCAN_Electron",
               "TrigIDSCAN_eGamma",
               "TrigIDSCAN_Bphysics",
               "TrigIDSCAN_FullScan", #should remove
               "TrigIDSCAN_Jet", # should remove
               "TrigIDSCAN_Tau", # should remove
               "TrigSiTrack_Tau_robust",
               "TrigSiTrack_Tau",
               "TrigSiTrack_Jet",
               "nroi_event",
               "nroi_call",
               "evt_time",
               "alg_time_per_roi"]

timingPlotL2Root=["TrigIDSCAN","TrigSiTrack"]

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

#import os,commands
def getCastorDirectoryList(path):
    cmd  = 'nsls %s/ ' % (path)
    file = ["%s" %i for i in commands.getoutput(cmd).split('\n')]
    return file

def getPlot(rfile,name,level):
    plot=rfile.Get('exec'+level).Get('tableRoI'+level).Get(name)
    return plot

def getPlotMean(rfile,name,level):
    plot=rfile.Get('exec'+level).Get('tableRoI'+level).Get(name)
    if plot:
        return plot.GetMean(1)
    print "Could not find histogram ",name
    return 0

def getPlotErr(rfile,name,level):
    plot=rfile.Get('exec'+level).Get('tableRoI'+level).Get(name)
    if plot:
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

def fillPlot(name,title,level):

    newname=name

    if name.count('alg_') and not name.count('time'):
        newname='alg_'+getPlot(fmu20,name,level).GetTitle()
        print "alg name",newname

    plot = TH1F( newname+'_vs_mu', title, 30 , -0.01, 30)
#    print 'mu0'
    plot.SetBinContent(1,getPlotMean(fmu0,  name,level))
    plot.SetBinError(1,getPlotErr(fmu0,  name,level))
#    print 'mu5'
    plot.SetBinContent(5,getPlotMean(fmu5,  name,level))
    plot.SetBinError(5,getPlotErr(fmu5,  name,level))
#    print 'mu10'
    plot.SetBinContent(10,getPlotMean(fmu10,name,level))
    plot.SetBinError(10,getPlotErr(fmu10,name,level))
#    print 'mu15'
    plot.SetBinContent(15,getPlotMean(fmu15,name,level))
    plot.SetBinError(15,getPlotErr(fmu15,name,level))
#    print 'mu20'
    plot.SetBinContent(20,getPlotMean(fmu20,name,level))
    plot.SetBinError(20,getPlotErr(fmu20,name,level))
#    print 'mu25'
    plot.SetBinContent(25,getPlotMean(fmu25,name,level))
    plot.SetBinError(25,getPlotErr(fmu25,name,level))
#    print 'mu30'
    plot.SetBinContent(30,getPlotMean(fmu30,name,level))
    plot.SetBinError(30,getPlotErr(fmu30,name,level))

    plot.Draw()
    plot.Fit('pol1')
    aplt = getPlot(fmu20,hname,fLevel)
    plot.SetTitle(aplt.GetTitle())
    plot.SetYTitle('Algorithm Time (ms)')
    plot.SetXTitle('Mean Number of Pileup Events')
    #plot.Write()

    for chn in timingPlotsL2:
        if newname.count(chn):
            if options.save:
                junk=raw_input('Hit enter when you are happy with how the plot looks')
                c1.SaveAs('del_'+level+newname+'_vs_mu.pdf')

    for chn in timingPlotsEF:
        if newname.count(chn):
            if options.save:
                junk=raw_input('Hit enter when you are happy with how the plot looks')
                c1.SaveAs('del_'+level+newname+'_vs_mu.pdf')

    return plot

def GetOnlinePlot(fFile,hist,alg_name,level):
    return fFile.Get('exec'+level).Get('tableAlg'+level).Get(alg_name).Get(hist)
    
def FillOnlinePlot(fFile,hist,alg_name,title,level,vsMu,run,nEntry):

    # Collect histogram for Algorithm alg_name and trigger level
    mu_plot=GetOnlinePlot(fFile,hist,alg_name,level);
    plt_alg=GetOnlinePlot(fFile,'alg_time_vs_lb',alg_name,level);

    # Create timing and error variables
    lb   = 0;
    time = 0.0;
    err  = 0.0;
    if not mu_plot:
        return vsMu

    while lb<mu_plot.GetNbinsX():
        
        nRoI=0.0
        if mu_plot.GetBinContent(lb)>0:
            nRoI=plt_alg.GetBinContent(lb)/mu_plot.GetBinContent(lb)
        if nRoI >2.0:
            mu_plot.SetBinError(lb,mu_plot.GetBinContent(lb)/TMath.Sqrt(nRoI))
        else:
            mu_plot.SetBinError(lb,0.3*mu_plot.GetBinContent(lb))
        lb=lb+1

    lb=0
    print "number of X bins:",mu_plot.GetNbinsX()
    print "entries: ",len(nEntry)

    while lb<mu_plot.GetNbinsX():
        
        time_lb=mu_plot.GetBinContent(lb)
        if lb%5 !=0:
            if time_lb < (.6*time/float(lb%5)):
                time_lb=time/float(lb%5)
        time=time_lb+time;
        err=mu_plot.GetBinError(lb)+err;
        
        # Averaging over 5 lbs 
        if lb%5==0 and lb != 0:

            if time==0:
                lb=1+lb;
                continue

            #Collect the mu
            mu=lbset[run].GetMeanPileUp(lb+int(mu_plot.GetBinLowEdge(0)))
            bin=int(600.0/30*mu)
            old_time=vsMu.GetBinContent(bin);
            old_err=vsMu.GetBinError(bin);
            if old_time>0.0:
                time=(5.0*old_time*nEntry[lb]+time)/(nEntry[lb]+1.0)
                err=(old_err*nEntry[lb]+err)/(TMath.Sqrt(nEntry[lb]+1.0))
            vsMu.SetBinContent(bin,time/5.0);
            vsMu.SetBinError(bin,err/11.2);
            if lb<len(nEntry):
                nEntry[lb]=1.0+nEntry[lb];
            else:
                print "Exceeded entry size ",lb
            # Reset variables
            time = 0.0;
            err  = 0.0;
            
        lb=1+lb;

    #vsMu.Fit('pol1')
    return vsMu;

def GetMCPlot(fFile,alg_name):
    hist=fFile.Get('alg_'+alg_name+'_vs_mu')
    return hist

def GetMeanEvtTime(fFile,lvl,alg_evt,run,nEntry):
    hist='htime_alg_vs_lb'
    plt=fFile.Get('exec'+lvl).Get('surveyLB'+lvl).Get(hist);
    if not alg_evt:
        alg_evt=TH1F(lvl+'_alg_evt_time_vs_mu','Algorithm RoI time vs mu',600,0,30.0)

    # Create timing and error variables
    lb   = 0;
    time = 0.0;
    err  = 0.0;

    while lb<plt.GetNbinsX():

        time_lb=plt.GetBinContent(lb)
        if lb%5 !=0:
            if time_lb < (.6*time/float(lb%5)):
                time_lb=time/float(lb%5)
        time=time_lb+time;
        
        # Averaging over 5 lbs 
        if lb%5==0 and lb != 0:

            if time==0:
                lb=1+lb;
                continue

            #Collect the mu
            mu=lbset[run].GetMeanPileUp(lb+int(plt.GetBinLowEdge(0)));
            bin=int(600.0/30*mu);
            old_time=alg_evt.GetBinContent(bin);
            if old_time>0.0:
                time=(5.0*old_time*nEntry[lb]+time)/(nEntry[lb]+1.0)
                err=err/1.41
            alg_evt.SetBinContent(bin,time/5.0);
            nEntry[lb]=1.0+nEntry[lb];
#            alg_evt.SetBinError(bin,err/11.2);
            # Reset variables
            time = 0.0;
            err  = 0.0;
            
        lb=1+lb;
        
    return alg_evt;
    

if __name__=="__main__":

    fMuOnline=None

    if len(inputRuns):
        fMuOnline=MuOnline[inputRuns[len(inputRuns)-1]]
    else:
        print "Must specify at least one run number"
        sys.exit()

    #    for run in inputRuns:
    execL2=fMuOnline.Get('exec'+fLevel);
        
    tableAlgL2=execL2.Get('tableAlg'+fLevel);

    ######################
    # Interesting plots to make
    #    - add name here for additional plots
    ######################
    histograms = ['alg_avg_roi_time_vs_lb']
                  #'alg_time_per_roi',
                  #'alg_time_vs_lb',
                  #'event_time_frac',
                  #'Event total time (ms)',
                  #'ROB Requests',
                  #'event_cpu_time',
                  #'calls_rob_nreq_retrv',
                  #'calls_rob_size',
                  #'calls_rob_size_retrv',
                  #'event_rob_size',
                  #'event_rob_size_retrv']
    #
    # Alg time per event vs LB
    #
    alg_evt=TH1F(fLevel+'_alg_evt_time_vs_mu',fLevel+'Algorithm Event time vs mu',600,0,30.0);
    nEntry=[]
    i=0
    while i<1250:
        nEntry.append(0.0)
        i=i+1
    for run in inputRuns:
        alg_evt=GetMeanEvtTime(MuOnline[run],fLevel,alg_evt,run,nEntry);
    alg_evt.SetXTitle('mu');
    alg_evt.SetYTitle('Event Time (ms)');
    alg_evt.SetMarkerSize(4)
    sf.cd()
    c1.SetTitle(fLevel+'Algorithm Event time vs mu')
    c1.SetName(fLevel+'Algorithm_Event_time_vs_mu')
    alg_evt.Fit('pol1')
    alg_evt.Write();
    if options.save:
        #junk=raw_input('Hit enter when you are happy with how the plot looks')
        #alg_evt.SaveAs(fLevel+'_alg_evt_time_vs_mu.pdf')
        c1.Write()

    #
    # Make sum plots for different classes of algorithm
    #
    if fLevel.count('EF'):
        timingPlotL2Root=timingPlotsEF
    for alg_type in timingPlotL2Root:

        type_plt_mu=TH1F(fLevel+'_'+alg_type+'_vs_mu','Sum of '+alg_type,600,0,30.0);
        type_MC_mu=TH1F(fLevel+'_'+alg_type+'_MC_vs_mu','Sum of '+alg_type,30,-0.01,30.0);
        
        for alg in tableAlgL2.GetListOfKeys():
            
            alg_name=str(alg.GetName()).strip();
            # add to plot if the algorithm is of alg_type
            if alg_name.count(alg_type):
                
                type_plt=TH1F(fLevel+'_'+alg_type+'_vs_mu_temp','Temp Sum of '+alg_type,600,0,30.0);

                pltMC=GetMCPlot(fMC,alg_name);
                
                if pltMC:
                    type_MC_mu.Add(pltMC,1.0)
                nEntry=[]
                i=0
                while i<1250:
                    nEntry.append(0.0)
                    i=i+1
                for run in inputRuns:
                    
                    #Check that plot is in file
                    n_algPlots=MuOnline[run].Get('exec'+fLevel).Get('tableAlg'+fLevel);
                    found=False
                    for n_alg in n_algPlots.GetListOfKeys():
                        n_alg_name=str(n_alg.GetName()).strip();
                        if n_alg_name == alg_name:
                            found=True
                    if found:
                        type_plt  = FillOnlinePlot(MuOnline[run],'alg_avg_roi_time_vs_lb',alg_name,'Event Time (ms)',fLevel,type_plt,run,nEntry)
                ## Add the stats from the las plot
                type_plt_mu.Add(type_plt,1.0)
        c1.SetTitle(fLevel+'_'+alg_type+'_vs_mu')
        c1.SetName(fLevel+'_'+alg_type+'_vs_mu_pad')
        type_plt_mu.SetXTitle('mu')
        type_plt_mu.SetYTitle('Algorithm RoI Time (ms)')
        type_plt_mu.SetMarkerSize(4)
        type_plt_mu.Fit('pol1')
        type_plt_mu.Draw()
        type_plt_mu.Write()

        type_MC_mu.SetLineColor(50);
        type_MC_mu.SetMarkerSize(4);
        type_MC_mu.SetTitle('ttbar MC');
        type_MC_mu.Fit('pol1')
        type_MC_mu.Write();
        type_MC_mu.Draw('same');
        
        c1.BuildLegend();
        c1.Write();
        

#        c1.SetTitle(alg_name+'_vs_mu');
#                c1.SetName(alg_name+'_vs_mu');
#                plt.Fit('pol1');
#                plt.SetMarkerSize(4)
#                plt.Draw();
#                sf.cd();
        
    #    
    # Loop over algorithms 
    #
    for alg in tableAlgL2.GetListOfKeys():

        alg_name=str(alg.GetName()).strip();
        
        algPlots=tableAlgL2.Get(alg_name)

        for hist in algPlots.GetListOfKeys():
            hist_name=str(hist.GetName()).strip();
            #print hist_name

            if hist_name in histograms:
                
                # Create new algorithm plot versus mu - mean pileup
                plt=TH1F(alg_name+'_'+hist_name+'_vs_mu','Online',600,0,30.0);
                plt.SetXTitle('mu')
                plt.SetYTitle('Algorithm RoI time')
                nEntry=[]
                i=0
                while i<1250:
                    nEntry.append(0.0)
                    i=i+1
                
                for run in inputRuns:

                    #Check that plot is in file
                    n_algPlots=MuOnline[run].Get('exec'+fLevel).Get('tableAlg'+fLevel);
                    found=False
                    for n_alg in n_algPlots.GetListOfKeys():
                        n_alg_name=str(n_alg.GetName()).strip();
                        if n_alg_name == alg_name:
                            found=True
                    if found:
                        plt  = FillOnlinePlot(MuOnline[run],hist_name,alg_name,'Event Time',fLevel,plt,run,nEntry)
                c1.SetTitle(alg_name+'_vs_mu');
                c1.SetName(alg_name+'_vs_mu');
                if plt:
                    plt.Fit('pol1');
                    plt.SetMarkerSize(4)
                    plt.Draw();
                    sf.cd();
                    plt.Write();
                    pltMC=GetMCPlot(fMC,alg_name);
                
                if pltMC:
                    pltMC.SetLineColor(50);
                    pltMC.SetMarkerSize(4);
                    pltMC.SetTitle('ttbar MC')
                    pltMC.Draw('same');
                    pltMC.Write();
                    c1.BuildLegend();

                if alg_name in timingPlotsL2 and options.save:
                    #junk=raw_input('Hit enter when you are happy with how the plot looks')
                    #c1.SaveAs(alg_name+'_wdata_'+fLevel+'.pdf')
                    c1.Write();
                efalg=False
                for alg in timingPlotsEF:
                    if alg_name.count(alg):
                        efalg=True
                if efalg and options.save:
                    #junk=raw_input('Hit enter when you are happy with how the plot looks')
                    #c1.SaveAs(alg_name+'_wdata_'+fLevel+'.pdf')
                    c1.Write()
                #if options.save:
                #    plt.SaveAs(alg_name+'_'+hist+'_'+fLevel+'.pdf');

    #tableRoIL2=execL2.Get('tableRoI'+fLevel);

    #for hist in tableRoIL2.GetListOfKeys():
    #    if str(hist.GetName()).count('alg_') and not str(hist.GetName()).count('time'):
    #        print str(hist.GetName()).strip()
    #
    #        hname=str(hist.GetName()).strip()
    #        # find hist in all files
    #        plt  = fillPlot(hname,'Event Time',fLevel)
    #        plt.Write()
    #
    #print "Mean number of roi per call0: ",getPlotMean(fmu0,'alg_time_per_roi',fLevel)
    #print "Mean number of roi per call15: ",getPlotMean(fmu15,'alg_time_per_roi',fLevel)
    #
    #evtTime = TH1F( 'evt_time_vs_mu', 'Event Time', 30 , -0.01, 30)
    #evtTime.SetBinContent(1,getEvtMean(fmu0,  'alg_evt_time',fLevel))
    #evtTime.SetBinContent(5,getEvtMean(fmu5,  'alg_evt_time',fLevel))
    #evtTime.SetBinContent(10,getEvtMean(fmu10,'alg_evt_time',fLevel))
    #evtTime.SetBinContent(15,getEvtMean(fmu15,'alg_evt_time',fLevel))
    #evtTime.SetBinContent(20,getEvtMean(fmu20,'alg_evt_time',fLevel))
    #evtTime.SetBinContent(25,getEvtMean(fmu25,'alg_evt_time',fLevel))
    #evtTime.SetBinContent(30,getEvtMean(fmu30,'alg_evt_time',fLevel))
    #evtTime.SetBinError(1, getEvtErr(fmu0,  'alg_evt_time',fLevel))
    #evtTime.SetBinError(5, getEvtErr(fmu5,  'alg_evt_time',fLevel))
    #evtTime.SetBinError(10,getEvtErr(fmu10,'alg_evt_time',fLevel))
    #evtTime.SetBinError(15,getEvtErr(fmu15,'alg_evt_time',fLevel))
    #evtTime.SetBinError(20,getEvtErr(fmu20,'alg_evt_time',fLevel))
    #evtTime.SetBinError(25,getEvtErr(fmu25,'alg_evt_time',fLevel))
    #evtTime.SetBinError(30,getEvtErr(fmu30,'alg_evt_time',fLevel))
    #evtTime.SetXTitle('Mean Pileup')
    #evtTime.SetYTitle('Mean Event Time (ms)')
    #evtTime.Fit('pol1')
    #evtTime.Draw()
    #if options.save:
    #    c1.SaveAs('del_'+fLevel+'alg_evt_time_vs_mu.pdf')
    #evtTime.Write()
    #
    #roiTime = fillPlot('alg_time_per_roi','Event Time',fLevel)
    #roiTime.SetXTitle('Mean Pileup')
    #roiTime.SetYTitle('Mean RoI Time (ms)')
    #roiTime.Write()
    #
    #nroi = fillPlot('nroi_event','Number of RoI',fLevel)
    #nroi.SetXTitle('Mean Pileup')
    #nroi.SetYTitle('# RoI')
    #nroi.Write()
    #
    #anroi = fillPlot('nroi_call','Algorithm Calls per RoI',fLevel)
    #anroi.SetXTitle('Mean Pileup')
    #anroi.SetYTitle('Algorithm Calls')
    #anroi.Write()

    sf.Close()
