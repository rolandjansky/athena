# SystematicsTool userguide

Table of Contents:
- [Prerequisites for using the systematics tool to evaluate uncertainties](#prerequisites-for-using-the-systematics-tool-to-evaluate-uncertainties)
- [I am using ROOT outputs from my analysis code](#i-am-using-root-outputs-from-my-analysis-code)
	- [Automatically merging per-OTF-weight analysis objects into per-variation (eg from my own ntuples)](#automatically-merging-per-otf-weight-analysis-objects-into-per-variation-eg-from-my-own-ntuples)
	- [Can I use the tool with CxAODReader-style outputs?](#can-i-use-the-tool-with-cxaodreader-style-outputs)
	- [What if I stored my weights by index rather than by name](#what-if-i-stored-my-weights-by-index-rather-than-by-name)
	- [How can I use the schema option to communicate my file structure/naming convention to the tool?](#how-can-i-use-the-schema-option-to-communicate-my-file-structurenaming-convention-to-the-tool)
	- [I can't figure out how to make the tool work with my analysis ntuples/root files](#i-can't-figure-out-how-to-make-the-tool-work-with-my-analysis-ntuplesroot-files)
	- [What if I want to make use of the other functionality of the tool, eg jobs submission, merging across jobs, slices, XS calculations and corrections, etc?](#what-if-i-want-to-make-use-of-the-other-functionality-of-the-tool-eg-jobs-submission-merging-across-jobs-slices-xs-calculations-and-corrections-etc)
		- [Using the PMGTruthWeightTool to automatically run AthAnalysis code once for each OTF weight](#using-the-pmgtruthweighttool-to-automatically-run-athanalysis-code-once-for-each-otf-weight)
	- [Merging GRID outputs using the systematicsTool](#merging-grid-outputs-using-the-systematicstool)
	- [Unpacking tarballs, merging across jobs for a give sample/slice or across slices of a sample](#unpacking-tarballs-merging-across-jobs-for-a-give-sampleslice-or-across-slices-of-a-sample)
- [I am using Rivet/YODA for truth-level studies or to prepare a rivet routine for my analysis](#i-am-using-rivetyoda-for-truth-level-studies-or-to-prepare-a-rivet-routine-for-my-analysis)
	- [I want to combine Rivet outputs (Yodas) from my GRID jobs](#i-want-to-combine-rivet-outputs-yodas-from-my-grid-jobs)
		- [Combining YODAs just for the nominal](#combining-yodas-just-for-the-nominal)
	- [Combining YODAs for all OTF-weights](#combining-yodas-for-all-otf-weights)
		- [Combining YODAs across slices of a sample](#combining-yodas-across-slices-of-a-sample)
	- [Combining YODAs for individual matrix-element weights into variations](#combining-yodas-for-individual-matrix-element-weights-into-variations)
<br/>

# Prerequisites for using the systematics tool to evaluate uncertainties

The systematics tool has been designed to help analysts evaulate their theory uncertainties automatically.
The tool should therefore be fairly adapatable within the following constraints:
- The tool works on `root` files, where your observables are binned in `TH1`s (or `TGraphs`)
- The tool cannot handle trees because the theory uncertainty prescriptions are not defined event-by-event. Your observables must be binned.
- For each observable, you need one copy of histogram per OTF weight
  - Say you wanted to look at leading jet pT, you would need ~100 copies of the lead jet pT histogram, one for each PDF, alphaS and scale weight
- You can use the PMG truthWeightsTool to propagate the weights from DAOD to your ntups (see section below). Or do it however you wish!
- Any sensible naming convention is fine: the tool can be configured using the schema option. eg `<INDIR>/<WEIGHTNAME>.root:<HISTNAME>` or `<INDIR>/myOutputFile.root:<HISTNAME>_<WEIGHTNAME>`
- Finally, you should keep track of the sum of processed weights for each OTF weight, so that weights can be correctly normalised. The easiest is to propagate the CutBookKeeper into your final outputs.

If you need an example of how to make an AthAnalysis job produce one copy of each hisrogram once per OTF-weight, you can follow the example in [Using the PMGTruthWeightTool to automatically run AthAnalysis code once for each OTF weight](#using-the-pmgtruthweighttool-to-automatically-run-athanalysis-code-once-for-each-me-weight)

# I am using ROOT outputs from my analysis code

## Automatically merging per-OTF-weight analysis objects into per-variation (eg from my own ntuples)

This is most likely the main use case of the `systematicsTool.py` for analysts outside the PMG. Each analysis is liekly to have it's own way of producing final analysis objects, but so long as they follow the prerequisites described above, it should still be possible to use the tool to combine.

The easiest way to do this is in a custom python script. Here I am assuming that the input is `root` file (or several root files) which have been merged across jobs, normalised by cross-section, and merged across (sub)samples. 

An example python script would look like this (the example should work if you want to run it!):
```
#!/usr/bin/env python

import systematicsTool as st #import the systematicl tool 
import readDatabase as rdb #import the systematic database
import os,sys



indir="/afs/cern.ch/user/l/lcorpe/work/public/SystToolsUnitTests/CxAODReader_VVSemileptonic"
outdir="output_dir"

inF="%s/testfile.root"%indir
regexFilter=".*ZCROfRSG.*" #you filter for only certain histogram names
regexVeto=None #you can veto any histogram names you wish to skip

weightList=rdb.getWeights('364120')[0] #retrieve the weights for this samples and their combination recipes
print weightList
schema="!INDIR/!INFILE.root:Sys!NSFWEIGHTNAME__1up/!AONAME_Sys!NSFWEIGHTNAME__1up" #define the schema, ie the naming convention of how you stored the analysis objects in your files(s) - see dedicated section on this topic in the documentations
os.system("mkdir -p %s" %outdir)

result= st.combineAllVariations(weightList, indir, outdir,regexFilter=regexFilter, regexVeto=None, returnOnlyVariationsInComination=True, schema=schema, inFile="testfile.root",) #automagically combine the per-weight histograms into per-variation analysis objects!

#optionally make some nice plots
plots=st.makeSystematicsPlotsWithROOT(result, outdir, nominalName='_Nominal', label="CxAODOutput_" ,ratioZoom=None, regexFilter=regexFilter, regexVeto=None)

```

And you are done! The trickiest part figuring out what schema to use: see [How can I use the schema option to communicate my file structure/naming convention to the tool?](#how-can-i-use-the-schema-option-to-communicate-my-file-structure/naming-convention-to-the-tool?) for more detail.

## Can I use the tool with CxAODReader-style outputs?

Yes ! See this example : https://gitlab.cern.ch/atlas-physics/pmg/tools/systematics-tools/blob/master/examples/systematicsTool-examples/CxAODOutputExample.py

## What if I stored my weights by index rather than by name

This is not recommended. It's much safe and less prone to mistakes to store weight by name. But if you did, you can provide the tool wil an ordered list of weight names (so it can map index<->name) and then you can proceed as usual:

```
#!/usr/bin/env python
import os, sys
import systematicsTool as st
import readDatabase as rdb



indir="/afs/cern.ch/user/l/lcorpe/work/public/SystToolsUnitTests/RootFormatSingleFile/"
inFile="DE_c2_M600.root"

weightList=rdb.getWeights('309126')[0]
schema="!INDIR/!INFILE.root:!AONAME_!WEIGHTINDEX"
orderedWeights= [' mur=0.5 muf=0.5 ', ' alpsfact=0', 'Member 89', 'Member 90', ... , 'Member 86', 'Member 87', 'Member 88', ]

result= st.combineAllVariations(weightList, indir, outdir, regexFilter=None, regexVeto=None, combinationRecipe=None, returnOnlyVariationsInComination=True, schema=schema, orderedWeights=orderedWeights, inFile=inFile)

```

See here for a complete working example : https://gitlab.cern.ch/atlas-physics/pmg/tools/systematics-tools/blob/master/unitTests/RootFormatSingleFile.py

## How can I use the schema option to communicate my file structure/naming convention to the tool?
  
  Of course, different analysis outputs will have different file structures. The tool is designed to be quite flexible and to be configurable to read many different file structures.
  
  In the example above the `schema` is 
  ```
  !INDIR/!INFILE.root:!AONAME__!WEIGHTNAME
  ```
  which indicates that all the per-weight copies of each analysis object (=histograms) live in the same directory (!INDIR) and file (!INFILE) and are labelled with the analysis object name (!AONAME), plus two underscores followed by the weight name (!WEIGHTNAME). The tool assumes that the weight names are 'file name safe', ie spaces and special characters have been replaced.
  
  More compicated schemas can be used, for example, this example is from `CxAODReader` output:
  ```
  !INDIR/!INFILE.root:Sys!NSFWEIGHTNAME__1up/!AONAME_Sys!NSFWEIGHTNAME__1up
  ```

  In this case, each analysis object (=histograms) live in the same directory (!INDIR) and file (!INFILE), but there is a separate folder in the root file for each weight (plus the "Sys" and "__1up" strings) and the actual histograms in each folder are labelled by name (!AONAME) and their weight (!NSFWEIGHTNAME). Note that !NSFWEIGHTNAME differs from !WEIGHTNAME only in that spaces and special characters have not been changed (NSF stands for 'not-safe-filename').
  
  One can also imagine having a separate input file for each variation, where the analysis objects are labelled indentically in each one. In that case the schema might be 
  ```
  !INDIR/!INFILE__!WEIGHTNAME.root:!AONAME
  ```

  Or perhaps the histograms are labelled by weight index instead of name (not recommended!), in which case you'd need to provide an orderer list of weights to the tool, and the `schema` would be:  
  ```
  !INDIR/!INFILE.root:!AONAME__!WEIGHTINDEX
  ```

  No other keywords aside from those listed in this section are implemented at the moment: hopefully this is already flexible enough to  meet analysts needs but more can be added upon requets by contacting `lcorpe@cern.ch` or creating a git issue for this repo.



## I can't figure out how to make the tool work with my analysis ntuples/root files

If nothing on this page helps, contact `lcorpe@cern.ch` or make a git issue in this repo, and I'll be happy to help!

## What if I want to make use of the other functionality of the tool, eg jobs submission, merging across jobs, slices, XS calculations and corrections, etc?

See below!

### Using the PMGTruthWeightTool to automatically run AthAnalysis code once for each OTF weight

The later stages of the workflow in this section are designed to work with most reasonable formats, but here is a basic example to get `AthAnalysis` to run over all OTF weights in a file and make a copy of the output histograms for each weight:
https://gitlab.cern.ch/atlas-physics/pmg/tools/systematics-tools/tree/dev/source/ExampleDAODAnalysis/src

To compile the example, try:
```
setupATLAS
mkdir -p build
cd build
acmSetup AthAnalysis,21.6.2
acm compile
cd ..
```

In particular, see how the PMGTruthWeightTool is included and initalised in the `.h`:

```
#include "PMGAnalysisInterfaces/IPMGTruthWeightTool.h"
(...)


std::string  m_stream = "MYSTREAM"; // This will be eventually replaced by the OTF-weight-name in the job options
std::string  m_mcweight_name = "";

asg::AnaToolHandle<PMGTools::IPMGTruthWeightTool> m_weightTool;

```

and then in the `.cxx`:

```
//in the initialisation
declareProperty( "Stream",m_stream="STREAM",
    "Name of the stream to write this isntance of the Algo to (eg Weight Name)" );

//initialise each histogram, where m_stream will eventually be given by the OTF-weight-name
m_leadJetPt = new TH1D(TString(m_stream+"__leadJetPt"),TString(m_stream+"__leadJetPt"),100,0,2000);
 CHECK( histSvc()->regHist("/STREAM/"+m_stream+"__leadJetPt", m_leadJetPt) ); //registers histogram to output stream


m_weightTool.setTypeAndName("PMGTools::PMGTruthWeightTool/PMGTruthWeightTool");
CHECK(m_weightTool.retrieve());

(...)

//per-event
auto weight = m_weightTool->getWeight(m_mcweight_name); //mc_weight_name is given in the python config, different for eahc instance which is run

(...)

const xAOD::JetContainer* jets = 0;
CHECK(evtStore()->retrieve( jets, "AntiKt4TruthWZJets" ));
for (auto jet : *jets){
  if( jet->pt() > 10000. && fabs(jet->eta()) < 2.8   ){ 
    m_leadJetPt->Fill(jet->pt() *0.001, weight); // fill with the weight for this OTF-weight, the corresponding histogram picks up the OTF-weight name
  }
  break;
}


```

and finally, to run the job, one needs a python config like this one:
https://gitlab.cern.ch/atlas-physics/pmg/tools/systematics-tools/blob/dev/source/ExampleDAODAnalysis/share/ExampleDAODAnalysisAlgJobOptions.py

where one instance of the athena job is set up for each OTF weight in the file:
```
systWeights={ 'MUR1_MUF1_PDF261039': 49, ..., 'MUR1_MUF1_PDF261056': 66, 'MUR1_MUF0.5_PDF261000': 6} # you can do this automatically with the tool
(...)
for sName,sIndex in systWeights.iteritems():
   systName=safeFileName(sName) #remove spaces and special characters
   thisAlg=CfgMgr.ExampleDAODAnalysisAlg(systName)     
   thisAlg.Stream=systName #set the stream (ie histo names) to the safe-file-name version of the syst name
   thisAlg.MCWeightName =sName # set the exact syst name (can have spaces and special characters)
   athAlgSeq +=  thisAlg # add another instance of your code to the athena job, one each for each me-weight

```

One can then finally submit the jobs and run them as in this file: https://gitlab.cern.ch/atlas-physics/pmg/tools/systematics-tools/blob/dev/examples/submissionTool-examples/submitExample_AthAnalysis.py
```
submissionTool.py -i sherpa_vjets_samples.txt --label DAODsubmissionTest --jo source/ExampleDAODAnalysis/share/ExampleDAODAnalysisAlgJobOptions.py --dryRun
```

where `sherpa_vjets_samples.txt` is a list of files to run over.

## Merging GRID outputs using the systematicsTool

A more important topic is how to merge GRID outputs from jobs, and combine per-matrix-element histograms into a single histogram/graph per systematic variation. For example, how to get from 101 PDF weights to a single PDf uncertainty, automatically.

## Unpacking tarballs, merging across jobs for a give sample/slice or across slices of a sample

If you followed the example above, and once the jobs have finished running, you can download them using rucio
```
mkdir samples
cd samples
rucio list-dids user.lcorpe:user.lcorpe*DAODsubmissionTest* --short --filter type=CONTAINER |grep -v log > out.dl
while read  p; do echo $p ; rucio download $p ; done < out.dl
cd -
```

you then should have a directories full of tarballs.

You can use the following command:
```
systematicsTool.py --directory samples --process .*Zee.*AOD.*:Zee --generator .*Sherpa_221.*:Sherpa -l xAODExampleAnalysis_  -j8 --plotsDir %s --schema !INDIR/!INFILE.root:!AONAME__!WEIGHTNAME
```

  What's going on in this syntax?
  - `--directory` tells the tool where the `rucio` downloads were downloaded to.
  - `--process` and `--generator` are used to indentify the files in the inoput directory which should be merged. The tool will create a regex to filter just for directories which match `.*generator.*process.*`. Both of these arguements can also support more complicated regexes, in the format `--process <regex>:<labe>` (same for `--generator`), eg `.*Sherpa_221.*:Sherpa`
  - `-l/--label` add a label to the output files so you can tag them 
  - `-j/--nThreads` the executable is multi-threaded, so you can specify if you want it to run on multiple cores to increase speed.
  - the `--schema` option tells the `systematicsTool` how your inputs are structured. See the dedicated section on that topic below.
  
  
  The tool will then go through and :
  - untar your job outputs
  - merge the nominal `root` files across jobs, for each slice of samples passing the regex
  - extract the DSID of each slice and retrieve the cross-section from the PMG database
  - merge the `root` files across slices according to the cross-section, and number of events processed in each slice
  - make some output plots of the distributions in the final `root` file



# I am using Rivet/YODA for truth-level studies or to prepare a rivet routine for my analysis

## I want to combine Rivet outputs (Yodas) from my GRID jobs


  When trying to produce a `yoda` file for a given MC prediction, with theory uncertainties, it is not trivial to keep track of the files and merge the weights correctly. The `systematicsTool.py` tool helps to accomplish the following steps:

  - Untar outputs from each job: eg, O(100) yoda files per job (one for each variation), depending on the OTF weights
  - `yodamerge` outputs of same type from within job from a given DSID
  - Apply on-the-fly correction to the variations to account for the fact that Rivet normalises by nominal XS rather than varied XS! https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PmgSystematicUncertaintyRecipes#On_the_fly_systematic_variations
- (Should not be needed when Rivet with multiweights is introduced)
  - Scale by DSID cross-section and merge for yoda of each variation type
  - Correctly combine O(100) weights into a systematic variations. Apply 7-point variation for scale, etc...
  - Produce a total uncertainty band
  - Plot the variations for your process...

  Once all or some of your jobs have finished running, you can download them using `rucio` in the usual way. Something like:
  ```
  rucio list-dids user.lcorpe:user.lcorpe.mc15_13TeV.*.Sherpa_221_NNPDF30NNLO_Zee*RIVET.ZJ_database_EXT0* --filter type=CONTAINER --short > out.dl
  while read p ; do rucio download --nrandom 10 $p; done < out.dl
  ```
  This will leave you with a bunch directories full of tarballs for each job. So how to get these tarballs and merge them across jobs and across samples to get your final prediction?

### Combining YODAs just for the nominal

  The easiest way to do this is using the `systematicsTool.py` as an executable like so:
  ```
  systematicsTool.py --directory . --process Zee --generator Sherpa -l mylabel -j8 --noSyst
  ```

  What's going on in this syntax?
  - `--directory` tells the tool where the `rucio` downloads were downloaded to.
  - `--process` and `--generator` are used to indentify the files in the inoput directory which should be merged. The tool will create a regex to filter just for directories which match `.*generator.*process.*`. Both of these arguements can also support more complicated regexes, in the format `--process <regex>:<labe>` (same for `--generator`), eg `.*Sherpa_221.*:Sherpa`
  - `-l/--label` add a label to the output files so you can tag them 
  - `--noSyst` just run on the nominal weight. If you submitted your jobs with the `--noSyst` option, then the nominal weight is usually not named, or just called `nominal`. If your nominal weight has a different name, or you want to treat a different weight as nominal, you can specify it with `--nw/--nominalWeight`
  - `-j/--nThreads` the executable is multi-threaded, so you can specify if you want it to run on multiple cores to increase speed.

  The tool will then go through and :
  - untar your job outputs
  - merge the nominal `yoda` files across jobs, for each slice of samples passing the regex
  - extract the DSID of each slice and retrieve the cross-section from the PMG database
  - merge the `yoda` files across slices according to the cross-section, and number of events processed in each slice
  - make some output plots of the distributions in the final `yoda` file

  See a working example here: `examples/systematicsTool-examples/usingExecutable_ZJetsExample.py`

  If you prefer not to use the executable, you can instead import the most useful functions of `systematicsTool.py` directly into your own `python` script.

  ```
  import systematicsTool as st 
  indir=<directory where you downloaded your grid jobs to>
files= st.extractTarballsFromDirectory(indir)
  ```

  Once they have been untarred, the `yoda` files can be merged manually using `yodamerge` or similar.

  See a complete working example here: `examples/systematicsTool-examples/extractTarballsAndMergeAcrossJobs.py`

## Combining YODAs for all OTF-weights

  If you wish to combine all the OTF weight rather than just the nominal, and additionally convert the merged-per-OTF-weight `yoda` files into a `yoda` per variation, then you can use the same syntax as the [Combining YODAs just for the nominal](#combining-yodas-just-for-the-nominal) example, but omitting the `--noSyst` option:
  ```
  systematicsTool.py --directory . --process Zee --generator Sherpa -l ZJ_database -j8
  ```

  In addition the the steps described in [Combining YODAs just for the nominal](#combining-yodas-just-for-the-nominal), the tool will:
  - merge across jobs for each OTF-weight, rather than just for the nominal
  - merge across slices for each OTF-weight, rather than just the nominal
- use the information from the systematics datase (`readDatabase.getWeights(dsid)`) 

  Once again, this can also be done within your own python script. 
  To unpack tarballs and merge across jobs, see  a working example here: `examples/systematicsTool-examples/extractTarballsAndMergeAcrossJobs.py`


  Then, to combine the individual OTF-weighted `yodas` into per-variation `yodas`, you would do something like:
  ```
  import systematicsTool as st 
  import readDatabase as rdb

  weightList=rdb.getWeights(364116)[0]  #list of variations for that DSID, and how to combine the weights
  indir=<directory containing the merged per-OTF yodas>
  outdir=<where the put the final per-variation yodas>
  result= st.combineAllVariations(weightList, indir, outdir)
  ```

  A complete working example can be found here `examples/systematicsTool-examples/combineWeightsIntoVariationsAndPlot.py`

### Combining YODAs across slices of a sample

  If you are using `systematicsTool.py` as an executable, then the syntax is the same as for [Combining YODAs for all OTF-weights](#combining-yodasfor-all-me-weights). 
  ```
  systematicsTool.py --directory . --process .*Zee.*database.*:Zee --generator .*Sherpa_221.*:Sherpa -l ZJ_database -j8
  ```

  The important point is that the `--process` and `--generator` regexes match the slices you are interested in. The tool will build a list of the files matching the regexes and extract the DSIDs, look up the cross-sections, and merge teh subsamples appropriately. For OTF-weights, one needs to do a cross-section correction to make sure the variation is with respect to the nominal. This will be done automatically.

You may of course do this manually too in your own `python`, but the book-keeping can be a bit of a nightmare (thus the appeal of the tool!)

## Combining YODAs for individual matrix-element weights into variations

  If you are using `systematicsTool.py` as an executable, then just follow the example from [Combining YODAs for all OTF-weights](#combining-yodasfor-all-me-weights). 
  In addition to merging the `yoda` files, it will combine the merged `yodas` for each variation 

