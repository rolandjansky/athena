#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT
import math


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


def linspace(start, stop, n):
    step = (stop - start) / float(n - 1)
    result = []
    for i in range(n):
        result.append(start + step * i)
    return result


class TestEgammaCalibrationAndSmearingTool(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.event = ROOT.xAOD.TEvent()
        cls.factory = ROOT.EgammaFactory()
        cls.tool_es2012c = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2012c")

    def test_initialization(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        self.assertFalse(tool.initialize().isSuccess())

        tool.setProperty("ESModel", "xyz").ignore()
        self.assertFalse(tool.initialize().isSuccess())

        tool.setProperty("ESModel", "es2010").ignore()
        self.assertTrue(tool.initialize().isSuccess())

    def generator_photon(self, eta_range=None, e_range=None):
        eta_range = eta_range or arange(-2.49, 2.49, 0.05)
        e_range = e_range or arange(5E3, 1000E3, 100E3)
        for eta in eta_range:
            for e in e_range:
                photon = self.factory.create_photon(eta, 0.1, e, 0)
                yield photon

    def generator_electron(self, eta_range=None, e_range=None):
        eta_range = eta_range or arange(-2.49, 2.49, 0.05)
        e_range = e_range or arange(5E3, 1000E3, 100E3)
        for eta in eta_range:
            for e in e_range:
                electron = self.factory.create_electron(eta, 0.1, e)
                yield electron


    def test_MVA_compatibility(self):
        """
        this test that without any correction the response of the tool is the same
        as the one with the MVA
        """
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c").ignore()
        tool.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool.initialize().isSuccess())

        tool_MVA = ROOT.egammaMVATool('MVA_tool')
        tool_MVA.setProperty("folder", "egammaMVACalib/v1").ignore()
        self.assertTrue(tool_MVA.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation
        for ph in self.generator_photon():
            e1 = tool.getPhotonEnergy(ph, ei)
            e2 = tool_MVA.getEnergy(ph.caloCluster(), ph)
            self.assertEqual(e1, e2)

    def test_es2015PRE(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_2015PRE")
        tool.setProperty("ESModel", "es2015PRE").ignore()
        tool.setProperty("int")("doSmearing", 0).ignore()

        self.assertTrue(tool.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation
        for el in self.generator_electron():
            e_before = el.e()
            e_cluster = el.caloCluster().energyBE(0) + el.caloCluster().energyBE(1) + el.caloCluster().energyBE(2) + el.caloCluster().energyBE(3)
            e_after = tool.getEnergy(el, ei)
            if e_before > 7E3:
                self.assertTrue(abs(e_before / e_after - 1) < 0.5,
                                msg="energy cluser/calibrated very different at eta = %f: %f/%f, e0=%.2f, e1=%.2f, e2=%.2f, e3=%.2f" % (el.eta(), e_cluster, e_after, el.caloCluster().energyBE(0), el.caloCluster().energyBE(1), el.caloCluster().energyBE(2), el.caloCluster().energyBE(3)))
            self.assertGreater(e_after, 0)

    def test_es2012XX(self):
        tool1 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool1")
        tool1.setProperty("ESModel", "es2012c").ignore()
        tool1.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool2")
        tool2.setProperty("ESModel", "es2012XX").ignore()
        tool2.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool2.initialize().isSuccess())

        tool3 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool3")
        tool3.setProperty("ESModel", "es2012c").ignore()
        tool3.setProperty("int")("doSmearing", 0).ignore()
        tool3.setProperty("MVAfolder", "egammaMVACalib/offline/v3").ignore()
        self.assertTrue(tool3.initialize().isSuccess())

        tool4 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool4")
        tool4.setProperty("ESModel", "es2015PRE").ignore()
        tool4.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool4.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            e1 = tool1.getPhotonEnergy(ph, ei)
            e2 = tool2.getPhotonEnergy(ph, ei)
            e3 = tool2.getPhotonEnergy(ph, ei)
            e4 = tool2.getPhotonEnergy(ph, ei)
            self.assertNotEqual(e1, e2)
            self.assertEqual(e2, e3)
            self.assertEqual(e2, e4)

    def test_differentMVA(self):
        """
        test if different MVA give different result
        """
        tool1 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool1.setProperty("ESModel", "es2012c").ignore()
        tool1.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool2")
        tool2.setProperty("ESModel", "es2012c").ignore()
        tool2.setProperty("int")("doSmearing", 0).ignore()
        tool2.setProperty("MVAfolder", "egammaMVACalib/v1").ignore()
        self.assertTrue(tool2.initialize().isSuccess())

        tool3 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool3")
        tool3.setProperty("ESModel", "es2012c").ignore()
        tool3.setProperty("int")("doSmearing", 0).ignore()
        tool3.setProperty("MVAfolder", "egammaMVACalib/offline/v3").ignore()
        self.assertTrue(tool3.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            e1 = tool1.getPhotonEnergy(ph, ei)
            e2 = tool2.getPhotonEnergy(ph, ei)
            e3 = tool3.getPhotonEnergy(ph, ei)
            self.assertNotEqual(e1, e3)
            self.assertEqual(e1, e2)

    def test_differentMVA2(self):
        """
        test if different MVA give different result
        """
        tool1 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool1.setProperty("ESModel", "es2015PRE").ignore()
        tool1.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool2")
        tool2.setProperty("ESModel", "es2015PRE").ignore()
        tool2.setProperty("int")("doSmearing", 0).ignore()
        tool2.setProperty("MVAfolder", "egammaMVACalib/v1").ignore()
        self.assertTrue(tool2.initialize().isSuccess())

        tool3 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool3")
        tool3.setProperty("ESModel", "es2015PRE").ignore()
        tool3.setProperty("int")("doSmearing", 0).ignore()
        tool3.setProperty("MVAfolder", "egammaMVACalib/offline/v3").ignore()
        self.assertTrue(tool3.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            e1 = tool1.getPhotonEnergy(ph, ei)
            e2 = tool2.getPhotonEnergy(ph, ei)
            e3 = tool3.getPhotonEnergy(ph, ei)
            self.assertEqual(e1, e3)
            self.assertNotEqual(e1, e2)

    def test_list_syst(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c").ignore()
        tool.setProperty("decorrelationModel", "1NP_v1").ignore()
        self.assertTrue(tool.initialize().isSuccess())
        s = tool.recommendedSystematics()
        for ss in s:
            print ss.name()
        # TODO

    def test_same_smearing(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c").ignore()
        self.assertTrue(tool.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)
        for ph in self.generator_photon():
            tool.applyCorrection(ph, ei)
            e1 = ph.e()
            tool.applyCorrection(ph, ei)
            e2 = ph.e()
            self.assertEqual(e1, e2,
                             msg="energies should be equal (%f!=%f) at eta = %f" % (e1, e2, ph.eta()))

    def test_correction(self):
        tool1 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool1.setProperty("ESModel", "es2012c").ignore()
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool2.setProperty("ESModel", "es2012c").ignore()
        tool2.setProperty("int")("doScaleCorrection", 0).ignore()
        self.assertTrue(tool2.initialize().isSuccess())

        ei = self.factory.create_eventinfo(False, 100000)  # data

        for ph in self.generator_photon():
            e1 = tool1.getPhotonEnergy(ph, ei)
            e1_bis = tool1.getPhotonEnergy(ph, ei)
            e2 = tool2.getPhotonEnergy(ph, ei)
            if abs(ph.eta()) < 2.47:  # TODO: move to 2.5
                self.assertNotEqual(e1, e2, msg="equal at eta = %f" % ph.eta())
            self.assertEqual(e1, e1_bis)

    def test_syst_bin(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c").ignore()
        tool.setProperty("int")("doSmearing", 0).ignore()
        self.assertTrue(tool.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            tool.applySystematicVariation(ROOT.CP.SystematicSet()).ignore()
            e_nom = tool.getPhotonEnergy(ph, ei)
            sys_set = ROOT.CP.SystematicSet()
            sys_set.insert(ROOT.CP.SystematicVariation("EG_SCALE_ZEESYST", 1.))
            tool.applySystematicVariation(sys_set).ignore()
            e_zee_syst = tool.getPhotonEnergy(ph, ei)
            sys_set = ROOT.CP.SystematicSet()
            sys_set.insert(ROOT.CP.SystematicVariation("EG_SCALE_LARCALIB__ETABIN0", 1.))
            tool.applySystematicVariation(sys_set).ignore()
            e_larcalib__bin0 = tool.getPhotonEnergy(ph, ei)
            if abs(ph.eta()) < 2.47:
                self.assertNotEqual(e_nom, e_zee_syst,
                                    msg="energy should not be equal, eta = %f" % ph.eta())
            if abs(ph.eta()) < 1.45:
                self.assertNotEqual(e_nom, e_larcalib__bin0,
                                    msg="energy should not be equal, eta = %f" % ph.eta())
            elif abs(ph.eta()) < 2.47:
                self.assertEqual(e_nom, e_larcalib__bin0,
                                 msg="energy should be equal (%f != %f), eta = %f" % (e_nom, e_larcalib__bin0, ph.eta()))

    def test_intermodule_correction_working(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c").ignore()
        tool_no_correction = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_no_correction")
        tool_no_correction.setProperty("ESModel", "es2012c").ignore()
        tool_no_correction.setProperty("int")("useIntermoduleCorrection", 0).ignore()

        self.assertTrue(tool.initialize().isSuccess())
        self.assertTrue(tool_no_correction.initialize().isSuccess())

        energy, energy_no_correction = [], []

        ei = self.factory.create_eventinfo(False, 100000)  # data
        for phi in linspace(-math.pi, math.pi, 400):
            ph = self.factory.create_photon(0.1, phi, 100E3, 0)
            energy.append(tool.getPhotonEnergy(ph, ei))
            energy_no_correction.append(tool_no_correction.getPhotonEnergy(ph, ei))

        self.assertFalse(all([x == y for x, y in zip(energy, energy_no_correction)]))

    def test_idempotence(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        tool.setProperty("ESModel", "es2015PRE").ignore()
        tool.initialize().ignore()
        ei = self.factory.create_eventinfo(False, 100000)  # data
        for generator in self.generator_photon, self.generator_electron:
            for p in generator():
                tool.applyCorrection(p, ei).ignore()
                e1 = p.e()
                pt1 = p.pt()
                eta1 = p.eta()
                tool.applyCorrection(p, ei).ignore()
                e2 = p.e()
                pt2 = p.pt()
                eta2 = p.eta()
                self.assertAlmostEqual(e1, e2)
                self.assertAlmostEqual(pt1, pt2)
                self.assertAlmostEqual(eta1, eta2)

    def test_1NP_100GeV_electron_es2015PRE(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        tool.setProperty("ESModel", "es2015PRE").ignore()
        tool.setProperty("decorrelationModel", "1NP_v1").ignore()
        tool.initialize().ignore()
        ei = self.factory.create_eventinfo(False, 100000)  # data
        el = self.factory.create_electron(0.1, 0.1, 100E3)
        e1 = tool.getEnergy(el, ei)
        self.assertFalse(math.isnan(e1))
        self.assertTrue(e1 > 0)
        self.assertTrue(abs(e1 / 100E3 - 1) < 0.1)
        self.assertNotAlmostEqual(e1, 100E3)

        ei = self.factory.create_eventinfo(True, 100000)  # MC
        e2 = tool.getEnergy(el, ei)
        self.assertFalse(math.isnan(e2))
        self.assertTrue(e2 > 0)
        self.assertTrue(abs(e2 / 100E3 - 1) < 0.1)
        self.assertNotAlmostEqual(e2, 100E3)
        self.assertNotAlmostEqual(e1, e2)

        all_syst = tool.recommendedSystematics()
        for sys in all_syst:
            sys_set = ROOT.CP.SystematicSet()
            sys_set.insert(sys)
            tool.applySystematicVariation(sys_set).ignore()
            e = tool.getEnergy(el, ei)
            self.assertFalse(math.isnan(e))
            self.assertTrue(e > 0)
            self.assertTrue(abs(e / 100E3 - 1) < 0.1)
            self.assertNotAlmostEqual(e2, e)
            self.assertNotAlmostEqual(e1, e)


if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    #ROOT.xAOD.TReturnCode.enableFailure()
    unittest.main()
