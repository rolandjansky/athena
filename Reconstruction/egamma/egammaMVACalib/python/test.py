# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT, user
ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C+");

print "\n *** Creating instance for photons ***"
mPh = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egPHOTON)
mPh.InitTree(0)

Etrue, Ecluster, Emva = 163944.71875, 160612.73, 169953.875
inputs = [12222.08, 49425.33, 89170.18, 655.61, 1.663827, 160612.73, 1.6522,
  2.060981035232544, 71608.8984375, 49311.08984375, 22297.919921875,
  2, 10, 0, 10, 117.15968322753906]

print "%-20s: %.4f GeV" % ("True energy", Etrue/1e3)
print "%-20s: %.4f GeV" % ("Cluster energy", Ecluster/1e3)
print "%-20s: %.4f GeV" % ("MVA energy (v10)", Emva/1e3)
print "%-20s: %.4f GeV" % ("MVA energy", mPh.getMVAEnergyPhoton(*inputs)/1e3)

#-----------------

print "\n *** Creating instance for electrons ***"
mEl = ROOT.egammaMVACalib(ROOT.egammaMVACalib.egELECTRON)
mEl.InitTree(0)

Etrue, Ecluster, Emva = 51110.449624549736, 48970.80859375, 49346.90625
inputs = [2943.845703125, 20473.12109375, 22390.435546875, 275.47125244140625,
  0.5844721794128418, 48970.80859375, 0.5835072994232178, 2.843465566635132]

print "%-20s: %.4f GeV" % ("True energy", Etrue/1e3)
print "%-20s: %.4f GeV" % ("Cluster energy", Ecluster/1e3)
print "%-20s: %.4f GeV" % ("MVA energy (v10)", Emva/1e3)
print "%-20s: %.4f GeV" % ("MVA energy", mEl.getMVAEnergyElectron(*inputs)/1e3)

