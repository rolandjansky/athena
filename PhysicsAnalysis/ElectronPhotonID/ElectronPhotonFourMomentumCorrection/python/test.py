#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import math
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


def arange(xmin, xmax, delta):
    # just to don't inject dep from numpy
    x = xmin
    while True:
        yield x
        x += delta
        if x >= xmax:
            break


def linspace(start, stop, n):
    step = (stop - start) / float(n - 1)
    for i in range(n):
        yield start + step * i


from PATCore.ParticleType import Electron, Photon, ConvertedPhoton, UnconvertedPhoton
from PATCore.ParticleDataType import Data, Full


class Test(unittest.TestCase):
    def setUp(self):
        self.all_systematics = [ROOT.egEnergyCorr.Scale.None, ROOT.egEnergyCorr.Scale.Nominal,
                                ROOT.egEnergyCorr.Scale.PSUp, ROOT.egEnergyCorr.Scale.PSDown,
                                ROOT.egEnergyCorr.Scale.ZeeStatUp, ROOT.egEnergyCorr.Scale.ZeeStatDown,
                                ROOT.egEnergyCorr.Scale.ZeeSystUp, ROOT.egEnergyCorr.Scale.ZeeSystDown,
                                ROOT.egEnergyCorr.Scale.ZeeAllUp, ROOT.egEnergyCorr.Scale.ZeeAllDown,
                                ROOT.egEnergyCorr.Scale.S12Up, ROOT.egEnergyCorr.Scale.S12Down,
                                ROOT.egEnergyCorr.Scale.L1GainDown, ROOT.egEnergyCorr.Scale.L1GainUp,
                                ROOT.egEnergyCorr.Scale.L2GainDown, ROOT.egEnergyCorr.Scale.L2GainUp,
                                ROOT.egEnergyCorr.Scale.LeakageUnconvUp, ROOT.egEnergyCorr.Scale.LeakageUnconvDown,
                                ROOT.egEnergyCorr.Scale.LeakageConvUp, ROOT.egEnergyCorr.Scale.LeakageConvDown,
                                ROOT.egEnergyCorr.Scale.MatIDUp, ROOT.egEnergyCorr.Scale.MatIDDown,
                                ROOT.egEnergyCorr.Scale.MatCryoUp, ROOT.egEnergyCorr.Scale.MatCryoDown,
                                ROOT.egEnergyCorr.Scale.MatCaloUp, ROOT.egEnergyCorr.Scale.MatCaloDown,
                                ROOT.egEnergyCorr.Scale.LArCalibUp, ROOT.egEnergyCorr.Scale.LArCalibDown,
                                ROOT.egEnergyCorr.Scale.LArUnconvCalibUp, ROOT.egEnergyCorr.Scale.LArUnconvCalibDown,
                                ROOT.egEnergyCorr.Scale.LArElecUnconvUp, ROOT.egEnergyCorr.Scale.LArElecUnconvDown,
                                ROOT.egEnergyCorr.Scale.AllUp, ROOT.egEnergyCorr.Scale.AllDown]

        # just a shortcut
        self.EnergyCorrectionTool = ROOT.AtlasRoot.egammaEnergyCorrectionTool
        self.es = [ROOT.egEnergyCorr.es2010,
                   ROOT.egEnergyCorr.es2011c,
                   #              ROOT.egEnergyCorr.es2011d,
                   ROOT.egEnergyCorr.es2012a,
                   ROOT.egEnergyCorr.es2012c]
        self.labels = ["es2010", "es2011c",
                       #  "es2011d",
                       "es2012a", "es2012c"]
        self.tools = {}
        for es, label in zip(self.es, self.labels):
            self.tools[label] = self.EnergyCorrectionTool()
            self.tools[label].setESModel(es)
            self.tools[label].initialize()

        input_file = ROOT.TFile("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/example.root")
        self.electron_tree = input_file.Get("electron")
        self.photon_tree = input_file.Get("photon")
