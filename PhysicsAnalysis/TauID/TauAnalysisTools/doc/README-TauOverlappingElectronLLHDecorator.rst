==================================
TauOverlappingElectronLLHDecorator
==================================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

This tool is for decorating reconstructed taus with a likelihood score of
matched reconstructed electrons. Additionally the tool decorates the decision
whether or not it passes a given working point.

The matching to electrons is done requiring a Delta R between reconstructed tau
and electron of less than 0.4. Only electrons with a pT of more than 5GeV are
considered for the matching. In case more than one electron is matched to the
reco-tau, the one with the largest pT is used for computing the likelihood
score. For this purpose the AsgElectronLikelihoodTool is utilized. The score
should in general be between -4 and 2, where large values indicate the reco-tau
to be faked by an electron. Taus with no matched electron are assigned a value
of -4.

The tool can in general be used, including the header file::

  #include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

and calling::

  TauAnalysisTools::TauOverlappingElectronLLHDecorator TOELLHDecorator( "TauOverlappingElectronLLHDecorator" );
  TOELLHDecorator.initialize();

  TOELLHDecorator.decorate(*tau);

Tool configuration
------------------

The electron container is retrieved directly from the eventstore. The name of
the electron container can be configured via::

  TOELLHDecorator.setProperty("ElectronContainerName", "CONTAINERNAME");

The default container name is ``Electrons``.

The working point used to decide if a tau passes the electron overlap removal
cut (see `TauSelectionTool <README-TauSelectionTool.rst>`_) can be configured by
setting the path to a root file, containing the cuts on the
``ele_match_lhscore``. This can be done like::

  TOELLHDecorator.setProperty("EleOLRFilePath", "PATH/TO/INPUT/FILE");

Tool output
-----------

.. list-table::
   :header-rows: 1
   :widths: 10 20 20
   
   * - decoration name
     - variable type
     - explanation
     
   * - ``ele_match_lhscore``
     - ``float``
     - LLH score of the matched electron

   * - ``ele_olr_pass``
     - ``char``
     - decision whether or not working point is passed 

   * - ``electronLink``
     - ``ElementLink< xAOD::ElectronContainer >``
     - element link to the matched electron,


If you want to retrieve the matched electron please check the validity of this
link before trying to access the linked particle or make use of the getLink
function from TauxAODHelpers::

  const xAOD::Electron* xElectron = xAOD::TauHelpers::getLink<xAOD::Electron>(xTau, "electronLink");

This of cause requires the following include::

  #include "xAODTau/TauxAODHelpers.h"

---
FAQ
---

#. **Question**: There are some events with really small llh score, about -15

   **Answer**: These are taus where the likelihood score of the matched electron is
   0, and for the purpose of the logarithmic transformation it is set to a default
   value, which results in a llh score of -15.1971.

----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_

    * `mc12 recommendations <README-TauEfficiencyCorrectionsTool-mc12.rst>`_
    * `mc15 pre-recommendations <README-TauEfficiencyCorrectionsTool-mc15_pre-recommendations.rst>`_
    * `TauEfficiencyCorrectionsTool Trigger <README-TauEfficiencyCorrectionsTool_Trigger.rst>`_

  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_
  * `TauOverlappingElectronLLHDecorator <README-TauOverlappingElectronLLHDecorator.rst>`_
