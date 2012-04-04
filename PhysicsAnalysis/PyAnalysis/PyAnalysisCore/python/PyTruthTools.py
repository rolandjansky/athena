# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""module to access truth objects interactively

Example is in `PyAnalysisExamples/TruthTest.py`_

.. _PyAnalysisExamples/TruthTest.py: http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/PhysicsAnalysis/PyAnalysis/PyAnalysisExamples/share/TruthTest.py?rev=HEAD&content-type=text/vnd.viewcvs-markup


:author: Tadashi Maeno
:contact: Tadashi.Maeno@cern.ch

"""
__docformat__ = "restructuredtext en"

from PyKernel import PyKernel as PyK

def getMcEvents (aKey):
    """Retrieve McEventCollection object from SG

    :param aKey: key of the object

    **examples**::

      athena> mcc = PyTruthTools.getMcEvents('GEN_EVENT')
      athena> len(mcc)
      athena> mc = mcc[0]
      athena> mc.alphaQCD()
      athena> it  = mc.particles_begin()       # GenEvent::particles_begin() and _end() give ietrators
      athena> itE = mc.particles_end()  
      athena> while (it != itE):               # loop over all particles
      ...  p = it.next()                       # dereference and increment the ietrator
      ...  print p.pdg_id()
      ...                                      # just hit return key


    **Note:** Some methods of GenEvent_/GenVertex_, e.g., particles_begin(), give an iterator. One may use it like as a python iterator. But don't try 'for i in iterator:'. It should cause a crash since an iterator of C++ doesn't know it's boundary.

    .. _GenEvent: http://reserve02.usatlas.bnl.gov/lxr/source/atlas/Simulation/HepMC/HepMC/GenEvent.h
    .. _GenVertex: http://reserve02.usatlas.bnl.gov/lxr/source/atlas/Simulation/HepMC/HepMC/GenVertex.h
    
    """
    return PyK.retrieve(PyK.GNS.McEventCollection,aKey)
