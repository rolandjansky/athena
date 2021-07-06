Tagging efficiency SF provider
===================

Tagging efficiency SF is decorated to the jet.
[Note] Currently, development is ongoing only for DNN top tagger.


Step 1: Decprate the truth labeling
===================

SF is provided according to the jet truth labels.
Truth labels are defined as enum in BoostedJetTaggers/FatjetLabelEnum.h:
```
  enum TypeEnum
  {
    UNKNOWN=0, // Not tagged yet
    tqqb,      // full-contained top->qqb
    Wqq,       // full-contained W->qq
    Zqq,       // full-contained Z->qq
    Wqq_From_t,// full-contained W->qq (also mathced to top)
    other_From_t, // matched to top
    other_From_V, // matched to W/Z
    notruth,   // failed to truth-jet matching (pileup)
    qcd,       // not matched to top or W/Z (background jet)
    Hbb,       // full-contained H->bb
    other_From_H, // matched to Higgs
  };  
```

I is decorated to the given jet as integer by decorateTruthLabel( ) function in BoostedJetTaggers/JSSTaggerBase.h, which is called inside the tag() function, with the following convention:
```
  inline int enumToInt(const TypeEnum type)
  {
    switch (type)
      {
      case tqqb:         return 1;
      case Wqq:          return 2;
      case Zqq:          return 3;
      case Wqq_From_t:   return 4;
      case other_From_t: return 5;
      case other_From_V: return 6;
      case notruth:      return 7;
      case qcd:          return 8;
      case Hbb:          return 9;
      case other_From_H: return	10;
      default:           return 0;
      }
  }  
```
* First of all, DecorateMatchedTruthJet( ) function, defined in BoostedJetTaggers/JSSTaggerBase.h, is called to decorate trimmed truth jet associated with the given jet by dR<0.75. The function automatically identifies the format of the truth particle container (TRUTH1 or TRUTH3).
* If the matching to truth jet is failed, FatjetTruthLabel::notruth is docorated as the truth label.
* Then getWTopContainment( ) function is called to decorate truth labeling according to the definitions below.

Details of truth definitions
-----------------------------------
* FatjetTruthLabel::tqqb
1. Associated trimmed truth jet is matched to truth top quark by dR<0.75
2. GhostBHadronsFinalCount is greater than 0
3. Trimmed truth jet mass satisfies 140 < mJ < 200GeV

* FatjetTruthLabel::Wqq_From_t
1. Associated trimmed truth jet is matched to both truth top and truth W boson by dR<0.75
2. GhostBHadronsFinalCount is equal to 0
3. Trimmed truth jet mass satisfies 50 < mJ < 100GeV

* FatjetTruthLabel::Wqq
1. Associated trimmed truth jet is matched to truth W boson by dR<0.75 but not matched to truth top
2. GhostBHadronsFinalCount is equal to 0
3. Trimmed truth jet mass satisfies 50 < mJ < 100GeV

* FatjetTruthLabel::Zqq
1. Associated trimmed truth jet is matched to truth Z boson by dR<0.75
2. Trimmed truth jet mass satisfies 60 < mJ < 110GeV

* FatjetTruthLabel::Hbb
1. Associated trimmed truth jet is matched to truth H boson by dR<0.75
2. GhostBHadronsFinalCount is greater than 1

* FatjetTruthLabel::other_From_t
If trimmed truth jet is matched to truth top quark but does not satisfy the additional requirements above, FatjetTruthLabel::other_From_t is decorated.

* FatjetTruthLabel::other_From_V
If trimmed truth jet is matched to truth W or Z but does not satisfy the additional requirements above, FatjetTruthLabel::other_From_V is decorated.

* FatjetTruthLabel::other_From_H
If trimmed truth jet is matched to truth Hbut does not satisfy the additional requirements above, FatjetTruthLabel::other_From_H is decorated.

* FatjetTruthLabel::unknown
All jets not satisfying the above are defined as FatjetTruthLabel::unknown.

It is implemented in Root/JSSTaggerBase.cxx.


Sherpa V+jets
-----------------------------------
Sherpa 2.2.1 V+jets samples do not contain the intermediate truth W/Z boson information in the TruthParticles container.
We can look at the pair of truth particles with status==3, check the flavors of them, and reconstruct the mass of truth W/Z to define the truth W/Z boson.
The function matchToWZ_Sherpa( ) is called only when the flag isSherpa is turned on. The flag is turned on based on dataset ID defined in getIsSherpa( ) function in BoostedJetTaggers/JSSTaggerBase.h.
The DSID is needed to determine if the isSherpa flag should be switched on. This is done now automatically in the code and does not need to be provided anymore






Step 2: Decorate the SF
===================

SF is decorated to the given jet according to the truth definitions above.
getWeight( ) function is called inside the tag( ) function.

SF values, as functions of jet pT and m/pT, are provided by TH2 histograms in ROOT file.
Path of the ROOT file, as well as some parameters for SF provider, are specified in the config file of BoostedJetTaggers tool.
An example is found in share/JSSWTopTaggingDNN/JSSDNNTagger_AntiKt10LCTopoTrimmed_TopQuarkContained_MC15c_20170824_BOOSTSetup80Eff.dat.




Systematic uncertainties
===================

We plan to provide the systematic uncertainties using JetUncertainties tool.
[Not ready yet]




Test the tool
===================
To run the script to apply SF to tagged jets,
```
$ test_JSSWTopTaggerDNN -f your.test.DAOD.root
```
you can find output_JSSWTopTaggerDNN.root in your directory.
