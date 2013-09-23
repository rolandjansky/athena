# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import array
import string
import random


class HistControl( ROOT.TNamed ):

    def __init__( self, name = "HistControl", title = "Controls the histograms", doOutputFile=False):

        # Initialise the base class:
        ROOT.TNamed.__init__( self, name, title )
        self.SetName( name )
        self.SetTitle( title )

        self.__OneDHists = {}
        self.__TwoDHists = {}
        self.__Graphs    = {}
        self.__TProfiles = {}
        self.__OutputROOTFile = None

        self.__doOutputFile = doOutputFile
            
        return

    def BookTProfile(self, name, title, numbinx, xmin, xmax):
        if name in self.__TProfiles.keys():
            print 'already stored a tprofile object with name', name
            return False
        tprof = ROOT.TProfile(name, title, numbinx, xmin, xmax)
        self.__TProfiles[ name ] = tprof
        return True

    def BookHist1D(self, name, title,  numbinx, xmin, xmax):

        if name in self.__OneDHists.keys():
            print 'already stored a th1f object with name', name
            return False
        hist1d = ROOT.TH1F(name, title,  numbinx, xmin, xmax)
        self.__OneDHists[ name ] = hist1d
        return True
        
        
    def BookHist2D(self, name, title,  numbinx, xmin, xmax, numbiny, ymin, ymax):
        if name in self.__TwoDHists.keys():
            print 'already stored a th2f object with name', name
            return False
        hist2d = ROOT.TH2F(name, title,  numbinx, xmin, xmax, numbiny, ymin, ymax)
        self.__TwoDHists[ name ] = hist2d
        return True
        

    #def BookGraph():

    def GetRandomString(self, N):
        str = ''.join(random.choice(string.ascii_uppercase + string.digits) for x in range(N))
        print 'randomstring', str
        return str

    def SaveTH1FToImage(self, histname, imagename, drawoptions = 'HIST',
                        colors = [ROOT.kRed, ROOT.kBlue, ROOT.kGreen, ROOT.kOrange, ROOT.kCyan, ROOT.kMagenta],
                        linewidth = 2):
        canvas = ROOT.TCanvas('canvas'+self.GetRandomString(6))
        canvas.cd(0)
        if isinstance(histname, list):
            maxy = -999.
            for (index,hname) in enumerate(histname):
                if self.__OneDHists[ hname ].GetMaximum() > maxy:
                    maxy = self.__OneDHists[ hname ].GetMaximum()
                    pass
                pass
            for (index,hname) in enumerate(histname):
                self.__OneDHists[ hname ].SetMaximum(maxy*1.2)
                pass
            for (index,hname) in enumerate(histname):
                self.__OneDHists[ hname ].SetLineWidth(linewidth)
                if index >= len(colors):
                    print 'add more colors'
                    assert false
                    pass
                self.__OneDHists[ hname ].SetLineColor(colors[index])
                if index == 0:
                    self.__OneDHists[ hname ].Draw(drawoptions)
                else:
                    self.__OneDHists[ hname ].Draw(drawoptions+'SAME')
                    pass
                pass
            pass
        else:
            self.__OneDHists[ hname ].SetLineWidth(linewidth)
            self.__OneDHists[ hname ].SetLineColor(colors[0])
            self.__OneDHists[ hname ].Draw(drawoptions)
            pass
        canvas.SaveAs('output/'+imagename+'.png')
        pass

    def SaveTH2FToImage(self, histname, imagename, doth2f2graph=False, minY=1., maxY=-1.):
        canvas = ROOT.TCanvas('canvas'+self.GetRandomString(6))
        canvas.cd(0)
        hist_frame = None
        if (minY<maxY) and doth2f2graph:
            hist_frame = ROOT.TH1F('hist_frame', self.__TwoDHists[ histname ].GetTitle(),
                                   100, self.__TwoDHists[ histname ].GetXaxis().GetXmin(), self.__TwoDHists[ histname ].GetXaxis().GetXmax())
            hist_frame.SetXTitle( self.__TwoDHists[ histname ].GetXaxis().GetTitle() )
            hist_frame.SetYTitle( self.__TwoDHists[ histname ].GetYaxis().GetTitle() )
            hist_frame.SetMinimum(minY)
            hist_frame.SetMaximum(maxY)
        if isinstance(histname, list):
            print 'still need to figure this out'
            pass
        else:
            if doth2f2graph:
                graph_err = self.TwoDHist2Graph(histname, doErros=True)
                graph     = self.TwoDHist2Graph(histname, doErros=False)
                if hist_frame is not None:
                    hist_frame.Draw()
                    graph.SetLineWidth(3)
                    graph_err.Draw('3SAME')
                    #graph.SetMarkerSize(0)
                    graph.Draw('CSAME')
                else:
                    graph.Draw('ALP')
                    pass
            else:
                self.__TwoDHists[ histname ].Draw()
                pass
            pass
        canvas.SaveAs('output/'+imagename+'.png')
        pass


    def getRMSs(self, hist):

        mean = hist.GetMean()
        nentries = hist.GetEntries()
        rmsDown = 0.
        rmsUp   = 0.
        nDown = 0
        nUp   = 0
        print 'mean', mean
        for bin in range(1, hist.GetNbinsX()+1):
            if hist.GetBinCenter(bin) < mean:
                print 'low', hist.GetBinContent(bin)
                nDown = nDown + hist.GetBinContent(bin)
                rmsDown += float(hist.GetBinContent(bin))*(mean - float(hist.GetBinCenter(bin)))**2
            else:
                nUp = nUp + hist.GetBinContent(bin)
                rmsUp   += float(hist.GetBinContent(bin))*(mean - float(hist.GetBinCenter(bin)))**2
                pass
            pass
        if nDown > 0:
            rmsDown = ROOT.TMath.Sqrt(rmsDown/float(nentries))
        else:
            rmsDown = 0.
            pass
        if nUp > 0:
            rmsUp   = ROOT.TMath.Sqrt(rmsUp/float(nentries))
        else:
            rmsUp   = 0.

        return (rmsDown,rmsUp)


    def TwoDHist2Graph(self, name, doErros=False):
        if not name in self.__TwoDHists.keys():
            print 'No such TH2F as', name
            return None

        hist = self.__TwoDHists[ name ]
        
        list_mean   = []
        list_x      = []
        list_xel    = []
        list_yel    = []
        list_xeh    = []
        list_yeh    = []

        for bin in range(1, hist.GetNbinsX()+1 ):
            hist_projY = hist.ProjectionY('pY'+str(bin), bin, bin)
            if hist_projY.GetEntries() != 0:
                mean = hist_projY.GetMean()
                list_mean.append( mean )
                list_x.append( hist.GetXaxis().GetBinCenter(bin) )
                list_xel.append(0.)
                list_xeh.append(0.)
                if doErros:
                    (yel,yeh) = self.getRMSs(hist_projY)
                else:
                    (yel,yeh) = (0., 0.)
                    pass
                list_yel.append(yel)
                list_yeh.append(yeh)
                pass
            hist_projY.Delete()
            pass

        array_mean      = array.array('d', list_mean)
        array_x         = array.array('d', list_x)
        array_xel       = array.array('d', list_xel)
        array_yel       = array.array('d', list_yel)
        array_xeh       = array.array('d', list_xeh)
        array_yeh       = array.array('d', list_yeh)

        if doErros:
            graph_mean    = ROOT.TGraphAsymmErrors(len(list_mean), array_x, array_mean, array_xel,array_xeh,array_yel,array_yeh)
            graph_mean.SetFillColor(ROOT.kRed)
        else:
            graph_mean    = ROOT.TGraph(len(list_mean), array_x, array_mean)
        return graph_mean


    def FillHist1D(self, name, value):
        if name in self.__OneDHists:
            self.__OneDHists[ name ].Fill(value)
        else:
            print 'No such th1f with name', name
            pass
        return

    def FillHist2D(self, name, valuex, valuey):
        if name in self.__TwoDHists:
            self.__TwoDHists[ name ].Fill(valuex, valuey)
        else:
            print 'No such th2f with name', name
            pass
        return

    def FillTProfile(self, name, valuex, valuey):
        if name in self.__TProfiles:
            self.__TProfiles[ name ].Fill(valuex, valuey)
        else:
            print 'No such tprofile with name', name
            pass
        return


    def SaveAllToFile(self):
        if not self.__doOutputFile:
            print 'Cannot save hists to file. No output file set.'
            return

        outputFile = ROOT.TFile(self.GetName()+'.root', 'RECREATE')
        for name1D, hist1D in self.__OneDHists.iteritems():
            if hist1D.GetEntries() != 0:
                hist1D.Write()
        for name2D, hist2D in self.__TwoDHists.iteritems():
            if hist2D.GetEntries() != 0:
                hist2D.Write()
        for nametprof, histtprof in self.__TProfiles.iteritems():
            histtprof.Write()
        #for graph in self.__Graphs:
        #    graph.Write()
        return
