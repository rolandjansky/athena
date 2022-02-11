#  TruthClassification

This repository contains the TruthClassification package, a tool for classifying electrons and muons based on their type and origin. Details about the classifications can be found in a variety of resources (e.g [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaTruthRun2), [here](http://cern.ch/go/8Qnf), [here](http://cern.ch/go/g6DR) and [here](http://cern.ch/go/9fBD)).

The purpose of the TruthClassification package is to categorize leptons into different background classes, which can be use e.g for investigation on lepton truth-compositions in the signal or control regions of an analysis or to measure the true real/fake efficiency or the true charge flip rates. It is using the classification scheme from the [MCTruthClassifier](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCTruthClassifier) and the origin/type of electrons/muon provided by the egamma and MCP groups. However, its purpose is to provide to only a reduced set of categories to the user, according to the physics process leading to prompt or non-prompt/fake leptons. 

The tool is supposed to run on derivations (e.g. **DAOD_PHYS/TOPQ/SUSY**), as it relies on truth-decorations on reco particles being applied. Right now, only classifications for electron and muons are fully supported. 

+ **Mailing list for technical questions about the tool:** atlas-phys-gen-IFF-leptonPhotonTools@cern.ch
+ **IFF mailing list (for general questions):** atlas-phys-gen-IsoFakeForum@cern.ch
+ Tool developers and maintainers: Fabio Cardillo, Otilia Ducu, Tom Neep (developer), Tadej Novak

 
---

1. [Input Variables and IFF Types](#1-input-variables-and-iff-types)
2. [Testing the TruthClassification](#2-testing-the-cruthclassification)
    - [AnalysisBase (EventLoop)](#analysisbase-(eventloop))
    - [Athena](#athena)
3. [Setting up and using the tool](#3-setting-up-and-using-the-tool)
    - [In AnalysisBase](#in-analysisbase)
4. [Details about the Lepton Categories](#4-details-about-the-lepton-categories)
    - [Prompt (isolated) electrons](#prompt-(isolated)-electrons)
    - [Prompt muons](#prompt-muons)
    - [Charge-flip electrons](#charge-flip-electrons)
    - [Prompt photon-conversions](#prompt-photon-conversions)
    - [Electrons from muons](#electrons-from-muons)
    - [Tau decays](#tau-decays)
    - [b- and c-hadron decays](#b-and-c-hadron-decays)
    - [Light-flavour decays](#light-flavour-decays)
    - [Charge-flip muons](#charge-flip-muons)
    - [KnownUnknown leptons](#knownunknown-leptons)
    - [Unknown leptons](#unknown-leptons)
    
---

## 1. Input Variables and IFF Types

The tool needs the following input variables from the lepton truth-record: `truthType`, `truthOrigin` (type/origin of input particle, as defined [here](https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/MCTruthClassifier/MCTruthClassifier/MCTruthClassifierDefs.h)), `firstEgMotherTruthType` (`lastEgMotherTruthType`), `firstEgMotherTruthOrigin` (`lastEgMotherTruthType`) and `firstEgMotherPdgId` (`lastEgMotherPdgId`) (origin, type and PDG-ID of first and last non-Geant mother-particle). Work in ongoing to include the fall back variables to improve the especially the unknown lepton classification; these variables are available only in SUSY2 derivations (see [talk](https://indico.cern.ch/event/862748/contributions/3639101/)). If the `lastEgMother`  or the fall back variables are not available in your derivations, the tool will still classify the leptons (but you might see a higher amount of leptons in the `KnownUnknown` category). The other decorations are mandatory, thus make sure they are available before running the tool. 

Based on this input, the function `StatusCode TruthClassificationTool::classify(const xAOD::IParticle &p, unsigned int &class)` will provide the following lepton categories (**IFF classes**):

+ Unknown
+ KnownUnknown
+ IsoElectron
+ ChargeFlipIsoElectron
+ PromptMuon
+ PromptPhotonConversion
+ ElectronFromMuon
+ TauDecay
+ BHadronDecay
+ CHadronDecay
+ LightFlavorDecay
+ ChargeFlipMuon


More details about the lepton categories can be found [here](#3-details-about-the-lepton-categories).

The types that deserve particular attention are the `Unknown` and `KnownUnknown` types. We want there to be no `Unknown` electrons or muons and so will print a warning if the classifier returns that type. If this happens please contact the IFF group to let them know! `KnownUnknown`s are things that we can't classify  because of missing information. This doesn't necessarily guarantee there isn't a  problem, but we don't print out a lot of warnings for these. Other types hopefully have somewhat intuitive names. If this isn't the case please get in touch.


## 2. Testing the TruthClassification 

### AnalysisBase (EventLoop)

In the non-athena releases, a test executable, `testTruthClassificationTool`, is compiled for you when building the package. It can be run via 
```
testTruthClassificationTool <my xAOD file> <no of events> 
```
The corresponding source file, `test/test_TruthClassificationTool.cxx` can be referred to for an example on tool setup in standalone jobs. 

### Athena 

When running in an athena environment, for example `AthAnalysis`, a test equivalent to the above is provided in form of the `IFFTruthClassifierTestAlg` algorithm.  A set of premade job options is included with the packaged that allow you to run this alg on an input file.  To do so, call 
```
athena --filesInput <my xAOD file>   IFFTruthClassifier/IFFTruthClassifierTest_jobOptions.py
```
Again, the corresponding sources (`src/IFFTruthClassifierTestAlg.cxx/h`) can be referred to as setup examples for athena. 


## 3. Setting up and using the tool

Since release [21.2.143](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/AnalysisBaseReleaseNotes21_2#21_2_143_built_on_2020_10_07) the tool is part of AnalysisBase. To use its functions, it can be simply added it to the header of an analysis framework.

```
#include "TruthClassification/TruthClassificationTool.h"

ToolHandle<CP::IClassificationTool> m_truthTool{"TruthClassificationTool/truthTool", this};
```
It may necessary to add `TruthClassification` to the `atlas_depends_on_subdirs()` of your `CMakeLists.txt` if you compile with cmake. The tool can be initialized in the following way:
```
AsgToolConfig config ("TruthClassificationTool");
ATH_CHECK(config.setProperty("separateChargeFlipElectrons", true));
ATH_CHECK(config.setProperty("separateChargeFlipMuons",     true));
ATH_CHECK(config.makePrivateTool(m_truthTool));
```

To retrieve the truth-categories for electrons of muons, the following can be done:
```
unsigned int IFFtype(0);
ATH_CHECK(m_truthTool.classify(*lep, IFFtype));
ATH_MSG_INFO("IFF truth class of lepton" << IFFtype);
```
Where `lep` is a pointer to a `xAOD::Electron` or `xAOD::Muon` object respectively.


## 4. Details about the Lepton Categories

The different lepton categories are defined following the recommendations from the egamma and MCP groups (see [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ElectronGamma) and [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MuonPerformance)), as well as studies performed inside the IFF. The following categories for electrons and muons are defined:

### Prompt (isolated) electrons

An electron is classified as prompt (or isolated) electron (**IFF class 2**), if its truth-type (or the type of its EG mother-particle) is corresponding to an isolated electron (`== 2`). Furthermore, if it or its mother-particle originates from an FSR photon (or bremsstrahlung) that subsequently converted, is is also classified as an isolated electron. This prescription can be found in the egamma CP recommendations for electron classification [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaTruthRun2#Background_Electron_Classificati). Electrons from prompt quarkonuim decays (e.g ccbar/bbbar -> e+ e-) are also entering this category.


### Prompt muons

An muon is classified as prompt (**IFF class 4**), if its truth-type is corresponding to an isolated muon (`== 6`), and it originates from a prompt source (e.g. W/Z, top, Higgs). More information can be found in the muon CP recommendations [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MuonTruthRun2). As for electrons, muons from prompt quarkonuim decays are considered as prompt.

### Charge-flip electrons

The distinction between electrons with correctly-assigned and mis-identified charge (charge-flip), is not relevant for all analyses. Thus, the separation between charge-flip electrons (**IFF class 3**) and prompt electrons, is an optional argument of the package: `separateChargeFlipElectrons` (`false` by default). Electrons are classified as charge-flip if the charge of the first non-Geant EG mother-particle is different than the reconstruction-level charge of the electron (see [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EGammaTruthRun2#Determining_the_truth_charge_of)).

Electrons originating from FSR are excluded, but studies in the egamma group showed that they produce electrons with correct or wrong charge with a 50:50 probability (see [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EgammaChargeMisIdentificationTool)). If the option `separateChargeFlipElectrons` is not enabled, all charge-flip electrons are considered as prompt, as explained [here](#prompt-(isolated)-electrons). 

### Prompt photon-conversions

Electrons originating from the conversion of prompt photons (**IFF class 5**) are identified if the truth-type corresponds to a background electron (`== 4`) and it originates from a photon conversion or an electromagnetic process. Also if the truth type corresponds to an isolated/background photon (`== 14/16`) and the origin is a prompt photon conversion or and unhadronised photon, this category is filled. It covers also two-step processes, such as H -> gg, g -> e+ e- by checking at the truth-type and origin of the first non-Geant mother-particle. Background electrons from photon conversions (with this info also for the EG first mother) with the last mum pdgID a photon or an electron are entering in this category, as detailed in [this report](https://indico.cern.ch/event/932298/#9-ifftruthclassifier-updates). The latest updates on the conversion-classification are summarised [here](https://indico.cern.ch/event/974759/contributions/4136470/attachments/2156133/3639731/IFFTruthClassifier_PhConv_25Nov.pdf).

No distinction between external (material) and internal conversions (depends on conversion radius) is performed in the current version of the tool. A manual separation of these conversion types can be done with the `DFCommonAddAmbiguity` flag, which should be available at derivation-level via the egamma smart slimming list. See [here](https://indico.cern.ch/event/862748/contributions/3635039/attachments/1946503/3235131/conversionsIFF2019.11.18.pdf) for more information. 

If this category is a major contribution in you analysis, you are encouraged to perform also some truth based studies to understand better the origin of these electrons. Please, give feedback to the IFF group.

### Electrons from muons

An electron is classified as a muon being reconstructed as electron (**IFF class 6**) if the truth-type of the electron corresponds to a non-isolated electron or photon (`== 3/15`) and its truth-origin is a muon. Unless such electrons are expected, if the [overlap removal](https://gitlab.cern.ch/atlas/athena/-/blob/master/PhysicsAnalysis/AnalysisCommon/AssociationUtils/README.rst) between electrons and muons is performed properly, the contribution from this category should be vanishing or very small. If this is not the case, please check it (see [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/OverlapRemoval#Feedback_from_PA_groups) for more details).

This category gathers also the electrons from muon brem. Such an example is presented in [this](https://indico.cern.ch/event/925595/contributions/3889713/attachments/2061466/3458234/IFF_2L2J_FNP_estimates.pdf) set of slides (by Eirik Gramstad), where the composition in a conversion enriched CR is shown; this CR is defined with a pairs of opposite-sign muons plus an extra electron.

### Tau decays

Non-isolated electrons and muons from hadronic tau-decays (**IFF class 7**), are identified if the truth-type of the leptons corresponds to a non-isolated muon (`== 7` for the muon case) or to a non-isolated electron/photon (`== 3/15` for the electron case) and its truth-origin is a tau lepton. Furthermore, the case of tau leptons radiating a photon which subsequent converts into an electron-position pair (tau -> tau+g, g -> e+ e-) is also entering this category. This is done by checking if the lepton comes from a photon conversion (as explained [here](#prompt-photon-conversion))  and if the first EG mother-particle is a non-isolated electron/photon originating from a tau lepton. Electrons and muons from leptonic tau-decays are entering the prompt categories in this classification scheme.

### b- and c-hadron decays

Electrons and muons originating from heavy-flavor decays can come from two types: b-hadrons (**IFF class 8**) and c-hadrons (**IFF class 9**) which are distinguished in the tool. It is checked if the truth-origin of an electron or muon corresponds to a bottom-meson or baryon (b-type) or to a charm-meson/baryon (c-type). For electrons, the check is also done for the origin of the first EG mother-particle (with the origin required to be b/c-type for either the EG mother- or the final-state particle). 

### Light-flavour decays

Leptons produced by light-flavor jets (**IFF class 10**) are identified by checking if the truth-type of the lepton corresponds to a hadron type (`== 17`). Also, if the truth-type corresponds to a background electron, muon or photon (`== 4/8/16`) and it originates from a light-meson, a strange-meson or light/strange-baryons, it is considered for the light-flavour category. For background electrons, the case of an intermediate photon-conversion (e.g. pi -> gg, g -> e+ e-) and Dalitz-decays are also considered for this category.

### Charge-flip muons

Muons with mis-identified charge can be selected. Similarly to electrons, it works via an extra option of the tool: `separateChargeFlipMuons` (`false` by default). If enabled, the tool will put (prompt) muons, where the reconstruction-level charge and the charge of the associated truth-particle are different, to a separate category (**IFF class 11**). This effect is expected to be extremely small compared to electrons and is not expected to have a sizable impact on physics analyses. It can, however, be used for specific studies on the underlying processes leading the charge-flip muons.

### KnownUnknown leptons

The `KnownUnknown` category (**IFF class 1**)  refers to leptons which can (in principle) be classified, but the tool fails with the classification due to missing information (e.g. the truth-type/origin correspond to the unknown (or non-defined) cases in the MCTruthClassifier (`== 0/1`).  This type of leptons is not recommended to be used (or to have a significant impact) for fake lepton estimations or efficiency measurements. Please contact the developers of the tool or the IFF contacts if these types occur frequently for the leptons in your analysis.  

### Unknown leptons

Leptons are entering the category `Unknown` (**IFF class 0**), if they cannot be attributed to any or the classes listed above. If leptons of this category are encountered in an analysis, the user should contact the IFF group (see mailing lists above) and report the details about the unclassified leptons (truth-origin/type, mother-particle-origin/type, PDG-ID, etc.), as well as the MC files where such leptons are found. The IFFTruhClassifier team will take care and classify these leptons. Merge requested proposed by the user with a fix are also higher encouraged!

Ideally, leptons should never fall into this category.
