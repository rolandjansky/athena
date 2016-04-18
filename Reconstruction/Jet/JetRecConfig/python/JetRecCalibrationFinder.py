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

class JetRecCalibrationFinder:
    
  # Dictionary of calibrations steps.
  calibStep = {
    "a":"JetArea",
    "r":"Residual",
    "o":"Origin",
    "j":"AbsoluteEtaJES",
    "g":"GSC",
    "i":"Insitu"
  }
    
  # Dictionary for calibration configurations.
  configDict = {
    "reco"            : "JES_Full2012dataset_Preliminary_Jan13.config",
    "trigger"         : "JES_Full2012dataset_Preliminary_Trigger.config",
    "triggerNoPileup" : "JES_Full2012dataset_Preliminary_Trigger_NoPileup.config",
    "pflow"           : "PFlowJES_September2014.config"
  }

  def find(self, alg, rad, inpin, seq, configkeyin, evsprefix):
    from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
    from JetRecStandardToolManager import jtm
    inp = inpin
    # Find the configuration file.
    configkey = configkeyin
    if configkey == "": configkey = "reco"
    if configkey in self.configDict:
      configfile = self.configDict[configkey]
    else:
      configfile = configkey
    # Assign name for tool
    jetdefn = alg + str(int(10*rad+0.1)) + inp
    tname = "calib_" + jetdefn + "_" + configkey.replace(".","_") + "_" + seq
    # Display configuration.
    myname = "JetRecCalibrationFinder:find: "
    print myname + "Building jet calibration tool."
    print myname + "  Arguments:"
    print myname + "    alg: " + str(alg)
    print myname + "    rad: " + str(rad)
    print myname + "    inp: " + str(inp)
    print myname + "    seq: " + str(seq)
    print myname + "  Jet definition: " + jetdefn
    print myname + "  Configuration file: " + configfile

    if tname in jtm.tools:
      print myname + "  Skipping previously-defined tool: " + tname
    else:
      # build calib tool
      print myname + "  Creating " + tname
      # ...define calbration sequence
      try:
        fullseq = [self.calibStep[l] for l in seq] # translate letters
      except KeyError as err:
        print myname + "  ERROR Invalid sequence: " + seq
        print myname + "  ERROR Unknown sequence key: " + err.message
        raise err
      fullseq = '_'.join(fullseq)  # join seq names with a '_'
      print myname + "  Calibration sequence: " + fullseq
      # ...define the key for the event shape container
      if   inpin == "EMTopo":
        evssuf="EMTopoEventShape"
      elif inpin == "LCTopo":
        evssuf="LCTopoEventShape"
      elif inpin == "EMPFlow":
        evssuf="EMPFlowEventShape"
      elif inpin == "EMCPFlow":
        evssuf="EMCPFlowEventShape"
      elif inpin == "LCPFlow":
        evssuf="LCPFlowEventShape"
      else:
        evssuf="INVALID"
        print myname + "  ERROR: Invalid input specifier: " + inp
        raise KeyError
      evskey = evsprefix + evssuf
      print myname + "  Event shape key: " + evskey
      # ...create the tool.
      jtm += JetCalibrationTool(tname, JetCollection=jetdefn, ConfigFile=configfile, CalibSequence=fullseq, RhoKey=evskey)

    return jtm.tools[tname]

# This is the only instance of the above class that should be used to
# that should be used to fetch calibration tools.
jrcf = JetRecCalibrationFinder()
