# TrigHTTSGInput
This package contains components necessary to generate HTT Simulation wrapper files
They work only in the RDO to ESD transform. That is they need both RDO input objects as well as the reconstructed objects. 

Example command:
```sh
Reco_tf.py --CA --preInclude "InDetConfig.ConfigurationHelpers.OnlyTrackingPreInclude" --inputRDOFile  RUN4_muons.RDO.pool.root --outputESDFile ESD.test.root --steering doRAWtoALL --postInclude "TrigHTTSGInput.TrigHTTSGInputConfig.TrigHTTSGInputCfg"
```
