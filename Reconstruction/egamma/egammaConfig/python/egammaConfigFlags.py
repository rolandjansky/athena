# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# this is based on MuonConfigFlags as a guide

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createEgammaConfigFlags(): 
    egcf=AthConfigFlags()
 
    # enable/disable the full egamma
    egcf.addFlag("Egamma.enabled",True)

    # do standard cluster-based egamma algorithm
    egcf.addFlag("Egamma.doCaloSeeded",True)
    egcf.addFlag("Egamma.doSuperclusters",True) # if true, do superculsers, false is SW
    egcf.addFlag("Egamma.doTopoSeeded",True) # if doing SW, also add toposeeded electrons

    # do forward egamma
    egcf.addFlag("Egamma.doForwardSeeded",True)

    # do egamma truth association when running on MC
    egcf.addFlag("Egamma.doTruthAssociation",True)

    # run the GSF refitting
    egcf.addFlag("Egamma.doGSF",True) 

    # build conversion vertices
    egcf.addFlag("Egamma.doConversionBuilding",True) 

    # The cluster corrections/calib
    egcf.addFlag("Egamma.Calib.ClusterCorrectionVersion", 'v12phiflip_noecorrnogap')
    egcf.addFlag("Egamma.Calib.MVAVersion", 'egammaMVACalib/offline/v7')
   
    ##################################################
    # The keys: should update to use file peeking info
    # Also not sure what's the best way to do this, might want to revisit
    # one idea is to make the keys have tuples with type, name, etc
    ##################################################

    egcf.addFlag("Egamma.Keys.Input.Cells","AllCalo") #should make a lambda
    egcf.addFlag("Egamma.Keys.Input.TopoClusters",'CaloTopoCluster') #input topoclusters
    egcf.addFlag("Egamma.Keys.Input.TruthParticles", 'TruthParticles')
    egcf.addFlag("Egamma.Keys.Input.TruthEvents", 'TruthEvents')

    # the topoclusters selected for egamma from the input topoclusters
    egcf.addFlag("Egamma.Keys.Internal.EgammaTopoClusters",'egammaTopoClusters') 
    egcf.addFlag("Egamma.Keys.Internal.EgammaRecs", 'egammaRecCollection')
    egcf.addFlag("Egamma.Keys.Internal.PhotonSuperRecs", 'PhotonSuperRecCollection')
    egcf.addFlag("Egamma.Keys.Internal.ElectronSuperRecs", 'ElectronSuperRecCollection')
 
    egcf.addFlag("Egamma.Keys.Output.ConversionVertices", 'GSFConversionVertices')
    egcf.addFlag("Egamma.Keys.Output.ConversionVerticesSuppESD", '-vxTrackAtVertex')
    egcf.addFlag("Egamma.Keys.Output.ConversionVerticesSuppAOD", '-vxTrackAtVertex')

    egcf.addFlag("Egamma.Keys.Output.CaloClusters", 'egammaClusters')
    egcf.addFlag("Egamma.Keys.Output.CaloClustersSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.CaloClustersSuppAOD", '')

    egcf.addFlag("Egamma.Keys.Output.TopoSeededClusters", 'egammaTopoSeededClusters')
    egcf.addFlag("Egamma.Keys.Output.TopoSeededClustersSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.TopoSeededClustersSuppAOD", '-CellLink')

    egcf.addFlag("Egamma.Keys.Output.Electrons",'Electrons')
    egcf.addFlag("Egamma.Keys.Output.ElectronsSuppESD",'')
    egcf.addFlag("Egamma.Keys.Output.ElectronsSuppAOD",
                 '-e033.-e011.-e333.-e335.-e337.-e377.-isEMLHLoose.-isEMLHTight.-isEMLHMedium.-isEMLoose.-isEMMultiLepton.-isEMMedium.-isEMTight')

    egcf.addFlag("Egamma.Keys.Output.ForwardElectrons", 'ForwardElectrons')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectronsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.ForwardElectronsSuppAOD", 
                 '-isEMTight.-isEMMedium.-isEMLoose')

    egcf.addFlag("Egamma.Keys.Output.ForwardClusters", 'ForwardElectronClusters')
    egcf.addFlag("Egamma.Keys.Output.ForwardClustersSuppESD", '-SisterCluster')
    egcf.addFlag("Egamma.Keys.Output.ForwardClustersSuppAOD", 
                 '-SisterCluster.-CellLink')

    egcf.addFlag("Egamma.Keys.Output.Photons", 'Photons')
    egcf.addFlag("Egamma.Keys.Output.PhotonsSuppESD", '')
    egcf.addFlag("Egamma.Keys.Output.PhotonsSuppAOD", 
                 '-e033.-e011.-e333.-e335.-e337.-e377.-isEMLoose.-isEMTight')

    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticles", 'GSFTrackParticles')
    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticlesSuppESD", 
                 '-caloExtension.-cellAssociation.-perigeeExtrapEta.-perigeeExtrapPhi')
    egcf.addFlag("Egamma.Keys.Output.GSFTrackParticlesSuppAOD",
                 '-caloExtension.-cellAssociation.-perigeeExtrapEta.-perigeeExtrapPhi')

    # not xAOD
    egcf.addFlag("Egamma.Keys.Output.GSFTracks", 'GSFTracks')

    egcf.addFlag("Egamma.Keys.Output.TruthParticles", 'egammaTruthParticles')
    egcf.addFlag("Egamma.Keys.Output.TruthParticlesSuppESD", '-caloExtension')
    egcf.addFlag("Egamma.Keys.Output.TruthParticlesSuppAOD", '-caloExtension')

    return egcf

# self test
import unittest

class TestEgammaConfigFlags(unittest.TestCase):
    def runTest(self):
        flags = createEgammaConfigFlags()
        self.assertEqual(flags.Egamma.Keys.Output.Photons, "Photons")
        self.assertEqual(flags._get("Egamma.Keys.Output.Photons"), "Photons")

if __name__ == "__main__":
    unittest.main()
