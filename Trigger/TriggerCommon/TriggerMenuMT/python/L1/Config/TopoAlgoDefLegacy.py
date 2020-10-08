# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from .TypeWideThresholdConfig import getTypeWideThresholdConfig

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW
from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.TopoAlgoDefLegacy')

class TopoAlgoDefLegacy:

    @staticmethod
    def registerTopoAlgos(tm):

        _etamax = 49
        _minet = 0
        _emscale_for_decision = 1000 / getTypeWideThresholdConfig("EM")["resolutionMeV"]

        alg = AlgConf.ClusterNoSort( name = 'EMall', inputs = 'ClusterTobArray', outputs = 'EMall', algoId = 0) 
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('OutputWidth', HW.InputWidthEM)
        alg.addvariable('IsoMask', 0)
        tm.registerTopoAlgo(alg)  

                                
        alg = AlgConf.ClusterSelect( name = 'TAUabi', inputs = 'ClusterTobArray', outputs = 'TAUabi', algoId = 1 )
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectTAU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)        
        alg.addvariable('MinET', 12) 
        alg.addvariable('IsoMask', 2) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 1)
        tm.registerTopoAlgo(alg) 


        alg = AlgConf.ClusterSelect( name = 'EMabi', inputs = 'ClusterTobArray', outputs = 'EMabi', algoId = 2 )
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET', 8)
        alg.addvariable('IsoMask', 2)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 1)
        tm.registerTopoAlgo(alg) 


        alg = AlgConf.ClusterSelect( name = 'EMabhi', inputs = 'ClusterTobArray', outputs = 'EMabhi', algoId = 3 )
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET', 8)
        alg.addvariable('IsoMask', 3)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 1)
        tm.registerTopoAlgo(alg) 

        
        alg = AlgConf.ClusterSelect( name = 'TAUab', inputs = 'ClusterTobArray', outputs = 'TAUab', algoId = 4 )
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectTAU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)
        alg.addvariable('MinET', 12) 
        alg.addvariable('IsoMask', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 0)
        tm.registerTopoAlgo(alg) 


        alg = AlgConf.ClusterSort( name = 'EMs', inputs = 'ClusterTobArray', outputs = 'EMs', algoId = 5 )
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('IsoMask', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax) 
        alg.addgeneric('DoIsoCut', '0')
        #alg.addgeneric('DoEtaCut', '1')
        tm.registerTopoAlgo(alg) 

        
        alg = AlgConf.ClusterSort( name = 'EMshi', inputs = 'ClusterTobArray', outputs = 'EMshi', algoId = 6 )
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('IsoMask', 3) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', '1')
        #alg.addgeneric('DoEtaCut', '1')
        tm.registerTopoAlgo(alg)


        alg = AlgConf.ClusterSort( name = 'TAUsi', inputs = 'ClusterTobArray', outputs = 'TAUsi', algoId = 7 )
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortTAU)
        alg.addvariable('IsoMask', 2) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', '1')
        #alg.addgeneric('DoEtaCut', '1')
        tm.registerTopoAlgo(alg)

        
        alg = AlgConf.JetNoSort( name = 'AJall', inputs = 'JetTobArray', outputs = 'AJall', algoId = 8 ) 
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', HW.DefaultJetSize)
        tm.registerTopoAlgo(alg)


        alg = AlgConf.JetNoSort( name = 'AJjall', inputs = 'JetTobArray', outputs = 'AJjall', algoId = 9 ) 
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)
        tm.registerTopoAlgo(alg)


        # for 0MATCH-4AJ20-4AJj15
        alg = AlgConf.JetNoSortMatch( name = 'AJMatchall', inputs = 'JetTobArray', outputs = 'AJMatchall', algoId = 10 ) 
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', 2 if HW.DefaultJetSize.value==2 else 1)
        alg.addvariable('MinET1', 15) # 4x4       
        alg.addvariable('MinET2', 20) # 8x8
        tm.registerTopoAlgo(alg)        


        # ab J lists:
        for idc, jet_type in enumerate(['J', 'CJ', 'FJ']):
            jetabseta = _etamax
            _minet = 25
            _mineta=0
            if jet_type=='J':
                jetabseta = 31
                _minet = 20
            elif jet_type=='CJ':
                jetabseta = 26 
                _minet = 15
            elif jet_type=='FJ':
                _mineta = 31
                _minet = 15
            alg = AlgConf.JetSelect( name = jet_type+'ab', inputs = 'JetTobArray', outputs = jet_type+'ab', algoId = 11 + idc )
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectJET )            
            alg.addgeneric('OutputWidth', HW.OutputWidthSelectJET)
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinET', _minet)  
            alg.addvariable('MinEta', _mineta)
            alg.addvariable('MaxEta', jetabseta)
            alg.addgeneric('DoEtaCut', 1)
            tm.registerTopoAlgo(alg) 


        alg = AlgConf.JetSort( name = 'AJjs', inputs = 'JetTobArray', outputs = 'AJjs', algoId = 14)
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoEtaCut', 0)
        tm.registerTopoAlgo(alg) 


        # Sorted J lists:
        for idc, jet_type in enumerate(['AJ', 'FJ']):
            jetabseta = _etamax
            _minet = 25
            _mineta = 0
            if jet_type=='J':
                jetabseta = 31
                _minet = 20
            elif jet_type=='CJ':
                jetabseta = 26 
                _minet = 15
            elif jet_type=='FJ':
                _mineta = 31
                _minet = 15
                
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s', algoId = 15 + idc )
            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addgeneric('JetSize', HW.DefaultJetSize.value) 
            alg.addvariable('MinEta', _mineta)
            alg.addvariable('MaxEta', jetabseta)
            if jet_type=='FJ':
                alg.addgeneric('DoEtaCut', 1)
            else:
                alg.addgeneric('DoEtaCut', 0)
            tm.registerTopoAlgo(alg) 


        alg = AlgConf.JetSort( name = 'Js', inputs = 'JetTobArray', outputs = 'Js', algoId = 17 )
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', HW.DefaultJetSize.value)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 31)
        alg.addgeneric('DoEtaCut', 1)
        tm.registerTopoAlgo(alg)


        alg = AlgConf.METNoSort( name = 'XENoSort', inputs = 'MetTobArray', outputs = 'XENoSort', algoId = 18 )
        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerTopoAlgo(alg)
                
        alg = AlgConf.MetSort( name = 'XE', inputs = 'MetTobArray', outputs = 'XE', algoId = 19 )
        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerTopoAlgo(alg)
        
        alg = AlgConf.MuonSelect( name = 'MUab', inputs = 'MuonTobArray', outputs = 'MUab', algoId = 20 )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinET', 4) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 25)
        tm.registerTopoAlgo(alg)            

        alg = AlgConf.MuonSort( name = 'MUs', inputs = 'MuonTobArray', outputs = 'MUs', algoId = 21 )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortMU)
        # alg.addvariable('MinET', 4)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 25)
        tm.registerTopoAlgo(alg)


        alg = AlgConf.MuonSelect( name = 'CMUab', inputs = 'MuonTobArray', outputs = 'CMUab', algoId = 22 )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinET', 4) 
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 10)
        tm.registerTopoAlgo(alg)


        alg = AlgConf.MuonSort_1BC( name = 'LMUs', inputs = 'LateMuonTobArray', outputs = 'LMUs', algoId = 23 )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        #alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortMU)
        alg.addgeneric('nDelayedMuons', 1)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 25)
        tm.registerTopoAlgo(alg)

        
        # Abbreviated lists:
        alg = AlgConf.ClusterSelect( name = 'EMab', inputs = 'ClusterTobArray', outputs = 'EMab', algoId = 24 )
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM ) 
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET', 5)
        alg.addvariable('IsoMask', 0)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 0)
        tm.registerTopoAlgo(alg) 
        

        # "all" lists:
        alg = AlgConf.ClusterNoSort( name = 'TAUall', inputs = 'ClusterTobArray', outputs = 'TAUall', algoId = 25) 
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('OutputWidth', HW.InputWidthTAU)
        alg.addvariable('IsoMask', 0)
        tm.registerTopoAlgo(alg)


        #######
        ##
        ## Decision algorithms
        ##
        #######
        currentAlgoId = 0        

        # VBF items
        algoList = [
            { "algoname": 'INVM_AJ_HighMass', "Threlist": [ 900, 800, 700, 500 ], "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s",
              "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6},
            { "algoname": 'INVM_AJ_LowMass',  "Threlist": [ 400, 300, 200, 100 ], "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s",
              "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6}
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = d.otype + d.olist
            toponames=[]
            for minInvm in d.Threlist:
                toponame = "%iINVM%i-%s%s%s%s-%s%s%s%s"  % (minInvm, d.maxInvm,
                                                            d.otype, str(d.ocut1) , d.olist, str(d.nleading1) if d.olist=="s" else "",
                                                            d.otype, str(d.ocut2) , d.olist, str(d.nleading2) if d.olist=="s" else "")
                toponames.append(toponame)
            alg = AlgConf.InvariantMassInclusive1( name = d.algoname, inputs = inputList, outputs = toponames, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('MaxTob', d.nleading1)
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, minInvm in enumerate(d.Threlist):
                alg.addvariable('MinET1', d.ocut1, bitid)
                alg.addvariable('MinET2', d.ocut2, bitid)
                alg.addvariable('MinMSqr', minInvm * minInvm, bitid)
                alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm, bitid)
            tm.registerTopoAlgo(alg)


        # dimu INVM items
        listofalgos = [
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
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist)
            toponame = "%iINVM%i-%s%s%s"  % (d.minInvm, d.maxInvm, "ONEBARREL-" if d.onebarrel==1 else "", obj1, "" if d.mult>1 else obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist, d.otype2 + d.olist]
            algoname = AlgConf.InvariantMassInclusive1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.InvariantMassInclusive2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', d.onebarrel)
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2 if d.ocut2>0 else d.ocut1)
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm)
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm)
            tm.registerTopoAlgo(alg)


        # dimu DR items
        listofalgos=[
            {"minDr": 2, "maxDr": 99, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, # SM Y  x, 2DR99-2MU4ab
            {"minDr": 0, "maxDr": 10, "mult": 1, "otype1" : "MU" ,"ocut1": 10, "olist" : "ab", "otype2" : "MU", "ocut2": 6, "onebarrel": 0}, # Exotic LFV x, 0DR10-MU10ab-MU6ab
            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0},   #x, 2DR15-2MU6ab
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR15-2MU4ab
            {"minDr": 0, "maxDr": 15, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0}, #0DR15-MU6ab-MU4ab
            {"minDr": 0, "maxDr": 34, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR34-2MU4ab 
            {"minDr": 0, "maxDr": 24, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR24-2MU4ab 
            {"minDr": 0, "maxDr": 22, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR22-2MU6ab
            {"minDr": 0, "maxDr": 22, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0}, #0DR22-MU6ab-MU4ab
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR15-2MU6ab  
        ]
        for x in listofalgos:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist)
            toponame = "%iDR%i-%s%s%s"  % (d.minDr, d.maxDr, "ONEBARREL-" if d.onebarrel==1 else "", obj1, "" if d.mult>1 else obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist, d.otype2 + d.olist]
            algoname = AlgConf.DeltaRSqrIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1 
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', d.onebarrel)
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr)
            tm.registerTopoAlgo(alg)


        # deta-dphi with ab+ab
        algolist=[
            { "minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU,
              "otype2" : "MU", "ocut2": 4, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-MU6ab-MU4ab
            { "minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU,
              "otype2" : "", "ocut2": 6, "olist2": "", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-2MU6ab           
        ]     
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist1)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2)
            toponame = "%sDETA%s-%sDPHI%s-%s%s"  % (d.minDeta, d.maxDeta, d.minDphi, d.maxDphi, obj1, "" if d.mult>1 else obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist1, d.otype2 + d.olist2]
            algoname = AlgConf.DeltaEtaPhiIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaEtaPhiIncl2
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', d.nleading1)
                alg.addgeneric('MaxTob', d.nleading1)
                alg.addvariable('MinET1', d.ocut1)
                alg.addvariable('MinET2', d.ocut2)
                alg.addvariable('MinDeltaEta', d.minDeta)
                alg.addvariable('MaxDeltaEta', d.maxDeta)
                alg.addvariable('MinDeltaPhi', d.minDphi)
                alg.addvariable('MaxDeltaPhi', d.maxDphi)
            else:
                alg.addgeneric('InputWidth1', d.nleading1)
                alg.addgeneric('InputWidth2', d.nleading2)
                alg.addgeneric('MaxTob1', d.nleading1)
                alg.addgeneric('MaxTob2', d.nleading2)
                alg.addvariable('DeltaEtaMin', d.minDeta)
                alg.addvariable('DeltaEtaMax', d.maxDeta)
                alg.addvariable('DeltaPhiMin', d.minDphi)
                alg.addvariable('DeltaPhiMax', d.maxDphi)
                alg.addvariable('MinET1', d.ocut1)
                alg.addvariable('MinET2', d.ocut2)
            tm.registerTopoAlgo(alg)


        algolist = [
            { "minDr": 0, "maxDr": 28, "otype1" : "MU" ,"ocut1": 10, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU,
              "otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU},   # 0DR28-MU10ab-TAU12abi
            { "minDr": 0, "maxDr": 28, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # 0DR28-TAU20abi-TAU12abi
            { "minDr": 0, "maxDr": 25, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # 0DR25-TAU20abi-TAU12abi
        ] 
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s" % (d.otype1, str(d.ocut1), d.olist1)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2)
            toponame = "%iDR%i-%s%s"  % (d.minDr, d.maxDr, obj1, obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1] if d.otype1==d.otype2 else [d.otype1 + d.olist1, d.otype2 + d.olist2]
            algoname = AlgConf.DeltaRSqrIncl1 if d.otype1==d.otype2 else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            if d.otype1==d.otype2:
                alg.addgeneric('InputWidth', d.inputwidth1)
                alg.addgeneric('MaxTob', d.nleading1)
            else:
                alg.addgeneric('InputWidth1', d.inputwidth1)
                alg.addgeneric('InputWidth2', d.inputwidth2)
                alg.addgeneric('MaxTob1', d.nleading1)
                alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            if d.otype1==d.otype2:
                alg.addvariable('MinET1', d.ocut1)
                alg.addvariable('MinET2', d.ocut2)
                alg.addvariable('DeltaRMin', d.minDr*d.minDr)
                alg.addvariable('DeltaRMax', d.maxDr*d.maxDr)
            else:
                alg.addvariable('MinET1', d.ocut1, 0)
                alg.addvariable('MinET2', d.ocut2, 0)
                alg.addvariable('DeltaRMin', d.minDr*d.minDr, 0)
                alg.addvariable('DeltaRMax', d.maxDr*d.maxDr, 0)                
            tm.registerTopoAlgo(alg)        
            

        # (ATR-8194) L1Topo HT Trigger
        algoList = [
            {"minHT": 150, "otype" : "J", "ocut" : 20, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 31}, #HT150-J20s5pETA31
            {"minHT": 190, "otype" : "J", "ocut" : 15, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 21}, #HT190-J15s5pETA21
            {"minHT": 190, "otype" : "AJ", "ocut" : 15, "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 21}, #HT190-AJ15allpETA21
            {"minHT": 150, "otype" : "AJ", "ocut" : 20, "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 31}, #HT150-AJ20allpETA31
            {"minHT": 150, "otype" : "AJj","ocut" : 15, "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 49}, #HT150-AJj15allpETA49
            {"minHT": 20,  "otype" : "AJj","ocut" : 15,  "olist" : "all", "nleading" : HW.InputWidthJET, "inputwidth": HW.InputWidthJET, "oeta" : 49}, #HT20-AJj15allpETA49
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "HT%d-%s%s%s%spETA%s" % (d.minHT, d.otype, str(d.ocut), d.olist, str(d.nleading) if d.olist=="s" else "", str(d.oeta))            
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.JetHT( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', d.nleading)
            alg.addgeneric('NumRegisters', 2 if d.olist=="all" else 0)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', d.ocut)
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', d.oeta)
            alg.addvariable('MinHt', d.minHT)
            tm.registerTopoAlgo(alg)  


        # INVM_EM for Jpsi
        algoList = [
            {"algoname": 'INVM_EMs6' , "ocutlist": [ 0, 7, 12 ], "minInvm": 1, "maxInvm": 5, "otype" : "EM", "olist" : "s", "nleading" : 1, "inputwidth": HW.OutputWidthSortEM}
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = d.otype + d.olist
            toponames=[]
            for ocut in d.ocutlist:
                toponame = "%iINVM%i-%s%s%s%s-EMs6"   % (d.minInvm, d.maxInvm, d.otype, str(ocut) if ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
                toponames.append(toponame)
            alg = AlgConf.InvariantMassInclusive2( name = d.algoname, inputs = [inputList, 'EMs'], outputs = toponames, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth)
            #alg.addgeneric('InputWidth2', HW.InputWidthEM)
            alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob1', d.nleading)
            #alg.addgeneric('MaxTob2', HW.InputWidthEM)
            alg.addgeneric('MaxTob2', HW.OutputWidthSortEM)
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, ocut in enumerate(d.ocutlist):
                alg.addvariable('MinET1', ocut, bitid)
                alg.addvariable('MinET2', 0, bitid)
                alg.addvariable('MinMSqr', (d.minInvm * _emscale_for_decision)*(d.minInvm * _emscale_for_decision), bitid)
                alg.addvariable('MaxMSqr', (d.maxInvm * _emscale_for_decision)*(d.maxInvm * _emscale_for_decision), bitid)                
            tm.registerTopoAlgo(alg)


        # W T&P: MINDPHI(J, XE0), (EM, XE0)
        alglist = [
            {"minDPhi":  5, "otype" : "AJj", "ocut" : 10, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortJET},
            {"minDPhi": 10, "otype" : "AJj", "ocut" : 10, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortJET},
            {"minDPhi": 15, "otype" : "AJj", "ocut" : 10, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortJET},
            {"minDPhi":  5, "otype" : "EM",  "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},#new
            {"minDPhi":  5, "otype" : "EM",  "ocut" : 15, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},#same
        ]
        for x in alglist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%02dMINDPHI-%s%s%s%s-XE0"  % (d.minDPhi, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', 1) 
            alg.addgeneric('MaxTob1', d.nleading)
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut)
            alg.addvariable('MinET2', 0)
            alg.addvariable('DeltaPhiMin', d.minDPhi, 0)
            tm.registerTopoAlgo(alg)


        # W T&P MT
        alglistmt = [
            {"minMT": 25, "otype" : "EM", "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            {"minMT": 35, "otype" : "EM", "ocut" : 15, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
        ]
        for x in alglistmt:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iMT-%s%s%s%s-XE0"  % (d.minMT, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist            
            alg = AlgConf.TransverseMassInclusive1( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob', str(d.nleading))
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', str(d.ocut))
            alg.addvariable('MinET2', 0)
            alg.addvariable('MinMTSqr', d.minMT*d.minMT)
            tm.registerTopoAlgo(alg)
            

        # VBF deta
        algoList = [
            { "minDeta": 63,  "maxDeta": 127, "otype" : "FJ",  "ocut1" : 20,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortJET,
              "ocut2" : 20, "nleading2": 2}, #63DETA127-FJ20s1-FJ20s2
            { "minDeta": 0,  "maxDeta": 20, "otype" : "J",  "ocut1" : 50,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortJET,
              "ocut2" : 0, "nleading2": 2}, #0DETA20-J50s1-Js2
        ]
        for x in algoList:                 
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDETA%i-%s%s%s%s-%s%s%s%s"  % (d.minDeta, d.maxDeta,
                                                        d.otype, str(d.ocut1) if d.ocut1 > 0 else "", d.olist, str(d.nleading1) if d.olist=="s" else "",
                                                        d.otype, str(d.ocut2) if d.ocut2 > 0 else "", d.olist, str(d.nleading2) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.DeltaEtaIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('MaxTob', d.nleading2)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', str(d.ocut1), 0)
            alg.addvariable('MinET2', str(d.ocut2), 0)
            alg.addvariable('MinDeltaEta', d.minDeta, 0)
            alg.addvariable('MaxDeltaEta', d.maxDeta, 0)
            tm.registerTopoAlgo(alg)

            
        # ZH Trigger
        supportedalgolist = [
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 50 }, #10MINDPHI-J20s2-XE50
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "ocut2": 50}, #10MINDPHI-J20ab-XE50
            {"minDPhi": 10, "otype" : "CJ","ocut" : 20, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "ocut2": 50}, #10MINDPHI-CJ20ab-XE50
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 30 }, #10MINDPHI-J20s2-XE30
        ]
        for x in supportedalgolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iMINDPHI-%s%s%s%s-XE%i"  % (d.minDPhi, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "",d.ocut2)
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', 1)  
            alg.addgeneric('MaxTob1', d.nleading)
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('DeltaPhiMin', d.minDPhi, 0)
            tm.registerTopoAlgo(alg)
            

        # added for muon-jet:
        algoList = [
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"}, #0DR04-MU4ab-CJ15ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 30, "olist2" : "ab"}, #0DR04-MU4ab-CJ30ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"}, #0DR04-MU4ab-CJ20ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 25, "olist2" : "ab"}, #0DR04-MU6ab-CJ25ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"}, #0DR04-MU6ab-CJ20ab
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (d.minDr, d.maxDr, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2), d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr, 0)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr, 0)
            tm.registerTopoAlgo(alg)


        # dimu INVM items
        algolist=[
            {"minInvm": 2, "maxInvm": 8, "mult": 1, "otype1" : "CMU","ocut1": 4, "olist" : "ab", "otype2" :"MU", "ocut2" : 4, "onebarrel": 0}, # 2INVM8-CMU4ab-MU4ab
            {"minInvm": 2, "maxInvm": 8, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist" : "ab", "otype2" : "MU","ocut2" : 4, "onebarrel": 1}, # 2INVM8-ONEBARREL-MU6ab-MU4ab
        ]
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist)
            toponame = "%iINVM%i-%s%s%s"  % (d.minInvm, d.maxInvm, "ONEBARREL-" if d.onebarrel==1 else "", obj1, "" if d.mult>1 else obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist, d.otype2 + d.olist]
            algoname = AlgConf.InvariantMassInclusive1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.InvariantMassInclusive2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', d.onebarrel)
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2 if d.ocut2>0 else d.ocut1)
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm)
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm)
            tm.registerTopoAlgo(alg)


        # dimu DR items
        algolist = [
            {"minDr": 0, "maxDr": 24, "mult": 2, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR24-2CMU4ab
            {"minDr": 0, "maxDr": 24, "mult": 1, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "MU","ocut2": 4, "onebarrel": 0}, #0DR24-CMU4ab-MU4ab  
        ]
        for x in algolist: 
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist)
            toponame = "%iDR%i-%s%s%s"  % (d.minDr, d.maxDr, "ONEBARREL-" if d.onebarrel==1 else "", obj1, "" if d.mult>1 else obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist, d.otype2 + d.olist]
            algoname = AlgConf.DeltaRSqrIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaRSqrIncl2
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
                alg.addgeneric('RequireOneBarrel', d.onebarrel)
            else:
                alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
                alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU) 
                alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
                alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr)
            tm.registerTopoAlgo(alg)


        # deta-dphi with ab+ab
        algoList = [
            { "minDeta": 0, "maxDeta": 20, "minDphi": 0, "maxDphi": 20, "mult": 1, "otype1" : "TAU", "ocut1": 20, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU },
        ]
        for x in algoList:                 
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist1)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2)
            toponame = "%sDETA%s-%sDPHI%s-%s%s"  % (d.minDeta, d.maxDeta, d.minDphi, d.maxDphi, obj1, "" if d.mult>1 else obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist1, d.otype2 + d.olist2]
            algoname = AlgConf.DeltaEtaPhiIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaEtaPhiIncl2
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)                        
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', d.nleading1)
                alg.addgeneric('MaxTob', d.nleading1)
                alg.addvariable('MinET1', d.ocut1, 0)
                alg.addvariable('MinET2', d.ocut2, 0)
                alg.addvariable('MinDeltaEta', d.minDeta, 0)
                alg.addvariable('MaxDeltaEta', d.maxDeta, 0)
                alg.addvariable('MinDeltaPhi', d.minDphi, 0)
                alg.addvariable('MaxDeltaPhi', d.maxDphi, 0)
            else:
                alg.addgeneric('InputWidth1', d.nleading1)
                alg.addgeneric('InputWidth2', d.nleading2)
                alg.addgeneric('MaxTob1', d.nleading1)
                alg.addgeneric('MaxTob2', d.nleading2)
                alg.addvariable('MinET1', d.ocut1, 0)
                alg.addvariable('MinET2', d.ocut2, 0)
                alg.addvariable('DeltaEtaMin', d.minDeta, 0)
                alg.addvariable('DeltaEtaMax', d.maxDeta, 0)
                alg.addvariable('DeltaPhiMin', d.minDphi, 0)
                alg.addvariable('DeltaPhiMax', d.maxDphi, 0)
            tm.registerTopoAlgo(alg)


        # LFV DETA ATR-14282
        algoList = [
            { "minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM,
              "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DETA04-EM8abi-MU10ab
            { "minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM,
              "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DETA04-EM15abi-MUab
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%sDETA%s-%s%s%s-%s%s%s"  % (d.minDeta, d.maxDeta, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaEtaIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('InputWidth1', d.nleading1)
            alg.addgeneric('InputWidth2', d.nleading2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('MinDeltaEta', d.minDeta, 0)
            alg.addvariable('MaxDeltaEta', d.maxDeta, 0)
            tm.registerTopoAlgo(alg)


        algoList = [
            { "minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM,
              "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DPHI03-EM8abi-MU10ab
            { "minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM,
              "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DPHI03-EM15abi-MUab
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%sDPHI%s-%s%s%s-%s%s%s"  % (d.minDphi, d.maxDphi, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaPhiIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)            
            alg.addgeneric('InputWidth1', d.nleading1)
            alg.addgeneric('InputWidth2', d.nleading2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('MinDeltaPhi', d.minDphi, 0)
            alg.addvariable('MaxDeltaPhi', d.maxDphi, 0)
            tm.registerTopoAlgo(alg)
            
        # JetMatch
        toponame = "0MATCH-4AJ20pETA31-4AJj15pETA31"
        alg = AlgConf.MultiplicityCustom( name = toponame, inputs = [ 'AJMatchall' ], outputs = [ toponame ], algoId = currentAlgoId )
        currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET', 0)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 31)
        alg.addvariable('MinMultiplicity', 4)
        tm.registerTopoAlgo(alg)

        
        # NoMatch for W T&P
        toponame = "NOT-02MATCH-EM10s1-AJj15allpETA49"
        alg = AlgConf.NotMatch( name = toponame, inputs = [ 'EMs', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId )
        currentAlgoId += 1
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
        tm.registerTopoAlgo(alg)


        # RATIO for W T&P
        algolist = [
            { "minRatio": 5, "ocut" : 15, "Ratio": "RATIO"},
            { "minRatio": 90, "ocut" : 15, "Ratio": "RATIO2"},
            { "minRatio": 250, "ocut" : 15, "Ratio": "RATIO2"},
        ]
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%02d%s-XE0-HT0-AJj%sallpETA49"  % (d.minRatio, d.Ratio, str(d.ocut))
            log.debug("Define %s", toponame)            
            alg = AlgConf.Ratio( name = toponame, inputs = ['XE', 'AJjall'], outputs = [ toponame ], algoId = currentAlgoId ) 
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', 1) 
            alg.addgeneric('InputWidth2', HW.InputWidthJET) 
            alg.addgeneric('MaxTob1', 1)
            alg.addgeneric('MaxTob2', HW.InputWidthJET)
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('isXE2', 1 if d.Ratio=="RATIO2" else 0)
            alg.addvariable('MinET2', str(d.ocut))
            alg.addvariable('EtaMin', 0)
            alg.addvariable('EtaMax', 49)
            alg.addvariable('MinET1', 0)                        
            alg.addvariable('HT', 0)
            alg.addvariable('Ratio', str(d.minRatio))
            tm.registerTopoAlgo(alg)
            

        # RATIO MATCH dedicated to Exotic 
        toponame = '100RATIO-0MATCH-TAU30si2-EMall'
        alg = AlgConf.RatioMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId )
        currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortTAU)
        alg.addgeneric('InputWidth2', HW.InputWidthEM)      
        alg.addgeneric('MaxTob1', 2)
        alg.addgeneric('MaxTob2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30)
        alg.addvariable('MinET2',  0)
        alg.addvariable('Ratio', 100, 0)
        tm.registerTopoAlgo(alg)        


        # NOT MATCH dedicated to Exotic
        toponame = 'NOT-0MATCH-TAU30si1-EMall'
        alg = AlgConf.NotMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId )
        currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortTAU)
        alg.addgeneric('InputWidth2', HW.InputWidthEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30)
        alg.addvariable('MinET2', 0)
        alg.addvariable('EtaMin1', 0)
        alg.addvariable('EtaMax1', 49)
        alg.addvariable('EtaMin2', 0)
        alg.addvariable('EtaMax2', 49)
        alg.addvariable('DRCut', 0)
        tm.registerTopoAlgo(alg)        


        # MULT-BIT
        algolist = [
            {"otype1" : "CMU" ,"ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU}, #MULT-CMU4ab
            {"otype1" : "CMU" ,"ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU}, #MULT-CMU6ab
        ]
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "MULT-%s%s%s" % (d.otype1, str(d.ocut1), d.olist1)
            toponames = [toponame+"[0]", toponame+"[1]"]
            log.debug("Define %s", toponames)
            inputList = [d.otype1 + d.olist1]
            alg = AlgConf.Multiplicity( name = toponame,  inputs = inputList, outputs = toponames, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('NumResultBits', 2)
            alg.addvariable('MinET', d.ocut1-1) # for MU threshold -1
            tm.registerTopoAlgo(alg)        


        # DISAMB 2 lists
        algolist = [
            { "disamb": 1, "otype1" : "TAU", "ocut1": 12, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU,
              "otype2" : "J", "ocut2": 25, "olist2": "ab", "nleading2": HW.OutputWidthSelectJET}, #1DISAMB-TAU12abi-J25ab
        ]
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2)
            toponame = "%sDISAMB-%s%s"  % ( d.disamb if d.disamb>0 else "", obj1, obj2)            
            log.debug("Define %s", toponame)            
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DisambiguationIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.nleading1 if d.olist1.find("ab")>=0 else -1000)
            alg.addgeneric('InputWidth2', d.nleading2 if d.olist2.find("ab")>=0 else -1000)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('ClusterOnly', 1 if (d.otype1=="EM" and d.otype2=="TAU") or (d.otype1=="TAU" and d.otype2=="EM") else 0 )
            alg.addgeneric('ApplyDR', 0)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('DisambDRSqrMin', d.disamb*d.disamb)
            tm.registerTopoAlgo(alg)
        

        # DISAMB 3 lists
        algoList = [
            { "disamb": 1, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET}, #1DISAMB-EM15his2-TAU12abi-J25ab
            { "disamb": 1, "otype1" : "TAU", "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectTAU, "inputwidth3": HW.OutputWidthSelectJET}, #1DISAMB-TAU20abi-TAU12abi-J25ab
        ]
        for x in algoList:     
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1.replace('shi','his') + (str(d.nleading1) if d.olist1.find('s')>=0 else ""))
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2.replace('shi','his') + (str(d.nleading2) if d.olist2.find('s')>=0 else ""))
            obj3 = "-%s%s%s" % (d.otype3, str(d.ocut3), d.olist3)
            toponame = "%sDISAMB-%s%s%s"  % ( d.disamb if d.disamb>0 else "", obj1, obj2, obj3)
            log.debug("Define %s", toponame)            
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2, d.otype3 + d.olist3]
            alg = AlgConf.DisambiguationIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('InputWidth3', d.inputwidth3)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('MaxTob3', d.nleading3)
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('ApplyDR', 0)
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('MinET3', d.ocut3, 0)
            alg.addvariable('DisambDRSqr', d.disamb*d.disamb, 0) # DisambDR
            tm.registerTopoAlgo(alg)         


        # DISAMB 3 lists with DR cut to 2nd and 3rd lists
        algolist = [
            { "disamb": 1, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET,
              "drcutmin": 0, "drcutmax": 28}, #1DISAMB-J25ab-0DR28-EM15his2-TAU12abi
            { "disamb": 1, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET,
              "drcutmin": 0, "drcutmax": 28}, # 1DISAMB-J25ab-0DR28-TAU20abi-TAU12abi
            { "disamb": 1, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "J", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET,
              "drcutmin": 0, "drcutmax": 25}, # 1DISAMB-J25ab-0DR25-TAU20abi-TAU12abi
        ]
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "-%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1.replace('shi','his') + (str(d.nleading1) if d.olist1.find('s')>=0 else ""))
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2.replace('shi','his') + (str(d.nleading2) if d.olist2.find('s')>=0 else ""))
            obj3 = "%s%s%s" % (d.otype3, str(d.ocut3), d.olist3)
            toponame = "%sDISAMB-%s-%dDR%d%s%s"  % ( str(d.disamb) if d.disamb>0 else "", obj3, d.drcutmin, d.drcutmax, obj1, obj2)
            log.debug("Define %s", toponame)            
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2, d.otype3 + d.olist3]
            alg = AlgConf.DisambiguationDRIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('InputWidth3', d.inputwidth3)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('MaxTob3', d.nleading3)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('MinET3', d.ocut3, 0)
            alg.addvariable('DisambDRSqrMin', d.drcutmin*d.drcutmin, 0)
            alg.addvariable('DisambDRSqrMax', d.drcutmax*d.drcutmax, 0)
            alg.addvariable('DisambDRSqr', d.disamb*d.disamb, 0)
            tm.registerTopoAlgo(alg)            


        # LAR  0<eta<1.4 and 9/16pi<phi<11/16pi for FE crate IO6 
        algoList = [
            {"minEta": 1, "maxEta": 15, "minPhi": 17, "maxPhi": 23, "otype" : "EM", "ocut" : 20,  "olist" : "shi", "inputwidth": HW.OutputWidthSortEM}, #LAR-EM20shi1
            {"minEta": 1, "maxEta": 14, "minPhi": 17, "maxPhi": 23, "otype" : "J", "ocut" : 100,  "olist" : "s", "inputwidth": HW.OutputWidthSortJET},  #LAR-J100s1
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "LAR-%s%s%s1"  % ( d.otype, str(d.ocut), d.olist )
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.EtaPhiWindow( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', 0)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(d.ocut))
            alg.addvariable('EtaMin', d.minEta)
            alg.addvariable('EtaMax', d.maxEta)
            alg.addvariable('PhiMin', d.minPhi)
            alg.addvariable('PhiMax', d.maxPhi)
            tm.registerTopoAlgo(alg)


        xemap = [
            {"etcut": 0, "Threlist": [ 40, 50, 55, 60, 65, 75 ]}
        ]
        for x in xemap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            log.debug("Define %s", toponame)
            inputList = ['XENoSort', 'AJall']
            toponames=[]
            for minxe in d.Threlist:
                toponames.append("KF-XE%s-AJall"  % (minxe))            
            alg = AlgConf.KalmanMETCorrection( name = "KF-XE-AJall", inputs = inputList, outputs = toponames, algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('NumResultBits', len(toponames))
            alg.addvariable('MinET', 0)
            for bitid,minxe in enumerate(d.Threlist):
                alg.addvariable('KFXE', str(minxe), bitid)            
            tm.registerTopoAlgo(alg)


        # W T&P: MINDPHI(J, XE0), (EM, XE0)
        algoList = [
            {"minDPhi": 15, "otype" : "EM",  "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
            {"minDPhi": 15, "otype" : "EM",  "ocut" : 15, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%02dMINDPHI-%s%s%s%s-XE0"  % (d.minDPhi, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', 1) 
            alg.addgeneric('MaxTob1', d.nleading)
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut)
            alg.addvariable('MinET2', 0)
            alg.addvariable('DeltaPhiMin', d.minDPhi, 0)
            tm.registerTopoAlgo(alg)


        # W T&P MT
        algoList = [
            {"minMT": 35, "otype" : "EM", "ocut" : 12, "olist" : "s", "nleading" : 6, "inputwidth": HW.OutputWidthSortEM},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iMT-%s%s%s%s-XE0"  % (d.minMT, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.TransverseMassInclusive1( name = toponame, inputs = [ inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob', str(d.nleading))
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', str(d.ocut))
            alg.addvariable('MinET2', 0)
            alg.addvariable('MinMTSqr', d.minMT*d.minMT)
            tm.registerTopoAlgo(alg)


        # DISAMB 2 lists with DR cut between objects in two lists
        algoList=[
            { "disamb": 0, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "drcutmin": 0, "drcutmax": 28},          
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "-%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1.replace('shi','his') + (str(d.nleading1) if d.olist1.find('s')>=0 else ""))
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2.replace('shi','his') + (str(d.nleading2) if d.olist2.find('s')>=0 else ""))
            toponame = "%sDISAMB-%dDR%d%s%s"  % ( str(d.disamb) if d.disamb>0 else "", d.drcutmin, d.drcutmax, obj1, obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DisambiguationDRIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('DisambDRSqrMin', d.drcutmin*d.drcutmin, 0)
            alg.addvariable('DisambDRSqrMax', d.drcutmax*d.drcutmax, 0)
            tm.registerTopoAlgo(alg)   


        # ZH Trigger
        algoList = [
            {"minDPhi": 10, "otype" : "AJ", "ocut" : 20,  "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET},
        ]
        for x in algoList:            
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iMINDPHI-%s%s%s%s-XE50"  % (d.minDPhi, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', 1)  
            alg.addgeneric('MaxTob1', d.nleading)
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut)
            alg.addvariable('MinET2', 50)
            alg.addvariable('DeltaPhiMin', d.minDPhi, 0)
            tm.registerTopoAlgo(alg)
                
        # LATE MUON : LATE-MU10s1
        for x in [     
            #{"otype" : "LATE-MU", "ocut" : 10, "inputwidth": HW.OutputWidthSortMU},
            {"otype" : "LATE-MU", "ocut" : 10, "inputwidth": HW.NumberOfDelayedMuons},
            ]:

            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%s%ss1"  % ( d.otype, str(d.ocut) )
            log.debug("Define %s", toponame)
            inputList = 'LMUs'
            alg = AlgConf.EtCut( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(d.ocut))
            tm.registerTopoAlgo(alg)
            

        # (ATR-12748) fat jet trigger with Simple Cone algo
        algoList = [
            {"minHT": 111, "otype" : "CJ", "ocut" : 15, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "oeta" : 26}, #SC111-CJ15ab.ETA26
            {"minHT": 85, "otype" : "CJ", "ocut" : 15, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "oeta" : 26},  #SC85-CJ15ab.ETA26
        ]
        for x in algoList:            
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "SC%d-%s%s%s%spETA%s" % (d.minHT, d.otype, str(d.ocut), d.olist, str(d.nleading) if d.olist=="s" else "", str(d.oeta))
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.SimpleCone( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addvariable('MinET', d.ocut)
            alg.addvariable('MinSumET', d.minHT)
            alg.addvariable('MaxRSqr', 10*10)                        
            tm.registerTopoAlgo(alg)  


        # DISAMB-INVM
        # DISAMB-30INVM-EM20his2-TAU12ab
        algoList = [
            { "disamb": 0, "minInvm": 30, "maxInvm": 9999,"otype1" : "EM",  "ocut1": 20, "olist1": "shi","nleading1": 2,
              "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12, "olist2": "ab", "nleading2": HW.OutputWidthSelectTAU,
              "inputwidth2": HW.OutputWidthSelectTAU},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1.replace('shi','his') + (str(d.nleading1) if d.olist1.find('s')>=0 else ""))
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2)
            toponame = "%sDISAMB-%iINVM%s-%s%s"  % ( d.disamb if d.disamb>0 else "", d.minInvm, str(d.maxInvm) if d.maxInvm<9999 else "", obj1, obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            #alg = AlgConf.DisambiguationInvariantMass2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1
            alg = AlgConf.DisambiguationInvmIncl2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm)
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm)
            tm.registerTopoAlgo(alg)


        # 400INVM9999-AJ30s6.ETA31-AJ20s6p31ETA49
        algoList = [
            {  "minInvm": 400, "maxInvm": 9999, "otype1" : "AJ", "ocut1": 30, "olist1" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET,
               "otype2" : "AJ", "ocut2": 20, "olist2" : "s", "nleading2" : 6, "inputwidth2": HW.OutputWidthSortJET, "applyEtaCut": 1,
               "minEta1": 0 ,"maxEta1": 31 , "minEta2": 31 ,"maxEta2": 49 , },
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%sp%sETA%i"  % (d.otype1, str(d.ocut1), d.olist1 + (str(d.nleading1) if d.olist1.find('s')>=0 else ""),str(d.minEta1) if d.minEta1>0 else "", d.maxEta1)
            obj2 = "-%s%s%sp%sETA%i"  % (d.otype2, str(d.ocut2), d.olist2 + (str(d.nleading2) if d.olist2.find('s')>=0 else ""),str(d.minEta2) if d.minEta2>0 else "", d.maxEta2)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            toponame = "%iINVM%i-%s%s"   % (d.minInvm, d.maxInvm, obj1, obj2)
            alg = AlgConf.InvariantMassInclusive2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            if (d.applyEtaCut>0):
                alg.addgeneric('ApplyEtaCut', d.applyEtaCut)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm )
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm )
            if (d.applyEtaCut>0):
                alg.addvariable('MinEta1', d.minEta1)
                alg.addvariable('MaxEta1', d.maxEta1)
                alg.addvariable('MinEta2', d.minEta2)
                alg.addvariable('MaxEta2', d.maxEta2)
            tm.registerTopoAlgo(alg)


        # FTK algorithms
        # they are not needed in Run 3, but leave in for consistency of the legacy topo menu
        algoList = [
            {"minEta": -16, "maxEta": 16, "minPhi": 15, "maxPhi": 29, "otype" : "EM", "ocut" : 20, "inputwidth": HW.OutputWidthSortEM},
            {"minEta": -16, "maxEta": 16, "minPhi": 15, "maxPhi": 29, "otype" : "J", "ocut" : 100, "inputwidth": HW.OutputWidthSortJET},
            {"minEta": -16, "maxEta": 16, "minPhi": 15, "maxPhi": 29, "otype" : "MU", "ocut" : 10, "inputwidth": HW.OutputWidthSortMU},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "FTK-%s%ss1"  % ( d.otype, str(d.ocut) if not d.otype=="EM" else "20" )  # noqa: F821
            log.debug("Define %s", toponame)
            inputList = d.otype + 's'  # noqa: F821
            alg = AlgConf.EtaPhiWindow( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)  # noqa: F821
            alg.addgeneric('MaxTob', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(d.ocut)) # noqa: F821
            alg.addvariable('EtaMin', d.minEta) # noqa: F821
            alg.addvariable('EtaMax', d.maxEta) # noqa: F821
            alg.addvariable('PhiMin', d.minPhi) # noqa: F821
            alg.addvariable('PhiMax', d.maxPhi) # noqa: F821
            tm.registerTopoAlgo(alg)


        # LAR  ZEE
        algoList = [
            {"otype" : "EM", "ocut1" : 20,  "ocut2" : 20, "olist" : "shi", "nleading1" : 2, "minInvm" : 60, "maxInvm" : 100, "inputwidth": HW.OutputWidthSortEM},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = 'ZEE-EM20shi2'
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', d.nleading1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('MinMSqr', (d.minInvm * _emscale_for_decision)*(d.minInvm * _emscale_for_decision))
            alg.addvariable('MaxMSqr', (d.maxInvm * _emscale_for_decision)*(d.maxInvm * _emscale_for_decision))
            tm.registerTopoAlgo(alg)


        #  0INVM9-EM7ab-EMab 
        algoList = [
            {"minInvm" : 0, "maxInvm": 9, "otype" : "EM", "ocut1" : 7, "olist" : "ab", "inputwidth": HW.OutputWidthSelectEM, "ocut2" : 0},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = d.otype + d.olist
            toponame = "%iINVM%i-%s%s%s-%s%s"  % (d.minInvm, d.maxInvm,
                                                  d.otype, str(d.ocut1) , d.olist,
                                                  d.otype, d.olist)
            alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', HW.OutputWidthSelectEM)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)
            alg.addvariable('MinET2', d.ocut2)
            alg.addvariable('MinMSqr', (d.minInvm * _emscale_for_decision)*(d.minInvm * _emscale_for_decision))
            alg.addvariable('MaxMSqr', (d.maxInvm * _emscale_for_decision)*(d.maxInvm * _emscale_for_decision))
            tm.registerTopoAlgo(alg)


        # added for b-phys, 0DR03-EM7ab-CJ15ab
        algoList = [
            {"minDr": 0, "maxDr": 3, "otype1" : "EM" ,"ocut1": 7,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"} 
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (d.minDr, d.maxDr, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2), d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectEM)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', d.ocut1, 0)
            alg.addvariable('MinET2', d.ocut2, 0)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr, 0)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr, 0)
            tm.registerTopoAlgo(alg)


        # VBF items INVM_NFF
        algoList = [
            { "algoname": 'INVM_NFF', "Threlist": [ 600, 500, 400, 200 ], "maxInvm": 9999, "otype1" : "J", "ocut1" : 30, "olist1" : "s", "nleading1" : 6, 
              "inputwidth": HW.OutputWidthSortJET,  "otype2" : "AJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }
        ]
        for x in algoList:            
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]
            toponames=[]
            for minInvm in d.Threlist:
                toponame = "%iINVM%i-%s%s%s%s-%s%s%s%s"  % (minInvm, d.maxInvm,
                                                            d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                            d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else "")
                toponames.append(toponame)
            alg = AlgConf.InvariantMassInclusive2( name = d.algoname, inputs = inputList, outputs = toponames, algoId = 93)
            currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', d.inputwidth)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, minInvm in enumerate(d.Threlist):
                alg.addvariable('MinET1', d.ocut1, bitid)
                alg.addvariable('MinET2', d.ocut2, bitid)
                alg.addvariable('MinMSqr', minInvm*minInvm , bitid)
                alg.addvariable('MaxMSqr', d.maxInvm *d.maxInvm , bitid)
            tm.registerTopoAlgo(alg)


        # Axion 2EM DPHI  
        # 27DPHI32-EMs1-EMs6
        algoList = [
            {"minDphi": 27,  "maxDphi": 32, "otype" : "EM",  "ocut1" : 0,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortEM, "ocut2" : 0, "nleading2": 6},
        ]
        for x in algoList:                 
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDPHI%i-%s%s%s%s-%s%s%s%s"  % (d.minDphi, d.maxDphi,
                                                        d.otype, str(d.ocut1) if d.ocut1 > 0 else "", d.olist, str(d.nleading1) if d.olist=="s" else "",
                                                        d.otype, str(d.ocut2) if d.ocut2 > 0 else "", d.olist, str(d.nleading2) if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.DeltaPhiIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId )
            currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('MaxTob', d.nleading2)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', d.ocut1 if d.ocut1 > 0 else 3, 0)
            alg.addvariable('MinET2', d.ocut2 if d.ocut2 > 0 else 3, 0)
            alg.addvariable('MinDeltaPhi', d.minDphi, 0)
            alg.addvariable('MaxDeltaPhi', d.maxDphi, 0)
            tm.registerTopoAlgo(alg)


        #ATR-20174, L1BPH-8M15-2MU4-BO
        toponame = "8INVM15-2CMU4ab"
        log.debug("Define %s", toponame)
        inputList = ['CMUab']
        alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr', 8*8)
        alg.addvariable('MaxMSqr', 15*15)
        alg.addvariable('MinET1', 4)
        alg.addvariable('MinET2', 4)
        tm.registerTopoAlgo(alg)

        

