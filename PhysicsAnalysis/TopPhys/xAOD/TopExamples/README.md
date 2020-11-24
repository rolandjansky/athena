This package can be used to submit grid jobs with AnalysisTop and TOPQ datasets.
If you're looking for a tutorial on how to submit a grid job, you might like to try
 * https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopxAODStartGuideR21

The classes are flexible enough to allow the user to define new sample/derivation lists in a few lines.

Usage (for submitting grid jobs)

> `getExamples`
# This will copy the scripts using PathResolver to ${PWD}/grid

> `lsetup rucio pyami panda`
# These tools need to be available for the TopExample scripts to function

> `./01SubmitToGrid.py`
# This will run through the preconfigured list of samples and submit grid jobs
# You will want to edit this file to specify your own grid name, your own job suffixes and point to your cut file
# You may also want to define new samples (see MC16.py to see how they are added to the classes)

> `./02DownloadFromGrid.py`
> `./03LocalAnalysis.py --run`
> `./04Plot.py`

Usage (for making histograms)

```
- top::NoSelectionAnalysis: If the grid part didn't do any selection, use me!
- top::DileptonAnalysis: If the grid part did ee, mumu and emu, use me!
- top::LJetsAnalysis: For e+jets and mu+jets use me!
- top::LJetsTtresAnalysis: Includes largeR jets!
```

All the codes above use (mostly) the same histograms which are added and filled by using DefaultPlots.h. 
The event loop is run by mini-to-plots.cxx, something like this:
 
```
./mini-to-plots Dilepton /path/to/file/grid_datasetname/input.txt
```

The word Dilepton tells it to load the top::DileptonAnalysis.  You can see how this works in AnalysisTools.h addPlots().  The grid tag is extracted from the path, so that's important to keep in.
