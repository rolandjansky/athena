Tagging efficiency SF provider
===================

Tagging efficiency SF is decorated to the jet.
[Note] Currently, development is ongoing only for DNN top tagger.


Step 1: Decprate the truth labeling
===================

SF is provided according to the jet truth labels.
Truth labels are defined as enum in BoostedJetTaggers/FatjetTruthLabel.h
```
enum class FatjetTruthLabel : int {tqqb = 1, Wqq = 2, Zqq = 3, Wqq_From_t = 4, other_From_t = 5, other_From_V = 6, notruth = 7, unknown = -1};
const FatjetTruthLabel FatjetTruthLabel_types [] = {FatjetTruthLabel::tqqb, FatjetTruthLabel::Wqq, FatjetTruthLabel::Zqq, FatjetTruthLabel::Wqq_From_t, FatjetTruthLabel::other_From_t, FatjetTruthLabel::other_From_V, FatjetTruthLabel::notruth, FatjetTruthLabel::unknown};
```


It is decorated to the given jet by decorateTruthLabel( ) function in BoostedJetTaggers/JSSTaggerBase.h, which is called inside the tag() function.
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

* FatjetTruthLabel::other_From_t
If trimmed truth jet is matched to truth top quark but does not satisfy the additional requirements above, FatjetTruthLabel::other_From_t is decorated.

* FatjetTruthLabel::other_From_V
If trimmed truth jet is matched to truth W or Z but does not satisfy the additional requirements above, FatjetTruthLabel::other_From_V is decorated.

* FatjetTruthLabel::unknown
All jets not satisfying the above are defined as FatjetTruthLabel::unknown.

It is implemented in Root/JSSTaggerBase.cxx.


Sherpa V+jets
-----------------------------------
Sherpa V+jets samples do not contain the intermediate truth W/Z boson information in the TruthParticles container.
We can look at the pair of truth particles with status==3, check the flavors of them, and reconstruct the mass of truth W/Z to define the truth W/Z boson.
The function matchToWZ_Sherpa( ) is called only when the flag isSherpa is turned on. The flag is turned on based on dataset ID defined in getIsSherpa( ) function in BoostedJetTaggers/JSSTaggerBase.h.
You can give the dataset ID when you initialize the tool e.g.
```
m_tool->setProperty("DSID", 364100);
```
If you don't set DSID, isSherpa flag is automatically turned off.






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
