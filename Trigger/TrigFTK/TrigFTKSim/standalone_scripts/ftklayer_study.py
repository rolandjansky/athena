#!/usr/bin/env python 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# standard libraries
import sys, os, os.path, glob
from math import *
from optparse import OptionParser

# try to load ROOT, not mandatory, if ROOT is found the
# data are collected in histograms, if not only ascii files
# are written to collect the information
try :
    import ROOT
    hasROOT = True
    print "INFO: ROOT histograms will be created"
except :
    hasROOT = False
    print "WARNING: ROOT histograms will not be created"

# load ftkutils library to read truth wrapper files and plane-map
import ftkutils
print "environment ready"

# create a plane-map object and load the 11L pmap, the pmap file
# should be coupied in the directory where the code is
pmapname = "raw_12Libl.pmap"
pmappath = None
for datadir in os.environ["DATAPATH"].split(":") :
    testpath = datadir+"/ftk_configuration/map_files/"+pmapname
    print testpath
    if os.path.exists(testpath) :
        print "PMAP file found in:", testpath
        pmappath = testpath
        break

if not pmappath :
    print "No PMAP"
    sys.exit(-1) 
pmap = ftkutils.FTKPlaneMap(pmappath)

# if ROOT is found create the histograms
if hasROOT :
    hfilelayer = ROOT.TFile.Open("layerstudy_histo.root","recreate")
    # histograms able to do statistic on the layers efficiency
    # whole detector
    h_layereff = ROOT.TH1F("layereff","Layer with hits",11,-.5,10.5)
    h_layereffN = ROOT.TH1F("layereffN","Layer with hits (Normalization)",11,-.5,10.5)
    h_layereff_phi = ROOT.TH2F("layereff_phi","Layer with hits vs #phi",11,-.5,10.5,10,-pi,pi)
    h_layereff_phiN = ROOT.TH2F("layereff_phiN","Layer with hits vs #phi (Normalization",11,-.5,10.5,10,-pi,pi)
    h_layereff_eta = ROOT.TH2F("layereff_eta","Layer with hits vs #eta",11,-.5,10.5,10,-2.5,2.5)
    h_layereff_etaN = ROOT.TH2F("layereff_etaN","Layer with hits vs #eta (Normalization",11,-.5,10.5,10,-2.5,2.5)
    # barrel 
    h_layereffB = ROOT.TH1F("layereffB","Layer with hits (Barrel)",11,-.5,10.5)
    h_layereffBN = ROOT.TH1F("layereffBN","Layer with hits (Barrel Normalization)",11,-.5,10.5)
    h_layereff_phiB = ROOT.TH2F("layereff_phiB","Layer with hits vs #phi (Barrel)",11,-.5,10.5,10,-pi,pi)
    h_layereff_phiBN = ROOT.TH2F("layereff_phiBN","Layer with hits vs #phi ( Barrel Normalization)",11,-.5,10.5,10,-pi,pi)
    h_layereff_etaB = ROOT.TH2F("layereff_etaB","Layer with hits vs #eta (Barrel)",11,-.5,10.5,5,-.9,.9)
    h_layereff_etaBN = ROOT.TH2F("layereff_etaBN","Layer with hits vs #eta (Barrel Normalization)",11,-.5,10.5,5,-.9,.9)
    # endcaps 
    h_layereffE = ROOT.TH1F("layereffE","Layer with hits (Endcaps)",11,-.5,10.5)
    h_layereffEN = ROOT.TH1F("layereffEN","Layer with hits (Endcaps Normalization)",11,-.5,10.5)
    h_layereff_phiE = ROOT.TH2F("layereff_phiE","Layer with hits vs #phi (Endcaps)",11,-.5,10.5,10,-pi,pi)
    h_layereff_phiEN = ROOT.TH2F("layereff_phiEN","Layer with hits vs #phi ( Endcaps Normalization)",11,-.5,10.5,10,-pi,pi)
    h_layereff_etaE = ROOT.TH2F("layereff_etaE","Layer with hits vs #eta (Endcaps)",11,-.5,10.5,5,-1.1,2.5)
    h_layereff_etaEN = ROOT.TH2F("layereff_etaEN","Layer with hits vs #eta (Endcaps Normalization)",11,-.5,10.5,5,1.1,2.5)


def fillNormalizationHisto(histo,val=None) :
    """Increment all the reference bins by 1"""
    nbinsX = histo.GetNbinsX()
    xvals = []
    for i in range(1,nbinsX+1) :
        x = histo.GetXaxis().GetBinCenter(i)
        xvals.append(x)
    if histo.InheritsFrom("TH2") :
        # 2D histogram case
        for x in xvals :
            histo.Fill(x,val)
    else :
        for x in xvals : 
            histo.Fill(x)

