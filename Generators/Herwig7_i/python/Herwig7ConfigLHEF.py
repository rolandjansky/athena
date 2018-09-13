#! /usr/bin/env python

## \file Herwig7ConfigLHEF.py
## \brief Configuration class for showering LHE files from MG5_aMC@NLO or PowhegBox
## \author Daniel Rauch (daniel.rauch@desy.de)

import os

import Herwig7Config as hw7Config
import Herwig7Control as hw7Control
import Herwig7Utils as hw7Utils


## Configuration class for showering LHE files from MG5_aMC@NLO or PowhegBox
##
## Example JobOptions are available in [`examples/LHEF`](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig7_i/trunk/examples/LHEF) and [`tests/athenaLHEF/jobOptions`](https://svnweb.cern.ch/trac/atlasoff/browser/Generators/Herwig7_i/trunk/tests/athenaLHEF/jobOptions).
## 
##
## Process Setup and Generator Configuration
## =========================================
##
## The LHE files have to follow a specific naming scheme which is detailed
## [here](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PreparingLesHouchesEven).
## The name of the LHE file is added as the command line parameter `--inputGeneratorFile`
## to the `Generate_tf.py` command and can be accessed using the variable
## `runArgs.inputGeneratorFile` in the JobOptions where they can be passed to the
## functions [lhef_mg5amc_commands](\ref Herwig7ConfigLHEF::Hw7ConfigLHEF#lhef_mg5amc_commands)
## or [lhef_powhegbox_commands](\ref Herwig7ConfigLHEF::Hw7ConfigLHEF#lhef_powhegbox_commands)
## in order to communicate the LHE file names to %Herwig7, e.g.
## 
## \code{.py}
## generator.lhef_mg5amc_commands(lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")
## \endcode
## 
## or
## 
## \code{.py}
## generator.lhef_powhegbox_commands(lhe_filename=runArgs.inputGeneratorFile, me_pdf_order="NLO")
## \endcode
##
##
## Event Generation
## ================
##
## To trigger the actual running of %Herwig7 in Athena please end the JobOptions with
## 
## \code{.py}
## # run the generator
## generator.run()
## \endcode
##
## More specifically, the `run()` function triggers the creation of the %Herwig7 input
## file and the preparation of the run (i.e. the `Herwig read` step).
## This means, that no %Herwig7 settings should be modified after calling the
## `run()` function because the changed settings would not be applied during the
## event generation.
##
class Hw7ConfigLHEF(hw7Config.Hw7Config):


  def __init__(self, genSeq, runArgs, run_name="Herwig"):

    ## provide variables initialized by the parent class
    super(Hw7ConfigLHEF, self).__init__(genSeq, runArgs, run_name)

    self.event_generator = "LHCGenerator"

    self.set_lhef_mg5amc_commands = False
    self.set_lhef_powhegbox_commands = False


  def local_pre_commands(self):

    return """
## ============================================
## Local Pre-Commands from Herwig7ConfigLHEF.py
## ============================================

# > no local pre-commands at the moment
"""


  def local_post_commands(self):

    return """
## =============================================
## Local Post-Commands from Herwig7ConfigLHEF.py
## =============================================

saverun {} /Herwig/Generators/LHCGenerator
""".format(self.run_name)



  ## High-level function for triggering the process setup and the event generation.
  ##
  ## \warning Please do not modify the generator configuration in the job options after calling the `run()` function as
  ##          the modified settings would not be applied in the event generation
  ##
  def run(self):

    ## add default settings if they were not overwritten in the JobOptions

    self.default_commands += self.random_seed_commands()
    
    if not self.set_printout_commands:
      self.default_commands += self.printout_commands()
    if not self.set_physics_parameter_commands:
      self.default_commands += self.physics_parameter_commands()
    if not self.set_technical_parameter_commands:
      self.default_commands += self.technical_parameter_commands()

    ## check for configuration errors

    if self.set_lhef_mg5amc_commands and self.set_lhef_powhegbox_commands:
      raise RuntimeError(hw7Utils.ansi_format_error("Please do not use both 'lhef_mg5amc_commands()' and 'lhef_powhegbox_commands()' at the same time!"))
    if not self.set_lhef_mg5amc_commands and not self.set_lhef_powhegbox_commands:
      raise RuntimeError(hw7Utils.ansi_format_error("Please use either 'lhef_mg5amc_commands()' or 'lhef_powhegbox_commands()' in the JobOptions!"))

    ## do read and run step in one go

    hw7Control.run(self)


  ## Sets up reading of events from an LHE file
  ##
  ## \param usespin Use the spin of tau leptons from the LHE file (spins of other particles are ignored anyways)
  def __lhef_commands(self, lhe_filename="events.lhe", me_pdf_order="NLO", usespin=True):

    if not me_pdf_order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Herwig7ConfigLHEF.py:__lhef_commands: Parameter 'me_pdf_order' must either be 'LO' or 'NLO'!"))

    if lhe_filename.endswith(".tar.gz"): # athena will decompress the LHE file for us
      lhe_filename = lhe_filename.replace(".tar.gz", ".events")
      if not os.path.isfile(lhe_filename):
        raise RuntimeError(hw7Utils.ansi_format_error("Herwig7ConfigLHEF.py:__lhef_commands: Could not find decompressed LHE file '{}'!".format(lhe_filename)))

    self.commands += """
## ----------------------------
## Read in Events from LHE File
## ----------------------------

## Create the Handler and Reader
library LesHouches.so
create ThePEG::LesHouchesFileReader /Herwig/EventHandlers/LHEReader
create ThePEG::LesHouchesEventHandler /Herwig/EventHandlers/LHEHandler

## Set LHE filename
set /Herwig/EventHandlers/LHEReader:FileName {FileName}

## Setup event handlers
set /Herwig/Generators/LHCGenerator:EventHandler /Herwig/EventHandlers/LHEHandler
insert /Herwig/EventHandlers/LHEHandler:LesHouchesReaders 0 /Herwig/EventHandlers/LHEReader
set /Herwig/EventHandlers/LHEHandler:PartonExtractor /Herwig/Partons/QCDExtractor
set /Herwig/EventHandlers/LHEHandler:CascadeHandler /Herwig/Shower/ShowerHandler
set /Herwig/EventHandlers/LHEHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler
set /Herwig/EventHandlers/LHEHandler:DecayHandler /Herwig/Decays/DecayHandler

##
set /Herwig/EventHandlers/LHEReader:IncludeSpin {IncludeSpin}
set /Herwig/EventHandlers/LHEReader:MomentumTreatment RescaleEnergy
set /Herwig/EventHandlers/LHEReader:AllowedToReOpen No
set /Herwig/EventHandlers/LHEHandler:WeightNormalization CrossSection
set /Herwig/EventHandlers/LHEHandler:WeightOption VarNegWeight

## Parton shower settings
set /Herwig/Shower/Evolver:HardVetoMode Yes
set /Herwig/Shower/Evolver:HardVetoScaleSource Read

## Don't use any cuts on LHE files
create ThePEG::Cuts /Herwig/Cuts/NoCuts
set /Herwig/EventHandlers/LHEReader:Cuts /Herwig/Cuts/NoCuts

## Set the PDF for the LHE reader.
set /Herwig/EventHandlers/LHEReader:PDFA /Herwig/Partons/Hard{MEPDFOrder}PDF
set /Herwig/EventHandlers/LHEReader:PDFB /Herwig/Partons/Hard{MEPDFOrder}PDF
""".format(FileName = lhe_filename, MEPDFOrder = me_pdf_order,
           IncludeSpin = "Yes" if usespin==True else "No")


  ## Commands specific to showering of events produced with MG5_aMC@NLO
  ##
  ## \param[in] lhe_filename name of the LHE file that is to be showered
  ## \param[in] me_pdf_order order (either `LO` or `NLO`) of the matrix element PDF.
  ##                         This has to be consistent with the PDF used in the
  ##                         hard process generation and the PDF set with
  ##                         generator.me_pdf_commands.
  ##
  ## The reason for these specific settings is that for MC@NLO-like matching schemes
  ## the subtraction of the parton shower contribution in the hard process and the
  ## parton shower settings have to be consistent in order to recover the
  ## correct terms at the NLO.
  ##
  ## For showering of previously existing LHE files
  ##
  ##     generator.lhef_powhegbox_commands(lhe_filename=runArgs.inputGeneratorFile)
  ##
  ## will work.
  ##
  def lhef_mg5amc_commands(self, lhe_filename="events.lhe", me_pdf_order="NLO", usespin=True):

    if not me_pdf_order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Herwig7ConfigLHEF.py:lhef_mg5amc_commands: Parameter 'me_pdf_order' must either be 'LO' or 'NLO'!"))

    self.set_lhef_mg5amc_commands = True

    self.__lhef_commands(lhe_filename, me_pdf_order, usespin)

    self.commands += """
## Commands specific to showering of events produced with MG5_aMC@NLO
set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption General
set /Herwig/Shower/KinematicsReconstructor:InitialInitialBoostOption LongTransBoost
set /Herwig/Shower/KinematicsReconstructor:InitialStateReconOption Rapidity
set /Herwig/Shower/KinematicsReconstructor:FinalStateReconOption Default
set /Herwig/Shower/ShowerHandler:RestrictPhasespace On

## matrix element corrections were switched off by us in the past
## now the Herwig7 authors recommend to go with the default, which is MECorrMode 1 [Yes]
## keeping it for the moment for mg5amc, though, until new settings validated
set /Herwig/Shower/Evolver:MECorrMode 0 ## keeping this here at the moment
"""


  ## Commands specific to showering of events produced with PowhegBox
  ##
  ## \param[in] lhe_filename name of the LHE file that is to be showered
  ## \param[in] me_pdf_order order (either `LO` or `NLO`) of the matrix element PDF.
  ##                         This has to be consistent with the PDF used in the
  ##                         hard process generation and the PDF set with
  ##                         generator.me_pdf_commands.
  ## \param[in] usespin 
  ##
  ## Specifying the LHE file name with
  ##
  ##     generator.lhef_powhegbox_commands(lhe_filename=runArgs.inputGeneratorFile)
  ## 
  ## should work both for showering existing LHE files as well as for on-the-fly (OTF)
  ## running of PowhegBox and %Herwig7.
  ##
  ## Since for OTF running the LHE file generated by PowhegBox is usually
  ## called `PowhegOTF._1.events` using
  ##
  ##     generator.lhef_powhegbox_commands(lhe_filename="PowhegOTF._1.events")
  ## 
  ## will normally also work in this case. If a different name for the LHE file
  ## generated by PowhegBox is specified with the `--outputTXTFile` argument to
  ## `Generate_tf.py` then `PowhegControl` will update the value of
  ## `runArgs.inputGeneratorFile` accordingly and
  ##
  ##     generator.lhef_powhegbox_commands(lhe_filename=runArgs.inputGeneratorFile)
  ##
  ## should still work.
  ##
  def lhef_powhegbox_commands(self, lhe_filename="events.lhe", me_pdf_order="NLO", usespin=True):

    if not me_pdf_order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Herwig7ConfigLHEF.py:lhef_powhegbox_commands: Parameter 'me_pdf_order' must either be 'LO' or 'NLO'!"))

    self.set_lhef_powhegbox_commands = True

    self.__lhef_commands(lhe_filename, me_pdf_order, usespin)

    self.commands += """
## Commands specific to showering of events produced with PowhegBox
set /Herwig/Shower/ShowerHandler:RestrictPhasespace On
set /Herwig/Shower/ShowerHandler:MaxPtIsMuF Yes
set /Herwig/Shower/PartnerFinder:PartnerMethod Random
set /Herwig/Shower/PartnerFinder:ScaleChoice Partner
"""
