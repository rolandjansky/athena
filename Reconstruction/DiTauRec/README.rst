======================
Di-Tau Reconstruction
======================

:authors: David Kirchmeier
:contact: david.kirchmeier@cern.ch

.. contents:: Table of contents

|
|
|

Introduction
============

The di-tau reconstruction package (``DiTauRec``) provides a new 
method for reconstructing boosted pairs of tau leptons. 
It is meant to be an addition to the approach of reconstructing single taus 
for a high pT boosted region.
With the approach of single tau reconstruction it is not possible to reconstruct
tau pair topologies with low spatial separation, corresponding to a di-tau pT > 500 GeV.

The current development of ``DiTauRec`` is focused on full-hadronic tau decays (hadhad) but
also an semi-leptonic decay mode (lephad) is implemented and ready for 
further investigation.
Additionally, a multi-variate di-tau identification algorithm is under development which will be able to suppress QCD jet background with high efficiency.

``DiTauRec`` uses anti-kt-10 fat jets as seeds for its search for di-tau candidates. 
These are filtered for subjets. 
A candidate for a full-hadronic tau pair decay should include at least two subjets, while 
each subjet has to include at least one track. 
For semi-leptonic tau pair decays also one subjet plus an electron or muon candidate within
the seed jet area is allowed.

A general introduction into di-tau tagging in ATLAS can be found in my `master thesis <https://cds.cern.ch/record/2105592>`_. 
Some slides about the general implementation can be found `here <https://indico.cern.ch/event/441941/session/1/contribution/7/attachments/1157918/1666256/15-09-22-TauWorkshop.pdf>`_.

|
|
|

Implementation
================

Di-Tau Builder
-------------------
`DiTauBuilder.cxx <src/DiTauBuilder.cxx>`_ is the main class for di-tau reconstruction.
It sets cuts on the seed jet and loops over all di-tau candidates. 
The di-tau candidate information is stored by 
`DiTauCandidateData.h <DiTauRec/DiTauCandidateData.h>`_.

`DiTauBuilder.py <python/DiTauBuilder.py>`_ is the main job options file.
It configures the DiTauBuilder class and defines the names of input and output containers, 
as well as the radius parameters for seed jet and subjet reconstruction.

|

Di-Tau Reconstruction Tools
---------------------------

DiTauBuilder.cxx makes use of several tools which are successively applied to a 
di-tau candidate.
The getter functions for these tools are defined in 
`DiTauAlgorithmsHolder.py <python/DiTauAlgorithmsHolder.py>`_.
These tools are applied in the following order:

* `SeedJetBuilder <src/SeedJetBuilder.cxx>`_ (stores seed jet information in 
  DiTauCandidateData)
* `ElMuFinder <src/ElMuFinder.cxx>`_ (searches for electrons and muons inside the seed jet)
* `SubjetBuilder <src/SubjetBuilder.cxx>`_ (reconstructs subjets within the seed jet)
* `VertexFinder <src/VertexFinder.cxx>`_ (finds the most likely primary vertex)
* `DiTauTrackFinder <src/DiTauTrackFinder.cxx>`_ (associates tracks to the seed jet and checks quality criteria)
* `CellFinder <src/CellFinder.cxx>`_ (stores information of calorimeter cells which are located inside the 
  subjets)
* `IDvarCalculator <src/IDvarCalculator.cxx>`_ (calculates the f_core ID variable for each subjet which 
  can be useful for background rejection later)

These tools are loaded and configured in DiTauBuilder.py and are executed in DiTauBuilder.cxx.
It has to be noted that CellFinder and IDvarCalculator do need cell information, which
is usually not provided in xAOD input files (we will come to that in `Reconstructing Di-Tau Candidates`_).

|

Di-Tau Quality Cuts
------------------------

These cuts are intended to give a reasonable quality of the di-tau candidate. 
By applying a high cut on the seed jet pT only di-tau candidates in the interesting highly boosted tau pair region are stored.

