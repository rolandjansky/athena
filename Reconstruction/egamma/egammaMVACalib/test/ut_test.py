#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import unittest
import ROOT


def arange(xmin, xmax, delta):
    # just to don't inject dep from numpy
    x = xmin
    while True:
        yield x
        x += delta
        if x >= xmax:
            break


def xAOD_electron_generator(tree):
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        for i in xrange(tree.Electrons.size()):
            el = tree.Electrons.at(i)
            yield el


def xAOD_photon_generator(tree):
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        for i in xrange(tree.Photons.size()):
            el = tree.Photons.at(i)
            yield el


def is_file_readable(path):
    f = ROOT.TFile.Open(path)
    return bool(f)


class TestEgammaMVACalib(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.particle_tree_electron = cls.ParticleTree(("el_rawcl_Es0", "el_rawcl_Es1", "el_rawcl_Es2", "el_rawcl_Es3",
                                                       "el_cl_eta", "el_cl_E", "el_cl_etaCalo", "el_cl_phiCalo"))
        cls.particle_tree_photon = cls.ParticleTree(("ph_rawcl_Es0", "ph_rawcl_Es1", "ph_rawcl_Es2", "ph_rawcl_Es3",
                                                     "ph_cl_eta", "ph_cl_E", "ph_cl_etaCalo", "ph_cl_phiCalo",
                                                     "ph_ptconv", "ph_pt1conv", "ph_pt2conv", "ph_convtrk1nPixHits",
                                                     "ph_convtrk1nSCTHits", "ph_convtrk2nPixHits",
                                                     "ph_convtrk2nSCTHits", "ph_Rconv"))

        cls.photon_tool_v1 = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/v1")
        cls.electron_tool_v1 = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/v1")
        cls.photon_tool_v3 = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/offline/v3")
        cls.electron_tool_v3 = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/offline/v3")
        cls.photon_tool_v3_online = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/online/v3")
        cls.electron_tool_v3_online = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/online/v3")

        cls.tools_electron = (cls.electron_tool_v1, cls.electron_tool_v3, cls.electron_tool_v3_online)
        cls.tools_photon = (cls.photon_tool_v1, cls.photon_tool_v3, cls.photon_tool_v3_online)

        for t in cls.tools_electron:
            t.InitTree(cls.particle_tree_electron.get_tree())
        for t in cls.tools_photon:
            t.InitTree(cls.particle_tree_photon.get_tree())


    class ParticleTree(object):
        def __init__(self, variable_names):
            from array import array
            self.tree = ROOT.TTree()
            self.variable_names = variable_names
            self.data = []
            for var_name in self.variable_names:
                d = array('f', [0])
                self.data.append(d)
                self.tree.Branch(var_name, d, "%s/F" % var_name)

        def update(self, vs):
            if len(vs) != len(self.data):
                raise ValueError("data and variable list must have the same length")
            for d, v in zip(self.data, vs):
                d[0] = v

        def get_tree(self):
            return self.tree

    def test_energy_v1_weights(self):
        """
        test for some fixed case if using run1 weights (v1) the tool reproduces
        the same outputs +/- 1 MeV
        """
        electron_tool = self.electron_tool_v1

        # first example
        example_inputs = (2943.845703125, 20473.12109375, 22390.435546875, 275.47125244140625,
                          0.5844721794128418, 48970.80859375, 0.5835072994232178, 2.843465566635132)
        self.particle_tree_electron.update(example_inputs)
        self.assertAlmostEqual(electron_tool.getMVAEnergy(), 49346.90625, delta=1)

        photon_tool = self.photon_tool_v1

        example_inputs = (12222.08, 49425.33, 89170.18, 655.61, 1.663827, 160612.73, 1.6522,
                          2.060981035232544, 71608.8984375, 49311.08984375, 22297.919921875,
                          2, 10, 0, 10, 117.15968322753906)
        self.particle_tree_photon.update(example_inputs)
        self.assertAlmostEqual(photon_tool.getMVAEnergy(), 169953.875, delta=1.)

    def do_test_file(self, tool, tree):
        assert tree
        tool.InitTree(tree)
        self.assertGreater(tree.GetEntries(), 0)
        for i in xrange(tree.GetEntries()):
            tree.GetEntry(i)
            output = tool.getMVAEnergy()
            expected_output = tree.exp_output_v1
            self.assertAlmostEqual(output, expected_output, delta=1.)

    def test_extensive_v1_electron(self):
        """
        test the expected output looping on a local file with a simple TTree. Electron, v1
        """
        electron_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/v1")
        f = ROOT.TFile.Open("electron_test.root")
        tree = f.Get("electron_test")
        self.do_test_file(electron_tool, tree)

    def test_extensive_v1_photon(self):
        """
        test the expected output looping on a local file with a simple TTree. Photons, v1
        """
        photon_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/v1")
        f = ROOT.TFile.Open("photon_test.root")
        tree = f.Get("photon_test")
        self.do_test_file(photon_tool, tree)

    def generator_test_input(self, filename):
        with open(filename) as f:
            for line in f:
                left, right = line.split("->")
                left = left.rstrip(' ]').lstrip(' [')
                right = right.strip()

                expected_output = float(right)
                left = left.split(',')
                example_input = [float(r) if "." in r else int(r) for r in left]
                yield example_input, expected_output

    def test_chain_electron(self):
        filename = "electron_test.root"
        f = ROOT.TFile.Open(filename)
        tree = f.Get(filename.split('.')[0])
        assert(tree)

        nevent_per_file = 2
        for i in xrange(tree.GetEntries() / nevent_per_file):
            temp_filename = "tmp_electron_%d.root" % i
            temp_file = ROOT.TFile(temp_filename, "recreate")
            newtree = tree.CopyTree("1", "", nevent_per_file, i * nevent_per_file)
            newtree.SetName("electron_test")
            newtree.Write()
            temp_file.Close()
        chain = ROOT.TChain("electron_test", "electron_test")
        chain.Add("tmp_electron*.root")
        electron_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON, True, "egammaMVACalib/v1")
        electron_tool.InitTree(chain)
        for ientry in xrange(chain.GetEntries()):
            chain.GetEntry(ientry)
            expected_result = chain.exp_output_v1
            result = electron_tool.getMVAEnergy()
            self.assertAlmostEqual(expected_result, result)
        self.assertGreater(ientry, 20)
        import glob
        import os
        for fl in glob.glob("tmp_electron*.root"):
            os.remove(fl)

    def test_chain_photon(self):
        filename = "photon_test.root"
        f = ROOT.TFile.Open(filename)
        tree = f.Get(filename.split('.')[0])
        assert(tree)

        nevent_per_file = 2
        for i in xrange(tree.GetEntries() / nevent_per_file):
            temp_filename = "tmp_photon_%d.root" % i
            temp_file = ROOT.TFile(temp_filename, "recreate")
            newtree = tree.CopyTree("1", "", nevent_per_file, i * nevent_per_file)
            newtree.SetName("photon_test")
            newtree.Write()
            temp_file.Close()
        chain = ROOT.TChain("photon_test", "photon_test")
        chain.Add("tmp_photon*.root")
        photon_tool = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON, True, "egammaMVACalib/v1")
        photon_tool.InitTree(chain)
        for ientry in xrange(chain.GetEntries()):
            chain.GetEntry(ientry)
            expected_result = chain.exp_output_v1
            result = photon_tool.getMVAEnergy()
            self.assertAlmostEqual(expected_result, result)
        self.assertGreater(ientry, 20)
        import glob
        import os
        for fl in glob.glob("tmp_photon*.root"):
            os.remove(fl)

    def test_coverage(self):
        for tool in self.tools_electron:
            tool.useClusterIf0(False)
        for tool in self.tools_photon:
            tool.useClusterIf0(False)

        for rconv in (0, 117):
            for eta in arange(-3, 3, 0.01):
                inputs = (12222.08, 49425.33, 89170.18, 655.61, eta, 160612.73, eta,
                          2.060981035232544, 71608.8984375, 49311.08984375, 22297.919921875,
                          2, 10, 0, 10, rconv)
                self.particle_tree_photon.update(inputs)
                mva_energy_v1 = self.photon_tool_v1.getMVAEnergy()
                mva_energy_v3 = self.photon_tool_v3.getMVAEnergy()
                mva_energy_v3_trigger = self.photon_tool_v3_online.getMVAEnergy()
                is_crack = 1.370001 < abs(eta) < 1.5199999
                is_forward = abs(eta) > 2.4700001 and abs(eta) < 2.5000001
                is_out = abs(eta) >= 2.5000001
                if is_crack:
                    self.assertEqual(mva_energy_v1, 0)
                    self.assertGreater(mva_energy_v3, 0)
                    if rconv == 0:
                        self.assertGreater(mva_energy_v3_trigger, 0)
                elif is_forward:
                    self.assertEqual(mva_energy_v1, 0)
                    self.assertGreater(mva_energy_v3, 0)
                    if rconv == 0:
                        self.assertGreater(mva_energy_v3_trigger, 0)
                elif is_out:
                    self.assertEqual(mva_energy_v1, 0)
                    self.assertEqual(mva_energy_v3, 0)
                    if rconv == 0:
                        self.assertEqual(mva_energy_v3_trigger, 0)
                else:
                    self.assertGreater(mva_energy_v1, 0)
                    self.assertGreater(mva_energy_v3, 0)
                    if rconv == 0:
                        self.assertGreater(mva_energy_v3_trigger, 0)

        for eta in arange(-3, 3, 0.01):
            inputs = (2943.845703125, 20473.12109375, 22390.435546875, 275.47125244140625,
                      eta, 48970.80859375, eta, 2.843465566635132)
            self.particle_tree_electron.update(inputs)
            mva_energy_v1 = self.electron_tool_v1.getMVAEnergy()
            mva_energy_v3 = self.electron_tool_v3.getMVAEnergy()
            mva_energy_v3_trigger = self.electron_tool_v3_online.getMVAEnergy()

            is_crack = 1.370001 < abs(eta) < 1.5199999
            is_forward = abs(eta) > 2.4700001 and abs(eta) < 2.5000001
            is_out = abs(eta) >= 2.5000001
            if is_crack:
                self.assertEqual(mva_energy_v1, 0)
                self.assertGreater(mva_energy_v3, 0)
                self.assertGreater(mva_energy_v3_trigger, 0)
            elif is_forward:
                self.assertEqual(mva_energy_v1, 0)
                self.assertGreater(mva_energy_v3, 0)
                self.assertGreater(mva_energy_v3_trigger, 0)
            elif is_out:
                self.assertEqual(mva_energy_v1, 0)
                self.assertEqual(mva_energy_v3, 0)
                self.assertEqual(mva_energy_v3_trigger, 0)
            else:
                self.assertGreater(mva_energy_v1, 0)
                self.assertGreater(mva_energy_v3, 0)
                self.assertGreater(mva_energy_v3_trigger, 0)

        for tool in self.tools_electron:
            tool.useClusterIf0(True)
        for tool in self.tools_photon:
            tool.useClusterIf0(True)

    def test_tool_initialization(self):
        tool = ROOT.egammaMVATool('the_tool')
        self.assertTrue(tool)


