Quick Instructions
==================

* Fork the Athena project  https://gitlab.cern.ch/atlas/athena
* Add `atlasbot` as `Developer` to your project (Project Settings -> Members)
* `mkdir Athena; cd Athena`
* `setupATLAS`
* `asetup AthDerivation,21.2,latest`
* `lsetup git`
* Instead of a full checkout:
```
git clone https://:@gitlab.cern.ch:8443/[YOUR_USER_NAME]/athena.git
cd athena
git remote add upstream https://:@gitlab.cern.ch:8443/atlas/athena.git
git remote -v show
```
we perform a "sparse" checkout
```
git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
```
* `cd athena/`
* `git fetch upstream`
* `git atlas addpkg DerivationFrameworkSUSY`
* `git checkout -b your_branch_name upstream/21.2 --no-track`
* `git branch` to make sure that you are indeed staninding on your branch
* `cd ../`
* `mkdir run build; cd build`
* `cmake ../athena/Projects/WorkDir`
* Compile `make -j$(nproc)'
* `source x86_64-slc6-gcc62-opt/setup.sh`
* `cd ../run/`
* Modify things and run
```
INPUT=AOD.input.pool.root
OUTPUT=AOD.output.pool.root
TRAIN="SUSY18"
NEVT=1000
Reco_tf.py \
    --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' \
    --inputAODFile $INPUT \
    --outputDAODFile $OUTPUT \
    --maxEvents $NEVT \
    --reductionConf $TRAIN

```
Note that the preexec for MC and data is different.
* Check your file staging status `git status`
* Commit changes `git commit -am "useful comments"`
* Push `git push --set-upstream origin <your branch>`

*Note:* 
The `--set-upstream`, or just `-u`, associates your branch with the copy on your fork. So subsequent pushes can be a plain `git push`.

* In case you need a specific tag:
```
git fetch upstream --tags
git checkout -b your_branch_name tags/release/21.2.3.0
```

* At every time login:
```
cd Athena/
asetup --restore
cd build/ && source x86_64-slc6-gcc62-opt/setup.sh && cd -
```

PreExec
=======

PreExec commands may vary between caches. Also, the syntax is different for MC and Data. Please consult this page: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DerivationProductionTeam#Info_on_AtlasDerivation_caches_a.

References
==========
https://atlassoftwaredocs.web.cern.ch/gittutorial/env-setup/
https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYxAODDerivationsr20#Release_21_2_X_git

