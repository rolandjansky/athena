# DirectIOART - Testing direct I/O in recent ATLAS offline software releases using ART (rel21)
## The DirectIOART package
DirectIOART is a package meant for testing and monitoring file access via direct I/O in (almost) all recent *ATLAS* offline software releases (**only** release 21 and master) making use of the ART ([*ATLAS* Release Tester](https://twiki.cern.ch/twiki/bin/view/AtlasComputing/ART)) framework. Its tests range from simple `TFile::Open` command to more involved `Reco_tf.py` reconstruction or derivation tests as well as small analysis algorithms. Each test is performed three times, in order to test three different protocols:
* **DAVS**  (davs://...)
* **HTTPS**  (https://...)
* **ROOT**  (root://...)

#### Package structure
The actual tests are implemented in small bash and python scripts stored in the `test/` folder. Some of them need additional code, namely athena job options, which can be found in the `share/` folder. The main directory contains a CMakeLists.txt file which does all the necessary configurations during build time.
Detailed lists of the tests that are performed within the particular software releases and the input files are given below.
##### Tests:
* **TFile::Open** (using an AOD input file)
  * master/Athena
  * 21.0/Athena
  * 21.0/AthSimulation
  * 21.2/AthAnalysis
  * 21.2/AthDerivation
  * 21.2/AnalysisBase
  * 21.2/AnalysisTop
  * 21.3/Athena
  * 21.9/Athena
* **`Reco_tf.py`** with RAW input file:
  * 21.0/Athena
  * 21.0/AthenaMP
* **`Reco_tf.py`** with RDO input file:
  * 21.0/Athena
  * 21.0/AthenaMP
* **`Reco_tf.py`** with AOD input file:
  * 21.2/AthDerivation
* **Analysis algorithm (`ParticleSelectionAlg`)** with AOD input file:
  * 21.2/AthAnalysis
* **`xAODChecker`** with DAOD input files:
  * 21.2/AnalysisBase

**Note:** For each test three test scripts have been generated except for the  `TFile::Open` test. At the moment the DirectIOART package contains 25 tests in total that are individually submitted.

##### Input files:
* **AOD** input file:
  * mc15_13TeV:AOD.05536542._000001.pool.root.1 from dataset mc15_13TeV:mc15_13TeV.423202.Pythia8B_A14_CTEQ6L1_Jpsie3e13.merge.AOD.e3869_s2608_s2183_r6630_r6264_tid05536542_00
* **DAOD** input file:
  * data18_13TeV:DAOD_MUON0.14316152._000001.pool.root.1 from dataset data18_13TeV:data18_13TeV.00348709.physics_Main.deriv.DAOD_MUON0.f937_m1972_p3553_tid14316152_00
* **RAW** input file:
  * data15_13TeV:data15_13TeV.00284285.physics_Main.daq.RAW._lb0856._SFO-1._0003.data from dataset data15_13TeV:data15_13TeV.00284285.physics_Main.daq.RAW
* **RDO** input file:
  * mc16_13TeV:RDO.11373415._000001.pool.root.1 from dataset mc16_13TeV:mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.RDO.e3601_s3126_r9546_tid11373415_00

**Note:** All input files but one are stored in the ART input container (`user.artprod:user.artprod.input.container`), whereas the AOD file is from the Hammercloud input container (`hc_test:hc_test.aft`).
#### Creating (new) ART tests
All test scripts kept in `test/` can be produced with the `generateARTtests.py` python script. It contains all relevant TURLs used within the test scripts. So, in order to create entirely new tests or replace the current input files by other/newer ones, this can be easily done by just modifying `generateARTtests.py`. It also takes care of specifying which tests will be run by which software release. This can be done with the ART header keyword `# art-include:` followed by the respective branch and software project, e.g.
```
# art-include: 21.2/AthAnalysis
```
## Running in *ATLAS* nightlies
After a nightly build is done, the athena repository is searched for all the available grid-type ART jobs which subsequently are submitted to the grid. This must **NOT** be done by users! All (including the sequence tag) is taken CARE of by gitlab-ci, so as a user you have nothing more to do but to get your tests into the athena repository.
#### ART results
Usually, the ART results are downloaded by request to a specific EOS area: `/eos/atlas/atlascerngroupdisk/data-art/grid-output/`.
#### Displaying ART jobs
All the necessary information for a given job, panda ID, job name, package name and nightly branch environment variables, are sent by ART to the bigpanda. The results from the grid jobs are displayed in the so-called [ART display](https://bigpanda.cern.ch/art/), where you can select a specific package, branch and nightly tag, respectively.
## Running manually
When developing new ART tests you might be interested in testing your code before submitting a new merge request and wait until your latest changes appear on the [ART display](https://bigpanda.cern.ch/art/). How to do so is shown to you by the following instructions.
#### Setup
To run the DirectIOART package by yourself, if you're not working on `lxplus`, first you have to set up the ATLAS user interface `ATLASLocalRootBase` (make sure your machine has access to `cvmfs`). In addition, set up git and PanDA before continuing with anything else (you will be needing it later on):
```
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
lsetup git
lsetup panda
```
Now, set up a new and clean work area. Therein, we need three empty folders:
```
mkdir TestingDirectIOART/
cd TestingDirectIOART/
mkdir build/ source/ run/
```
Afterwards, go to the `build/` directory and set up the *ATLAS* software release and version in which you want to launch your tests (as an example, we take **AthAnalysis**):
```
cd build/
acmSetup 21.2,AthAnalysis,latest
```
For getting the DirectIOART git repository in there, do a "sparse checkout" of the athena repository with `acm` (requires forking the atlas/athena repository first, as described [here](https://atlassoftwaredocs.web.cern.ch/gittutorial/gitlab-fork)):
```
acm sparse_clone_project athena
```
This will create a working copy of the ATLAS Offline Git repository (it might take a while) and setup a sparse checkout but doesn’t check out any packages yet. It will also setup your personal fork of the athena repository as `origin` and the main repository as `upstream`. On top of that, `acm` makes sure that you are in the branch appropriate for the release you've set up locally and that everything is up-to-date with the upstream repository. Note that outside of `lxplus` you may have to do this sparse checkout differently (if you don't have a valid kerberos ticket for your CERN credentials), for example via ssh authentication (where `YOUR_GITLAB_NAME` is your gitlab user name):
```
acm sparse_clone_project ssh://git@gitlab.cern.ch:7999/YOUR_GITLAB_NAME/athena.git
```

In order to add the DirectIOART package to your checkout, execute
```
acm add_pkg athena/Tools/DirectIOART
```
This will checkout the DirectIOART package (the full path resolves to `Tools/DirectIOART/`), but leaves the other parts out of the work tree. Use `acm add_pkg` to add as many packages as you like and `acm rm_pkg` to remove them. Note that `acm` will also take care of any necessary entries in the `package_filters.txt` file, which is used by cmake to filter packages for the build.

After that, you should compile your code, using
```
acm compile
```

After all this worked, go to the `run/` directory of your work area in order to launch the ART tests (as described in the following sections):
```
cd ../../run/
```
#### Validating ART tests
Once the DirectIOART package has been properly setup, the test scripts can be validated by executing the following command:
```
art.py validate -v ../source/athena/Tools/DirectIOART
```
`"art.py"` calls the ART script which is doing all the hard work. As positional arguments you have to provide the path to the directory containing the `test/` folder with all the test scripts in it (e.g. `"../source/DirectIOART/"`). With the keyword `"validate"` the validation procedure will be initiated.  Finally, with the `"-v"` flag the verbosity is turned on in order to get a more detailed terminal output.
#### Running locally
For running the tests on your local machine, use the following command:
```
art.py run --type=grid -v ../source/athena/Tools/DirectIOART/ .
```
After calling `"art.py"`, the keyword `"run"` tells ART to run the tests locally. Again, the `"../source.DirectIOART/"` path has to be given, followed by the so-called sequence tag (`"."`) which in this case is just the current directory `run/`. This means, that all the output created by the ART test will go there. In addition, one has to specify the type of the ART tests, which is for the DirectIOART package `"grid"`.
The results of all tests are written into a JSON file (`status.json`) containing information about the job (exit code, job name, etc.). Furthermore, for each test script an output directory is created, in which all detailed output (`stdout.txt`, `stderr.txt`, `PoolFileCatalog.xml`, output ROOT files, log files etc.) are stored. 
#### Running on the grid
In order to send jobs from the DirectIOART package to the grid instead of running them locally, type the following command:
```
art.py grid -v ../source/athena/Tools/DirectIOART/ DirectIOART_20180724_1
```
After calling `"art.py"`, the keyword `"grid"` tells ART to prepare for grid submission. Again, the `"../source.DirectIOART/"` path has to be given, but now with a sequence tag which is composed of, firstly, the package name, secondly, the submission date, and third, a counting index. Every `"art.py grid ..."` command will create a separate job containing all executed tests as single tasks. Actually, what the sequence tag looks like doesn't matter as long as it is unique, since it will be part of the grid job's output name as displayed on your [bigpanda](https://bigpanda.cern.ch/) page, e.g.:
```
user.flfische.atlas.21.2.AnalysisBase.x86_64-slc6-gcc62-opt.2018-07-04T0335.DirectIOART_20180706_2.DirectIOART/
```
