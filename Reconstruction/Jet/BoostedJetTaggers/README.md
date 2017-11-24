Boosted Jet Taggers
===================

This package contains the tagging code for boosted Top/W/Z/Higgs tagging


How to add a tagger
===================

We assume you have access to `/cvmfs`

Step 1: Set up a release
------------------------

First you should pick a relatively recent version of AnalysisBase. To
see the versions, look here:

```
/cvmfs/atlas.cern.ch/repo/sw/software/21.2/AnalysisBase
```

You should probably pick the highest number.

Next move to an empty directory and make a setup script. Something
like this should work:

```
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
alias setupATLAS='source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh'

setupATLAS
asetup AnalysisBase,21.2.XX
```

where you can replace the `XX` on the last line with the most recent
release you found above. I recommend putting this into a `setup.sh`
script in the empty directory, so you can source it every time you log
in.

Once you have this, run `source setup.sh`.

Step 2: Check out BoostedJetTaggers
-----------------------------------

We follow the "sparse checkout" instructions on the
[Atlas docs][1]. Assuming you've already created your fork of Athena,
you clone Athena and check out BoostedJetTaggers with

```
lsetup git
git atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git
cd athena
git checkout -t upstream/21.2
git atlas addpkg BoostedJetTaggers
```

When this finishes you'll see `Reconstruction` in your local
directory. The boosted taggers package is inside.

[1]: https://atlassoftwaredocs.web.cern.ch/gittutorial/

Step 3: Build
-------------

Go back to the top level directory (with your `setup.sh` script) and
run the following

```
mkdir build
cd build
cmake ../athena/Projects/WorkDir/
make -j 4
```

Step 4: Run a test job
----------------------

You want to make sure something works before you start hacking
away. To run a simple test job, you can use the built in unit tests
before doing anything though, you need to set up the local
environment:

```
source x86_64-slc6-gcc62-opt/setup.sh
```

In general you have to do this every time you start working (after
setting up the release).

Now run

```
./x86_64-slc6-gcc62-opt/bin/test_SmoothedWZTagger
```

it should start printing lines that indicate it's processing events.

Step 5: Start a New Tagger
--------------------------

We recommend reading through one of the existing taggers to get an
idea of how they work. There are a few caveats to keep in mind when
creating one:

 - You should add the `.cxx` file under the `atlas_add_library` list
   in the `CMakeLists.txt` file. This will ensure that CMake doesn't
   get confused when you change something.

 - To ensure usability with ROOT (both inside and outside Athena),
   you'll have to add your tagger to both `selection.xml` and
   `BoostedJetTaggersDict.h`. Both of these files are in
   `BoostedJetTaggers/BoostedJetTaggers/`.

 - If you have large configuration files that need to be read by your
   tagger (BDT or NN weights, for example), you should store them in
   [cvmfs][2]. To create these files you'll have to file a JIRA ticket
   similar to this one: https://its.cern.ch/jira/browse/ATLINFR-1354

We also encourage you to copy one of the `test_*` executables in
`util` so that you can test your tagger before running it.

Once you're reasonably happy with your tagger, you should create a
[merge request][3] with the 21.2 branch. We encourage you to do this
even if the tagger isn't completely final and to mark the request as
"WIP" (work in progress) so that people can see and discuss your
changes.


[2]: http://atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData/BoostedJetTaggers/
[3]: https://atlassoftwaredocs.web.cern.ch/gittutorial/merge-request/
