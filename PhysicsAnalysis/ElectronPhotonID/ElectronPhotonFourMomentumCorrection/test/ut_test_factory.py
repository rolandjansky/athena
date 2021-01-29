#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import ROOT
import unittest


class TestEgammaFactory(unittest.TestCase):

    def test_eventinfo(self):
        ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()
        ei1 = factory.create_eventinfo(True, 100000)
        self.assertTrue(ei1.eventType(ROOT.xAOD.EventInfo.IS_SIMULATION))
        ei2 = factory.create_eventinfo(False, 100000)
        self.assertFalse(ei2.eventType(ROOT.xAOD.EventInfo.IS_SIMULATION))

    def test_unconverted(self):
        ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()

        runnumber = 10000
        ei = factory.create_eventinfo(True, runnumber)
        self.assertEqual(ei.runNumber(), runnumber)
        eta = 0.1
        phi = 0.2
        e = 100E3
        ph = factory.create_unconverted_photon(eta, phi, e)
        self.assertEqual(ph.caloCluster().nSamples(), 8)
        self.assertFalse(ph.vertex())
        self.assertAlmostEqual(ph.eta(), eta)
        self.assertAlmostEqual(ph.caloCluster().eta(), eta)
        self.assertAlmostEqual(ph.phi(), phi)
        self.assertAlmostEqual(ph.caloCluster().phi(), phi)
        self.assertAlmostEqual(
            ph.caloCluster().auxdata("float")("etaCalo"), eta)
        self.assertAlmostEqual(
            ph.caloCluster().auxdata("float")("phiCalo"), phi)
        self.assertAlmostEqual(ph.e(), e, delta=0.01)
        self.assertEqual(ph.caloCluster().e(), e)
        for i in range(3):
            self.assertGreater(ph.caloCluster().energyBE(i), 0)

    def test_converted(self):
        event = ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()

        runnumber = 10000
        ei = factory.create_eventinfo(True, runnumber)
        self.assertEqual(ei.runNumber(), runnumber)
        eta = 0.1
        phi = 0.2
        e = 100E3
        ph = factory.create_converted_photon(eta, phi, e)
        self.assertEqual(ph.caloCluster().nSamples(), 8)
        self.assertGreater(ph.vertex().x(), 0)
        self.assertAlmostEqual(ph.eta(), eta)
        self.assertAlmostEqual(ph.caloCluster().eta(), eta)
        self.assertAlmostEqual(ph.phi(), phi)
        self.assertAlmostEqual(ph.caloCluster().phi(), phi)
        self.assertAlmostEqual(
            ph.caloCluster().auxdata("float")("etaCalo"), eta)
        self.assertAlmostEqual(
            ph.caloCluster().auxdata("float")("phiCalo"), phi)
        self.assertAlmostEqual(ph.e(), e, delta=0.01)
        self.assertEqual(ph.caloCluster().e(), e)
        for i in range(3):
            self.assertGreater(ph.caloCluster().energyBE(i), 0)

    def test_photon(self):
         ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()

        runnumber = 10000
        ei = factory.create_eventinfo(True, runnumber)
        self.assertEqual(ei.runNumber(), runnumber)
        e0 = 10E3
        e1 = 20E3
        e2 = 60E3
        e3 = 20E3
        r = 10.
        eta = 0.1
        phi = 0.2
        e = 1.2 * (e0 + e1 + e2 + e3)
        ph = factory.create_photon(eta, phi, e0, e1, e2, e3, e, r)
        self.assertEqual(ph.caloCluster().nSamples(), 8)
        self.assertEqual(ph.caloCluster().energyBE(0), e0)
        self.assertEqual(ph.caloCluster().energyBE(1), e1)
        self.assertEqual(ph.caloCluster().energyBE(2), e2)
        self.assertEqual(ph.caloCluster().energyBE(3), e3)
        self.assertEqual(ph.vertex().x(), r)
        self.assertAlmostEqual(ph.eta(), eta)
        self.assertAlmostEqual(ph.caloCluster().eta(), eta)
        self.assertAlmostEqual(ph.phi(), phi)
        self.assertAlmostEqual(ph.caloCluster().phi(), phi)
        self.assertAlmostEqual(
            ph.caloCluster().auxdata("float")("etaCalo"), eta)
        self.assertAlmostEqual(
            ph.caloCluster().auxdata("float")("phiCalo"), phi)
        self.assertAlmostEqual(ph.e(), e, delta=0.01)
        self.assertEqual(ph.caloCluster().e(), e)

        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool.setProperty("ESModel", "es2012c")
        self.assertTrue(tool.initialize().isSuccess())

        tool.applyCorrection(ph, ei)

    def test_electron(self):
        ROOT.xAOD.TEvent()
        factory = ROOT.EgammaFactory()

        runnumber = 10000
        ei = factory.create_eventinfo(True, runnumber)
        self.assertEqual(ei.runNumber(), runnumber)
        e0 = 10E3
        e1 = 20E3
        e2 = 60E3
        e3 = 20E3
        eta = 0.1
        phi = 0.2
        e = 1.2 * (e0 + e1 + e2 + e3)
        el = factory.create_electron(eta, phi, e0, e1, e2, e3, e)
        self.assertEqual(el.caloCluster().nSamples(), 8)
        self.assertEqual(el.caloCluster().energyBE(0), e0)
        self.assertEqual(el.caloCluster().energyBE(1), e1)
        self.assertEqual(el.caloCluster().energyBE(2), e2)
        self.assertEqual(el.caloCluster().energyBE(3), e3)
        self.assertAlmostEqual(el.eta(), eta)
        self.assertAlmostEqual(el.caloCluster().eta(), eta)
        self.assertAlmostEqual(el.phi(), phi)
        self.assertAlmostEqual(el.caloCluster().phi(), phi)
        self.assertAlmostEqual(
            el.caloCluster().auxdata("float")("etaCalo"), eta)
        self.assertAlmostEqual(
            el.caloCluster().auxdata("float")("phiCalo"), phi)
        self.assertAlmostEqual(el.e(), e, delta=0.01)
        self.assertEqual(el.caloCluster().e(), e)
        self.assertAlmostEqual(el.trackParticle().eta(), eta, delta=0.00001)

    def test_clean(self):
        ROOT.xAOD.TEvent()

        factory = ROOT.EgammaFactory()
        runnumber = 10000
        ei = factory.create_eventinfo(True, runnumber)
        self.assertEqual(ei.runNumber(), runnumber)

        el = factory.create_electron(1, 2, 3, 4, 5, 6, 7)
        self.assertAlmostEqual(el.eta(), 1)

        factory.clear()

        el = factory.create_electron(1, 2, 3, 4, 5, 6, 7)
        self.assertAlmostEqual(el.eta(), 1)

    def test_decoration(self):
        ROOT.xAOD.TEvent()

        factory = ROOT.EgammaFactory()
        runnumber = 10000
        ei = factory.create_eventinfo(True, runnumber)
        self.assertEqual(ei.runNumber(), runnumber)

        el = factory.create_electron(1, 2, 3, 4, 5, 6, 7)
        el.auxdecor("double")("mydeco")
        el.auxdataConst("double")("mydeco")

        el = factory.create_electron(1, 2, 3, 4, 5, 6, 7)
        el.auxdataConst("double")("mydeco")

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    unittest.main()