#        self.electron_tree = self.electron_tree.CloneTree()
#        self.electron_tree.SetDirectory(0)
#        self.photon_tree = self.photon_tree.CloneTree()
#        self.photon_tree.SetDirectory(0)
        self.E0MC = input_file.Get("ElectronProfiles/p0MC")
        self.E1MC = input_file.Get("ElectronProfiles/p1MC")
        self.E2MC = input_file.Get("ElectronProfiles/p2MC")
        self.E3MC = input_file.Get("ElectronProfiles/p3MC")
        self.E0MC.SetDirectory(0)
        self.E1MC.SetDirectory(0)
        self.E2MC.SetDirectory(0)
        self.E3MC.SetDirectory(0)

        self.output_file = ROOT.TFile("output_test.root", "recreate")

    def example_input(self, particle=Electron, eta=0.0,
                      std_energy=35E3, pdata=Full, phi=0.):
        return (195848,                         # 0
                pdata,
                particle,
                std_energy * 0.05,  # Es0
                std_energy * 0.1,   # Es1
                std_energy * 0.7,   # Es2       # 5
                std_energy * 0.15,  # Es3
                eta,                # cl_eta
                phi,                # cl_phi
                eta,                # trk_eta
                std_energy,         # cl_E      # 10
                eta,                # cl_etaCalo
                phi,                # cl_phiCalo
                std_energy * 1.1 if particle == Photon else -999,               # ptconv
                std_energy * 0.4 if particle == Photon else -999,               # pt1conv   # 15
                std_energy * 0.7 if particle == Photon else -999,               # pt2conv
                3 if particle == Photon else -999,       # convtrk1nPixHits
                5 if particle == Photon else -999,       # convtrk1nSCTHits
                2 if particle == Photon else -999,       # convtrk2nPixHits
                4 if particle == Photon else -999,                                          # 20
                300 if particle == Photon else -999)     # RConv

    def example_electron(self, eta=0.0, std_energy=35E3, pdata=Full, phi=0.):
        inp = self.example_input(particle=Electron, eta=eta, std_energy=std_energy, pdata=pdata, phi=phi)
        return inp[3:13]

    def example_photon(self, eta=0.0, std_energy=35E3, pdata=Full, phi=0.):
        inp = self.example_input(particle=Photon, eta=eta, std_energy=std_energy, pdata=pdata, phi=phi)
        return inp[3:9] + inp[10:]

    def iscrack(self, eta):
        # ranges used by MVA calibration
        return 1.37 <= abs(eta) <= 1.52

    def test_init(self):
        for es in [ROOT.egEnergyCorr.es2010, ROOT.egEnergyCorr.es2011c,
                   ROOT.egEnergyCorr.es2011d, ROOT.egEnergyCorr.es2012a, ROOT.egEnergyCorr.es2012c]:
            tool = self.EnergyCorrectionTool()
            tool.setESModel(es)
            self.assertTrue(tool)
            try:
                tool.initialize()
            except Exception as exception:
                self.fail("EnergyCorrectionTool.initialize raises exception %s" % str(exception))

    def test_mva_working(self):
        tool_mva = self.EnergyCorrectionTool()
        tool_mva.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_std = self.EnergyCorrectionTool()
        tool_std.setESModel(ROOT.egEnergyCorr.es2010)

        tool_mva.initialize()
        tool_std.initialize()

        for eta in linspace(-2.5, 2.5, 200):
            inputs = self.example_input(eta=eta)
            std_energy_input = inputs[10]

            energy_mva = tool_mva.getCorrectedEnergy(*inputs)
            energy_std = tool_std.getCorrectedEnergy(*inputs)
            self.assertAlmostEqual(std_energy_input, energy_std)
            if (not self.iscrack(eta) and not abs(eta) >= 2.47):
                self.assertNotAlmostEqual(energy_mva, energy_std)
            else:
                self.assertAlmostEqual(energy_mva, energy_std)

    def test_intermodule_correction_working(self):
        tool = self.EnergyCorrectionTool()
        tool.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_no_correction = self.EnergyCorrectionTool()
        tool_no_correction.setESModel(ROOT.egEnergyCorr.es2012c)

        tool.initialize()
        tool_no_correction.initialize()

        tool_no_correction.useIntermoduleCorrection(False)

        energy, energy_no_correction = [], []

        for phi in linspace(-math.pi, math.pi, 400):
            input_phi = self.example_input(phi=phi, eta=0.1, pdata=Data)
            energy.append(tool.getCorrectedEnergy(*input_phi))
            energy_no_correction.append(tool_no_correction.getCorrectedEnergy(*input_phi))

        self.assertFalse(all([x == y for x, y in zip(energy, energy_no_correction)]))

    def test_phicorrection_working(self):
        tool = self.EnergyCorrectionTool()
        tool.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_no_correction = self.EnergyCorrectionTool()
        tool_no_correction.setESModel(ROOT.egEnergyCorr.es2012c)

        tool.initialize()
        tool_no_correction.initialize()

        tool_no_correction.usePhiUniformCorrection(False)

        h = ROOT.TH2F("phi_correction", "phi_correction", 200, -2.5, 2.5, 200, -math.pi, math.pi)

        energy, energy_no_correction = [], []
        for eta in linspace(-3, 3, 200):
            for phi in linspace(-math.pi, math.pi, 200):
                input_phi = self.example_input(phi=phi, pdata=Data, eta=eta)

                e = tool.getCorrectedEnergy(*input_phi)
                e_no_correction = tool_no_correction.getCorrectedEnergy(*input_phi)

                energy.append(e)
                energy_no_correction.append(e_no_correction)
                h.Fill(eta, phi, e / e_no_correction)

        self.assertFalse(all([x == y for x, y in zip(energy, energy_no_correction)]))
        self.assertTrue(all([abs(x / y - 1) < 0.3 for x, y in zip(energy, energy_no_correction)]))

        m = min([x / y for x, y in zip(energy, energy_no_correction)])
        M = max([x / y for x, y in zip(energy, energy_no_correction)])

        canvas_phicorrection = ROOT.TCanvas()
        h.SetMaximum(m)
        h.SetMinimum(M)
        h.Draw("colz")
        h.SetMaximum(m)
        h.SetMinimum(M)
        canvas_phicorrection.Update()
        canvas_phicorrection.Modified()
        canvas_phicorrection.SaveAs("canvas_phicorrection.png")

    def test_interface_equivalence(self):
        tool = self.EnergyCorrectionTool()
        tool.setESModel(ROOT.egEnergyCorr.es2012c)
        tool.initialize()

        for eta in linspace(-2.5, 2.5, 200):
            input_electron = self.example_input(pdata=Data, eta=eta, particle=Electron)
            input_photon = self.example_input(pdata=Data, eta=eta, particle=Photon)

            input_electron2 = self.example_electron(pdata=Data, eta=eta)
            input_photon2 = self.example_photon(pdata=Data, eta=eta)

            electron = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(*input_electron2)
            photon = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(*input_photon2)

            energy_electron2 = tool.getCorrectedEnergyElectron(input_electron[0],
                                                               input_electron[1],
                                                               *input_electron2)
            energy_photon2 = tool.getCorrectedEnergyPhoton(input_electron[0],
                                                           input_electron[1],
                                                           *input_photon2)

            energy_electron1 = tool.getCorrectedEnergy(*input_electron)
            energy_photon1 = tool.getCorrectedEnergy(*input_photon)

            energy_electron3 = tool.getCorrectedEnergy(input_electron[0],
                                                       input_electron[1],
                                                       electron)

            energy_photon3 = tool.getCorrectedEnergy(input_electron[0],
                                                     input_electron[1],
                                                     photon)

            self.assertAlmostEqual(energy_electron1, energy_electron2)
            self.assertAlmostEqual(energy_electron2, energy_electron3)
            self.assertAlmostEqual(energy_photon1, energy_photon2)
            self.assertAlmostEqual(energy_photon2, energy_photon3)

    def test_scale_factors(self):
        graph_MC_nominal_ratio = {}
        graph_data_nominal_ratio = {}
        for label in self.labels:
            graph_MC_nominal_ratio[label] = ROOT.TGraph()
            graph_MC_nominal_ratio[label].SetTitle("MC " + label)
            graph_data_nominal_ratio[label] = ROOT.TGraph()
            graph_data_nominal_ratio[label].SetTitle("data " + label)

            ipoint = 0

            for eta in linspace(-2.5, 2.5, 250):
                input_electron = self.example_input(eta=eta, particle=Electron)
                particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(*(input_electron[3:13]))
                nominal = ROOT.egEnergyCorr.Scale.Nominal
                none = ROOT.egEnergyCorr.Scale.None

                energy_MC_none = self.tools[label].getCorrectedEnergy(0, Full, particle_information, ROOT.egEnergyCorr.Scale.None)
                energy_data_none = self.tools[label].getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)

                energy_MC_nominal = self.tools[label].getCorrectedEnergy(0, Full, particle_information, ROOT.egEnergyCorr.Scale.Nominal)
                energy_data_nominal = self.tools[label].getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.Nominal)

                # TODO: check also forward scales
                if (-2.47 < eta < 2.47):
                    self.assertAlmostEqual(energy_MC_nominal/energy_MC_none, 1.)
                    self.assertNotAlmostEqual(energy_data_nominal/energy_data_none, 1.)
                    graph_MC_nominal_ratio[label].SetPoint(ipoint, eta, energy_MC_nominal/energy_MC_none)
                    graph_data_nominal_ratio[label].SetPoint(ipoint, eta, energy_data_nominal/energy_data_none)
                    ipoint += 1

        canvas_MC = ROOT.TCanvas("canvas_MC_ratio")
        legend = ROOT.TLegend(0.6, 0.6, 0.9, 0.9)
        for i, graph in enumerate(graph_MC_nominal_ratio.itervalues()):
            graph.SetLineColor(i + 1)
            graph.GetYaxis().SetTitle("Nominal-corrected energy / None-corrected energy")
            graph.GetXaxis().SetTitle("#eta")
            graph.SetFillColor(0)
            graph.SetMarkerColor(i + 1)
            graph.SetMarkerStyle(24 + i)
            graph.Draw("APL" if i==0 else "PL")
            legend.AddEntry(graph)
        legend.Draw()
        canvas_MC.SaveAs(canvas_MC.GetTitle() + ".png")

        canvas_data = ROOT.TCanvas("canvas_data_ratio")
        legend2 = ROOT.TLegend(0.6, 0.6, 0.9, 0.9)
        for i, graph in enumerate(graph_data_nominal_ratio.itervalues()):
            graph.SetLineColor(i + 1)
            graph.GetYaxis().SetTitle("Nominal-corrected energy / None-corrected energy")
            graph.GetXaxis().SetTitle("#eta")
            graph.SetFillColor(0)
            graph.SetMarkerColor(i + 1)
            graph.SetMarkerStyle(24 + i)
            graph.Draw("APL" if i==0 else "PL")
            legend2.AddEntry(graph)
        legend2.Draw()
        canvas_data.SaveAs(canvas_data.GetTitle() + ".png")


    def test_data_over_MC(self):
        graph_none_ratio = {}
        for label in self.labels:
            graph_none_ratio[label] = ROOT.TGraph()
            graph_none_ratio[label].SetTitle("data_over_MC_ " + label)

            ipoint = 0

            for eta in linspace(-2.5, 2.5, 250):
                input_electron = self.example_input(eta=eta, particle=Electron)
                particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(*(input_electron[3:13]))

                energy_MC_none = self.tools[label].getCorrectedEnergy(0, Full, particle_information, ROOT.egEnergyCorr.Scale.None)
                energy_data_none = self.tools[label].getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)

                # TODO: check also forward scales
                if (-2.47 < eta < 2.47):
