# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
import sys, os
import math, time   , array

from ROOT import gROOT, gDirectory, gStyle, gPad
from ROOT import TFile
from ROOT import TH1I, TH2I, TF1
from ROOT import TH1F, TH2F, TGraph, TMultiGraph
from ROOT import TCanvas, TPad, TPaveLabel, TPaveText, TLegend

#from ROOT import TCanvas # available at startup
root_file = 'mems.root'  
freed_file = 'heapmon.freed.root'

hole_trace_start = 0

def iReport(file):
    global root_file
    root_file = file
    report()

def pReport():
    global root_file    
    memscanner = 'HeapMon.MemoryScanner' in sys.modules and \
    sys.modules[ 'HeapMon.MemoryScanner' ] or None    
    if memscanner:
        holes_file = memscanner.freescan_output
    #root_file = file
    report()

def findStaticHoles():
    global freed_file
    global hole_trace_start     
    tfile = TFile(freed_file, "READ")
    
    #mychain = gDirectory.Get( 'memTree' )    
    tree = tfile.Get("holeTree")
    entry_number = tree.GetEntries()
    print("There are ", entry_number , " entries in ", root_file)
    previous_scan_number = -1;
    
    holes_dict = dict()
    previous_holes_dict = dict()

    for i in xrange(entry_number):
        tree.GetEntry(i)
        if tree.scan_number < hole_trace_start:
            continue
        if previous_scan_number == tree.scan_number:
            #print "add new hole ", tree.hole_start, "size ", tree.hole_size
            holes_dict[tree.hole_start] = tree.hole_size;
        else:
            previous_scan_number = tree.scan_number
            if previous_holes_dict:
                print "looking for intersection with next scan dict"
                intersection = dict([(item,holes_dict[item]) for item in holes_dict.keys() if previous_holes_dict.has_key(item)])
                previous_holes_dict = intersection
                print len(intersection), " elements in intersection"
                #print "intersection=", intersection
            else:
                previous_holes_dict = holes_dict                
            
            holes_dict.clear()
    
    intersection = dict([(item,holes_dict[item]) for item in holes_dict.keys() if previous_holes_dict.has_key(item)])
    
    print "intersection=", intersection
    print len(intersection), " static holes identified"
    
    import cPickle
    pickle_file = "mems.dat"
    file = open(pickle_file, 'w')
    cPickle.dump(intersection, file)
    file.close()    
    del file
    

