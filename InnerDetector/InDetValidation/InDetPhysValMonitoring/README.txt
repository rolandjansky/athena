====
  Introduction
====

This package was introduced initially as a copy of the old RTT (Run Time Test) package.
The principal tool, InDetPhysValMonitoringTool, has a simple structure:
* retrieve containers
* loop over containers using a selection tool to select elements
* decorate those elements
* do plots

The plots are intended to be as simple structures as possible, only retrieving and plotting
the added decoration variables. There are numerous decorators for either truth or track particles,
inheriting from a common interface. 

===
  Instruction for sparse git checkout and build in 'dev' (April 2017)
===
Assuming you are starting from a clean user directory and have used git at least once 
before and have therefore set up your git configuration and already forked the main 
repository to a new repository with your name:

1. Set up the Atlas environment and clone the repository
setupATLAS
lsetup git
mkdir athena build run
git-atlas init-workdir https://:@gitlab.cern.ch:8443/atlas/athena.git

2. Do a 'sparse checkout'
cd ~/athena
git-atlas addpkg InDetPhysValMonitoring
git fetch upstream
git checkout -b 22.0-idpvm upstream/master --no-track

3. Setup the dev release
asetup master,r02,Athena,gcc62
(note: the r02 refers to a release on the specific day [2] of the current month)

4. Develop, then build with cmake
cd ~/build
cmake ../athena/Projects/AthenaWorkDir
make

5. Get some data to work on
cd ~/run
source ../athena/InnerDetector/InDetValidation/InDetPhysValMonitoring/scripts/getSomeData.sh
ln -s /tmp/$USER/valid3/ESD.05297574._000080.pool.root.1 AOD.pool.root

6. Run
source ../build/x86_64-slc6-gcc49-opt/setup.sh
athena ../athena/InnerDetector/InDetValidation/InDetPhysValMonitoring/run/PhysVal_jobOptions.py

7. Commit code
cd ~/athena
git commit -a -m "My new commit"
(note: at this point you have only committed to your _local_ repository)
git push --set-upstream origin 22.0-idpvm

At this point, you have committed to your previously forked repository which is public.

8. Make a 'Merge Request'
This is done on the webpage of your personal forked repository. Your code will be reviewed
and (hopefully) accepted to master; this may take ~ 1 day.




====
	Making and running a test job(CMAKE)
====
From your Athena working directory:

mkdir build source run
cd source
asetup here, dev, rel_5, cmake
svnco InDetPhysValMonitoring
cd ../build
cmake ../source
make -j4
cd ..
ln -s $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/share .
cd run
source ../build/x86_64-slc6-gcc49-opt/setup.sh
athena $TestArea/InnerDetector/InDetValidation/InDetPhysValMonitoring/run/PhysVal_jobOptions.py


========
  Run a physics validation reco transform
========

---
  Quick start
---

1) setup the athena release in which You want to run the validation job
   Presumably anything older than 20.1.5.12 will not work



2). Run the reco transform.

    To enable the InDetPhysValMonitoring histograms, add "--valid True --validationFlags doInDet --outputNTUP_PHYSVALFile physval.root"
    to the transform You would run without the additional validation histograms. For example: 

    Reco_tf.py   --maxEvents 25 --valid True --inputRDOFile root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105001.pythia_minbias.digit.RDO.e574_s932_s946_d369/RDO.197112._000270.pool.root.1 root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105001.pythia_minbias.digit.RDO.e574_s932_s946_d369/RDO.197112._000757.pool.root.1 --preExec "from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doMinBias.set_Value_and_Lock(True)"  --outputNTUP_PHYSVALFile physval.root --outputAODFile physval.AOD.root --validationFlags doInDet

    This will run on a minimum bias RDO, decorate the xAODTrackParticles, create
    an xAOD physval.AOD.root and the validation histograms physval.root

    The switch --valid True will enable the decoration, 
               --validationFlags doInDet will enable the InDetPhysValMonitoring histograms
               --outputNTUP_PHYSVALFile physval.root tells the Reco_tf to run the physics validation
                                                     monitoring and create the histogram files.

    The InDetPhysValMonitoring is configured in python/InDetPhysValMonitoringTool.py. The 
    special parameters of the InDetPhysValMonitoringTool which are different from the default
    ones are set in the last few lines [2015-07-07].

    The decoration is configured in python/InDetPhysValDecoration.py in the method addDecorator 
    and the tools in use are configured in special python classes. 





========
  Running InDetPhysValMonitoring RTT (Run Time Tester) tests
========

The RTT manual can be found here [2015-07-07]:
https://atlas-rtt.cern.ch/prod/docs/userguide/

The DCube documentation [2015-07-07] here:
https://twiki.cern.ch/twiki/bin/view/AtlasComputing/DCubeSoftware


---
  RTT quick start
---

0. the RTT are defined in this package in test/InDetPhysValMonitoring_TestConfiguration.xml
   Configure new tests here. For the automatic Dcube analysis also Dcube configuration and
   reference files are needed for each test (also see below).


1. Checkout RTT :

 Create a working directory and checkout the RTT package
 mkdir rtt
 cd rtt
 curl -sk https://atlas-rtt.cern.ch/prod/deploy/setup.sh | sh


2. Setup a nightly. 

   RTT will only work with nightlies.
   For example :
    asetup --nightliesarea=/afs/cern.ch/atlas/software/builds/nightlies  AtlasProduction,20.1.X.Y-VAL,rel_2,slc6,64,gcc48,opt,here