*  Jet pT > 300 GeV
*  Subjet pT > 15 GeV, nTracks > 1, n < 5
*  Track pT > 1 GeV, IPd0Max = 1, IPz0Max = 1.5
*  Electron pT > 7 GeV, \|eta\| < 2.47, author: Electron or Ambiguous
*  Muon pT > 7 GeV, \|eta\| < 2.47, muon quality = 2 (MuonSelectionTool)

|

Di-Tau Event Data Model (EDM)
------------------------------

The implementation of the Di-Tau EDM can be found in
`Event/xAOD/xAODTau <https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTau/trunk>`_.
The pool converters are implemented in
`Event/xAOD/xAODTauAthenaPool <https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTauAthenaPool>`_.

| 
|
|

Reconstructing Di-Tau Candidates
=================================

This section describes how to use ``DiTauRec``. 
In samples which are produced with Athena release 20.7.2 
(and above) di-tau candidates will already be available.
If your sample is a HITS, RDO or ESD file and you want to apply ``DiTauRec`` as part of the full reconstruction chain, follow `Environmental Setup`_ and `DiTauRec in Full Reconstruction`_. 
If your sample is a xAOD file and you want to apply ``DiTauRec`` follow `Environmental Setup`_ and `DiTauRec with xAOD input`_.
If your xAOD sample already includes di-tau candidates you can continue with
the next chapter `Di-Tau Container Readout`_. 

|
|

DiTauRec in Full Reconstruction
----------------------------------

This section is about reconstructing di-tau candidates in HITS, RDO or ESD files.
``DiTauRec`` is executed here as part of the full reconstruction chain. 
The advantage is that the ``DiTauRec`` is able to calculate ID variables
by using cell information.
This does not effect the performance of the reconstruction but may improve 
a later background separation.
The full reconstruction in athena is done with the ``Reco_tf.py`` script
(for more information see `RecoTf <https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RecoTf>`_ 
and `JobTransform <https://twiki.cern.ch/twiki/bin/view/AtlasComputing/JobTransform#General_Documentation>`_). 
To get the correct command (which corresponds to the ami tag of the input file)
we have to load ``pyami``:

.. code-block:: bash

    cd $TestArea
    voms-proxy-init -voms atlas
    lsetup pyami

and execute ``GetTfCommand.py`` (also see `TransformConfiguration#Configuration_from_AMI <https://twiki.cern.ch/twiki/bin/view/AtlasComputing/TransformConfiguration#Configuration_from_AMI>`_), e.g.:

.. code-block:: bash

    GetTfCommand.py --AMI r6869

The printed Reco_tf command may have a number of configurations dedicated to the 
ami tag you have provided. For the sake of simplicity we will continue with a minimal RecoTf command 
(do not forget to change the input file path):

.. code-block:: bash

    cd $TestArea/WorkArea/run  # you may also create a sub-folder for each run
    Reco_tf.py --autoConfiguration='everything' --maxEvents '100' --inputESDFile '/path/to/file' --outputAODFile 'myAOD.pool.root'

In case the run was successful you should see the file ``myAOD.pool.root`` containing 
a full set of reconstructed physics objects including ``DiTauJets``.
If this is not the case make sure that you checked out the correct tags of ``RecExCommon`` and ``RecExPers`` and that your input events contain and a sufficient amount of high-pT jets. 

You can configure the above command by using several `DiTauRecFlags <python/DiTauRecFlags.py>`_.
For example you can lower the seed jet cut to 30 GeV with

.. code-block:: bash

    Reco_tf.py --autoConfiguration='everything' --maxEvents '100' --preExec 'ESDtoAOD:from DiTauRec.DiTauRecFlags import diTauFlags; diTauFlags.diTauRecJetSeedPt.set_Value_and_Lock(30000)' --inputESDFile '/path/to/file' --outputAODFile 'myAOD.pool.root'

|

DiTauRec with xAOD input
---------------------------