@unittest.skipIf(not is_file_readable('root://eosatlas.cern.ch//eos/atlas/user/t/turra/user.blenzi.4956574.EXT0._000001.AOD.pool.root'),
                 "input file not available")
class TestEgammaMVATool(unittest.TestCase):
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

        cls.xAOD_tool = ROOT.egammaMVATool("egammaMVATool")
        cls.xAOD_tool.setProperty("folder", "egammaMVACalib/v1")
        cls.xAOD_tool.initialize()

    @classmethod
    def tearDownClass(cls):
        ROOT.xAOD.ClearTransientTrees()

    ## TODO: recreate file since numbers changed after https://its.cern.ch/jira/browse/ATLASRECTS-2911
    def testElectrons(self):
        N = 200
        for i, el in enumerate(xAOD_electron_generator(self.tree)):
            old_energy = el.e()
            new_energy = self.xAOD_tool.getEnergy(el.caloCluster(), el)
            self.assertAlmostEqual(old_energy, new_energy, delta=10, msg="energy different than expected %.2f != %.2f at eta=%.2f" % (old_energy, new_energy, el.eta()))
            if i >= N:
                break
        print "tested %d electrons" % i
        self.assertGreater(i, 10, msg="too few electrons")


    def atestPhotons(self):
        N = 200
        for i, ph in enumerate(xAOD_photon_generator(self.tree)):
            if ph.vertex():
                continue
            old_energy = ph.e()
            new_energy = self.xAOD_tool.getEnergy(ph.caloCluster(), ph)
            print old_energy, new_energy, ph.eta()
            self.assertAlmostEqual(old_energy, new_energy, delta=1)
            if i >= N:
                break
        print "tested %d photons" % i
        self.assertGreater(i, 10, msg="too few photons")


