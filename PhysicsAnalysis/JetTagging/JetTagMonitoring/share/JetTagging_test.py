#!/usr/bin/env python
# -*- coding: utf-8 -*-

#simple script to compare the actual data file with a reference and give a result depending on a 
#K test 
# use with > ./JetTagging_test.py [data file] [reference file] > ktest_[run].txt
# so you'll get an pdf file with an graphical representation and the actual test result in
# the text file.


import ROOT, sys, math, time, re
#from ROOT import gROOT, TCanvas, TH1, TH2, TH1F

h2list = []



if __name__ == "__main__":

    ROOT.gStyle.SetPalette(1)

    #Reference file (maybe outdated
    MonFile = "root://castoratlas//castor/cern.ch/grid/atlas/caf/atlcal/perm/dataqual/han_references/Collisions/data11_7TeV.00183347.express_express.merge.HIST.f384_m875._0001.1.root"

    print sys.argv

    tf = ROOT.TFile.Open(sys.argv[1])

    if len(sys.argv) > 2:
        tfref = ROOT.TFile.Open(sys.argv[2])
    else:
        tfref = ROOT.TFile.Open(MonFile)

    #get run number from root directory
    rDir =  tf.GetListOfKeys()[0].GetName()
    runTemp = re.search("\d{5,8}", rDir )
    if len(runTemp.group(0))==6:
        run = runTemp.group(0)
    elif len(runTemp.group(0))==8:
        run = runTemp.group(0)[2:8]
    else:
        print ("No run number in the filename %s in format XXXXXX or 00XXXXXX given" % tf.GetName() )
    
    print "run number from data file:\t %s " % run
     
    #get run number of reference from root directory
    rDir = tfref.GetListOfKeys()[0].GetName()
    runTempRef = re.search("\d{5,8}", rDir )
    if len(runTempRef.group(0))==6:
        runref = runTempRef.group(0)
    elif len(runTempRef.group(0))==8:
        runref = runTempRef.group(0)[2:8]
    else:
        print ("No run number in reference filename %s in format XXXXXX or 00XXXXXX given" % tfref.GetName() )
        
    print "run number from reference file:\t %s " % runref
    
  

   
    
    
   #Define "to look at" histograms
    histDict = {    "jet_electrons_n" : { "LogY" : True, "PosInCanvas" : 1},
                    "jet_muons_n" : { "LogY" : False, "PosInCanvas" : 2},
                    "jet_tracks_hits_BLayer" : { "LogY" : False, "PosInCanvas" : 3},
                    "jet_tracks_hits_Pixel" : { "LogY" : False, "PosInCanvas" : 4},
                    "jet_tracks_hits_SCT" : { "LogY" : False, "PosInCanvas" : 5},
                    "jet_tracks_n" : { "LogY" : True, "PosInCanvas" : 0},
                    "tag_JetProb_w" : { "LogY" : True, "PosInCanvas" : 0},
                    "tag_SV0_w" : { "LogY" : True, "PosInCanvas" : 6},
                    "tag_COMB_w" : { "LogY" : False, "PosInCanvas" : 7},
                    "tag_IP1D_w" : { "LogY" : False, "PosInCanvas" : 0},
                    "tag_IP2D_w" : { "LogY" : False, "PosInCanvas" : 0},
                    "tag_IP3D_w" : { "LogY" : False, "PosInCanvas" : 8},
                    "tag_TrkCnt_w" : { "LogY" : False, "PosInCanvas" : 0},
                    "tag_SOFTE_w" : { "LogY" : False, "PosInCanvas" : 0},
                    "tag_SOFTM_w" : { "LogY" : False, "PosInCanvas" : 0},
                    "track_selector_eff" : { "LogY" : False, "PosInCanvas" : 9}
                    }
    
    #list for the test results
    KolmogorovResult = [ 0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0, 0.0, 0.0,
          0.0, 0.0, 0.0]
    
    #histogram with all tracks (to normalise)
    ref2d = tf.Get("run_%s/JetTagging/track_selector_all" % run )
            
    #open canvas
    c2 = ROOT.TCanvas("c2", "Histograms for shifters", 0, 0, 1200, 1000)
    c2.Divide(3, 3)
    
    #iterate through the histograms
    for key,value in histDict.iteritems():
      if value["PosInCanvas"] == 0:
          continue
      print "opening histogram %s" % key
      c2.cd(value["PosInCanvas"])
      if key == "track_selector_eff":
        #special theatment of 2D histos
        
        h4       = tf.Get("run_%s/JetTagging/%s" % (run, key))
        if h4 is None :
          print "hist ", key, " not found"
        h4.Divide(ref2d)
        c2.cd(value["PosInCanvas"])
        h4.Draw("colz")
        
      else:
        #all others
        
        
        #get and check entries
        h2  = tf.Get("run_%s/JetTagging/%s" % (run, key))
        #h3 = ROOT.TH1F(tfref.Get("run_%s/JetTagging/%s" % (runref, key)))
        if h2 is None :
          print "hist ", key, " not found"
        
        h3 = tfref.Get("run_%s/JetTagging/%s" % (runref, key))
        if h3 is None :
          print "hist ", key, " not found"
      
        #c2.cd(value["PosInCanvas"])
        
        
        
        # check if histos have entries and normalise them
        if h2.Integral() > 0 and h3.Integral() > 0  :
          if value["LogY"]==True:
            ROOT.gPad.SetLogy(1)
          else:
            ROOT.gPad.SetLogy(0)
          h2.Scale(1. / h2.Integral())
          h3.Scale(1. / h3.Integral())
          
          # compare them
          KolmogorovResult[value["PosInCanvas"]] = h3.KolmogorovTest(h2,"M")  
          print "KolmogorovResult for %s: %g" % (h2.GetName(), KolmogorovResult[value["PosInCanvas"]])
          
          #line styling
          h3.SetLineColor(2)
          h2.SetLineColor(4)
          ROOT.gPad.SetFrameFillColor(10)
          
          # depending on test result the frames got red (all ok), yellow, red (bad result) colour
          if(KolmogorovResult[value["PosInCanvas"]] < 0.15):
             c2.cd(value["PosInCanvas"]).SetFillColor(3)
          elif (KolmogorovResult[value["PosInCanvas"]] < 0.3):
             c2.cd(value["PosInCanvas"]).SetFillColor(5)
          else:
             c2.cd(value["PosInCanvas"]).SetFillColor(2)
         
         #draw the histos
          h2.Draw("")
          h3.Draw("same")
        else:
          print "Integral of % s was 0" % key
          h3.SetLineColor(2)
          h2.SetLineColor(4)
          h2.Draw("")
          h3.Draw("same")
        
        #print "Closed histogram %s" % key
    c2.SaveAs("JetTaggingDQ_%s_run-%s.pdf" % ( time.strftime("%Y%m%d", time.localtime()), run))
    
    




    

