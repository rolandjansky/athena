# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenu.L1Topo.py')

from TriggerMenu.l1topo.TopoAlgos import SortingAlgo, DecisionAlgo
from TriggerJobOpts.TriggerFlags import TriggerFlags

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW

import re


class TopoAlgoDef:

    @staticmethod
    def registerTopoAlgos(tm):
        currentAlgoId = 0

        _etamax = 49
        
        # Sorted lists:
        
        for jet_type in ['AJ', 'CJ', 'J']:
            jetabseta = _etamax
            if jet_type=='J':
                jetabseta = 32
            elif jet_type=='CJ':
                jetabseta = 26 

            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s', algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addgeneric('JetSize', HW.DefaultJetSize.value) # JetSize
            #alg.addvariable('MinEt', 0) # Need to adjust
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', jetabseta)
            tm.registerAlgo(alg) # OK

            alg = AlgConf.JetSelect( name = jet_type+'ab', inputs = 'JetTobArray', outputs = jet_type+'ab', algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectJET )            
            alg.addgeneric('OutputWidth', HW.OutputWidthSelectJET)
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinEt', 0)  
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', jetabseta)
            tm.registerAlgo(alg) # OK

        alg = AlgConf.JetSort( name = 'AJjs', inputs = 'JetTobArray', outputs = 'AJjs', algoId = currentAlgoId); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg) # OK 

        alg = AlgConf.ClusterSort( name = 'EMs', inputs = 'ClusterTobArray', outputs = 'EMs', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('IsoMask', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax) # OK
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSort( name = 'EMshi', inputs = 'ClusterTobArray', outputs = 'EMshi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('IsoMask', 3) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg)
        
        alg = AlgConf.ClusterSort( name = 'TAUsi', inputs = 'ClusterTobArray', outputs = 'TAUsi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortTAU)
        alg.addvariable('IsoMask', 4) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg)

        alg = AlgConf.ClusterSort( name = 'TAUs', inputs = 'ClusterTobArray', outputs = 'TAUs', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortTAU)
        alg.addvariable('IsoMask', 0) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg)        

        alg = AlgConf.MetSort( name = 'XE', inputs = 'MetIn', outputs = 'XE', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerAlgo(alg)
        
        # Abbreviated lists:
        alg = AlgConf.ClusterSelect( name = 'EMab', inputs = 'ClusterTobArray', outputs = 'EMab', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinEt', 0)
        alg.addvariable('IsoMask', 0)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg) 
        
        alg = AlgConf.ClusterSelect( name = 'EMabi', inputs = 'ClusterTobArray', outputs = 'EMabi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinEt', 0)
        alg.addvariable('IsoMask', '00001') # Need to confirm with the simulation side for 'i'
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSelect( name = 'EMabhi', inputs = 'ClusterTobArray', outputs = 'EMabhi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinEt', 0)
        alg.addvariable('IsoMask', 3)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSelect( name = 'TAUab', inputs = 'ClusterTobArray', outputs = 'TAUab', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectTAU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)
        alg.addvariable('MinEt', 0) 
        alg.addvariable('IsoMask', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSelect( name = 'TAUabi', inputs = 'ClusterTobArray', outputs = 'TAUabi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectTAU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)        
        alg.addvariable('MinEt', 0) # --> MinEt               
        alg.addvariable('IsoMask', 4) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        tm.registerAlgo(alg) 

        alg = AlgConf.MuonSelect( name = 'MUab', inputs = 'MuonTobArray', outputs = 'MUab', algoId = currentAlgoId ); currentAlgoId += 1                                      
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinEt', 4)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 25)
        tm.registerAlgo(alg)
        
        alg = AlgConf.MuonSelect( name = 'CMUab', inputs = 'MuonTobArray', outputs = 'CMUab', algoId = currentAlgoId ); currentAlgoId += 1                                     
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinEt', 4)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 10)
        tm.registerAlgo(alg)

        # All lists:
        alg = AlgConf.ClusterNoSort( name = 'EMall', inputs = 'ClusterTobArray', outputs = 'EMall', algoId = currentAlgoId) ; currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('OutputWidth', HW.InputWidthEM)
        alg.addvariable('IsoMask', 0)
        tm.registerAlgo(alg)  

        alg = AlgConf.ClusterNoSort( name = 'TAUall', inputs = 'ClusterTobArray', outputs = 'TAUall', algoId = currentAlgoId) ; currentAlgoId += 1                                         
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('OutputWidth', HW.InputWidthTAU)
        alg.addvariable('IsoMask', 0)
        tm.registerAlgo(alg)

        alg = AlgConf.JetNoSort( name = 'AJall', inputs = 'JetTobArray', outputs = 'AJall', algoId = currentAlgoId ) ; currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', HW.DefaultJetSize)
        tm.registerAlgo(alg)

        alg = AlgConf.JetNoSort( name = 'AJjall', inputs = 'JetTobArray', outputs = 'AJjall', algoId = currentAlgoId ) ; currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)
        tm.registerAlgo(alg)

        alg = AlgConf.MuonNoSort( name = 'MUall', inputs = 'MuonTobArray', outputs = 'MUall',algoId = currentAlgoId) ; currentAlgoId += 1                                      
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('OutputWidth', HW.InputWidthMU)
        tm.registerAlgo(alg)
                
        # Decision algorithms
        currentAlgoId = 1

        # DPHI    
        for x in [
            {"minDphi": 0, "maxDphi": 16, "otype1" : "AJ", "ocut1" : 0, "olist" : "s", "nleading1" : HW.OutputWidthSortJET, "otype2" : "AJ", "ocut2" : 0, "nleading2" : HW.OutputWidthSortJET},
            {"minDphi": 0, "maxDphi": 16, "otype1" : "EM", "ocut1" : 0, "olist" : "s", "nleading1" : HW.OutputWidthSortEM, "otype2" : "TAU","ocut2" : 0, "nleading2" : HW.OutputWidthSortTAU},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            #toponame = "%iDPHI%i-%s%s%s%s-%s%s%s%s"  % (minDphi, maxDphi, otype1, str(ocut1), olist, str(nleading1), otype2, str(ocut2), olist, str(nleading2))
            toponame = "DPhi_%s%s%s%s"  % (otype1, olist, otype2, olist)                
            log.info("Define %s" % toponame)
            inputList = (otype1+olist) if otype1==otype2 else [otype1 + olist, otype2 + olist]
                        
            algoname = AlgConf.DeltaPhiIncl1 if (otype1==otype2) else AlgConf.DeltaPhiIncl2
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
            if otype1==otype2:
                alg.addgeneric('InputWidth', nleading1) 
            else:
                alg.addgeneric('InputWidth1', nleading1)
                alg.addgeneric('InputWidth2', nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinEt1', ocut1)
            alg.addvariable('MinEt2', ocut2) 
            alg.addvariable('MinDeltaPhi', minDphi, 0)
            alg.addvariable('MaxDeltaPhi', maxDphi, 0)
            tm.registerAlgo(alg)
            

        
        # for M7: DETA20-AJ2ab-AJ2ab, 0DETA20-EM2ab-TAU2ab
        for x in [     
            {"minDeta": 0, "maxDeta": 10, "otype1" : "AJ", "ocut1" : 0, "olist" : "ab", "nleading1" : HW.OutputWidthSelectJET, "otype2" : "AJ", "ocut2" : 0, "nleading2" : HW.OutputWidthSelectJET},
            {"minDeta": 0, "maxDeta": 10, "otype1" : "EM", "ocut1" : 0, "olist" : "ab", "nleading1" : HW.OutputWidthSelectEM, "otype2" : "TAU", "ocut2" : 0, "nleading2" : HW.OutputWidthSelectTAU},
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "DEta_%s%s%s%s"  % (otype1, olist,
                                           otype2, olist)
            
            log.info("Define %s" % toponame)
            
            inputList = (otype1+olist) if otype1==otype2 else [otype1 + olist, otype2 + olist]

            algoname = AlgConf.DeltaEtaIncl1 if (otype1==otype2) else AlgConf.DeltaEtaIncl2
            alg = algoname( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId ); currentAlgoId += 1
            if otype1==otype2:
                alg.addgeneric('InputWidth', nleading1)
            else:
                alg.addgeneric('InputWidth1', nleading1)
                alg.addgeneric('InputWidth2', nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinEt1', str(ocut1))
            alg.addvariable('MinEt2', str(ocut2))
            alg.addvariable('MinDeltaEta', minDeta, 0)
            alg.addvariable('MaxDeltaEta', maxDeta, 0)
            tm.registerAlgo(alg)        

        #HT0-AJ0s.ETAxy
        # (ATR-8194) L1Topo HT Trigger
        for x in [     
            {"minHT": 0,   "otype" : "AJ", "ocut" : 0,  "olist" : "all", "nleading" : HW.InputWidthJET, "oeta" : 49},
            {"minHT": 20,  "otype" : "AJj","ocut" : 0,  "olist" : "all", "nleading" : HW.InputWidthJET, "oeta" : 49},
            {"minHT": 190, "otype" : "AJ", "ocut" : 15, "olist" : "all", "nleading" : HW.InputWidthJET, "oeta" : 20},
            {"minHT": 150, "otype" : "AJ", "ocut" : 20, "olist" : "all", "nleading" : HW.InputWidthJET, "oeta" : 30},
            {"minHT": 150, "otype" : "AJj","ocut" : 15, "olist" : "all", "nleading" : HW.InputWidthJET, "oeta" : 49},
            {"minHT": 190, "otype" : "J", "ocut" : 15, "olist" : "s",   "nleading" : 5, "oeta" : 20},
            {"minHT": 150, "otype" : "J", "ocut" : 20, "olist" : "s",   "nleading" : 5, "oeta" : 30},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "HT%d-%s%s%s%s.ETA%s" % (minHT, otype, str(ocut), olist, str(nleading) if olist=="s" else "", str(oeta))
            
            log.info("Define %s" % toponame)
            
            inputList = otype + olist

            alg = AlgConf.JetHT( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', nleading)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinEt', ocut)                        
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', oeta)            
            alg.addvariable('MinHt', minHT) # OK
            tm.registerAlgo(alg)  

        # (ATR-8192) L1Topo ZH Trigger
        for x in [
            {"minDPhi": 10, "otype" : "J", "ocut" : 0,  "olist" : "s", "nleading" : 2 },
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2 },
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET },
            {"minDPhi": 10, "otype" : "CJ","ocut" : 20, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET },
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%iMINDPHI-%s%s%s%s-XE50"  % (minDPhi, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")
            log.info("Define %s" % toponame)
            
            inputList = otype + olist

            alg = AlgConf.MinDPhiIncl2( name = toponame, inputs = [inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading)
            alg.addgeneric('NumberLeading2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut)
            alg.addvariable('MinET2', 50)
            alg.addvariable('DeltaPhiMin', minDPhi, 0)
            tm.registerAlgo(alg)

        #VBF invm
        for x in [
            {"minInvm": 400, "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 350, "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 300, "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 200, "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 350, "maxInvm": 9999, "otype" : "J",  "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 300, "maxInvm": 9999, "otype" : "J",  "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 250, "maxInvm": 9999, "otype" : "J",  "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minInvm": 200, "maxInvm": 9999, "otype" : "J",  "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6}, 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iINVM%i-%s%s%s%s-%s%s%s%s"  % (minInvm, maxInvm,
                                                        otype, str(ocut1) if ocut1 > 0 else "", olist, str(nleading1) if olist=="s" else "",
                                                        otype, str(ocut2) if ocut2 > 0 else "", olist, str(nleading2) if olist=="s" else "")
                                                        
            log.info("Define %s" % toponame)
            
            inputList = otype + olist

            alg = AlgConf.InvariantMassInclusive1( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading1)
            alg.addgeneric('NumberLeading2', nleading2)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('InvMassMin', minInvm)
            alg.addvariable('InvMassMax', maxInvm)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            tm.registerAlgo(alg)

        # VBF deta     
        for x in [     
            {"minDeta": 40, "maxDeta": 99, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minDeta": 40, "maxDeta": 99, "otype" : "J",  "ocut1" : 30, "olist" : "s", "nleading1" : 6, "ocut2" : 20, "nleading2" : 6},
            {"minDeta": 0,  "maxDeta": 10, "otype" : "J",  "ocut1" : 0,  "olist" : "s", "nleading1" : 1, "ocut2" : 0, "nleading2" : 2},
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iDETA%i-%s%s%s%s-%s%s%s%s"  % (minDeta, maxDeta,
                                                        otype, str(ocut1) if ocut1 > 0 else "", olist, str(nleading1) if olist=="s" else "",
                                                        otype, str(ocut2) if ocut2 > 0 else "", olist, str(nleading2) if olist=="s" else "")
            
            log.info("Define %s" % toponame)

            inputList = otype + olist
            
            alg = AlgConf.DeltaEtaIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', nleading1)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinEt1', str(ocut1))            
            alg.addvariable('MinEt2', str(ocut2))
            alg.addvariable('MinDeltaEta', minDeta, 0)
            alg.addvariable('MaxDeltaEta', maxDeta, 0)
            tm.registerAlgo(alg)
            
        # (ATR-8193) L1Topo Jpsi T&P trigger 
        for x in [
            {"minInvm": 1, "maxInvm": 5, "otype" : "EM", "ocut" : 0, "olist" : "s", "nleading" : 2 },
            {"minInvm": 1, "maxInvm": 5, "otype" : "EM", "ocut" : 7, "olist" : "s", "nleading" : 2 },
            {"minInvm": 1, "maxInvm": 5, "otype" : "EM", "ocut" : 12,"olist" : "s", "nleading" : 2 },
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%iINVM%i-%s%s%s%s-EMall"  % (minInvm, maxInvm, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")
            
            log.info("Define %s" % toponame)
            
            inputList = otype + olist

            alg = AlgConf.InvariantMassInclusive2( name = toponame,  inputs = [inputList, 'EMall'], outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading)
            alg.addgeneric('NumberLeading2', HW.OutputWidthSelectEM)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('InvMassMin', minInvm)
            alg.addvariable('InvMassMax', maxInvm)
            alg.addvariable('MinET1', ocut)
            alg.addvariable('MinET2', 0)
            tm.registerAlgo(alg)

        # added for muon-jet items: '0DR04-MU4ab-CJ15ab', '0DR04-MU4ab-CJ30ab', '0DR04-MU6ab-CJ25ab', '0DR04-MU4ab-CJ17ab', '0DR04-MU4ab-CJ20ab'
        for x in [  
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 30, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 25, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 17, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"} 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (minDr, maxDr, otype1, str(ocut1), olist1, otype2, str(ocut2), olist2)
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2]

            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', HW.OutputWidthSelectMU)
            alg.addgeneric('NumberLeading2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('DeltaRMin', minDr)
            alg.addvariable('DeltaRMax', maxDr)
            alg.addvariable('MinET1', ocut1, 0)
            alg.addvariable('MinET2', ocut2, 0)
            tm.registerAlgo(alg)
            
        # dimu INVM items
        for x in [
            {"minInvm": 2, "maxInvm": 999, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist" : "ab", "otype2" :"", "ocut2" : 0},
            {"minInvm": 2, "maxInvm": 999, "mult": 2, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"", "ocut2" : 0},
            {"minInvm": 2, "maxInvm": 999, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" :"", "ocut2" : 0},
            {"minInvm": 2, "maxInvm": 999, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"MU", "ocut2" : 4},
            {"minInvm": 2, "maxInvm": 999, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" :"MU", "ocut2" : 4},
            {"minInvm": 2, "maxInvm": 999, "mult": 1, "otype1" : "CMU","ocut1": 6, "olist" : "ab", "otype2" :"CMU","ocut2" : 4},

            {"minInvm": 4, "maxInvm": 8, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0},
            {"minInvm": 4, "maxInvm": 8, "mult": 2, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0},
            {"minInvm": 4, "maxInvm": 8, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "",  "ocut2" : 0},
            {"minInvm": 4, "maxInvm": 8, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" : "MU","ocut2" : 4},
            {"minInvm": 4, "maxInvm": 8, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4},
            {"minInvm": 4, "maxInvm": 8, "mult": 1, "otype1" : "CMU","ocut1": 6, "olist" : "ab", "otype2" : "CMU","ocut2": 4},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist)
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)
            toponame = "%iINVM%i-%s%s"  % (minInvm, maxInvm, obj1, "" if mult>1 else obj2)
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist]
            algoname = AlgConf.InvariantMassInclusive1 if (mult>1 or otype1==otype2) else AlgConf.InvariantMassInclusive2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', HW.OutputWidthSelectMU)
            alg.addgeneric('NumberLeading2', HW.OutputWidthSelectMU)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('InvMassMin', minInvm)
            alg.addvariable('InvMassMax', maxInvm)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2 if ocut2>0 else ocut1)
            tm.registerAlgo(alg)
            
        # Missing
        # '2DR15-ONEBARREL10-CMU6ab-MU4ab', # Need DR-ONEBARREL algo
        # 'MULT-BIT{0,1}-CMU{4,6}ab' # Need MULTI-BIT algo
        # '2INVM999-ONEBARREL10-MU4ab-MU6ab', # Need INVM-ONEBARREL algo
        # '4INVM8-ONEBARREL10-MU4ab-MU6ab', # Need INVM-ONEBARREL algo
            
        # dimu DR items
        for x in [  
            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 0}, # Bphys
            {"minDr": 2, "maxDr": 15, "mult": 1, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "MU", "ocut2": 4},
            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 0},
            {"minDr": 2, "maxDr": 15, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4},
            {"minDr": 2, "maxDr": 15, "mult": 1, "otype1" : "CMU","ocut1": 6,  "olist" : "ab", "otype2" : "CMU","ocut2": 4},
            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 0},
            {"minDr": 2, "maxDr": 99, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 0}, # SM Y
            {"minDr": 0, "maxDr": 10, "mult": 1, "otype1" : "MU" ,"ocut1": 10, "olist" : "ab", "otype2" : "MU", "ocut2": 6}, # Exotic LFV 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist)
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)
            toponame = "%iDR%i-%s%s"  % (minDr, maxDr, obj1, "" if mult>1 else obj2)

            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist]
            algoname = AlgConf.DeltaRSqrIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', HW.OutputWidthSelectMU)
            alg.addgeneric('NumberLeading2', HW.OutputWidthSelectMU)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('DeltaRMin', minDr)
            alg.addvariable('DeltaRMax', maxDr)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            tm.registerAlgo(alg)

        # deta-dphi with ab+ab
        # Need L1TopoAlgorithms-00-00-14 or later with MinET1 and MinET2
        for x in [     
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "MU", "ocut2": 4, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 6, "olist2": "", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 4, "olist2": "", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 0, "maxDeta": 35, "minDphi": 0, "maxDphi": 25, "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 0, "maxDeta": 35, "minDphi": 0, "maxDphi": 25, "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 0, "maxDeta": 20, "minDphi": 0, "maxDphi": 20, "mult": 1, "otype1" : "TAU", "ocut1": 20, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist1)
            obj2 = "-%s%s%s" % (otype2, str(ocut2) if ocut2>0 else "", olist2)
            toponame = "%iDETA%i-%iDPHI%i-%s%s"  % (minDeta, maxDeta, minDphi, maxDphi, obj1, "" if mult>1 else obj2)
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1] if (mult>1 or otype1==otype2) else [otype1 + olist1, otype2 + olist2]
            algoname = AlgConf.DeltaRApproxBoxCutIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaRApproxBoxCutIncl2            
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading1)
            alg.addgeneric('NumberLeading2', nleading2)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('DeltaEtaMin', minDeta)
            alg.addvariable('DeltaEtaMax', maxDeta)
            alg.addvariable('DeltaPhiMin', minDphi)
            alg.addvariable('DeltaPhiMax', maxDphi)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            tm.registerAlgo(alg)
            
        # W T&P: MINDPHI(J, XE0), (EM, XE0)
        for x in [
            {"minDPhi": 10, "otype" : "AJj", "ocut" : 15, "olist" : "s", "nleading" : 2},
            {"minDPhi": 20, "otype" : "AJj", "ocut" :  0, "olist" : "s", "nleading" : 6},
            {"minDPhi": 20, "otype" : "AJj", "ocut" : 15, "olist" : "s", "nleading" : 2},
            {"minDPhi": 10, "otype" : "EM",  "ocut" :  6, "olist" : "s", "nleading" : 1},
            {"minDPhi": 20, "otype" : "EM",  "ocut" :  9, "olist" : "s", "nleading" : 6},
            {"minDPhi": 20, "otype" : "EM",  "ocut" :  6, "olist" : "s", "nleading" : 1},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%iMINDPHI-%s%s%s%s-XE0"  % (minDPhi, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")
            log.info("Define %s" % toponame)

            inputList = otype + olist

            alg = AlgConf.MinDPhiIncl2( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading)
            alg.addgeneric('NumberLeading2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut)
            alg.addvariable('MinET2', 0)
            alg.addvariable('DeltaPhiMin', minDPhi, 0)
            tm.registerAlgo(alg)

        # JetMatch
        toponame = "0MATCH-4AJ20.ETA32-4AJj15"
        alg = AlgConf.JetMatch( name = toponame, inputs = [ 'AJall' ], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', 4)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 20) # for 8x8
        alg.addvariable('MinET2', 15) # for 4x4
        alg.addvariable('EtaMin', 0)
        alg.addvariable('EtaMax', 32)
        tm.registerAlgo(alg)
        
        # NoMatch for W T&P
        toponame = "NOT-02MATCH-EM9s1-AJj15all.ETA49"
        alg = AlgConf.NoMatch( name = toponame, inputs = [ 'EMs', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', 1)
        alg.addgeneric('NumberLeading2', HW.InputWidthJET)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 9)
        alg.addvariable('MinET2', 15)
        alg.addvariable('EtaMin1', 0)
        alg.addvariable('EtaMax1', 49)
        alg.addvariable('EtaMin2', 0)
        alg.addvariable('EtaMax2', 49)
        alg.addvariable('DRCut', 2)
        tm.registerAlgo(alg)

        # RATIO SUM for W T&P 
        toponame = "05RATIO-XE0-SUM0-EM9s1-HT0-AJj15all.ETA49"
        alg = AlgConf.RatioSum( name = toponame, inputs = ['XE', 'EMs', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', 1)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 9) 
        alg.addvariable('MinET2', 15)
        alg.addvariable('EtaMin1', 0)
        alg.addvariable('EtaMax1', 49)
        alg.addvariable('EtaMin2', 0)
        alg.addvariable('EtaMax2', 49)
        alg.addvariable('HT', 0, 0)
        alg.addvariable('SUM', 0, 0)
        alg.addvariable('Ratio', 5, 0)
        tm.registerAlgo(alg)

        # RATIO for W T&P
        for x in [
            {"minRatio": 5, "ocut" : 15,"Ratio": "RATIO"},
            {"minRatio": 8, "ocut" : 0, "Ratio": "RATIO"},
            {"minRatio": 40, "ocut" : 15,"Ratio": "RATIO2"},
            {"minRatio": 90, "ocut" : 0, "Ratio": "RATIO2"},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%02d%s-XE0-HT0-AJj%sall.ETA49"  % (minRatio, Ratio, str(ocut))
            log.info("Define %s" % toponame)
            
            alg = AlgConf.Ratio( name = toponame, inputs = ['XE', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ) if Ratio=="RATIO" else AlgConf.Ratio2( name = toponame, inputs = ['XE', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ) 
            currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', 0) 
            alg.addvariable('MinET2', str(ocut))
            alg.addvariable('EtaMin', 0) 
            alg.addvariable('EtaMax', 49)
            alg.addvariable('HT', 0, 0)
            alg.addvariable('Ratio' if Ratio=="RATIO" else 'Ratio2', str(minRatio), 0)
            tm.registerAlgo(alg)

            
        # RATIO MATCH dedicated to Exotic 
        toponame = '210RATIO-0MATCH-TAU30si2-EMall'
        alg = AlgConf.RatioMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', 2)
        alg.addgeneric('NumberLeading2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30)
        alg.addvariable('MinET2',  0)
        alg.addvariable('EtaMin',  0) # should be EtaMin1, 2?
        alg.addvariable('EtaMax', 49) # should be EtaMax1, 2?
        alg.addvariable('DeltaR',  0)
        alg.addvariable('Ratio', 210, 0)
        tm.registerAlgo(alg)        

        # NOT MATCH dedicated to Exotic
        toponame = 'NOT-0MATCH-TAU30si2-EMall'
        alg = AlgConf.NoMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('NumberLeading1', 2)
        alg.addgeneric('NumberLeading2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30)
        alg.addvariable('MinET2', 0)
        alg.addvariable('EtaMin1', 0)
        alg.addvariable('EtaMax1', 49)
        alg.addvariable('EtaMin2', 0)
        alg.addvariable('EtaMax2', 49)
        alg.addvariable('DRCut', 0)
        tm.registerAlgo(alg)        

        for x in [  
            {"minDr": 0, "maxDr": 28, "otype1" : "MU" ,"ocut1": 10, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU},
            {"minDr": 0, "maxDr": 28, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU},
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s" % (otype1, str(ocut1), olist1)
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2)
            toponame = "%iDR%i-%s%s"  % (minDr, maxDr, obj1, obj2)

            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1] if otype1==otype2 else [otype1 + olist1, otype2 + olist2]
            algoname = AlgConf.DeltaRSqrIncl1 if otype1==otype2 else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading1.value)
            alg.addgeneric('NumberLeading2', nleading2.value)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('DeltaRMin', minDr)
            alg.addvariable('DeltaRMax', maxDr)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            tm.registerAlgo(alg)        
        
        # DISAMB 2 lists
        for x in [     
            {"disamb": 1, "otype1" : "TAU", "ocut1": 12, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU, "otype2" : "J", "ocut2": 25, "olist2": "ab", "nleading2": HW.OutputWidthSelectJET},
            {"disamb": 0, "otype1" : "EM",  "ocut1": 15, "olist1" : "abhi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "TAU", "ocut2": 40, "olist2": "ab", "nleading2": HW.OutputWidthSelectTAU},
            {"disamb": 1, "otype1" : "TAU", "ocut1": 20, "olist1" : "ab", "nleading1": HW.OutputWidthSelectTAU,  "otype2" : "J", "ocut2": 20, "olist2": "ab", "nleading2": HW.OutputWidthSelectJET},
            {"disamb": 0, "otype1" : "EM",  "ocut1": 15, "olist1" : "abhi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s"  % (otype1, str(ocut1), olist1)
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2)
            toponame = "%sDISAMB-%s%s"  % ( disamb if disamb>0 else "", obj1, obj2)
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2]
            alg = AlgConf.DisambiguationIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading1)
            alg.addgeneric('NumberLeading2', nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            alg.addvariable('EtaMin1', 0)
            alg.addvariable('EtaMax1', 49)
            alg.addvariable('EtaMin2', 0)
            alg.addvariable('EtaMax2', 49)
            alg.addvariable('DisambDR', disamb, 0)
            tm.registerAlgo(alg)
        
        # DISAMB 3 lists
        for x in [     
            {"disamb": 1, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab"},
            {"disamb": 1, "otype1" : "TAU", "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab"}, 
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s"  % (otype1, str(ocut1), olist1.replace('shi','his') + (str(nleading1) if olist1.find('s')>=0 else ""))
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2.replace('shi','his') + (str(nleading2) if olist2.find('s')>=0 else ""))
            obj3 = "-%s%s%s" % (otype3, str(ocut3), olist3)
            toponame = "%sDISAMB-%s%s%s"  % ( disamb if disamb>0 else "", obj1, obj2, obj3)
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2, otype3 + olist3]
            alg = AlgConf.DisambiguationIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading1)
            alg.addgeneric('NumberLeading2', nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            alg.addvariable('MinET3', ocut3)
            alg.addvariable('EtaMin1', 0)
            alg.addvariable('EtaMax1', 49)
            alg.addvariable('EtaMin2', 0)
            alg.addvariable('EtaMax2', 49)
            alg.addvariable('EtaMin3', 0)
            alg.addvariable('EtaMax3', 49)
            alg.addvariable('DisambDR', disamb, 0)
            tm.registerAlgo(alg)

        # DISAMB 3 lists with DR cut to 2nd and 3rd lists
        for x in [     
            {"disamb": 1, "otype1" : "J", "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "drcutmin": 0, "drcutmax": 28},
            {"disamb": 1, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "drcutmin": 0, "drcutmax": 28}, 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "-%s%s%s"  % (otype1, str(ocut1), olist1.replace('shi','his') + (str(nleading1) if olist1.find('s')>=0 else ""))
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2.replace('shi','his') + (str(nleading2) if olist2.find('s')>=0 else ""))
            obj3 = "%s%s%s" % (otype3, str(ocut3), olist3)
            toponame = "%sDISAMB-%s-%dDR%d%s%s"  % ( str(disamb) if disamb>0 else "", obj3, drcutmin, drcutmax, obj1, obj2)

            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2, otype3 + olist3]
            alg = AlgConf.DisambiguationDRIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', nleading1)
            alg.addgeneric('NumberLeading2', nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)
            alg.addvariable('MinET2', ocut2)
            alg.addvariable('MinET3', ocut3)
            alg.addvariable('EtaMin1', 0)
            alg.addvariable('EtaMax1', 49)
            alg.addvariable('EtaMin2', 0)
            alg.addvariable('EtaMax2', 49)
            alg.addvariable('EtaMin3', 0)
            alg.addvariable('EtaMax3', 49)
            alg.addvariable('DRCutMin', drcutmin)
            alg.addvariable('DRCutMax', drcutmax)
            alg.addvariable('DisambDR', disamb, 0)
            tm.registerAlgo(alg)            

        # LAR  0<eta<1.4 and 9/16pi<phi<11/16pi for FE crate IO6 
        for x in [     
            {"minEta": 0, "maxEta": 14, "minPhi": 17, "maxPhi": 22, "otype" : "EM", "ocut" : 50},
            {"minEta": 0, "maxEta": 14, "minPhi": 17, "maxPhi": 22, "otype" : "J", "ocut" : 100}, 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "LAR-%s%ss1"  % ( otype, str(ocut))
            
            log.info("Define %s" % toponame)
            
            inputList = otype + 's'
            
            alg = AlgConf.LAR( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('NumberLeading1', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(ocut))
            alg.addvariable('EtaMin', minEta)
            alg.addvariable('EtaMax', maxEta)
            alg.addvariable('PhiMin', minPhi)
            alg.addvariable('PhiMax', maxPhi)
            tm.registerAlgo(alg)

