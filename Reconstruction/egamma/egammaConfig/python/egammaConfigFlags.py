# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this is based on MuonConfigFlags as a guide

import unittest
from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def createEgammaConfigFlags():
    egcf = AthConfigFlags()

    # do standard cluster-based egamma algorithm
    egcf.addFlag("Egamma.doCentral",
                 lambda prevFlags: prevFlags.Detector.EnableCalo)

    # do forward egamma
    egcf.addFlag("Egamma.doForward",
                 lambda prevFlags: prevFlags.Detector.EnableCalo)

    # run the GSF refitting/egamma Tracking it is calo seeded
    egcf.addFlag("Egamma.doTracking",
                 lambda prevFlags: (
                     prevFlags.Detector.EnableID
                     or prevFlags.Detector.EnableITk)
                 and prevFlags.Detector.EnableCalo)

    # build photon conversion vertices
    egcf.addFlag("Egamma.doConversionBuilding",
                 lambda prevFlags: prevFlags.Egamma.doTracking)

    # do egamma truth association when running on MC
    egcf.addFlag("Egamma.doTruthAssociation",
                 lambda prevFlags: prevFlags.Input.isMC)

    # Do e/gamma track thinning (Although we call the alg slimming...)
    egcf.addFlag("Egamma.doTrackThinning",
                 lambda prevFlags: prevFlags.Output.doWriteAOD and
                 prevFlags.Egamma.doTracking)

    # Keep egamma Cells in AOD
    egcf.addFlag("Egamma.keepCaloCellsAOD",
                 lambda prevFlags: prevFlags.Egamma.doCentral or
                 prevFlags.Egamma.doForward)

    # The cluster corrections/calib
    egcf.addFlag("Egamma.Calib.ClusterCorrectionVersion",
                 'v12phiflip_noecorrnogap')
    egcf.addFlag("Egamma.Calib.SuperClusterCorrectionVersion",
                 'v12phiflip_supercluster')
    egcf.addFlag("Egamma.Calib.MVAVersion", 'egammaMVACalib/offline/v7')

    # The output keys
    egcf.addFlag("Egamma.Keys.Input.CaloCells", 'AllCalo')
    egcf.addFlag("Egamma.Keys.Input.TopoClusters",
                 'CaloTopoClusters')  # input topoclusters
    egcf.addFlag("Egamma.Keys.Input.TruthParticles", 'TruthParticles')
    egcf.addFlag("Egamma.Keys.Input.TruthEvents", 'TruthEvents')
    egcf.addFlag("Egamma.Keys.Input.TrackParticles",
                 'InDetTrackParticles')  # input to GSF

    # the topoclusters selected for egamma from the input topoclusters
    egcf.addFlag("Egamma.Keys.Internal.EgammaTopoClusters",
                 'egammaTopoClusters')
    egcf.addFlag("Egamma.Keys.Internal.EgammaRecs", 'egammaRecCollection')
    egcf.addFlag("Egamma.Keys.Internal.PhotonSuperRecs",
                 'PhotonSuperRecCollection')
    egcf.addFlag("Egamma.Keys.Internal.ElectronSuperRecs",
                 'ElectronSuperRecCollection')

    egcf.addFlag("Egamma.Keys.Output.ConversionVertices",
                 'GSFConversionVertices')
    egcf.addFlag("Egamma.Keys.Output.ConversionVerticesSuppESD",
                 '-vxTrackAtVertex')
    egcf.addFlag("Egamma.Keys.Output.ConversionVerticesSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.ConversionVerticesSuppESD))

    egcf.addFlag("Egamma.Keys.Output.CaloClusters", 'egammaClusters')
    egcf.addFlag("Egamma.Keys.Output.CaloClustersSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.CaloClustersSuppAOD", '')

    egcf.addFlag("Egamma.Keys.Output.EgammaSuppAOD",
                 '-e033.-e011.-e333.-e335.-e337.-e377.'
                 '-isEMLoose.-isEMTight.'
                 '-ptconeCorrBitset.-ptconecoreTrackPtrCorrection.'
                 '-topoetconeCorrBitset')

    egcf.addFlag("Egamma.Keys.Output.Electrons", 'Electrons')
    egcf.addFlag("Egamma.Keys.Output.ElectronsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.ElectronsSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.ElectronsSuppESD +
                     prevFlags.Egamma.Keys.Output.EgammaSuppAOD + '.'
                     "-EgammaCovarianceMatrix."
                     "-isEMLHLoose.-isEMLHTight.-isEMLHMedium.-isEMMedium"))

    egcf.addFlag("Egamma.Keys.Input.ForwardTopoClusters",
                 'CaloCalTopoClusters')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectrons", 'ForwardElectrons')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectronsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectronsSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.ForwardElectronsSuppESD +
                     '-isEMTight.-isEMMedium.-isEMLoose'))

    egcf.addFlag("Egamma.Keys.Output.ForwardClusters",
                 'ForwardElectronClusters')
    egcf.addFlag("Egamma.Keys.Output.ForwardClustersSuppESD", '-SisterCluster')
    egcf.addFlag("Egamma.Keys.Output.ForwardClustersSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.ForwardClustersSuppESD))

    egcf.addFlag("Egamma.Keys.Output.Photons", 'Photons')
    egcf.addFlag("Egamma.Keys.Output.PhotonsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.PhotonsSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.PhotonsSuppESD +
                     prevFlags.Egamma.Keys.Output.EgammaSuppAOD + '.'
                     '-ptvarcone20.-ptvarcone30'))

    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticles", 'GSFTrackParticles')
    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticlesSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticlesSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.GSFTrackParticlesSuppESD))

    egcf.addFlag("Egamma.Keys.Output.TruthParticles", 'egammaTruthParticles')
    egcf.addFlag("Egamma.Keys.Output.TruthParticlesSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.TruthParticlesSuppAOD",
                 lambda prevFlags: (
                     prevFlags.Egamma.Keys.Output.TruthParticlesSuppESD))

    # not xAOD
    egcf.addFlag("Egamma.Keys.Output.GSFTracks", 'GSFTracks')

    # These are the clusters that are used to determine
    # which cells to write out to AOD
    # don't define SuppAOD because the whole container is suppressed
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeClusters", 'egamma711Clusters')
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeClustersSuppESD", '')

    egcf.addFlag("Egamma.Keys.Output.EgammaLargeFWDClusters",
                 'egamma66FWDClusters')
    egcf.addFlag("Egamma.Keys.Output.EgammaLargeFWDClustersSuppESD", '')

    return egcf


# self test


class TestEgammaConfigFlags(unittest.TestCase):

    def runTest(self):
        flags = createEgammaConfigFlags()
        self.assertEqual(flags.Egamma.Keys.Output.Photons, "Photons")
        self.assertEqual(flags._get("Egamma.Keys.Output.Photons"), "Photons")


if __name__ == "__main__":

    unittest.main()