def report():
    global freed_file
    print'    HeapMonReport.report(): heapmon_file=', freed_file
    
    #findStaticHoles()
          
    tfile = TFile(freed_file, "READ")
    print "   root compression factor = ", tfile.GetCompressionFactor()
   
    mem_canvas = TCanvas("HeapMon_report", "Memory Holes Statistics", 10, 10, 800, 1034);
    mem_canvas.SetFillColor(17);
    mem_canvas.cd()
    
    pad1 = TPad("pad1","pad1",0.01,0.57,0.50,0.93,33);
    pad2 = TPad("pad2","pad2",0.51,0.57,0.99,0.93,33);
    pad3 = TPad("pad3","pad3",0.01,0.01,0.99,0.50,33);
    pad3.SetPhi(210);
    pad3.SetTheta(25);

    pad1.Draw(); pad2.Draw(); pad3.Draw();
    memTree = tfile.Get("holeTree")
    infoTree = tfile.Get("infoTree")
    mallocTree = tfile.Get("mallocTree")
    #holesm_th1i =   TH1I('holesm_th1i', 'Holes size evolution', fcount, 0, fcount)    
    #holesn_th1i =   TH1I('holesn_th1i', 'Holes number evolution', fcount, 0, fcount)  
    #total_th1i  =   TH1I('total_th1i', 'Total memory size evolution', fcount, 0, fcount)
    
    max_hole_size = memTree.GetMaximum("hole_size")
    print "    max_hole_size=", max_hole_size, "    min_hole_size", memTree.GetMinimum("hole_size")
    max_scan_number = memTree.GetMaximum("scan_number")
    print "     max_scan_number=", max_scan_number
    
    memHist1  = TH2I("mem2d","Hole-sizes distribution evolution", 128, -0.5, max_hole_size - 0.5, 50, 0, max_scan_number) 
    memTree.Project("mem2d", "scan_number:hole_size"); 
    
    multiGraph1 = TMultiGraph();
    multiGraph2 = TMultiGraph();

    print " memHist.GetMaximum() = ",    memHist1.GetMaximum();
    
    # Working on a Report
    gStyle.SetOptStat(0);
    gStyle.SetPalette(1);
    gStyle.SetCanvasColor(33);
    gStyle.SetFrameFillColor(18);
    
    memHist1.SetFillColor(30);
    memHist1.SetFillStyle(0);
    memHist1.GetXaxis().SetTitle("Size of holes, kb");
    memHist1.GetXaxis().SetLabelOffset(0.02);
    memHist1.GetXaxis().SetLabelSize(0.02);
    memHist1.GetXaxis().SetTitleSize(0.04);
    memHist1.GetXaxis().SetTitleColor(2);
    memHist1.GetYaxis().SetTitle("Event number");
    memHist1.GetYaxis().SetLabelSize(0.04);
    memHist1.GetXaxis().SetLabelOffset(0.04);
    memHist1.GetYaxis().SetTitleSize(0.04);
    memHist1.GetYaxis().SetTitleColor(2);
    memHist1.GetZaxis().SetTitle("Number of holes");
    memHist1.GetZaxis().SetLabelSize(0.02);
    memHist1.GetZaxis().SetTitleSize(0.04);
    memHist1.GetZaxis().SetTitleColor(2);
    
    title = TPaveLabel(0.1,0.95,0.9,0.99, "Job Memory Usage Plots");
    title.SetFillColor(42);
    title.SetTextFont(42);
    title.Draw();
    
    text_box = TPaveText(0.1,0.51,0.9,0.54);
    text_box.AddText("Malloc freed ('marked') Heap Memory Profile");
    text_box.SetFillColor(42);
    text_box.SetTextAlign(12);
    text_box.SetTextFont(42);
    text_box.Draw();
    
    x1=0.2; y1=0.91; x2=0.8; y2=0.98;

