# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
from copy import copy
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


class TestLayer(unittest.TestCase):
    def setUp(self):
        self.egammaLayerRecalibTool = ROOT.egammaLayerRecalibTool
        self.output_file = ROOT.TFile("test_output.root", "recreate")

    def tearDown(self):
        self.output_file.Close()

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
        inputs.phi = 0.;
        return inputs

    def test_init(self):
        tool = self.egammaLayerRecalibTool('2012')
        tool = self.egammaLayerRecalibTool('2012_alt')
        tool = self.egammaLayerRecalibTool('2011_alt')
        tool = self.egammaLayerRecalibTool('2012_with_layer2')
        tool = self.egammaLayerRecalibTool('2011_with_layer2')
        tool = self.egammaLayerRecalibTool('2012_alt_with_layer2')
        tool = self.egammaLayerRecalibTool('2011_alt_with_layer2')
        tool = self.egammaLayerRecalibTool('2012_up')
        tool = self.egammaLayerRecalibTool('2012_down')
        tool = self.egammaLayerRecalibTool('2012_errup')
        tool = self.egammaLayerRecalibTool('2012_errdown')
        tool = self.egammaLayerRecalibTool('2012_ps_up')
        tool = self.egammaLayerRecalibTool('2012_ps_down')
        tool = self.egammaLayerRecalibTool('2012_ps_errup')
        tool = self.egammaLayerRecalibTool('2012_ps_errdown')
        tool = self.egammaLayerRecalibTool('2012_layer1_up')
        tool = self.egammaLayerRecalibTool('2012_layer1_down')
        tool = self.egammaLayerRecalibTool('2012_layer1_errup')
        tool = self.egammaLayerRecalibTool('2012_layer1_errdown')
        tool = self.egammaLayerRecalibTool('2011')
        tool = self.egammaLayerRecalibTool('2010')


