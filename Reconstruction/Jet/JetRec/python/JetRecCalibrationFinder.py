# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecCalibrationFinder.py

# David Adams
# September 2014
#
# Class to retrieve the calibration tool for a given jet definition
# and calibration sequence.  The calibration tool is created if it
# does not already exist.
#
# Usage:
#   from JetRec.JetRecCalibrationFinder import jrcf
#   tool = jrcf.find(alg, rad, inp, seq, con)
#
# Arguments:
#   alg - Algorithm name: AntiKt, CamKt or Kt
#   rad - Jet size parameter, e.g. 0.4
#   inp - Input type: EMTopo or LCTopo
#   seq - Calibration sequence as one letter for each step, e.g. "ar"
#            a = Active area correction (rho*A)
#            r = Pileup residual correction (i.e. using mu and NPV)
#            j = JES correction (from MC)
#            g = GSC correction (from MC)
#            i = Insitu correction (data only)
#   con - Configuration file name or entry in jrcf.configDict
#
# To add to the confid dictionary:
#   jrcf.configDict["myname"] = "someConfigFile.config"

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

class JetRecCalibrationFinder:
    
  # Dictionary of calibrations steps.
  calibStep = {
    "a":"JetArea",
    "r":"Residual",
    "o":"Origin",
    "j":"AbsoluteEtaJES",
    "m":"JMS",
    "g":"GSC",
    "i":"Insitu"
  }
    
  # Dictionary for calibration configurations.
  configDict = {
    "reco"            : "JES_data2016_data2015_Recommendation_Dec2016_rel21.config",
    "trigger"         : "JES_Full2012dataset_Preliminary_Trigger.config",
    "triggerNoPileup" : "JES_Full2012dataset_Preliminary_Trigger_NoPileup.config",
    "trigger2016"     : "JES_MC15cRecommendation_May2016_Trigger.config",
    "triggerTrim"     : "JES_MC15recommendation_FatJet_June2015.config",
    "pflow"           : "JES_MC15cRecommendation_PFlow_Aug2016_rel21.config"
  }

  def find(self, alg, rad, inpin, seq, configkeyin, evsprefix):
    from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
    from JetRec.JetRecStandardToolManager import jtm
    inp = inpin
    # Find the configuration file.
    configkey = configkeyin
    if configkey == "": configkey = "reco"
    if configkey in self.configDict:
      configfile = self.configDict[configkey]
    else:
      configfile = configkey
    # Assign name for tool
    jetdefn = alg + str(int(10*rad+0.1)) + inp.split("Origin")[0]
    tname = "calib_" + jetdefn + "_" + configkey.replace(".","_") + "_" + seq
    # Display configuration.
    myname = "JetRecCalibrationFinder:find: "
    jetlog.info( myname + "Building jet calibration tool." )
    jetlog.info( myname + "  Arguments:" )
    jetlog.info( myname + "    alg: " + str(alg) )
    jetlog.info( myname + "    rad: " + str(rad) )
    jetlog.info( myname + "    inp: " + str(inp) )
    jetlog.info( myname + "    seq: " + str(seq) )
    jetlog.info( myname + "  Jet definition: " + jetdefn )
    jetlog.info( myname + "  Configuration file: " + configfile )

    if tname in jtm.tools:
      jetlog.info( myname + "  Skipping previously-defined tool: " + tname )
    else:
      # build calib tool
      jetlog.info( myname + "  Creating " + tname )
      # ...define calbration sequence
      try:
        fullseq = [self.calibStep[l] for l in seq] # translate letters
      except KeyError as err:
        jetlog.info( myname + "  ERROR Invalid sequence: " + seq )
        jetlog.info( myname + "  ERROR Unknown sequence key: " + err.message )
        raise err
      fullseq = '_'.join(fullseq)  # join seq names with a '_'
      jetlog.info( myname + "  Calibration sequence: " + fullseq )
      # ...define the key for the event shape container
      if   inpin == "EMTopo":
        evssuf="EMTopoEventShape"
      elif inpin == "LCTopo":
        evssuf="LCTopoEventShape"
      elif inpin.startswith("LCTopoTrimmed"):
        evssuf="LCTopoEventShape"
      elif inpin == "EMTopoOrigin":
        evssuf="EMTopoOriginEventShape"
      elif inpin == "LCTopoOrigin":
        evssuf="LCTopoOriginEventShape"
      elif inpin == "EMPFlow":
        evssuf="EMPFlowEventShape"
      elif inpin == "EMCPFlow":
        evssuf="EMCPFlowEventShape"
      elif inpin == "LCPFlow":
        evssuf="LCPFlowEventShape"
      else:
        evssuf="INVALID"
        jetlog.info( myname + "  ERROR: Invalid input specifier: " + inp )
        raise KeyError
      evskey = evsprefix + evssuf
      jetlog.info( myname + "  Event shape key: " + evskey )
      # ...create the tool.
      jtm += JetCalibrationTool(tname, JetCollection=jetdefn, ConfigFile=configfile, CalibSequence=fullseq, RhoKey=evskey)

    return jtm.tools[tname]

# This is the only instance of the above class that should be used to
# that should be used to fetch calibration tools.
jrcf = JetRecCalibrationFinder()