#Drawing upper-left corner Pad1 of the report
    pad1.cd();
    pad1.SetGridx();pad1.SetGridy();
    infoTree.Draw("total_maps_memory:scan_number","", "goff")
    mapsGraph=TGraph(int(infoTree.GetSelectedRows()), infoTree.GetV2(), infoTree.GetV1());    
    mapsGraph.SetLineColor(1); mapsGraph.SetLineWidth(1); #mapsGraph.SetFillStyle(3001); mapsGraph.SetFillColor(2);
    mapsGraph.SetName("total_maps_memory");
    #VmSize, VmLck, VmRSS, VmData, VmStk, VmExe, VmLib;
    infoTree.Draw("VmSize:scan_number", "","goff"); 
    vmsizeGraph=TGraph(int(infoTree.GetSelectedRows()), infoTree.GetV2(), infoTree.GetV1());    
    vmsizeGraph.SetLineColor(2); vmsizeGraph.SetLineWidth(1); #vmsizeGraph.SetFillStyle(3002); vmsizeGraph.SetFillColor(3);
    vmsizeGraph.SetName("VmSize");
    
    infoTree.Draw("VmRSS:scan_number", "", "goff"); 
    vmrssGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1());
    vmrssGraph.SetLineColor(3); vmrssGraph.SetLineWidth(1); #vmrssGraph.SetFillStyle(3003); vmrssGraph.SetFillColor(4);
    vmrssGraph.SetName("VmRSS"); 
    
    infoTree.Draw("VmData:scan_number", "", "goff"); 
    vmdataGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1());
    vmdataGraph.SetLineColor(4); vmdataGraph.SetLineWidth(1); #vmdataGraph.SetFillStyle(3004); vmdataGraph.SetFillColor(4);
    vmdataGraph.SetName("VmData"); 
    
    infoTree.Draw("VmStk:scan_number", "", "goff"); 
    vmstkGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1());
    vmstkGraph.SetLineColor(5); vmstkGraph.SetLineWidth(1); #vmstkGraph.SetFillStyle(3005); vmstkGraph.SetFillColor(4);
    vmstkGraph.SetName("VmStk")
    
    infoTree.Draw("VmExe:scan_number", "", "goff"); 
    vmexeGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1());
    vmexeGraph.SetLineColor(6); vmexeGraph.SetLineWidth(1); #vmexeGraph.SetFillStyle(3003); vmexeGraph.SetFillColor(4);
    vmexeGraph.SetName("VmExe");
    
    infoTree.Draw("VmLib:scan_number", "", "goff"); 
    vmlibGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1());
    vmlibGraph.SetLineColor(7); vmlibGraph.SetLineWidth(1); #vmlibGraph.SetFillStyle(3003); vmlibGraph.SetFillColor(4);
    vmlibGraph.SetName("VmLib");
  
    infoTree.Draw("VmLck:scan_number", "", "goff"); 
    vmlckGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1());
    vmlckGraph.SetLineColor(8); vmlckGraph.SetLineWidth(1); #vmlckGraph.SetFillStyle(3003); vmlckGraph.SetFillColor(4);
    vmlckGraph.SetName("VmLck");

    infoTree.Draw("total_hole_memory:scan_number", "", "goff");
    holeGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1())    
    holeGraph.SetLineColor(9); holeGraph.SetLineWidth(1); #holeGraph.SetFillStyle(3004); holeGraph.SetFillColor(5);
    holeGraph.SetName("HolesSize"); 
        
    mallocTree.Draw("malloc_free:scan_number", "", "goff"); 
    freeGraph=TGraph(mallocTree.GetSelectedRows(), mallocTree.GetV2(), mallocTree.GetV1())    
    freeGraph.SetLineColor(11); freeGraph.SetLineWidth(1); freeGraph.SetFillStyle(3003); freeGraph.SetFillColor(4);
    freeGraph.SetName("malloc_free");
    
    mallocTree.Draw("malloc_inuse:scan_number", "", "goff"); 
    inuseGraph=TGraph(mallocTree.GetSelectedRows(), mallocTree.GetV2(), mallocTree.GetV1())    
    inuseGraph.SetLineColor(21); inuseGraph.SetLineWidth(1); inuseGraph.SetFillStyle(3003); inuseGraph.SetFillColor(4);
    inuseGraph.SetName("malloc_inuse");

    mallocTree.Draw("malloc_sbrk:scan_number", "", "goff"); 
    sbrkGraph=TGraph(mallocTree.GetSelectedRows(), mallocTree.GetV2(), mallocTree.GetV1())    
    sbrkGraph.SetLineColor(31); sbrkGraph.SetLineWidth(1); sbrkGraph.SetFillStyle(3003); sbrkGraph.SetFillColor(4);
    sbrkGraph.SetName("malloc_sbrk");
    
    mallocTree.Draw("malloc_mmap:scan_number", "", "goff"); 
    mmapGraph=TGraph(mallocTree.GetSelectedRows(), mallocTree.GetV2(), mallocTree.GetV1())    
    mmapGraph.SetLineColor(41); mmapGraph.SetLineWidth(1); mmapGraph.SetFillStyle(3003); mmapGraph.SetFillColor(4);
    mmapGraph.SetName("malloc_mmap");
    
    pad1.cd();
    multiGraph1.Add(mapsGraph);multiGraph1.Add(vmsizeGraph);
    multiGraph1.Add(vmrssGraph);multiGraph1.Add(vmdataGraph);
    multiGraph1.Add(vmlckGraph);multiGraph1.Add(vmexeGraph);
    multiGraph1.Add(vmstkGraph);#multiGraph1.Add(vmlibGraph);
    multiGraph1.Add(inuseGraph);
    multiGraph1.Add(sbrkGraph);multiGraph1.Add(mmapGraph);
    multiGraph1.Add(holeGraph);multiGraph1.Add(freeGraph);


    
    #multiGraph1.SetTitle("PROCESS VM and Malloc MEMORY USAGE"); 
    title.DrawPaveLabel(x1,y1,x2,y2,"PROCESS VM and Malloc MEMORY USAGE","brNDC");

    multiGraph1.Draw("ALg")
    hist = multiGraph1.GetHistogram(); hist.SetXTitle("Event Number"); hist.SetYTitle("Memory, kb");
    legend1 = TLegend(0.84,0.20,0.99,0.90);
    legend1.AddEntry(mapsGraph,  "Maps",            "l");
    legend1.AddEntry(vmsizeGraph,"VmSize",          "l");
    legend1.AddEntry(vmrssGraph, "VmRSS",           "l");
    legend1.AddEntry(vmdataGraph,"VmData",          "l");

    legend1.AddEntry(sbrkGraph,  "MallocSbrk",      "l");
    legend1.AddEntry(inuseGraph, "MallocInuse",     "l");    
    #legend1.AddEntry(vmlibGraph, "VmLib",          "l");
    legend1.AddEntry(mmapGraph,  "MallocMmap",      "l");    
    legend1.AddEntry(freeGraph,  "MallocFree",      "l");
    legend1.AddEntry(holeGraph,  "Freed-Holes",     "l");     
    legend1.AddEntry(vmstkGraph, "VmStk",           "l");
    legend1.AddEntry(vmexeGraph, "VmExe",           "l");
    legend1.AddEntry(vmlckGraph, "VmLck",           "l");
    legend1.Draw();

    #multiGraph1.Draw("ALg")    
    #title.DrawPaveLabel(x1,y1,x2,y2,"Process Memory Usage Charts","brNDC");
    
    