#                    self.assertNotAlmostEqual(energy_MC_none, energy_data_none)
                    graph_none_ratio[label].SetPoint(ipoint, eta, energy_data_none / energy_MC_none)
                    ipoint += 1

        canvas_MC = ROOT.TCanvas("canvas_dataOverMC_ratio")
        legend = ROOT.TLegend(0.6, 0.6, 0.9, 0.9)
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("Data / MC no scale;#eta; data/MC None corrected");
        for i, graph in enumerate(graph_none_ratio.itervalues()):
            graph.SetLineColor(i + 1)
            graph.SetMarkerStyle(24 + i)
            graph.SetFillColor(0)
            graph.GetYaxis().SetTitle("Data None corrected / MC None corrected")
            graph.GetXaxis().SetTitle("#eta")
            multigraph.Add(graph)
            legend.AddEntry(graph)
        multigraph.Draw("APL")
        legend.Draw()
        canvas_MC.SaveAs(canvas_MC.GetTitle() + ".png")

    def test_compare_layer2_recalib(self):
        tool_E2 = self.EnergyCorrectionTool()
        tool_E2.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_E1 = self.EnergyCorrectionTool()
        tool_E1.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_E1.useLayer2Recalibration(False)
        tool_E1.initialize()
        tool_E2.initialize()

        tool_E1.useIntermoduleCorrection(False)
        tool_E1.usePhiUniformCorrection(False)
        tool_E2.useIntermoduleCorrection(False)
        tool_E2.usePhiUniformCorrection(False)

        graph_E1 = ROOT.TGraph()
        graph_E1.SetName("graph_es2012c_data_E1recalib")
        graph_E1.SetTitle("layer 1 recalib")
        graph_E2 = ROOT.TGraph()
        graph_E2.SetName("graph_es2012c_data_E2recalib")
        graph_E2.SetTitle("layer 2 recalib")
        ipoint = 0
        for eta in linspace(-2.5, 2.5, 250):
            E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(eta)) * 1E3
            E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(eta)) * 1E3
            E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(eta)) * 1E3
            E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(eta)) * 1E3

            phi = 0
            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                 eta,
                                                                                                 phi,
                                                                                                 eta,
                                                                                                 40E3,
                                                                                                 eta,
                                                                                                 phi)
            energy_E1_MC = tool_E1.getCorrectedEnergy(0, Full, particle_information, ROOT.egEnergyCorr.Scale.None)
            energy_E1_data = tool_E1.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)
            energy_E2_MC = tool_E2.getCorrectedEnergy(0, Full, particle_information, ROOT.egEnergyCorr.Scale.None)
            energy_E2_data = tool_E2.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)
            graph_E1.SetPoint(ipoint, eta, energy_E1_data / energy_E1_MC)
            graph_E2.SetPoint(ipoint, eta, energy_E2_data / energy_E2_MC)
            ipoint += 1
            self.assertAlmostEqual(energy_E1_MC, energy_E2_MC)
        graph_E1.SetLineColor(ROOT.kRed)
        graph_E2.SetLineColor(ROOT.kBlue)
        graph_E1.SetMarkerColor(ROOT.kRed)
        graph_E2.SetMarkerColor(ROOT.kBlue)
        legend = ROOT.TLegend(0.6, 0.6, 0.9, 0.9)
        for g in (graph_E1, graph_E2):
            g.GetXaxis().SetTitle("#eta")
            g.GetYaxis().SetTitle("data None / MC None")
            g.SetFillColor(0)
            legend.AddEntry(g)
        self.output_file.cd()
        canvas = ROOT.TCanvas()
        canvas.SetName("layer1_vs_layer2")
        graph_E1.Draw("APL")
        graph_E2.Draw("PLsame")
        legend.Draw()
        canvas.SaveAs("layer1_vs_layer2.png")
        canvas.Write()

    def do_test_layer(self, tool, canvas_name, function=None):
        if function is not None:
            function(tool)

        tool.useIntermoduleCorrection(False)
        tool.usePhiUniformCorrection(False)

        canvas = ROOT.TCanvas()
        canvas.SetName("canvas_" + canvas_name)
        canvas.Divide(2, 2)
        gr = []
        for i in range(4):
            gr.append(ROOT.TGraph())
            gr[i].SetTitle(canvas_name + " layer %d" % i)
            gr[i].GetXaxis().SetTitle("#eta")
        ipoint = 0

        for eta in linspace(-3.5, 3.5, 300):
            E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(eta)) * 1E3
            E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(eta)) * 1E3
            E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(eta)) * 1E3
            E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(eta)) * 1E3
            input_electron = self.example_input(eta=eta, particle=Electron)
            phi = 0
            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                 eta,
                                                                                                 phi,
                                                                                                 eta,
                                                                                                 40E3,
                                                                                                 eta,
                                                                                                 phi)
            energy = tool.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)
            r = [1, 1, 1, 1]
            r[0] = 1 if particle_information.rawcl_Es0==0 else tool.getCalibInputs(0) / particle_information.rawcl_Es0
            r[1] = 1 if particle_information.rawcl_Es1==0 else tool.getCalibInputs(1) / particle_information.rawcl_Es1
            r[2] = 1 if particle_information.rawcl_Es2==0 else tool.getCalibInputs(2) / particle_information.rawcl_Es2
            r[3] = 1 if particle_information.rawcl_Es3==0 else tool.getCalibInputs(3) / particle_information.rawcl_Es3
            for i in range(4):
                gr[i].SetPoint(ipoint, eta, r[i])
            ipoint += 1
        for i in range(4):
            canvas.cd(i + 1)
            gr[i].Draw("APL")
        canvas.SaveAs("canvas_" + canvas_name + ".png")
        self.output_file.cd()
        canvas.Write()


    def test_layer(self):
        tool_es2012c = self.EnergyCorrectionTool()
        tool_es2012c.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c.initialize()

        tool_es2011d = self.EnergyCorrectionTool()
        tool_es2011d.setESModel(ROOT.egEnergyCorr.es2011d)
        tool_es2011d.initialize()

        self.do_test_layer(tool_es2012c, "es2012c")
        self.do_test_layer(tool_es2011d, "es2011d")

        tool_es2012c_layer1 = self.EnergyCorrectionTool()
        tool_es2012c_layer1.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c_layer1.useLayer2Recalibration(False)
        tool_es2012c_layer1.initialize()

        tool_es2011d_layer1 = self.EnergyCorrectionTool()
        tool_es2011d_layer1.setESModel(ROOT.egEnergyCorr.es2011d)
        tool_es2011d_layer1.useLayer2Recalibration(False)
        tool_es2011d_layer1.initialize()

        self.do_test_layer(tool_es2012c_layer1, "es2012c_with_layer1")
        self.do_test_layer(tool_es2011d_layer1, "es2011d_with_layer1")

    def test_gain(self):
        tool_es2012c = self.EnergyCorrectionTool()
        tool_es2012c.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c.initialize()
        tool_es2012c.useIntermoduleCorrection(False)
        tool_es2012c.usePhiUniformCorrection(False)

        tool_es2012c_nogain = self.EnergyCorrectionTool()
        tool_es2012c_nogain.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c_nogain.initialize()
        tool_es2012c_nogain.useGainCorrection(False)
        tool_es2012c_nogain.useIntermoduleCorrection(False)
        tool_es2012c_nogain.usePhiUniformCorrection(False)

        graph = ROOT.TGraph()
        ipoint = 0
        for eta in linspace(-3, 3, 300):
            input_electron = self.example_input(eta=eta, particle=Electron)
            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(*(input_electron[3:13]))
            energy = tool_es2012c.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)
            energy_nogain = tool_es2012c_nogain.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)

            graph.SetPoint(ipoint, eta, energy/energy_nogain)
            ipoint += 1

        self.output_file.cd()
        canvas = ROOT.TCanvas()
        canvas.SetName("canvas_gain")
        graph.GetXaxis().SetTitle("#eta")
        graph.GetYaxis().SetTitle("E_{gain} / E_{no gain} data")
        graph.Draw("APL")
        canvas.SaveAs("canvas_gain.png")
        canvas.Write()


    def test_gain2(self):
        tool_es2012c = self.EnergyCorrectionTool()
        tool_es2012c.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c.initialize()
        tool_es2012c.useIntermoduleCorrection(False)
        tool_es2012c.usePhiUniformCorrection(False)

        tool_es2012c_nogain = self.EnergyCorrectionTool()
        tool_es2012c_nogain.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c_nogain.initialize()
        tool_es2012c_nogain.useGainCorrection(False)
        tool_es2012c_nogain.useIntermoduleCorrection(False)
        tool_es2012c_nogain.usePhiUniformCorrection(False)

        graph = ROOT.TGraph()
        ipoint = 0
        for eta in linspace(-3.5, 3.5, 300):
            E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(eta)) * 1E3
            E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(eta)) * 1E3
            E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(eta)) * 1E3
            E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(eta)) * 1E3

            phi = 0

            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                 eta,
                                                                                                 phi,
                                                                                                 eta,
                                                                                                 40E3,
                                                                                                 eta,
                                                                                                 phi)
            energy = tool_es2012c.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)
            energy_nogain = tool_es2012c_nogain.getCorrectedEnergy(0, Data, particle_information, ROOT.egEnergyCorr.Scale.None)

            if energy_nogain != 0:
                graph.SetPoint(ipoint, eta, energy/energy_nogain)
                ipoint += 1
            elif energy_nogain == 0 and energy == 0:
                graph.SetPoint(ipoint, eta, 1)
                ipoint += 1

        self.output_file.cd()
        canvas = ROOT.TCanvas()
        canvas.SetName("canvas_gain")
        graph.GetXaxis().SetTitle("#eta")
        graph.GetYaxis().SetTitle("E_{gain} / E_{no gain} data")
        graph.Draw("APL")
        canvas.SaveAs("canvas_gain2.png")
        canvas.Write()

    def energy_scan(self, tool, datatype, scale, smearing, etas):
        energies = []
        for eta in etas:
            E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(eta)) * 1E3
            E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(eta)) * 1E3
            E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(eta)) * 1E3
            E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(eta)) * 1E3

            phi = 0

            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                 eta,
                                                                                                 phi,
                                                                                                 eta,
                                                                                                 40E3,
                                                                                                 eta,
                                                                                                 phi)
            energy = tool.getCorrectedEnergy(0, datatype, particle_information,
                                             scale, smearing)
            energies.append(energy)
        return energies

    def test_nan(self):
        tool = self.EnergyCorrectionTool()
        tool.setESModel(ROOT.egEnergyCorr.es2012c)
        tool.initialize()

        for particle in ("electron", "converted", "unconverted"):
            for scale_energy in [0.05, 0.1, 0.5, 1., 2.0, 10., 100.]:
                for sys in self.all_systematics:
                    for eta in linspace(-5, 5, 500):
                        E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(eta)) * 1E3 * scale_energy
                        E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(eta)) * 1E3 * scale_energy
                        E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(eta)) * 1E3 * scale_energy
                        E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(eta)) * 1E3 * scale_energy
                        if E0 == 0 and E1 == 0 and E2 == 0 and E3 == 0:
                            E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(2)) * 1E3 * scale_energy
                            E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(2)) * 1E3 * scale_energy
                            E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(2)) * 1E3 * scale_energy
                            E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(2)) * 1E3 * scale_energy
                        cl_E = 40E3 * scale_energy

                        phi = 0

                        if particle == "converted":
                            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                                 eta,
                                                                                                                 phi,
                                                                                                                 cl_E,
                                                                                                                 eta,
                                                                                                                 phi,
                                                                                                                 35E3 * scale_energy,
                                                                                                                 20E3 * scale_energy,
                                                                                                                 18E3 * scale_energy,
                                                                                                                 1,
                                                                                                                 1,
                                                                                                                 1,
                                                                                                                 1, 100)
                            particle_str = "converted eta:%f, phi:%f, E0:%f, E1:%f, E2:%f, E3:%f, cl_E:%f, ptconv: %f, p1: %f, p2: %f, R: %f" % (eta, phi, E0, E1, E2, E3, cl_E, 35E3*scale_energy, 20E3*scale_energy, 18E3 * scale_energy, 100)

                        elif particle == "unconverted":
                            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                                 eta,
                                                                                                                 phi,
                                                                                                                 cl_E,
                                                                                                                 eta,
                                                                                                                 phi,
                                                                                                                 0,
                                                                                                                 0,
                                                                                                                 0,
                                                                                                                 0,
                                                                                                                 0,
                                                                                                                 0,
                                                                                                                 0, 0)
                            particle_str = "unconverted eta:%f, phi:%f, E0:%f, E1:%f, E2:%f, E3:%f, cl_E:%f" % (eta, phi, E0, E1, E2, E3, cl_E)

                        elif particle == "electron":
                            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                                 eta,
                                                                                                                 phi,
                                                                                                                 eta,
                                                                                                                 cl_E,
                                                                                                                 eta, phi)

                            particle_str = "electron eta:%f, phi:%f, E0:%f, E1:%f, E2:%f, E3:%f, cl_E:%f" % (eta, phi, E0, E1, E2, E3, cl_E)

                        energy = tool.getCorrectedEnergy(0, Full, particle_information, sys, ROOT.egEnergyCorr.Resolution.None)
                        self.assertFalse(math.isnan(energy), msg="got nan for sys=%f, %s" % (sys, particle_str))

                        if sys in (ROOT.egEnergyCorr.Scale.None, ROOT.egEnergyCorr.Scale.Nominal):
                            energy = tool.getCorrectedEnergy(0, Full, particle_information, sys, ROOT.egEnergyCorr.Resolution.Nominal)
                            self.assertFalse(math.isnan(energy), msg="got nan for Nominal res, %s" % particle_str)
                            energy = tool.getCorrectedEnergy(0, Full, particle_information, sys, ROOT.egEnergyCorr.Resolution.AllUp)
                            self.assertFalse(math.isnan(energy), msg="got nan for ErrorUp res, %s" % particle_str)
                            energy = tool.getCorrectedEnergy(0, Full, particle_information, sys, ROOT.egEnergyCorr.Resolution.AllDown)
                            self.assertFalse(math.isnan(energy), msg="got nan for ErrorDown res, %s" % particle_str)
                            energy = tool.getCorrectedEnergy(0, Data, particle_information, sys, ROOT.egEnergyCorr.Resolution.None)
                            self.assertFalse(math.isnan(energy), msg="got nan for Data, sys=%s, %s" % (sys, particle_str))

    def test_systematics(self):
        tool = self.EnergyCorrectionTool()
        tool.setESModel(ROOT.egEnergyCorr.es2012c)
        tool.initialize()

        etas = list(linspace(0, 2.4, 300))

        datatype = Full

        systematics = self.all_systematics

        def create_graph(etas, nominal, distorted):
            gr = ROOT.TGraph()
            for i, (eta, n, d) in enumerate(zip(etas, nominal, distorted)):
                gr.SetPoint(i, eta, d / n)
            return gr

        all_energies_smearing = {}
        all_energies_nosmearing = {}

        for sys in systematics + [ROOT.egEnergyCorr.Scale.Nominal]:
            tool.setRandomSeed(1)
            energies_sys_smearing = self.energy_scan(tool, datatype, sys, ROOT.egEnergyCorr.Resolution.Nominal, etas)
            energies_sys_nosmearing = self.energy_scan(tool, datatype, sys, ROOT.egEnergyCorr.Resolution.None, etas)
            all_energies_smearing[sys] = energies_sys_smearing
            all_energies_nosmearing[sys] = energies_sys_nosmearing

        ratios_smearing = {}
        ratios_nosmearing = {}
        for sys in systematics:
            ratios_smearing[sys] = create_graph(etas, all_energies_smearing[sys], all_energies_smearing[ROOT.egEnergyCorr.Scale.Nominal])
            ratios_nosmearing[sys] = create_graph(etas, all_energies_nosmearing[sys], all_energies_nosmearing[ROOT.egEnergyCorr.Scale.Nominal])
            ratios_smearing[sys].SetLineColor(ROOT.kRed)
            ratios_smearing[sys].SetMarkerStyle(24)
            ratios_smearing[sys].SetMarkerColor(ROOT.kRed)

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("All;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.AllUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.AllDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.AllUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.AllDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_All_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("LeakageUnconv;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.LeakageUnconvUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.LeakageUnconvDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.LeakageUnconvUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.LeakageUnconvDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_LeakageUnconv_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("LeakageConv;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.LeakageConvUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.LeakageConvDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.LeakageConvUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.LeakageConvDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_LeakageConv_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("S12;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.S12Up])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.S12Down])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.S12Up])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.S12Down])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_S12_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("PS;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.PSUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.PSDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.PSUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.PSDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_PS_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("ZeeStat;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.ZeeStatUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.ZeeStatDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.ZeeStatUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.ZeeStatDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_ZeeStat_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("ZeeSyst;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.ZeeSystUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.ZeeSystDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.ZeeSystUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.ZeeSystDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_ZeeSyst_es2012c.png")

        canvas = ROOT.TCanvas()
        multigraph = ROOT.TMultiGraph()
        multigraph.SetTitle("mg_Zeeall;#eta; E_{sys} / E_{nom}")
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.ZeeAllUp])
        multigraph.Add(ratios_smearing[ROOT.egEnergyCorr.Scale.ZeeAllDown])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.ZeeAllUp])
        multigraph.Add(ratios_nosmearing[ROOT.egEnergyCorr.Scale.ZeeAllDown])
        multigraph.Draw("APL")
        canvas.SaveAs("sys_ZeeAll_es2012c.png")

    def test_layer_invariance(self):
        tool_es2012c_layer1 = self.EnergyCorrectionTool()
        tool_es2012c_layer1.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c_layer1.useLayer2Recalibration(False)
        tool_es2012c_layer1.initialize()

        tool_es2012c_layer2 = self.EnergyCorrectionTool()
        tool_es2012c_layer2.setESModel(ROOT.egEnergyCorr.es2012c)
        tool_es2012c_layer2.initialize()

        for eta in linspace(-3.5, 3.5, 300):
            E0 = self.E0MC.GetBinContent(self.E0MC.FindBin(eta)) * 1E3
            E1 = self.E1MC.GetBinContent(self.E1MC.FindBin(eta)) * 1E3
            E2 = self.E2MC.GetBinContent(self.E2MC.FindBin(eta)) * 1E3
            E3 = self.E3MC.GetBinContent(self.E3MC.FindBin(eta)) * 1E3

            phi = 0

            particle_information = ROOT.AtlasRoot.egammaEnergyCorrectionTool.ParticleInformation(E0, E1, E2, E3,
                                                                                                 eta,
                                                                                                 phi,
                                                                                                 40E3,
                                                                                                 eta,
                                                                                                 phi,
                                                                                                 35E3,
                                                                                                 20E3,
                                                                                                 18E3,
                                                                                                 1,
                                                                                                 1,
                                                                                                 1,
                                                                                                 1, 100)
            energy1 = tool_es2012c_layer1.getCorrectedEnergy(0, Full, particle_information,
                                                             ROOT.egEnergyCorr.Scale.None,
                                                             ROOT.egEnergyCorr.Resolution.None)
            energy2 = tool_es2012c_layer2.getCorrectedEnergy(0, Full, particle_information,
                                                             ROOT.egEnergyCorr.Scale.None,
                                                             ROOT.egEnergyCorr.Resolution.None)

            self.assertEqual(energy1, energy2)


