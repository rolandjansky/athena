# Submitting jobs to the GRID with submissionTool

Table of Contents:
- [Submitting jobs for the nominal weight](#submitting-jobs-for-the-nominal-weight)
- [Submitting jobs for all the matrix-element weights at once](#submitting-jobs-for-all-the-matrix-element-weights-at-once)
- [Submitting jobs for multiple samples at once](#submitting-jobs-for-multiple-samples-at-once)
- [Submitting jobs using a custom rivet routine](#submitting-jobs-using-a-custom-rivet-routine)
		- [Submitting jobs using a custom sample](#submitting-jobs-using-a-custom-sample)
<br/>


# Submitting jobs for the nominal weight

Suppose you wanted to submit `rivet` jobs for an existing, validated analysis, for an official `EVNT` sample, and only running on the nominal weight.
Let us say for instance that one wanted to run the routine:
`ATLAS_2017_I1512451` (Z+Jets at 13 TeV) with a single V+jets slice, say DSID 364127

(Make sure you have correctly initiated your Grid proxy ("voms-proxy-init -voms atlas") and initiated `lsetup rucio` and `lsetup pathena`. 
 The setup scrpt should have done this for you, but if you have problems this would be the first thing to check)

```
# add your sample(s) to a text file
echo "mc15_13TeV:mc15_13TeV.364127.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV1000_E_CMS.evgen.EVNT.e5299" > sample.txt  
# then simply run 
submissionTool.py -i samples.txt -l mylabel  -a ATLAS_2017_I1514251 --nJobs 100 --nFilesPerJob 10 --noSyst
```

The tool will then loop through the samples(s) in `sample.txt` (specified by `-i/--inputSamples`) and submit a job for each one.
The `--noSyst` option ensures that only jobs for the nominal weight are submitted
The `-a/--rivetAnalysis` specifies which `rivet` routines you would like to run, and this option accepted comma-separated lists so you can run multiple analyses at once.
The `-l/--label` option adds a short string to the name of your jobs/output samples so you can recognise them.
The `--nJobs` and `--nFilesPerJob` are the same as for pathena.

If you wanted to test everything was working before submitting, you can add the `--dryRun` option, which prepares the job options but does not submit.
The job options file created is called soemthing like `RivetAnalysis_JO_noSyst_<DSID>_<label>.py`
If you also add the `--downloadSample` option, the tool will even download a test file for you, and add it to the config file.
The you can check everythign works locally by running:
```
athena RivetAnalysis_JO_noSyst_<DSID>_<label>.py
```

# Submitting jobs for all the matrix-element weights at once

Now, suppose you wanted to submit `rivet` jobs which run all the OTF-weights in a given sample rather than just the nominal.
The workflow is exactly the same as for [Submitting jobs for the nominal weight](#submitting-jobs-for-the-nominal-weight), but omitting the `--noSyst` option.

```
## add your sample(s) to a text file
cho "mc15_13TeV:mc15_13TeV.364127.Sherpa_221_NNPDF30NNLO_Zee_MAXHTPTV1000_E_CMS.evgen.EVNT.e5299" > sample.txt  
# then simply run 
submissionTool.py -i samples.txt -l mylabel  -a ATLAS_2017_I1514251 --nJobs 100 --nFilesPerJob 10 # --dryRun
```

By default, the tool will extract the DSID from each input sample name, and use `readDatabase.getWeights(dsid)` as described [here](#seeing-what-weights-are-available-for-a-given-dsid,-and-how-they-should-be-combined) 
  to extract the names of the available weights. If the sampel you requested is not yet included in the database, then the tool will instead try to download a test EVNT sample and manually check what weights are available.
  You can also force this behaviour using the `--ds/--downloadSample` option.
  ```
  submissionTool.py -i samples.txt -l mylabel  -a ATLAS_2017_I1514251 --nJobs 100 --nFilesPerJob 10 --downloadSample  # --dryRun
  ```

  The tool will create once instance of `rivet` for each OTF weight in the sample, and produce output files for each OTF-weight labelled by the weight name.
  Note that the time to complete each job grows linearly with the number of `rivet` instances/OTF-weights, so you may want to take this into account when choosing appropriate values for `--nJobs` and  `--nFilesPerJob`

# Submitting jobs for multiple samples at once

  If you wanted to run over samples with multiple slices, runnign over a single DSID is not enough.
  The workflow is exactly the same as for [Submitting jobs for the nominal weight](#submitting-jobs-for-the-nominal-weight), but adding one DSID name per line in the text file.
  For example, to run over all the slices of the Sherpa 2.2.1 Z(ee)+jets sample, you would run over `examples/submissionTool-examples/sherpa_vjets_samples.txt`:
  ```
  submissionTool.py -i examples/submissionTool-examples/sherpa_vjets_samples.txt -l mylabel  -a ATLAS_2017_I1514251 --nJobs 100 --nFilesPerJob 10 # --dryRun
  ```

# Submitting jobs using a custom rivet routine 

  If your routine is not one which comes bundled with `rivet`, then you can still submit it with the tool.
  Once you have tested your routine locally and compiled it eg with:
  ```
  rivet-buildplugin RivetAnalysis_MC_TTbar_TruthSel.so MC_TTbar_TruthSel.cc
  ```

  Then you can simply submit your routine by specifying the routine name using the `-a/--rivetAnalysis` option:
  ```
  submissionTool.py -i powheg_pythia8_ttbar_sample.txt -l ttbar_unvalRivet  -a MC_TTbar_TruthSel --nJobs 100 --nFilesPerJob 1  --extraFiles Steering_TTbar_Truth,RivetAnalysis_MC_TTbar_TruthSel.so --downloadSample
  ```

  In principle the tool will dynamically look for your routine's `.so` file if you followed the `RivetAnalysis_MC_TTbar_TruthSel.so` convention.
  Otherwise, you may want to add it to the job's list fo files to include using the `--ef/extraFiles` option, which can handles comma-separated values.
  You'd want to also include in this way any other required files, such as `yoda` or `plot`, or configuration files, which your routine may need.

  See the example `examples/submissionTool-examples/submitExample_CustomRivetRoutine.py` for a working example.

# Submitting jobs using a custom sample

  If you are using a custom sample, or set of samples, which is on `rucio`, the workflow is the same as in [Submitting jobs for all the matrix-element weights at once](#submitting-jobs-for-all-the-matrix-element-weights-at-once), just add the names of the samples to  a text file and feed that to the tool. The only difference is that the systematics database will likely not have the information about which OTF weight are available for these custom samples, so the easiest thing to do is force the `--downlaodSamples` option to retrieve a test file and check if there are any OTF weights, and create multiple instances of `rivet` in each job if needed. 
  ```
  submissionTool.py -i my_custom_sample.txt -l Custom_ZInc_sys -a MC_ZINC,ATLAS_2017_I1514251 --nJobs 100 --nFilesPerJob 1 --downloadSample
  ```
  A working example is provided in `examples/submissionTool-examples/submitExample_CustomSamples.py`