This section is about reconstructing di-tau candidates in (x)AOD files.
The reconstruction is done again with the ``Reco_tf.py`` script
(for more information see `RecoTf <https://twiki.cern.ch/twiki/bin/view/AtlasComputing/RecoTf>`_ 
and `JobTransform <https://twiki.cern.ch/twiki/bin/view/AtlasComputing/JobTransform#General_Documentation>`_). 
At first go into the run folder

.. code-block:: bash

  cd $TestArea/WorkArea/run

then execute ``DiTauRec`` by using the following command (change the input file path first):

.. code-block:: bash

    Reco_tf.py --preInclude 'DiTauRec/DiTauRecAOD_config.py' --maxEvents '100' --preExec 'all:from DiTauRec.DiTauRecFlags import diTauFlags; diTauFlags.doVtxFinding.set_Value_and_Lock(True); diTauFlags.diTauRecJetSeedPt.set_Value_and_Lock(30000); diTauFlags.doCellFinding.set_Value_and_Lock(False)' --inputAODFile='/path/to/file' --outputAOD_RPRFile='myAOD.pool.root'

The ``--preInclude`` option defines the job options `DiTauRecAOD_config.py <share/DiTauRecAOD_config.py>`_, which is dedicated to an execution on AOD input files. 
After the ``--preExec`` option several `DiTauRecFlags <python/DiTauRecFlags.py>`_ are configured.
Note that vertexing is usally turned off in AOD-to-AOD reconstruction, though it has to be explicitly turned on for ``DiTauRec`` here. 
Since cell information is not available in AOD files, the cell finding is turned off.
This does not effect the performance of the reconstruction but may decrease 
a later background separation power.
The location of the input file is defined with ``--inputAODFile``, the name of the output file is
defined with the ``--outputAOD_RPRFile`` option.
In case the run was successful you should see the file ``myAOD.pool.root`` containing 
the set of reconstructed physics objects you had before plus ``DiTauJets``.
If this is not the case make sure that DiTauRecAOD_config.py is included successfully and that your input events contain and a sufficient amount of high-pT jets. 

|
|
|

Di-Tau Container Readout
=========================

This section is about reading a di-tau container in RootCore. 
In general, reading di-tau candidates from xAOD works as usual (see `xAOD Analysis in RootCore <https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/SoftwareTutorialxAODAnalysisInROOT>`_).
In older base releases the ``xAODTau`` package has to be checked out, since it includes the definitions of the di-tau EDM:

.. code-block:: bash 

    svn co svn+ssh://svn.cern.ch/reps/atlasoff/Event/xAOD/xAODTau/trunk ./xAODTau/

In your RootCore executable you have to include the DiTauJetContainer header:

.. code-block:: c++
    
    #include "xAODTau/DiTauJetContainer.h"

In your ``execute ()`` function do the following

.. code-block:: c++
  
    // get the event
    xAOD::TEvent* event = wk()->xaodEvent();
    // get the di-tau container
    const xAOD::DiTauJetContainer* xDiTauJetContainer = 0;
    if ( !event->retrieve(xDiTauJetContainer, "DiTauJets").isSuccess() ) {
        Error("execute()", "Failed to retrieve DiTauJetContainer") );
        return EL::StatusCode::FAILURE;
    }

Now you can loop over the di-tau candidates of the event and access their variables, e.g.:

.. code-block:: c++
  
    for (const auto* ditau: *xDiTauContainer){
      // di-tau pt
      pt = ditau->pt()

      // number of subjets
      n = ditau->nSubjets()

      // pt of the leading subjet
      pt = subjetPt(0)
      // pt of the subleading subjet
      pt = subjetPt(1)

      // f_core of the leading subjet (only if cell information was provided in sample production)
      fCoreLead = ditau->fCore(0)
      // f_core of the subleading subjet (only if cell information was provided in sample production)
      fCoreSubl = ditau->fCore(1)


To see all variables available in the DiTau EDM consult `DiTauJet_v1.cxx <https://svnweb.cern.ch/trac/atlasoff/browser/Event/xAOD/xAODTau/trunk/Root/DiTauJet_v1.cxx>`_ in ``Event/xAOD/xAODTau``.

|
|
|
|
