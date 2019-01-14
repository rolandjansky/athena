#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

def writeEmulationFiles(data):
    """ Writes emulation files. key in the dict is a file name (+.dat), list which is value of each dict el is enetered into the file, one el. per line"""
    for name, d in data.iteritems():
        with open(name+".dat", "w") as f:
            for event in d:
                f.write(event)
                f.write("\n")

# Testing menu used in the L1 decoders
class MenuTest:
    
    CTPToChainMapping = {"HLT_e3_etcut":   "L1_EM3",        
                         "HLT_e5_etcut":   "L1_EM3",        
                         "HLT_g5_etcut":   "L1_EM3",        
                         "HLT_e7_etcut":   "L1_EM7",        
                         "HLT_mu6idperf": "L1_MU6",        
                         "HLT_mu6":       "L1_MU6",        
                         "HLT_mu20":       "L1_MU20",        
                         "HLT_xs20":      "L1_EM7",        
                         "HLT_2e3_etcut": "L1_2EM3",        
                         "HLT_e3e5_etcut":"L1_2EM3",        
                         "HLT_2mu6":      "L1_2MU4",        
                         "HLT_e15mu24":    "L1_EM7_MU15",    
                         "HLT_xe10":      "L1_XE10",   
                         "HLT_te15":      "L1_TE15.0ETA24", 
                         "HLT_j85":       "L1_J20",         
                         "HLT_j60":       "L1_J20",
                         "HLT_j35_gsc45_boffperf_split" : "L1_J20",                         
                         "HLT_j35_gsc45_bmv2c1070_split" : "L1_J20",
                         "HLT_j35_gsc45_bmv2c1070" : "L1_J20"
      }

def applyMenu(l1decoder ):
    l1decoder.ChainToCTPMapping = MenuTest.CTPToChainMapping
        
    
# L1Decoder for bytestream
from L1Decoder.L1DecoderConf import L1Decoder
class L1DecoderTest(L1Decoder) :
    def __init__(self, name='L1DecoderTest', *args, **kwargs):
        super(L1DecoderTest, self).__init__(name, *args, **kwargs)
        
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from L1Decoder.L1DecoderMonitoring import CTPUnpackingMonitoring, RoIsUnpackingMonitoring
        from L1Decoder.L1DecoderConf import CTPUnpackingTool, EMRoIsUnpackingTool, MURoIsUnpackingTool, METRoIsUnpackingTool
        from L1Decoder.L1DecoderConf import TAURoIsUnpackingTool, JRoIsUnpackingTool
        from L1Decoder.L1DecoderConf import RerunRoIsUnpackingTool
        from TrigEDMConfig.TriggerEDMRun3 import recordable
        # CTP unpacker

        ctpUnpacker = CTPUnpackingTool(OutputLevel = self.OutputLevel,
                                       ForceEnableAllChains = True)


        self.ChainToCTPMapping = MenuTest.CTPToChainMapping
        self.ctpUnpacker = ctpUnpacker

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
          
            emUnpacker = EMRoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                                Decisions = recordable("L1EM"),
                                                OutputTrigRoIs = recordable("EMRoIs"),
                                                MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 ))

#            emUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="EM", maxCount=30 )
            self.roiUnpackers += [emUnpacker]
            self.rerunRoiUnpackers += [ RerunRoIsUnpackingTool("EMRerunRoIsUnpackingTool", 
                                                               SourceDecisions="L1EM", 
                                                               Decisions="RerunL1EM" 
                                                               ) ]

            metUnpacker = METRoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                               Decisions = recordable("L1MET"))

            self.roiUnpackers += [metUnpacker]

            tauUnpacker = TAURoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                             Decisions = "L1TAU",
                                             OutputTrigRoIs = "TAURoIs")

            tauUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="TAU", maxCount=30 )
            self.roiUnpackers += [tauUnpacker]

            jUnpacker = JRoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                             Decisions = recordable("L1J"),
                                             OutputTrigRoIs = recordable("JETRoI") )

            jUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="J", maxCount=30 )
            self.roiUnpackers += [jUnpacker]


        # MU unpacker
        if TriggerFlags.doMuon():
            muUnpacker = MURoIsUnpackingTool(OutputLevel = self.OutputLevel,
                                             Decisions = recordable("L1MU"),
                                             OutputTrigRoIs = recordable("MURoIs"))

            muUnpacker.MonTool = RoIsUnpackingMonitoring( prefix="MU", maxCount=20 )
            self.roiUnpackers += [muUnpacker]
            self.rerunRoiUnpackers += [  RerunRoIsUnpackingTool("MURerunRoIsUnpackingTool",
                                                                SourceDecisions="L1MU", 
                                                                Decisions="RerunL1MU"
                                                                ) ]
        self.L1DecoderSummaryKey = "L1DecoderSummary"


