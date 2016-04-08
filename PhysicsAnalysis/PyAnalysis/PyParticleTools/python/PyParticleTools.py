# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""module to access particle objects interactively

Examples are in `PyAnalysisExamples/ParticleTest.py`_

.. _PyAnalysisExamples/ParticleTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/ParticleTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup

:author: Tadashi Maeno
:contact: Tadashi.Maeno@cern.ch

"""
__docformat__ = "restructuredtext en"

from PyKernel import PyKernel as PyK

def getElectrons (aKey):
    """Retrieve ElectronContainer object from SG

    :param aKey: key of the object

    **examples**::

      athena> econ = PyParticleTools.getElectrons('ElectronCollection')
      athena> for e in econ:
      ...  print e.pt()  # don't forget to put an additional whitespace before 'print'
      ...                # just hit the return key

    """
    return PyK.retrieve(PyK.GNS.ElectronContainer,aKey)

def getMuons (aKey):
    """Retrieve MuonContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.Analysis.MuonContainer,aKey)

def getNeutrinos (aKey):
    """Retrieve NeutrinoContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.NeutrinoContainer,aKey)

def getParticleBases (aKey):
    """Retrieve ParticleBaseContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.ParticleBaseContainer,aKey)

def getParticleJets (aKey):
    """Retrieve ParticleJetContainer object from SG
       OBSOLETE due to ParticleJet/Jet merger
    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.ParticleJetContainer,aKey)

def getJets (aKey):
    """Retrieve JetContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.JetCollection,aKey)

def getPhotons (aKey):
    """Retrieve PhotonContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.PhotonContainer,aKey)

def getTauJets (aKey):
    """Retrieve TauJetContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.Analysis.TauJetContainer,aKey)

def getTruthParticles (aKey):
    """Retrieve TruthParticleContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.TruthParticleContainer,aKey)

def getIParticles (aKey):
    """Retrieve IParticleContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.IParticleContainer,aKey)

def getBJets (aKey):
    """Retrieve BJetContainer object from SG

    :param aKey: key of the object

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.BJetContainer,aKey)

def getMissingET (aKey):
    """Retrieve MissingET object from SG

    :param aKey: key of the object

    **examples**::

      athena> met = PyParticleTools.getMissingET('MET_Final')
      athena> print met.sumet()

    """
    return PyK.retrieve(PyK.GNS.MissingET,aKey)

def getMissingETSig (aKey):
    """Retrieve MissingETSig object from SG

    :param aKey: key of the object

    **examples**::

      athena> met = PyParticleTools.getMissingETSig('METSig')
      athena> print met.met()

    """
    return PyK.retrieve(PyK.GNS.MissingETSig,aKey)

def getMissingEtCalo (aKey):
    """Retrieve MissingEtCalo object from SG

    :param aKey: key of the object

    **examples**::

      athena> met = PyParticleTools.getMissingEtCalo('MET_Base')
      athena> print met.calibType()

    """
    return PyK.retrieve(PyK.GNS.MissingEtCalo,aKey)

def getMissingEtTruth (aKey):
    """Retrieve MissingEtTruth object from SG

    :param aKey: key of the object

    **examples**::

      athena> met = PyParticleTools.getMissingEtTruth('MET_Truth')
      athena> print met.sumet()

    """
    return PyK.retrieve(PyK.GNS.MissingEtTruth,aKey)

def getMissingETSigObjs (aKey):
    """Retrieve MissingETSigObjContainer object from SG

    :param aKey: key of the object

    see `getElectrons`
    """
    return PyK.retrieve(PyK.GNS.MissingETSigObjContainer,aKey)

def getCaloClusters (aKey):
    """Retrieve CaloClusterContainer object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.CaloClusterContainer,aKey)

def getVertices (aKey):
    """Retrieve VxContainer object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.VxContainer,aKey)


def getTrackParticleTruths (aKey):
    """Retrieve TrackParticleTruthCollection object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.TrackParticleTruthCollection,aKey)


def getTrackParticles (aKey):
    """Retrieve TrackParticleContainer object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.Rec.TrackParticleContainer,aKey)


def getTrackRecords (aKey):
    """Retrieve TrackRecordCollection object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    # nb. TrackRecordCollection is a typedef, but that was the name used
    # in CLASS_DEF.  So we have to pass that name into SG, not the name
    # of the actual class.
    return PyK.retrieve("TrackRecordCollection",aKey)


def getDetStatusMap (aKey):
    """Retrieve DetStatusMap object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.DetStatusMap,aKey)


def getMuonSpShowers (key):
    """Retrieve Rec::MuonSpShowerContainer object from SG

    :param aKey: key of the object

    **examples**::

    see `getElectrons`

    """
    return PyK.retrieve(PyK.GNS.Rec.MuonSpShowerContainer,key)


# Workarounds for pyroot bugs.
from RootUtils import PyROOTFixes
PyROOTFixes.fix_method ("Analysis::TauJet", "clusterLink")
PyROOTFixes.fix_method ("Analysis::TauJet", "cellClusterLink")
PyROOTFixes.fix_method ("Analysis::TauJet", "jetLink")
