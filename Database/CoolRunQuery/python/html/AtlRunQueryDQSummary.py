#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import time, math

from ROOT import TColor, TText, TH1F, TH2F, TCanvas, THStack, TLegend
from ROOT import gPad, gStyle, kPink, kBlue, kOrange, kYellow, kRed, kGreen, kAzure, kGray
from CoolRunQuery.AtlRunQueryQueryConfig import QC
from CoolRunQuery.selector.AtlRunQuerySelectorBase import DataKey
from CoolRunQuery.utils.AtlRunQueryUtils import importroot
importroot()

from CoolRunQuery.output.AtlRunQueryRoot import SetStyle
from CoolRunQuery.utils.AtlRunQueryUtils import timer
from CoolRunQuery.AtlRunQueryRun import Run
from CoolRunQuery.AtlRunQueryCOMA import ARQ_COMA

import xmlrpc.client
global server
server = xmlrpc.client.ServerProxy('http://atlasdqm.cern.ch')

from DQDefects import DefectsDB

global readylb
global lumifolder
global usenb
global unit
global livetrigger

########################################################################
def MapToSystem(defect):

    # Store detectors/cp per "family" for readability
    global_systems_map = {'LAR':'Calo','TILE':'Calo','CALO':'Calo',
                          'TAU':'Calo','EGAMMA':'Calo','JET':'Calo','MET':'Calo',
                          'ALFA':'Trigger & Lumi','LCD':'Trigger & Lumi',
                          'ZDC':'Trigger & Lumi','BCM':'Trigger & Lumi',
                          'LUMI':'Trigger & Lumi','TRIG':'Trigger & Lumi',
                          'GLOBAL':'Trigger & Lumi',
                          'MS_CSC':'Muon','MS_RPC':'Muon','MS_TGC':'Muon','MS_MDT':'Muon',
                          'MS':'Muon','MBTS':'Muon','MCP':'Muon',
                          'PIXEL':'Tracking','SCT':'Tracking','TRT':'Tracking',
                          'ID':'Tracking','BTAG':'Tracking'}
    words = defect.split('_')
    for w in words:
        for item in global_systems_map:
            if w in item:
                return global_systems_map[item]
    return ' '

########################################################################

def ComputeRunLumi(dic,run,lumiperlb):
    # consider ATLAS ready only
    global readylb
    total = 0.
    for lbl in lumiperlb:
        if lbl not in readylb:
            continue
        total += lumiperlb[lbl]
    return total


def GetLBLumi(dic,run):
    global lumifolder
    global usenb
    
    lumifolder = ''
    lumiperlb = {}
    for k,v in dic.items():
        if 'ofllumi' not in k.ResultKey:
            continue
        lumifolder = k.ResultKey
    if lumifolder == '':
        return lumiperlb

    lbdic = dic[DataKey(lumifolder)][run]
    lbtime = dic[DataKey('#LB')][run][1]

    for i,item in enumerate(lbdic):
        for lb in range(int(item.startlb),int(item.endlb)):
            lumiperlb[lb] = 0.
            if item.value == "n.a.":
                continue
            if (lbtime[lb]-lbtime[lb-1])<0:
                continue
            # change unit (pb/nb)
            scale = 0.001
            if usenb:
                scale = 1.
            lumiperlb[lb] = float(item.value)*(lbtime[lb]-lbtime[lb-1])*0.001*scale # output in nb-1/pb-1
            #print ("DEBUG",run,lb,float(item.value),lbtime[lb]-lbtime[lb-1])
    return lumiperlb

def GetLBLiveFraction(dic,run):
    global livetrigger
    livefraction = {}
    #trigrate = dic[DataKey('TriggerRates')][run][0].value['L1_EM5']
    triggerRates = dic[DataKey('TriggerRates')][run][0].value
    if triggerRates != "No match":
        try:
            trigrate = triggerRates[livetrigger]
            for item in trigrate:
                (lb, tbp, tap, tav) = map(int, item)
                livefraction[lb]= 0.
                if tap != 0:
                    livefraction[lb] = float(tav)/tap
        except KeyError:
            print ("%s trigger not found - no live fraction computation" % livetrigger)
    return livefraction

def GetLBReady(dic,run):
    rlb = []
    for b in dic[DataKey('Ready for physics')][run]:
        for lb in range(b.startlb,b.endlb):
            if b.value == "1" :
                rlb.append(lb)
    return rlb
    

########################################################################
def MakePlot_SummaryLumiLoss(loss,colors,dicsum,name):

    global usenb
    global unit

    SetStyle()
    gStyle.SetTitleX(0.5)
    gStyle.SetTitleAlign(23)

    ## ATLAS Ready Recorded Lumi
    TotalLumi = dicsum[DataKey('TotalLumi')][0]

    ## x labels
    xlabels = [s for s in loss]

    ## runs
    runs = [item for item in loss[xlabels[0]]]
    runs.sort()

    ## compute TOTAL lumi per system
    lumi = {}
    max = 0.
    for sys in loss:
        lumi[sys] = 0.
        for r in runs:
            lumi[sys] += loss[sys][r]
        if lumi[sys] > max:
            max=lumi[sys]
    max = max*1.5

    # if no entry, print no plot
    empty = False
    if max == 0.:
        empty = True
        max = 1.
        

    ## create frame
    t1 = "       Runs [%i-%i]"%(runs[0],runs[-1])
    if len(runs) == 1:
        t1 = "            Run [%i]"%runs[0]
    t2 = "ATLAS Ready Lumi: %.2f %s"%(TotalLumi,unit)
    h1_frame = TH1F("h1_frame","#splitline{%s}{%s}"%(t1,t2),len(loss),-0.5,len(loss)-0.5)
    SetXLabel(h1_frame,xlabels)
    h1_frame.SetMinimum(0.)
    h1_frame.SetMaximum(max)
    h1_frame.SetTickLength(0.,"X")
    h1_frame.GetYaxis().SetTitle("Luminosity Loss during ATLAS Ready [%s]"%unit)
    h1_frame.LabelsOption("a","X") # order X label by alphabetical order

    ## create systems plots 
    h1_lumi = []
    text_lumi = []
    for sys in loss:
        h1_lumi.append(TH1F("h1_lumi_%s"%sys,"",len(loss),-0.5,len(loss)-0.5))
        SetXLabel(h1_lumi[-1],xlabels)
        h1_lumi[-1].LabelsOption("a","X") # order X label by alphabetical order
        h1_lumi[-1].SetBarWidth(0.90)
        h1_lumi[-1].SetTickLength(0.,"X")
        h1_lumi[-1].SetMinimum(0.)
        h1_lumi[-1].SetMaximum(max)
        h1_lumi[-1].SetFillColor(colors[sys])
        if sys == '_TOTAL':
            h1_lumi[-1].SetFillStyle(3244)
        ibin = h1_lumi[-1].GetXaxis().FindBin(sys)
        #print (sys,ibin,colors[sys],lumi[sys]/TotalLumi,h1_lumi[-1].GetName())
        h1_lumi[-1].SetBinContent(ibin,lumi[sys])
        if lumi[sys] > 0. :
            text_lumi.append(TText(lumi[sys]*1.1,ibin-1,"%.2f %%"%(100*lumi[sys]/TotalLumi)))
        else:
            text_lumi.append(TText(lumi[sys]*1.1,ibin-1,""))

    
    ## create png
    canvas = TCanvas( 'c_SummaryLumiLoss',"Summary Lumi Losses", 200, 10, 600, 700)
    canvas.Divide(1,1)
    canvas.cd(1)
    gPad.SetTopMargin(0.15)
    h1_frame.Draw("HBAR")
    subtitle = TText(max,0.,name)
    subtitle.SetTextSize(0.03)
    subtitle.SetTextAngle(90)
    subtitle.SetTextColor(kGray+1)
    subtitle.Draw()
    if empty:
        bgtext = TText(0.5,round(len(loss)/2)-0.5,"No Luminosity Losses")
        bgtext.SetTextAlign(22)
        bgtext.SetTextColor(kRed)
        bgtext.SetTextFont(43)
        bgtext.SetTextSize(40)
        bgtext.SetTextAngle(45)
        bgtext.Draw()
        
    for i,h in enumerate(h1_lumi):
        text_lumi[i].SetTextAlign(12) # Vertical align
        text_lumi[i].SetTextSize(0.04) # Text Size
        if not empty:
            h.Draw("HBAR SAME")
            text_lumi[i].Draw()
    canvas.Update()
    from CoolRunQuery.AtlRunQueryQueryConfig import QC
    pname = QC.datapath+'/'+name+'_lumiloss.png'
    canvas.Print(pname)
    return pname

