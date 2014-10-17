# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecCalibrationFinder.py

# David Adams
# September 2014
#
# Class to retrieve the calibration tool for a given jet definition
# and calibration sequence.
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
# The calibration tool is created

class JetRecCalibrationFinder:
    
    calibStep = {
        "a":"JetArea",
        "r":"Residual",
        "j":"AbsoluteEtaJES",
        "g":"GSC",
        "i":"Insitu"
        }
    
    def find(self, alg, rad, inpin, seq):
        from JetCalibTools.JetCalibToolsConf import JetCalibrationTool
        from JetRec.JetRecStandard import jtm
        config = "JES_Full2012dataset_Preliminary_MC14.config",   # Apparently does not have the required initial P4 state name
        config = "JES_Full2012dataset_Preliminary_Jan13.config"
        inp = inpin
        if config == "JES_Full2012dataset_Preliminary_Jan13.config":
          if inp == "EMTopo": inp = "TopoEM"
        configfile = 'JetCalibTools/CalibrationConfigs/' + config
        myname = "JetRecCalibrationFinder:find: "
        print myname + "Arguments:"
        print myname + "  alg: " + str(alg)
        print myname + "  rad: " + str(rad)
        print myname + "  inp: " + str(inp)
        print myname + "  seq: " + str(seq)
        jetdefn = alg + str(int(10*rad+0.1)) + inp
        print myname + "jetdefn: " + jetdefn
        tname = "calib_" + jetdefn + "_" + seq
        print myname + "tool name: " + tname

        if not tname in jtm.tools:
            # build calib tool
            print myname + "Creating " + tname
            try:
                fullseq = [self.calibStep[l] for l in seq] # translate letters
            except KeyError as err:
                print myname + "  ERROR Invalid sequence: " + seq
                print myname + "  ERROR Unknown sequence key: " + err.message
                raise err
            fullseq = '_'.join(fullseq)  # join seq names with a '_'
            print myname + "  Calibration sequence: " + fullseq
            jtm += JetCalibrationTool(tname, JetCollection =jetdefn, ConfigFile =configfile, CalibSequence =fullseq)
        return jtm.tools[tname]

# This is the only instance of the above class that should be used to
# that should be used to fetch calibration tools.
jrcf = JetRecCalibrationFinder()