class TestConsistencyEgammaMVACalib(unittest.TestCase):
    def test_electron(self):
        weight_version = "egammaMVACalib/v1"
        tool_egammaMVACalib = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON,
                                                  True, weight_version)
        tool_egammaMVACalib.InitTree(0)
        # egammaEnergyCorrectionTool uses as default v1
        tool_EP4MC = ROOT.AtlasRoot.egammaEnergyCorrectionTool()
        tool_EP4MC.setESModel(6)   # es2012c
        tool_EP4MC.initialize()
        from itertools import product
        for eta, phi, phi_shift, eta_shift, el_cl_E in product(arange(-2.5, 2.5, 0.02),
                                                               arange(-3, 3, 0.2),
                                                               (-0.01, 0, 0.01),
                                                               (-0.01, 0, 0.01),
                                                               arange(1E3, 200E3, 20E3)):
            Es0 = el_cl_E * 0.8 * 0.05
            Es1 = el_cl_E * 0.8 * 0.10
            Es2 = el_cl_E * 0.8 * 0.65
            Es3 = el_cl_E * 0.8 * 0.20
            phiCalo = phi + phi_shift
            trk_eta = eta + eta_shift
            etaCalo = eta - eta_shift
            args = (0, 1,   # run number, Full simulation
                    Es0, Es1, Es2, Es3,
                    eta, phi, trk_eta,
                    el_cl_E,
                    etaCalo, phiCalo,
                    0, 0)  # no scale factor, no smearing
            energy_EP4MC = tool_EP4MC.getCorrectedEnergyElectron(*args)
            args = (Es0, Es1, Es2, Es3, eta, el_cl_E, etaCalo, phiCalo)
            energy_egammaMVACalib = tool_egammaMVACalib.getMVAEnergyElectron(*args)
            self.assertAlmostEqual(energy_EP4MC, energy_egammaMVACalib,
                                   msg="different energy eta=%f" % eta)

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREBIN/scripts/load_packages.C")
#    ROOT.gROOT.LoadMacro("AtlasStyle.C")
#    ROOT.SetAtlasStyle()
    ROOT.gStyle.SetCanvasDefH(800)
    ROOT.gStyle.SetCanvasDefW(800)
    ROOT.gStyle.SetPadTickX(1)
    ROOT.gStyle.SetPadTickY(1)
    ROOT.gStyle.SetLabelSize(0.03, "X")
    ROOT.gStyle.SetLabelSize(0.03, "Y")
    ROOT.gStyle.SetTitleSize(0.03, "X")
    ROOT.gStyle.SetTitleSize(0.03, "Y")
    unittest.main()