def MakePlot_PerRunLumiLoss(loss,colors,dicsum,name):

    global usenb
    global unit

    ## get runs list - remove runs with no defect
    runs = []
    for r in loss['_TOTAL']:
        if loss['_TOTAL'][r]>0. :
            runs.append(r)
    #if len(runs) == 0 : return
    empty = False
    if len(runs) == 0 :
        empty = True
        runs.append("-")
    runs.sort()

    ## ATLAS Ready Recorded Lumi
    TotalLumi = dicsum[DataKey('TotalLumi')][0]

    ## center histo Title
    SetStyle()
    gStyle.SetTitleX(0.5)
    gStyle.SetTitleAlign(23)
    
    ## create stack
    h_stack = THStack("hs_PerRun","Luminosity Loss during ATLAS Ready [%s]"%unit)

    ## create legends
    x0=0.17
    y0=0.62
    nsplits = 4
    dx = 0.2
    dy = 0.05
    leg = TLegend(x0,y0,x0+dx,y0+nsplits*dy)
    nhists = 0

    ## settings for BAR histos
    SHIFT = 0.1
    WIDTH = 0.4
    
    ## create total plot 
    h1_total = TH1F("h1_total","",len(runs),-0.5,len(runs)-0.5)
    h1_total.SetMinimum(0.)
    h1_total.SetFillColor(colors['_TOTAL'])
    h1_total.SetFillStyle(3244)
    h1_total.SetBarWidth(WIDTH)
    h1_total.SetBarOffset(SHIFT+WIDTH)

    for r,run in enumerate(runs):
        if run != "-" and loss['_TOTAL'][run] > 0:
            h1_total.Fill(r,loss['_TOTAL'][run])
    nhists +=1
    leg.AddEntry(h1_total,'TOTAL',"f")
                
    ## create systems plots 
    h1_lumi = {}    
    for i,sys in enumerate(loss):
            
        if sys == '_TOTAL':
            continue
        
        h1_lumi[sys] = TH1F("h1_lumi_%s"%sys,"",len(runs),-0.5,len(runs)-0.5)
        h1_lumi[sys].SetMinimum(0.)
        h1_lumi[sys].SetFillColor(colors[sys])
        h1_lumi[sys].SetBarWidth(WIDTH)
        h1_lumi[sys].SetBarOffset(SHIFT)
        for r,run in enumerate(runs):
            if run != "-" and loss[sys][run]> 0.:
                h1_lumi[sys].Fill(r,loss[sys][run])
                
    ## Add to stack
    for sys in h1_lumi:
        if h1_lumi[sys].GetEntries() > 0:
            h_stack.Add(h1_lumi[sys])
            leg.AddEntry(h1_lumi[sys],sys,"f")
            nhists +=1
            
    ## hmax 
    hmax = h_stack.GetMaximum()*1.6

    ## create frame
    if len(runs) == 1:
        if runs[0] == "-":
            t1 = "            Run [-]"
        else:
            t1 = "            Run [%i]"%runs[0]  
    else:
        t1 = "       Runs [%i-%i]"%(runs[0],runs[-1])

    t2 = "ATLAS Ready Lumi: %.2f %s"%(TotalLumi,unit)
    h1_frame = TH1F("h1_frame_runs","#splitline{%s}{%s}"%(t1,t2),len(runs),-0.5,len(runs)-0.5)
    SetXLabel(h1_frame,runs)
    h1_frame.SetLabelSize(0.06,"X")
    h1_frame.SetMinimum(0.)
    h1_frame.GetYaxis().SetTitle("Luminosity Loss during ATLAS Ready [%s]"%unit) 
    h1_frame.GetYaxis().SetTitleOffset(1.3)

    ## create png
    canvas = TCanvas( 'c_PerRunLumiLoss',"Luminosity Losses Per Run", 200, 10, 600, 700)
    canvas.Divide(1,1)
    canvas.cd(1)
    gPad.SetTopMargin(0.15)
    h1_frame.SetMaximum(hmax)
    h_stack.SetMaximum(hmax)
    #h_total.SetBarOffset((shift)
    
    h1_frame.Draw("")
    h_stack.Draw("BARsame")
    h1_total.Draw("BARsame")
    ncolumns = int(math.ceil(float(nhists)/float(nsplits)))
    leg.SetNColumns(ncolumns)
    leg.Paint() # Draw once to access coordinates
    leg.SetX2NDC(leg.GetX1NDC()+ncolumns*dx) # Modify legend size w.r.t number of columns
    nrows = leg.GetNRows()
    leg.SetY2NDC(leg.GetY1NDC()+nrows*dy) # Modify legend size w.r.t number of rows
    if not empty:
        leg.Draw() # Draw again

    subtitle = TText(len(runs)-0.35,0.,name)
    subtitle.SetTextSize(0.03)
    subtitle.SetTextAngle(90)
    subtitle.SetTextColor(kGray+1)
    subtitle.Draw()
    if empty:
        bgtext = TText(0.0,0.5,"No Luminosity Losses")
        bgtext.SetTextAlign(22)
        bgtext.SetTextColor(kRed)
        bgtext.SetTextFont(43)
        bgtext.SetTextSize(40)
        bgtext.SetTextAngle(45)
        bgtext.Draw()

    canvas.Update()
    from CoolRunQuery.AtlRunQueryQueryConfig import QC
    pname = QC.datapath+'/'+name+'_lumiloss_perrun.png'
    canvas.Print(pname)
    return pname

