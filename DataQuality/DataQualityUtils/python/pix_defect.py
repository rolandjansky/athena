# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
#from DQDefects import DefectsDB
from ROOT import gROOT, TCanvas, TDirectory, TFile, TH1F, TH2F, TF1, TProfile
import collections
defect_val = collections.namedtuple('defect_val',
                                    'defect, comment, recoverable')
defect_iov = collections.namedtuple('defect_iov',
                                    'defect, comment, recoverable, since, until')


def find_standby(nlayer, hhits, lb_max, standby_lb):
     threshold_hit = 10
     for i in range(0, nlayer):
          start = 0
          start_lb = 0
          end_lb = 0
          nbin = hhits[i].GetNbinsX()

          start_lb = 0
          for j in range(1, nbin-1):
               val = hhits[i].GetBinContent(j+1)
               if val >= threshold_hit:
                    end_lb = j
                    if start_lb == 0:
                         start_lb = j

          for j in range(1, nbin-1):
               #val_global = hglobal.GetBinContent(j+1)
               val = hhits[i].GetBinContent(j+1)
               if val < threshold_hit:
                    if start == 0:
                         start = 1
                         start_lb = j
               elif start == 1:
                    if j > start_lb and  j < end_lb:
                         standby_lb.append([i, start_lb, j-1])
                    start = 0
                    start_lb = 0
          if start == 1:
               if lb_max > start_lb:
                    standby_lb.append([i, start_lb, lb_max])


def find_notready(nlayer, hist, pct_low, pct_high, notready_lb):
     for i in range(0, nlayer):
          start = 0
          start_lb = 0
          end_lb = 0
          nbin = hist[i].GetNbinsX()
          if i == 2:
               for j in range(1, nbin-1):
                    val = hist[2].GetBinContent(j+1)*38/90 + hist[3].GetBinContent(j+1)*52/90
                    if val >= pct_low and val < pct_high:
                         if start == 0:
                              start = 1
                              start_lb = j
                    elif start == 1:
                         notready_lb.append([i, start_lb, j-1])
                         start = 0
                         start_lb = 0
               if start == 1:
                    notready_lb.append([i, start_lb, 2000])
          elif i == 3:
               #nothing to do
               start = 0
               start_lb = 0
               end_lb = 0
          else:
               for j in range(1, nbin-1):
                    val = hist[i].GetBinContent(j+1)
                    if val >= pct_low and val < pct_high:
                         if start == 0:
                              start = 1
                              start_lb = j
                    elif start == 1:
                         notready_lb.append([i, start_lb, j-1])
                         start = 0
                         start_lb = 0
               if start == 1:
                    notready_lb.append([i, start_lb, 2000])


def print_def(defect_name, defect_lb):
     for i in range(0, len(defect_lb)):
          print defect_name, "[", defect_lb[i][0], "]:", defect_lb[i][1], "-", defect_lb[i][2]

def assign_defect(db, defect_name, run, defect_lb):
     for i in range(0, len(defect_lb)):
          layer = defect_lb[i][0]
          if layer == 0:
               slayer = "IBL"
          elif layer == 1:
               slayer = "LAYER0"
          elif layer == 2:
               slayer = "BARREL"
          elif layer == 3:
               slayer = "BARREL"
          elif layer == 4:
               slayer = "ENDCAPA"
          elif layer == 5:
               slayer = "ENDCAPC"
               
          sdefect = "PIXEL_" + slayer + "_" + defect_name
          
          lbstart = defect_lb[i][1]
          lbend = defect_lb[i][2]

          start = (run << 32) + lbstart 
          until = (run << 32) + lbend + 1
               
          #print sdefect, ": LB = ", lbstart,"-",lbend,")"
          comment = "assign " + sdefect
          db.append(defect_iov(sdefect, comment, False, start, until))

def assign_lowstat(db, run, comment):
     sdefect = "PIXEL_LOWSTAT"
     lbstart = 1
     lbend = 2000
     present = True
     recoverable = True
     start = (run << 32) + lbstart 
     until = (run << 32) + lbend + 1
     #print sdefect, ": LB = ", lbstart,"-",lbend,")"
     db.append(defect_iov(sdefect, comment, False, start, until))

