# TrigTransform
##### Python package to run the Trigger Transform as used in Trigger Reprocessings and Debug Stream Recovery

### Trig_reco_tf.py
Based on Reco_tf.py this transform runs both rerunning the trigger and the reconstruction of this output.

- Rerunning the trigger can be done using athenaHLT or athena, and can be configured to run from the trigger database.
- Outputs are renamed by the transform in cases where the argument name isn't handled directly
- Reconstruction can be set to run in the same job or separately (at which point should be identical to running Reco_tf.py)

Full details of the transform are documented on the twiki:
https://twiki.cern.ch/twiki/bin/viewauth/Atlas/TriggerTransform

### Cost output

As well as rerunning the trigger, the outputs required for uploading to the trigCost website are produced as one of the sub-steps when running the
transform. These are produced starting from the DRAW_TRIGCOST file, which is the BS file containing the COSTMON stream

### Debug Stream Recovery

When running the debug stream recovery there is an additional output file produced from looking at the contents of the HLTResult before and after
rerunning the trigger. Also the input file is used to make sure exactly the same release is used to reprocess the data, unless any local patches are
included in the setup.

### Examples

The transform is tested in the nightly tests and these give examples of which options are to be used:
- [test_trigP1_v1PhysP1_T0MonTrf_build.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TrigValidation/TrigP1Test/test/test_trigP1_v1PhysP1_T0MonTrf_build.py) Running steps from BS as in Trigger Reprocessings
- [test_trigP1_v1PhysP1_trfDbgStream_build.py](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TrigValidation/TrigP1Test/test/test_trigP1_v1PhysP1_trfDbgStream_build.py) Running the transform in the debug recovery mode



