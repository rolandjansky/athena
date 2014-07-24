
================================================
TauCorrUncert: Tau Corrections and Uncertainties
================================================

:authors: Dirk Duschinger
:former authors: Marcus M. Morgenstern, Noel Dawe
:contact: dirk.duschinger@cern.ch

------------
Introduction
------------

This package is designed to retrieve tau correction factors and associated
uncertainties. The tau energy scale, tau identification and electron veto scale
factors are currently provided. All relevant information about the actual
measurement of these uncertainties can be found here:
`<https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TauRecommendationsWinterConf2013>`_.
These numbers are mostly valid for 2012 data analysis using reprocessed data,
i.e. p1443 (p1344/p1345).

-----
Setup
-----

The package can be built with RootCore (using the provided
cmt/Makefile.RootCore) or standalone::

   cd cmt && make

If not using RootCore, you will have to add ``./standalone`` to your
``LD_LIBRARY_PATH`` by sourcing the provided ``setup.sh`` (that can be sourced from
anywhere)::

   source setup.sh

This is also required if you want to run the examples.

To build TauCorrUncert and run the unit tests::

   source setup.sh
   make

-------
Running
-------

An example is given in ``examples/TestEVeto.py``

1. Remember to add this to your compilation flags::

   -I<path to TauCorrUncert package>/TauCorrUncert

2. Remember to add this to your linker flags::

   -L<path to TauCorrUncert package>/lib -lTauCorrUncert

-----------------
TES Uncertainties
-----------------

Use the ``TESUnertainty`` class to scale your tau pT up or down (in MC) according to the
TES uncertainties (which are analysis independent)::

   using namespace TauCorrUncert;

   TESUncertainty* tes = new TESUncertainty(<full path to input file>);
   float uncert = tes->GetTESUncertainty(pt, eta, numTrack)
   float tau_pt_scaled_up = tau_pt * (1 + uncert);
   float tau_pt_scaled_down = tau_pt * (1 - uncert);

where pt is the tau pt in MeV. More examples of usage in the examples directory.
If you like to access a specific component of the TES uncertainty, ile
statistical uncertainty of the in-situ mreasurement you need to call::

   float uncert = tes->GetTESUncertainty(pt, eta, numTrack,INSITUSTAT)

**However, you should get in contact with TauWG first before doing this in your analysis!**

In data the tau pT is shifted and needs to be corrected. For this you have to do::

   float shift = tes->GetTESShift(pt, numTrack);
   float pt_corr = (1 + shift) * pt;

New nuisance parameters are provided for single TES components::

   * FINAL: "old style" total TES uncertainty
   * TOTAL: total TES uncertainty w/ constraints from in-situ measurement at low pt (pt < 50 GeV), i.e. sqrt(MODELING**2 + CLOSURE**2 + INSITUINTERPOL**2 + SINGLEPARTICLE**2)
   * INSITU: total in-site component, i.e. sqrt(INSITUSYS**2 + INSITUSTAT**2) **NOTE: no interpolation is applied here; if you want to apply interpolation take INSITUINTERPOL**
   * INSITUINTERPOL: total in-situ component with pt interpolation
     accoording to sqrt(1 - (pt -50)/20) * INSITU for 50 GeV < pt < 70 GeV; above pt > 70 GeV the interpolation factor is 0, while for pt < 50 GeV it is 1
   * INSITUSTAT/INSITUSYST: statistical and systematic component of in-situ measurement
   * SINGLEPARTICLEINTERPOL: single particle response interpolated as
     "switch-on", i.e. (1 - sqrt(1 - (pt -50)/20)) * SINGLEPARTICLE
   * MODELING: modeling component
   
--------------------
Tau ID Scale Factors
--------------------

The TauSF class should be used to access tau identification and electron veto
scale factors.

To create the tool::

   using namespace TauCorrUncert;
   TauSF* SFTool = new TauSF(<path to share directory>)

where myPath is a string specifying the location of the package (in order to
find the share directory). You can either pass an absolute or relative path to
TauCorrUncert's share directory.

-----------------
Use for tau ID SF
-----------------

Use the tau ID scale factors to assign a weight for truth matched reconstructed
taus. The scale factors account for mismodeling of MC and are derived from
MC/Data comparison. To access their uncertainties during the evaluation of
systematics use the ``GetIDStatUnc`` and ``GetIDSysUnc`` method. The scale factors
depend on the tau ID working point which are stored in enums. It is recommended
to use eta binned, pt-inclusive and ID-inclusive (i.e. loose, medium, tight)
scale factors, which can be accessed via::

   float sf = SFTool->GetIDSF(TauCorrections::BDTMEDIUM, tau.eta,
   tau.numTrack, tau.pt /*optional*/)

The pt has to be provided in **GeV**. The statistical and systematic
uncertainties one can get by calling::

   float stat_unc = SFTool->GetIDStatUnc(TauCorrections::BDTMEDIUM, tau.eta,
   tau.numTrack, tau.pt /*optional*/)

and::

   float sys_unc = SFTool->GetIDSysUnc(TauCorrections::BDTMEDIUM, tau.eta,
   tau.numTrack, tau.pt /*optional*/)

