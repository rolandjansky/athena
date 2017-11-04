#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT
import math
import random

RUN2016 = 297730l
RUN2015 = 252604l

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
        tool.msg().setLevel(ROOT.MSG.WARNING)
        self.assertFalse(tool.initialize().isSuccess())

        self.assertTrue(tool.setProperty("ESModel", "xyz").isSuccess())
        self.assertFalse(tool.initialize().isSuccess())

        self.assertTrue(tool.setProperty("ESModel", "es2010").isSuccess())
        self.assertTrue(tool.initialize().isSuccess())

    def generator_kinematics(self, eta_range=None, e_range=None, phi_range=None):
        eta_range = eta_range or arange(-2.49, 2.49, 0.05)
        e_range = e_range or arange(5E3, 1000E3, 100E3)
        phi_range = phi_range or [0.]
        from itertools import product
        return product(e_range, eta_range, phi_range)

    def generator_photon(self, eta_range=None, e_range=None, phi_range=None):
        random.seed(10)
        factory = self.factory
        for e, eta, phi in self.generator_kinematics(eta_range, e_range, phi_range):
            isconv = random.random() < 0.3
            if isconv:
                yield factory.create_converted_photon(eta, phi, e)
            else:
                yield factory.create_unconverted_photon(eta, phi, e)
        factory.clear()

    def generator_converted(self, eta_range=None, e_range=None, phi_range=None):
        factory = self.factory
        for e, eta, phi in self.generator_kinematics(eta_range, e_range, phi_range):
            yield factory.create_converted_photon(eta, phi, e)
        factory.clear()

    def generator_unconverted(self, eta_range=None, e_range=None, phi_range=None):
        factory = self.factory
        for e, eta, phi in self.generator_kinematics(eta_range, e_range, phi_range):
            yield factory.create_unconverted_photon(eta, phi, e)
        factory.clear()

    def generator_electron(self, eta_range=None, e_range=None, phi_range=None):
        factory = self.factory
        for e, eta, phi in self.generator_kinematics(eta_range, e_range, phi_range):
            yield factory.create_electron(eta, phi, e)
        factory.clear()

    def generators(self, *args, **kwargs):
        return {'electron': self.generator_electron(*args, **kwargs),
                'converted': self.generator_converted(*args, **kwargs),
                'unconverted': self.generator_unconverted(*args, **kwargs)}

    def test_MVA_compatibility(self):
        """
        this test that without any correction the response of the tool is the same
        as the one with the MVA
        """
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        self.assertTrue(tool.setProperty("ESModel", "es2012c").isSuccess())
        self.assertTrue(tool.setProperty("int")("randomRunNumber", RUN2015).isSuccess())
        self.assertTrue(tool.setProperty("int")("doSmearing", 0).isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)

        self.assertTrue(tool.initialize().isSuccess())

        tool_MVA = ROOT.egammaMVATool('MVA_tool')
        tool_MVA.setProperty("folder", "egammaMVACalib/v1").ignore()
        tool_MVA.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool_MVA.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation
        for ph in self.generator_photon():
            e2 = tool_MVA.getEnergy(ph.caloCluster(), ph)
            tool.applyCorrection(ph, ei)
            e1 = ph.e()
            self.assertAlmostEqual(e1, e2, delta=0.1)

    def test_AllUp(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_2015PRE")
        self.assertTrue(tool.setProperty("ESModel", "es2015PRE").isSuccess())
        self.assertTrue(tool.setProperty("int")("doSmearing", 0).isSuccess())
        self.assertTrue(tool.setProperty("decorrelationModel", "1NP_v1"). isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool.initialize().isSuccess())
        ei = self.factory.create_eventinfo(True, 100000)   # simulation
        set_all_up = ROOT.CP.SystematicSet()
        set_all_up.insert(ROOT.CP.SystematicVariation("EG_SCALE_ALL", 1))
        for el in self.generator_photon():
            tool.applySystematicVariation(ROOT.CP.SystematicSet()).ignore()
            nominal_energy = tool.getEnergy(el, ei)
            tool.applySystematicVariation(set_all_up).ignore()
            all_up = tool.getEnergy(el, ei)
            self.assertNotAlmostEqual(nominal_energy, all_up)

    def test_es2015PRE(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_2015PRE")
        self.assertTrue(tool.setProperty("ESModel", "es2015PRE").isSuccess())
        self.assertTrue(tool.setProperty("int")("doSmearing", 0).isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation
        for el in self.generator_electron():
            e_before = el.e()
            e_cluster = el.caloCluster().energyBE(0) + el.caloCluster().energyBE(1) + el.caloCluster().energyBE(2) + el.caloCluster().energyBE(3)
            tool.applyCorrection(el, ei)
            e_after = el.e()
            if e_before > 7E3:
                self.assertTrue(abs(e_before / e_after - 1) < 0.5,
                                msg="energy cluser/calibrated very different at eta = %f: %f/%f, e0=%.2f, e1=%.2f, e2=%.2f, e3=%.2f" % (el.eta(), e_cluster, e_after, el.caloCluster().energyBE(0), el.caloCluster().energyBE(1), el.caloCluster().energyBE(2), el.caloCluster().energyBE(3)))
            self.assertGreater(e_after, 0)

    def test_es2012XX(self):
        tool1 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool1")
        tool1.setProperty("ESModel", "es2012c").ignore()
        tool1.setProperty("int")("doSmearing", 0).ignore()
        tool1.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool2")
        tool2.setProperty("ESModel", "es2012XX").ignore()
        tool2.setProperty("int")("doSmearing", 0).ignore()
        tool2.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool2.initialize().isSuccess())

        tool3 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool3")
        tool3.setProperty("ESModel", "es2012c").ignore()
        tool3.setProperty("int")("doSmearing", 0).ignore()
        tool3.setProperty("MVAfolder", "egammaMVACalib/offline/v3").ignore()
        tool3.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool3.initialize().isSuccess())

        tool4 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool4")
        tool4.setProperty("ESModel", "es2015PRE").ignore()
        tool4.setProperty("int")("doSmearing", 0).ignore()
        tool4.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool4.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            e1 = tool1.getEnergy(ph, ei)
            e2 = tool2.getEnergy(ph, ei)
            e3 = tool2.getEnergy(ph, ei)
            e4 = tool2.getEnergy(ph, ei)
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
        tool1.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool2")
        tool2.setProperty("ESModel", "es2012c").ignore()
        tool2.setProperty("int")("doSmearing", 0).ignore()
        tool2.setProperty("MVAfolder", "egammaMVACalib/v1").ignore()
        tool2.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool2.initialize().isSuccess())

        tool3 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool3")
        tool3.setProperty("ESModel", "es2012c").ignore()
        tool3.setProperty("int")("doSmearing", 0).ignore()
        tool3.setProperty("MVAfolder", "egammaMVACalib/offline/v3").ignore()
        tool3.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool3.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            e1 = tool1.getEnergy(ph, ei)
            e2 = tool2.getEnergy(ph, ei)
            e3 = tool3.getEnergy(ph, ei)
            self.assertNotEqual(e1, e3)
            self.assertEqual(e1, e2)

    def test_differentMVA2(self):
        """
        test if different MVAs give different result
        """
        tool1 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool1.setProperty("ESModel", "es2015PRE").ignore()
        tool1.setProperty("int")("doSmearing", 0).ignore()
        tool1.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool2")
        tool2.setProperty("ESModel", "es2015PRE").ignore()
        tool2.setProperty("int")("doSmearing", 0).ignore()
        tool2.setProperty("MVAfolder", "egammaMVACalib/v1").ignore()
        tool2.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool2.initialize().isSuccess())

        tool3 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool3")
        tool3.setProperty("ESModel", "es2015PRE").ignore()
        tool3.setProperty("int")("doSmearing", 0).ignore()
        tool3.setProperty("MVAfolder", "egammaMVACalib/offline/v3").ignore()
        tool3.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool3.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            e1 = tool1.getEnergy(ph, ei)
            e2 = tool2.getEnergy(ph, ei)
            e3 = tool3.getEnergy(ph, ei)
            self.assertEqual(e1, e3)
            self.assertNotEqual(e1, e2)

    # rename it test* if you want to generate a new file
    def create_MVA_testfile(self, esmodel='es2015cPRE', particle='electron', isdata=True):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        self.assertTrue(tool.setProperty("ESModel", esmodel).isSuccess())
        self.assertTrue(tool.setProperty("int")("doSmearing", 0).isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool.initialize().isSuccess())
        ei = self.factory.create_eventinfo(not isdata, 100000)

        import random
        import csv
        with open("testmva_%s_%s_%s.csv" % (esmodel, particle, "data" if isdata else "fullsim"), 'wb') as csvfile:
            spamwriter = csv.writer(csvfile, delimiter=' ', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            if particle == "photon":
                spamwriter.writerow(('eta', 'phi', 'e0', 'e1', 'e2', 'e3', 'e', 'rconv', 'zconv', 'output'))
            elif particle == "electron":
                spamwriter.writerow(('eta', 'phi', 'e0', 'e1', 'e2', 'e3', 'e', 'output'))

            for eta in arange(-3.0, 3.0, 0.05):
                phi = 0
                e = random.expovariate(1. / 200E3)
                e0 = random.uniform(0.05, 0.1) * e
                e1 = random.uniform(0.1, 0.2) * e
                e2 = random.uniform(0.4, 0.6) * e
                e3 = random.uniform(0.1, 0.2) * e
                e = (e0 + e1 + e2 + e3)

                if particle == "photon":
                    rconv = random.uniform(0, 2000)
                    zconv = random.uniform(0, 800)
                    if rconv > 1000:
                        rconv = 0
                        zconv = 0

                    args = [eta, phi, e0, e1, e2, e3, e, rconv, zconv]
                    p = self.factory.create_photon(*args)
                elif particle == "electron":
                    args = [eta, phi, e0, e1, e2, e3, e]
                    p = self.factory.create_electron(*args)

                tool.applyCorrection(p, ei)
                calibrated_energy = p.e()

                spamwriter.writerow(args + [calibrated_energy])

    @unittest.skip("ATLASG-694")  # FIXME: the problem is in the factory
    def test_MVA_all_simulation(self):
        for particle in 'electron', 'photon':
            self._test_MVA('es2015PRE', particle, False)
            self._test_MVA('es2015cPRE', particle, False)
            self._test_MVA('es2012c', particle, False)

    @unittest.skip("ATLASG-694")  # FIXME: the problem is in the factory
    def test_MVA_all_data(self):
        for particle in 'electron', 'photon':
            self._test_MVA('es2015PRE', particle, True)
            self._test_MVA('es2015cPRE', particle, True)
            self._test_MVA('es2012c', particle, True)

    def _test_MVA(self, esmodel, particle, isdata):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        self.assertTrue(tool.setProperty("ESModel", esmodel).isSuccess())
        self.assertTrue(tool.setProperty("int")("doSmearing", 0).isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool.initialize().isSuccess())
        ei = self.factory.create_eventinfo(not isdata, 100000)   # simulation

        import os
        basedir = os.path.dirname(os.path.abspath(__file__))
        filename = os.path.join(basedir, "testmva_%s_%s_%s.csv" % (esmodel, particle, "data" if isdata else "fullsim"))

        import csv
        with open(filename, 'rb') as csvfile:
            reader = csv.reader(csvfile, delimiter=' ', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            next(reader, None)  # skip header
            for row in reader:
                args = map(float, row[:-1])
                p = {'electron': self.factory.create_electron,
                     'photon': self.factory.create_photon}[particle](*args)
                tool.applyCorrection(p, ei)
                calibrated_energy = p.e()
                self.assertAlmostEqual(calibrated_energy, float(row[-1]), delta=0.1)
        self.factory.clear()

    def test_list_syst(self):
        def get_names_sys(tool):
            s = tool.recommendedSystematics()
            return [ss.name() for ss in s]

        def _test_list_syst(model, decorrelation, decorrelation_scale, decorrelation_resolution, allsyst, success=True):
            """
            test that the systematic variations are the expected ones
            """
            tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
            tool.msg().setLevel(ROOT.MSG.WARNING)
            self.assertTrue(tool.setProperty("int")("useMVACalibration", 0).isSuccess())
            self.assertTrue(tool.setProperty("ESModel", model).isSuccess())
            if decorrelation is not None:
                self.assertTrue(tool.setProperty("decorrelationModel", decorrelation).isSuccess())
            if decorrelation_scale is not None:
                self.assertTrue(tool.setProperty("decorrelationModelScale", decorrelation_scale).isSuccess())
            if decorrelation_resolution is not None:
                self.assertTrue(tool.setProperty("decorrelationModelResolution", decorrelation_resolution).isSuccess())

            tool.msg().setLevel(ROOT.MSG.WARNING)
            if success:
                self.assertTrue(tool.initialize().isSuccess(), msg='cannot initialize tool with %s' % model)
            else:
                self.assertFalse(tool.initialize().isSuccess(), msg='should not possible to initialize tool with %s' % model)
                return
            sys_list = get_names_sys(tool)
            if type(allsyst) is int:
                self.assertEqual(len(sys_list), allsyst, msg='actual (expected) number of sys %d(%d): %s' % (len(sys_list), allsyst, sys_list))
            else:
                self.assertItemsEqual(sys_list, allsyst)
            return sys_list


        list_1NP_scale = ['EG_SCALE_ALL__1down', 'EG_SCALE_ALL__1up']
        list_1NP_resolution = ['EG_RESOLUTION_ALL__1down', 'EG_RESOLUTION_ALL__1up']
        list_FULL_resolution = ['EG_RESOLUTION_MATERIALCALO__1down', 'EG_RESOLUTION_MATERIALCALO__1up',
                                'EG_RESOLUTION_MATERIALCRYO__1down', 'EG_RESOLUTION_MATERIALCRYO__1up',
                                'EG_RESOLUTION_MATERIALGAP__1down', 'EG_RESOLUTION_MATERIALGAP__1up',
                                'EG_RESOLUTION_MATERIALID__1down', 'EG_RESOLUTION_MATERIALID__1up',
                                'EG_RESOLUTION_PILEUP__1down', 'EG_RESOLUTION_PILEUP__1up',
                                'EG_RESOLUTION_SAMPLINGTERM__1down', 'EG_RESOLUTION_SAMPLINGTERM__1up',
                                'EG_RESOLUTION_ZSMEARING__1down', 'EG_RESOLUTION_ZSMEARING__1up']
        list_1NPCOR_PLUS_UNCOR_scale = ['EG_SCALE_ALLCORR__1down', 'EG_SCALE_ALLCORR__1up',
                                        'EG_SCALE_E4SCINTILLATOR__1down', 'EG_SCALE_E4SCINTILLATOR__1up',
                                        'EG_SCALE_LARCALIB_EXTRA2015PRE__1down', 'EG_SCALE_LARCALIB_EXTRA2015PRE__1up',
                                        'EG_SCALE_LARTEMPERATURE_EXTRA2015PRE__1down', 'EG_SCALE_LARTEMPERATURE_EXTRA2015PRE__1up',
                                        'EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__1down', 'EG_SCALE_LARTEMPERATURE_EXTRA2016PRE__1up']
        _test_list_syst("es2015PRE", "1NP_v1", None, None, list_1NP_scale + list_1NP_resolution)
        _test_list_syst("es2012c", "1NP_v1", None, None, list_1NP_scale + list_1NP_resolution)
        _test_list_syst("es2016PRE", None, "1NP_v1", "1NP_v1", list_1NP_scale + list_1NP_resolution)
        _test_list_syst("es2016PRE", None, "1NP_v1", "FULL_v1", list_1NP_scale + list_FULL_resolution)
        _test_list_syst("es2015PRE", "1NPCOR_PLUS_UNCOR", None, None, list_1NP_resolution + list_1NPCOR_PLUS_UNCOR_scale)
        _test_list_syst("es2015PRE", "1NP_v1", "1NPCOR_PLUS_UNCOR", None, list_1NP_resolution + list_1NPCOR_PLUS_UNCOR_scale)
        _test_list_syst("es2015c_summer", "1NP_v1", None, None, list_1NP_scale + list_1NP_resolution)

        _test_list_syst("es2015PRE", "FULL_ETACORRELATED_v1", None, None, 58)
        _test_list_syst("es2012c", "FULL_ETACORRELATED_v1", None, None, 54)
        _test_list_syst("es2016PRE", "FULL_ETACORRELATED_v1", None, None, 62)
        _test_list_syst("es2015c_summer", "FULL_ETACORRELATED_v1", None, None, 60)
        _test_list_syst("es2016data_mc15c",  "FULL_ETACORRELATED_v1", None, None, 68)
        _test_list_syst("es2012c", "FULL_v1", None, None, 148)
        _test_list_syst("es2012c", None, "FULL_v1", "FULL_v1", 148)
        _test_list_syst("es2015PRE", "FULL_v1", None, None, 158)
        _test_list_syst("es2015PRE", None, "FULL_v1", "FULL_v1", 158)
        _test_list_syst("es2015PRE", None, None, None, 158)

        # these works, but generate FATALS, as expected
        _test_list_syst("es2016PRE", "1NP_v1", "1NP_v1", "1NP_v1", [], success=False)



    def test_same_smearing(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        self.assertTrue(tool.setProperty("ESModel", "es2012c").isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
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
        tool1.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool1.initialize().isSuccess())

        tool2 = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        tool2.setProperty("ESModel", "es2012c").ignore()
        tool2.setProperty("int")("doScaleCorrection", 0).ignore()
        tool2.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool2.initialize().isSuccess())

        ei = self.factory.create_eventinfo(False, 100000)  # data

        for ph in self.generator_photon():
            e1 = tool1.getEnergy(ph, ei)
            e1_bis = tool1.getEnergy(ph, ei)
            e2 = tool2.getEnergy(ph, ei)
            if abs(ph.eta()) < 2.47:  # TODO: move to 2.5
                self.assertNotEqual(e1, e2, msg="equal at eta = %f" % ph.eta())
            self.assertEqual(e1, e1_bis)

    def test_syst_bin(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
        self.assertTrue(tool.setProperty("ESModel", "es2012c").isSuccess())
        self.assertTrue(tool.setProperty("int")("doSmearing", 0).isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
        self.assertTrue(tool.initialize().isSuccess())

        ei = self.factory.create_eventinfo(True, 100000)   # simulation

        for ph in self.generator_photon():
            tool.applySystematicVariation(ROOT.CP.SystematicSet()).ignore()
            e_nom = tool.getEnergy(ph, ei)
            sys_set = ROOT.CP.SystematicSet()
            sys_set.insert(ROOT.CP.SystematicVariation("EG_SCALE_ZEESYST", 1.))
            tool.applySystematicVariation(sys_set).ignore()
            e_zee_syst = tool.getEnergy(ph, ei)
            sys_set = ROOT.CP.SystematicSet()
            sys_set.insert(ROOT.CP.SystematicVariation("EG_SCALE_LARCALIB__ETABIN0", 1.))
            tool.applySystematicVariation(sys_set).ignore()
            e_larcalib__bin0 = tool.getEnergy(ph, ei)
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
        self.assertTrue(tool.setProperty("ESModel", "es2012c").isSuccess())
        tool_no_correction = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_no_correction")
        tool_no_correction.setProperty("ESModel", "es2012c").ignore()
        tool_no_correction.setProperty("int")("useIntermoduleCorrection", 0).ignore()

        self.assertTrue(tool.initialize().isSuccess())
        self.assertTrue(tool_no_correction.initialize().isSuccess())

        energy, energy_no_correction = [], []

        ei = self.factory.create_eventinfo(False, 100000)  # data
        phi_range = linspace(-math.pi, math.pi, 400)
        for ph in self.generator_photon([0.1], [100E3], phi_range):
            energy.append(tool.getEnergy(ph, ei))
            energy_no_correction.append(tool_no_correction.getEnergy(ph, ei))

        self.assertFalse(all([x == y for x, y in zip(energy, energy_no_correction)]))

    def test_idempotence(self):
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        self.assertTrue(tool.setProperty("ESModel", "es2015PRE").isSuccess())
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

    def test_1NP_vs_FULL_es2017(self):
        """ check that the 1NP model is the squared sum of the single systematics """
        tool_1NP = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2016data_mc15c_1NP")
        tool_1NP.setProperty("ESModel", "es2016data_mc15c").ignore()
        tool_1NP.setProperty("decorrelationModel", "1NP_v1").ignore()
        tool_1NP.setProperty("int")("randomRunNumber", RUN2015).ignore()
        #tool_1NP.setProperty("int")("doSmearing", 0).ignore()   # remove
        #tool_1NP.msg().setLevel(ROOT.MSG.DEBUG)
        
        tool_1NP.initialize().ignore()

        tool_FULL = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2016data_mc15c_FULL")
        tool_FULL.setProperty("ESModel", "es2016data_mc15c").ignore()
        tool_FULL.setProperty("int")("randomRunNumber", RUN2015).ignore()
        # use ETACORRELATED to compare. FULL_v1 will differ (very small difference) since by default
        # FULL_v1 divide the ZEESTAT by the sqrt(#bins)
        tool_FULL.setProperty("decorrelationModel", "FULL_ETACORRELATED_v1").ignore()
        #tool_FULL.setProperty("int")("doSmearing", 0).ignore()    # remove
        #tool_FULL.msg().setLevel(ROOT.MSG.DEBUG)
        tool_FULL.initialize().ignore()

        ei = self.factory.create_eventinfo(True, 100000)  # MC
        for ptype, generator in self.generators().iteritems():
            for particle in generator:
                sys_set = ROOT.CP.SystematicSet()
                tool_FULL.applySystematicVariation(sys_set).ignore()
                tool_1NP.applySystematicVariation(sys_set).ignore()
                e_full = tool_FULL.getEnergy(particle, ei)
                e_1NP = tool_1NP.getEnergy(particle, ei)

                self.assertAlmostEqual(e_full, e_1NP, msg="energy calibrated with different correlation schemes"
                                       " are different. eta = %.2f, energy with full = %.2f"
                                       " energy with 1NP = %.2f" % (particle.eta(), e_full, e_1NP))
                all_syst_FULL = tool_FULL.recommendedSystematics()
                all_syst_1NP = tool_1NP.recommendedSystematics()

                self.assertTrue('EG_SCALE_ALL__1up' in [s.name() for s in list(all_syst_1NP)])

                all_syst_FULL_scale_up = [s for s in all_syst_FULL if ('SCALE' in s.name() and '__1up' in s.name())]

                all_biases = []
                for sys in all_syst_FULL_scale_up:
                    sys_set = ROOT.CP.SystematicSet()
                    sys_set.insert(sys)
                    tool_FULL.applySystematicVariation(sys_set).ignore()
                    e = tool_FULL.getEnergy(particle, ei)
                    bias = e - e_full
                    #print "{:<40} {:10.5f} {:10.5f}".format(sys.name(), e, bias)
                    all_biases.append(bias)

                sum_quadrature = math.sqrt(sum([b * b for b in all_biases]))
                #print "bias sum quadrature: ", sum_quadrature


                bias_up, bias_down = None, None
                for sys in all_syst_1NP:
                    if 'RESOLUTION' in sys.name():
                        continue
                    sys_set = ROOT.CP.SystematicSet()
                    sys_set.insert(sys)
                    tool_1NP.applySystematicVariation(sys_set).ignore()
                    e = tool_1NP.getEnergy(particle, ei)
                    bias = e - e_full
                    if sys.name() == 'EG_SCALE_ALL__1up':
                        bias_up = bias
                    elif sys.name() == 'EG_SCALE_ALL__1down':
                        bias_down = bias
                    #print sys.name() + "\t" + str(bias)


                self.assertAlmostEqual(sum_quadrature, bias_up, delta=1, msg="sum of errors not equal to 1NP (up) %.7f!=%.7f" % (sum_quadrature, bias_up))
                # TODO: check why fails
                #self.assertAlmostEqual(sum_quadrature, -bias_down, delta=1, msg="sum of errors not equal to 1NP (down) %.7f!=%.7f" % (sum_quadrature, bias_down))

    def test_1NP_100GeV_electron_es2015PRE(self):
        """
        check that data and MC energy are diffferent
        check that systematics 1NP variations are != 0
        """
        tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        self.assertTrue(tool.setProperty("ESModel", "es2015PRE").isSuccess())
        self.assertTrue(tool.setProperty("decorrelationModel", "1NP_v1").isSuccess())
        tool.msg().setLevel(ROOT.MSG.WARNING)
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
        self.factory.clear()

    def test_es2015cPRE(self):
        """
        check that es2015cPRE == es2015PRE for electron except for crack region [1.4-1.6]
        check that es2015cPRE == es2015PRE for photons
        """
        tool_es2015PRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015PRE")
        tool_es2015PRE.setProperty("ESModel", "es2015PRE").ignore()
        tool_es2015PRE.setProperty("decorrelationModel", "1NP_v1").ignore()
        tool_es2015PRE.setProperty("int")("doSmearing", 0).ignore()
        tool_es2015PRE.initialize().ignore()

        tool_es2015cPRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015cPRE")
        tool_es2015cPRE.setProperty("ESModel", "es2015cPRE").ignore()
        tool_es2015cPRE.setProperty("decorrelationModel", "1NP_v1").ignore()
        tool_es2015cPRE.setProperty("int")("doSmearing", 0).ignore()
        tool_es2015cPRE.initialize().ignore()

        ei = self.factory.create_eventinfo(True, 100000)  # MC
        for el in self.generator_electron(eta_range=arange(-2.46, 2.52, 0.11)):
            # need to use strange range since there is problem for round numbers
            break  # TODO: REMOVE
            e_es2015PRE = tool_es2015PRE.getEnergy(el, ei)
            e_es2015cPRE = tool_es2015cPRE.getEnergy(el, ei)

            #print el.eta(), el.e(), e_es2015PRE, e_es2015cPRE
            if 1.4 < abs(el.eta()) < 1.6:
                self.assertNotAlmostEqual(e_es2015PRE, e_es2015cPRE)
            else:
                self.assertAlmostEqual(e_es2015PRE, e_es2015cPRE)

        for ph in self.generator_photon():

            e_es2015PRE = tool_es2015PRE.getEnergy(ph, ei)
            e_es2015cPRE = tool_es2015cPRE.getEnergy(ph, ei)
            self.assertAlmostEqual(e_es2015PRE, e_es2015cPRE,
                                   msg="energies are not the same for photots"
                                       " eta=%.2f phi=%.2f" % (ph.eta(), ph.phi()))

    def test_es2015c_summer(self):
        """
        check that es2015c_summer == es2015cPRE for electrons
                                  !=            for photons
        """
        tool_es2015c_summer = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015c_summer")
        tool_es2015c_summer.setProperty("ESModel", "es2015c_summer").ignore()
        tool_es2015c_summer.setProperty("decorrelationModel", "1NPCOR_PLUS_UNCOR").ignore()
        tool_es2015c_summer.setProperty("int")("doSmearing", 0).ignore()
        tool_es2015c_summer.msg().setLevel(ROOT.MSG.WARNING)
        tool_es2015c_summer.initialize().ignore()

        tool_es2015cPRE = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015cPRE")
        tool_es2015cPRE.setProperty("ESModel", "es2015cPRE").ignore()
        tool_es2015cPRE.setProperty("decorrelationModel", "1NPCOR_PLUS_UNCOR").ignore()
        tool_es2015cPRE.setProperty("int")("doSmearing", 0).ignore()
        tool_es2015cPRE.msg().setLevel(ROOT.MSG.WARNING)
        tool_es2015cPRE.initialize().ignore()

        ei = self.factory.create_eventinfo(True, 100000)  # MC

        for el in self.generator_electron(eta_range=arange(-2.52, 2.52, 0.11)):
            e_es2015c_summer = tool_es2015c_summer.getEnergy(el, ei)
            self.assertGreater(e_es2015c_summer, 0)
            e_es2015cPRE = tool_es2015cPRE.getEnergy(el, ei)
            self.assertGreater(e_es2015cPRE, 0)

            self.assertAlmostEqual(e_es2015c_summer, e_es2015cPRE,
                                   msg="e_es2015c_summer != e_es2015cPRE at eta=%.2f for electrons" % el.eta())

        for ph in self.generator_photon():
            e_es2015c_summer = tool_es2015c_summer.getEnergy(ph, ei)
            self.assertGreater(e_es2015c_summer, 0)
            e_es2015cPRE = tool_es2015cPRE.getEnergy(ph, ei)
            self.assertGreater(e_es2015cPRE, 0)
            self.assertNotAlmostEqual(e_es2015c_summer, e_es2015cPRE,
                                      msg="e_es2015c_summer == e_es2015cPRE == %.2f at eta=%.2f for photons" % (e_es2015c_summer, ph.eta()))

    def test_es2015c_summer_data(self):
        """
        check that energy > 0
        """
        tool_es2015c_summer = ROOT.CP.EgammaCalibrationAndSmearingTool("tool_es2015c_summer")
        tool_es2015c_summer.setProperty("ESModel", "es2015c_summer").ignore()
        tool_es2015c_summer.setProperty("decorrelationModel", "1NPCOR_PLUS_UNCOR").ignore()
        tool_es2015c_summer.msg().setLevel(ROOT.MSG.WARNING)
        tool_es2015c_summer.initialize().ignore()

        ei = self.factory.create_eventinfo(False, 100000)  # data

        for el in self.generator_electron():
            e_es2015c_summer = tool_es2015c_summer.getEnergy(el, ei)
            self.assertGreater(e_es2015c_summer, 0)
        for ph in self.generator_photon():
            e_es2015c_summer = tool_es2015c_summer.getEnergy(ph, ei)
            self.assertGreater(e_es2015c_summer, 0)


if __name__ == '__main__':
    ROOT.PyConfig.IgnoreCommandLineOptions = True
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
#    from ROOT import EgammaCalibPeriodRunNumbersExample

    #ROOT.xAOD.TReturnCode.enableFailure()
    unittest.main()
