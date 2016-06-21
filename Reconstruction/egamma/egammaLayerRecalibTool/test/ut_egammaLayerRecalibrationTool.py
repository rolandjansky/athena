#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT
import itertools
ROOT.PyConfig.IgnoreCommandLineOptions = True


def arange(xmin, xmax, delta):
    # just to don't inject dep from numpy
    result = []
    x = xmin
    while True:
        result.append(x)
        x += delta
        if x >= xmax:
            break
    return result


def xAOD_electron_generator(tree):
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        for i in xrange(tree.Electrons.size()):
            el = tree.Electrons.at(i)
            yield el


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
        inputs.phi = 0.
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
        self.assertTrue(tool is not None)

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
                               self.example_inputs().E0raw * (1 + 0.01) * (1 + 0.01), places=5)

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

    def _test_scaleps2(self):  # TODO: fix PathResolver https://its.cern.ch/jira/browse/ATLASG-580
        from PathResolver import PathResolver
        tool = self.egammaLayerRecalibTool('ps_2012_v3')
        f = PathResolver("egammaLayerRecalibTool/egammaLayerRecalibTunes.root")
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

    def _test_layer1_2010_v5(self): # TODO: fix path
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

    def _oldtest_multi_modifier(self): # TODO: fix path
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


def is_file_readable(path):
    f = ROOT.TFile.Open(path)
    return bool(f)


@unittest.skipIf(not is_file_readable('root://eosatlas.cern.ch//eos/atlas/user/t/turra/user.blenzi.4956574.EXT0._000001.AOD.pool.root'),
                 "input file not available")
class TestLayerxAOD(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        """ using a filename where the egamma.e() is already computed
        v1 MVA calibration
        """
        filename = 'root://eosatlas.cern.ch//eos/atlas/user/t/turra/user.blenzi.4956574.EXT0._000001.AOD.pool.root'
        if (not ROOT.xAOD.Init().isSuccess()):
            print "Failed xAOD.Init()"

        treeName = "CollectionTree"

        f = ROOT.TFile.Open(filename)
        if not f:
            print "file %s not found" % filename
        cls.tree = ROOT.xAOD.MakeTransientTree(f, treeName)

    @classmethod
    def tearDownClass(cls):
        ROOT.xAOD.ClearTransientTrees()

    def test_decoration(self):
        tune = "2012_alt_with_layer2"
        tool = ROOT.egammaLayerRecalibTool(tune)

        N = 200
        i = 0
        for el in itertools.islice(xAOD_electron_generator(self.tree), None, N):
            event_info = self.tree.EventInfo

            self.assertTrue(tool.applyCorrection(el, event_info).code() == ROOT.CP.CorrectionCode.Ok)

            inputs = ROOT.StdCalibrationInputs()
            inputs.RunNumber = event_info.runNumber()
            inputs.E0raw = el.caloCluster().energyBE(0)
            inputs.E1raw = el.caloCluster().energyBE(1)
            inputs.E2raw = el.caloCluster().energyBE(2)
            inputs.E3raw = el.caloCluster().energyBE(3)
            inputs.phi = el.caloCluster().phi()
            inputs.eta = el.caloCluster().eta()

            tool.scale_inputs(inputs)
            self.assertAlmostEqual(inputs.E0raw, el.caloCluster().auxdataConst("double")("correctedcl_Es0"))
            self.assertAlmostEqual(inputs.E1raw, el.caloCluster().auxdataConst("double")("correctedcl_Es1"))
            self.assertAlmostEqual(inputs.E2raw, el.caloCluster().auxdataConst("double")("correctedcl_Es2"))
            self.assertAlmostEqual(inputs.E3raw, el.caloCluster().auxdataConst("double")("correctedcl_Es3"))
#            self.assertTrue(el.caloCluster().auxdataConst("string")("layer_correction") == tune) # TODO: check why not working in 2.4
            i += 1

        self.assertGreater(i, 10, msg="too few electrons")

    # TODO: presently not useful, since all electrons are inside the calibrated region
    def test_range(self):
        tune = "2012_alt_with_layer2"
        tool = ROOT.egammaLayerRecalibTool(tune)

        N = 200
        for el in itertools.islice(xAOD_electron_generator(self.tree), None, N):
            event_info = self.tree.EventInfo

            code = tool.applyCorrection(el, event_info).code()
            if abs(el.eta() < 2.5):
                self.assertTrue(code == ROOT.CP.CorrectionCode.Ok)


if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    ROOT.gStyle.SetCanvasDefH(800)
    ROOT.gStyle.SetCanvasDefW(800)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)

    unittest.main()
