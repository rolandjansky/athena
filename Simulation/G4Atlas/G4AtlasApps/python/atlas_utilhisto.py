# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
This Python module provides access to the HistogramDataSvc from the
Athena Python prompt. The 1D histograms can be book from Python and
filled on fly in the Athena Python prompt or can be imported from a
ROOT file .
"""

# TODO: Remove? Totally unused, I think.  (AB, 07/2011)

__author__ = 'M. Gallas'

class HistoAtHistoSvc(object):
    """ Histogram at the HistogramDataSvc

        - As example the histograms to be used by the single particle
        generator in any of the possible 'orders' of the
        ParticleGenerator.

        - The histogram can be retrieved from a ROOT file using the
        method 'retrieve_historoot' and afterwards dumped to the
        HistogramDataSvc using the method 'dump_histo' or it can be
        booked (use method 'book_histo') and created on the fly.

        - The retrieved ROOT histogram and the one that is created in
        the HistogramDataSvc can be ploted within Athena using PyROOT.

        - The defaults values are set to have the histograms whithin
        the HistogramDataSvc under the path '/stat/particlegenerator/'
        but this is a parameter that can be changed.

        NOTE: only 1D histograms are allowed.
    """
    def __init__(self,name,dim='1D',path='/stat/particlegenerator/'):
        """ Defines a histogram at the HistogramDataSvc with the
            name and path here defined.

            The histogram can be booked or imported from a ROOT file
        """
        self.Name=name
        self.PathInHistoSvc=path
        self.Dimension=dim

    def book_histo(self,theApp,nbins,xmin,xmax):
        """ Books a 1D histogram at the HistogramDataSvc.

           You will need to fill the histogram later. If you
           want to import a TH1F from a root file you do not
           need to use this method.
        """
        HistoSvc=theApp.histsvc('HistogramDataSvc')
        HistoSvc.book(self.PathInHistoSvc+self.Name,
                      self.Name,
                      nbins,xmin,xmax)
        self.HistoSvcEntry=HistoSvc.retrieve(self.PathInHistoSvc+
                                              self.Name)

    def retrieve_historoot(self,root_file_name,root_histo_name):
        """ Retrieves a TH1F ROOT histogram.
        """
        from ROOT import TFile
        self.RootHistoName=root_histo_name
        self.RootFileName=(root_file_name)
        self.RootFile=TFile(self.RootFileName)
        self.HistoRoot=self.RootFile.Get(self.RootHistoName)

    def dump_histo(self,theApp):
        """ Dumps the information of the TH1F ROOT file in a 1D
            at the HistogramDataSvc
        """
        HistoSvc=theApp.histsvc('HistogramDataSvc')
        HistoSvc.book(self.PathInHistoSvc+self.Name,
                    self.Name,
                    self.HistoRoot.GetNbinsX(),
                    self.HistoRoot.GetBinLowEdge(1),
                    self.HistoRoot.GetBinLowEdge(self.HistoRoot.GetNbinsX())+\
                    self.HistoRoot.GetBinWidth(self.HistoRoot.GetNbinsX()))
        self.HistoSvcEntry=HistoSvc.retrieve(self.PathInHistoSvc+
                                              self.Name)
        if self.HistoRoot.GetNbinsX()==len(self.HistoSvcEntry.contents()):
            print (len(self.HistoSvcEntry.contents()))
            print (self.HistoRoot.GetNbinsX())
        print (HistoSvc.dump())
        # ROOT starts at bin 1 and AIDA at bin 0
        for i in range(len(self.HistoSvcEntry.contents())):
            self.HistoSvcEntry.fill(self.HistoRoot.GetBinCenter(i+1),\
                                    self.HistoRoot.GetBinContent(i+1) )

    def plot_historoot(self):
        """ Plots the imported ROOT histogram.
        """
        from ROOT import gROOT, TCanvas
        gROOT.Reset()
        c1 = TCanvas('c1','Imported ROOT histogram',200,10,700,500)
        self.HistoRoot.Draw()
        c1.SetGrid()
        c1.Update()
        return c1

    def plot_histosvc(self):
        """ Plots the histogram at the HistogramDataSvc as a
            ROOT TGraph.
        """
        from ROOT import gROOT, TCanvas, TGraph
        from array import array
        n=len(self.HistoSvcEntry.contents())
        x, y = array( 'f' ), array( 'f' )
        for i in range(len(self.HistoSvcEntry.contents())) :
                x.append(self.HistoSvcEntry.binMean(i+1))
                y.append(self.HistoSvcEntry.binHeight(i+1))
        self.HistoSvc2Root = TGraph( n, x, y )
        gROOT.Reset()
        c2 = TCanvas('c2','Histogram at HistogramDataSvc',200,10,700,500)
        self.HistoSvc2Root.SetTitle('Histo at HistogramDataSvc as TGraph')
        self.HistoSvc2Root.GetXaxis().SetTitle( 'x axis' )
        self.HistoSvc2Root.GetYaxis().SetTitle( 'y axis' )
        self.HistoSvc2Root.Draw( 'ACP' )
        c2.SetGrid()
        c2.Update()
        return c2

#   def storeinroot_histosvc(self,name_root_output_file):
#       """
#          Writes the ROOT conversion of the histosvc
#          into a ROOT file
#       """
#       if not(self.HistoSvc2Root):
#           from ROOT import TGraph
#           from array import array
#           n=len(self.HistoSvcEntry.contents())
#           x, y = array( 'f' ), array( 'f' )
#           for i in range(len(self.HistoSvcEntry.contents())) :
#               x.append(self.HistoSvcEntry.binMean(i+1))
#               y.append(self.HistoSvcEntry.binHeight(i+1))
#           self.HistoSvc2Root = TGraph( n, x, y )
#       from ROOT import TFile, gDirectory
#       OutPutFile=TFile(name_root_output_file,"NEW")
#       gDirectory.Append(self.HistoSvc2Root)
#       gDirectory.Write()
#       OutPutFile.Close()
#
#=======================================================================