def MakePlot_DefectsPerSystem(sys,intolerable,tolerable,ignored,dic,run):

    global readylb

    from CoolRunQuery.AtlRunQueryQueryConfig import QC
    
    hname = ['','','']
    h2_DefectVsLB = ['','','','']

    ## style
    SetStyle()
    gStyle.SetTitleX(0.5)
    gStyle.SetTitleAlign(23)

    lbrange = readylb[-1]-readylb[0] # can be disconnected !
    #print (run,len(readylb),lbrange)
    
    ## Intolerable ##
    if len(intolerable) > 0 :
        #TCol = TColor.GetColor( "#5D6B7D")
        TCol = TColor.GetColor( "#0f3a60")
        hname[0]= "%s_Intolerable_Run%s"%(sys,str(dic[DataKey('Run')][run]))
        htitle = "%s - Intolerable defects - Run %s"%(sys,str(dic[DataKey('Run')][run]))
        h2_DefectVsLB[0] = TH2F("h2_defectVsLB_%s"%hname[0],htitle,
                                lbrange,readylb[0]-0.5,readylb[-1]+0.5,
                                len(intolerable),0.,len(intolerable))
        h2_DefectVsLB[0].SetFillColor(TCol)
        h2_DefectVsLB[0].GetXaxis().SetTitle("Lumiblocks with ATLAS Ready")
        SetYLabel(h2_DefectVsLB[0],[defect for defect in intolerable])
        h2_DefectVsLB[0].LabelsOption("a","Y") # order Y label by alphabetical order
    
        # Text to store lumi loss number
        ttext = {}
        # Fill & Compute lumi losses
        for i,item in enumerate(intolerable):
            ibiny = h2_DefectVsLB[0].GetYaxis().FindBin(item)
            binxmax = h2_DefectVsLB[0].GetXaxis().GetXmax()
            frac = 100*float(len(intolerable[item]))/float(len(readylb))
            if frac> 0.:
                ttext[item] = TText(binxmax,ibiny-0.8,"  %.2f %%"%frac)
            else:
                ttext[item] = TText(binxmax,ibiny-0.8,"")
            ttext[item].SetTextSize(0.03)
            ttext[item].SetTextColor(TCol)
            for lb in intolerable[item]:
                if lb not in readylb:
                    continue
                ibinx = h2_DefectVsLB[0].GetXaxis().FindBin(lb)
                h2_DefectVsLB[0].SetBinContent(ibinx,ibiny,1.)
        
        # create png
        canvas = TCanvas( 'c_Int',"Systems Defects - %s"%hname[0], 200, 10, 1000, 800)
        canvas.Divide(1,1)
        canvas.cd(1)
        gPad.SetLeftMargin(0.40)
        gPad.SetRightMargin(0.1)
        h2_DefectVsLB[0].Draw("BOX")
        for item in intolerable:
            ttext[item].Draw()
        canvas.Update()
        hname[0] = QC.datapath+"/"+hname[0]+".png"
        canvas.Print(hname[0])
        
    ## Tolerable ##
    if len(tolerable) > 0 :

        all = {**tolerable, **intolerable, **ignored}

        TCol1 = TColor.GetColor("#0f3a60")
        TCol2 = TColor.GetColor("#a3c0d9")
        TCol3 = TColor.GetColor("#4681b3")
        #TCol1 = TColor.GetColor("#015669")
        #TCol2 = TColor.GetColor("#5cbad0")
        #TCol3 = TColor.GetColor("#258aa1")
        hname[1]= "%s_All_Run%s"%(sys,str(dic[DataKey('Run')][run]))
        htitle = "%s - All defects - Run %s"%(sys,str(dic[DataKey('Run')][run]))
        
        # intolerable
        h2_DefectVsLB[1] = TH2F("h2_defectVsLB_int_%s"%hname[1],htitle,
                                lbrange,readylb[0]-0.5,readylb[-1]+0.5,
                                len(all),0.,len(all))
        # tolerable
        h2_DefectVsLB[2] = TH2F("h2_defectVsLB_tol_%s"%hname[1],htitle,
                                lbrange,readylb[0]-0.5,readylb[-1]+0.5,
                                len(all),0.,len(all))
        
        # ignored
        h2_DefectVsLB[3] = TH2F("h2_defectVsLB_ign_%s"%hname[1],htitle,
                                lbrange,readylb[0]-0.5,readylb[-1]+0.5,
                                len(all),0.,len(all))
        
        h2_DefectVsLB[1].SetFillColor(TCol1)
        h2_DefectVsLB[2].SetFillColor(TCol2)
        h2_DefectVsLB[3].SetFillColor(TCol3)
        #h2_DefectVsLB[3].SetFillStyle(3004) # not a great idea for single LBs
        h2_DefectVsLB[1].GetXaxis().SetTitle("Lumiblocks with ATLAS Ready")
        h2_DefectVsLB[2].GetXaxis().SetTitle("Lumiblocks with ATLAS Ready")
        h2_DefectVsLB[3].GetXaxis().SetTitle("Lumiblocks with ATLAS Ready")
        SetYLabel(h2_DefectVsLB[1],[defect for defect in all])
        SetYLabel(h2_DefectVsLB[2],[defect for defect in all])
        SetYLabel(h2_DefectVsLB[3],[defect for defect in all])
        h2_DefectVsLB[1].LabelsOption("a","Y") # order Y label by alphabetical order
        h2_DefectVsLB[2].LabelsOption("a","Y") # order Y label by alphabetical order
        h2_DefectVsLB[3].LabelsOption("a","Y") # order Y label by alphabetical order

        # Text to store lumi loss number
        ttext = {}
        
        # Fill 
        for i,item in enumerate(all):
            ibiny = h2_DefectVsLB[1].GetYaxis().FindBin(item)
            binxmax = h2_DefectVsLB[1].GetXaxis().GetXmax()
            frac = 100*float(len(all[item]))/float(len(readylb))
            if frac >0.:
                ttext[item] = TText(binxmax,ibiny-0.8,"  %.2f %%"%frac)
            else:
                ttext[item] = TText(binxmax,ibiny-0.8,"")
            ttext[item].SetTextSize(0.03)
            if item in intolerable:
                ttext[item].SetTextColor(TCol1)
            if item in tolerable:
                ttext[item].SetTextColor(TCol2)
            if item in ignored:
                ttext[item].SetTextColor(TCol3)    
            for lb in all[item]:
                ibinx = h2_DefectVsLB[1].GetXaxis().FindBin(lb)
                if item in intolerable:
                    h2_DefectVsLB[1].SetBinContent(ibinx,ibiny,1.)
                if item in tolerable:
                    h2_DefectVsLB[2].SetBinContent(ibinx,ibiny,1.)
                if item in ignored:
                    h2_DefectVsLB[3].SetBinContent(ibinx,ibiny,1.)

        # create png
        canvas = TCanvas( 'c_Tol',"Systems Defects - %s"%hname[1], 200, 10, 1000, 800)
        canvas.Divide(1,1)
        canvas.cd(1)
        gPad.SetLeftMargin(0.40)
        gPad.SetRightMargin(0.1)
        h2_DefectVsLB[2].Draw("BOX")
        h2_DefectVsLB[3].Draw("BOXSAME")
        h2_DefectVsLB[1].Draw("BOXSAME")

        for item in all:
            ttext[item].Draw()
        canvas.Update()
        hname[1] = QC.datapath+"/"+hname[1]+".png"
        canvas.Print(hname[1])

    return hname

