# Introduction to sherpaTarCreator

Input tarballs containing integration results are often used to avoid a lengthy initialisation time for central production jobs. Since the integrations are often costly it is advisable to use a parallelised (MPI) Sherpa installation/cluster to generate them. Here are instructions for the `sherpaTarCreator.py` tool, which creates the input tarball given a job option (JO) using a MPI Sherpa installation from cvmfs.

# Setup

The `sherpaTarCreator.py` script will be automatically available after `asetup` (since 21.6.49,AthGeneration). One crucial argument you need to specify when running it is the choice of batch system you are running on (`sherpaTarCreator.py -b <batchSystem>`). The default is set up for HTCondor on lxplus, and other options can be found with `sherpaTarCreator.py --help`

At the moment sherpaTarCreator is available for the batch systems HTCondor, LSF, Slurm and SGE. These are successfully tested on the clusters: naf, lxplus, taurus in Dresden, and Grace at UCL.
If creating a new batch system setup, to make best use of the batch queues available at a given local cluster, you might want to adapt the memory requirements in the .py files for the correct batch system.

# Setting up the job option (JO) file

The JO can contain the following steering parameters for the tar creator:
* `genSeq.Sherpa_i.RunCard` (mandatory): the RunCard of the process
* `genSeq.Sherpa_i.NCores` (default=24): number of cores used for the integration
* `genSeq.Sherpa_i.MemoryMB` (default=2500): reserved memory for the integration
* `genSeq.Sherpa_i.OpenLoopsLibs` (default=[]): list of needed OpenLoops libraries -- this is usually not needed anymore for Sherpa >=2.2.8 in Athena >=21.6.13 since they can be read from the central cvmfs installation (`/cvmfs/sft.cern.ch/lcg/releases/LCG_88b/MCGenerators/openloops/2.0.0/x86_64-centos7-gcc62-opt/proclib/`) and thus don't need to be included in the tarball anymore. In case you need a library that is not included in cvmfs you can add it through `OpenLoopsLibs`.
* `genSeq.Sherpa_i.ExtraFiles` (default=[]): list of extra files which should end up in the tarball (if needed)

Example for a setup with a few options:
```python
genSeq.Sherpa_i.NCores = 96
genSeq.Sherpa_i.OpenLoopsLibs = [ "ppln", "pplnj", "pplnjj" ]
genSeq.Sherpa_i.ExtraFiles = [ "libSherpaFastjetMAXHTPTV.so" ]
```

# Starting the input tarball creation
* Create the `mc.*.py` JO in a fresh working directory:
```
$ ls myExampleSetup/
mc.Sh_Example.py
```
* Optional: Copy or link additional needed files into the working directory, like "libSherpaFastjetMAXHTPTV.so" (these files have to be listed in "genSeq.Sherpa_i.ExtraFiles" in the JO in order to end up in the tarball)
* `asetup 21.6.xy,AthGeneration`
* Start sherpaTarCreator in dry-run mode if you want to check what it would write out and submit to the cluster: `sherpaTarCreator.py myExampleSetup -d`
* Start sherpaTarCreator and submit jobs to the cluster: `sherpaTarCreator.py myExampleSetup`

# Monitoring the progress

The commands for listing the currently running jobs and cancelling them depend on the batch system.
* HTCondor:
   * `condor_q -dag -nobatch`
   * `condor_rm [ID]`
* LSF:
   * `bjobs`
   * `bkill [job_ID]`
* Slurm:
   * `squeue -u [username] -o "%.9i %50j %.2t %.8M %.3C %20E"`
   * `scancel [JOBID]`
The jobs depend on each other. So, by cancelling one job other jobs might get automatically cancelled, too.

The jobs create logfiles in the initial working directory, numerated by their order of execution. E.g. "1.createLibs.log"

_Note_: Please start from a fresh and empty working directory if you have to restart sherpaTarCreator to be on the safe side.

# Finding the output

The `mc.*.py` jobOption file and `mc_13TeV.*.GRID.tar.gz` tarball will be finalised in the working directory once the jobs are finished. This includes an updated `nEventsPerJob` setting in the jobOption file.

# What happens inside?

When running sherpaTarCreator several steps will happen consecutively. Some of them are not always necessary as shown in the following:

always:
* creates Run.dat from JO

if not all required OpenLoops libraries are available, i.e. list of files matching "Process/OpenLoops/proclib/*.so" dissimilar to genSeq.Sherpa_i.OpenLoopsLibs:
* job: _getOpenLoops_
   * downloads OpenLoops and installs the packages listed in "genSeq.Sherpa_i.OpenLoopsLibs''

if process specific libraries not available, i.e. no file "Process/*.db" exists:
* job: _createLibs_ (run Sherpa initialisation)
   * writes out needed libraries into "Process/" directory
* job: _makelibs_
   * if libraries need compilation: compiles them

next steps happen per center of mass energy (Ecm):

if there is no file "Results.db" for that Ecm, i.e. file "ecmXX/Results.db" does not exist:
* job: _integrate_ (run Sherpa multicore)
   * performs the integration producing "Results.db"

always:
* job: _makeTarball_
   * creates the tarball, including:
      * Results.db
      * "Process/" folder
      * logfile "integrate.log" from the integration
* job: _evgen_
   * uses tarball and JO to generate 100 test-events in fresh directory
   * checks the log-file and extracts the number of events such that the event-generation of these events would not exceed 12 hours (evgenConfig.nEventsPerJob)
   * provides logfile
