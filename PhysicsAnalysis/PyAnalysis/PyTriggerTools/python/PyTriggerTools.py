# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""module to access trigger objects interactively

Examples are in `PyAnalysisExamples/TriggerTest.py`_

.. _PyAnalysisExamples/TriggerTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/TriggerTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup

:author: Tadashi Maeno
:contact: Tadashi.Maeno@cern.ch

"""
__docformat__ = "restructuredtext en"

from PyKernel import PyKernel as PyK

############ CTP Decision
def getCTP_Decision (aKey):
    """Retrieve CTP_Decision object from SG

    :param aKey: key of the object

    **examples**::
  
      athena> ctp = PyTriggerTools.getCTP_Decision('CTP_Decision')
      athena> dir(ctp)                 # see methods of the CTP_Decision class      
      athena> print ctp.isAccepted()   # see if this event was accepted by LVL1

    """
    return PyK.retrieve(PyK.GNS.CTP_Decision,aKey)


############ LVL1 ROI
def getLVL1_ROI (aKey):
    """Retrieve LVL1_ROI object from SG

    :param aKey: key of the object

    **examples**::
  
      athena> rois = PyTriggerTools.getLVL1_ROI('LVL1_ROI')
      athena> dir(rois)                # see methods of the LVL1_ROI class
      athena> em = rois.getEmTauROIs() # get EMTau ROIs
      athena> len(em)                  # get length of the EMTau ROI vector
      athena> print em[0].getEta()     # see Eta of the first ROI

    """
    return PyK.retrieve(PyK.GNS.LVL1_ROI,aKey)


############ TrigDecision
def getTrigDecision (aKey):
    """Retrieve TrigDecision object from SG

    :param aKey: key of the object


    """
    return PyK.retrieve(PyK.GNS.TrigDec.TrigDecision,aKey)

############ HLTResult
def getHLTResult (aKey):
    """Retrieve HLT::HLTResult object from SG

    :param aKey: key of the object


    """
    return PyK.retrieve(PyK.GNS.HLT.HLTResult,aKey)

############ HLTAODConfigData
def getHLTAODConfigData (aKey):
    """Retrieve TrigConf::HLTAODConfigData object from SG

    :param aKey: key of the object


    """
    return PyK.retrieve(PyK.GNS.TrigConf.HLTAODConfigData,aKey)

############ Lvl1AODConfigData
def getLvl1AODConfigData (aKey):
    """Retrieve TrigConf::Lvl1AODConfigData object from SG

    :param aKey: key of the object


    """
    return PyK.retrieve(PyK.GNS.TrigConf.Lvl1AODConfigData,aKey)

############ Lvl1AODPrescaleConfigData
def getLvl1AODPrescaleConfigData (aKey):
    """Retrieve TrigConf::Lvl1AODPrescaleConfigData object from SG

    :param aKey: key of the object


    """
    return PyK.retrieve(PyK.GNS.TrigConf.Lvl1AODPrescaleConfigData,aKey)
