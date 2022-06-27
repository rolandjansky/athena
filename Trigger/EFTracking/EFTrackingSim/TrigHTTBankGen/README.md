## TrigHTTBankGen

Last update: June 17,2022

This package is used to make banks (fit constants, sectors) for the EF Tracking system (a la HTT). The instructions are for 21.9 and will need to be updated when we have files for Rel 22

### Files

#### Matrix generation

Matrix files accumulate track information from training muons, for generating sectors and constants. Matrix files condense information from HITS files to only what we need, but can still be merged with other matrix files to increase statistics (whereas the actual constants files can't be merged).

* HTTMatrixGenAlgo: Core matrix file generation code.
* HTTMatrixMergeAlgo: Algorithm to merge matrix files.
* HTTMatrixReductionAlgo: Reduces second-stage matrices to first-stage ones.
* HTTMatrixAccumulator: The actual "matrix" object, which is stored in the matrix files.
* HTTMatrixIO: Utility functions to read and write matrix files.

#### Fit constants generation

* HTTConstGenAlgo: Main algo to generate fit constants from matrix files. Also dumps sector definitions and slices.



### Workflow
Below is the workflow for how to start with HITS files and end up with final pattern banks (and all the other files produced along the way).
Please read carefully!
For more information on each step, you can look at the docstrings in the transforms and the C header files, and also run with the `-h` flag in all transforms.

#### Step 0 (Setup and starting with HITS with single muons)

The code assumes you have 21.9 checked out and you're using: *asetup 21.9.2,Athena,here*

The instructions assume that you have HITS with single muons. If you do not, you can see instructions here:
https://twiki.cern.ch/twiki/bin/view/Atlas/FTKL1TrackSoftwareIntegration#Generating\_Banks
or look for samples here:
https://its.cern.ch/jira/browse/ATDUHWTT-44

#### Step 1 (on the grid) to produce matrix files
This assumes you are using input single muons from data set user.bstamas.20SingleMuonsL1Track.retry.HITS\_EXT0. Note that you may also want to change the number of events per job and per file.


```sh
inDS=user.bstamas.20SingleMuonsL1Track.retry.HITS_EXT0
outDS=user.$USERNAME.singlemuons.13L.v1
pathena \
    --outDS $outDS \
    --inDS $inDS \
    --extOutFile=matrix_13L_18Dim.root \
    --trf="Reco_tf.py
        --inputHITSFile=%IN
        --outputRDOFile=%OUT.RDO.L1Track.pool.root
        --outputESDFile=%OUT.ESD.L1Track.pool.root
        --conditionsTag=default:OFLCOND-MC15c-SDR-14-03
        --geometryVersion=default:ATLAS-P2-ITK-20-00-00
        --digiSteeringConf=StandardInTimeOnlyTruth
        --autoConfiguration=everything
        --postInclude 'all:InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.DigitalClustering.py'
        --preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock(\"geant4\");from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption=\"InclinedDuals\"; from RecExConfig.RecFlags import rec; rec.doCalo.set_Value_and_Lock(False); rec.doMuon.set_Value_and_Lock(False);rec.doJetMissingETTag.set_Value_and_Lock(False);rec.doEgamma.set_Value_and_Lock(False);rec.doMuonCombined.set_Value_and_Lock(False); rec.doTau.set_Value_and_Lock(False); rec.doTrigger.set_Value_and_Lock(False); rec.doTagRawSummary.set_Value_and_Lock(False)' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=[\"SimLayout\",\"PhysicsList\"];' 'RAWtoESD:rec.UserAlgs=[\"TrigHTTBankGen/r2e_HTTBankGenITk_joboptions.py\"]'
        --preInclude 'all:from AthenaCommon.DetFlags import DetFlags;DetFlags.ID_setOn();DetFlags.TRT_setOff();DetFlags.detdescr.TRT_setOff();DetFlags.makeRIO.TRT_setOff();DetFlags.BCM_setOff();DetFlags.detdescr.BCM_setOff();DetFlags.makeRIO.BCM_setOff();DetFlags.DBM_setOff();DetFlags.detdescr.DBM_setOff();DetFlags.makeRIO.DBM_setOff();DetFlags.Calo_setOff();DetFlags.Muon_setOff();DetFlags.Truth_setOn();DetFlags.simulate.TRT_setOff();DetFlags.simulate.BCM_setOff();DetFlags.simulate.DBM_setOff(),InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py' 'default:InDetSLHC_Example/preInclude.SLHC.py,from AthenaCommon.DetFlags import DetFlags;DetFlags.ID_setOn();DetFlags.TRT_setOff();DetFlags.detdescr.TRT_setOff();DetFlags.makeRIO.TRT_setOff();DetFlags.BCM_setOff();DetFlags.detdescr.BCM_setOff();DetFlags.makeRIO.BCM_setOff();DetFlags.DBM_setOff();DetFlags.detdescr.DBM_setOff();DetFlags.makeRIO.DBM_setOff();DetFlags.Calo_setOff();DetFlags.Muon_setOff();DetFlags.Truth_setOn();DetFlags.simulate.TRT_setOff();DetFlags.simulate.BCM_setOff();DetFlags.simulate.DBM_setOff(),InDetSLHC_Example/preInclude.SLHC.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py'
        --postExec 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=[\"HepMcParticleLink\"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;'
        --DataRunNumber '240000'" \
   --split 8000 \
   --nJobs 8000 \
   --nEventsPerFile 40000 \
   --nEventsPerJob 20000
```

#### Step 2 Matrix file merging
You could run this step on the grid and then merge a smaller number of jobs locally, which might make your workflow easier.

```sh
PATHTOFILES='/home/jahreda/xdata/httsim_bankgen_check/merge/user*roo*'
HTTMatrixMerge_tf.py \
    --NBanks 96 \
    --genconst False \
    --HTTMatrixFileRegEx  $PATHTOFILES \
    --MaxInputFiles 5000 \
    --bankregion 0 \
    --outputMergedHTTMatrixFile combined_matrix_13L_18Dim.root
```

#### Step 3 Produce constants (1nd and 2nd stage) including reduced matrix file
Note that you will want to change the path to the merged matrix file from above. You should also check that the pmap files for 8L and 12L are the ones that you want, and compatible with the above steps. For more details on the pmap, see https://twiki.cern.ch/twiki/bin/view/Atlas/FTKConfigFiles#Plane_map_PMAP

```sh
PATHTOMERGEDMATRIX='/xdata/jahreda/httsim_bankgen/merge/combined_matrix_13L_18Dim.root'
HTTConstReduceConstGen_tf.py \
    --NBanks 96 \
    --bankregion 0 \
    --pmap_12L raw_FTKppExample.pmap \
    --pmap_8L L1TrackExample.pmap \
    --allregions False \
    --inputHTTMatrixFile $PATHTOMERGEDMATRIX \
    --outputHTTGoodMatrixReducedFile good_matrix_myreduced.root
```
