=======================================
TauEfficiencyCorrectionsTool -- Trigger
=======================================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

Tau trigger efficiency scale factors for run 2 have recently become
available. The the current implementation is not complete and does not provide
all features. However, the basic setup to provide scale factors for EOYE
analysis is in place. As it is expected that there will be some changes to the
tool handling and default behaviour it is recommended to check the README's on a
regular basis.

More information on trigger scale factors can be found in this `presentation
<https://indico.cern.ch/event/463811/contribution/2/attachments/1192916/1732031/TauTriggerSliceMeeging_tagandprobe_23Nov2015_v2.pdf>`_.

**IMPORTANT:** Use the tool only for taus matched to the trigger!

To be able to use trigger scale factors from the TauEfficiencyCorrectionsTool
one needs a separate tool instance with at least the following configuration:

.. list-table::
   :header-rows: 1
	      
   * - property name
     - variable type
     - default value
     - explanation
	 
   * - TriggerName
     - ``std::string``
     - ``""`` (empty)
     - trigger name, like ``"HLT_tau125_medium1_tracktwo"``, other available
       option can be found below
	 
   * - IDLevel
     - ``int``
     - ``JETIDBDTMEDIUM`` 
     - level of offline ID, it is the same property as for jet ID scale
       factors. A list of supported values can be found below

In addition the variable ``EfficiencyCorrectionTypes`` needs to be set to the
value ``std::vector<int>({SFTriggerHadTau})``

-----------------
Quick start guide
-----------------
     
To get started you can do the following::
  
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauTriggerEffTool( "TauTriggerEfficiencyCorrectionsTool" );

  CHECK(TauTriggerEffTool.setProperty("EfficiencyCorrectionTypes", std::vector<int>({SFTriggerHadTau}) ));
  CHECK(TauTriggerEffTool.setProperty("TriggerName", "HLT_tau25_medium1_tracktwo" ));
  CHECK(TauTriggerEffTool.setProperty("IDLevel", (int)JETIDBDTTIGHT ));

  CHECK(TauTriggerEffTool.initialize());

Remember to use the cast to int for enums, like in the setting of ``IDLevel`` to
``(int)JETIDBDTTIGHT``.

Then in your loop you can apply or get scale factors from the tool by::

  TauTriggerEffTool.applyEfficiencyScaleFactor(xTau);                                     // either directly appending scale factors to the xAOD tau auxiliary store
  TauTriggerEffTool.getEfficiencyScaleFactor(xTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor

EOYE setup
----------

Please use tag ``TauAnalysisTools-00-01-21`` or ABR 2.3.36 for EOYE
analyses. This includes the recommended trigger scale factors.

The recommended systematic variations are as of now:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST``

an additional systematic variation, representing the statistical combination of
the recommended systematics is also available:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL``

**IMPORTANT:** Once again, use the tool only for taus matched to the trigger!

---------------------
Supported tau trigger
---------------------

At the moment the following tau trigger are supported:

* ``HLT_tau25_medium1_tracktwo``
* ``HLT_tau35_medium1_tracktwo``
* ``HLT_tau50_medium1_tracktwo_L1TAU12``
* ``HLT_tau80_medium1_tracktwo``
* ``HLT_tau125_medium1_tracktwo``

-----------------
Supported IDLevel
-----------------

At the moment the following offline ID working points are supported:

* ``JETIDBDTLOOSE``
* ``JETIDBDTMEDIUM``
* ``JETIDBDTTIGHT``

------------------
Supported binnings
------------------

Binning in tau-eta
------------------

By default scale factors are not binned in tau-eta (this may change in the future if more statistics are available). However, it is possible to use scale factors binned in tau-eta setting the property ``UseTriggerInclusiveEta`` to false, i.e.::

  TauEffTool.setProperty("UseTriggerInclusiveEta", false);


Binning in data periods
-----------------------

Not implemented yet


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