def makeEfficiency(name,title,histoP,histoT) :
    """Evaluate he efficiency graph, if 2D histograms are compared the result
    is an array fo graphs"""
    if histoP.InheritsFrom("TH2") :
        graphs = []
        nbinsy = histoT.GetNbinsY()
        for ib in range(1,nbinsy+1) :
            hslP = histoP.ProjectionX("htemp"+str(ib),ib,ib)
            hslT = histoT.ProjectionX("htempN"+str(ib),ib,ib)
            curgr = ROOT.TGraphAsymmErrors(hslP,hslT)
            curgr.SetName(name+str(ib))
            curgr.SetTitle(title+str(ib)) # improve
            graphs.append(curgr)
        return graphs
    else :
        graph = ROOT.TGraphAsymmErrors(histoP,histoT)
        graph.SetName(name)
        graph.SetTitle(title)
        graph.Write()
        return graph

# prepare the configurations to study
# a configuration is a sequence of integer, each indicating a
# logic layer ID.  The logical layer ID is based on the 11L pmap
conf7L = [0,1,2,3,5,7,9]
conf8L = [[0,1,2,3,4,5,7,9],
          [0,1,2,3,5,6,7,9],
          [0,1,2,3,5,7,8,9],
          [0,1,2,3,5,7,9,10],
          [0,1,2,3,4,5,6,7],
          [0,1,2,3,4,5,6,8],
          [0,1,2,3,4,5,6,9],
          [0,1,2,3,4,5,6,10]]

def nhits(layerConfiguration, hitsPerLayer) :
    """This function returns the number of layers having hits assuming a given layer configuration"""
    nhits = 0
    bitmask = 0
    for ilayer in layerConfiguration :
        if len(hitsPerLayer[ilayer])>0 :
            nhits += 1 # increment the counter of planes with hits
            bitmask |= 1<<ilayer # set on the bit for the given layer
    return (nhits, bitmask)

# create the ASCII file to store the number of hit in each layer and
# information on the truth and offline track in the event
layerout = open("layerstat.dat","w")
layerout.write("ievt Td0 Tphi0 Tpt Tz0 Teta Od0 Ophi0 Opt Oz0 Oeta BCfrac l0 l1 l2 l3 l4 l5 l6 l7 l8 l9 l10\n")

# create an ascii file storing for each event the information on
# the truth (T*) and offline (O*) track then the number of hits and
# the bitmask of fired layers in the configuration
confout = open("confstat.dat","w")
confout.write("ievt Td0 Tphi0 Tpt Tz0 Teta Od0 Ophi0 Opt Oz0 Oeta BCfrac C7L M7L C8La M8La C8Lb M8Lb C8Lc M8Lc C8Ld M8Ld C8LSa M8LSa C8LSb M8LSb C8LSc M8LSc C8LSd M8LSd\n")

# in prun the input files are in argv[1] divided by ','
inputlist = sys.argv[1].split(',')
print inputlist