@unittest.skipIf(not is_file_readable('root://eosatlas.cern.ch//eos/atlas/user/t/turra/user.blenzi.4956574.EXT0._000001.AOD.pool.root'),
                 "input file not available")
class TestEgammaMVATrigger(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        filename = 'root://eosatlas.cern.ch//eos/atlas/user/t/turra/user.blenzi.4956574.EXT0._000001.AOD.pool.root'
        if not ROOT.xAOD.Init().isSuccess():
            print "Failed xAOD.Init()"
        treeName = "CollectionTree"

        f = ROOT.TFile.Open(filename)
        if not f:
            print "ERROR: problem opening eos file"
        cls.tree = ROOT.xAOD.MakeTransientTree(f, treeName)

    @classmethod
    def tearDownClass(cls):
        ROOT.xAOD.ClearTransientTrees()

    def testElectron(self):
        """ use the HLT interface to apply electron offline calibration """
        xAOD_tool = ROOT.egammaMVATool("egammaMVATool")
        xAOD_tool.setProperty("folder", "egammaMVACalib/online/v3")
        xAOD_tool.setProperty("use_layer_corrected", False)
        xAOD_tool.initialize()
        for i, el in enumerate(xAOD_electron_generator(self.tree)):
            self.assertAlmostEqual(xAOD_tool.getEnergy(el.caloCluster(), "Electron"),
                                   xAOD_tool.getEnergy(el.caloCluster(), el))
        self.assertGreater(i, 100, msg="not enough electrons")

    def testPhoton(self):
        """ test HLT interface to apply photon trigger calibration positive """
        xAOD_tool = ROOT.egammaMVATool("egammaMVATool")
        xAOD_tool.setProperty("folder", "egammaMVACalib/online/v3")
        xAOD_tool.setProperty("use_layer_corrected", False)
        xAOD_tool.initialize()

        for i, ph in enumerate(xAOD_photon_generator(self.tree)):
            trigger_energy = xAOD_tool.getEnergy(ph.caloCluster(), "Photon")
            self.assertGreater(trigger_energy, 0., msg="energy is negative item=%d, eta=%f, e=%f" % (i, ph.eta(), ph.e()))


if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    unittest.main()
