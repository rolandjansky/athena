#!/usr/bin/env python

from TrigHLTRatesFromCool import TrigHltRates
import unittest

class TestTrigHltRates(unittest.TestCase):

    def setUp(self):
        self.hltrates = TrigHltRates.TrigHltRates(ratetag="TestTagRateM13", chaintag="TestTagChainM13")
        self.runno = 319018 # Run during M13

    #Each test function MUST start with test, rest is not important
    def testException(self):
        """ Test exceptions that is raised by __get_sor_eor in case run number doesn't exist """
        self.assertRaises(Exception, self.hltrates.getTimestamp(runno=400000, iov=0))

    def testIOV(self):
        """ Test IOV-timestamp conversions """

        iov = self.hltrates.getIOV(runno=self.runno, timestamp=1490875399)
        self.assertEqual(iov,-1)

        iov = self.hltrates.getIOV(runno=self.runno, timestamp=1490875400)
        self.assertEqual(iov,0)

        iov = self.hltrates.getIOV(runno=self.runno, timestamp=1490883690)
        self.assertEqual(iov,138)

        iov = self.hltrates.getIOV(runno=self.runno, timestamp=1490888120)
        self.assertEqual(iov,-1)

        timestamp =  self.hltrates.getTimestamp(runno=self.runno, iov=0)
        self.assertEqual(timestamp,1490875400)

        timestamp =  self.hltrates.getTimestamp(runno=self.runno, iov=1)
        self.assertEqual(timestamp,1490875460)

        timestamp =  self.hltrates.getTimestamp(runno=self.runno, iov=138)
        self.assertEqual(timestamp,1490883680)

        timestamp =  self.hltrates.getTimestamp(runno=self.runno, iov=-1)
        self.assertEqual(timestamp,-1)

        timestamp =  self.hltrates.getTimestamp(runno=self.runno, iov=1000)
        self.assertEqual(timestamp,-1)

    def testRegex(self):
        """ Test if regex matching works correctly """

        return_code, chainratedict =  self.hltrates.getRates(runno=319018,chains=[".*rp_Cosm.*"],iov=5)
        expected = ['grp_CosmicSlice', 'grp_Cosmic_Calo', 'grp_Cosmic_LArCalibration', 'grp_Cosmic_MinBias', 'grp_Cosmic_Muon', 'grp_Cosmic_TileCalibration', 'grp_Cosmic_Tracking']
        result = chainratedict.keys(); result.sort()
        self.assertEqual(result,expected)

    def testReturnCodes(self):
        """ Test if return codes are correct. From negative codes, only -2 is possible to test """

        return_code, chainratedict =  self.hltrates.getAllRates(runno=self.runno,chains=[".*rp_Cosm.*"])
        self.assertEqual(return_code,7)

        return_code, chainratedict =  self.hltrates.getAllRates(runno=self.runno,chains=["HLT.*"])
        self.assertEqual(return_code,2578)

        return_code, chainratedict =  self.hltrates.getAllRates(runno=self.runno,chains=[".*rp_Cosm.*", "HLT.*"])
        self.assertEqual(return_code,2578+7)

        return_code, chainratedict =  self.hltrates.getRates(runno=400000,chains=[".*rp_Cosm.*"],iov=5)
        self.assertEqual(return_code,-2)

        return_code, chainratedict =  self.hltrates.getRates(runno=self.runno,chains=["Wrong_Pattern.*"],iov=5)
        self.assertEqual(return_code,0)

        return_code, chainratedict =  self.hltrates.getRates(runno=self.runno,iov=5)
        self.assertEqual(return_code,2766)

#To run the tests:
unittest.main()
