#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
#
# Author: Margherita Spalla (margherita.spalla@cern.ch)
# Function for post-processing of histograms from CaloMonitoring
#
# The functions in this file are being implemented through the yaml config file inside the DataQuality package:
#   DataQuality/DataQualityUtils/data/postprocessing/CaloMonPostProc.yaml
#

#from ROOT import TMath
import cppyy


def divideHistCaloMon(inputs,doPercentage=False):
    """ This function returns the ratio of two ROOT histograms """
    assert len(inputs) == 1  
    assert len(inputs[0][1]) == 3

    cl = inputs[0][1][0].Clone()
    cl.Divide(inputs[0][1][1])

    label = inputs[0][0]['thr']

    hnorm = inputs[0][1][2].Clone()
    n0 = hnorm.GetNbinsX()
    fbin = hnorm.GetXaxis().FindBin(label)

    if fbin>=n0 or fbin<=0:
        return [cl]
        
    thenorm = hnorm.GetBinContent(fbin)
    cl.SetEntries(thenorm)
    
    if not doPercentage or thenorm==0:
        return [cl]

    cl.Scale(100.0/thenorm)
    
    return [cl]


cppyy.cppdef("""
void binbybinDiv(size_t nCells,TH2F* fraction, const TH2F* total, const TH2* occupancy) {
for (size_t i=0;i<nCells;++i) {
    const double t = total->GetBinContent(i);
    const double o = occupancy->GetBinContent(i);
    if (o>0) {
      fraction->SetBinContent(i,t/o);
      fraction->SetBinError(i,std::sqrt(1./o));
    }
  }
}
""")


def divideByOccupancy(inputs,titleToReplace="",replaceTitWith=""):
    """ This function returns the ratio of two ROOT histograms """
    assert len(inputs) == 1  
    assert len(inputs[0][1]) == 2

    cl = inputs[0][1][0].Clone()
    cl.Reset()

    if titleToReplace!="":
        tit = cl.GetTitle()
        tit = tit.replace(titleToReplace,replaceTitWith)
        cl.SetTitle(tit)

    nCells = cl.GetNcells()
    assert(nCells==inputs[0][1][1].GetNcells())

    cppyy.gbl.binbybinDiv(nCells,cl,inputs[0][1][0],inputs[0][1][1])
 #   for i in range(nCells):
  #      t = inputs[0][1][0].GetBinContent(i);
   #     o = inputs[0][1][1].GetBinContent(i);
    #    if o>0:
     #       cl.SetBinContent(i,t/o);
      #      cl.SetBinError(i,TMath.Sqrt(1./o));
       #     pass
        #pass

    cl.SetEntries(inputs[0][1][0].GetEntries())

    return [cl]


def divideByAcceptedEvts(inputs):
    """ This function divides the input histogram by the number of events that passed a given selection"""
    assert len(inputs) == 1  
    assert len(inputs[0][1]) == 2

    cl = inputs[0][1][0].Clone()

    label = inputs[0][0]['thr']

    hnorm = inputs[0][1][1].Clone()
    n0 = hnorm.GetNbinsX()
    fbin = hnorm.GetXaxis().FindBin(label)

    if fbin>=n0 or fbin<=0: #threshold not found
        return [cl]
        
    thenorm = hnorm.GetBinContent(fbin)
    
    if thenorm==0:
        return [cl]
    cl.Scale(100.0/thenorm)
    
    return [cl]
