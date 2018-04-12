=======================================
TauEfficiencyCorrectionsTool -- Trigger
=======================================

:authors: Dirk Duschinger
:contact: dirk.duschinger@cern.ch

.. contents:: Table of contents

------------
Introduction
------------

Latest information on trigger scale factors for analysing 2015 and 2016 data
taking can be found in this `presentation
<https://indico.cern.ch/event/539998/contributions/2192863/attachments/1287118/1915369/tautrig_160608.pdf>`_. Scale
factors are only applied to reconstructed taus matched to truth hadronic tau
decays, which is done internally and does not require action by the user if the
requirements listed on the `twiki
<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauRecommendationsSummer2016#Important_prerequisites_for_appl>`_
are fulfilled

**IMPORTANT:** Use the tool only for taus matched to the trigger!

-----------------
Quick start guide
-----------------
     
To get started you can do the following::
  
  TauAnalysisTools::TauEfficiencyCorrectionsTool TauTriggerEffTool( "TauTriggerEfficiencyCorrectionsTool" );

  CHECK(TauTriggerEffTool.setProperty("EfficiencyCorrectionTypes", std::vector<int>({SFTriggerHadTau}) ));
  CHECK(TauTriggerEffTool.setProperty("TriggerName", "HLT_tau25_medium1_tracktwo" ));
  CHECK(TauTriggerEffTool.setProperty("IDLevel", (int)JETIDBDTTIGHT ));
  CHECK(TauTriggerEffTool.setProperty("PileupReweightingTool", PileupReweightingTool ));

  CHECK(TauTriggerEffTool.initialize());

The ``PileupReweightingTool`` is required to be a ToolHandle to the same
PileupReweightingTool that you use for you analysis. This tool is used to obtain
the random run number to make a decision on the year of data taking, that is
based on the ``LumiCalcFiles``. So if you have an ``LumiCalcFiles`` based only
on 2015(2016), then only 2015(2016) scale factors are used. If this file is for
both data taking periods, then the scale factors are used either from 2015
or 2016.

Remember to use the cast to
int for enums, like in the setting of ``IDLevel`` to ``(int)JETIDBDTTIGHT``.

Then in your loop you can apply or get scale factors from the tool by::

  TauTriggerEffTool.applyEfficiencyScaleFactor(xTau);                                     // either directly appending scale factors to the xAOD tau auxiliary store
  TauTriggerEffTool.getEfficiencyScaleFactor(xTau, dEfficiencyScaleFactor);               // or storing fake factors in variable dEfficiencyScaleFactor

Notes on different scale factors for 2015, 2016 and 2017
+-----------------------------------------------------------

By default, if the ``PRWTool`` property is not set, only scale factors for 2016
data analysis are applied. If you are only analysing 2015 data, this can be
changed by setting the ``TriggerYear`` property of the tool to ``"2015"``,
like::

  CHECK(TauTriggerEffTool.setProperty("TriggerYear", "2015" ));

The final trigger scale factors for 2016 data are extracted from three different measurements: Ztautau, ttbar and their combination. With ``TriggerSFMeasurement`` property one can choose between those measurements. Plots summarizing the trigger SF distributions are available here: https://qbuat-trigger.web.cern.ch/qbuat-trigger/tau_trigger_20.7_final/lmt_overlayed/index.html 

In the latest version of TauAnalysisTools pre-recommendations for 2017 tau triggers are available. Here, all scale factors are set to 1.0, with the relative error taken from the respective 2016 result.

--------------------
Available properties
--------------------

To be able to use trigger scale factors from the TauEfficiencyCorrectionsTool
one needs a separate tool instance with at least the following configuration:

.. list-table::
   :header-rows: 1
	      
   * - property name
     - variable type
     - default value
     - explanation
	 
   * - ``TriggerName``
     - ``std::string``
     - ``""`` (empty)
     - trigger name, like ``"HLT_tau125_medium1_tracktwo"``, other available
       option can be found below
	 
   * - ``TriggerYear``
     - ``std::string``
     - ``"2016"``
     - year of data taking, not necessary if PileupReweightingTool Property is used
   
   * - ``PileupReweightingTool``
     - ``ToolHandle<CP::PileupReweightingTool>``
     - empty
     - pass this tool to automatically decide which trigger scale factors to
       apply
     
   * - ``IDLevel``
     - ``int``
     - ``JETIDBDTMEDIUM`` 
     - level of offline ID, it is the same property as for jet ID scale
       factors. A list of supported values can be found below
    
   * - ``TriggerSFMeasurement``
     - ``std::string``
     - ``"combined"``
     - choose between the measurements from which scale factors are extracted from. Options are: ``"combined"``, ``"Ztautau"`` and ``"ttbar"``


In addition the variable ``EfficiencyCorrectionTypes`` needs to be set to the
value ``std::vector<int>({SFTriggerHadTau})``

----------------------
Overview of Variations
----------------------

MC16 Pre-Recommendations
--------------------------

The recommended systematic variations are as of now for 2015, 2016 or 2017 as
indicated by the postfix number:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2017``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2017``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2017``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2017``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2017``

The following additional systematic variations are also available (**NOT recommended**):

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016``

2017 Moriond
---------------

The recommended systematic variations are as of now for 2015 or 2016 as
indicated by the postfix number:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2016``

The following additional systematic variations are also available (**NOT recommended**):

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016``

2016 fall
---------

The recommended systematic variations are as of now for 2015 or 2016 as
indicated by the postfix number:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2016``

The following additional systematic variations are also available (**NOT recommended**):

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016``

2016 ICHEP
----------

The recommended systematic variations are as of now for 2015 or 2016 as
indicated by the postfix number:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2016``

The following additional systematic variations are also available (**NOT recommended**):

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL2015``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC2016``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST2016``

2015 EOYE and 2016 moriond
--------------------------

The recommended systematic variations are as of now:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATDATA``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_STATMC``
* ``TAUS_TRUEHADTAU_EFF_TRIGGER_SYST``

an additional systematic variation, representing the statistical combination of
the recommended systematics is also available:

* ``TAUS_TRUEHADTAU_EFF_TRIGGER_TOTAL``

---------------------
Supported tau trigger
---------------------

**IMPORTANT:** Once again, use the tool only for taus matched to the trigger!

At the moment the following tau trigger are supported:

2016 fall
----------------

* ``HLT_tau25_medium1_tracktwo``
* ``HLT_tau35_medium1_tracktwo``
* ``HLT_tau50_medium1_tracktwo_L1TAU12``
* ``HLT_tau80_medium1_tracktwo``
* ``HLT_tau125_medium1_tracktwo``
* ``HLT_tau160_medium1_tracktwo``

until 2016 ichep
----------------

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

..
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
