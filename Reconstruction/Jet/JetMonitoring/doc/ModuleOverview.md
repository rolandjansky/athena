Overview of Run3 JetMonitoring Tools
=====

This overview presents the basic functionality of the tools available in the Run3 JetMonitoring framework, as well as the helper functions / dictionaries used to simplify the monitoring config.
It only contains brief examples, taken from the [`TrigJetMonitorAlgorithm.py`](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TrigMonitoring/TrigJetMonitoring/python/TrigJetMonitorAlgorithm.py) and other standard configurations.

For more information on the functionality, structure, and usage of the Offline JetMonitoring tool, refer to the [`JetTriggerRun3OfflineMonitoring twiki`](https://twiki.cern.ch/twiki/bin/view/Atlas/JetTriggerRun3OfflineMonitoring).


[JetMonitoringConfig](../python/JetMonitoringConfig.py)
-----

Contains helper functions to simplify the creation of JetMonitoringAlg instances and histograms in `TrigJetMonitoriAlgorithm.py` or [`JetMonitoringStandard`](../python/JetMonitoringStandard.py). This is done via specifying dictionaries in your python config:
* `JetMonAlgSpec` creates a [`JetMonitoringAlg`](../src/JetMonitoringAlg.cxx) instance
* `ToolSpec` is a general dictionary to specify and call other tools (also any external athena tools)
* `VarSpec` is a dictionary to specify a jet variable based on existing decorations/variables of your jets; calls [`JetHistoVarTool`](../src/JetHistoVarTool.cxx)
* `HistoSpec` is a dictionary to create a histogram from a jet variable (implicitly or explicitly created by a `VarSpec`); calls [`JetHistoAttributeFiller`](../src/JetHistoAttributeFiller.cxx)
* `EventHistoSpec` is a dictionary to create a histogram from an EventInfo variable; calls [`JetHistoEventLevelFiller`](../src/JetHistoEventLevelFiller.cxx)
* `SelectSpec` is a dictionary that first applies a selection on your jets or events and then fills the histograms; calls [`JetHistoSelectSort`](../src/JetHistoSelectSort.cxx)

[JetStandardHistoSpecs](../python/JetStandardHistoSpecs.py)
-----

Contains commonly used instances of `VarSpec`, `HistoSpec`, and `ToolSpec` (esp. `EventHistoVarTool` and `NumJetVarTool`). These can be added directly via `appendHistos()` to your `JetMonAlgSpec` without needing to specify them further.

[JetMonitoringAlg](../src/JetMonitoringAlg.cxx)
-----

Processes the input jet collection and calls the specified tools to create and fill histograms from those jets. Tools are called via an interface to [`IJetHistoFiller`](https://gitlab.cern.ch/atlas/athena/-/blob/master/Reconstruction/Jet/JetMonitoring/JetMonitoring/IJetHistoFiller.h).

[JetVariable](../JetMonitoring/JetVariable.h)
-----

Interface class for the JetHistoVarTool. It accesses the variables/decorations of your jets and returns their value (or combines them into a more complex variable).
Examples:
```
  struct EtVar : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { return j.p4().Et()*m_scale;}
  };

  struct FChargedVar : public Variable {
    using Variable::Variable;
    virtual float value(const xAOD::Jet & j) const { 
      bool status = false;
      float constScalePt = 0.; 
      std::vector<float> SumPtChargedPFOPt500;
      status = j.getAttribute<float>("JetConstitScaleMomentum_pt", constScalePt ); // Jet pT at the constituent scale
      if (!status) return 0;
      status = j.getAttribute<std::vector<float> >("SumPtChargedPFOPt500", SumPtChargedPFOPt500 ); //Vector over all vertices in the event, each element contains the sum pT of all charged PFO with a pT > 0.5 GeV associated to the vertex.
      if (!status) return 0;
      return SumPtChargedPFOPt500.at(0)/=constScalePt; //definition of "fCharge", index 0 points to the primary vertex
    }
  };
```

This allows an easy access to non-trivial jet variables by simply providing a short string defined in [`JetVariable.cxx`](../src/JetVariable.cxx) to the `HistoSpec`.

[JetHistoVarTool](../src/JetHistoVarTool.cxx)
-----

Creates a jet variable from a `VarSpec` and returns its value using the `JetVariable` class above. Examples:
* EPS = VarSpec('EnergyPerSampling', 'vecfloat')
* JVF0 = VarSpec('JVF', 'vecfloat', 0)

[EventHistoVarTool](../src/EventHistoVarTool.cxx)
-----

Creates an EventInfo variable and returns its value. An example is defined in [`JetStandardHistoSpecs`](../python/JetStandardHistoSpecs.py) as: 
```
avgMu = ToolSpec('EventHistoVarTool', 'avgMu', Attribute='averageInteractionsPerCrossing') 
```

[NumJetVarTool](../src/NumJetVarTool.cxx)
-----

Returns the number of jets fulfilling specified pT, ET, and eta cuts. An example is defined in [`JetStandardHistoSpecs`](../python/JetStandardHistoSpecs.py) as: 
```
njetsPt20 = ToolSpec('NumJetVarTool', 'njetsPt20', PtCut=20.)
njetsEt330Eta0_32 = ToolSpec('NumJetVarTool', 'njetsEt330Eta0_32', EtCut=330., EtaMin=0., EtaMax=3.2)
```

[JetHistoAttributeFiller](../src/JetHistoAttributeFiller.cxx)
-----

Creates a 1D, 2D, or 3D histogram from a `HistoSpec`. Examples:
* HistoSpec( 'pt:GeV',  (100,0,750) , title='p_{T};p_{T} [GeV];')
* HistoSpec( 'pt:GeV;m:GeV',  (100,0,1000, 100,0,300) , title='mass vs p_{T};p_{T};mass [GeV];')
* HistoSpec( 'JVF[1]',  (100,0,1.2) , title='JVF for vtx 1;JVF[1];Entries', )

As shown in this last example, the index can be specified for vector variables.

[JetHistoEventLevelFiller](../src/JetHistoEventLevelFiller.cxx)
-----

Creates a 1D histogram from an `EventHistoSpec`. Currently existing histograms are average/actual mu, or jet multiplicity with various selection cuts. Example:
* EventHistoSpec('njetsPt20', (25,0,25), title='NJetsPt20;NJetsPt20;Entries' )

[JetHistoSelectSort](../src/JetHistoSelectSort.cxx)
-----

Creates histograms from a `SelectSpec`. The tool first applies selection cuts on jet or EventInfo variables, then fills specified histograms. Examples:
* SelectSpec( 'forward', '3.2<|eta|', path=... , FillerTools = \["pt","et","m"\] )
* SelectSpec( 'lowmu', 'avgMu<30', path=... , isEventVariable=True, FillerTools = \["pt","et","m","phi","eta"\] )

Multiple selections can be applied, simply separate them using '&', for example `'50<m:GeV&0.5<|eta|<1.5'`.
The jet collection can be sorted by a chosen variable before applying the selection, such as the jet mass.
The selection can be inverted, meaning the tool keeps only those jets/events that fail the selection.
If you want to select on an EventInfo variable, you must set `isEventVariable=True`.

[JetSelectorAttribute](../Root/JetSelectorAttribute.cxx)
-----

Tool that applies the selection on the jet variable specified in `SelectSpec`.

[JetEventSelector](../Root/JetEventSelector.cxx)
-----

Tool that applies the selection on the EventInfo variable specified in `SelectSpec`. Requires to set `isEventVariable=True` in the `SelectSpec`.

[JetMatcherAlg](../src/JetMatcherAlg.cxx)
-----

The JetMatcherAlg does what you'd think: it matches jets of one collection to jets of another. Currently it's possible to match `L1 jets / HLT jets / offline jets` to `HLT / offline jets`. The matching is done via dR matching (default is dR<0.3)

Example:
```
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  cfg = ComponentAccumulator()

  # Match HLT jets to offline jets
  UniqueAlgName = ...
  alg = CompFactory.JetMatcherAlg(UniqueAlgName, JetContainerName1=JetCollectionName,JetContainerName2=OtherJetCollectionName)
  # optional argument: JetCalibScale, for example JetCalibScale='EMScale'
  # OR if L1: alg = CompFactory.JetMatcherAlg(UniqueAlgName, L1JetContainerName1=L1JetCollectionName,JetContainerName2=JetCollectionName,MatchL1=True)
  cfg.addEventAlgo(alg)
  # OR if using main function: cfg.addEventAlgo(alg,sequenceName='AthMonSeq_TrigJetMonitorAlgorithm')
```

The variables that are monitored from the JetMatcherAlg are the difference as well as the response in pT, E, m; and the pT and eta of the reference jets (JetContainerName2). However, variables are only stored for a jet if it is matched to another jet.

[JetHistoMatchedFiller](../src/JetHistoMatchedFiller.cxx)
-----

Fills histograms for a jet collection that has been matched to another using the JetMatcherAlg above. Only histograms of variables monitored in the JetMatcherAlg can be created.

Example configuration:
```
conf = JetMonAlgSpec( ... )
def defineHistoForHLTJetMatch(conf, parentAlg, monhelper , path):
           # create a monitoring group with the histo path starting from the parentAlg
           group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+'/'+conf.bottomLevelDir+'/NoTriggerSelection/')
           # define the histograms
           for histname in [ 'ptdiff', 'energydiff', 'massdiff' ]: #defines which variable difference will be plotted
             group.defineHistogram(histname,title=histname, type="TH1F", path='MatchedJets_{}'.format(MatchedJetCollectionName), xbins=100 , xmin=-100000., xmax=100000. ,)
           for histname in [ 'ptresp', 'energyresp', 'massresp' ]:
             group.defineHistogram(histname,title=histname, type="TH1F", path='MatchedJets_{}'.format(MatchedJetCollectionName), xbins=100 , xmin=-2., xmax=2. ,)
           group.defineHistogram('ptresp,ptref;ptresp_vs_ptRef',title='ptresponse vs ptRef', type="TH2F", path='MatchedJets_{}'.format(MatchedJetCollectionName), xbins=10 , xmin=-2., xmax=2., ybins=10, ymin=0., ymax=500000.,)
           group.defineHistogram('ptresp,etaref;ptresp_vs_etaRef',title='ptresponse vs etaRef', type="TH2F", path='MatchedJets_{}'.format(MatchedJetCollectionName), xbins=10 , xmin=-2., xmax=2., ybins=10, ymin=-5., ymax=5.,)
       jetmatchKey      = '{}.matched_{}'.format(JetCollectionName,MatchedJetCollectionName) #we can get specific calibration scales by adding e.g. '_EtaJESScale' to the strings
       jetptdiffKey     = '{}.ptdiff_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetenergydiffKey = '{}.energydiff_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetmassdiffKey   = '{}.massdiff_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetptrespKey     = '{}.ptresp_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetenergyrespKey = '{}.energyresp_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetmassrespKey   = '{}.massresp_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetptrefKey      = '{}.ptRef_{}'.format(JetCollectionName,MatchedJetCollectionName)
       jetetarefKey     = '{}.etaRef_{}'.format(JetCollectionName,MatchedJetCollectionName)
       name = 'jetMatched_{}_{}'.format(JetCollectionName,MatchedJetCollectionName)
       conf.appendHistos(ToolSpec('JetHistoMatchedFiller',name,JetMatchedKey=jetmatchKey,JetPtDiffKey=jetptdiffKey,JetEnergyDiffKey=jetenergydiffKey,
                                  JetMassDiffKey=jetmassdiffKey,JetPtRespKey=jetptrespKey,JetEnergyRespKey=jetenergyrespKey,JetMassRespKey=jetmassrespKey,
                                  JetPtRefKey=jetptrefKey,JetEtaRefKey=jetetarefKey,
                                  defineHistoFunc=defineHistoForHLTJetMatch,Group='matchedJets_'+JetCollectionName)
       )

```

Admittedly it is a bit clumsy on the configuration side, but on the positive side it's also quite flexible and customizable.

[JetHistoTriggEfficiency](../src/JetHistoTriggEfficiency.cxx)
-----

This tool allows to measure the trigger efficiency of a jet chain and plot it in a histogram. Example configuration for large-R chains with a mass cut:

```
trigConf = JetMonAlgSpec( ... )
def defineHistoForJetTrigg(conf, parentAlg, monhelper , path):
       # create a monitoring group with the histo path starting from the parentAlg
       group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+JetCollectionFolderName+'/')
       # define the histogram; CAUTION: give them individual names so they don't overwrite each other, this is just an example!
       histname = "trigEff_vs_"+VariableName+"_offlineCut_et500"
       xbins, xmin, xmax = getBinningFromThreshold(JetChainName,VariableName)
       group.defineHistogram('trigPassed,jetVar;'+histname,title=histname, type="TEfficiency", path=JetChainFolderName, xbins=xbins , xmin=xmin, xmax=xmax ,)
if 'smc' in JetChainName:
     trigConf.appendHistos(SelectSpec( 'et500', '500<et:GeV&0.0<|eta|<3.2', JetChainFolderName, SelectedIndex=0, FillerTools = [
        ToolSpec('JetHistoTriggEfficiency', JetChainName+'_offlineCut_et500',
                 Group='jetTrigGroup_'+JetChainName+'_et500',
                 Var=retrieveVarToolConf("m"), # In this context we can not just pass a str alias to describe a histo variable
                 SortJets=True,
                 ProbeTrigChain=JetChainName,defineHistoFunc=defineHistoForJetTrigg),
        ])
```

As you can see, the tool itself is only responsible for monitoring the efficiency, thus it can be used as a histogram specification in the `FillerTools` options of a `SelectSpec`.

[JetHistoHTFiller](../src/JetHistoHTFiller.cxx)
-----

Tool to monitor the HT (scalar sum pT) of jets that pass a min pT cut and max Eta cut. Example configuration for HT chains:

```
trigConf = JetMonAlgSpec( ... )
if 'ht' in JetChainName or 'HT' in JetChainName:
     def defineHistoForHTChain(conf, parentAlg, monhelper, path):
         # create a monitoring group with the histo path starting from the parentAlg
         group = monhelper.addGroup(parentAlg, conf.Group, conf.topLevelDir+JetCollectionFolderName+'/')
         # define the histograms
         xbins, xmin, xmax = getHTBinning(chain,25) # helper function; second argument is bin width in GeV
         group.defineHistogram("jetHT;HT",title="Jet HT;H_{T} [GeV];Entries", type="TH1F", path=JetChainFolderName, xbins=xbins , xmin=xmin, xmax=xmax ,)
     trigConf.appendHistos(ToolSpec('JetHistoHTFiller','JetHistoHTFiller_'+JetChainName,MinPt=30.,MaxEta=3.2,FailureOnMissingContainer=False,
                                  defineHistoFunc=defineHistoForHTChain,Group='jetHT_'+JetCollectionName))
```