#Drawing upper-left corner Pad1 of the report
    pad2.cd();
    pad2.SetGridx(); pad2.SetGridy();

    infoTree.Draw("total_hole_memory:scan_number", "", "goff");
    holeGraph=TGraph(infoTree.GetSelectedRows(), infoTree.GetV2(), infoTree.GetV1())    
    holeGraph.SetLineColor(9); holeGraph.SetLineWidth(1); holeGraph.SetFillStyle(3004); holeGraph.SetFillColor(5);
    holeGraph.SetName("total_hole_memory"); 

    mallocTree.Draw("malloc_free:scan_number", "", "goff");
    freeGraph=TGraph(mallocTree.GetSelectedRows(), mallocTree.GetV2(), mallocTree.GetV1())    
    freeGraph.SetLineColor(11); freeGraph.SetLineWidth(1); freeGraph.SetFillStyle(3004); freeGraph.SetFillColor(5);
    freeGraph.SetName("malloc_free");
    
    pad2.cd();
    multiGraph2.Add(holeGraph);
    multiGraph2.Add(freeGraph);
    #multiGraph2.Add(sbrkGraph);
    #multiGraph2.Add(mmapGraph);
    
    #multiGraph2.SetTitle("Free and Marked Holes Memory Graph"); 
    title.DrawPaveLabel(x1,y1,x2,y2,"Malloc Free and Marked Holes Memory","brNDC");
    
    multiGraph2.Draw("ALg")    
    hist = multiGraph2.GetHistogram(); hist.SetXTitle("Event Number"); hist.SetYTitle("Memory, kb");
    
    legend2 = TLegend(0.9,0.30,0.99,0.90);
    legend2.AddEntry(freeGraph,  "Free",   "l");
    legend2.AddEntry(holeGraph,  "Holes",  "l");
    #legend2.AddEntry(inuseGraph, "Inuse",  "l");
    #legend2.AddEntry(mmapGraph,  "Mmap",  "l");
    #legend2.AddEntry(sbrkGraph,  "Sbrk",  "l");

    legend2.Draw()
    #multiGraph2.Draw("ALg")    
    #title.DrawPaveLabel(x1,y1,x2,y2,"Malloc Memory Usage and Deallocation Charts","brNDC");  
  
  #PAD3
    pad3.cd()
    pad3.SetLogz()
    memHist1.Draw("lego2");
    #title.DrawPaveLabel(x1,y1,x2,y2,"TH2I-LEGO2","brNDC");

    mem_canvas.SetBorderSize(1);
    #mem_canvas.Modified()
    mem_canvas.Update()
    mem_canvas.Print(".pdf")
    mem_canvas.Print(".C")