2b. At the moment some additional packages are still needed :

   * At least InDetRecExample-02-05-57
   * At least PhysValMonitoring-00-01-40
   * InDetPhysValMonitoring-00-00-22-03 (old, but should work) or at least revision 680873 of the trunk

   These packages only need to be checked out and compiled if they are not yet
   part of the release. 


3. Fill the RTT configuration file

   When deploying RTT a default configuration file is created in cfg/rtt/[nightly]/[CMT-config]/[project]/cfg.xml
   For example:
      cfg/rtt/20.1.X.Y-VAL/x86_64-slc6-gcc48-opt/AtlasProduction/cfg.xml

   The relevant xml tags which need to be filled are there but out-commented. 

   a) You may want to run the RTT tests of the InDetPhysValMonitoring i.e. those of a local package. 
   You need to add
   
   <localConfFiles>
         <testProjectCMTDir>[the full path to the cmt directory of InDetPhysValMonitoring]</testProjectCMTDir> 
         <testArea>[Your test area, i.e. the path where You executed asetup ...,here which is also stored in the environment variable $TestArea] </testArea>
   </localConfFiles>

   for example :

   <localConfFiles>
      <testProjectCMTDir>/afs/cern.ch/work/u/user/rtt/InnerDetector/InDetValidation/InDetPhysValMonitoring/cmt</testProjectCMTDir> 
      <testArea>/afs/cern.ch/work/u/user/rtt</testArea>
   </localConfFiles>

   If You want to use packages which are not in the release the testarea should be
   the one in which You checked out the packages. 

   
   b) The above lines only provide the necessary information to run the tests, the
   tests which are actually run have to be selected for example :

    <packageAccepts>
        <packageAccept>InDetPhysValMonitoring</packageAccept>
    </packageAccepts>


    The RTT configuration is much more flexible. This is just one possible way to
    get the RTT tests defined in InDetPhysValMonitoring running. It also will
    run all tests which are defined in the test/InDetPhysValMonitoring_TestConfiguration.xml
    of InDetPhysValMonitoring


4.  run the tests 

    when executing:

    ./runrtt.py

    You will be asked several questions [as of 2015-07-07]:

    a) Choose the nightly/architecture by giving the number corresponding to the 
       nightly You want to run in the offered selection.
    b) Choose the week day of the nightly 0 -> rel_0, 1 -> rel_1, ....
    c) Choose whether You want to run the RTT as batch jobs or in the current shell.
    d) Confirm the selection.

   
    If You run in the current shell You will get log files in the directory "log" in
    the current directory. The results will be somewhere in the "Results" directory e.g.

       Results/rtt/rel_2/20.1.X.Y-VAL/build/x86_64-slc6-gcc48-opt/offline/InDetPhysValMonitoring/IdPhysValMinBias

    There You find ESDs, AODs, log-files, the histograms (physval.root) and the DCube
    html pages.

   
4a. Create or recreate the DCube configuration and reference histograms.

    If the references or Dcube configuration is out-dates or missing. The configuration
    and references can be updated (or set) as described below:

    a) If an athena release is setup, DCcube should be available. To generate a
    default configuration file :
    
    dcube.py -g -r [histogram-file-name] -c [reference-dir/output-configuration-file-name] 

    For example 

    dcube.py -g -r dcube.py -g -r Results/rtt/rel_2/20.1.X.Y-VAL/build/x86_64-slc6-gcc48-opt/offline/InDetPhysValMonitoring/idIdPhysValMinBias/physval.root -c ../ref/IdPhysValMinBias/DCube_IdPhysValMinBias.xml

    You may want to play with the switches -f, -w to change the probability thresholds for failures
    and warnings and maybe adjust the test with -t.

    After that You may want to tweak the resulting configuration file manually.
    
    b) Then copy the reference histogram file to the reference location

    c) Adjust the references in the test/InDetPhysValMonitoring_TestConfiguration.xml to
       point to the new configuration or reference in case the locations are different.
       For example:

         <arg>
            <argname>DCubeCfg</argname>
            <argvalue>/afs/cern.ch/atlas/project/RTT/DCubeReference/offline/InnerDetector/InDetValidation/InDetPhysValMonitoring/IdPhysValTtbar_DCubeConfig.xml</argvalue>
         </arg>
         <arg>
            <argname>DCubeRef</argname>
            <argvalue>/afs/cern.ch/atlas/project/RTT/DCubeReference/offline/InnerDetector/InDetValidation/InDetPhysValMonitoring/IdPhysValTtbar_physval.root</argvalue>
         </arg>
    
       [ I was expecting that the The default search path for DCube is this 
       /afs/cern.ch/atlas/project/RTT/DCubeReference/offline/InnerDetector/InDetValidation/InDetPhysValMonitoring/
       or this
       /afs/cern.ch/atlas/project/RTT/Results/reference/offline/InnerDetector/InDetValidation/InDetPhysValMonitoring/
       but this does not seem to be the case]
  


4b.  Rerun RTT tests with updated references 

     ... and verify that everything is now green in DCube, since rerunning should 
     perfectly reproduce the histograms.



Use this command:

    dcube.py -r Results/rtt/rel_4/dev/build/x86_64-slc6-gcc49-opt/offline/InDetPhysValMonitoring/IdPhysValTtbar/Mttbar_9Nov_v0.root -c ../Testdir/ref/IdPhysValTtbar/DCube_IdPhysValTtbar.xml Results/rtt/rel_4/dev/build/x86_64-slc6-gcc49-opt/offline/InDetPhysValMonitoring/IdPhysValTtbar/Mttbar_9Nov_v0.root
