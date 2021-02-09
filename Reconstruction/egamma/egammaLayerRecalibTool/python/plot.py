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
        tunes = (
            # high-level tunes used in the calibration tool (usually composition of low-level tunes)
            '2011_alt_with_layer2', '2012_alt_with_layer2', 'es2017_20.7_improved', 'es2017_20.7_final',
            # low-level tunes used in the calibration tool
            'layer2_alt_2011_v5', "ps_2011",                # used in 2011_alt_with_layer2
            "ps_HV1", "layer2_alt_2012_v5", "ps_2012",      # used in 2012_alt_with_layer2
            "pileup_20.7", "run2_alt_with_layer2_modif",    # used in es2017_20.7_final
            "2012_alt_with_layer2_modif",                   # used in es2017_20.7_improved
            "ps_EMECHV1", "layer2_alt_run2_v1", "ps_2016",  # used by run2_alt_with_layer2_modif
            )
        for tune in tunes:
            self.plot_tune(self.egammaLayerRecalibTool(tune), '%s.png' % tune)

    def plot2d(self):
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1_2d_period0.png', run_number = 204931)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1_2d_period1.png', run_number = 204932)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1_2d_period2.png', run_number = 205112)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1_2d_period3.png', run_number = 211670)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1_2d_period4.png', run_number = 212619)
        self.plot_tune2d(self.egammaLayerRecalibTool('ps_HV1'), 'ps_HV1_2d_period5.png', run_number = 212809)

        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d_period0.png', run_number = 204931)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d_period1.png', run_number = 204932)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d_period2.png', run_number = 205112)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d_period3.png', run_number = 211670)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d_period4.png', run_number = 212619)
        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d_period5.png', run_number = 212809)

        self.plot_tune2d(self.egammaLayerRecalibTool('2012'), '2012_2d.png')
        self.plot_tune2d(self.egammaLayerRecalibTool('layer1_1'), 'layer1_1_2d.png')
        self.plot_tune2d(self.egammaLayerRecalibTool('layer1_2'), 'layer1_2_2d.png')

        self.plot_tune2d(self.egammaLayerRecalibTool('2012_alt_with_layer2'), '2012_alt_with_layer2_2d.png')
        self.plot_tune2d(self.egammaLayerRecalibTool('es2017_20.7_improved'), 'es2017_20.7_improved_2d.png')

    def plot_compare(self):
        self.plot_tune((self.egammaLayerRecalibTool('es2017_20.7_final'),
                        self.egammaLayerRecalibTool('es2017_20.7_improved')),
                        'es2017_20.7_final__vs__es2017_20.7_improved.png',
                        ('es2017_20.7_final', 'es2017_20.7_improved'))

    def plot_tune(self, tools, canvas_name, names=None):
        canvas = ROOT.TCanvas(canvas_name, canvas_name)
        canvas.Divide(2, 2)
        canvas.mem = []

        legend = None
        if names:
            legend = ROOT.TLegend(0.2, 0.6, 0.8, 0.8)

        if type(tools) is not tuple:
            tools = (tools, )
        legend_names = names or [""] * len(tools)

        for itool, (tool, name) in enumerate(zip(tools, legend_names)):

            h0 = ROOT.TGraph()
            h1 = ROOT.TGraph()
            h2 = ROOT.TGraph()
            h3 = ROOT.TGraph()

            for i in range(100):
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
                g.SetTitle('layer %d' % ig)
                g.SetLineColor(itool + 1)
                g.SetMarkerColor(itool + 1)
                g.GetXaxis().SetTitle("#eta")
                g.GetYaxis().SetTitle("(E_{%d})-corrected / (E_{%d})-non-corrected" % (ig, ig))
                g.SetFillStyle(0)
                g.GetYaxis().SetTitleOffset(1.5)
                g.Draw("APL" if not itool else "PLsame")
                g.GetYaxis().SetRangeUser(0.92, 1.08)
                if ig == 3 and legend:
                    legend.AddEntry(g, name)
                canvas.mem.append(g)
        if legend:
            canvas.cd(4)
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

        for iphi in range(nbins_phi):
            inputs = self.example_inputs()
            inputs.RunNumber = run_number
            for ieta in range(nbins_eta):
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
