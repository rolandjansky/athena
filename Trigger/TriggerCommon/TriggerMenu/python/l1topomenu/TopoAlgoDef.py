# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenu.L1Topo.py')

#from TriggerMenu.l1topo.TopoAlgos import SortingAlgo, DecisionAlgo
from TriggerJobOpts.TriggerFlags import TriggerFlags

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW

#import re


class TopoAlgoDef:

    @staticmethod
    def registerTopoAlgos(tm):
        currentAlgoId = 0

        _etamax = 49
        _minet = 0
        usev7 = False

        if '_v7' in TriggerFlags.triggerMenuSetup():
            usev7 = True
        if '_PhaseII' in TriggerFlags.triggerMenuSetup():
            usev7 = True
        
        _emscale_for_decision = 2 # global scale for EM, TAU        
        if hasattr(TriggerFlags, 'useRun1CaloEnergyScale'):
            if TriggerFlags.useRun1CaloEnergyScale :
                _emscale_for_decision=1     
                log.info("Changed mscale_for_decision %s for Run1CaloEnergyScale" % _emscale_for_decision)

        alg = AlgConf.ClusterNoSort( name = 'EMall', inputs = 'ClusterTobArray', outputs = 'EMall', algoId = currentAlgoId) ; currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('OutputWidth', HW.InputWidthEM)
        alg.addvariable('IsoMask', 0)
        tm.registerAlgo(alg)  
                                
        alg = AlgConf.ClusterSelect( name = 'TAUabi', inputs = 'ClusterTobArray', outputs = 'TAUabi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectTAU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)        
        alg.addvariable('MinET', 12) 
        alg.addvariable('IsoMask', 2) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 1)
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSelect( name = 'EMabi', inputs = 'ClusterTobArray', outputs = 'EMabi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET', 8)
        alg.addvariable('IsoMask', 2)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 1)
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSelect( name = 'EMabhi', inputs = 'ClusterTobArray', outputs = 'EMabhi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET', 8)
        alg.addvariable('IsoMask', 3)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 1)
        tm.registerAlgo(alg) 
        
        alg = AlgConf.ClusterSelect( name = 'TAUab', inputs = 'ClusterTobArray', outputs = 'TAUab', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectTAU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)
        alg.addvariable('MinET', 12) 
        alg.addvariable('IsoMask', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 0)
        tm.registerAlgo(alg) 

        alg = AlgConf.ClusterSort( name = 'EMs', inputs = 'ClusterTobArray', outputs = 'EMs', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('IsoMask', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax) 
        alg.addgeneric('DoIsoCut', '0')
        #alg.addgeneric('DoEtaCut', '1')
        tm.registerAlgo(alg) 
        
        alg = AlgConf.ClusterSort( name = 'EMshi', inputs = 'ClusterTobArray', outputs = 'EMshi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('IsoMask', 3) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', '1')
        #alg.addgeneric('DoEtaCut', '1')
        tm.registerAlgo(alg)
                
        alg = AlgConf.ClusterSort( name = 'TAUsi', inputs = 'ClusterTobArray', outputs = 'TAUsi', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortTAU)
        alg.addvariable('IsoMask', 2) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', '1')
        #alg.addgeneric('DoEtaCut', '1')
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

        # for 0MATCH-4AJ20-4AJj15
        alg = AlgConf.JetNoSortMatch( name = 'AJMatchall', inputs = 'JetTobArray', outputs = 'AJMatchall', algoId = currentAlgoId ) ; currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', 2 if HW.DefaultJetSize.value==2 else 1)
        alg.addvariable('MinET1', 15) # 4x4       
        alg.addvariable('MinET2', 20) # 8x8
        tm.registerAlgo(alg)        


        # ab J lists:
        for jet_type in ['J', 'CJ']:
            jetabseta = _etamax
            _minet = 25
            if jet_type=='J':
                jetabseta = 31
                _minet = 20
            elif jet_type=='CJ':
                jetabseta = 26 
                _minet = 15
            
            alg = AlgConf.JetSelect( name = jet_type+'ab', inputs = 'JetTobArray', outputs = jet_type+'ab', algoId = currentAlgoId ); currentAlgoId += 1

            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectJET )            
            alg.addgeneric('OutputWidth', HW.OutputWidthSelectJET)
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinET', _minet)  
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', jetabseta)
            alg.addgeneric('DoEtaCut', 1)
            tm.registerAlgo(alg) 

        alg = AlgConf.JetSort( name = 'AJjs', inputs = 'JetTobArray', outputs = 'AJjs', algoId = currentAlgoId); currentAlgoId += 1

        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoEtaCut', 0)
        tm.registerAlgo(alg) 
        
        # Sorted J lists:
        for jet_type in ['AJ']:
            jetabseta = _etamax
            _minet = 25
            if jet_type=='J':
                jetabseta = 31
                _minet = 20
            elif jet_type=='CJ':
                jetabseta = 26 
                _minet = 15
                
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s', algoId = currentAlgoId ); currentAlgoId += 1

            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addgeneric('JetSize', HW.DefaultJetSize.value) 
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', jetabseta)
            alg.addgeneric('DoEtaCut', 0)
            tm.registerAlgo(alg) 

        for jet_type in ['J']:
            jetabseta = _etamax
            _minet = 25
            if jet_type=='J':
                jetabseta = 31
                _minet = 20
            elif jet_type=='CJ':
                jetabseta = 26
                _minet = 15

            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s', algoId = currentAlgoId ); currentAlgoId += 1

            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', jetabseta)
            alg.addgeneric('DoEtaCut', 1)
            tm.registerAlgo(alg)

        alg = AlgConf.METNoSort( name = 'XENoSort', inputs = 'MetTobArray', outputs = 'XENoSort', algoId = currentAlgoId ); currentAlgoId += 1

        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerAlgo(alg)
                
        alg = AlgConf.MetSort( name = 'XE', inputs = 'MetTobArray', outputs = 'XE', algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerAlgo(alg)
        
        alg = AlgConf.MuonSelect( name = 'MUab', inputs = 'MuonTobArray', outputs = 'MUab', algoId = currentAlgoId ); currentAlgoId += 1                                      

        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinET', 4) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 25)
        tm.registerAlgo(alg)            

        alg = AlgConf.MuonSelect( name = 'CMUab', inputs = 'MuonTobArray', outputs = 'CMUab', algoId = currentAlgoId ); currentAlgoId += 1                                     

        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinET', 4) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 10)
        tm.registerAlgo(alg)

        alg = AlgConf.MuonSort_1BC( name = 'LMUs', inputs = 'LateMuonTobArray', outputs = 'LMUs', algoId = currentAlgoId ); currentAlgoId += 1
        #alg = AlgConf.MuonSort_1BC( name = 'LMUs', inputs = 'MuonTobArray', outputs = 'LMUs', algoId = currentAlgoId ); currentAlgoId += 1

        alg.addgeneric('InputWidth', HW.InputWidthMU)
        #alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortMU)
        alg.addgeneric('nDelayedMuons', 1)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 25)
        tm.registerAlgo(alg)
        
        # Abbreviated lists:
        alg = AlgConf.ClusterSelect( name = 'EMab', inputs = 'ClusterTobArray', outputs = 'EMab', algoId = currentAlgoId ); currentAlgoId += 1

        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET', 8)
        alg.addvariable('IsoMask', 0)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        #alg.addgeneric('DoIsoCut', 1)
        tm.registerAlgo(alg) 
        
        # All lists:

        alg = AlgConf.ClusterNoSort( name = 'TAUall', inputs = 'ClusterTobArray', outputs = 'TAUall', algoId = currentAlgoId) ; currentAlgoId += 1                                         

        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('OutputWidth', HW.InputWidthTAU)
        alg.addvariable('IsoMask', 0)
        tm.registerAlgo(alg)

        alg = AlgConf.MuonNoSort( name = 'MUall', inputs = 'MuonTobArray', outputs = 'MUall',algoId = currentAlgoId) ; currentAlgoId += 1                                      

        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('OutputWidth', HW.InputWidthMU)
        tm.registerAlgo(alg)
                
        # Decision algorithms
        currentAlgoId = 0
        
        # VBF items    
        for x in [
            {"algoname": 'INVM_AJ_HighMass', "Threlist": [ 900, 800, 700, 500 ], "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6},
            {"algoname": 'INVM_AJ_LowMass',  "Threlist": [ 400, 300, 200, 100 ], "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
		
            inputList = otype + olist # noqa: F821
            toponames=[]

            for minInvm in Threlist:  # noqa: F821
                toponame = "%iINVM%i-%s%s%s%s-%s%s%s%s"  % (minInvm, maxInvm,                                                    # noqa: F821
                                                            otype, str(ocut1) , olist, str(nleading1) if olist=="s" else "",     # noqa: F821
                                                            otype, str(ocut2) , olist, str(nleading2) if olist=="s" else "")     # noqa: F821 
                toponames.append(toponame)
                
            alg = AlgConf.InvariantMassInclusive1( name = algoname, inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1  # noqa: F821

            alg.addgeneric('InputWidth', inputwidth1)      # noqa: F821
            alg.addgeneric('MaxTob', nleading1)            # noqa: F821
            alg.addgeneric('NumResultBits', len(toponames))

            for bitid, minInvm in enumerate(Threlist):     # noqa: F821
                alg.addvariable('MinET1', ocut1, bitid)    # noqa: F821
                alg.addvariable('MinET2', ocut2, bitid)    # noqa: F821
                alg.addvariable('MinMSqr', minInvm * minInvm, bitid)  # noqa: F821
                alg.addvariable('MaxMSqr', maxInvm * maxInvm, bitid)  # noqa: F821

            tm.registerAlgo(alg)


        # dimu INVM items

        listofalgos=[

            #{"minInvm": 1, "maxInvm": 19, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0},#1INVM19-2MU4ab 
            {"minInvm": 2, "maxInvm": 8, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0}, #2INVM8-2MU4ab 
            {"minInvm": 2, "maxInvm": 9, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0}, #2INVM9-2MU4ab 
            {"minInvm": 8, "maxInvm": 15, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 0}, #8INVM15-MU6ab-MU4ab
            {"minInvm": 2, "maxInvm": 8, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 0}, #2INVM8-MU6ab-MU4ab
            {"minInvm": 2, "maxInvm": 9, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 0}, #2INVM9-MU6ab-MU4ab
            {"minInvm": 8, "maxInvm": 15, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0}, #8INVM15-2MU6ab
            {"minInvm": 2, "maxInvm": 9, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0},  #2INVM9-2MU6ab 
            {"minInvm": 7, "maxInvm": 15, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0}, #7INVM15-2MU4ab 

            ]

        for x in listofalgos:
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist)   # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)                                   # noqa: F821
            toponame = "%iINVM%i-%s%s%s"  % (minInvm, maxInvm, "ONEBARREL-" if onebarrel==1 else "", obj1, "" if mult>1 else obj2)  # noqa: F821

            log.info("Define %s" % toponame)


            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist]                             # noqa: F821
            algoname = AlgConf.InvariantMassInclusive1 if (mult>1 or otype1==otype2) else AlgConf.InvariantMassInclusive2                # noqa: F821
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1

            if (mult>1 or otype1==otype2):  # noqa: F821
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', onebarrel) # noqa: F821
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)

            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)                           # noqa: F821
            alg.addvariable('MinET2', ocut2 if ocut2>0 else ocut1)     # noqa: F821
            alg.addvariable('MinMSqr', minInvm * minInvm)              # noqa: F821
            alg.addvariable('MaxMSqr', maxInvm * maxInvm)              # noqa: F821
            tm.registerAlgo(alg)


        # dimu DR items
        listofalgos=[  
            {"minDr": 2, "maxDr": 99, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, # SM Y  x
            {"minDr": 0, "maxDr": 10, "mult": 1, "otype1" : "MU" ,"ocut1": 10, "olist" : "ab", "otype2" : "MU", "ocut2": 6, "onebarrel": 0}, # Exotic LFV x 
            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0},   #x
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR15-2MU4ab
            {"minDr": 0, "maxDr": 15, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0}, #0DR15-MU6ab-MU4ab
            {"minDr": 0, "maxDr": 34, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR34-2MU4ab 
            {"minDr": 0, "maxDr": 24, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR24-2MU4ab 
            {"minDr": 0, "maxDr": 22, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR22-2MU6ab
            {"minDr": 0, "maxDr": 22, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0}, #0DR22-MU6ab-MU4ab
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR15-2MU6ab  
            ]

        for x in listofalgos:
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist)    # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)                                    # noqa: F821
            toponame = "%iDR%i-%s%s%s"  % (minDr, maxDr, "ONEBARREL-" if onebarrel==1 else "", obj1, "" if mult>1 else obj2)  # noqa: F821

            log.info("Define %s" % toponame)

            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist]    # noqa: F821
            algoname = AlgConf.DeltaRSqrIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaRSqrIncl2         # noqa: F821
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
 
            if (mult>1 or otype1==otype2):  # noqa: F821
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', onebarrel)  # noqa: F821
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)


            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)            # noqa: F821
            alg.addvariable('MinET2', ocut2)            # noqa: F821
            alg.addvariable('DeltaRMin', minDr*minDr)   # noqa: F821
            alg.addvariable('DeltaRMax', maxDr*maxDr)   # noqa: F821
            tm.registerAlgo(alg)
        

            
        # deta-dphi with ab+ab
        for x in [     
            #{"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 4, "olist2": "", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "MU", "ocut2": 4, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 6, "olist2": "", "nleading2": HW.OutputWidthSelectMU},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist1)  # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2) if ocut2>0 else "", olist2)  # noqa: F821
            toponame = "%sDETA%s-%sDPHI%s-%s%s"  % (minDeta, maxDeta, minDphi, maxDphi, obj1, "" if mult>1 else obj2)  # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1] if (mult>1 or otype1==otype2) else [otype1 + olist1, otype2 + olist2]    # noqa: F821
            algoname = AlgConf.DeltaEtaPhiIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaEtaPhiIncl2        # noqa: F821     
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)                        

            
            if (mult>1 or otype1==otype2):  # noqa: F821
                alg.addgeneric('InputWidth', nleading1)  # noqa: F821
                alg.addgeneric('MaxTob', nleading1)      # noqa: F821
                alg.addvariable('MinET1', ocut1)         # noqa: F821
                alg.addvariable('MinET2', ocut2)         # noqa: F821
                alg.addvariable('MinDeltaEta', minDeta)  # noqa: F821
                alg.addvariable('MaxDeltaEta', maxDeta)  # noqa: F821
                alg.addvariable('MinDeltaPhi', minDphi)  # noqa: F821
                alg.addvariable('MaxDeltaPhi', maxDphi)  # noqa: F821
            else:
                alg.addgeneric('InputWidth1', nleading1) # noqa: F821
                alg.addgeneric('InputWidth2', nleading2) # noqa: F821
                alg.addgeneric('MaxTob1', nleading1)     # noqa: F821
                alg.addgeneric('MaxTob2', nleading2)     # noqa: F821
                alg.addvariable('DeltaEtaMin', minDeta)  # noqa: F821
                alg.addvariable('DeltaEtaMax', maxDeta)  # noqa: F821
                alg.addvariable('DeltaPhiMin', minDphi)  # noqa: F821
                alg.addvariable('DeltaPhiMax', maxDphi)  # noqa: F821
                alg.addvariable('MinET1', ocut1)         # noqa: F821
                alg.addvariable('MinET2', ocut2)         # noqa: F821
            
            tm.registerAlgo(alg)



        algolist=[
               {"minDr": 0, "maxDr": 28, "otype1" : "MU" ,"ocut1": 10, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU, "otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU},   # 0DR28-MU10ab-TAU12abi
               {"minDr": 0, "maxDr": 28, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,"otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # 0DR28-TAU20abi-TAU12abi
               {"minDr": 0, "maxDr": 25, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,"otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, #	0DR25-TAU20abi-TAU12abi
            ] 


        for x in algolist:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s" % (otype1, str(ocut1), olist1)         # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2)        # noqa: F821
            toponame = "%iDR%i-%s%s"  % (minDr, maxDr, obj1, obj2) # noqa: F821

            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1] if otype1==otype2 else [otype1 + olist1, otype2 + olist2]  # noqa: F821
            algoname = AlgConf.DeltaRSqrIncl1 if otype1==otype2 else AlgConf.DeltaRSqrIncl2          # noqa: F821
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1

                            
            if otype1==otype2: # noqa: F821
                alg.addgeneric('InputWidth', inputwidth1)   # noqa: F821
                alg.addgeneric('MaxTob', nleading1)         # noqa: F821
            else:
                alg.addgeneric('InputWidth1', inputwidth1)  # noqa: F821
                alg.addgeneric('InputWidth2', inputwidth2)  # noqa: F821
                alg.addgeneric('MaxTob1', nleading1)        # noqa: F821
                alg.addgeneric('MaxTob2', nleading2)        # noqa: F821

            alg.addgeneric('NumResultBits', 1)

            if otype1==otype2:  # noqa: F821
                alg.addvariable('MinET1', ocut1) # noqa: F821
                alg.addvariable('MinET2', ocut2) # noqa: F821
                alg.addvariable('DeltaRMin', minDr*minDr) # noqa: F821
                alg.addvariable('DeltaRMax', maxDr*maxDr) # noqa: F821
            else:
                alg.addvariable('MinET1', ocut1, 0) # noqa: F821
                alg.addvariable('MinET2', ocut2, 0) # noqa: F821
                alg.addvariable('DeltaRMin', minDr*minDr, 0) # noqa: F821
                alg.addvariable('DeltaRMax', maxDr*maxDr, 0) # noqa: F821
                
            tm.registerAlgo(alg)        

            
        # (ATR-8194) L1Topo HT Trigger
        for x in [            
            {"minHT": 150, "otype" : "J", "ocut" : 20, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 31},
            {"minHT": 190, "otype" : "J", "ocut" : 15, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 21},
            {"minHT": 190, "otype" : "AJ", "ocut" : 15, "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 21},
            {"minHT": 150, "otype" : "AJ", "ocut" : 20, "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 31},
            {"minHT": 150, "otype" : "AJj","ocut" : 15, "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 49},
            {"minHT": 20,  "otype" : "AJj","ocut" : 15,  "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 49},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "HT%d-%s%s%s%s.ETA%s" % (minHT, otype, str(ocut), olist, str(nleading) if olist=="s" else "", str(oeta)) # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = otype + olist # noqa: F821

            alg = AlgConf.JetHT( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId ); currentAlgoId += 1


            alg.addgeneric('InputWidth', inputwidth)   # noqa: F821
            alg.addgeneric('MaxTob', nleading)         # noqa: F821
            alg.addgeneric('NumRegisters', 2 if olist=="all" else 0) # noqa: F821
            alg.addgeneric('NumResultBits', 1) # noqa: F821
            alg.addvariable('MinET', ocut)     # noqa: F821                    
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', oeta)    # noqa: F821
            alg.addvariable('MinHt', minHT)    # noqa: F821
            tm.registerAlgo(alg)  

        # INVM_EM for Jpsi    
        invm_map = {"algoname": 'INVM_EMs6' , "ocutlist": [ 0, 7, 12 ], "minInvm": 1, "maxInvm": 5, "otype" : "EM", "olist" : "s", "nleading" : 1, "inputwidth": HW.OutputWidthSortEM}
        for x in [ invm_map,
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            inputList = otype + olist  # noqa: F821
            toponames=[]

            for ocut in ocutlist:      # noqa: F821
                toponame = "%iINVM%i-%s%s%s%s-EMs6"   % (minInvm, maxInvm, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")          # noqa: F821
                toponames.append(toponame)

            alg = AlgConf.InvariantMassInclusive2( name = algoname, inputs = [inputList, 'EMs'], outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1     # noqa: F821


            alg.addgeneric('InputWidth1', inputwidth) # noqa: F821
            #alg.addgeneric('InputWidth2', HW.InputWidthEM)
            alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob1', nleading)       # noqa: F821
            #alg.addgeneric('MaxTob2', HW.InputWidthEM)
            alg.addgeneric('MaxTob2', HW.OutputWidthSortEM)
            alg.addgeneric('NumResultBits', len(toponames))

            for bitid, ocut in enumerate(ocutlist): # noqa: F821
                alg.addvariable('MinET1', ocut, bitid)
                alg.addvariable('MinET2', 0, bitid)
                alg.addvariable('MinMSqr', (minInvm * _emscale_for_decision)*(minInvm * _emscale_for_decision), bitid)        # noqa: F821         
                alg.addvariable('MaxMSqr', (maxInvm * _emscale_for_decision)*(maxInvm * _emscale_for_decision), bitid)        # noqa: F821
                
            tm.registerAlgo(alg)

        # W T&P: MINDPHI(J, XE0), (EM, XE0)


        alglist = [
            {"minDPhi":  5, "otype" : "AJj", "ocut" : 10, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortJET},
            {"minDPhi": 10, "otype" : "AJj", "ocut" : 10, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortJET},
            {"minDPhi": 15, "otype" : "AJj", "ocut" : 10, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortJET},
            {"minDPhi": 05, "otype" : "EM",  "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},#new
            #{"minDPhi": 10, "otype" : "EM",  "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},#new
            {"minDPhi": 05, "otype" : "EM",  "ocut" : 15, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},#same
            ]

        for x in alglist:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%02dMINDPHI-%s%s%s%s-XE0"  % (minDPhi, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")  # noqa: F821
            log.info("Define %s" % toponame)

            inputList = otype + olist  # noqa: F821

            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1


            alg.addgeneric('InputWidth1', inputwidth)  # noqa: F821
            alg.addgeneric('InputWidth2', 1) 
            alg.addgeneric('MaxTob1', nleading)        # noqa: F821
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut)            # noqa: F821
            alg.addvariable('MinET2', 0)
            alg.addvariable('DeltaPhiMin', minDPhi, 0) # noqa: F821
            tm.registerAlgo(alg)

        # W T&P MT

        alglistmt = [
            {"minMT": 25, "otype" : "EM", "ocut" : "12", "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            #{"minMT": 30, "otype" : "EM", "ocut" : "12", "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            {"minMT": 35, "otype" : "EM", "ocut" : "15", "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            ]
        for x in alglistmt:
            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iMT-%s%s%s%s-XE0"  % (minMT, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "") # noqa: F821
            log.info("Define %s" % toponame)

            inputList = otype + olist  # noqa: F821
            
            alg = AlgConf.TransverseMassInclusive1( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1



            alg.addgeneric('InputWidth', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob', str(nleading))  # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', str(ocut))     # noqa: F821
            alg.addvariable('MinET2', 0)
            alg.addvariable('MinMTSqr', minMT*minMT) # noqa: F821                  
            tm.registerAlgo(alg)

            
        # VBF deta     
        for x in [
            #{"minDeta": 0,  "maxDeta": 10, "otype" : "J",  "ocut1" : 0,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 0, "nleading2": 2},
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iDETA%i-%s%s%s%s-%s%s%s%s"  % (minDeta, maxDeta,  # noqa: F821
                                                        otype, str(ocut1) if ocut1 > 0 else "", olist, str(nleading1) if olist=="s" else "", # noqa: F821
                                                        otype, str(ocut2) if ocut2 > 0 else "", olist, str(nleading2) if olist=="s" else "") # noqa: F821
            
            log.info("Define %s" % toponame)
            inputList = otype + olist # noqa: F821
            
            alg = AlgConf.DeltaEtaIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1



            alg.addgeneric('InputWidth', inputwidth1)  # noqa: F821
            alg.addgeneric('MaxTob', nleading2)        # noqa: F821
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', str(ocut1), 0)   # noqa: F821
            alg.addvariable('MinET2', str(ocut2), 0)   # noqa: F821
            alg.addvariable('MinDeltaEta', minDeta, 0) # noqa: F821
            alg.addvariable('MaxDeltaEta', maxDeta, 0) # noqa: F821
            tm.registerAlgo(alg)
            
        # ZH Trigger

        supportedalgolist = [
               #{"minDPhi": 10, "otype" : "J", "ocut" : 0,  "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 50 },
               {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 50 },
               {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "ocut2": 50},
               {"minDPhi": 10, "otype" : "CJ","ocut" : 20, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "ocut2": 50},
               {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 30 }, # NEW
            ]

        for x in supportedalgolist:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%iMINDPHI-%s%s%s%s-XE%i"  % (minDPhi, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "",ocut2) # noqa: F821
            log.info("Define %s" % toponame)
            
            inputList = otype + olist # noqa: F821

            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1

            alg.addgeneric('InputWidth1', inputwidth) # noqa: F821
            alg.addgeneric('InputWidth2', 1)  
            alg.addgeneric('MaxTob1', nleading)       # noqa: F821
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut)           # noqa: F821
            alg.addvariable('MinET2', ocut2)          # noqa: F821
            alg.addvariable('DeltaPhiMin', minDPhi, 0)# noqa: F821
            tm.registerAlgo(alg)

            
        # added for muon-jet:
        for x in [  
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 30, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 25, "olist2" : "ab"},
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"} 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (minDr, maxDr, otype1, str(ocut1), olist1, otype2, str(ocut2), olist2) # noqa: F821
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2] # noqa: F821

            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1

            alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', ocut2, 0) # noqa: F821
            alg.addvariable('DeltaRMin', minDr*minDr, 0) # noqa: F821
            alg.addvariable('DeltaRMax', maxDr*maxDr, 0) # noqa: F821
            tm.registerAlgo(alg)
            
        # dimu INVM items

        algolist=[
                #            {"minInvm": 2, "maxInvm": 999, "mult": 2, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"", "ocut2" : 0, "onebarrel": 0},
                #            {"minInvm": 2, "maxInvm": 999, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"MU", "ocut2" : 4, "onebarrel": 0},
                #            {"minInvm": 2, "maxInvm": 999, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" :"MU", "ocut2" : 4, "onebarrel": 1},
                #            {"minInvm": 2, "maxInvm": 999, "mult": 1, "otype1" : "CMU","ocut1": 6, "olist" : "ab", "otype2" :"CMU","ocut2" : 4, "onebarrel": 0},


                #            {"minInvm": 4, "maxInvm": 8, "mult": 2, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" : "",  "ocut2" : 0, "onebarrel": 0},
                #            {"minInvm": 4, "maxInvm": 8, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 0},
                #            {"minInvm": 4, "maxInvm": 8, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 1},
                #            {"minInvm": 4, "maxInvm": 8, "mult": 1, "otype1" : "CMU","ocut1": 6, "olist" : "ab", "otype2" : "CMU","ocut2": 4, "onebarrel": 0},
                #SX
                #{"minInvm": 1, "maxInvm": 19, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"MU", "ocut2" : 4, "onebarrel": 0}, #1INVM19-CMU4ab-MU4ab
                {"minInvm": 2, "maxInvm": 8, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"MU", "ocut2" : 4, "onebarrel": 0}, #2INVM8-CMU4ab-MU4ab
                #{"minInvm": 1, "maxInvm": 19, "mult": 2, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"", "ocut2" : 0, "onebarrel": 0}, #1INVM19-2CMU4ab
                #{"minInvm": 2, "maxInvm": 8, "mult": 2, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"", "ocut2" : 0, "onebarrel": 0}, #2INVM8-2CMU4ab
                {"minInvm": 2, "maxInvm": 8, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 1}, #2INVM8-ONEBARREL-MU6ab-MU4ab
            ]

        for x in algolist:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist) # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)                                 # noqa: F821
            toponame = "%iINVM%i-%s%s%s"  % (minInvm, maxInvm, "ONEBARREL-" if onebarrel==1 else "", obj1, "" if mult>1 else obj2) # noqa: F821

            log.info("Define %s" % toponame)


            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist]              # noqa: F821
            algoname = AlgConf.InvariantMassInclusive1 if (mult>1 or otype1==otype2) else AlgConf.InvariantMassInclusive2 # noqa: F821
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            if (mult>1 or otype1==otype2): # noqa: F821
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', onebarrel)  # noqa: F821
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)

            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1)                       # noqa: F821
            alg.addvariable('MinET2', ocut2 if ocut2>0 else ocut1) # noqa: F821
            alg.addvariable('MinMSqr', minInvm * minInvm)          # noqa: F821
            alg.addvariable('MaxMSqr', maxInvm * maxInvm)          # noqa: F821
            tm.registerAlgo(alg)

        # dimu DR items
        algolist=[#            {"minDr": 2, "maxDr": 15, "mult": 1, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0},
                #            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0},
                #            {"minDr": 2, "maxDr": 15, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU","ocut2": 4, "onebarrel": 1},            
                #            {"minDr": 2, "maxDr": 15, "mult": 1, "otype1" : "CMU","ocut1": 6,  "olist" : "ab", "otype2" : "CMU","ocut2": 4, "onebarrel": 0},

                #SX
                {"minDr": 0, "maxDr": 24, "mult": 2, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR24-2CMU4ab
                {"minDr": 0, "maxDr": 24, "mult": 1, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "MU","ocut2": 4, "onebarrel": 0}, #0DR24-CMU4ab-MU4ab  
            ]

        for x in algolist : 
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist) # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist)                                 # noqa: F821
            toponame = "%iDR%i-%s%s%s"  % (minDr, maxDr, "ONEBARREL-" if onebarrel==1 else "", obj1, "" if mult>1 else obj2) # noqa: F821

            log.info("Define %s" % toponame)

            inputList = [otype1 + olist] if (mult>1 or otype1==otype2) else [otype1 + olist, otype2 + olist] # noqa: F821
            algoname = AlgConf.DeltaRSqrIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaRSqrIncl2      # noqa: F821
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            if (mult>1 or otype1==otype2): # noqa: F821
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', onebarrel) # noqa: F821
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)


            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1) # noqa: F821
            alg.addvariable('MinET2', ocut2) # noqa: F821
            alg.addvariable('DeltaRMin', minDr*minDr) # noqa: F821
            alg.addvariable('DeltaRMax', maxDr*maxDr) # noqa: F821
            tm.registerAlgo(alg)


        # deta-dphi with ab+ab
        if usev7:
            algoList = [
               {"minDeta": 0, "maxDeta": 20, "minDphi": 0, "maxDphi": 20, "mult": 1, "otype1" : "TAU", "ocut1": 20, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU},
            ]
        else:
            algoList = [
               {"minDeta": 0, "maxDeta": "04", "minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
               {"minDeta": 0, "maxDeta": "04", "minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
               {"minDeta": 0, "maxDeta": 20, "minDphi": 0, "maxDphi": 20, "mult": 1, "otype1" : "TAU", "ocut1": 20, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU},
            ]

        for x in algoList:                 
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s%s" % ((str(mult) if mult>1 else ""), otype1, str(ocut1), olist1) # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2) if ocut2>0 else "", olist2) # noqa: F821
            toponame = "%sDETA%s-%sDPHI%s-%s%s"  % (minDeta, maxDeta, minDphi, maxDphi, obj1, "" if mult>1 else obj2) # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1] if (mult>1 or otype1==otype2) else [otype1 + olist1, otype2 + olist2] # noqa: F821
            algoname = AlgConf.DeltaEtaPhiIncl1 if (mult>1 or otype1==otype2) else AlgConf.DeltaEtaPhiIncl2 # noqa: F821
            
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)                        
            
            if (mult>1 or otype1==otype2):                 # noqa: F821
                alg.addgeneric('InputWidth', nleading1)    # noqa: F821
                alg.addgeneric('MaxTob', nleading1)        # noqa: F821
                alg.addvariable('MinET1', ocut1, 0)        # noqa: F821
                alg.addvariable('MinET2', ocut2, 0)        # noqa: F821
                alg.addvariable('MinDeltaEta', minDeta, 0) # noqa: F821
                alg.addvariable('MaxDeltaEta', maxDeta, 0) # noqa: F821
                alg.addvariable('MinDeltaPhi', minDphi, 0) # noqa: F821
                alg.addvariable('MaxDeltaPhi', maxDphi, 0) # noqa: F821
            else:
                alg.addgeneric('InputWidth1', nleading1)   # noqa: F821
                alg.addgeneric('InputWidth2', nleading2)   # noqa: F821
                alg.addgeneric('MaxTob1', nleading1)       # noqa: F821
                alg.addgeneric('MaxTob2', nleading2)       # noqa: F821
                alg.addvariable('MinET1', ocut1, 0)        # noqa: F821
                alg.addvariable('MinET2', ocut2, 0)        # noqa: F821
                alg.addvariable('DeltaEtaMin', minDeta, 0) # noqa: F821
                alg.addvariable('DeltaEtaMax', maxDeta, 0) # noqa: F821
                alg.addvariable('DeltaPhiMin', minDphi, 0) # noqa: F821
                alg.addvariable('DeltaPhiMax', maxDphi, 0) # noqa: F821

            
            tm.registerAlgo(alg)

        # LFV DETA ATR-14282
        if usev7:
            algoList = [
                {"minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
                {"minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
                ]
        else:
            algoList = []

        for x in algoList:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%sDETA%s-%s%s%s-%s%s%s"  % (minDeta, maxDeta, otype1, str(ocut1), olist1, otype2, str(ocut2) if ocut2>0 else "", olist2) # noqa: F821
 
            log.info("Define %s" % toponame)

            inputList = [otype1 + olist1, otype2 + olist2] # noqa: F821
            alg = AlgConf.DeltaEtaIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)

            alg.addgeneric('InputWidth1', nleading1)   # noqa: F821
            alg.addgeneric('InputWidth2', nleading2)   # noqa: F821
            alg.addgeneric('MaxTob1', nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', nleading2)       # noqa: F821
            alg.addvariable('MinET1', ocut1, 0)        # noqa: F821
            alg.addvariable('MinET2', ocut2, 0)        # noqa: F821
            alg.addvariable('MinDeltaEta', minDeta, 0) # noqa: F821
            alg.addvariable('MaxDeltaEta', maxDeta, 0) # noqa: F821

            tm.registerAlgo(alg)

        if usev7:
            algoList = [
                {"minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
                {"minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU},
                ]
        else:
            algoList = []

        for x in algoList:

            for k in x:
                exec("%s = x[k]" % k)
            
            toponame = "%sDPHI%s-%s%s%s-%s%s%s"  % (minDphi, maxDphi, otype1, str(ocut1), olist1, otype2, str(ocut2) if ocut2>0 else "", olist2) # noqa: F821
 
            log.info("Define %s" % toponame)

            inputList = [otype1 + olist1, otype2 + olist2] # noqa: F821
            alg = AlgConf.DeltaPhiIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)
            
            alg.addgeneric('InputWidth1', nleading1)   # noqa: F821
            alg.addgeneric('InputWidth2', nleading2)   # noqa: F821
            alg.addgeneric('MaxTob1', nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', nleading2)       # noqa: F821
            alg.addvariable('MinET1', ocut1, 0)        # noqa: F821
            alg.addvariable('MinET2', ocut2, 0)        # noqa: F821
            alg.addvariable('MinDeltaPhi', minDphi, 0) # noqa: F821
            alg.addvariable('MaxDeltaPhi', maxDphi, 0) # noqa: F821

            tm.registerAlgo(alg)

            
        # JetMatch
        toponame = "0MATCH-4AJ20.ETA31-4AJj15.ETA31"
        alg = AlgConf.MultiplicityCustom( name = toponame, inputs = [ 'AJMatchall' ], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 31)
        alg.addvariable('MinMultiplicity', 4)
        tm.registerAlgo(alg)
        
        # NoMatch for W T&P
        toponame = "NOT-02MATCH-EM10s1-AJj15all.ETA49"
        alg = AlgConf.NotMatch( name = toponame, inputs = [ 'EMs', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortEM)
        alg.addgeneric('InputWidth2', HW.InputWidthJET)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', HW.InputWidthJET)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 10)
        alg.addvariable('MinET2', 15)
        alg.addvariable('EtaMin1', 0)
        alg.addvariable('EtaMax1', 49)
        alg.addvariable('EtaMin2', 0)
        alg.addvariable('EtaMax2', 49)
        alg.addvariable('DRCut', 4)
        tm.registerAlgo(alg)

        # RATIO SUM for W T&P 
        #toponame = "05RATIO-XE0-SUM0-EM10s1-HT0-AJj15all.ETA49"
        #alg = AlgConf.RatioSum( name = toponame, inputs = ['XE', 'AJjall', 'EMs'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        #alg.addgeneric('InputWidth1', 1)
        #alg.addgeneric('InputWidth2', HW.InputWidthJET) 
        #alg.addgeneric('InputWidth3', HW.OutputWidthSortEM) 
        #alg.addgeneric('MaxTob1', 1)
        #alg.addgeneric('MaxTob2', HW.InputWidthJET)
        #alg.addgeneric('MaxTob3', 1)
        #alg.addgeneric('NumResultBits', 1)
        #alg.addgeneric('UseCluster05Granularity', 1)
        #alg.addvariable('MinET2', 15)
        #alg.addvariable('EtaMin2', 0)
        #alg.addvariable('EtaMax2', 49)
        #alg.addvariable('MinET3', 10)
        #alg.addvariable('EtaMin3', 0)
        #alg.addvariable('EtaMax3', 49)
        #alg.addvariable('MinMET', 0)
        #alg.addvariable('HT', 0)
        #alg.addvariable('SUM', 0)
        #alg.addvariable('Ratio', 5, 0)
        #tm.registerAlgo(alg)

        # RATIO for W T&P
        for x in [
            {"minRatio": 5, "ocut" : 15, "Ratio": "RATIO"},
            #{"minRatio": 8, "ocut" : 15, "Ratio": "RATIO"},
            {"minRatio": 90, "ocut" : 15, "Ratio": "RATIO2"},
            {"minRatio": 250, "ocut" : 15, "Ratio": "RATIO2"},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%02d%s-XE0-HT0-AJj%sall.ETA49"  % (minRatio, Ratio, str(ocut)) # noqa: F821
            log.info("Define %s" % toponame)
            
            alg = AlgConf.Ratio( name = toponame, inputs = ['XE', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ) 
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', 1) 
            alg.addgeneric('InputWidth2', HW.InputWidthJET) 
            alg.addgeneric('MaxTob1', 1)
            alg.addgeneric('MaxTob2', HW.InputWidthJET)
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('isXE2', 1 if Ratio=="RATIO2" else 0)   # noqa: F821
            alg.addvariable('MinET2', str(ocut)) # noqa: F821
            alg.addvariable('EtaMin', 0)
            alg.addvariable('EtaMax', 49)
            alg.addvariable('MinET1', 0)                        
            alg.addvariable('HT', 0)
            alg.addvariable('Ratio', str(minRatio)) # noqa: F821
            tm.registerAlgo(alg)

            
        # RATIO MATCH dedicated to Exotic 
        toponame = '100RATIO-0MATCH-TAU30si2-EMall'
        alg = AlgConf.RatioMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortTAU)
        alg.addgeneric('InputWidth2', HW.InputWidthEM)      
        alg.addgeneric('MaxTob1', 2)
        alg.addgeneric('MaxTob2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30)
        alg.addvariable('MinET2',  0)
        alg.addvariable('Ratio', 100, 0)
        tm.registerAlgo(alg)        


        # NOT MATCH dedicated to Exotic
        toponame = 'NOT-0MATCH-TAU30si2-EMall'
        alg = AlgConf.NotMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortTAU)
        alg.addgeneric('InputWidth2', HW.InputWidthEM)
        alg.addgeneric('MaxTob1', 2)
        alg.addgeneric('MaxTob2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30)
        alg.addvariable('MinET2', 0)
        alg.addvariable('EtaMin1', 0)
        alg.addvariable('EtaMax1', 49)
        alg.addvariable('EtaMin2', 0)
        alg.addvariable('EtaMax2', 49)
        alg.addvariable('DRCut', 0)
        tm.registerAlgo(alg)        

        # MULT-BIT
        for x in [
            {"otype1" : "CMU" ,"ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU},
            {"otype1" : "CMU" ,"ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU},
            ]:
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "MULT-%s%s%s" % (otype1, str(ocut1), olist1) # noqa: F821
            toponames = [toponame+"[0]", toponame+"[1]"]
            log.info("Define %s" % toponames)
            
            inputList = [otype1 + olist1]  # noqa: F821
            alg = AlgConf.Multiplicity( name = toponame,  inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1
            
            alg.addgeneric('InputWidth', inputwidth1) # noqa: F821
            alg.addgeneric('NumResultBits', 2)
            alg.addvariable('MinET', ocut1-1) # for MU threshold -1   # noqa: F821
            tm.registerAlgo(alg)        
            
        
        # DISAMB 2 lists
        for x in [     
            {"disamb": 1, "otype1" : "TAU", "ocut1": 12, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU, "otype2" : "J", "ocut2": 25, "olist2": "ab", "nleading2": HW.OutputWidthSelectJET},
            #{"disamb": 0, "otype1" : "EM",  "ocut1": 15, "olist1" : "abhi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "TAU", "ocut2": 40, "olist2": "ab", "nleading2": HW.OutputWidthSelectTAU},
            #{"disamb": 1, "otype1" : "TAU", "ocut1": 20, "olist1" : "ab", "nleading1": HW.OutputWidthSelectTAU,  "otype2" : "J", "ocut2": 20, "olist2": "ab", "nleading2": HW.OutputWidthSelectJET},
            #{"disamb": 0, "otype1" : "EM",  "ocut1": 15, "olist1" : "abhi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU},
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s"  % (otype1, str(ocut1), olist1) # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2) # noqa: F821
            toponame = "%sDISAMB-%s%s"  % ( disamb if disamb>0 else "", obj1, obj2) # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2] # noqa: F821
            alg = AlgConf.DisambiguationIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', nleading1 if olist1.find("ab")>=0 else -1000) # noqa: F821
            alg.addgeneric('InputWidth2', nleading2 if olist2.find("ab")>=0 else -1000)  # noqa: F821
            alg.addgeneric('MaxTob1', nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', nleading2) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('ClusterOnly', 1 if (otype1=="EM" and otype2=="TAU") or (otype1=="TAU" and otype2=="EM") else 0 ) # noqa: F821
            alg.addgeneric('ApplyDR', 0)
            alg.addvariable('MinET1', ocut1) # noqa: F821
            alg.addvariable('MinET2', ocut2) # noqa: F821
            alg.addvariable('DisambDRSqrMin', disamb*disamb) # noqa: F821
            tm.registerAlgo(alg)
        
        # DISAMB 3 lists
        for x in [     
            {"disamb": 1, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET},
            {"disamb": 1, "otype1" : "TAU", "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectTAU}, 
            ]:
            
            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "%s%s%s"  % (otype1, str(ocut1), olist1.replace('shi','his') + (str(nleading1) if olist1.find('s')>=0 else "")) # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2.replace('shi','his') + (str(nleading2) if olist2.find('s')>=0 else "")) # noqa: F821
            obj3 = "-%s%s%s" % (otype3, str(ocut3), olist3) # noqa: F821
            toponame = "%sDISAMB-%s%s%s"  % ( disamb if disamb>0 else "", obj1, obj2, obj3) # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2, otype3 + olist3] # noqa: F821
            alg = AlgConf.DisambiguationIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', inputwidth2) # noqa: F821
            alg.addgeneric('InputWidth3', inputwidth3) # noqa: F821
            alg.addgeneric('MaxTob1', nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', nleading2) # noqa: F821
            alg.addgeneric('MaxTob3', nleading3) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('ApplyDR', 0)
            alg.addvariable('MinET1', ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', ocut2, 0) # noqa: F821
            alg.addvariable('MinET3', ocut3, 0) # noqa: F821
            alg.addvariable('DisambDRSqr', disamb*disamb, 0) # DisambDR # noqa: F821
            tm.registerAlgo(alg)         

        # DISAMB 3 lists with DR cut to 2nd and 3rd lists

        algolist=[
               {"disamb": 1, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET, "drcutmin": 0, "drcutmax": 28}, 
               {"disamb": 1, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET, "drcutmin": 0, "drcutmax": 28}, # 1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi
               {"disamb": 1, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET, "drcutmin": 0, "drcutmax": 25}, # 1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi
            ]

        for x in algolist:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "-%s%s%s"  % (otype1, str(ocut1), olist1.replace('shi','his') + (str(nleading1) if olist1.find('s')>=0 else ""))  # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2.replace('shi','his') + (str(nleading2) if olist2.find('s')>=0 else "")) # noqa: F821
            obj3 = "%s%s%s" % (otype3, str(ocut3), olist3) # noqa: F821
            toponame = "%sDISAMB-%s-%dDR%d%s%s"  % ( str(disamb) if disamb>0 else "", obj3, drcutmin, drcutmax, obj1, obj2)  # noqa: F821

            log.info("Define %s" % toponame)
            
            inputList = [otype1 + olist1, otype2 + olist2, otype3 + olist3]  # noqa: F821
            alg = AlgConf.DisambiguationDRIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', inputwidth2) # noqa: F821
            alg.addgeneric('InputWidth3', inputwidth3) # noqa: F821
            alg.addgeneric('MaxTob1', nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', nleading2) # noqa: F821
            alg.addgeneric('MaxTob3', nleading3) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', ocut2, 0) # noqa: F821
            alg.addvariable('MinET3', ocut3, 0) # noqa: F821
            alg.addvariable('DisambDRSqrMin', drcutmin*drcutmin, 0) # noqa: F821
            alg.addvariable('DisambDRSqrMax', drcutmax*drcutmax, 0) # noqa: F821
            alg.addvariable('DisambDRSqr', disamb*disamb, 0) # noqa: F821
            tm.registerAlgo(alg)            


        # LAR  0<eta<1.4 and 9/16pi<phi<11/16pi for FE crate IO6 
        for x in [     
            {"minEta": 0, "maxEta": 15, "minPhi": 16, "maxPhi": 23, "otype" : "EM", "ocut" : 20, "inputwidth": HW.OutputWidthSortEM},
            {"minEta": 0, "maxEta": 14, "minPhi": 16, "maxPhi": 23, "otype" : "J", "ocut" : 100, "inputwidth": HW.OutputWidthSortJET}, 
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "LAR-%s%ss1"  % ( otype, str(ocut) if not otype=="EM" else "50" )  # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = otype + 's'  # noqa: F821
            
            alg = AlgConf.LAr( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', inputwidth)  # noqa: F821
            alg.addgeneric('MaxTob', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(ocut)) # noqa: F821
            alg.addvariable('EtaMin', minEta) # noqa: F821
            alg.addvariable('EtaMax', maxEta) # noqa: F821
            alg.addvariable('PhiMin', minPhi) # noqa: F821
            alg.addvariable('PhiMax', maxPhi) # noqa: F821
            tm.registerAlgo(alg)




        xemap = {"etcut": 0, "Threlist": [ 40, 50, 55, 60, 65, 75 ]}
        for x in [ 
            xemap,
            ]:
                
            for k in x:
                exec("%s = x[k]" % k)
            
            log.info("Define %s" % toponame)
            
            inputList = ['XENoSort', 'AJall']
            toponames=[]

            for minxe in Threlist:  # noqa: F821
                toponames.append("KF-XE%s-AJall"  % (minxe))
            
            alg = AlgConf.KalmanMETCorrection( name = "KF-XE-AJall", inputs = inputList, outputs = toponames, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('NumResultBits', len(toponames))
            alg.addvariable('MinET', 0)
            for bitid,minxe in enumerate(Threlist):  # noqa: F821
                alg.addvariable('KFXE', str(minxe), bitid)
            
            tm.registerAlgo(alg)

        # W T&P: MINDPHI(J, XE0), (EM, XE0)

        alglist = [
            {"minDPhi": 15, "otype" : "EM",  "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            {"minDPhi": 15, "otype" : "EM",  "ocut" : 15, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            ]

        for x in alglist:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%02dMINDPHI-%s%s%s%s-XE0"  % (minDPhi, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")  # noqa: F821
            log.info("Define %s" % toponame)

            inputList = otype + olist  # noqa: F821

            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1


            alg.addgeneric('InputWidth1', inputwidth) # noqa: F821
            alg.addgeneric('InputWidth2', 1) 
            alg.addgeneric('MaxTob1', nleading) # noqa: F821
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut) # noqa: F821
            alg.addvariable('MinET2', 0)
            alg.addvariable('DeltaPhiMin', minDPhi, 0)  # noqa: F821
            tm.registerAlgo(alg)

            
        # W T&P MT

        alglistmt = [
            {"minMT": 35, "otype" : "EM", "ocut" : "12", "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            ]
        for x in alglistmt:
            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%iMT-%s%s%s%s-XE0"  % (minMT, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "")  # noqa: F821
            log.info("Define %s" % toponame)

            inputList = otype + olist  # noqa: F821
            
            alg = AlgConf.TransverseMassInclusive1( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1



            alg.addgeneric('InputWidth', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob', str(nleading)) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', str(ocut)) # noqa: F821
            alg.addvariable('MinET2', 0)
            alg.addvariable('MinMTSqr', minMT*minMT) # noqa: F821
            tm.registerAlgo(alg)
            
        # DISAMB 2 lists with DR cut between objects in two lists

        for x in [     
            {"disamb": 0, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "drcutmin": 0, "drcutmax": 28},
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            obj1 = "-%s%s%s"  % (otype1, str(ocut1), olist1.replace('shi','his') + (str(nleading1) if olist1.find('s')>=0 else "")) # noqa: F821
            obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2.replace('shi','his') + (str(nleading2) if olist2.find('s')>=0 else "")) # noqa: F821
            toponame = "%sDISAMB-%dDR%d%s%s"  % ( str(disamb) if disamb>0 else "", drcutmin, drcutmax, obj1, obj2) # noqa: F821

            log.info("Define %s" % toponame)

            inputList = [otype1 + olist1, otype2 + olist2] # noqa: F821
            alg = AlgConf.DisambiguationDRIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', inputwidth2) # noqa: F821
            alg.addgeneric('MaxTob1', nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', nleading2) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', ocut2, 0) # noqa: F821
            alg.addvariable('DisambDRSqrMin', drcutmin*drcutmin, 0) # noqa: F821
            alg.addvariable('DisambDRSqrMax', drcutmax*drcutmax, 0) # noqa: F821
            tm.registerAlgo(alg)   

        # ZH Trigger

        supportedalgolist = [
            {"minDPhi": 10, "otype" : "AJ", "ocut" : 20,  "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET},
        ]            
        for x in supportedalgolist:
            
            for k in x:
                exec("%s = x[k]" % k)
                
            toponame = "%iMINDPHI-%s%s%s%s-XE50"  % (minDPhi, otype, str(ocut) if ocut > 0 else "", olist, str(nleading) if olist=="s" else "") # noqa: F821
            log.info("Define %s" % toponame)
            
            inputList = otype + olist # noqa: F821

            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1

            alg.addgeneric('InputWidth1', inputwidth) # noqa: F821
            alg.addgeneric('InputWidth2', 1)  
            alg.addgeneric('MaxTob1', nleading) # noqa: F821
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', ocut) # noqa: F821
            alg.addvariable('MinET2', 50)
            alg.addvariable('DeltaPhiMin', minDPhi, 0)  # noqa: F821
            tm.registerAlgo(alg)
                
        # LATE MUON
        for x in [     
            #{"otype" : "LATE-MU", "ocut" : 10, "inputwidth": HW.OutputWidthSortMU},
            {"otype" : "LATE-MU", "ocut" : 10, "inputwidth": HW.NumberOfDelayedMuons},
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "%s%ss1"  % ( otype, str(ocut) ) # noqa: F821

            log.info("Define %s" % toponame)

            inputList = 'LMUs'

            alg = AlgConf.EtCut( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', inputwidth)  # noqa: F821
            alg.addgeneric('MaxTob', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(ocut)) # noqa: F821
            tm.registerAlgo(alg)
            

        # (ATR-12748) fat jet trigger with Simple Cone algo
        for x in [            
            {"minHT": 111, "otype" : "CJ", "ocut" : 15, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "oeta" : 26},
            {"minHT": 85, "otype" : "CJ", "ocut" : 15, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "oeta" : 26},                
            ]:

            for k in x:
                exec("%s = x[k]" % k)

            toponame = "SC%d-%s%s%s%s.ETA%s" % (minHT, otype, str(ocut), olist, str(nleading) if olist=="s" else "", str(oeta)) # noqa: F821

            log.info("Define %s" % toponame)

            inputList = otype + olist # noqa: F821

            alg = AlgConf.SimpleCone( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId ); currentAlgoId += 1


            alg.addgeneric('InputWidth', inputwidth) # noqa: F821
            alg.addvariable('MinET', ocut)           # noqa: F821       
            alg.addvariable('MinSumET', minHT)       # noqa: F821
            alg.addvariable('MaxRSqr', 10*10)                        
            tm.registerAlgo(alg)  

        # DISAMB-INVM

        for x in [
                {"disamb": 0, "minInvm": 30, "maxInvm": 9999,"otype1" : "EM",  "ocut1": 20, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12, "olist2": "ab", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # DISAMB-30INVM-EM20his2-TAU12ab
                ]:
            
                for k in x:
                    exec("%s = x[k]" % k)

                obj1 = "%s%s%s"  % (otype1, str(ocut1), olist1.replace('shi','his') + (str(nleading1) if olist1.find('s')>=0 else "")) # noqa: F821
                obj2 = "-%s%s%s" % (otype2, str(ocut2), olist2) # noqa: F821
                toponame = "%sDISAMB-%iINVM%s-%s%s"  % ( disamb if disamb>0 else "", minInvm, str(maxInvm) if maxInvm<9999 else "", obj1, obj2)       # noqa: F821    

                log.info("Define %s" % toponame)
                inputList = [otype1 + olist1, otype2 + olist2]      # noqa: F821         
                #alg = AlgConf.DisambiguationInvariantMass2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1
                alg = AlgConf.DisambiguationInvmIncl2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1

                alg.addgeneric('InputWidth1', inputwidth1) # noqa: F821
                alg.addgeneric('InputWidth2', inputwidth2) # noqa: F821
                alg.addgeneric('MaxTob1', nleading1) # noqa: F821
                alg.addgeneric('MaxTob2', nleading2) # noqa: F821
                alg.addgeneric('NumResultBits', 1)
            
                alg.addvariable('MinET1', ocut1) # noqa: F821
                alg.addvariable('MinET2', ocut2) # noqa: F821
                alg.addvariable('MinMSqr', minInvm * minInvm) # noqa: F821
                alg.addvariable('MaxMSqr', maxInvm * maxInvm) # noqa: F821
                
                tm.registerAlgo(alg)


        for x in [
                {  "minInvm": 400, "maxInvm": 9999, "otype1" : "AJ", "ocut1": 30, "olist1" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "otype2" : "AJ", "ocut2": 20, "olist2" : "s", "nleading2" : 6, "inputwidth2": HW.OutputWidthSortJET, "applyEtaCut":1, "minEta1": 0 ,"maxEta1": 31 , "minEta2": 31 ,"maxEta2": 49 , }, #400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49
                ]:
            
                for k in x:
                    exec("%s = x[k]" % k)

                obj1 = "%s%s%s.%sETA%i"  % (otype1, str(ocut1), olist1 + (str(nleading1) if olist1.find('s')>=0 else ""),str(minEta1) if minEta1>0 else "", maxEta1) # noqa: F821
                obj2 = "-%s%s%s.%sETA%i"  % (otype2, str(ocut2), olist2 + (str(nleading2) if olist2.find('s')>=0 else ""),str(minEta2) if minEta2>0 else "", maxEta2) # noqa: F821
            
                inputList = [otype1 + olist1, otype2 + olist2] # noqa: F821
                        
                toponame = "%iINVM%i-%s%s"   % (minInvm, maxInvm, obj1, obj2) # noqa: F821
                alg = AlgConf.InvariantMassInclusive2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1    


                alg.addgeneric('InputWidth1', inputwidth1) # noqa: F821
                alg.addgeneric('InputWidth2', inputwidth2) # noqa: F821
                alg.addgeneric('MaxTob1', nleading1) # noqa: F821
                alg.addgeneric('MaxTob2', nleading2) # noqa: F821
                alg.addgeneric('NumResultBits', 1)
                if (applyEtaCut>0): # noqa: F821
                    alg.addgeneric('ApplyEtaCut', applyEtaCut) # noqa: F821

                alg.addvariable('MinET1', ocut1) # noqa: F821
                alg.addvariable('MinET2', ocut2) # noqa: F821
                alg.addvariable('MinMSqr', minInvm * minInvm ) # noqa: F821
                alg.addvariable('MaxMSqr', maxInvm * maxInvm ) # noqa: F821
                if (applyEtaCut>0): # noqa: F821
                    alg.addvariable('MinEta1', minEta1) # noqa: F821
                    alg.addvariable('MaxEta1', maxEta1) # noqa: F821
                    alg.addvariable('MinEta2', minEta2) # noqa: F821
                    alg.addvariable('MaxEta2', maxEta2) # noqa: F821

                tm.registerAlgo(alg)