#    def test_wrong_init(self):
#        self.assertRaises(SystemExit, lambda: ROOT.egammaLayerRecalibTool('wrong_tune'))
#        with self.assertRaises(SystemExit) as cm:
#            ROOT.egammaLayerRecalibTool('wrong_tune')

    def test_manual_scale(self):
        return 
        tool = self.egammaLayerRecalibTool('')

        modifier = ROOT.ScaleE0(ROOT.InputModifier.ZEROBASED)
        amounter = ROOT.GetAmountFixed(0.01)
        tool.add_scale(modifier, amounter)

        inputs = self.example_inputs()

        tool.scale_inputs(inputs)
        self.assertAlmostEqual(inputs.E0raw,
                               self.example_inputs().E0raw * (1 + 0.01), places=5)
        self.assertAlmostEqual(inputs.E1raw, self.example_inputs().E1raw)

        modifier2 = ROOT.ScaleE1(ROOT.InputModifier.ZEROBASED)
        histo = ROOT.TH1F("h", "h", 100, -2.5, 2.5)
        histo.Fill(0.03, 0.05)
        amounter2 = ROOT.GetAmountHisto1D(histo)
        tool.add_scale(modifier2, amounter2)

        inputs.eta = -0.03
        tool.scale_inputs(inputs)
        self.assertAlmostEqual(inputs.E1raw,
                               self.example_inputs().E1raw, places=5)
        self.assertAlmostEqual(inputs.E0raw,
                               self.example_inputs().E0raw* (1 + 0.01) * (1 + 0.01), places=5)

        inputs = self.example_inputs()
        inputs.eta = 0.03
        tool.scale_inputs(inputs)
        self.assertAlmostEqual(inputs.E1raw,
                               self.example_inputs().E1raw * (1. + 0.05), places=5)
        self.assertAlmostEqual(inputs.E0raw,
                               self.example_inputs().E0raw * (1. + 0.01), places=5)

    def test_scale(self):
        tool = self.egammaLayerRecalibTool('test1')
        inputs = self.example_inputs()
        inputs.eta = 2.

        tool.scale_inputs(inputs)
        self.assertAlmostEqual(inputs.E1raw,
                               self.example_inputs().E1raw * (1. + 0.01), places=5)
        self.assertAlmostEqual(inputs.E0raw,
                               self.example_inputs().E0raw * (1 + 0.1), places=5)

        inputs = self.example_inputs()
        inputs.eta = -2.
        tool.scale_inputs(inputs)
        self.assertAlmostEqual(inputs.E1raw,
                               self.example_inputs().E1raw * (1. + 0.01), places=5)
        self.assertAlmostEqual(inputs.E0raw,
                               self.example_inputs().E0raw * (1 - 0.1), places=5)


    def test_scaleps2(self):
        tool = self.egammaLayerRecalibTool('ps_2012_v3')
        f = ROOT.TFile("$ROOTCOREDIR/data/egammaLayerRecalibTool/egammaLayerRecalibTunes.root")
        histo_ps_tot_error = f.Get("hPS_2012")

        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5
            bin = histo_ps_tot_error.FindBin(inputs.eta)
            amount = histo_ps_tot_error.GetBinContent(bin)
            tool.scale_inputs(inputs)
            self.assertAlmostEqual(inputs.E0raw,
                                   self.example_inputs().E0raw / amount, places=5)


    def test_null(self):
        tool = self.egammaLayerRecalibTool('')

        modifier1 = ROOT.ScaleE0(ROOT.InputModifier.ZEROBASED)
        modifier2 = ROOT.ScaleE1(ROOT.InputModifier.ONEBASED)
        amounter1 = ROOT.GetAmountFixed(0)
        amounter2 = ROOT.GetAmountFixed(1)

        tool.add_scale(modifier1, amounter1)
        tool.add_scale(modifier2, amounter2)

        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5
            tool.scale_inputs(inputs)
            self.assertAlmostEqual(inputs.E0raw, self.example_inputs().E0raw)
            self.assertAlmostEqual(inputs.E1raw, self.example_inputs().E1raw)


    def test_psHV(self):
        tool = self.egammaLayerRecalibTool('ps_HV1')

        for i in xrange(100):
            eta = i * 0.05 - 2.5
            inputs = self.example_inputs()
            inputs.eta = eta
            inputs.RunNumber = 200803
            tool.scale_inputs(inputs)
            self.assertAlmostEqual(inputs.E1raw, self.example_inputs().E1raw)
            self.assertAlmostEqual(inputs.E2raw, self.example_inputs().E2raw)
            self.assertAlmostEqual(inputs.eta, eta, 4)

        for i in xrange(100):
            eta = i * 0.05 - 2.5
            inputs = self.example_inputs()
            inputs.eta = eta
            inputs.RunNumber = 212810
            tool.scale_inputs(inputs)
            if eta > 1.5:
                self.assertAlmostEqual(inputs.E0raw, self.example_inputs().E0raw)

            
    def test_layer1(self):
        tool = self.egammaLayerRecalibTool('layer1_1')

        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5

            alpha = 0.97 if (abs(inputs.eta) < 1.425) else 1

            tool.scale_inputs(inputs)

            self.assertAlmostEqual(inputs.E0raw, self.example_inputs().E0raw, 5)
            self.assertAlmostEqual(inputs.E1raw, self.example_inputs().E1raw / alpha, 5)


    def test_layer1_2010_v5(self):
        tool = self.egammaLayerRecalibTool('layer1_2010_v5')

        f = ROOT.TFile("$ROOTCOREDIR/data/egammaLayerRecalibTool/egammaLayerRecalibTunes.root")
        histo = f.Get("hE1E2ave_2010")
        etas = [histo.GetBinCenter(i) for i in range(histo.GetNbinsX())]

        for eta in etas:
            inputs = self.example_inputs()
            inputs.eta = eta
            bin = histo.FindFixBin(eta)
            amount = histo.GetBinContent(bin)

            if amount == 0:
                continue

            tool.scale_inputs(inputs)

            self.assertAlmostEqual(inputs.E0raw, self.example_inputs().E0raw, 5)
            self.assertAlmostEqual(inputs.E1raw, self.example_inputs().E1raw / amount, 5)


    def oldtest_multi_modifier(self):
        tool = self.egammaLayerRecalibTool('layer1_2')
        tool.add_scale('ps_1')
        f = ROOT.TFile("$ROOTCOREDIR/data/egammaLayerRecalibTool/EnergyRescalerData.root")
        histo_ps_tot_error = f.Get("Scales/2011/alphaPSmod_b12Fit_errTot")

        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5

            bin = histo_ps_tot_error.FindBin(inputs.eta)
            amount = histo_ps_tot_error.GetBinContent(bin)

            alpha = 0.97 if (abs(inputs.eta) < 1.425) else 1.05

            tool.scale_inputs(inputs)
            self.assertAlmostEqual(inputs.E0raw, self.example_inputs().E0raw / amount, places=5)
            self.assertAlmostEqual(inputs.E1raw, self.example_inputs().E1raw / alpha, 5)

    def test_values(self):
        tool = self.egammaLayerRecalibTool('layer1_2')
        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5

            original_inputs = copy(inputs)

            k = tool.values(inputs)
            tool.scale_inputs(inputs)
            self.assertAlmostEqual(k.E1raw, inputs.E1raw / original_inputs.E1raw)

    def test_scale_and_values(self):
        tool = self.egammaLayerRecalibTool('layer1_2')
        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5

            original_inputs = copy(inputs)

            k = tool.scale_and_values(inputs)
            self.assertAlmostEqual(k.E1raw, inputs.E1raw / original_inputs.E1raw)          


    def test_plot(self):
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


    def test_plot_error(self):
        self.plot_tune_error(self.egammaLayerRecalibTool('2012'), "error_2012.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2011'), "error_2011.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2010'), "error_2010.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2012_with_layer2'), "error_2012_with_layer2.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2011_with_layer2'), "error_2011_with_layer2.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2012_alt'), "error_2012_alt.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2011_alt'), "error_2011_alt.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2012_alt_with_layer2'), "error_alt_2012_with_layer2.png")
        self.plot_tune_error(self.egammaLayerRecalibTool('2011_alt_with_layer2'), "error_alt_2011_with_layer2.png")

        

    def test_plot2d(self):
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

        

    def test_compare(self):
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

    def test_overflow(self):
        tool = self.egammaLayerRecalibTool('')
        modifier = ROOT.ScaleE0(ROOT.InputModifier.ONEBASED)
        histo = ROOT.TH1F("h", "h", 100, -1, 1)
        for i in range(100): histo.SetBinContent(i + 1, 2)
        amounter = ROOT.GetAmountHisto1D(histo)
        tool.add_scale(modifier, amounter)
        
        for i in xrange(100):
            inputs = self.example_inputs()
            inputs.eta = i * 0.05 - 2.5
            tool.scale_inputs(inputs)
            if (inputs.eta >= -1 and inputs.eta < 1):
                self.assertEqual(inputs.E0raw, self.example_inputs().E0raw * 2.)
            else:
                self.assertEqual(inputs.E0raw, self.example_inputs().E0raw)

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


    def plot_tune_error(self, tools, canvas_name, names=None):
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

                errors = tool.rel_error(inputs)

                h0.SetPoint(i, inputs.eta, errors.E0raw)
                h1.SetPoint(i, inputs.eta, errors.E1raw)
                h2.SetPoint(i, inputs.eta, errors.E2raw)
                h3.SetPoint(i, inputs.eta, errors.E3raw)

            grs = (h0, h1, h2, h3)
            for ig, g in enumerate(grs):
                canvas.cd(ig + 1)
                g.SetTitle(name)
                g.SetLineColor(itool + 1)
                g.SetMarkerColor(itool + 1)
                g.GetXaxis().SetTitle("#eta")
                g.GetYaxis().SetTitle("rel error on energy")
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

    def testMaartenNumbers(self):
        tool = self.egammaLayerRecalibTool('')
        modifier = ROOT.ScaleE0(ROOT.InputModifier.ONEBASED_ALPHA) # correction = 1 / alpha_ps = k
        amounter = ROOT.GetAmountFixed(1.05, 0.1)  # alpha_ps = 1.05, alpha_ps_abs_err = 0.1
        # corrected-energy = non-corrected-energy * 1 / alpha_ps
        tool.add_scale(modifier, amounter)

        inputs = self.example_inputs()
        k = tool.values(inputs)         # multiplicative corrections: corrected-energy = k * non-corrected-energy
        err = tool.rel_error(inputs)    # error on multiplicative corrections
        self.assertAlmostEqual(k.E0raw, 1 / 1.05, 6)       # k is equal to 1 / 1.05 up to 6 digits
        self.assertAlmostEqual(err.E0raw, 0.1 / 1.05**2)   # error on k is: alpha_ps_abs_err / alpha_ps ** 2

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    ROOT.gStyle.SetCanvasDefH(800)
    ROOT.gStyle.SetCanvasDefW(800)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    unittest.main()
