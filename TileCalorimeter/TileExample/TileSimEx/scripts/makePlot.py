# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-----------------------------------------------------
#     Author: Dengfeng Zhang
#     dengfeng.zhang@cern.ch
#-----------------------------------------------------
# This script has for functions:
# GetPlotSingleProperty(): Get energy response, energy resolution, 
#                          lateral spread and longitudinal profile  of MC.
# GetDataPlotSingleProperty(): Get energy response, energy resolution, 
#                              lateral spread and longitudinal profile of data.
# ComDataMC(): Compare MC results with data results, get the ratio of MC to data. 
#              Draw all MC, data and their ratios on one plot.
#              (energy response and resolution, lateral spread and longitudinal spread) ;
# ComparePhysicsList(): Draw Draw all MC results, data results on one plot, not ratios.
#                       (energy response and resolution, lateral spread)
#-----------------------------------------------------

import ROOT,math,os,array
from ROOT import *
gROOT.ProcessLine("#include \"GraphToolKit.h\"")
gROOT.ProcessLine("#include \"HistToolKit.h\"")

Energies = [20000, 50000, 100000, 180000] # beam energy lists
Particles = ['pi', 'pr'] # particle types 
PhysicsLists = ['FTFP_BERT', 'FTFP_BERT_ATL', 'QGSP_BERT', "QGSP_BIC"] # physics lists

# Get the current working dir
Dir = os.getcwd()
# Check main output dir holding output root files does exist in current working dir,
#  otherwise create it.
ResultDir = Dir+"/results/"
if ROOT.gSystem.AccessPathName(ResultDir):
  print ResultDir, "doesn't exist! Making"
  gSystem.Exec("mkdir {}".format(ResultDir))
# Check main output dir holding plots does exist in current working dir,
#  otherwise create it.
PlotDir = Dir+"/plots/"
if ROOT.gSystem.AccessPathName(PlotDir):
  print PlotDir, "doesn't exist! Making"
  gSystem.Exec("mkdir {}".format(PlotDir))

# Get the energy response and resolution, lateral spread 
# and longitudinal of each type of particles, each beam energy and each physics list.
def GetPlotSingleProperty():
  for Particle in Particles: # loop over particle types
    # input path containing root files generated in GetEnergy.cxx
    InPath = ResultDir+"/{}/".format(Particle)
    # create output root file
    # grapherrors of renponse, resolution and lateral spread 
    # and histograms of longitudinal profile will be wrote in it.
    outputFile = ROOT.TFile.Open('{}/Properities_{}.root'.format(InPath,Particle),'RECREATE')
    for PhysicsList in PhysicsLists:  # loop over physics lists
      # define array or list of responses, resolutions, 
      # lateral spreads and longitudinal profiles  of all beam energies
      Response = array.array('f') # array of energy responses of all beam energies for each type of particles and for each physics lists
      ResponseError = array.array('f') #array of energy response errors (only statistical)
      Resolution = array.array('f') #array of energy resolutions of all beam energies
      ResolutionError = array.array('f') #array of energy resolution errors
      LateralSpread = array.array('f') #array of lateral spreads of all beam energies
      LateralSpreadError = array.array('f') #array of lateral spread errors of all beam energies
      Es = array.array('f') # array of beam energies 
      EsError = array.array('f') # # array of beam energy errors, always 0
      LongitudinalProfileList = [] # list of longitudinal profiles of all beam energies
      NormalizedLongitudinalProfileList = [] # list of normalized longitudinal profiles of all beam energies

      for Energy in Energies: # loop over all beam energies
        Es.append(Energy/1000.)
        EsError.append(0.)
        # get input file generated in GetEnergy.cxx 
        # attaced to each beam energy, particle and physics list
        inputFile = ROOT.TFile('{}/tiletb90-E{}-{}_{}.root'.format(InPath,Energy,Particle,PhysicsList),"read")
        if not inputFile:
          continue
        print "InFile: ",inputFile.GetName()

        # get histograms in input file
        h_E = inputFile.Get("RecoE") # total energy distribution
        h_EM0 = inputFile.Get("RecoEModule0") # distribution of energy in barrel module 0
        h_EB = inputFile.Get("RecoECentralModule") # distribution of energy in central barrel module
        h_LP = inputFile.Get("LongitudinalProfile") # get the longitudinal profile
        h_LP.SetDirectory(0)

        # define a gaus fun to fit total energy distribution
        func = ROOT.TF1("func","gaus",h_E.GetMean()-2*h_E.GetRMS(),h_E.GetMean()+2*h_E.GetRMS())
        print h_E.GetMean()-2*h_E.GetRMS()," ", h_E.GetMean()+2*h_E.GetRMS()
        h_E.Fit("func","R") # fit the total energy distribution by a Gaussian 
        gStyle.SetOptFit(1)
        canvas = ROOT.TCanvas("canvas","",800,600)
        h_E.Draw()
        canvas.Print(ResultDir+'/{}/totalE_{}_{}_{}.pdf'.format(Particle,Particle,Energy,PhysicsList))
        # energy response is the mean of the gaussian fitting/beam energy,
        # energy resolution is sigma/mean of the gaussian fitting 
        Response.append(func.GetParameter(1)*1000/Energy) 
        ResponseError.append(func.GetParError(1)*1000/Energy)
        Resolution.append(func.GetParameter(2)/func.GetParameter(1)*100)
        ResolutionError.append(func.GetParError(2)/func.GetParameter(1)*100)

        # Get lateral spread(mean energy in module 0/ mean energy in central barrel)
        LS = h_EM0.GetMean()/h_EB.GetMean() 
        LSError = LS*math.sqrt(pow(h_EM0.GetMeanError()/h_EM0.GetMean(), 2)+pow(h_EB.GetMeanError()/h_EB.GetMean(), 2))
        LateralSpread.append(LS)
        LateralSpreadError.append(LSError)

        # get the longitudinal profiles scaling by the energy response
        h_LP.Scale(1./(func.GetParameter(1)*1000/Energy)) #FIXME
        #h_LP.Scale(Energy/1000/h_LP.Integral("width")) #FIXME
        # get the normalized longitudinal profiles normalize it to 1
        h_NormalizedLP=h_LP.Clone()
        h_NormalizedLP.SetDirectory(0)
        h_NormalizedLP.Scale(1./h_LP.Integral("width"))
        h_LP.SetName("{}_{}GeV_{}_LongitudinalProfile".format(Particle,Energy/1000, PhysicsList))
        h_LP.SetTitle("{} GeV".format(Energy/1000))
        h_NormalizedLP.SetName("{}_{}GeV_{}_NormalizedLongitudinalProfile".format(Particle, Energy/1000, PhysicsList))
        h_NormalizedLP.SetTitle("{} GeV".format(Energy/1000))
        h_NormalizedLP.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
        LongitudinalProfileList.append(h_LP)
        NormalizedLongitudinalProfileList.append(h_NormalizedLP)
      print LongitudinalProfileList, NormalizedLongitudinalProfileList
      outputFile.cd()
      # create the grapherrors of energy responses
      gr_response = ROOT.TGraphErrors(len(Es),Es,Response,EsError,ResponseError)
      gr_response.SetName("{}_{}_Response".format(Particle,PhysicsList))
      gr_response.SetTitle("{} {} Response".format(Particle,PhysicsList))
      gr_response.GetXaxis().SetTitle("E_{beam}[GeV]")
      gr_response.GetYaxis().SetTitle("E_{total}/E_{beam}")
      # create the grapherrors of energy resolutions
      gr_resolution = ROOT.TGraphErrors(len(Es),Es,Resolution,EsError,ResolutionError)
      gr_resolution.SetName("{}_{}_Resolution".format(Particle,PhysicsList))
      gr_resolution.SetTitle("{} {} Resolution".format(Particle,PhysicsList))
      gr_resolution.GetYaxis().SetTitle("resolution[%]")
      gr_resolution.GetXaxis().SetTitle("E_{beam}[GeV]")
      # create the grapherrors of lateral spread
      gr_lateralspread = ROOT.TGraphErrors(len(Es),Es,LateralSpread,EsError,LateralSpreadError)
      gr_lateralspread.SetName("{}_{}_LateralSpread".format(Particle,PhysicsList))
      gr_lateralspread.SetTitle("{} {} LateralSpread".format(Particle,PhysicsList))
      gr_lateralspread.GetYaxis().SetTitle("E_{Module0}/E_{Barrel}")
      gr_lateralspread.GetXaxis().SetTitle("E_{beam}[GeV]")
      # set the x range of grapherrors of response and resolution
      gr_response.GetXaxis().SetRangeUser(10, 210)
      gr_response.GetYaxis().SetNdivisions(510)
      gr_resolution.GetXaxis().SetRangeUser(10, 210)
      gr_resolution.GetYaxis().SetNdivisions(510)
      gr_lateralspread.GetXaxis().SetRangeUser(10, 210)
      gr_lateralspread.GetYaxis().SetNdivisions(510)

      # set the x range of grapherrors of lateral spread
      if(Particle=="pi"):
        gr_lateralspread.GetYaxis().SetRangeUser(0.025, 0.055)
        gr_lateralspread.GetYaxis().SetNdivisions(503) ;
      elif(Particle=="pr"):
        gr_lateralspread.GetYaxis().SetRangeUser(0.025, 0.065)
        gr_lateralspread.GetYaxis().SetNdivisions(504) ;

      # define output for each particle type,
      # if this dir doesn't exist, create it.
      OutPath = PlotDir+"/{}/".format(Particle)
      if ROOT.gSystem.AccessPathName(OutPath):
        print OutPath, "doesn't exist! Making"
        ROOT.gSystem.Exec("mkdir {}".format(OutPath))

      FullParticleName=""
      if Particle=='pi':
        FullParticleName = "Pion"
      elif Particle=='pr':
        FullParticleName = "Proton"

      # loop over beam energies to draw single longitudinal profile
      for i, Energy in enumerate(Energies):
        LongitudinalProfileList[i].Write()
        NormalizedLongitudinalProfileList[i].Write()
        # draw the single plot of normalized longitudinal profile and longitudinal profile        # of each type of particle and each physics list and each beam energy
        DrawSingleHistOnCanvas(OutPath+LongitudinalProfileList[i].GetName(), LongitudinalProfileList[i], "PE", False, True, False, "#splitline{"+"{}GeV {}".format(Energy/1000, FullParticleName)+"}{"+"{}".format(PhysicsList)+"}")
        DrawSingleHistOnCanvas(OutPath+NormalizedLongitudinalProfileList[i].GetName(), NormalizedLongitudinalProfileList[i], "PE",False, True, False, "#splitline{"+"{}GeV {}".format(Energy/1000, FullParticleName)+"}{"+"{}".format(PhysicsList)+"}")

      LongitudinalProfileList[0].GetYaxis().SetRangeUser(1E-3, 100.)
      NormalizedLongitudinalProfileList[0].GetYaxis().SetRangeUser(1E-5, 1.)
      # Draw four longitudinal profiles of 4 beam energies of each type of particle and
      # each physics list on same canvas
      DrawFourHistsOnCanvas("{}/{}_{}_LongitudinalProfile_LogY".format(OutPath,Particle,PhysicsList), LongitudinalProfileList[0], LongitudinalProfileList[1], LongitudinalProfileList[2], LongitudinalProfileList[3],"pe", "pesame", "pesame", "pesame", False, True, False, FullParticleName, PhysicsList)
      DrawFourHistsOnCanvas("{}/{}_{}_NormalizedLongitudinalProfile_LogY".format(OutPath,Particle,PhysicsList), NormalizedLongitudinalProfileList[0], NormalizedLongitudinalProfileList[1], NormalizedLongitudinalProfileList[2], NormalizedLongitudinalProfileList[3],"pe", "pesame", "pesame", "pesame", False, True, False, FullParticleName, PhysicsList)
      # don't use logy on y axis
      LongitudinalProfileList[0].GetYaxis().SetRangeUser(0., 40.)
      NormalizedLongitudinalProfileList[0].GetYaxis().SetRangeUser(0., 0.25)
      DrawFourHistsOnCanvas("{}/{}_{}_LongitudinalProfile".format(OutPath,Particle,PhysicsList), LongitudinalProfileList[0], LongitudinalProfileList[1], LongitudinalProfileList[2], LongitudinalProfileList[3],"pe", "pesame", "pesame", "pesame", False, False, False, FullParticleName, PhysicsList)
      DrawFourHistsOnCanvas("{}/{}_{}_NormalizedLongitudinalProfile".format(OutPath,Particle,PhysicsList), NormalizedLongitudinalProfileList[0], NormalizedLongitudinalProfileList[1], NormalizedLongitudinalProfileList[2], NormalizedLongitudinalProfileList[3],"pe", "pesame", "pesame", "pesame", False, False, False, FullParticleName, PhysicsList)

      # draw single grapherrors of responses , resolutions and lateral spread
      DrawSingleGraphErrorsOnCanvas("{}/{}_{}_Response".format(OutPath,Particle,PhysicsList),gr_response,"AP", False, False, False, FullParticleName+" "+PhysicsList)
      DrawSingleGraphErrorsOnCanvas("{}/{}_{}_Resolution".format(OutPath,Particle,PhysicsList),gr_resolution,"AP", False, False, False, FullParticleName+" "+PhysicsList)
      DrawSingleGraphErrorsOnCanvas("{}/{}_{}_LateralSpread".format(OutPath,Particle,PhysicsList), gr_lateralspread,"AP", False, False, False, FullParticleName+" "+PhysicsList)

      gr_response.Write()
      gr_resolution.Write()
      gr_lateralspread.Write()
      print Response
      print Resolution
      print LateralSpread
    outputFile.Write()