########################################################################
def SetXLabel(h,labels):
    for i in range(len(labels)):
         h.GetXaxis().SetBinLabel(i+1,str(labels[i]))
    return

def SetYLabel(h,labels):
    for i in range(len(labels)):
         h.GetYaxis().SetBinLabel(i+1,str(labels[i]))
    return

def listify(lbl): # compactify lb lists
    if len(lbl) == 0 :
        return ''
    i = 0
    newlist=[]
    while i < len(lbl):
        start=lbl[i]
        j=i
        while(j+1<len(lbl) and lbl[j+1]==lbl[j]+1):
            j+=1
        end=lbl[j]
        i=j+1
        if start==end:
            newlist.append(start)
        else:
            newlist.append(str(start)+'-'+str(end))
    return  ", ".join([str(x) for x in newlist])
########################################################################

class DQSummary:

    @classmethod
    def makeHTML(cls, dic, dicsum, doPickle=True, doDQSummary=True, doDQPlots=True, dqsumGRL="PHYS_StandardGRL_All_Good_25ns", dbbTag=("HEAD", "HEAD")):
        """ method returns a string (unicode is fine) of html code, with out tag <div>...</div> """

        #####################################################################
        # Jessica, your main code goes here                                 #
        # (note that all methods in the class are 'static' -> @classmethod  #
        #####################################################################
        
        ### WARNING messages
        warning = ''

        ### Global Variables ###
        with timer("DQSummary"):

            ### loop once over runs to decide between pb/nb###
            global usenb
            usenb = False
            global livetrigger
            livetrigger ='L1_EM30'
            for r,run in enumerate(dic[DataKey('Run')]):
                ptag = dic[DataKey('Project tag')][r][0].value
                if 'data13' in ptag:
                    usenb = True
                    livetrigger = 'L1_EM5'
                elif 'data16_hip5TeV' in ptag or 'data16_hip8TeV' in ptag:
                    usenb = True
                    livetrigger = 'L1_EM12'
                elif 'data15' in ptag or 'data16' in ptag or 'data17' in ptag or 'data18' in ptag:
                    usenb = False
                    livetrigger = 'L1_EM12'

            ## Retrieve DQ primary defects relative to GRL
            # Check if defect data base has correct format (should be always the case)
            if len(dbbTag) != 2:
                warning += '<font color="red"> WARNING (DQSummary): The defect database tag "%s" must be a 2-element sequence. Will use "HEAD" tag instead!</font><br>' %(dbbTag)
                dbbTag = ("HEAD", "HEAD")
            #Create dbb dummy with HEAD tag
            ddb = DefectsDB()
            #check if defect tag is defined in defect database
            if dbbTag[0] not in ['HEAD'] + ddb.defects_tags:
                warning += '<font color="red"> WARNING (DQSummary): The defined defect tag "%s" is not defined in defect database. Will use "HEAD" tag instead!</font><br>' %(dbbTag[0])
                dbbTag = ("HEAD", dbbTag[1])
            #check if defect and logic tag is defined in defect database
            if dbbTag[1] not in ['HEAD'] + ddb.logics_tags:
                warning += '<font color="red"> WARNING (DQSummary): The defined logic tag "%s" is not defined in defect database. Will use "HEAD" tag instead!</font><br>' %(dbbTag[1])
                dbbTag = (dbbTag[0], "HEAD")
            #Now set tags
            ddb = DefectsDB(tag=(dbbTag[0], dbbTag[1]))
            #Check if virtual defect is defined in logic
            if dqsumGRL in ddb.virtual_defect_logics.keys():
                defects_primary_grl = ddb.resolve_primary_defects(ddb._resolve_evaluation_order([dqsumGRL]))
            else:
                warning += '<font color="red"> WARNING (DQSummary): The defined virtual defect or GRL "%s" is not defined in logic. Will use no GRL for calculation!</font><br>' %(dqsumGRL)
                dqsumGRL = 'no'

            #Check if cosmic run
            cosmics = False
            if 'Cosmics' in dqsumGRL:
                cosmics = True
           
            global unit
            unit = 'pb^{-1}'
            if usenb:
                unit ='nb^{-1}'
            if cosmics:
                unit = '%'
    
            ### Total Lumi : 0 = Recorded  1 = Loss (in ATLAS Ready)
            dicsum[DataKey('TotalLumi')] = [0.,0.]

            ### ATLAS not Ready Total/Per run (in stable Beams)
            dicsum[DataKey('TotalNotReady')] = 0.
            dicsum[DataKey('TotalBusy')] = 0.
            
            ### Initialize Detectors Variables
            detectors = ['ALFA','LCD','ZDC','BCM','PIXEL','SCT','TRT','LAR','TILE',
                         'MS_CSC','MS_RPC','MS_TGC','MS_MDT','MBTS','TRIG','GLOBAL']
            detectors_color = {'ALFA':kYellow-9,'LCD':kYellow-7,'ZDC':kYellow-4,
                               'BCM':kOrange+1,'PIXEL':kOrange-3,'SCT':kOrange+7,'TRT':kRed-3,
                               'LAR':kRed+2,'TILE':kPink-7,
                               'MS_CSC':kBlue+4,'MS_RPC':kBlue-2,'MS_MDT':kAzure+3,'MS_TGC':kBlue-9,'MBTS':kBlue-6,
                               'TRIG':kGreen-3,'GLOBAL':kGreen+3,'_TOTAL':TColor.GetColor("#585858")}
            detectors_lumiloss = {}
            detectors_affectedLBs = {}

            for d in detectors:
                detectors_lumiloss[d]={}
                detectors_affectedLBs[d]={}

            ## Additional field for TOTAL
            detectors_lumiloss['_TOTAL']={}
            detectors_affectedLBs['_TOTAL']={}
                
            ### Initialize Performances Variables
            performances = ['LUMI','ID','CALO','TAU','EGAMMA','JET','MET','MCP','BTAG']
            performances_color = {'LUMI':kYellow-9,'ID':kOrange+7,'CALO':kRed+2,'TAU':kBlue+2,
                                  'EGAMMA':kGreen-3,'JET':kAzure+3,'MET':kGreen-6,'MCP':kBlue-3,
                                  'BTAG':kPink,'_TOTAL':TColor.GetColor("#585858")}
            performances_lumiloss = {}
            performances_affectedLBs = {}

            for p in performances:
                performances_lumiloss[p]={}
                performances_affectedLBs[p]={}

            ## Additional field for TOTAL
            performances_lumiloss['_TOTAL']={}
            performances_affectedLBs['_TOTAL']={}

            ### Initialize table content // Considered systems ###
            columns = []
            global_systems = []
            columns = ['Run Info','ES1','BLK']
            global_systems = ['Calo','Tracking','Muon','Trigger & Lumi']
            columns += global_systems 


        with timer('Create summary results table'):
            ### create big results table ###
            summarytable = '<div><table id="resulttable" class="resulttable" style="margin-left: 13px; margin-right: 13px;" >'
            ### create first title row ###
            summarytable += '<tr>'
            summarytable += '<th colspan="1"></th>' # run info column

            #summarytable += '<th colspan="2">Tier0 Reco Status</th>'
            summarytable += '<th colspan="2">Missing Sign-Off</th>'

            summarytable += '<th colspan="%i">Defects in ATLAS Ready<br>'%len(global_systems)
            summarytable += '<div style="font-size: xx-small; cursor: pointer;" onclick="toggle_dq(this)">[show tolerable]</div></th>'
            summarytable += '</tr>'

            ### create subtitle row ###
            summarytable += '<tr>'
            #for item in columns: summarytable += '<th>%s</th>'%item.split(":")[0]
            for item in columns:
                summarytable += '<th>%s</th>'%item
            summarytable += '</tr>'

            totalNumberOfReadyLB = 0
            
            tot_nlb = 0
            #determine total number of lbs in case of cosmics
            if cosmics:
                for r,run in enumerate(dic[DataKey('Run')]):
                    tot_nlb += dic[DataKey('#LB')][r][0]

            runsWithoutReadyForPhysics = []
            runsWithoutOfflineLumiInfo = []

            ### loop over runs ###
            for r,run in enumerate(dic[DataKey('Run')]):

                ## Get list of ATLAS Ready LBs
                global readylb
                #ignore ATLAS not ready for Cosmics
                nlb = dic[DataKey('#LB')][r][0]

                if cosmics:
                    readylb = range(1,nlb+1)
                else:
                    readylb = GetLBReady(dic,r)

                totalNumberOfReadyLB += len(readylb)

                ## If no ATLAS ready LB: skip the run
                if len(readylb) == 0:
                    runsWithoutReadyForPhysics += [ str(run) ]
                    continue

                ## Get lumi per LB and live fraction for the current run
                #for cosmic use fractional lb
                if cosmics:
                    lumiperlb = dict(zip(readylb, [100./tot_nlb] * nlb))
                    livefrac = dict(zip(readylb, [1] * nlb))
                else:
                    lumiperlb = GetLBLumi(dic,r)                  
                    livefrac = GetLBLiveFraction(dic,r)

                if len(lumiperlb) == 0:
                    runsWithoutOfflineLumiInfo += [ str(run) ]
                    continue

                ## Correct lumi per LB with live fraction
                #print ('DEBUG',run,len(lumiperlb),len(livefrac),len(readylb))
                for lbl in lumiperlb:
                    # IF statement used when len(lumiperlb)!= len(livefrac)
                    if lbl not in readylb:
                        continue
                    if lbl not in livefrac:
                        pass # in run 3 we don't have trigger rates yet, as they are no longer in COOL
                        # print ("--> Warning: live fraction not available for LB %i. Setting live fraction to 1." % lbl)
                    else:
                        lumiperlb[lbl]*=livefrac[lbl]


                ## Initialize columns content for current run            
                content = {}
                for i,item in enumerate(columns):
                    content[item]=''

                ## Retrieve and store run info
                dp = '?'
                ptag = dic[DataKey('Project tag')][r][0].value
                year = '20'+ptag.split('_')[0][4:]
                DP = ARQ_COMA.get_periods_for_run(run)
                if len(DP)>0:
                    dp=DP[0]
                content['Run Info'] = '<b>%s</b>&nbsp;<font style="font-size:10px;">data period <b>%s</b></font><br>'%(str(run),dp)

                ### In case some systems should be removed
                tobeignored = ['ALFA','ZDC','LCD']
                import re
                if re.match('data1[3-9]', ptag[0:6]):
                    tobeignored = []

                ## useful links
                target='target="blank"'
                somelinks = '<font style="font-size:10px;color:#0000FF">'
                somelinks += '<a href="https://atlas-tagservices.cern.ch/tagservices/RunBrowser/runBrowserReport/runBrowserReport.php?runs=%s&pn=%s&fnt=%s" %s>COMA</a>'%(run,dp,ptag,target)
                somelinks += ', <a href="https://atlasdqm.web.cern.ch/atlasdqm/DQBrowser/makeMatrix.php?selection=All&run=%s&runup=&dbinstance=CONDBR2_DCS&tag=DEFAULT" %s>DCS</a>'%(run,target)
                somelinks += ', <a href="https://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/DataSummary/%s/run.py?run=%s" %s>DS</a>'%(year,run,target)
                somelinks += ', <a href="https://atlasdqm.cern.ch/defectentry/?run=%s" %s>defects</a>'%(run,target)
                somelinks += '<br><a href="https://atlasdqm.cern.ch/dqsignoff/%s/" %s>DQlogbook</a>'%(run,target)
                somelinks += ', <a href="https://atlasdqm.cern.ch/webdisplay/tier0?lowrun=%s&highrun=%s&show_all=on" %s>DQWebDisplay</a>'%(run,run,target)
                somelinks += '</font><br>'

                content['Run Info'] += somelinks

                ## Start and End time
                rundate = dic[DataKey('Start and endtime')][r].split(",")
                tstart = time.strftime("%a, %d %b %Y %H:%M:%S",time.localtime(float(rundate[0])))
                tend = time.strftime("%a, %d %b %Y %H:%M:%S",time.localtime(float(rundate[1])))
                content['Run Info'] += '<font style="font-size:10px;">'
                content['Run Info'] += '<br>Start:&nbsp;<font style="color:#4C7D7E;">%s</font>'%tstart
                content['Run Info'] += '<br>End:&nbsp;<font style="color:#4C7D7E;">%s</font>'%tend
                content['Run Info'] += '</font>'

                #nevents = str(dic[DataKey('#Events')][r][0].value)
                #content['Run Info'] += 'Recorded events: '+nevents+'<br>'
                #content['Run Info'] += 'Number of LBs : '+dic[DataKey('#LB')][r][0]+'<br>'
                #content['Run Info'] += 'LHC Fill : %s<br>'%(dic[DataKey('lhc:fillnumber')][r][0].value)+'<br>'

                ## Add stable Beam / ATLAS Ready info in lhctable
                lhctable = '<br><font style="font-size:10px;">'
                lhctable += '<table><tr><td>'
                lhctable += '<b>Stable&nbsp;Beam</b><br>%s'%'<br>'.join([ '%s-%s:&nbsp<font style="color:#4C7D7E">%s</font>'%(b.startlb,b.endlb,b.value) for b in dic[DataKey('lhc:stablebeams')][r]])
                lhctable += '</td><td>'
                lhctable += '<b>ATLAS&nbsp;Ready</b><br>%s'% '<br>'.join([ '%s-%s:&nbsp<font style="color:#4C7D7E">%s</font>'%(b.startlb,b.endlb,b.value) for b in dic[DataKey('Ready for physics')][r]])
                lhctable += '</td></tr></table>'
                lhctable += '</font>'
                content['Run Info'] += lhctable

                ## Add Total Lumi
                lumitot = ComputeRunLumi(dic,r,lumiperlb)
                dicsum[DataKey('TotalLumi')][0]+= lumitot
                content['Run Info'] += '<font style="font-size:10px;">'
                content['Run Info'] += 'Ready Recorded: <b>%.2f</b> %s'%(lumitot,unit)
                content['Run Info'] += '</font><br>'

                ## Retrieve DQ defects
                defects = dic[DataKey('DQ')][r]
                ## Select only primary flags
                defects_primary = [d for d in defects if d.value.primary]
                
                ## Initialize list of affected LBs per detector/cp
                ## Initialize lumi losses due to intolerable defects
                for d in detectors:
                    detectors_lumiloss[d][run]= 0.
                    detectors_affectedLBs[d][run]=[]
                for p in performances:
                    performances_lumiloss[p][run]=0.
                    performances_affectedLBs[p][run]=[]

                # Total per run
                detectors_lumiloss['_TOTAL'][run]= 0.
                detectors_affectedLBs['_TOTAL'][run]=[]
                performances_lumiloss['_TOTAL'][run]= 0.
                performances_affectedLBs['_TOTAL'][run]=[]

                # And the big sums
                total_affectedLBs = []
                GlobalNotReady = []
                GlobalBusy = []

                ## Store list of defects and affected LBs to print systems table
                ListOfIntolerableDefects = {}
                ListOfTolerableDefects = {}
                ListOfIgnoredDefects = {}

                ## Loop over defects
                for item in defects_primary:
                    defect = item.value.defect
                    comment = item.value.comment
                    user = item.value.user
                    startlb = item.startlb
                    endlb = item.endlb
                    tolerable = item.value.tolerable
                    system = MapToSystem(defect)
                    grl_ignored = False
                    
                    #Make it relative to GRL
                    if not dqsumGRL == 'no' and not tolerable:
                        if defect not in defects_primary_grl:
                            grl_ignored = True


                    if 'GLOBAL_NOTREADY' in defect:
                        GlobalNotReady += [ lb for lb in range(startlb,endlb)]

                    if 'GLOBAL_BUSY' in defect:
                        GlobalBusy += [ lb for lb in range(startlb,endlb) if lb in readylb]

                    ## Missing Sign-Offs ##

                    ## FINAL sign-off
                    if 'UNCHECKED_FINAL' in defect:
                        continue

                    ## BULK sign-off
                    if 'BULK_UNCHECKED' in defect:
                        short = defect.split('_UNCHECKED')[0]
                        content['BLK']+='<font style="font-size:8px;font-weight:bold;">%s</font><br>'%short
                        continue
                    ## ES sign-off
                    if 'UNCHECKED' in defect:
                        short = defect.split('_UNCHECKED')[0]
                        content['ES1']+='<font style="font-size:8px;font-weight:bold;">%s</font><br>'%short
                        continue

                    ## Some cross-checks 
                    if system =='':
                        print ('run %s: this defect is fishy %s '%(run,defect))
                        continue

                    ## Some cross-checks 
                    word = defect.split('_')
                    cpdet = word[0]
                    if word[0]=='MS':
                        cpdet += "_"+word[1] # MS systems
                    if cpdet not in detectors and cpdet not in performances:
                        print ('This system is not included: %s (%s)'%(cpdet,defect))
                        continue

                    ## Store intolerable defects if in ATLAS Ready LBs only
                    if not tolerable:
                        RangeDefect = [ lb for lb in range(startlb,endlb) if lb in readylb ]
                        if len(RangeDefect)>0:
                            if not grl_ignored:
                                if defect not in ListOfIntolerableDefects:
                                    ListOfIntolerableDefects[defect]= [[],'']
                                    ListOfIntolerableDefects[defect][0] = RangeDefect
                                    ListOfIntolerableDefects[defect][1] = user+':'+comment
                                else:
                                    ListOfIntolerableDefects[defect][0]+= RangeDefect
                                    if comment not in ListOfIntolerableDefects[defect][1]:
                                        ListOfIntolerableDefects[defect][1]+= ' '+user+':'+comment
                                # This is used to compute lumilosses
                                # Here, we do not include systems "to be ignored"
                                if cpdet in tobeignored:
                                    continue
                                # Store list of affected LBs per detector/cp group
                                # we can have a double counting of LBs if defects overlap - fixed later
                                if cpdet in detectors:
                                    detectors_affectedLBs[cpdet][run]+=RangeDefect
                                if cpdet in performances:
                                    performances_affectedLBs[cpdet][run]+=RangeDefect
                                total_affectedLBs += RangeDefect
                            #treat differently if ignored by GRL although intolerable
                            elif grl_ignored:
                                if defect not in ListOfIgnoredDefects:
                                    ListOfIgnoredDefects[defect]= [[],'']
                                    ListOfIgnoredDefects[defect][0]= RangeDefect
                                    ListOfIgnoredDefects[defect][1]= '[%s]:%s '%(user,comment)
                                else:
                                    ListOfIgnoredDefects[defect][0]+= RangeDefect
                                    if comment not in ListOfIgnoredDefects[defect][1]:
                                        ListOfIgnoredDefects[defect][1] += '[%s]:%s '%(user,comment)                                
                    ## Store tolerable defects
                    else:
                        RangeDefect = [ lb for lb in range(startlb,endlb) if lb in readylb]
                        if len(RangeDefect)>0:
                            if defect not in ListOfTolerableDefects:
                                ListOfTolerableDefects[defect]= [[],'']
                                ListOfTolerableDefects[defect][0]= RangeDefect
                                ListOfTolerableDefects[defect][1]= '[%s]:%s '%(user,comment)
                            else:
                                ListOfTolerableDefects[defect][0]+= RangeDefect
                                if comment not in ListOfTolerableDefects[defect][1]:
                                    ListOfTolerableDefects[defect][1] += '[%s]:%s '%(user,comment)
                ## end of defects loop ##

                ## Create defects table for each system
                for item in global_systems:
                    content[item]='<table class="dqdefects">'

                for item in ListOfIntolerableDefects:
                    system = MapToSystem(item)
                    if system not in global_systems:
                        continue
                    lbs = listify(ListOfIntolerableDefects[item][0])
                    tip = ListOfIntolerableDefects[item][1]
                    tipkey = "dqcomment_int_%s_%s"%(item,run)
                    Run.addGlobalToolTip(tipkey,tip)
                    content[system]+='<tr class="showTip %s" >'%(tipkey)
                    content[system]+='<td class="intolerable">%s</td>'%(item)
                    content[system]+='<td class="lb">%s</td>'%(lbs)
                    content[system]+='</tr>'
                    
                for item in ListOfIgnoredDefects:
                    system = MapToSystem(item)
                    if system not in global_systems:
                        continue
                    lbs = listify(ListOfIgnoredDefects[item][0])
                    tip = ListOfIgnoredDefects[item][1]
                    tipkey = "dqcomment_ign_%s_%s"%(item,run)
                    Run.addGlobalToolTip(tipkey,tip)
                    content[system]+='<tr class="showTip %s" >'%(tipkey)
                    content[system]+='<td class="ignored">%s</td>'%(item)
                    content[system]+='<td class="lb">%s</td>'%(lbs)
                    content[system]+='</tr>'

                for item in ListOfTolerableDefects:
                    system =  MapToSystem(item)
                    if system not in global_systems:
                        continue
                    lbs = listify(ListOfTolerableDefects[item][0])
                    tip = ListOfTolerableDefects[item][1]
                    tipkey = "dqcomment_tol_%s_%s"%(item,run)
                    Run.addGlobalToolTip(tipkey,tip)
                    content[system]+='<tr class="showTip %s tolerable" style="visibility:collapse;">'%(tipkey)
                    content[system]+='<td>%s</td>'%(item)
                    content[system]+='<td class="lb">%s</td>'%(lbs)
                    content[system]+='</tr>'

                ## close systems defects tables
                for item in global_systems:
                    content[item]+='</table><br>'

                ## Add defects plots in each system column
                thumbsize = 70
                imgsize = 600
                for gsys in global_systems:
                    tol = {}
                    int = {}
                    ign = {}
                    for defect in ListOfTolerableDefects:
                        # remove systems to be ignored from the plots
                        word = defect.split('_')
                        cpdet = word[0]
                        if word[0]=='MS':
                            cpdet += "_"+word[1] # MS systems
                        if cpdet in tobeignored:
                            continue
                        if gsys==MapToSystem(defect):
                            tol[defect]=ListOfTolerableDefects[defect][0]
                    for defect in ListOfIntolerableDefects:
                        # remove systems to be ignored from the plots
                        word = defect.split('_')
                        cpdet = word[0]
                        if word[0]=='MS':
                            cpdet += "_"+word[1] # MS systems
                        if cpdet in tobeignored:
                            continue
                        if gsys==MapToSystem(defect):
                            int[defect]=ListOfIntolerableDefects[defect][0]
                    for defect in ListOfIgnoredDefects:
                        # remove systems to be ignored from the plots
                        word = defect.split('_')
                        cpdet = word[0]
                        if word[0]=='MS':
                            cpdet += "_"+word[1] # MS systems
                        if cpdet in tobeignored:
                            continue
                        if gsys==MapToSystem(defect):
                            ign[defect]=ListOfIgnoredDefects[defect][0]

                    hname = MakePlot_DefectsPerSystem(gsys,int,tol,ign,dic,r)
                    for h in hname:
                        if len(h)== 0:
                            continue
                        title = "Click to zoom"
                        wincontent = "<img src=&quot;%s&quot; height=%s/>"%(h,imgsize)
                        openwin = "javascript:openLargeWindow('Print1','"
                        openwin += "<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;>"
                        openwin += "<html xmlns:&quot;my&quot;><head><title>Defects for run %s</title></head>"%run
                        openwin += "<body style=&quot;background-color:#ffffff&quot;>%s</body></html>"%wincontent
                        openwin += "')"
                        content[gsys]+='<a title="%s" href="%s" ><img src="%s" height=%s/></a>'%(title,openwin,h,thumbsize)

                ## Compute Global not ready ##
                _lblumi = 0.
                if not GlobalNotReady:
                    for lb in GlobalNotReady:
                        _lblumi += lumiperlb[lb]
                dicsum[DataKey('TotalNotReady')] += _lblumi
                content['Run Info'] += '<font style="font-size:10px;">'
                content['Run Info'] += 'Global Not Ready: <b>%.2f</b> %s'%(_lblumi,unit)
                content['Run Info'] += '</font><br>'

                ## Compute Global Busy ##
                _lblumi = 0.
                for lb in GlobalBusy:
                    _lblumi += lumiperlb[lb]
                dicsum[DataKey('TotalBusy')]+= _lblumi
                content['Run Info'] += '<font style="font-size:10px;">'
                content['Run Info'] += 'Global Busy: <b>%.2f</b> %s'%(_lblumi,unit)
                content['Run Info'] += '</font><br>'

                ## Compute cp/det lumilosses for current run
                for d in detectors:
                    if len(detectors_affectedLBs[d][run]) == 0:
                        continue
                    # Remove double counting (defects overlap)
                    dll = list(set(detectors_affectedLBs[d][run]))
                    detectors_affectedLBs['_TOTAL'][run]+= dll
                    for lb in dll:
                        detectors_lumiloss[d][run]+= lumiperlb[lb]
                for p in performances:
                    if len(performances_affectedLBs[p][run]) == 0:
                        continue
                    # Remove double counting (defects overlap)
                    pll = list(set(performances_affectedLBs[p][run]))
                    performances_affectedLBs['_TOTAL'][run]+= pll
                    for lb in pll:
                        performances_lumiloss[p][run]+= lumiperlb[lb]

                ## Compute total Lumiloss for this run
                ## Remove double counting (defects overlap)
                detectors_affectedLBs['_TOTAL'][run]=list(set(detectors_affectedLBs['_TOTAL'][run]))
                performances_affectedLBs['_TOTAL'][run]=list(set(performances_affectedLBs['_TOTAL'][run]))
                total_affectedLBs = list(set(total_affectedLBs))
                totallossperrun = 0. 
                # Store the values
                for lb in total_affectedLBs:
                    totallossperrun += lumiperlb[lb]    
                dicsum[DataKey('TotalLumi')][1] +=  totallossperrun
                # Store the values
                for lb in  detectors_affectedLBs['_TOTAL'][run]:
                    detectors_lumiloss['_TOTAL'][run] +=  lumiperlb[lb]
                # Store the values
                for lb in  performances_affectedLBs['_TOTAL'][run]:
                    performances_lumiloss['_TOTAL'][run] += lumiperlb[lb]

                ## Add Total LumiLoss in run info column
                content['Run Info'] += '<font style="font-size:10px;">'
                content['Run Info'] += 'DQ Lumi Loss: <b>%.2f</b> %s'%(totallossperrun,unit) 
                content['Run Info'] += '</font><br>'

                ### Print run row ###
                summarytable+='<tr class="out2">'
                for item in columns:
                    summarytable+='<td>%s</td>'%content[item]
                summarytable+='</tr>'

            ### end of run loop ###
            summarytable += '</table></div><br>'            
            ### end of results table ###
            if runsWithoutReadyForPhysics:
                warning += '<div style="margin-left: 13px; color: darkred; width: 90%%;">WARNING! The following runs have no lumi block with ATLAS Ready: %s</div><br>' % ", ".join(runsWithoutReadyForPhysics)
            if runsWithoutOfflineLumiInfo:
                warning += '<div style="margin-left: 13px; color: darkred; width: 90%%;">WARNING! The following runs have no offline lumi info: %s</div><br>' % ", ".join(runsWithoutOfflineLumiInfo)



        ##########################
        ## Create Summary Plots ##
        ##########################

        summaryplot = ''
        plots = []
        if totalNumberOfReadyLB > 0:
            plots.append(MakePlot_SummaryLumiLoss(detectors_lumiloss,detectors_color,dicsum,'detectors'))
            plots.append(MakePlot_SummaryLumiLoss(performances_lumiloss,performances_color,dicsum,'performances'))
            plots.append(MakePlot_PerRunLumiLoss(detectors_lumiloss,detectors_color,dicsum,'detectors'))
            plots.append(MakePlot_PerRunLumiLoss(performances_lumiloss,performances_color,dicsum,'performances'))        
        
        imgsize=500
        thumbsize=300
        title = "Click to zoom"

        ## Start plots table at the top of the page
        summaryplot += '<table align="center" width="90%">'
        summaryplot += '<th>'

        for p in plots:
            
            wincontent = "<img src=&quot;%s&quot; width=%s/>"%(p,imgsize)
            
            #openwincmd = "javascript:openLargeWindow('Print%i','"%(r)
            openwincmd = "javascript:openWindow('Print%i','"%(run)
            openwincmd += "<!DOCTYPE html PUBLIC &quot;-//W3C//DTD HTML 4.01 Transitional//EN&quot;>"
            openwincmd += "<html xmlns:&quot;my&quot;><head><title>DQ Summary Plot - Run %s</title></head>"%(run)
            openwincmd += "<body style=&quot;background-color:#ffffff&quot;>%s</body></html>"%wincontent
            openwincmd += "')"
                
            summaryplot+='<td><a title="%s" href="%s" ><img src="%s" height=%s/></a></td>'%(title,openwincmd,p,thumbsize)

        ## End plots table
        summaryplot += '</th></table>'

        ## debug ##
        #s = "<div>%s</div>" % '<br>'.join([ "%s: %s" % (k.ResultKey,v) for k,v in dicsum.items()])
        #s = "<div>%s</div>" % '<br>'.join([ "%s: %s" % (k.ResultKey,v) for k,v in dic.items()])
        #print (s)

        ###########################
        ## Print Summary numbers ##
        ###########################

        totalLumiRecorded = dicsum[DataKey('TotalLumi')][0]
        totalLumiLoss = dicsum[DataKey('TotalLumi')][1]
        lumiLossFraction = (100. * totalLumiLoss / totalLumiRecorded) if totalLumiRecorded > 0 else 0


        print ('+++++++++++++++++++++++++ Summary +++++++++++++++++++')
        print ('  Total Ready Recorded Luminosity: %.2f %s-1' % ( totalLumiRecorded, unit ))
        print ('  Total Luminosity Loss (ATLAS Ready) : %.2f %s-1' % ( totalLumiLoss, unit ))
        print ('  Total Global Not Ready (Stable Beams): %.2f %s-1'%(dicsum[DataKey('TotalNotReady')],unit))
        print ('  Total Global Busy (Stable Beams): %.2f %s-1'%(dicsum[DataKey('TotalBusy')],unit))
        print ('+++++++++++++++++++++++++++++++++++++++++++++++++++++')

        global lumifolder

        if totalNumberOfReadyLB>0:
            summaryinfo =  '<table align="center" style="font-size:80%;"><tr>'
            summaryinfo += '<td> Total Luminosity Loss: <b>%.2f %s</b> (%.2f%%)' % ( totalLumiLoss, unit, lumiLossFraction )
            summaryinfo += '<br>Excluded Systems: %s</td></tr>' % tobeignored
            if not cosmics:
                summaryinfo += '<tr><td style="font-size:70%%">using %s // %s</td></tr>'%(lumifolder.split(':')[2],livetrigger)
            summaryinfo += '</table>'
        else:
            summaryinfo =  '<table align="center" style="font-size:80%;"><tr>'
            summaryinfo += '<td> No ready lumi block </td>'
            summaryinfo += '</tr></table>'

        #############################
        ## return web page content ##
        #############################
        
        pagecontent = summaryplot +'<br>'
        pagecontent += summaryinfo +'<br>'
        if doDQSummary:
            pagecontent += summarytable
        pagecontent += warning
        return pagecontent




if __name__ == "__main__":
    # for standalone tests (without querying) just run this and view under test.html
    
    import pickle
    QC.datapath = "data"

    pageinfo = pickle.load(open('%s/dqsum_pi.pickle' % QC.datapath))

    from CoolRunQuery.html.AtlRunQueryHTML import ResultPageMaker as RPM
    RPM.makePage(pageinfo, doDQPickle=False)

    
