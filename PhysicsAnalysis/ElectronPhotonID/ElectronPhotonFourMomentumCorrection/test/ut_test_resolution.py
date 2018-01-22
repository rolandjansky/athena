#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT
import math
from array import array


def arange(xmin, xmax, delta):
    # just to don't inject dep from numpy
    x = xmin
    while True:
        yield x
        x += delta
        if x >= xmax:
            break


class TestEgammaResolution(unittest.TestCase):
    """
    This test case tests the resolution class used for the expected resolution
    """

    @classmethod
    def setUpClass(cls):
        cls.tool_run1 = ROOT.eg_resolution("run1")
        cls.tool_run2 = ROOT.eg_resolution("run2_pre")
        cls.event = ROOT.xAOD.TEvent()
        cls.factory = ROOT.EgammaFactory()

    def loop_combination(self, eta_min=-2.5, eta_max=2.5, eta_step=0.05,
                              energy_min=0, energy_max=200E3, energy_step=10E3,
                              ptypes=(0, 1, 2, 3),
                              resol_types=(0, 1, 2)):
        for ptype in ptypes:
            for resol_type in resol_types:
                for eta in arange(eta_min, eta_max, eta_step):
                    for energy in arange(energy_min, energy_max, energy_step):
                        yield ptype, energy, eta, resol_type

    #@unittest.expectedFailure  # return nan in the crack (1.37-1.52), inf at >= 2.4
    def test_resolution_positive(self):
        for ptype, energy, eta, resol_type in self.loop_combination():
            for tool in self.tool_run1, self.tool_run2:
                if 1.4 < abs(eta) <= 1.51 or abs(eta) > 2.39:
                    continue
                if ptype == 3:
                    continue
                resolution = tool.getResolution(ptype, energy, eta, resol_type)
                self.assertFalse(math.isnan(resolution), msg="resolution is nan for eta=%f" % eta)
                self.assertGreater(resolution, 0)

    def create_response_run1(self):
        """
        this should not be executed, to don't invalidate reference file created
        on 2015-04-27 with ElectronPhotonFourMomentumCorrection-00-01-23
        """
        fout = ROOT.TFile("$ROOTCOREBIN/data/ElectronPhotonFourMomentumCorrection/test_resolution_nonregression_run1_data.root", "recreate")
        tree = ROOT.TTree("test_resolution_nonregression_data_run1", "test_resolution_nonregression_data")
        data_ptype, data_resol_type, data_eta, data_energy, data_resolution = map(lambda t: array(t, [0]), ('i', 'i', 'f', 'f', 'f'))

        tree.Branch("ptype", data_ptype, "ptype/I")
        tree.Branch("resol_type", data_resol_type, "resol_type/I")
        tree.Branch("eta", data_eta, "eta/F")
        tree.Branch("energy", data_energy, "energy/F")
        tree.Branch("resolution", data_resolution, "resolution/F")

        for ptype, energy, eta, resol_type in self.loop_combination_run1():
            resolution = self.tool_run1.getResolution(ptype, energy, eta, resol_type)
            data_ptype[0] = ptype
            data_energy[0] = energy
            data_eta[0] = eta
            data_resol_type[0] = resol_type
            data_resolution[0] = resolution

            tree.Fill()

        tree.Write()
        fout.Close

    def test_difference_run1_run2_pre(self):
        tool_run2 = self.tool_run2
        for ptype in 0, 1, 2:
            for eta in arange(0, 1.37, 0.1):
                for e in arange(10E3, 1000E3, 100E3):
                    for t in 0, 1, 2:
                        resolution_run1 = self.tool_run1.getResolution(ptype, e, eta, t)
                        resolution_run2 = tool_run2.getResolution(ptype, e, eta, t)
                        self.assertNotAlmostEqual(resolution_run1, resolution_run2,
                                                  msg="resolution should be different for particle=%d, eta=%f, e=%d, rtype=%d" % (ptype, eta, e, t))

    def test_nonregression_run1(self):
        from PathResolver import PathResolver
        rootfile = PathResolver.FindCalibFile("ElectronPhotonFourMomentumCorrection/v8/test_resolution_nonregression_run1_data.root")
        f = ROOT.TFile(rootfile)
        tree = f.Get("test_resolution_nonregression_data_run1")

        for ievent in xrange(tree.GetEntries()):

            tree.GetEntry(ievent)
            resolution = self.tool_run1.getResolution(tree.ptype,
                                                      tree.energy, tree.eta,
                                                      tree.resol_type)
            expected_response = tree.resolution
            if math.isnan(resolution):
                self.assertTrue(math.isnan(expected_response), msg="resolution is nan, but expected resonse is %f" % expected_response)
            elif math.isnan(expected_response):
                self.assertTrue(math.isnan(resolution), msg="expected response is nan, but resolution is %f" % resolution)
            else:
                self.assertAlmostEqual(resolution, expected_response,
                                       msg="resolution mismatch %f != %f at eta=%f, energy=%f, ptype=%d, resol_type=%d" % (resolution, expected_response, tree.eta, tree.energy, tree.ptype, tree.resol_type))

    @unittest.skip("CHECK")
    def test_resolution_interface(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c").ignore()
        tool.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool.initialize().isSuccess())

        energies = arange(5E3, 1000E3, 100E3)

        for particle in 'unconverted', 'converted', 'electron':
            for energy in energies:
                if particle == 'unconverted':
                    p = self.factory.create_photon(0., 0.1, energy, 0)
                    r1 = tool.resolution(energy, 0., 0., ROOT.PATCore.ParticleType.UnconvertedPhoton)
                    r2 = tool.getResolution(p)
                    self.assertAlmostEqual(r1, r2)
                elif particle == 'converted':
                    p = self.factory.create_photon(0., 0.1, energy, 100)
                    r1 = tool.resolution(energy, 0., 0., ROOT.PATCore.ParticleType.ConvertedPhoton)
                    r2 = tool.getResolution(p)
                    self.assertAlmostEqual(r1, r2)
                elif particle == 'electron':
                    p = self.factory.create_electron(0., 0.1, energy)
                    r1 = tool.resolution(energy, 0., 0., ROOT.PATCore.ParticleType.Electron)
                    r2 = tool.getResolution(p)
                    self.assertAlmostEqual(r1, r2)
                else:
                    raise ValueError()

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    unittest.main()