# L1 emulation for RDO
class L1EmulationTest(L1Decoder) :
    def __init__(self, name='L1EmulationTest', *args, **kwargs):
        super(L1EmulationTest, self).__init__(name, *args, **kwargs)

        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool

        self.RoIBResult = ""

        data = {}
        data['CTPEmulation'] = ['HLT_e3 HLT_g5 HLT_e7 HLT_2e3 HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4',
                                'HLT_e3 HLT_g5 HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4',
                                'HLT_e3 HLT_g5 HLT_e7 HLT_2e3 HLT_mu6 HLT_2mu6',
                                'HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4', ]   # just to see some change
        data['RoIEmulation'] = ['1.3,2.9,2704088841,EM3,EM7; 1.2,3.1,2972524297,EM3,EM7,EM10,EM15; -3.2,-2.0,3103727387,MU0,MU4,MU6,MU8',
                                '1.2,1.9,2733969453,MU0,MU4,MU6,MU8,MU10;2.2,1.0,2733969453,MU0,MU4,MU6',
                                '-3.2,3.0,2704088841,MU0,MU4,MU6,MU8;3.0,1.6,2972524297,MU0,MU4',
                                '1.3,1.9,3103727387,MU0,MU10;1.2,2.6,2733969453,MU6;-1.1,2.6,2972524297,MU6; -1.2,2.6,2704088842,MU20']

        from TrigUpgradeTest.TestUtils import writeEmulationFiles
        writeEmulationFiles(data)
        ctpUnpacker = CTPUnpackingEmulationTool(OutputLevel = self.OutputLevel,
                                                ForceEnableAllChains = True)
        self.ctpUnpacker = ctpUnpacker
        self += ctpUnpacker

        self.ChainToCTPMapping = {'HLT_mu20'     : 'L1_MU8', 
                                  'HLT_mu81step' : 'L1_MU8', 
                                  'HLT_mu8'      : 'L1_MU8', 
                                  'HLT_e20'      : 'L1_EM12', 
                                  'HLT_e8'       : 'L1_EM7', 
                                  'HLT_mu8_e8'   : 'L1_EM3_MU6'}

        # EM unpacker
        if TriggerFlags.doID() or TriggerFlags.doCalo():
            emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool",
                                                    Decisions = "EMRoIDecisions",
                                                    OutputTrigRoIs = "EMRoIs",
                                                    OutputLevel = self.OutputLevel)
            emUnpacker.ThresholdToChainMapping = ['EM7 : HLT_e20', 
                                                  'EM7 : HLT_e8', 
                                                  'EM7 : HLT_mu8_e8']
            self.roiUnpackers += [emUnpacker]
            print emUnpacker


        # MU unpacker
        if TriggerFlags.doMuon():
            muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool",
                                                    Decisions = "MURoIDecisions",
                                                    OutputTrigRoIs = "MURoIs",
                                                    OutputLevel=self.OutputLevel)
            muUnpacker.ThresholdToChainMapping =  ['MU10 : HLT_mu20', 
                                                   'MU6 : HLT_mu81step', 
                                                   'MU6 : HLT_mu8', 
                                                   'MU6 : HLT_mu8_e8']
            self.roiUnpackers += [muUnpacker]

        self.L1DecoderSummaryKey = "L1DecoderSummary"

if __name__ == "__main__":
    from AthenaCommon.Constants import DEBUG
    real = L1DecoderTest(OutputLevel=DEBUG)
    