# create a chain of wrapper files
ftkchain = ftkutils.FTKFileChain(inputlist)
ftkchain.FTKEvent.ignoreOffline = True
while ftkchain.nextevent() : # event loop
    # remove the hits in the event hanving barcode other then the truth
    ftkchain.FTKEvent.filterHitsBarcode(10001)

    # select the truth track
    truthTrack = None
    for itrk in ftkchain.FTKEvent.truth_tracks :
        # truthTrack is the track with the single generated track barcode
        if itrk.barcode == 10001 :
            truthTrack = itrk
            break

    # the events w/o, this shuold not happen, are skipped
    if not truthTrack :
        continue
    elif abs(truthTrack.eta)>2.5 : # out of the eta range
        continue

    # check if endcap or barrel event
    isBarrelEvt = False
    isEndcapEvt = False
    if abs(truthTrack.eta)<.9 :
        isBarrelEvt = True
    elif abs(truthTrack.eta)>1.1 :
        isEndcapEvt = True

    # check the offline track
    offlineTrack = None
    for itrk in ftkchain.FTKEvent.off_tracks :
        # select the offlne track as a track with the same barcode 
        # of the truth, no threshold is applied
        if itrk.barcode == 10001 :
            offlineTrack = itrk
            break
    # when offline is not present a set of dummy value is used
    if not offlineTrack :
        offlineTrack = ftkutils.FTKOffline([0]*8)

    # each layer is a position in the following array
    nhitsLayer = []
    for il in xrange(pmap.NLogicalLayers) :
        # for each layer there is a python dictionary storing
        # the layer position for each hit
        nhitsLayer.append({})
        
    for ihit in ftkchain.FTKEvent.raw_hits : # hits loop
        # evaluate the logical layer I using the PMAP
        logic_layer = pmap.data[ihit.type][not ihit.BarEcp==0][ihit.layer]

        # all the hits associated with the truth track are
        # grouped per logical layer keeping the information
        # on the original ATLAS layer ID
        modid = (ihit.layer,ihit.phi_module,ihit.eta_module)
        if not modid in nhitsLayer[logic_layer] :
            nhitsLayer[logic_layer][modid] = 1
        else :
            nhitsLayer[logic_layer][modid] += 1
        # end hits loop

    # print the information in the ASCII file that stores the
    # information in each layer
    print >>layerout, ftkchain.FTKEvent.event,
    print >>layerout, truthTrack.d0, truthTrack.phi0, truthTrack.pt, truthTrack.z0, truthTrack.eta,
    print >>layerout, offlineTrack.d0, offlineTrack.phi0, offlineTrack.qpt, offlineTrack.z0, offlineTrack.ctheta, offlineTrack.barcode_frac,

    totLayer = 0
    totOverlap = 0
    # print the number of hits in each logical layer
    for (i,layer) in enumerate(nhitsLayer) :
        nl = len(layer)
        print >>layerout, nl,
        if hasROOT and nl>0:
            h_layereff.Fill(i)
            h_layereff_phi.Fill(i,truthTrack.phi0)
            h_layereff_eta.Fill(i,truthTrack.eta)
            if isBarrelEvt :
                h_layereffB.Fill(i)
                h_layereff_phiB.Fill(i,truthTrack.phi0)
                h_layereff_etaB.Fill(i,truthTrack.eta)
            elif isEndcapEvt :
                h_layereffE.Fill(i)
                h_layereff_phiE.Fill(i,truthTrack.phi0)
                h_layereff_etaE.Fill(i,truthTrack.eta)
    print >>layerout # end-line

    # print the information in the ASCII file that stores the information
    # on the configuration efficiency
    print >>confout, ftkchain.FTKEvent.event,
    print >>confout, truthTrack.d0, truthTrack.phi0, truthTrack.pt, truthTrack.z0, truthTrack.eta,
    print >>confout, offlineTrack.d0, offlineTrack.phi0, offlineTrack.qpt, offlineTrack.z0, offlineTrack.ctheta, offlineTrack.barcode_frac,

    # print the 7L configuration 
    nhits7L, mask = nhits(conf7L,nhitsLayer)
    print >>confout, nhits7L, mask,

    # print the number of hits and the bitmask for each 8L configuration
    for lc8L in conf8L :
        nhits8L, mask = nhits(lc8L,nhitsLayer)
        print >>confout, nhits8L, mask,
    print >>confout # endline
            
    if hasROOT :
        # if ROOT is enabled fill the histograms
        fillNormalizationHisto(h_layereffN)
        fillNormalizationHisto(h_layereff_phiN,truthTrack.phi0)
        fillNormalizationHisto(h_layereff_etaN,truthTrack.eta)
        if isBarrelEvt :
            fillNormalizationHisto(h_layereffBN)
            fillNormalizationHisto(h_layereff_phiBN,truthTrack.phi0)
            fillNormalizationHisto(h_layereff_etaBN,truthTrack.eta)
        elif isEndcapEvt :
            fillNormalizationHisto(h_layereffEN)
            fillNormalizationHisto(h_layereff_phiEN,truthTrack.phi0)
            fillNormalizationHisto(h_layereff_etaEN,truthTrack.eta)        
    # end event loop

if hasROOT :
    # if ROOT is enabled write the file
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff,h_layereffN)
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff_phi,h_layereff_phiN)
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff_eta,h_layereff_etaN)
    makeEfficiency("gr_layereffB","Layer efficiency",h_layereffB,h_layereffBN)
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff_phi,h_layereff_phiBN)
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff_eta,h_layereff_etaBN)
    makeEfficiency("gr_layereffE","Layer efficiency",h_layereffE,h_layereffEN)
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff_phi,h_layereff_phiEN)
    makeEfficiency("gr_layereff","Layer efficiency",h_layereff_eta,h_layereff_etaEN)
    hfilelayer.Write()
    hfilelayer.Close()