#def execute(run, sfile, lb_max,  user, db):
def execute(run, sfile, lb_max):
     db = []
     gROOT.Reset();
     file = TFile(sfile)

     shmu = "Global/Luminosity/AnyTrigger/actualMu_vs_LB"
     hmu = file.Get(shmu)

     shits = []
     shits.append("InnerDetector/Pixel/PIXIBL/Hits/AvgOcc_active_per_lumi_IBL")
     shits.append("InnerDetector/Pixel/PIX0/Hits/AvgOcc_active_per_lumi_B0")
     shits.append("InnerDetector/Pixel/PIX1/Hits/AvgOcc_active_per_lumi_B1")
     shits.append("InnerDetector/Pixel/PIX2/Hits/AvgOcc_active_per_lumi_B2")
     shits.append("InnerDetector/Pixel/PIXECA/Hits/AvgOcc_active_per_lumi_ECA")
     shits.append("InnerDetector/Pixel/PIXECC/Hits/AvgOcc_active_per_lumi_ECC")

     sdisabled = []
     sdisabled.append("InnerDetector/Pixel/PIXIBL/_Experts/DisableAndErrorsLB/DisabledAndSyncErrorsModules_per_lumi_IBL2D_byPostProcess")
     sdisabled.append("InnerDetector/Pixel/PIX0/DisableAndErrorsLB/DisabledAndSyncErrorsModules_per_lumi_B0_byPostProcess")
     sdisabled.append("InnerDetector/Pixel/PIX1/DisableAndErrorsLB/DisabledAndSyncErrorsModules_per_lumi_B1_byPostProcess")
     sdisabled.append("InnerDetector/Pixel/PIX2/DisableAndErrorsLB/DisabledAndSyncErrorsModules_per_lumi_B2_byPostProcess")
     sdisabled.append("InnerDetector/Pixel/PIXECA/DisableAndErrorsLB/DisabledAndSyncErrorsModules_per_lumi_ECA_byPostProcess")
     sdisabled.append("InnerDetector/Pixel/PIXECC/DisableAndErrorsLB/DisabledAndSyncErrorsModules_per_lumi_ECC_byPostProcess")

     nlayer = 6
     hhits = []
     hdisabled = []
     for i in range(0, nlayer):
          hhits.append(file.Get(shits[i]))
          hdisabled.append(file.Get(sdisabled[i]))

     standby_lb = []
     find_standby(nlayer, hhits, lb_max, standby_lb)

     notready5to7pct_lb = []
     notready7to10pct_lb = []
     notready10to20pct_lb = []
     notready20to30pct_lb = []
     notready_gt30pct_lb = []
     find_notready(nlayer, hdisabled, 0.05, 0.07, notready5to7pct_lb)
     find_notready(nlayer, hdisabled, 0.07, 0.1, notready7to10pct_lb)
     find_notready(nlayer, hdisabled, 0.1, 0.2, notready10to20pct_lb)
     find_notready(nlayer, hdisabled, 0.2, 0.3, notready20to30pct_lb)
     find_notready(nlayer, hdisabled, 0.3, 1., notready_gt30pct_lb)

     #print_def("standby", standby_lb)
     #user = "atlpixdq"
     #assign_defect(db, "STANDBY", run, standby_lb)

     if 0:
          print_def("5to7pct", notready5to7pct_lb)
          print_def("7to10pct", notready7to10pct_lb)
          print_def("10to20pct", notready10to20pct_lb)
          print_def("20to30pct", notready20to30pct_lb)
          print_def("gt30pct", notready_gt30pct_lb)


     if 1:
          assign_defect(db, "5to7pct_NOTREADY", run, notready5to7pct_lb)
          assign_defect(db, "7to10pct_NOTREADY", run, notready7to10pct_lb)
          assign_defect(db, "10to20pct_NOTREADY", run, notready10to20pct_lb)
          assign_defect(db, "20to30pct_NOTREADY", run, notready20to30pct_lb)
          assign_defect(db, "GT30pct_NOTREADY", run, notready_gt30pct_lb)

     #nevent = hmu.GetEntries()
     #if nevent < 100000:
     #assign_lowstat(db, run, "assign lowstat")

     return db
