# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


class LayerPlotter:
    def __init__(self):
        self.egammaLayerRecalibTool = ROOT.egammaLayerRecalibTool
        self.output_file = ROOT.TFile("test_output.root", "recreate")

    def iscrack(self, eta):
        aeta = abs(eta)
        return 1.37 <= aeta <= 1.55

    def example_inputs(self):
        inputs = ROOT.StdCalibrationInputs()
        inputs.E0raw = 10.
        inputs.E1raw = 20.
        inputs.E2raw = 30.
        inputs.E3raw = 40.
        inputs.R = 0.
        inputs.phi = 0.
        return inputs

    def plot(self):
        self.plot_tune(self.egammaLayerRecalibTool('2012'), '2012.png')
        self.plot_tune(self.egammaLayerRecalibTool('2011'), '2011.png')
        self.plot_tune(self.egammaLayerRecalibTool('2010'), '2010.png')
        self.plot_tune(self.egammaLayerRecalibTool('2012_with_layer2'), '2012_with_layer2.png')
        self.plot_tune(self.egammaLayerRecalibTool('2011_with_layer2'), '2011_with_layer2.png')
        self.plot_tune(self.egammaLayerRecalibTool('2010_with_layer2'), '2010_with_layer2.png')
        self.plot_tune(self.egammaLayerRecalibTool('2012_alt'), '2012_alt.png')
        self.plot_tune(self.egammaLayerRecalibTool('2011_alt'), '2011_alt.png')
        self.plot_tune(self.egammaLayerRecalibTool('2012_alt_with_layer2'), '2012_alt_with_layer2.png')
        self.plot_tune(self.egammaLayerRecalibTool('2011_alt_with_layer2'), '2011_alt_with_layer2.png')

        self.plot_tune(self.egammaLayerRecalibTool('layer1_2012_v5'), 'layer1_2012_v5.png')
        self.plot_tune(self.egammaLayerRecalibTool('ps_2012_v3'), 'ps_2012_v3.png')
        self.plot_tune(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1.png')
        self.plot_tune(self.egammaLayerRecalibTool('layer2_2012_v5'), 'layer2_2012_v5.png')
        self.plot_tune(self.egammaLayerRecalibTool('layer2_2011_v5'), 'layer2_2011_v5.png')
        self.plot_tune(self.egammaLayerRecalibTool('layer2_2010_v5'), 'layer2_2010_v5.png')

    def plot2d(self):
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), "ps_HV1_2d_period0.png", run_number = 204931)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), "ps_HV1_2d_period1.png", run_number = 204932)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), "ps_HV1_2d_period2.png", run_number = 205112)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), "ps_HV1_2d_period3.png", run_number = 211670)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), "ps_HV1_2d_period4.png", run_number = 212619)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), "ps_HV1_2d_period5.png", run_number = 212809)

        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d_period0.png", run_number = 204931)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d_period1.png", run_number = 204932)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d_period2.png", run_number = 205112)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d_period3.png", run_number = 211670)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d_period4.png", run_number = 212619)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d_period5.png", run_number = 212809)

        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), "2012_2d.png")
        self.plot_tune2d(self.egammaLayerRecalibTool('layer1_1'), "layer1_1_2d.png")
        self.plot_tune2d(self.egammaLayerRecalibTool('layer1_2'), "layer1_2_2d.png")

    def plot_compare(self):
        self.plot_tune((self.egammaLayerRecalibTool('2012'),
                        self.egammaLayerRecalibTool('2012_up'),
                        self.egammaLayerRecalibTool('2012_down')),
                       "2012_all_up_down.png",
                       ("central", "up", "down"))

        self.plot_tune((self.egammaLayerRecalibTool('2012'),
                        self.egammaLayerRecalibTool('2012_layer1_up'),
                        self.egammaLayerRecalibTool('2012_layer1_down')),
                       "2012_layer1_up_down.png",
                       ("central", "up", "down"))

        self.plot_tune((self.egammaLayerRecalibTool('2012'),
                        self.egammaLayerRecalibTool('2012_ps_up'),
                        self.egammaLayerRecalibTool('2012_ps_down')),
                       "2012_ps_up_down.png",
                       ("central", "up", "down"))

        self.plot_tune((self.egammaLayerRecalibTool('2012_errup'),
                        self.egammaLayerRecalibTool('2012_errdown')),
                       "2012_all_errup_down.png",
                       ("errorup", "errordown"))

        self.plot_tune((self.egammaLayerRecalibTool('layer2_2012_v5_errup'),
                        self.egammaLayerRecalibTool('layer2_2012_v5_errdown')),
                       "2012_layer2_errup_down.png",
                       ("errorup", "errordown"))

    def plot_tune(self, tools, canvas_name, names=None):
        canvas = ROOT.TCanvas(canvas_name, canvas_name)
        canvas.Divide(2, 2)
        canvas.mem = []

        legend = None
        if names:
            legend = ROOT.TLegend(0.2, 0.5, 0.4, 0.8)

        if type(tools) is not tuple:
            tools = (tools, )
        legend_names = names or [""] * len(tools)

        for itool, (tool, name) in enumerate(zip(tools, legend_names)):

            h0 = ROOT.TGraph()
            h1 = ROOT.TGraph()
            h2 = ROOT.TGraph()
            h3 = ROOT.TGraph()

            for i in xrange(100):
                inputs = self.example_inputs()
                inputs.RunNumber = 0
                inputs.eta = i * 0.05 - 2.5
                inputs.E0raw = 1
                inputs.E1raw = 1
                inputs.E2raw = 1
                inputs.E3raw = 1

                tool.scale_inputs(inputs)

                h0.SetPoint(i, inputs.eta, inputs.E0raw)
                h1.SetPoint(i, inputs.eta, inputs.E1raw)
                h2.SetPoint(i, inputs.eta, inputs.E2raw)
                h3.SetPoint(i, inputs.eta, inputs.E3raw)

            grs = (h0, h1, h2, h3)
            for ig, g in enumerate(grs):
                canvas.cd(ig + 1)
                g.SetTitle(name)
                g.SetLineColor(itool + 1)
                g.SetMarkerColor(itool + 1)
                g.GetXaxis().SetTitle("#eta")
                g.GetYaxis().SetTitle("(E_{%d})-corrected / E_{%d}-non-corrected" % (ig, ig))
                g.SetFillStyle(0)
                g.GetYaxis().SetTitleOffset(1.5)
                g.Draw("APL" if not itool else "PLsame")
                if ig == 3 and legend:
                    legend.AddEntry(g)
                canvas.mem.append(g)
        if legend:
            canvas.cd(3)
            legend.Draw()
        canvas.SaveAs(canvas_name)
        self.output_file.cd()
        canvas.Write()

    def plot_tune2d(self, tool, canvas_name, run_number=0):
        canvas = ROOT.TCanvas()
        canvas.Divide(2, 2)
        nbins_eta = 100
        nbins_phi = 100
        h0 = ROOT.TH2F("h0", "h0", nbins_eta, -2.5, 2.5, nbins_phi, -3.1415, 3.1415)
        h1 = ROOT.TH2F("h1", "h1", nbins_eta, -2.5, 2.5, nbins_phi, -3.1415, 3.1415)
        h2 = ROOT.TH2F("h2", "h2", nbins_eta, -2.5, 2.5, nbins_phi, -3.1415, 3.1415)
        h3 = ROOT.TH2F("h3", "h3", nbins_eta, -2.5, 2.5, nbins_phi, -3.1415, 3.1415)

        for iphi in xrange(nbins_phi):
            inputs = self.example_inputs()
            inputs.RunNumber = run_number
            for ieta in xrange(nbins_eta):
                inputs.eta = h0.GetXaxis().GetBinCenter(ieta)
                inputs.phi = h0.GetYaxis().GetBinCenter(iphi)
                inputs.E0raw = 1.
                inputs.E1raw = 1.
                inputs.E2raw = 1.
                inputs.E3raw = 1.

                tool.scale_inputs(inputs)

                h0.SetBinContent(ieta, iphi, inputs.E0raw)
                h1.SetBinContent(ieta, iphi, inputs.E1raw)
                h2.SetBinContent(ieta, iphi, inputs.E2raw)
                h3.SetBinContent(ieta, iphi, inputs.E3raw)

        grs = (h0, h1, h2, h3)

        for i, g in enumerate(grs):
            canvas.cd(i + 1)
            g.GetXaxis().SetTitle("#eta")
            g.GetYaxis().SetTitle("#phi")
            g.SetTitle("(E_{%d})-corrected / E_{%d}-non-corrected" % (i, i))
            g.SetContour(999)
            g.GetZaxis().SetRangeUser(0.93,
                                      1.07)
#            g.GetZaxis().SetRangeUser(0.97, 1.03)
#            g.GetZaxis().SetRangeUser(0.985, 1.0105) # mimic Guillaume ranges
            g.SetStats(0)
            g.Draw("colz")

        canvas.SaveAs(canvas_name)

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    ROOT.gStyle.SetCanvasDefH(800)
    ROOT.gStyle.SetCanvasDefW(800)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    plotter = LayerPlotter()
    plotter.plot()
    plotter.plot2d()
    plotter.plot_compare()
