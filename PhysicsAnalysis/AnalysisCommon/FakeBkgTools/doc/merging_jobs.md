## Merging jobs

When the processing of data files is split into several parallel tasks, the predicted background yields can only be combined trivially (by taking the sum over all jobs) for linear methods. To allow a parallel workflow for all methods, the tools allow the necessary information obtained from each job to be stored into ROOT files, which can then be merged and subsequently fed back into a new tool instance for the final computation. 

This is done simply by first calling, at the end of each parallel job's event loop, the function `saveProgress()` supplied with an open `TFile` pointer (or an arbitrary subdirectory within that file) where the intermediary results should be written: 
```c++
ATH_CHECK( tool.initialize() );
{ /// event loop
    ATH_CHECK( tool->addEvent(particles) );
}
auto directory = outputfile->mkdir("Fakes");
ATH_CHECK( tool.saveProgress(directory) );
}
```

then the output of all jobs should be merged with `hadd` into a single ROOT file. The final estimate can be obtained by setting up a new tool instance and pointing it to the proper ROOT file via the *ProgressFileName*. The tool configuration must be identical to that used in the parallel jobs. Example:
```c++
ATH_CHECK( tool.setProperty("ProgressFileName", "path/merged_data.root") );
ATH_CHECK( tool.setProperty("ProgressFileDirectory", "Fakes") );
ATH_CHECK( tool.initialize() );
float yield, statUp, statDown;
ATH_CHECK( tool.getTotalYield(yield, statUp, statDown) );
}
```

Systematic variations can be retrieved by calls to `applySystematicVariation()`. 

Histograms registered to the tools, in the parallel jobs, are saved as well. To retrieve the binned predictions, the user should create again histograms (**with identical names / binning**) and register them with the tool instance used to process the merged intermediary results before calling getTotalYield(). 