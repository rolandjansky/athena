====================
TrigTauMatchingTool
====================
----------------------------------------
A tool for tau trigger matching
----------------------------------------

Introduction
====================
The ``TrigTauMatchingTool`` matches offline hadronic tau objects (``TauJet``)
with a corresponding tau object at the trigger level that fired a specific
trigger. It does so by retrieving the container associated with the fired
trigger, and matching the objects at the trigger level with the given offline
tau object within a cone in dR (default: dR < 0.2 for HLT, dR < 0.3 for L1).
The first object in the trigger associated container that satisfies the dR
requirement is selected as a match.

Functionality
====================
* HLT trigger matching is provided by the ``TrigTauMatchingTool::match()``
  function. This function has two variants: the first matches the offline tau
  to an HLT tau by accessing the features associated with a trigger. (See
  Known problems below.) The second variant takes an
  ``xAOD::TauJetContainer`` containing the HLT taus as an additional argument
  and does a simple dR match with the offline tau.

* The matched HLT trigger object can be retrieved with the
  ``TrigTauMatchingTool::getHLT()``, which operates in the same way as the
  first variant of ``TrigTauMatchingTool::match()``. (See Known problems
  below.)

* The L1 trigger object associated with a matched HLT trigger object can be
  retrieved with the ``TrigTauMatchingTool::getL1FromHLT()``.

* L1 trigger trigger matching is provided by the
  ``TrigTauMatchingTool::matchL1()`` function. This function takes an
  ``xAOD::EmTauRoIContainer`` containing the L1 EM/tau trigger objects, and
  does a simple dR match with offline tau.

* It is possible to use resurrected triggers by setting the
  ``AllowResurrectedDecision`` property to ``true``. See
  ``util/TrigTauMatching_example.cxx`` for an example. *NOTE*: Resurrected
  triggers should *NOT* be used for physics, but only for trigger scale
  factor derivations and other trigger related studies and with great care.

Usage
====================

Full examples
--------------------

* *RootCore*

  - ``util/TrigTauMatching_example.cxx``

* *AthAnalysisBase*

  - ``share/TrigTauMatching_test.py``
  - ``src/TrigTauMatching_example.h``
  - ``src/TrigTauMatching_example.cxx``

Initialization
--------------------
In *RootCore*::

  // Trigger configuration tool
  TrigConf::xAODConfigTool configTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&configTool);
  ASG_CHECK_SA("MyAlg", configHandle->initialize());

  // The trigger decision tool
  Trig::TrigDecisionTool trigDecTool("TrigDecTool");
  ToolHandle<Trig::TrigDecisionTool> decisionToolHandle(&trigDecTool);
  ASG_CHECK_SA("MyAlg", decisionToolHandle->setProperty("ConfigTool", configHandle));
  ASG_CHECK_SA("MyAlg", decisionToolHandle->setProperty("TrigDecisionKey", "xTrigDecision"));
  ASG_CHECK_SA("MyAlg", decisionToolHandle->initialize());

  // The tau trigger matching tool
  Trig::TrigTauMatchingTool matchTool("TrigTauMatching");
  ASG_CHECK_SA("MyAlg", matchTool.setProperty("TrigDecisionTool", decisionToolHandle));
  ASG_CHECK_SA("MyAlg", matchTool.initialize());

(See ``util/TrigTauMatching_example.cxx``)

In *AthAnalysisBase*::

  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  trigDecTool = Trig__TrigDecisionTool('TrigDecisionTool')
  ToolSvc += trigDecTool

  from TrigTauMatching.TrigTauMatchingConf import Trig__TrigTauMatchingTool
  tauMatchingTool = Trig__TrigTauMatchingTool('TrigTauMatchingTool');
  tauMatchingTool.TrigDecisionTool = trigDecTool
  ToolSvc += tauMatchingTool

(See ``share/TrigTauMatching_test.py``)

Options
--------------------
Options are accessible as properties, i.e. via
``TrigTauMatchingTool::setProperty`` in *RootCore* and through direct access
in the job options file in *AthAnalysisBase*.

* *HLTLabel* (string): Label of the HLT tau object container to use for the
  trigger matching. Default: ``TrigTauRecMerged``.

* *AllowResurrectedDecision* (bool): Allow matching of resurrected triggers.
  (Uses the condition ``TrigDefs::Physics |
  TrigDefs::allowResurrectedDecision``.) Use with care.

Invocation
--------------------

::

    const xAOD::TauJetContainer *taus;
    RETURN_CHECK("MyAlg", event->retrieve(taus, "TauJets"));

    const std::string trigger("HLT_tau125_medium1_tracktwo");

    // Match a tau with the default dR = 0.2
    xAOD::TauJet *matchedTau = 0;
    for (auto tau: taus)
    {
      if (matchTool.match(tau, trigger))
      {
        matchedTau = tau;
        break;
      }
    }

    // Get the HLT and L1 objects that matches the first offline tau
    if (taus->size() > 0)
    {
      xAOD::TauJet *tau = taus->at(0);

      // Get the matched HLT object (0 if no match)
      xAOD::TauJet *hltTau = matchTool.getHLT(tau, trigger);

      // Get the matched L1 object (0 if no match)
      xAOD::EmTauRoI *l1Tau = matchTool.getL1FromHLT(tau, trigger);
    }

    // Match a tau with a looser dR requirement
    xAOD::TauJet *looserMatchedTau = 0;
    for (auto tau: taus)
    {
      if (matchTool.match(tau, trigger, 0.3))
      {
        looserMatchedTau = tau;
        break;
      }
    }

Notes
====================

Contact
--------------------
``TrigTauMatchingTool`` is written by Henrik Öhman <ohman@cern.ch>

JIRA: https://its.cern.ch/jira/browse/ATR-11094

Source: https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigAnalysis/TrigTauAnalysis/TrigTauMatching

Known problems
--------------------
* The functionality of ``TrigTauMatching::getL1FromHLT()`` is blocked by
  https://its.cern.ch/jira/browse/ATR-11765
* Feature based trigger matching for tau+MET chains is blocked by
  https://its.cern.ch/jira/browse/ATR-11199

Todo
--------------------
* The dR based matching functions lack the corresponding functions to
  return the matched object, e.g. ``TrigTauMatching::getHLT()`` with the
  extra ``xAOD::TauJetContainer`` and ``TrigTauMatching::getL1()``.
* Trigger matching for di-tau triggers.
