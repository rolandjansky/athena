# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT

import logger
import AtlasStyle
from analysis import makeDirectory

ROOT.gROOT.SetBatch(True)


class Proc:
    def __init__(self, filename, colour, caption, isMC=True):
        self.name = filename
        self.colour = colour
        self.caption = caption
        self.isMC = isMC


class Plotter:
    def __init__(self, htmlOutputName, stack, mcSF):
        self.out = open(htmlOutputName, 'w')
        self.out.write('<html>\n')
        self.out.write('<title>Plots plots plots</title>\n')

        self.imgDirectory = 'output/'
        self.c1 = ROOT.TCanvas('c1', '', 400, 400)
        self.files = {}
        self.stack = stack

        self.showLegend = True
        self.mcSF = mcSF

        makeDirectory(self.imgDirectory)

        print(logger.OKBLUE + 'Stack configuration' + logger.ENDC)
        for currentProcess in stack:
            print('  Name {0} Colour {1} Caption {2}'.format(
                currentProcess.name,
                currentProcess.colour,
                currentProcess.caption))

    def doneAndDusted(self):
        self.out.write('</html>\n')
        self.out.close()

    def getPlot(self, fileName, histName):
        if fileName not in self.files:
            f = ROOT.TFile.Open(fileName)
            if not f:
                print(logger.FAIL + 'Failed to open ' + fileName + logger.ENDC)
                raise Exception('Failed to open ' + fileName)

            self.files[fileName] = f

        return self.files[fileName].Get(histName)

    def thumbnail(self, plot, width, height):
        img = ROOT.TASImage(plot)
        img.Scale(width, height)
        img.WriteImage(plot.replace('.gif', '_tn.gif'))

    #Legend of plot, not Zelda, sorry
    def genLegend(self, entries):
        legend = ROOT.TLegend(0.56, 0.67, 0.8, 0.93)
        legend.SetFillColor(0)
        legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        legend.SetTextFont(72)
        legend.SetTextSize(0.04)

        for l in reversed(entries):
            legend.AddEntry(l[0], l[1], l[2])

        return legend

    def plot(self, histName):
        print('plotting ', histName)
        self.c1.Clear()

        entries = []
        ths = ROOT.THStack()
        blackoutline = None

        nonStackHistograms = []

        counterForMCSamples = 0
        for c, currentProcess in enumerate(reversed(self.stack)):
            h = self.getPlot(currentProcess.name, histName)

            #if it's MC scale to the integrated luminosity.  Assume it's already
            #scaled to 1 fb-1
            style = 'f'
            if currentProcess.isMC:
                h.SetFillColor(currentProcess.colour)
                h.SetLineColor(currentProcess.colour)
                counterForMCSamples += 1

                h.Scale(self.mcSF)
                ths.Add(h)

                if blackoutline is None:
                    blackoutline = h.Clone()
                else:
                    blackoutline.Add(h)
            else:
                nonStackHistograms.append(h)
                style = 'p'

            entries.append([h, currentProcess.caption, style])

        #calculate max y
        ymax = 0
        if blackoutline:
            ymax = blackoutline.GetBinContent(blackoutline.GetMaximumBin())

        for h in nonStackHistograms:
            current = h.GetBinContent(h.GetMaximumBin())
            if current > ymax:
                ymax = current

        if counterForMCSamples > 0:
            ths.SetMinimum(0.)
            ths.SetMaximum(1.2 * ymax)
            ths.Draw('hist')

            ths.GetHistogram().SetXTitle(h.GetXaxis().GetTitle())
            ths.GetHistogram().SetYTitle(h.GetYaxis().GetTitle())
            ths.GetHistogram().GetYaxis().SetTitleOffset(1.6)

            #outline around all plots in the stack
            blackoutline.SetLineColor(ROOT.kBlack)
            blackoutline.SetFillStyle(0)
            blackoutline.Draw('hist same')

        options = ''
        if counterForMCSamples > 0:
            options += ' same'

        for h in nonStackHistograms:
            h.Draw(options)

        if self.showLegend:
            legend = self.genLegend(entries)
            legend.Draw("same")

        showIntLumi = True
        if showIntLumi:
            s = "#int Ldt = " + str(round(self.mcSF, 1)) + " fb^{-1}"
            lmode = ROOT.TLatex()
            lmode.SetNDC()
            lmode.SetTextColor(1)
            lmode.SetTextSize(0.04)
            lmode.DrawLatex(0.2, 0.84, s)

        showCaption = True
        if showCaption:
            lmode = ROOT.TLatex()
            lmode.SetTextAlign(31); #right, obviously
            lmode.SetNDC()
            lmode.SetTextColor(1)
            lmode.SetTextFont(72)
            lmode.SetTextSize(0.05)

            x = 0.95
            y = 0.955
            lmode.DrawLatex(x, y, histName.split('/')[0])

        #save a gif (and a little thumbnail)
        #and an eps version of the plot
        imgname = histName.replace('/', '.') + '.gif'
        self.c1.Print(self.imgDirectory + imgname)
        self.c1.Print(self.imgDirectory + imgname.replace('.gif', '.eps'))
        self.thumbnail(self.imgDirectory + imgname, 200, 200)
        self.out.write('<a href="{0}"><img src="{1}"></a>\n'.format(
            self.imgDirectory + imgname,
            self.imgDirectory + imgname.replace('.gif', '_tn.gif')))

    def br(self):
        self.out.write('<br>\n')

    def h1(self, txt):
        self.out.write('<h1>{0}</h1>\n'.format(txt))

    def h2(self, txt):
        self.out.write('<h2>{0}</h2>\n'.format(txt))

    def h3(self, txt):
        self.out.write('<h3>{0}</h3>\n'.format(txt))
