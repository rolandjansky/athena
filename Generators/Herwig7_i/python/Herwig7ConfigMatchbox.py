#! /usr/bin/env python

## \file Herwig7ConfigMatchbox.py
## \brief Configuration class for Matchbox runs with %Herwig7
## \author Daniel Rauch (daniel.rauch@desy.de)

import os

import Herwig7Config as hw7Config
import Herwig7Control as hw7Control
import Herwig7Utils as hw7Utils

from AthenaCommon import Logging
athMsgLog = Logging.logging.getLogger('Herwig7ConfigMatchbox')


## Configuration class for Matchbox runs with %Herwig7
##
## Example JobOptions are available in [`examples/Matchbox`](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig7_i/trunk/examples/Matchbox) and [`tests/athenaMatchbox/jobOptions`](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig7_i/trunk/tests/athenaMatchbox/jobOptions).
##
##
## Process Setup and Generator Configuration
## =========================================
##
## To use the Matchbox run mode of %Herwig7 load the corresponding modules and add %Herwig7 to the generator sequence
## 
## \code{.py}
## from Herwig7_i.Herwig7_iConf import Herwig7
## from Herwig7_i.Herwig7ConfigMatchbox import Hw7ConfigMatchbox
## 
## genSeq += Herwig7()
## \endcode
## 
## You may specify details of the run a la
## 
## \code{.py}
## # Provide config information
## evgenConfig.generators += ["Herwig7"]
## evgenConfig.tune        = "MMHT2014"
## evgenConfig.description = "Herwig7 Zee sample with CT10 ME PDF and MMHT2014 PS and UE tune"
## evgenConfig.keywords    = ["SM","Z","electron"]
## evgenConfig.contact     = ["Your Name (your.name@cern.ch)"]
## \endcode
## 
## You can initialize a generator configuration object with
## 
## \code{.py}
## # initialize generator configuration object
## generator = Hw7ConfigMatchbox(genSeq, runArgs, run_name="HerwigMatchbox", beams="pp")
## \endcode
## 
## Please see the [constructor](\ref Herwig7ConfigMatchbox::Hw7ConfigMatchbox#__init__) for
## more information on the arguments.
## 
## You can use the functions
## [me_alphas_commands](\ref Herwig7Config::Hw7Config#me_alphas_commands),
## [me_pdf_commands](\ref Herwig7Config::Hw7Config#me_pdf_commands) and
## [tune_commands](\ref Herwig7Config::Hw7Config#tune_commands)
## from Herwig7Config::Hw7Config to configure the strong coupling and the PDFs
## for the hard process and the parton shower and underlying event tunes, e.g.
## 
## \code{.py}
## # configure generator
## generator.me_pdf_commands(order="NLO", name="CT10")
## generator.tune_commands()
## \endcode
## 
## You can add direct %Herwig7 commands like this
## 
## \code{.py}
## generator.add_commands("""
## 
## # Model assumptions
## read Matchbox/StandardModelLike.in
## 
## # Set the hard process
## set /Herwig/MatrixElements/Matchbox/Factory:OrderInAlphaS 0
## set /Herwig/MatrixElements/Matchbox/Factory:OrderInAlphaEW 2
## do /Herwig/MatrixElements/Matchbox/Factory:Process p p -> e+ e-
## 
## # Cut selection (see the documentation for more options)
## set /Herwig/Cuts/ChargedLeptonPairMassCut:MinMass 60*GeV
## set /Herwig/Cuts/ChargedLeptonPairMassCut:MaxMass 120*GeV
## 
## # Scale choice (see the documentation for more options)
## cd /Herwig/MatrixElements/Matchbox
## set Factory:ScaleChoice /Herwig/MatrixElements/Matchbox/Scales/LeptonPairMassScale
## 
## # Matching and shower selection
## read Matchbox/MCatNLO-DefaultShower.in
## 
## # Choose a flavour scheme
## read Matchbox/FiveFlavourScheme.in
## """)
## \endcode
## 
## Please see the [Herwig7 tutorial page](http://herwig.hepforge.org/tutorials/index.html) for in-depth information on the different settings and have a look at Herwig7ConfigMatchbox::Hw7ConfigMatchbox and Herwig7Config::Hw7Config to see if there are convenience functions that accomplish the task provided in the interface already.
## 
##
## Integration and Event Generation
## ================================
##
## To trigger the actual running of %Herwig7 in Athena please end the JobOptions with
## 
## \code{.py}
## # run the generator
## generator.run()
## \endcode
##
## More specifically, the `run()` function triggers the creation of the %Herwig7 input
## file and the preparation of the run (i.e. the `Herwig [build, integrate,
## mergegrids]` sequence). This means, that no %Herwig7 settings should be modified
## after calling the `run()` function because the changed settings would not be applied
## during the event generation.
##
##
## Cleanup After the Event Generation
## ----------------------------------
##
## By default the folder `Herwig-scratch` which created by %Herwig7 and contains
## the process libraries and several other files (e.g. integration grids)
## will be cleaned up and deleted after the event generation in order to save
## disk space. You can prevent this with:
## 
## \code{.py}
## # run the generator
## generator.run(cleanup_herwig_scratch=True)
## \endcode
## 
## 
## Parallel Integration
## --------------------
##
## Machine-local parallelization of the integration can be achieved with
## \code{.py}
## # run the generator
## generator.run(integration_jobs=2)
## \endcode
##
## `Generate_tf.py` comes with the command line parameter `--generatorJobNumber`
## that can be used to specify the number of integration jobs dynamically, instead
## of fixing it in the JobOptions
## \code{.py}
## # run the generator
## generator.run(integration_jobs=runArgs.generatorJobNumber)
## \endcode
## 
## 
## Gridpacks
## ---------
##
## A gridpack can be created with
## \code{.py}
## # run the generator
## generator.run(gridpack="gridpack.tar.gz")
## \endcode
## 
## The JobOptions can be run in athena with a command such as
## 
##     Generate_tf.py --jobConfig=MC15.999999.H7_MB_Int_Zee_MCatLO_QTilde_H7_UE_MMHT2014.py --runNumber=999999 --ecmEnergy=13000 --randomSeed=12011988 --maxEvents=100 --outputEVNTFile=evgen.root
## 
## If you have previously created a gridpack called `gridpack.tar.gz` and want to generate events from it, specify the name of the compressed gridpack using the `--inputGenConfFile` command line parameter, e.g.
## 
##     Generate_tf.py --jobConfig=MC15.999999.H7_MB_Int_Zee_MCatLO_QTilde_H7_UE_MMHT2014.py --inputGenConfFile=gridpack.tar.gz --runNumber=999999 --ecmEnergy=13000 --randomSeed=12011988 --maxEvents=100 --outputEVNTFile=evgen.root
## 
## 
## Full Flexibility and Batch-Parallel Integration
## -----------------------------------------------
##
## The `--generatorRunMode` command line argument for `Generate_tf.py` can be used
## to put in place a very fine-grained control over the various steps that happen
## before the event generation.
##
## In the following example this is combined with the creation and use of
## a gridpack. This, however, doesn't have to be done and can be left out.
## \code{.py}
##     if runArgs.generatorRunMode == 'build':
##       # use the --generatorJobNumber command line parameter to dynamically
##       # specify the total number of parallel integration jobs
##       generator.do_build(integration_jobs=runArgs.generatorJobNumber)
## 
##     elif runArgs.generatorRunMode == 'integrate':
##       # use the --generatorJobNumber command line parameter to dynamically
##       # specify which specific integration job is to be run
##       generator.do_integrate(runArgs.generatorJobNumber)
##
##     elif runArgs.generatorRunMode == 'mergegrids':
##       # combine integration grids and prepare a gridpack
##       # use the --generatorJobNumber command line parameter to dynamically
##       # specify the total number of parallel integration jobs
##       generator.do_mergegrids(integration_jobs=runArgs.generatorJobNumber, gridpack="gridpack.tar.gz")
##
##     elif runArgs.generatorRunMode == 'run':
##       # generate events using the specified gridpack
##       generator.do_run(gridpack="gridpack.tar.gz")
## \endcode
##
## Please note that for the `do_build()` function the same caveat applies as for
## the `run()` function: The %Herwig7 generator configuration should not be modified
## afterwards in the job options as the new settings would not be applied in the
## event generation.
##
## \note Currently the build, the integration and the mergegrids steps have to
##       run in the same directory which means that the files written by athena
##       will be re-created and overwritten.
##       In one of the next releases %Herwig7 will bring infrastructure to re-use
##       `Herwig-scratch` folders from a different location which will allow for
##       a full batch parallelization of the integration in athena.
##
class Hw7ConfigMatchbox(hw7Config.Hw7Config):


  ## \brief Initialize a generator configuration object for the Matchbox run mode
  def __init__(self, genSeq, runArgs, run_name="Herwig", beams="pp"):

    beams = beams.upper()
    if not beams in ["EE", "EP", "PP"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Parameter 'beams' must be one of the following ['EE', 'EP', 'PP']!"))

    ## provide variables initialized by the parent class
    super(Hw7ConfigMatchbox, self).__init__(genSeq, runArgs, run_name)

    self.beams = beams


  def local_pre_commands(self):

    # try to locate the MG5_aMC@NLO installation
    MG5aMC_path = os.environ['MADPATH']
    if not os.path.isfile(os.path.join(MG5aMC_path, 'bin', 'mg5_aMC')):
      athMsgLog.warn(hw7Utils.ansi_format_warning("The MadGraph5_aMC@NLO installation can't be found from $MADPATH = {}, so don't be surprised if your run crashes in you are using matrix elements from MG5_aMC@NLO in Herwig7 / Matchbox. Please ensure that the location exists, that you have permissions to access it and that it contains the executable 'bin/mg5_aMC'".format(MG5aMC_path)))

    # try to locate the GoSam installation
    try:
      GoSam_path = os.environ['GOSAM_PATH']
    except:
      # \todo Get rid of this and just use the environment variable `GOSAMPATH`
      GoSam_path = '/afs/.cern.ch/sw/lcg/external/MCGenerators_lcgcmt67c/gosam/2.0.3/x86_64-slc6-gcc47-opt'
      athMsgLog.warn(hw7Utils.ansi_format_warning("Falling back to hard-coded GoSam installation location at '{}' - please ensure that the GOSAM_PATH environment variable is correctly set".format(GoSam_path)))
    if not os.path.isfile(os.path.join(GoSam_path, 'bin', 'gosam.py')):
      athMsgLog.warn(hw7Utils.ansi_format_warning("The GoSam installation can't be found from $GOSAMPATH = {}, so don't be surprised if your run crashes in you are using matrix elements from GoSam in Herwig7 / Matchbox. Please ensure that the location exists, that you have permissions to access it and that it contains the script 'bin/gosam.py'".format(GoSam_path)))

    try:
      OpenLoops_path= os.environ['OPENLOOPS_PATH']
    except:
      OpenLoops_path = '/afs/.cern.ch/sw/lcg/releases/MCGenerators/openloops/1.3.1-1f1f6/x86_64-slc6-gcc49-opt'
      athMsgLog.warn(hw7Utils.ansi_format_warning("Falling back to hard-coded OPENLOOPS installation location at '{}' - please ensure that the OPENLOOPS_PATH environment variable is correctly set".format(OpenLoops_path)))
    if not os.path.isdir(os.path.join(OpenLoops_path, "proclib")):
      athMsgLog.warn(hw7Utils.ansi_format_warning("The OpenLoops process libraries can't be found from $OPENLOOPS_PATH = {}".format(OpenLoops_path)))

    return """
## ================================================
## Local Pre-Commands from Herwig7ConfigMatchbox.py
## ================================================

## Fixing interface locations for MadGraph
set /Herwig/MatrixElements/Matchbox/Amplitudes/MadGraph:BinDir {0}
set /Herwig/MatrixElements/Matchbox/Amplitudes/MadGraph:DataDir {1}
set /Herwig/MatrixElements/Matchbox/Amplitudes/MadGraph:MadgraphPrefix {2}

## Fixing interface locations for GoSam
set /Herwig/MatrixElements/Matchbox/Amplitudes/GoSam:BinDir {0}
set /Herwig/MatrixElements/Matchbox/Amplitudes/GoSam:DataDir {1}
set /Herwig/MatrixElements/Matchbox/Amplitudes/GoSam:GoSamPrefix {3}

##Fixing interface locations of Openloops
set /Herwig/MatrixElements/Matchbox/Amplitudes/OpenLoops:OpenLoopsLibs {5}
set /Herwig/MatrixElements/Matchbox/Amplitudes/OpenLoops:OpenLoopsPrefix {6}

read snippets/Matchbox.in
read snippets/{4}Collider.in
""".format(hw7Control.herwig7_bin_path,
           hw7Control.herwig7_share_path,
           MG5aMC_path,
           GoSam_path,
           self.beams,
           os.path.join(OpenLoops_path,"proclib"),
           OpenLoops_path)


  def local_post_commands(self):

    return """
## =================================================
## Local Post-Commands from Herwig7ConfigMatchbox.py
## =================================================

do /Herwig/MatrixElements/Matchbox/Factory:ProductionMode
saverun {} /Herwig/Generators/EventGenerator
""".format(self.run_name)


  def __configure(self):

    ## add default settings if they were not overwritten in the JobOptions

    self.default_commands += self.energy_commands()
    self.default_commands += self.random_seed_commands()
    
    if not self.set_printout_commands:
      self.default_commands += self.printout_commands()
    if not self.set_physics_parameter_commands:
      self.default_commands += self.physics_parameter_commands()
    if not self.set_technical_parameter_commands:
      self.default_commands += self.technical_parameter_commands()


  ## High-level function for triggering the process setup and the event generation.
  ##
  ## \param[in] integration_jobs        Number of threads for machine-local parallelization of the integration
  ## \param[in] gridpack                Name of the gridpack that possibly is to be created (after the integration)
  ##                                    or to be used (for the event generation).
  ##                                    No gridpack is created or used if the parameter is not specified.
  ##                                    If you have created a gridpack
  ##                                    and would like to generate events from it, please pass the name of the
  ##                                    gridpack to the `Generate_tf.py` command using the `--inputGenConfFile`
  ##                                    command line argument
  ## \param[in] cleanup_herwig_scratch  Remove the `Herwig-scratch` folder after event generation to save disk space
  ##
  ## \warning Please do not modify the generator configuration in the job options after calling the `run()` function as
  ##          the modified settings would not be applied in the event generation
  ##
  def run(self, integration_jobs=1, gridpack=None, cleanup_herwig_scratch=True):

    self.__configure()

    if gridpack:
      hw7Control.matchbox_run_gridpack(self, integration_jobs, gridpack, cleanup_herwig_scratch)
    else:
      hw7Control.matchbox_run(self, integration_jobs, cleanup_herwig_scratch)


  # atomic functions for complete control from within the JobOptions ----------

  ## Atomic steering function for doing the build step alone
  ##
  ## \param[in] integration_jobs Number of integration jobs to be prepared
  ##
  ## \warning Please do not modify the generator configuration in the job options after calling the `do_build()` function as
  ##          the modified settings would not be applied in the event generation
  ##
  def do_build(self, integration_jobs):
    self.__configure()
    hw7Control.do_build(self, integration_jobs)
    hw7Control.do_abort()

  ## Atomic steering function for doing one specific integration job
  ## \todo provide info about the range
  def do_integrate(self, integration_job):
    hw7Control.do_integrate(self.run_name, integration_job)
    hw7Control.do_abort()

  ## Atomic steering function for combining the integration grids and
  ## possibly creating a gridpack
  ## \param[in] integration_jobs Number of integration jobs
  ## \param[in] gridpack         Name of the gridpack to be created.
  ##                             No gridpack is created if the parameter is not specified.
  def do_mergegrids(self, integration_jobs, gridpack=None):
    hw7Control.do_mergegrids(self.run_name, integration_jobs)
    if gridpack:
      hw7Control.do_compress_gridpack(self.run_name, gridpack)
    hw7Control.do_abort()

  ## Atomic steering function for possibly unpacking a gridpack
  ## and generating events
  ## \param[in] gridpack                Name of the gridpack to be used.
  ##                                    No gridpack is used if the parameter is not specified.
  ## \param[in] cleanup_herwig_scratch  Remove the `Herwig-scratch` folder after event generation to save disk space
  def do_run(self, gridpack=None, cleanup_herwig_scratch=True):
    if gridpack:
      hw7Control.do_uncompress_gridpack(gridpack)
    hw7Control.do_run(self, cleanup_herwig_scratch)


  # configuration commands ----------------------------------------------------

  ## Configure the sampler
  ##
  ## \warning Please be very careful when modifying the sampler settings
  ##          and ensure that the statistics in the integration phase is
  ##          sufficient to provide an adequate phase space sampling and setup.
  ##          The total cross section estimated after the integration is
  ##          output before the event generation, please watch out for the
  ##          corresponding lines beginning with
  ##
  ##              Py:Herwig7Utils      INFO Calculating cross section after integration
  ##
  ##          If the statistical uncertainty on this estimated total cross
  ##          section is larger than 0.2% a warning is printed, encouraging
  ##          you to consider increasing the statistics of the integration.
  ##
  def sampler_commands(self, bin_sampler="CellGridSampler",
                       initial_points=10000, n_iterations=1, remapper_points=50000,
                       exploration_steps=4, exploration_points=500):

    bin_samplers = ["CellGridSampler", "MonacoSampler", "FlatBinSampler"]

    if not bin_sampler in bin_samplers:
      raise RuntimeError(hw7Utils.ansi_format_error("Parameter 'bin_sampler' must be one of {}!".format(bin_samplers)))

    self.commands += """
## Sampler Configuration
set /Herwig/Samplers/Sampler:BinSampler /Herwig/Samplers/{}
set /Herwig/Samplers/Sampler:BinSampler:InitialPoints {}
set /Herwig/Samplers/Sampler:BinSampler:NIterations {}
set /Herwig/Samplers/Sampler:BinSampler:RemapperPoints {}
""".format(bin_sampler, initial_points, n_iterations, remapper_points)

    if bin_sampler == "CellGridSampler":
      self.commands += """
set /Herwig/Samplers/CellGridSampler:ExplorationSteps {}
set /Herwig/Samplers/CellGridSampler:ExplorationPoints {}
""".format(exploration_steps, exploration_points)
