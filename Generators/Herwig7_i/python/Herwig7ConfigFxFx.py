#! /usr/bin/env python

## \file Herwig7ConfigFxFx.py
## \brief Configuration class for showering FxFx-merged LHE files from MG5_aMC@NLO
## \author Krishna Kulkarni (krishna.kulkarni@cern.ch)
## \author Daniel Rauch (daniel.rauch@desy.de)
##
## \warning Only use this for testing/debugging purposes.
##          This run mode is neither fully implemented nor validated or supported yet.

import Herwig7Config as hw7Config
import Herwig7Control as hw7Control
import Herwig7Utils as hw7Utils

from AthenaCommon import Logging
athMsgLog = Logging.logging.getLogger('Herwig7Control')


## Configuration class for showering FxFx-merged LHE files from MG5_aMC@NLO
##
## \warning Only use this for testing/debugging purposes.
##          This run mode is neither fully implemented nor validated or supported yet.
class Hw7ConfigFxFx(hw7Config.Hw7Config):


  def __init__(self, genSeq, runArgs, run_name="Herwig", beams="pp"):

    beams = beams.upper()
    if not beams in ["EE", "EP" , "PP"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Parameter 'beams' must be one of the following: ['EE', 'EP' , 'PP']"))

    # provide variables initialized by the parent class
    super(Hw7ConfigFxFx, self).__init__(genSeq, runArgs, run_name)

    self.beams = beams

    self.set_fxfx_commands = False


  def local_pre_commands(self):

    return """
## ============================================
## Local Pre-Commands from Herwig7ConfigFxFx.py
## ============================================

# > no local pre-commands at the moment
"""


  def local_post_commands(self):

    return """
## =============================================
## Local Post-Commands from Herwig7ConfigFxFx.py
## =============================================

saverun {} /Herwig/Generators/EventGenerator
""".format(self.run_name)



  ## High-level function for triggering the process setup and the event generation.
  ##
  ## \warning Please do not modify the generator configuration in the job options after calling the `run()` function as
  ##          the modified settings would not be applied in the event generation
  ##
  def run(self):

    # add default settings if they were not overwritten in the JobOptions

    self.default_commands += self.beam_commands()
    self.default_commands += self.random_seed_commands()
    
    if not self.set_printout_commands:
      self.default_commands += self.printout_commands()
    if not self.set_physics_parameter_commands:
      self.default_commands += self.physics_parameter_commands()
    if not self.set_technical_parameter_commands:
      self.default_commands += self.technical_parameter_commands()

    # check for configuration errors

    if not self.set_fxfx_commands:
      raise RuntimeError(hw7Utils.ansi_format_error("Please use 'fxfx_commands()' in the JobOptions!"))

    # exit with error message since this run mode is not yet to be used
    # raise RuntimeError(hw7Utils.ansi_format_error("Please don't run FxFx event generation as the interface for the FxFx run mode is neither fully implemented nor validated or supported yet"))

    # do read and run step in one go
    hw7Control.run(self)


  ## Sets up reading of events from an LHE file
  ##
  ## \todo Settings which are not FxFx-specific should be identical to the LHEF showering of MG5_aMC@NLO events.
  ##       This should probably be enforced in some way
  ##
  ## \param[in] merging_scale Merging scale in units of GeV
  ## \param[in] njets_max     Number of (Born-level) light jets in maximum-multiplicity FxFx process
  ## \param[in] me_pdf_order  Order of the PDF set to be used
  ## \param[in] r_clus        Jet radius used in clustering in the merging
  ## \param[in] eta_clus_max  Max |eta| for jets in clustering in the merging
  ##                          (should match the range used for jets in the hard process generation, which should normally be unrestricted
  ## \param[in] usespin       Use the spin of tau leptons from the LHE file (spins of other particles are ignored anyways)
  ##
  ## \todo Remove ihrd and ihvy parameters beginning with Herwig 7.1.0 since the hard process will be deduced automatically
  def fxfx_commands(self,
                    merging_scale,
                    njets_max,
                    lhe_filename = "events.lhe",
                    me_pdf_order = "NLO",
                    r_clus = 1.0,
                    eta_clus_max = 15.0,
                    usespin = True,
                    automatic_process_detection = True,
                    ihrd = None,
                    ihvy = None):

    if not me_pdf_order in ["LO", "NLO"]:
      raise RuntimeError(hw7Utils.ansi_format_error("Herwig7ConfigLHEF.py:__lhef_commands: Parameter 'me_pdf_order' must either be 'LO' or 'NLO'!"))

    self.set_fxfx_commands = True

    process_selection = ""
    if not automatic_process_detection:
      process_selection = """
## The ihrd and ihvy parameters are Alpgen parameters to identify the hard process
set /Herwig/Shower/FxFxShowerHandler:ihrd {}
set /Herwig/Shower/FxFxShowerHandler:ihvy {}
""".format(ihrd, ihvy)

    self.commands += """
## ----------------------------
## Read in Events from LHE File
## ----------------------------

#library LesHouches.so
#create ThePEG::LesHouchesFileReader /Herwig/EventHandlers/LHEReader


## Create the Handler and Reader
library FxFx.so
create Herwig::FxFxEventHandler /Herwig/EventHandlers/FxFxLHEHandler
create Herwig::FxFxFileReader /Herwig/EventHandlers/FxFxLHEReader

library FxFxHandler.so
create Herwig::FxFxHandler /Herwig/Shower/FxFxShowerHandler
set /Herwig/Shower/FxFxShowerHandler:ShowerModel /Herwig/Shower/ShowerModel
set /Herwig/Shower/FxFxShowerHandler:SplittingGenerator /Herwig/Shower/SplittingGenerator


## Set up the LHE reader
set /Herwig/EventHandlers/FxFxLHEReader:WeightWarnings      false
set /Herwig/EventHandlers/FxFxLHEReader:FileName            {FileName}
set /Herwig/EventHandlers/FxFxLHEReader:MomentumTreatment   RescaleEnergy
# set /Herwig/EventHandlers/FxFxLHEReader:IgnoreIDPRUP      Yes

# Don't use any cuts on LHE files
create ThePEG::Cuts /Herwig/Cuts/NoCuts
set /Herwig/EventHandlers/FxFxLHEReader:Cuts /Herwig/Cuts/NoCuts



insert /Herwig/EventHandlers/FxFxLHEHandler:FxFxReaders 0 /Herwig/EventHandlers/FxFxLHEReader
set /Herwig/EventHandlers/FxFxLHEHandler:PartonExtractor /Herwig/Partons/PPExtractor
set /Herwig/EventHandlers/FxFxLHEHandler:CascadeHandler /Herwig/Shower/FxFxShowerHandler
set /Herwig/EventHandlers/FxFxLHEHandler:HadronizationHandler /Herwig/Hadronization/ClusterHadHandler
set /Herwig/EventHandlers/FxFxLHEHandler:DecayHandler /Herwig/Decays/DecayHandler

set /Herwig/EventHandlers/FxFxLHEHandler:WeightOption VarNegWeight


## MC@NLO settings

#set /Herwig/Shower/KinematicsReconstructor:ReconstructionOption General 
#set /Herwig/Shower/KinematicsReconstructor:InitialInitialBoostOption LongTransBoost
#set /Herwig/Shower/KinematicsReconstructor:FinalStateReconOption Default
#set /Herwig/Shower/KinematicsReconstructor:InitialStateReconOption Rapidity
set /Herwig/Shower/ShowerHandler:MaxPtIsMuF Yes
set /Herwig/Shower/ShowerHandler:RestrictPhasespace Yes
#set /Herwig/Shower/ShowerHandler:ReconstructionOption OffShell4

set /Herwig/Shower/PartnerFinder:PartnerMethod Random
set /Herwig/Shower/PartnerFinder:ScaleChoice Partner
set /Herwig/Shower/ShowerHandler:SpinCorrelations No

set /Herwig/Shower/ShowerHandler:MaxTry               100


## Set up the FxFxShowerHandler

set /Herwig/Shower/FxFxShowerHandler:MPIHandler  /Herwig/UnderlyingEvent/MPIHandler
set /Herwig/Shower/FxFxShowerHandler:RemDecayer  /Herwig/Partons/RemnantDecayer
set /Herwig/Shower/FxFxShowerHandler:ShowerAlpha /Herwig/Shower/AlphaQCD

## Use the automatic process detection
set /Herwig/Shower/FxFxShowerHandler:HardProcessDetection {AutomaticProcessDetection}{ProcessSelection}

# Number of (Born-level) light jets in maximum-multiplicity FxFx process
set /Herwig/Shower/FxFxShowerHandler:njetsmax      {NjetsMax} # Make sure this is set correctly!

# Mimimum parton-parton R-sep used for generation.
# (legacy from MLM merging, should stay fixed to zero in FxFx as this is NLO)
set /Herwig/Shower/FxFxShowerHandler:drjmin      0.0

## Recommended key MLM merging parameters below

# turn the Vetoing On or Off completely
set /Herwig/Shower/FxFxShowerHandler:VetoIsTurnedOff VetoingIsOn
# merging scale
set /Herwig/Shower/FxFxShowerHandler:ETClus {MergingScale}*GeV
# jet radius used in clustering in merging.
set /Herwig/Shower/FxFxShowerHandler:RClus {RClus}
# factor to multiply RClus (if DR(parton,jet)<rclusfactor*rclus
# the parton and jet are said to have been matched)
set /Herwig/Shower/FxFxShowerHandler:RClusFactor 1.0
# Max |eta| for jets in clustering in merging.
set /Herwig/Shower/FxFxShowerHandler:EtaClusMax {EtaClusMax}

## Setup event handlers
set /Herwig/Generators/EventGenerator:EventHandler /Herwig/EventHandlers/FxFxLHEHandler


## UNSURE ABOUT THESE ---------------------------------------------------------

# set /Herwig/EventHandlers/FxFxLHEReader:IncludeSpin {IncludeSpin}
# set /Herwig/EventHandlers/FxFxLHEReader:AllowedToReOpen No

## Set the PDF for the LHE reader.
set /Herwig/EventHandlers/FxFxLHEReader:PDFA /Herwig/Partons/Hard{MEPDFOrder}PDF
set /Herwig/EventHandlers/FxFxLHEReader:PDFB /Herwig/Partons/Hard{MEPDFOrder}PDF
# The PDF for beam particles A/B - overrides particle's own PDF above
set /Herwig/Shower/FxFxShowerHandler:PDFA    /Herwig/Partons/Hard{MEPDFOrder}PDF
set /Herwig/Shower/FxFxShowerHandler:PDFB    /Herwig/Partons/Hard{MEPDFOrder}PDF
""".format(MergingScale = merging_scale,
           NjetsMax = njets_max,
           FileName = lhe_filename,
           MEPDFOrder = me_pdf_order,
           RClus = r_clus,
           EtaClusMax = eta_clus_max,
           IncludeSpin = "Yes" if usespin==True else "No",
           AutomaticProcessDetection = "Automatic" if automatic_process_detection else "Manual",
           ProcessSelection = process_selection)
