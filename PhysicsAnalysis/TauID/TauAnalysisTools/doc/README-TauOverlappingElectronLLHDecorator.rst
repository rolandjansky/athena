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
matched reconstructed electrons. The matching is done requiring a Delta R
between reconstructed tau and electron of less than 0.4. Only electrons with a
pT of more than 5GeV are considered for the matching. In case more than one
electron is matched to the reco-tau, the one with the largest pT is used for
computing the likelihood score. For this purpose the AsgElectronLikelihoodTool
is utilized. The score should in general be between -4 and 4, where large values
indicate the reco-tau to be faked by an electron. Taus with no matched electron
are assigned a value of -4.


The tool can in general be used, including the header file::

  #include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"

and calling::

  TauAnalysisTools::TauOverlappingElectronLLHDecorator TOELLHDecorator( "TauOverlappingElectronLLHDecorator" );
  TOELLHDecorator.initialize();

  TOELLHDecorator.decorate(*tau);

The electron container is retrieved directly from the eventstore. The name of
the electron container can be configured via::

  TOELLHDecorator.setProperty("ElectronContainerName", "CONTAINERNAME");

The default container name is `Electrons`.

The name of the score decoration is `ele_match_lhscore` which is of type float.

An additional decoration is done to the reco tau object, holding an element link
to the matched electron. The link is named `electronLink`. Please check the
validity of this link before trying to access the linked particle or make use
of the getLink function from TauxAODHelpers::

  const xAOD::Electron* xElectron = xAOD::TauHelpers::getLink<xAOD::Electron>(xTau, "electronLink");
  
----------
Navigation
----------

* `TauAnalysisTools <../README.rst>`_

  * `TauSelectionTool <README-TauSelectionTool.rst>`_
  * `TauSmearingTool <README-TauSmearingTool.rst>`_
  * `TauEfficiencyCorrectionsTool <README-TauEfficiencyCorrectionsTool.rst>`_
  * `TauTruthMatchingTool <README-TauTruthMatchingTool.rst>`_
  * `TauTruthTrackMatchingTool <README-TauTruthTrackMatchingTool.rst>`_