# get the energy responses and resolutions, lateral spreads and longitudianl of data
# data results are extracted from http://indico.cern.ch/event/135703/contributions/134036/attachments/108421/154300/main.pdf
def GetDataPlotSingleProperty():
  # pion responses of data
  PiResponse = array.array('f', [0.808, 0.844, 0.856, 0.867])
  PiResponseError = array.array('f', [0., 0., 0., 0.])
  # proton responses of data, proton only has 3 beam energies
  PrResponse = array.array('f', [0.811, 0.83, 0.845])
  PrResponseError = array.array('f', [0., 0., 0.])
  # pion resolutions of data
  PiResolution = array.array('f', [11.94, 8.92, 6.78, 6.02])
  PiResolutionError = array.array('f', [0., 0., 0., 0.])
  # proton resolutions of data, proton only has 3 beam energies
  PrResolution = array.array('f', [8.63, 5.97, 5.16])
  PrResolutionError = array.array('f', [0., 0., 0.])
  # pion latreal spreads of data
  PiLateralSpread = array.array('f', [0.044, 0.0379, 0.0342, 0.034])
  PiLateralSpreadError = array.array('f', [0., 0., 0., 0.])
  PrLateralSpread = array.array('f', [0.045, 0.0403, 0.0396])
  PrLateralSpreadError = array.array('f', [0., 0., 0.])
  # pion has four beam energies
  PiEs = array.array('f', [20., 50., 100., 180.])
  PiEsError = array.array('f', [0., 0., 0., 0.])
  # Be careful that proton only has three beam energies
  PrEs = array.array('f', [50., 100., 180.])
  PrEsError = array.array('f', [0., 0., 0.])

  # pion longitudinal profiles of data
  PiLongitudinalProfile20GeV = array.array('f',[4.88076, 4.29345, 1.90255, 0.760799, 0.336904, 0.116429, 0.0472258, 0.0212191, 0.010869])
  PiLongitudinalProfileError20GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0])
  PiLongitudinalProfile50GeV = array.array('f',[10.1243, 10.3069, 5.44077, 2.55502, 1.18216, 0.486682, 0.197446, 0.0913368, 0.0474821, 0.0181673, 0.00878025])
  PiLongitudinalProfileError50GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
  PiLongitudinalProfile100GeV = array.array('f',[16.6323,21.0755,12.1435,6.13442,3.14342,1.37201,0.625483,0.31123,0.143954,0.0619092,0.022023,0.0199365])
  PiLongitudinalProfileError100GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
  PiLongitudinalProfile180GeV = array.array('f',[28.1277,37.7873,21.7727,11.4903,6.33449,2.88857,1.31695,0.655294,0.303115,0.140209,0.0739654,0.0318035,0.0145007])
  PiLongitudinalProfileError180GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0])
  # proton longitudinal profiles of data
  PrLongitudinalProfile50GeV = array.array('f',[10.2289,10.3627,5.51951,2.54066,1.16948,0.472035,0.174547,0.0747019,0.0310458,0.0099195,0.0043075])
  PrLongitudinalProfileError50GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
  PrLongitudinalProfile100GeV = array.array('f',[18.3511,21.2032,11.4597,5.51097,2.61195,1.039,0.431832,0.193063,0.0814251,0.0364116,0.00962173,0.00783076])
  PrLongitudinalProfileError100GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
  PrLongitudinalProfile180GeV = array.array('f',[30.1568,39.1626,21.7967,10.7928,5.42299,2.2868,0.978724,0.437566,0.198557,0.0813227,0.0256083,0.0114493,0.00382185])
  PrLongitudinalProfileError180GeV = array.array('f',[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])

  # define output dir of root files and plots,
  # if they doesn't exist, create them.
  OutPathResult = ResultDir+"/data/"
  OutPathPlot = PlotDir+"/data/"
  if ROOT.gSystem.AccessPathName(OutPathResult):
    print OutPathResult, "doesn't exist! Making"
    ROOT.gSystem.Exec("mkdir {}".format(OutPathResult))
  if ROOT.gSystem.AccessPathName(OutPathPlot):
    print OutPathPlot, "doesn't exist! Making"
    ROOT.gSystem.Exec("mkdir {}".format(OutPathPlot))
  # create out file
  outputFile = ROOT.TFile.Open('{}/data.root'.format(OutPathResult),'RECREATE')
  # create out grapherrors of response, resolution and lateral spreads
  gr_piresponse = ROOT.TGraphErrors(len(PiEs),PiEs,PiResponse,PiEsError,PiResponseError)
  gr_piresponse.SetName("pi_Response")
  gr_piresponse.SetTitle("Pion")
  gr_piresponse.GetXaxis().SetTitle("E_{beam}[GeV]")
  gr_piresponse.GetYaxis().SetTitle("E_{total}/E_{beam}")
  gr_prresponse = ROOT.TGraphErrors(len(PrEs),PrEs,PrResponse,PrEsError,PrResponseError)
  gr_prresponse.SetName("pr_Response")
  gr_prresponse.SetTitle("Proton")
  gr_prresponse.GetXaxis().SetTitle("E_{beam}[GeV]")
  gr_prresponse.GetYaxis().SetTitle("E_{total}/E_{beam}")
  gr_piresolution = ROOT.TGraphErrors(len(PiEs),PiEs,PiResolution,PiEsError,PiResolutionError)
  gr_piresolution.SetName("pi_Resolution")
  gr_piresolution.SetTitle("Pion")
  gr_piresolution.GetXaxis().SetTitle("E_{beam}[GeV]")
  gr_piresolution.GetYaxis().SetTitle("resolution[%]")
  gr_prresolution = ROOT.TGraphErrors(len(PrEs),PrEs,PrResolution,PrEsError,PrResolutionError)
  gr_prresolution.SetName("pr_Resolution")
  gr_prresolution.SetTitle("Proton")
  gr_prresolution.GetXaxis().SetTitle("E_{beam}[GeV]")
  gr_prresolution.GetYaxis().SetTitle("resolution[%]")
  gr_pilateralspread = ROOT.TGraphErrors(len(PiEs),PiEs,PiLateralSpread,PiEsError,PiLateralSpreadError)
  gr_pilateralspread.SetName("pi_LateralSpread")
  gr_pilateralspread.SetTitle("Pion")
  gr_pilateralspread.GetXaxis().SetTitle("E_{beam}[GeV]")
  gr_pilateralspread.GetYaxis().SetTitle("E_{Module0}/E_{Barrel}")
  gr_prlateralspread = ROOT.TGraphErrors(len(PrEs),PrEs,PrLateralSpread,PrEsError,PrLateralSpreadError)
  gr_prlateralspread.SetName("pr_LateralSpread")
  gr_prlateralspread.SetTitle("Proton")
  gr_prlateralspread.GetXaxis().SetTitle("E_{beam}[GeV]")
  gr_prlateralspread.GetYaxis().SetTitle("E_{Module0}/E_{Barrel}")

  NBCells=13 # only use 13 of 18 B cells
  # bin edges of longitudinal profiles histograms
  xBLow =  array.array('f',[0.119333,1.67226,3.44703,5.0887,6.686,8.15019,9.61438,10.9898,12.3582,13.7407,15.1233,16.4916,17.8671,19.3313])
  # create longitudinal profiles histograms for all particles and beam energies
  pi_LongitudinalProfile20GeV = bookTH1F("pi_LongitudinalProfile_20GeV", "20 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  pi_LongitudinalProfile50GeV = bookTH1F("pi_LongitudinalProfile_50GeV", "50 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  pi_LongitudinalProfile100GeV = bookTH1F("pi_LongitudinalProfile_100GeV", "100 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  pi_LongitudinalProfile180GeV = bookTH1F("pi_LongitudinalProfile_180GeV", "180 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  pr_LongitudinalProfile50GeV = bookTH1F("pr_LongitudinalProfile_50GeV", "50 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  pr_LongitudinalProfile100GeV = bookTH1F("pr_LongitudinalProfile_100GeV", "100 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  pr_LongitudinalProfile180GeV = bookTH1F("pr_LongitudinalProfile_180GeV", "180 GeV", "x[#lambda]", "dE/dx[GeV/#lambda]", NBCells, xBLow)
  # fill longitudinal profile histograms
  for i in range(len(PiLongitudinalProfile20GeV)):
    pi_LongitudinalProfile20GeV.SetBinContent(i+1,PiLongitudinalProfile20GeV[i])
    pi_LongitudinalProfile20GeV.SetBinError(i+1,PiLongitudinalProfileError20GeV[i])
  for i in range(len(PiLongitudinalProfile50GeV)):
    pi_LongitudinalProfile50GeV.SetBinContent(i+1,PiLongitudinalProfile50GeV[i])
    pi_LongitudinalProfile50GeV.SetBinError(i+1,PiLongitudinalProfileError50GeV[i])
  for i in range(len(PiLongitudinalProfile100GeV)):
    pi_LongitudinalProfile100GeV.SetBinContent(i+1,PiLongitudinalProfile100GeV[i])
    pi_LongitudinalProfile100GeV.SetBinError(i+1,PiLongitudinalProfileError100GeV[i])
  for i in range(len(PrLongitudinalProfile180GeV)):
    pi_LongitudinalProfile180GeV.SetBinContent(i+1,PiLongitudinalProfile180GeV[i])
    pi_LongitudinalProfile180GeV.SetBinError(i+1,PiLongitudinalProfileError180GeV[i])
  for i in range(len(PrLongitudinalProfile50GeV)):
    pr_LongitudinalProfile50GeV.SetBinContent(i+1,PrLongitudinalProfile50GeV[i])
    pr_LongitudinalProfile50GeV.SetBinError(i+1,PrLongitudinalProfileError50GeV[i])
  for i in range(len(PrLongitudinalProfile100GeV)):
    pr_LongitudinalProfile100GeV.SetBinContent(i+1,PrLongitudinalProfile100GeV[i])
    pr_LongitudinalProfile100GeV.SetBinError(i+1,PrLongitudinalProfileError100GeV[i])
  for i in range(len(PrLongitudinalProfile180GeV)):
    pr_LongitudinalProfile180GeV.SetBinContent(i+1,PrLongitudinalProfile180GeV[i])
    pr_LongitudinalProfile180GeV.SetBinError(i+1,PrLongitudinalProfileError180GeV[i])

  # get the normalized longitudinal profiles
  pi_NormalizedLongitudinalProfile20GeV=pi_LongitudinalProfile20GeV.Clone()
  pi_NormalizedLongitudinalProfile20GeV.Scale(1./pi_LongitudinalProfile20GeV.Integral("width"))
  pi_NormalizedLongitudinalProfile20GeV.SetName("pi_NormalizedLongitudinalProfile20GeV")
  pi_NormalizedLongitudinalProfile20GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
  pi_NormalizedLongitudinalProfile50GeV=pi_LongitudinalProfile50GeV.Clone()
  pi_NormalizedLongitudinalProfile50GeV.Scale(1./pi_LongitudinalProfile50GeV.Integral("width"))
  pi_NormalizedLongitudinalProfile50GeV.SetName("pi_NormalizedLongitudinalProfile50GeV")
  pi_NormalizedLongitudinalProfile50GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
  pi_NormalizedLongitudinalProfile100GeV=pi_LongitudinalProfile100GeV.Clone()
  pi_NormalizedLongitudinalProfile100GeV.Scale(1./pi_LongitudinalProfile100GeV.Integral("width"))
  pi_NormalizedLongitudinalProfile100GeV.SetName("pi_NormalizedLongitudinalProfile100GeV")
  pi_NormalizedLongitudinalProfile100GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
  pi_NormalizedLongitudinalProfile180GeV=pi_LongitudinalProfile180GeV.Clone()
  pi_NormalizedLongitudinalProfile180GeV.Scale(1./pi_LongitudinalProfile180GeV.Integral("width"))
  pi_NormalizedLongitudinalProfile180GeV.SetName("pi_NormalizedLongitudinalProfile180GeV")
  pi_NormalizedLongitudinalProfile180GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
  pr_NormalizedLongitudinalProfile50GeV=pr_LongitudinalProfile50GeV.Clone()
  pr_NormalizedLongitudinalProfile50GeV.Scale(1./pr_LongitudinalProfile50GeV.Integral("width"))
  pr_NormalizedLongitudinalProfile50GeV.SetName("pr_NormalizedLongitudinalProfile50GeV")
  pr_NormalizedLongitudinalProfile50GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
  pr_NormalizedLongitudinalProfile100GeV=pr_LongitudinalProfile100GeV.Clone()
  pr_NormalizedLongitudinalProfile100GeV.Scale(1./pr_LongitudinalProfile100GeV.Integral("width"))
  pr_NormalizedLongitudinalProfile100GeV.SetName("pr_NormalizedLongitudinalProfile100GeV")
  pr_NormalizedLongitudinalProfile100GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")
  pr_NormalizedLongitudinalProfile180GeV=pr_LongitudinalProfile180GeV.Clone()
  pr_NormalizedLongitudinalProfile180GeV.Scale(1./pr_LongitudinalProfile180GeV.Integral("width"))
  pr_NormalizedLongitudinalProfile180GeV.SetName("pr_NormalizedLongitudinalProfile180GeV")
  pr_NormalizedLongitudinalProfile180GeV.GetYaxis().SetTitle("1/E_{tot}#timesdE/dx[1/#lambda]")

  # draw plots of response resolution and longitudinal profile
  DrawSingleGraphErrorsOnCanvas("{}/pi_LateralSpread".format(OutPathPlot), gr_pilateralspread,"AP", False, False, False)
  DrawSingleGraphErrorsOnCanvas("{}/pi_Response".format(OutPathPlot), gr_piresponse,"AP",False, False, False)
  DrawSingleGraphErrorsOnCanvas("{}/pi_Resolution".format(OutPathPlot), gr_piresolution,"AP", False, False, False)
  DrawSingleGraphErrorsOnCanvas("{}/pr_LateralSpread".format(OutPathPlot), gr_pilateralspread,"AP", False, False, False)
  DrawSingleGraphErrorsOnCanvas("{}/pr_Resolution".format(OutPathPlot), gr_prresolution,"AP")

  # draw of response resolution and longitudinal profile of pion and proton on same canvas
  DrawTwoGraphErrorsOnCanvas("{}/pipr_Resolution".format(OutPathPlot), gr_piresolution, gr_prresolution,"AP", "AP", False, False, False)
  DrawTwoGraphErrorsOnCanvas("{}/pipr_Response".format(OutPathPlot), gr_piresponse, gr_prresponse,"AP", "AP", False, False, False)
  DrawTwoGraphErrorsOnCanvas("{}/pipr_LateralSpread".format(OutPathPlot), gr_pilateralspread, gr_prlateralspread,"AP", "AP", False, False, False)

  # draw single longitudinal profile of each beam energy
  DrawSingleHistOnCanvas("{}/pi_LongitudinalProfile20GeV".format(OutPathPlot),pi_LongitudinalProfile20GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_LongitudinalProfile50GeV".format(OutPathPlot),pi_LongitudinalProfile50GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_LongitudinalProfile100GeV".format(OutPathPlot),pi_LongitudinalProfile100GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_LongitudinalProfile180GeV".format(OutPathPlot),pi_LongitudinalProfile180GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_NormalizedLongitudinalProfile20GeV".format(OutPathPlot),pi_LongitudinalProfile20GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_NormalizedLongitudinalProfile50GeV".format(OutPathPlot),pi_LongitudinalProfile50GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_NormalizedLongitudinalProfile100GeV".format(OutPathPlot),pi_LongitudinalProfile100GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pi_NormalizedLongitudinalProfile180GeV".format(OutPathPlot),pi_LongitudinalProfile180GeV, "PE", False, True, False)

  # draw 4 longitudinal profiles of pions of 4 beam energies on same canvas
  pi_LongitudinalProfile20GeV.GetYaxis().SetRangeUser(1E-3, 100.)
  pi_NormalizedLongitudinalProfile20GeV.GetYaxis().SetRangeUser(1E-5, 1.)
  DrawFourHistsOnCanvas("{}/pi_LongitudinalProfile_LogY".format(OutPathPlot),pi_LongitudinalProfile20GeV,pi_LongitudinalProfile50GeV,pi_LongitudinalProfile100GeV,pi_LongitudinalProfile180GeV,"pe", "pesame", "pesame", "pesame", False, True, False, "Pion")
  DrawFourHistsOnCanvas("{}/pi_NormalizedLongitudinalProfile_LogY".format(OutPathPlot),pi_NormalizedLongitudinalProfile20GeV,pi_NormalizedLongitudinalProfile50GeV,pi_NormalizedLongitudinalProfile100GeV,pi_NormalizedLongitudinalProfile180GeV,"pe", "pesame", "pesame", "pesame", False, True, False, "Pion")
  pi_LongitudinalProfile20GeV.GetYaxis().SetRangeUser(0., 40.)
  pi_NormalizedLongitudinalProfile20GeV.GetYaxis().SetRangeUser(0., 0.25)
  DrawFourHistsOnCanvas("{}/pi_LongitudinalProfile".format(OutPathPlot),pi_LongitudinalProfile20GeV,pi_LongitudinalProfile50GeV,pi_LongitudinalProfile100GeV,pi_LongitudinalProfile180GeV,"pe", "pesame", "pesame", "pesame", False, False, False, "Pion")
  DrawFourHistsOnCanvas("{}/pi_NormalizedLongitudinalProfile".format(OutPathPlot),pi_NormalizedLongitudinalProfile20GeV,pi_NormalizedLongitudinalProfile50GeV,pi_NormalizedLongitudinalProfile100GeV,pi_NormalizedLongitudinalProfile180GeV,"pe", "pesame", "pesame", "pesame", False, False, False, "Pion")

  # draw 3 longitudinal profiles of pions of 3 beam energies  of pion on same canvas
  DrawSingleHistOnCanvas("{}/pr_LongitudinalProfile50GeV".format(OutPathPlot),pr_LongitudinalProfile50GeV,"PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pr_LongitudinalProfile100GeV".format(OutPathPlot),pr_LongitudinalProfile100GeV, "PE", False, True, False)
  DrawSingleHistOnCanvas("{}/pr_LongitudinalProfile180GeV".format(OutPathPlot),pr_LongitudinalProfile180GeV, "PE", False, True, False)

  # draw 3 longitudinal profiles of proton of 3 beam energies on same canvas
  pr_LongitudinalProfile50GeV.GetYaxis().SetRangeUser(1E-3, 100.)
  pr_NormalizedLongitudinalProfile50GeV.GetYaxis().SetRangeUser(1E-5, 1.)
  DrawThreeHistsOnCanvas("{}/pr_LongitudinalProfile_LogY".format(OutPathPlot),pr_LongitudinalProfile50GeV, pr_LongitudinalProfile100GeV, pr_LongitudinalProfile180GeV, "pe", "pesame", "pesame",  False, True, False, "Proton")
  DrawThreeHistsOnCanvas("{}/pr_NormalizedLongitudinalProfile_LogY".format(OutPathPlot),pr_NormalizedLongitudinalProfile50GeV, pr_NormalizedLongitudinalProfile100GeV, pr_NormalizedLongitudinalProfile180GeV, "pe", "pesame", "pesame",  False, True, False, "Proton")
  pr_LongitudinalProfile50GeV.GetYaxis().SetRangeUser(0., 40.)
  pr_NormalizedLongitudinalProfile50GeV.GetYaxis().SetRangeUser(0., 0.25)
  DrawThreeHistsOnCanvas("{}/pr_LongitudinalProfile".format(OutPathPlot),pr_LongitudinalProfile50GeV, pr_LongitudinalProfile100GeV, pr_LongitudinalProfile180GeV, "pe", "pesame", "pesame",  False, False, False, "Proton")
  DrawThreeHistsOnCanvas("{}/pr_NormalizedLongitudinalProfile".format(OutPathPlot),pr_NormalizedLongitudinalProfile50GeV, pr_NormalizedLongitudinalProfile100GeV, pr_NormalizedLongitudinalProfile180GeV, "pe", "pesame", "pesame",  False, False, False, "Proton")
  # save
  gr_piresponse.Write()
  gr_piresolution.Write()
  gr_pilateralspread.Write()
  gr_prresponse.Write()
  gr_prresolution.Write()
  gr_prlateralspread.Write()
  pi_LongitudinalProfile20GeV.Write()
  pi_LongitudinalProfile50GeV.Write()
  pi_LongitudinalProfile100GeV.Write()
  pi_LongitudinalProfile180GeV.Write()
  pi_NormalizedLongitudinalProfile20GeV.Write()
  pi_NormalizedLongitudinalProfile50GeV.Write()
  pi_NormalizedLongitudinalProfile100GeV.Write()
  pi_NormalizedLongitudinalProfile180GeV.Write()
  pr_LongitudinalProfile50GeV.Write()
  pr_LongitudinalProfile100GeV.Write()
  pr_LongitudinalProfile180GeV.Write()
  pr_NormalizedLongitudinalProfile50GeV.Write()
  pr_NormalizedLongitudinalProfile100GeV.Write()
  pr_NormalizedLongitudinalProfile180GeV.Write()
  outputFile.Write()

# compare the mc results with data
def ComDataMC():
  for Particle in Particles: # loop over particles
    inputFile = ROOT.TFile.Open('{}/{}/Properities_{}.root'.format(ResultDir,Particle,Particle)) # input file generated in GetPlotSingleProperty(), contain all MC results
    inputFile2 = ROOT.TFile.Open('{}/data/data.root'.format(ResultDir)) # input file generated in GetDataPlotSingleProperty(), contain all Data results
    if not inputFile:
      continue
    outputFile = ROOT.TFile.Open('{}/{}/{}_Ratio.root'.format(ResultDir, Particle, Particle),'RECREATE') # out file to store rations of MC to data

    ResponseList = [] # list of responses of all physics lists
    ResolutionList = [] #  list of reslotionss of all physics lists
    LateralSpreadList = [] # list of latreal spreadd of all physics lists
    ResponseRatioList = [] #list of ratios of responses MCs with all physics lists to data
    ResolutionRatioList = [] # list of ratios of resolutions MCs to data
    LateralSpreadRatioList = [] # list of ratios of lateral spreads MCs to data

    # get grapherrors from data file
    ger_dataresponse = inputFile2.Get("{}_Response".format(Particle))
    ger_dataresolution = inputFile2.Get("{}_Resolution".format(Particle))
    ger_datalateralspread = inputFile2.Get("{}_LateralSpread".format(Particle))
    # list of profiles of all beam energies
    datalongitudinalprofilelist =  []
    datanormalizedlongitudinalprofilelist =  []

    # list of profiles of MC
    mclongitudinalprofilelists =  []
    mcnormalizedlongitudinalprofilelists =  []
    # list of ratios of profiles of MCs to data
    longitudinalprofileratiolists =  []
    normalizedlongitudinalprofileratiolists =  []

    # loop over the beam energies to get all profiles of this particle of data
    for Energy in Energies:
      # proton doesn't has beam energy 20 GeV
      if Particle=='pr' and Energy==20000: continue
      datalongitudinalprofilelist.append(inputFile2.Get("{}_LongitudinalProfile_{}GeV".format(Particle, Energy/1000)))
      datanormalizedlongitudinalprofilelist.append(inputFile2.Get("{}_NormalizedLongitudinalProfile{}GeV".format(Particle, Energy/1000)))

    # loop over physics lists, 
    # to get all responses, resolutions and lateral spreads of each physics lists.
    for PhysicsList in PhysicsLists:
      ger_mcresponse = inputFile.Get("{}_{}_Response".format(Particle, PhysicsList))
      ger_mcresponse.SetTitle(PhysicsList)
      ger_mcresolution = inputFile.Get("{}_{}_Resolution".format(Particle, PhysicsList))
      ger_mcresolution.SetTitle(PhysicsList)
      ger_mclateralspread = inputFile.Get("{}_{}_LateralSpread".format(Particle, PhysicsList))
      ger_mclateralspread.SetTitle(PhysicsList)

      ResponseList.append(ger_mcresponse)
      ResolutionList.append(ger_mcresolution)
      LateralSpreadList.append(ger_mclateralspread)

      N = ger_dataresponse.GetN()
      # create histograms of responses, resolutions and lateral spreands of data,
      # divide by the corresponding histogram of MC.
      # number of bins = number of points in corresponding grapherrors.
      h_data_response = ROOT.TH1F("h_data_response","data",N, 0, N) ;
      h_data_resolution = ROOT.TH1F("h_data_resolution","data",N, 0, N) ;
      h_data_lateralspread = ROOT.TH1F("h_data_lateralspread","data",N, 0, N) ;
      Xs = ger_dataresponse.GetX()
      Xerrors = ger_dataresponse.GetEX()
      dataresponses = ger_dataresponse.GetY()
      dataresolutions = ger_dataresolution.GetY()
      datalateralspreads = ger_datalateralspread.GetY()
      # fill the point values to histograms
      for i in range(N):
        h_data_response.SetBinContent(i+1, dataresponses[i])
        h_data_response.SetBinError(i+1, ger_dataresponse.GetErrorY(i))
        h_data_resolution.SetBinContent(i+1, dataresolutions[i])
        h_data_resolution.SetBinError(i+1, ger_dataresolution.GetErrorY(i))
        h_data_lateralspread.SetBinContent(i+1, datalateralspreads[i])
        h_data_lateralspread.SetBinError(i+1, ger_datalateralspread.GetErrorY(i))
      # create histograms of responses, resolutions and lateral spreands of MC.
      h_mc_response = ROOT.TH1F("h_mc_response","",N, 0, N) ;
      h_mc_resolution = ROOT.TH1F("h_mc_resolution","",N, 0, N) ;
      h_mc_lateralspread = ROOT.TH1F("h_mc_lateralspread","",N, 0, N) ;
      mcresponses = ger_mcresponse.GetY()
      mcresolutions = ger_mcresolution.GetY()
      mclateralspreads = ger_mclateralspread.GetY()
      for i in range(N):
        if Particle=="pr": 
          # ptoton doesn't have 20 GeV, so skip the first point in grapherrors
          h_mc_response.SetBinContent(i+1, mcresponses[i+1])
          h_mc_response.SetBinError(i+1, ger_mcresponse.GetErrorY(i+1))
          h_mc_resolution.SetBinContent(i+1, mcresolutions[i+1])
          h_mc_resolution.SetBinError(i+1, ger_mcresolution.GetErrorY(i+1))
          h_mc_lateralspread.SetBinContent(i+1, mclateralspreads[i])
          h_mc_lateralspread.SetBinError(i+1, ger_mclateralspread.GetErrorY(i+1))
        elif Particle=="pi":
          h_mc_response.SetBinContent(i+1, mcresponses[i])
          h_mc_response.SetBinError(i+1, ger_mcresponse.GetErrorY(i))
          h_mc_resolution.SetBinContent(i+1, mcresolutions[i])
          h_mc_resolution.SetBinError(i+1, ger_mcresolution.GetErrorY(i))
          h_mc_lateralspread.SetBinContent(i+1, mclateralspreads[i])
          h_mc_lateralspread.SetBinError(i+1, ger_mclateralspread.GetErrorY(i))
      # divide two hists to get the ratios
      h_response_ratio = h_mc_response.Clone()
      h_response_ratio.Divide(h_data_response)
      h_resolution_ratio = h_mc_resolution.Clone()
      h_resolution_ratio.Divide(h_data_resolution)
      h_lateralspread_ratio = h_mc_lateralspread.Clone()
      h_lateralspread_ratio.Divide(h_data_lateralspread)
      # create grapherrors of ratios
      ger_response_ratio = ROOT.TGraphErrors()
      ger_response_ratio.SetName("{}_{}_Response_Ratio".format(Particle, PhysicsList))
      ger_response_ratio.SetTitle(PhysicsList)
      ger_resolution_ratio = ROOT.TGraphErrors()
      ger_resolution_ratio.SetName("{}_{}_Resolution_Ratio".format(Particle, PhysicsList))
      ger_resolution_ratio.SetTitle(PhysicsList)
      ger_lateralspread_ratio = ROOT.TGraphErrors()
      ger_lateralspread_ratio.SetName(PhysicsList)
      ger_lateralspread_ratio.SetTitle(PhysicsList)
      # set point values of grapherrors of ratios
      for i in range(N):
        ger_response_ratio.SetPoint(i, Xs[i], h_response_ratio.GetBinContent(i+1))
        ger_response_ratio.SetPointError(i, Xerrors[i], h_response_ratio.GetBinError(i+1))
        ger_resolution_ratio.SetPoint(i, Xs[i], h_resolution_ratio.GetBinContent(i+1))
        ger_resolution_ratio.SetPointError(i, Xerrors[i], h_resolution_ratio.GetBinError(i+1))
        ger_lateralspread_ratio.SetPoint(i, Xs[i], h_lateralspread_ratio.GetBinContent(i+1))
        ger_lateralspread_ratio.SetPointError(i, Xerrors[i], h_lateralspread_ratio.GetBinError(i+1))
      ger_response_ratio.GetXaxis().SetTitle("E_{beam}[GeV]")
      ger_response_ratio.GetYaxis().SetTitle("MC/Data")
      ger_resolution_ratio.GetXaxis().SetTitle("E_{beam}[GeV]")
      ger_resolution_ratio.GetYaxis().SetTitle("MC/Data")
      ger_lateralspread_ratio.GetXaxis().SetTitle("E_{beam}[GeV]")
      ger_lateralspread_ratio.GetYaxis().SetTitle("MC/Data")
     
      outputFile.cd()
      # save
      ger_response_ratio.Write()
      ger_resolution_ratio.Write()
      ger_lateralspread_ratio.Write()
      # append to list
      ResponseRatioList.append(ger_response_ratio)
      ResolutionRatioList.append(ger_resolution_ratio)
      LateralSpreadRatioList.append(ger_lateralspread_ratio)
      # draw the single ratio
      DrawSingleGraphErrorsOnCanvas("{}/{}/{}_{}_Response_Ratio".format(PlotDir, Particle, Particle, PhysicsList), ger_response_ratio,"AP", False, False, False, PhysicsList)
      DrawSingleGraphErrorsOnCanvas("{}/{}/{}_{}_LateralSpread_Ratio".format(PlotDir, Particle, Particle, PhysicsList), ger_lateralspread_ratio,"AP", False, False, False, PhysicsList)
      DrawSingleGraphErrorsOnCanvas("{}/{}/{}_{}_Resolution_Ratio".format(PlotDir, Particle, Particle, PhysicsList), ger_resolution_ratio,"AP", False, False, False, PhysicsList)

      #------------------Longitudinal Profile----------------------------
      # list of longitudinal profile of all types of particles and all beam energies and all physics lists
      # N = N of types of particles *  N of beam energies * N of physics lists
      mclongitudinalprofilelist=[]
      mcnormalizedlongitudinalprofilelist=[]
      mclongitudinalprofileratiolist=[]
      mcnormalizedlongitudinalprofileratiolist=[]
      for Energy in Energies:
        # skip 20 GeV for proton
        if Particle=='pr' and Energy==20000: continue
        mclongitudinalprofilelist.append(inputFile.Get("{}_{}GeV_{}_LongitudinalProfile".format(Particle, Energy/1000,PhysicsList)))
        mcnormalizedlongitudinalprofilelist.append(inputFile.Get("{}_{}GeV_{}_NormalizedLongitudinalProfile".format(Particle, Energy/1000, PhysicsList)))
      print mclongitudinalprofilelist, mcnormalizedlongitudinalprofilelist

      # get the ratios of longitudinal profiles
      for i in range(len(mclongitudinalprofilelist)):
        longitudinalprofilelistratio = mclongitudinalprofilelist[i].Clone()
        longitudinalprofilelistratio.Divide(datalongitudinalprofilelist[i])
        longitudinalprofilelistratio.SetName(longitudinalprofilelistratio.GetName()+"_Ratio")
        longitudinalprofilelistratio.GetYaxis().SetTitle("MC/Data")
        longitudinalprofilelistratio.GetYaxis().SetRangeUser(0.65, 1.45)
        longitudinalprofilelistratio.Write()
        mclongitudinalprofileratiolist.append(longitudinalprofilelistratio)
        normalizedlongitudinalprofilelistratio = mcnormalizedlongitudinalprofilelist[i].Clone()
        normalizedlongitudinalprofilelistratio.Divide(datanormalizedlongitudinalprofilelist[i])
        normalizedlongitudinalprofilelistratio.SetName(normalizedlongitudinalprofilelistratio.GetName()+"_Ratio")
        normalizedlongitudinalprofilelistratio.GetYaxis().SetTitle("MC/Data")
        normalizedlongitudinalprofilelistratio.GetYaxis().SetRangeUser(0.65, 1.45)
        normalizedlongitudinalprofilelistratio.Write()
        mcnormalizedlongitudinalprofileratiolist.append(normalizedlongitudinalprofilelistratio)
        # draw single ratio of longitudinal profiles
        if Particle=="pr":
          DrawSingleHistOnCanvas("{}/{}/{}_{}_{}_LongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, Energies[i+1]/1000, PhysicsList),longitudinalprofilelistratio, "PE", False, False)
          DrawSingleHistOnCanvas("{}/{}/{}_{}_{}_NormalizedLongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, Energies[i+1]/1000, PhysicsList),normalizedlongitudinalprofilelistratio, "PE", False, False)
        elif Particle=="pi":
          DrawSingleHistOnCanvas("{}/{}/{}_{}_{}_LongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, Energies[i]/1000, PhysicsList),longitudinalprofilelistratio, "PE", False, False)
          DrawSingleHistOnCanvas("{}/{}/{}_{}_{}_NormalizedLongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, Energies[i]/1000, PhysicsList),normalizedlongitudinalprofilelistratio, "PE", False, False)
      # append the ratio to list
      mclongitudinalprofilelists.append(mclongitudinalprofilelist)
      mcnormalizedlongitudinalprofilelists.append(mcnormalizedlongitudinalprofilelist)
      longitudinalprofileratiolists.append(mclongitudinalprofileratiolist)
      normalizedlongitudinalprofileratiolists.append(mcnormalizedlongitudinalprofileratiolist)
      FullParticleName=""
      # draw rations of longitudinal profiles of all beam energies on same canvas
      if Particle=='pi':
        FullParticleName = "Pion"
        DrawFourHistsOnCanvas("{}/{}/{}_{}_LongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, PhysicsList),mclongitudinalprofileratiolist[0],mclongitudinalprofileratiolist[1],mclongitudinalprofileratiolist[2],mclongitudinalprofileratiolist[3], "PE","pesame","pesame","pesame", False, False, False, FullParticleName, PhysicsList)
        DrawFourHistsOnCanvas("{}/{}/{}_{}_NormalizedLongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, PhysicsList),mcnormalizedlongitudinalprofileratiolist[0],mcnormalizedlongitudinalprofileratiolist[1],mcnormalizedlongitudinalprofileratiolist[2],mcnormalizedlongitudinalprofileratiolist[3], "PE","pesame","pesame","pesame", False, False, False, FullParticleName,PhysicsList)
      else:
        FullParticleName = "Proton"
        DrawThreeHistsOnCanvas("{}/{}/{}_{}_LongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, PhysicsList),mclongitudinalprofileratiolist[0],mclongitudinalprofileratiolist[1],mclongitudinalprofileratiolist[2], "PE","pesame","pesame", False, False, False, FullParticleName, PhysicsList)
        DrawThreeHistsOnCanvas("{}/{}/{}_{}_NormalizedLongitudinalProfile_Ratio".format(PlotDir, Particle, Particle, PhysicsList),mcnormalizedlongitudinalprofileratiolist[0],mcnormalizedlongitudinalprofileratiolist[1],mcnormalizedlongitudinalprofileratiolist[2], "PE","pesame","pesame", False, False, False, FullParticleName, PhysicsList)
        
    FullParticleName=""
    if Particle=='pi':
      FullParticleName = "Pion"
      ger_dataresponse.SetTitle("Data")
      ger_dataresolution.SetTitle("Data")
      ger_datalateralspread.SetTitle("Data")
    elif Particle=='pr':
      FullParticleName = "Proton"
      ger_dataresponse.GetXaxis().SetRangeUser(40, 190) 
      ger_dataresolution.GetXaxis().SetRangeUser(40, 190) 
      ger_datalateralspread.GetXaxis().SetRangeUser(40, 190) 
      ger_dataresponse.SetTitle("Data")
      ger_dataresolution.SetTitle("Data")
      ger_datalateralspread.SetTitle("Data")
      for npr in range(len(ResponseList)):
        ResponseList[npr].GetXaxis().SetRangeUser(40, 190)
        ResolutionList[npr].GetXaxis().SetRangeUser(40, 190)
        LateralSpreadList[npr].GetXaxis().SetRangeUser(40, 190)
        ResponseRatioList[npr].GetXaxis().SetRangeUser(40, 190)
        ResolutionRatioList[npr].GetXaxis().SetRangeUser(40, 190)
        LateralSpreadRatioList[npr].GetXaxis().SetRangeUser(40, 190)
        ResponseList[npr].RemovePoint(0)
        ResolutionList[npr].RemovePoint(0)
        LateralSpreadList[npr].RemovePoint(0)

    # draw responses, resolutions and lateral spread of all physcis lists on same canvas.
    # draw responses, resolutions and lateral spread of all physcis lists and data on top,
    # and ratios of MC to data on bottom
    DrawFourGraphErrorsOnCanvas("{}/{}/{}_Response_Ratio".format(PlotDir, Particle, Particle),ResponseRatioList[0], ResponseRatioList[1], ResponseRatioList[2],ResponseRatioList[3], "AP","AP","AP","AP", False, False, False, FullParticleName)
    DrawTopFiveGraphErrorsAndBottomFourGraphErrorsOnCanvas("{}/{}/{}_TopResponseBottomRatio".format(PlotDir, Particle, Particle),ger_dataresponse, ResponseList[0], ResponseList[1], ResponseList[2],ResponseList[3], ResponseRatioList[0], ResponseRatioList[1], ResponseRatioList[2],ResponseRatioList[3], "AP","AP","AP","AP", "AP","AP","AP","AP", "AP", False, False, False, False, FullParticleName)
    DrawFourGraphErrorsOnCanvas("{}/{}/{}_Resolution_Ratio".format(PlotDir, Particle, Particle),ResolutionRatioList[0], ResolutionRatioList[1], ResolutionRatioList[2],ResolutionRatioList[3],"AP","AP","AP","AP", False, False, False,FullParticleName)
    DrawTopFiveGraphErrorsAndBottomFourGraphErrorsOnCanvas("{}/{}/{}_TopResolutionBottomRatio".format(PlotDir, Particle, Particle),ger_dataresolution, ResolutionList[0], ResolutionList[1], ResolutionList[2],ResolutionList[3], ResolutionRatioList[0], ResolutionRatioList[1], ResolutionRatioList[2],ResolutionRatioList[3], "AP","AP","AP","AP", "AP","AP","AP","AP", "AP", False, False, False, False, FullParticleName)
    DrawFourGraphErrorsOnCanvas("{}/{}/{}_LateralSpread_Ratio".format(PlotDir, Particle, Particle),LateralSpreadRatioList[0], LateralSpreadRatioList[1], LateralSpreadRatioList[2],LateralSpreadRatioList[3],"AP","AP","AP","AP", False, False, False,FullParticleName)
    DrawTopFiveGraphErrorsAndBottomFourGraphErrorsOnCanvas("{}/{}/{}_TopLateralSpreadBottomRatio".format(PlotDir, Particle, Particle),ger_datalateralspread, LateralSpreadList[0], LateralSpreadList[1], LateralSpreadList[2],LateralSpreadList[3], LateralSpreadRatioList[0], LateralSpreadRatioList[1], LateralSpreadRatioList[2],LateralSpreadRatioList[3], "AP","AP","AP","AP", "AP","AP","AP","AP", "AP", False, False, False, False, FullParticleName)

    for i in range(len(Energies)):
      if Particle=="pi":
        datalongitudinalprofilelist[i].GetYaxis().SetRangeUser(5E-3, 100.)
        if(Energies[i]==20000): 
          datalongitudinalprofilelist[i].GetYaxis().SetRangeUser(5E-3, 10.)
        datalongitudinalprofilelist[i].SetTitle("Data")
        mclongitudinalprofilelists[0][i].SetTitle(PhysicsLists[0])
        mclongitudinalprofilelists[1][i].SetTitle(PhysicsLists[1])
        mclongitudinalprofilelists[2][i].SetTitle(PhysicsLists[2])
        mclongitudinalprofilelists[3][i].SetTitle(PhysicsLists[3])
        longitudinalprofileratiolists[0][i].SetTitle(PhysicsLists[0])
        longitudinalprofileratiolists[1][i].SetTitle(PhysicsLists[1])
        longitudinalprofileratiolists[2][i].SetTitle(PhysicsLists[2])
        longitudinalprofileratiolists[3][i].SetTitle(PhysicsLists[3])
        datanormalizedlongitudinalprofilelist[i].GetYaxis().SetRangeUser(5E-5, 1.)
        datanormalizedlongitudinalprofilelist[i].SetTitle("Data")
        mcnormalizedlongitudinalprofilelists[0][i].SetTitle(PhysicsLists[0])
        mcnormalizedlongitudinalprofilelists[1][i].SetTitle(PhysicsLists[1])
        mcnormalizedlongitudinalprofilelists[2][i].SetTitle(PhysicsLists[2])
        mcnormalizedlongitudinalprofilelists[3][i].SetTitle(PhysicsLists[3])
        normalizedlongitudinalprofileratiolists[0][i].SetTitle(PhysicsLists[0])
        normalizedlongitudinalprofileratiolists[1][i].SetTitle(PhysicsLists[1])
        normalizedlongitudinalprofileratiolists[2][i].SetTitle(PhysicsLists[2])
        normalizedlongitudinalprofileratiolists[3][i].SetTitle(PhysicsLists[3])
        # draw profiles of all physcis lists of each beam energy on same canvas
        # draw profiles of all physcis lists of each beam energy and data on top, ratios of MC to data on bottom
        DrawFiveHistsOnCanvas("{}/{}/{}_LongitudinalProfileWithData_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datalongitudinalprofilelist[i],mclongitudinalprofilelists[0][i], mclongitudinalprofilelists[1][i],mclongitudinalprofilelists[2][i],mclongitudinalprofilelists[3][i], "PE", "PESame", "PESame", "PESame", "PESame", False, True, False, "Pion", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawFiveHistsOnCanvas("{}/{}/{}_NormalizedLongitudinalProfileWithData_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datanormalizedlongitudinalprofilelist[i],mcnormalizedlongitudinalprofilelists[0][i], mcnormalizedlongitudinalprofilelists[1][i],mcnormalizedlongitudinalprofilelists[2][i],mcnormalizedlongitudinalprofilelists[3][i], "PE", "PESame", "PESame", "PESame", "PESame", False, True, False, "Pion", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawFourHistsOnCanvas("{}/{}/{}_LongitudinalProfile_Ratio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),longitudinalprofileratiolists[0][i], longitudinalprofileratiolists[1][i],longitudinalprofileratiolists[2][i],longitudinalprofileratiolists[3][i], "PE", "PESame", "PESame", "PESame", False, False, False, "Pion", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawFourHistsOnCanvas("{}/{}/{}_NormalizedLongitudinalProfile_Ratio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),normalizedlongitudinalprofileratiolists[0][i], normalizedlongitudinalprofileratiolists[1][i],normalizedlongitudinalprofileratiolists[2][i],normalizedlongitudinalprofileratiolists[3][i], "PE", "PESame", "PESame", "PESame", False, False, False, "Pion", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawTopFiveHistsAndBottomFourHistsOnCanvas("{}/{}/{}_TopLongitudinalProfileBottomRatio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datalongitudinalprofilelist[i], mclongitudinalprofilelists[0][i], mclongitudinalprofilelists[1][i],mclongitudinalprofilelists[2][i],mclongitudinalprofilelists[3][i], longitudinalprofileratiolists[0][i], longitudinalprofileratiolists[1][i],longitudinalprofileratiolists[2][i],longitudinalprofileratiolists[3][i], "PE", "PESame", "PESame", "PESame", "PESame", "PE", "PESame", "PESame", "PESame", False, True, False, False, "Pion", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawTopFiveHistsAndBottomFourHistsOnCanvas("{}/{}/{}_TopNormalizedLongitudinalProfileBottomRatio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datanormalizedlongitudinalprofilelist[i], mcnormalizedlongitudinalprofilelists[0][i], mcnormalizedlongitudinalprofilelists[1][i],mcnormalizedlongitudinalprofilelists[2][i],mcnormalizedlongitudinalprofilelists[3][i], normalizedlongitudinalprofileratiolists[0][i], normalizedlongitudinalprofileratiolists[1][i],normalizedlongitudinalprofileratiolists[2][i],normalizedlongitudinalprofileratiolists[3][i], "PE", "PESame", "PESame", "PESame", "PESame", "PE", "PESame", "PESame", "PESame", False, True, False, False, "Pion", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
      elif Particle=="pr":
        # proton doesn't have beam energy og 20 GeV in data.
        if Energies[i]==20000: continue
        datalongitudinalprofilelist[i-1].GetYaxis().SetRangeUser(5E-3, 100.)
        datalongitudinalprofilelist[i-1].SetTitle("Data")
        mclongitudinalprofilelists[0][i-1].SetTitle(PhysicsLists[0])
        mclongitudinalprofilelists[1][i-1].SetTitle(PhysicsLists[1])
        mclongitudinalprofilelists[2][i-1].SetTitle(PhysicsLists[2])
        mclongitudinalprofilelists[3][i-1].SetTitle(PhysicsLists[3])
        longitudinalprofileratiolists[0][i-1].SetTitle(PhysicsLists[0])
        longitudinalprofileratiolists[1][i-1].SetTitle(PhysicsLists[1])
        longitudinalprofileratiolists[2][i-1].SetTitle(PhysicsLists[2])
        longitudinalprofileratiolists[3][i-1].SetTitle(PhysicsLists[3])
        datanormalizedlongitudinalprofilelist[i-1].GetYaxis().SetRangeUser(5E-5, 1.)
        datanormalizedlongitudinalprofilelist[i-1].SetTitle("Data")
        mcnormalizedlongitudinalprofilelists[0][i-1].SetTitle(PhysicsLists[0])
        mcnormalizedlongitudinalprofilelists[1][i-1].SetTitle(PhysicsLists[1])
        mcnormalizedlongitudinalprofilelists[2][i-1].SetTitle(PhysicsLists[2])
        mcnormalizedlongitudinalprofilelists[3][i-1].SetTitle(PhysicsLists[3])
        normalizedlongitudinalprofileratiolists[0][i-1].SetTitle(PhysicsLists[0])
        normalizedlongitudinalprofileratiolists[1][i-1].SetTitle(PhysicsLists[1])
        normalizedlongitudinalprofileratiolists[2][i-1].SetTitle(PhysicsLists[2])
        normalizedlongitudinalprofileratiolists[3][i-1].SetTitle(PhysicsLists[3])
        DrawFiveHistsOnCanvas("{}/{}/{}_LongitudinalProfileWithData_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datalongitudinalprofilelist[i-1],mclongitudinalprofilelists[0][i-1], mclongitudinalprofilelists[1][i-1],mclongitudinalprofilelists[2][i-1], mclongitudinalprofilelists[3][i-1], "PE", "PESame", "PESame", "PESame", "PESame", False, True, False, "Proton", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawFiveHistsOnCanvas("{}/{}/{}_NormalizedLongitudinalProfileWithData_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datanormalizedlongitudinalprofilelist[i-1],mcnormalizedlongitudinalprofilelists[0][i-1], mcnormalizedlongitudinalprofilelists[1][i-1],mcnormalizedlongitudinalprofilelists[2][i-1], mcnormalizedlongitudinalprofilelists[3][i-1], "PE", "PESame", "PESame", "PESame", "PESame", False, True, False, "Proton", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawFourHistsOnCanvas("{}/{}/{}_LongitudinalProfile_Ratio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),longitudinalprofileratiolists[0][i-1], longitudinalprofileratiolists[1][i-1],longitudinalprofileratiolists[2][i-1], longitudinalprofileratiolists[3][i-1], "PE", "PESame", "PESame", "PESame", False, False, False, "Proton","E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawFourHistsOnCanvas("{}/{}/{}_NormalizedLongitudinalProfile_Ratio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),normalizedlongitudinalprofileratiolists[0][i-1], normalizedlongitudinalprofileratiolists[1][i-1],normalizedlongitudinalprofileratiolists[2][i-1], normalizedlongitudinalprofileratiolists[3][i-1],"PE", "PESame", "PESame", "PESame", False, False, False, "Proton", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawTopFiveHistsAndBottomFourHistsOnCanvas("{}/{}/{}_TopLongitudinalProfileBottomRatio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datalongitudinalprofilelist[i-1], mclongitudinalprofilelists[0][i-1], mclongitudinalprofilelists[1][i-1],mclongitudinalprofilelists[2][i-1],mclongitudinalprofilelists[3][i-1], longitudinalprofileratiolists[0][i-1], longitudinalprofileratiolists[1][i-1],longitudinalprofileratiolists[2][i-1],longitudinalprofileratiolists[3][i-1], "PE", "PESame", "PESame", "PESame", "PESame", "PE", "PESame", "PESame", "PESame", False, True, False, False, "Proton", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
        DrawTopFiveHistsAndBottomFourHistsOnCanvas("{}/{}/{}_TopNormalizedLongitudinalProfileBottomRatio_{}GeV".format(PlotDir, Particle, Particle, Energies[i]/1000),datanormalizedlongitudinalprofilelist[i-1], mcnormalizedlongitudinalprofilelists[0][i-1], mcnormalizedlongitudinalprofilelists[1][i-1],mcnormalizedlongitudinalprofilelists[2][i-1],mcnormalizedlongitudinalprofilelists[3][i-1], normalizedlongitudinalprofileratiolists[0][i-1], normalizedlongitudinalprofileratiolists[1][i-1],normalizedlongitudinalprofileratiolists[2][i-1],normalizedlongitudinalprofileratiolists[3][i-1], "PE", "PESame", "PESame", "PESame", "PESame", "PE", "PESame", "PESame", "PESame", False, True, False, False, "Proton", "E_{beam}="+"{}GeV".format(Energies[i]/1000))
      

# draw data and MC  on same camvas, no ratios
def ComparePhysicsList():
  for Particle in Particles: # loop over particles
    # mc input files containing grapherrorses of responses, resolutions and lateral spreads and histograms of longitudinal profiles
    inputFile = ROOT.TFile.Open('{}/{}/Properities_{}.root'.format(ResultDir, Particle, Particle))
    # data input files containing grapherrorses of responses, resolutions and lateral spreads and histograms of longitudinal profiles
    inputFile2 = ROOT.TFile.Open('{}/data/data.root'.format(ResultDir))
    if not inputFile:
      print "File: ",inputFile.GetName()," doesn't exist!!"
      continue
    # list of grapherrors and responses, resolutions and lateral spreads of MC
    ResponseList = []
    ResolutionList = []
    LateralSpreadList = []
    # list of grapherrors and responses, resolutions and lateral spreads of data and MC
    ResponseListWithData = []
    ResolutionListWithData = []
    LateralSpreadListWithData = []
    # get data results 
    ger_dataresponse = inputFile2.Get("{}_Response".format(Particle))
    ger_dataresponse.SetTitle("Data")
    ger_dataresolution = inputFile2.Get("{}_Resolution".format(Particle))
    ger_dataresolution.SetTitle("Data")
    ger_datalateralspread = inputFile2.Get("{}_LateralSpread".format(Particle))
    ger_datalateralspread.SetTitle("Data")
    ResponseListWithData.append(ger_dataresponse)
    ResolutionListWithData.append(ger_dataresolution)
    LateralSpreadListWithData.append(ger_datalateralspread)
    # loop over physics to get grapherrors and responses, 
    # resolutions and lateral spreads of MC
    for PhysicsList in PhysicsLists:
      ger_response = inputFile.Get("{}_{}_Response".format(Particle, PhysicsList))
      ger_response.SetTitle(PhysicsList)
      ger_resolution = inputFile.Get("{}_{}_Resolution".format(Particle, PhysicsList))
      ger_resolution.SetTitle(PhysicsList)
      ger_lateralspread = inputFile.Get("{}_{}_LateralSpread".format(Particle, PhysicsList))
      ger_lateralspread.SetTitle(PhysicsList)
      ResponseList.append(ger_response)
      ResolutionList.append(ger_resolution)
      LateralSpreadList.append(ger_lateralspread)
      ResponseListWithData.append(ger_response)
      ResolutionListWithData.append(ger_resolution)
      LateralSpreadListWithData.append(ger_lateralspread)
    print ResponseList,ResolutionList,LateralSpreadList

    FullParticleName=""
    if Particle=='pi':
      FullParticleName = "Pion"
    else:
      FullParticleName = "Proton"
    # draw results of proton of MC and data on same canvas
    if len(ResponseList)==3:
      DrawThreeGraphErrorsOnCanvas("{}/{}/{}_Response".format(PlotDir,Particle,Particle),ResponseList[0], ResponseList[1], ResponseList[2],"AP","AP","AP")
      DrawThreeGraphErrorsOnCanvas("{}/{}/{}_Resolution".format(PlotDir,Particle,Particle),ResolutionList[0], ResolutionList[1], ResolutionList[2],"AP","AP","AP")
    # draw results of pion of MC and data on same canvas
    elif len(ResponseList)==4:
      DrawFourGraphErrorsOnCanvas("{}/{}/{}_Response".format(PlotDir,Particle,Particle),ResponseList[0], ResponseList[1], ResponseList[2],ResponseList[3], "AP","AP","AP","AP", False, False, False,FullParticleName)
      DrawFourGraphErrorsOnCanvas("{}/{}/{}_Resolution".format(PlotDir,Particle,Particle),ResolutionList[0], ResolutionList[1], ResolutionList[2],ResolutionList[3],"AP","AP","AP","AP", False, False, False, FullParticleName)
      DrawFourGraphErrorsOnCanvas("{}/{}/{}_LateralSpread".format(PlotDir,Particle,Particle),LateralSpreadList[0], LateralSpreadList[1], LateralSpreadList[2],LateralSpreadList[3],"AP","AP","AP","AP", False, False, False, FullParticleName)
      DrawFiveGraphErrorsOnCanvas("{}/{}/{}_ResponseWithData".format(PlotDir,Particle,Particle),ResponseListWithData[0], ResponseListWithData[1], ResponseListWithData[2],ResponseListWithData[3], ResponseListWithData[4], "AP","AP","AP","AP", "AP", False, False, False,FullParticleName)
      DrawFiveGraphErrorsOnCanvas("{}/{}/{}_ResolutionWithData".format(PlotDir,Particle,Particle),ResolutionListWithData[0], ResolutionListWithData[1], ResolutionListWithData[2],ResolutionListWithData[3],ResolutionListWithData[4], "AP","AP","AP","AP", "Ap", False, False, False,FullParticleName)
      DrawFiveGraphErrorsOnCanvas("{}/{}/{}_LateralSpreadWithData".format(PlotDir,Particle,Particle),LateralSpreadListWithData[0], LateralSpreadListWithData[1], LateralSpreadListWithData[2],LateralSpreadListWithData[3],LateralSpreadListWithData[4],"AP","AP","AP","AP","AP", False, False, False,FullParticleName)

if __name__ == '__main__':
  GetPlotSingleProperty()
  GetDataPlotSingleProperty()
  ComDataMC()
  ComparePhysicsList()
