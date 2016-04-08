# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
from array import array
import ROOT
from PyKernel import PyKernel
import scipy
import sys,traceback

# vscharf 20150519: This algorithm doesn't work with release 20.1.X-VAL due to
# memory management issues between ROOT and python. Replaced by
# TrigT1CaloCalibUtils/L1CaloDumpRampData
class L1CaloDumpRampDataAlgorithm(PyAthena.Alg):
    """ Test reading back in conditions data. """
    def __init__(self, name="L1CaloDumpRampDataAlgorithm"):
        super(L1CaloDumpRampDataAlgorithm, self).__init__(name=name)
        self.detectorStore = None
    
    def initialize(self):
        self.detectorStore = PyAthena.py_svc("StoreGateSvc/DetectorStore")

        if self.detectorStore is None:
            self.msg.failure("Problem retrieving DetectorStore")
            return PyAthena.StatusCode.Failure

        return PyAthena.StatusCode.Success

    def execute(self):
        """ Read L1CaloRampData in finalize, nothing to do here. """
        return PyAthena.StatusCode.Success
        
    def finalize(self):
        try: # retrieve ramp data container from detstore
            rampDataContainer = PyKernel.retrieveDet(PyAthena.L1CaloRampDataContainer,
                                                     "/L1CaloRampMaker/L1CaloRampDataContainer")
            energyScanResults = PyKernel.retrieveDet(PyAthena.CondAttrListCollection,
                                                     "/TRIGGER/L1Calo/V1/Results/EnergyScanResults")
        except:
            rampDataContainer = None

        if rampDataContainer is None:
            self.msg.error("Error retrieve L1Calo data.")
            return PyAthena.StatusCode.Failure

        # root tgraph expects array objects
        x = array('d') # calorimeter
        y = array('d') # level1 adc
        ex = array('d')
        ey = array('d')

        xmlfile = open("rampdata.xml", "w")
        func = ROOT.TF1("func", "pol1", 0., 255.)
        xml_channel = "<Channel coolid='0x%x' slope='%.4f' offset='%.4f' chi2='%.4f' />\n"
        isfinite = scipy.isfinite

        xmlfile.write("""<?xml version='1.0' encoding='utf-8'?>
<TriggerTowerDecoratorData>
<default name='slope' />
""")

        rootfile = ROOT.TFile.Open("graphs.root", "RECREATE")
        if not rootfile:
            self.msg.error("Could not open root file.")
            return PyAthena.StatusCode.Failure

        for rampDataContainerIt in rampDataContainer:
            del x[:]
            del y[:]
            del ex[:]
            del ey[:]

            coolId, rampData = (rampDataContainerIt.first, rampDataContainerIt.second)
            for step in xrange(rampData.getNSteps()):
                if step != 0 and rampData.getLevel1(step).mean() < x[-1]:
                    continue # protect against saturated tower problems
                x.append(rampData.getLevel1(step).mean())
                ex.append(rampData.getLevel1(step).rms())
                y.append(rampData.getCalo(step).mean())
                ey.append(rampData.getCalo(step).rms())

            if len(x) == 0: continue
            try:
                ramp = ROOT.TGraphErrors(len(x), x, y, ex, ey)
            except Exception, e:
                traceback.print_exc(e)
                continue
            
            attrList = energyScanResults.attributeList(coolId)
            try:
                slope = attrList["Slope"]
                offset = attrList["Offset"]
                chi2 = attrList["Chi2"]
                func.SetParameters(offset, slope)

            except Exception,e:
                self.msg.warning("Could not obtain fit data from database.")

                func.SetParameters(0., 1.)
                ramp.Fit(func, "QFR0")
                slope = func.GetParameter(1)
                offset = func.GetParameter(0)
                chi2 = func.GetChisquare()

            if isfinite(chi2) and isfinite(offset) and isfinite(slope):
                ramp.GetListOfFunctions().Add(func.Clone("func_0x%x" % coolId))
                xmlfile.write(xml_channel % (coolId, slope, offset, chi2))

            ramp.SetName("0x%x" % coolId)
            ramp.SetTitle("Ramp 0x%x" % coolId)
            ramp.Write()

        try:
            rootfile.Close()
        except Exception, e:
            traceback.print_exc(e)

        xmlfile.write("</TriggerTowerDecoratorData>\n")
        xmlfile.close()
        return PyAthena.StatusCode.Success