respectively.
Exclusive ID (i.e. fail loose, loose not medium, medium not tight and tight)
scale factors can be accessed via::

   float sf = SFTool->GetExclIDSF(TauCorrections::BDTMEDIUM, tau.eta,
   tau.numTrack, tau.pt /*optional*/)

*These are not the recommended by default, but you should discuss with
TauWG!!!* To get the corresponding uncertainties call::

   float stat_unc = SFTool->GetExclIDStatUnc(TauCorrections::BDTMEDIUM, tau.eta,
   tau.numTrack, tau.pt /*optional*/)

and::

   float sys_unc = SFTool->GetExclIDSysUnc(TauCorrections::BDTMEDIUM, tau.eta,
   tau.numTrack, tau.pt /*optional*/)

respectively.

To change the default behaviour call::

  SFTool->SwitchOnEtaBinnedIDSF(false);

which will switch to eta inclusive scale factors. Call::

  SFTool->SwitchOnPtBinnedIDSF(true);

in order to access pt binned scale factors. Call::

  SFTool->SwitchOnHighPtUncert(true);

in order to inflate uncertainty for pt > 100GeV. For details please refer to these `slides <https://indico.cern.ch/event/304094/contribution/2/material/slides/0.pdf>`_.

-------------------------------
Continuous tau ID Scale Factors
-------------------------------

The continuous tau ID scale factors can be accessed via the BDTSF tool::

   using namespace TauCorrUncert;
   BDTSF* SFTool = new BDTSF(<path to share directory>)

To access the scale factors and their uncertainties one needs to call::


   float sf = SFTool->getBDTbinnedSF(tau.numTrack, tau.BDTJetScore)

The statistical and systematic uncertainties one can get by calling::

   float stat_unc = SFTool->getBDTbinnedSFStatUnc(tau.numTrack, tau.BDTJetScore)

and::

   float sys_unc_up = SFTool->getBDTbinnedSFSysUnc(tau.numTrack, tau.BDTJetScore, "UP")

   float sys_unc_down = SFTool->getBDTbinnedSFSysUnc(tau.numTrack, tau.BDTJetScore, "DOWN")

respectively.
   

------------------------
Use for electron veto SF
------------------------

Use the tau electron veto scale factors to assign a weight for reconstructed
taus matched to a truth electron. The scale factors account for mismodeling of
MC and are derived from MC/Data comparison. To access their uncertainties during
the evaluation of systematics use the ``GetEvetoSFUnc`` method. The scale factors
depend on the leading track eta, tau ID working point, electron veto working
point and the level of overlap removal. The tau ID working points are stored as
enum, e.g. ``ID::BDTMedium``. Further enums are provided for the electron veto
working point (``EVETO::NONE``, ``EVETO::LOOSE``, ``EVETO::MEDIUM``,
``EVETO::TIGHT``) and the identification level of electrons used for overlap
removal (``OLR::OLRNONE``, ``OLR::LOOSEPP``, ``OLR::MEDIUMPP``, ``OLR::TIGHTPP``)

The following will return the SF for medium working points::

   float sf = tc->GetEVetoSF(leadTrkEta, ID::BDTMEDIUM, EVETO::MEDIUM, OLR::MEDIUMPP)

Only combinations of tau ID, eveto and overlap removal which are mention on the
TWiki are provided (others will result in segfault, so far :( ).
(more examples of usage in the 'examples' directory)

To access the uncertainty of the scale factor for systematics studies, use the
GetEVetoUnc method, which takes as an additional parameter the direction,
i.e. scale up (1) or down (-1)::

   float sf_unc_up = tc->GetEVetoSFUnc(leadTrkEta, ID::BDTMEDIUM, EVETO::MEDIUM, OLR::MEDIUMPP, +1) // up systematics
   float sf_unc_down = tc->GetEVetoSFUnc(leadTrkEta, , ID::BDTMEDIUM, EVETO::MEDIUM, OLR::MEDIUMPP, -1) // down systematics

.. note:: **The uncertainty is provided as nominal +- systematic**, not relative to
   the scale factor, i.e. in your analysis you need to do::

      weight *= scale_factor_unc

As observed in the SM electron can fake 3-prong taus as well. Corresponding
scale factors and uncertainties are now provided. They can be accessed via::

   float sf = tc->GetEVetoSF(leadTrkEta, numTrack, ID::BDTMEDIUM, EVETO::LOOSE, OLR::TIGHTPP)
   float sf_unc_up = tc->GetEVetoSFUnc(leadTrkEta, numTrack, ID::BDTMEDIUM, EVETO::LOOSE, OLR::TIGHTPP, +1) // up systematics
   float sf_unc_down = tc->GetEVetoSFUnc(leadTrkEta, numTrack, ID::BDTMEDIUM, EVETO::LOOSE, OLR::TIGHTPP, -1) // down systematics


 .. note:: The this method can be used for 1-prong tau candidates as well. The
    old accessors mentioned above are **only** wrappers to provide backwards
    compatibility and may become deprecated in the future. Hence users are
    admonished to change this sooner than later.
