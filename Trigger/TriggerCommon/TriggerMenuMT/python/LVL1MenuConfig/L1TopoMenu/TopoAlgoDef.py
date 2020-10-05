# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Disable flake8 checking due to the use of 'exec':
# flake8: noqa
#

from AthenaCommon.Logging import logging
log = logging.getLogger('TriggerMenuMT.L1Topo.TopoAlgoDef')

from TriggerJobOpts.TriggerFlags import TriggerFlags

# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW


class TopoAlgoDef(object):
    
    @staticmethod
    def registerTopoAlgos(tm):
        currentAlgoId = 0
        
        _etamax = 49
        _minet = 0
 
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
        for jet_type in ['J', 'CJ', 'FJ']:
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

            alg = AlgConf.JetSelect( name = jet_type+'ab', inputs = 'JetTobArray', outputs = jet_type+'ab', algoId = currentAlgoId ); currentAlgoId += 1

            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectJET )            
            alg.addgeneric('OutputWidth', HW.OutputWidthSelectJET)
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinET', _minet)  
            alg.addvariable('MinEta', _mineta)
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


        #input list needed for ATR-18824
        alg = AlgConf.JetSort( name = 'FJjs23ETA49', inputs = 'JetTobArray', outputs = 'FJjs23ETA49', algoId = currentAlgoId); currentAlgoId += 1

        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)
        alg.addvariable('MinEta', 23)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoEtaCut', 1)
        tm.registerAlgo(alg)
            
        
        alg = AlgConf.JetSort( name = 'CJsETA21', inputs = 'JetTobArray', outputs = 'CJsETA21', algoId = currentAlgoId); currentAlgoId += 1
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', HW.DefaultJetSize.value)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 21)
        tm.registerAlgo(alg)

        # Sorted J lists:
        for jet_type in ['AJ', 'FJ']:
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
                
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s', algoId = currentAlgoId ); currentAlgoId += 1

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
            tm.registerAlgo(alg) 

        for jet_type in ['J','CJ']:
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


        alg = AlgConf.MuonSort( name = 'MUs', inputs = 'MuonTobArray', outputs = 'MUs', algoId = currentAlgoId ); currentAlgoId += 1

        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortMU)
#        alg.addvariable('MinET', 4)
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
        alg.addvariable('MinET', 5)
        alg.addvariable('IsoMask', 0)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', _etamax)
        alg.addgeneric('DoIsoCut', 0)
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

        #########
        ##
        ## Decision algorithms
        ##
        #########
        currentAlgoId = 0
        
        # VBF items
        for x in [
                {"algoname": 'INVM_AJ_HighMass', "Threlist": [ 700], "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6},
                {"algoname": 'INVM_AJ_LowMass',  "Threlist": [ 300], "maxInvm": 9999, "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6}
        ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            inputList = d.otype + d.olist
            toponames=[]
            for minInvm in d.Threlist:  
                toponame = "%iINVM%i-%s%s%s%s-%s%s%s%s"  % (minInvm, d.maxInvm,
                                                            d.otype, str(d.ocut1) , d.olist, str(d.nleading1) if d.olist=="s" else "",     
                                                            d.otype, str(d.ocut2) , d.olist, str(d.nleading2) if d.olist=="s" else "")     
                toponames.append(toponame)
            alg = AlgConf.InvariantMassInclusive1( name = d.algoname, inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1  
            alg.addgeneric('InputWidth', d.inputwidth1)      
            alg.addgeneric('MaxTob', d.nleading1)            
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, minInvm in enumerate(d.Threlist):     
                alg.addvariable('MinET1', d.ocut1, bitid)    
                alg.addvariable('MinET2', d.ocut2, bitid)    
                alg.addvariable('MinMSqr', minInvm * minInvm, bitid)  
                alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm, bitid)  
            tm.registerAlgo(alg)


        # dimu DR items
        listofalgos=[
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR15-2MU6ab        
        ]
        for x in listofalgos:
            class d : pass
            for k in x:
                setattr (d, k, x[k])                
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist)    
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist)                                    
            toponame = "%iDR%i-%s%s%s"  % (d.minDr, d.maxDr, "ONEBARREL-" if d.onebarrel==1 else "", obj1, "" if d.mult>1 else obj2)  # noqa: F821
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist, d.otype2 + d.olist]    
            algoname = AlgConf.DeltaRSqrIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaRSqrIncl2         
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1 
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
            tm.registerAlgo(alg)

            
        # deta-dphi with ab+ab
        for x in [
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "MU", "ocut2": 4, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-MU6ab-MU4ab
        {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 6, "olist2": "", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-2MU6ab
        {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 4, "olist2": "", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-2MU4ab
            ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s%s" % ((str(d.mult) if d.mult>1 else ""), d.otype1, str(d.ocut1), d.olist1)  
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2)  
            toponame = "%sDETA%s-%sDPHI%s-%s%s"  % (d.minDeta, d.maxDeta, d.minDphi, d.maxDphi, obj1, "" if d.mult>1 else obj2)            
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1] if (d.mult>1 or d.otype1==d.otype2) else [d.otype1 + d.olist1, d.otype2 + d.olist2]    
            algoname = AlgConf.DeltaEtaPhiIncl1 if (d.mult>1 or d.otype1==d.otype2) else AlgConf.DeltaEtaPhiIncl2        
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)                        
            if (d.mult>1 or d.otype1==d.otype2):  # noqa: F821
                alg.addgeneric('InputWidth', d.nleading1)  # noqa: F821
                alg.addgeneric('MaxTob', d.nleading1)      # noqa: F821
                alg.addvariable('MinET1', d.ocut1)         # noqa: F821
                alg.addvariable('MinET2', d.ocut2)         # noqa: F821
                alg.addvariable('MinDeltaEta', d.minDeta)  # noqa: F821
                alg.addvariable('MaxDeltaEta', d.maxDeta)  # noqa: F821
                alg.addvariable('MinDeltaPhi', d.minDphi)  # noqa: F821
                alg.addvariable('MaxDeltaPhi', d.maxDphi)  # noqa: F821
            else:
                alg.addgeneric('InputWidth1', d.nleading1) # noqa: F821
                alg.addgeneric('InputWidth2', d.nleading2) # noqa: F821
                alg.addgeneric('MaxTob1', d.nleading1)     # noqa: F821
                alg.addgeneric('MaxTob2', d.nleading2)     # noqa: F821
                alg.addvariable('DeltaEtaMin', d.minDeta)  # noqa: F821
                alg.addvariable('DeltaEtaMax', d.maxDeta)  # noqa: F821
                alg.addvariable('DeltaPhiMin', d.minDphi)  # noqa: F821
                alg.addvariable('DeltaPhiMax', d.maxDphi)  # noqa: F821
                alg.addvariable('MinET1', d.ocut1)         # noqa: F821
                alg.addvariable('MinET2', d.ocut2)         # noqa: F821
            tm.registerAlgo(alg)


        algolist=[
            {"minDr": 0, "maxDr": 28, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,"otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # 0DR28-TAU20abi-TAU12abi
            {"minDr": 0, "maxDr": 25, "otype1" : "TAU" ,"ocut1": 20, "olist1" : "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,"otype2" : "TAU", "ocut2": 12, "olist2" : "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, #        0DR25-TAU20abi-TAU12abi
        ]
        for x in algolist:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s" % (d.otype1, str(d.ocut1), d.olist1)       
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2)      
            toponame = "%iDR%i-%s%s"  % (d.minDr, d.maxDr, obj1, obj2) 
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1] if d.otype1==d.otype2 else [d.otype1 + d.olist1, d.otype2 + d.olist2] 
            algoname = AlgConf.DeltaRSqrIncl1 if d.otype1==d.otype2 else AlgConf.DeltaRSqrIncl2         
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1                            
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
            tm.registerAlgo(alg)        

            
        # (ATR-8194) L1Topo HT Trigger
        algoList = [
            {"minHT": 150, "otype" : "J", "ocut" : 20, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 31}, #HT150-J20s5.ETA31
            {"minHT": 190, "otype" : "J", "ocut" : 15, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 21}, #HT190-J15s5.ETA21
            ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
             
                
            toponame = "HT%d-%s%s%s%s.ETA%s" % (d.minHT, d.otype, str(d.ocut), d.olist, str(d.nleading) if d.olist=="s" else "", str(d.oeta)) 
            
            log.info("Define %s" % toponame)
            
            inputList = d.otype + d.olist 

            alg = AlgConf.JetHT( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId ); currentAlgoId += 1


            alg.addgeneric('InputWidth', d.inputwidth)   # noqa: F821
            alg.addgeneric('MaxTob', d.nleading)         # noqa: F821
            alg.addgeneric('NumRegisters', 2 if d.olist=="all" else 0) # noqa: F821
            alg.addgeneric('NumResultBits', 1) # noqa: F821
            alg.addvariable('MinET', d.ocut)     # noqa: F821                    
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', d.oeta)    # noqa: F821
            alg.addvariable('MinHt', d.minHT)    # noqa: F821
            tm.registerAlgo(alg)


            
        # INVM_EM for Jpsi
        invm_map = {"algoname": 'INVM_EMs6' , "ocutlist": [ 7, 12 ], "minInvm": 1, "maxInvm": 5, "otype" : "EM", "olist" : "s", "nleading" : 1, "inputwidth": HW.OutputWidthSortEM}

        for x in [ invm_map,
            ]:

            class d : pass
            for k in x:
                setattr (d, k, x[k])


            inputList = d.otype + d.olist  # noqa: F821
            toponames=[]

            for ocut in d.ocutlist:      # noqa: F821
                toponame = "%iINVM%i-%s%s%s%s-EMs6"   % (d.minInvm, d.maxInvm, d.otype, str(ocut) if ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")          # noqa: F821
                toponames.append(toponame)

            alg = AlgConf.InvariantMassInclusive2( name = d.algoname, inputs = [inputList, 'EMs'], outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1     # noqa: F821


            alg.addgeneric('InputWidth1', d.inputwidth) # noqa: F821
            #alg.addgeneric('InputWidth2', HW.InputWidthEM)
            alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob1', d.nleading)       # noqa: F821
            #alg.addgeneric('MaxTob2', HW.InputWidthEM)
            alg.addgeneric('MaxTob2', HW.OutputWidthSortEM)
            alg.addgeneric('NumResultBits', len(toponames))

            for bitid, ocut in enumerate(d.ocutlist): # noqa: F821
                alg.addvariable('MinET1', ocut, bitid)
                alg.addvariable('MinET2', 0, bitid)
                alg.addvariable('MinMSqr', (d.minInvm * _emscale_for_decision)*(d.minInvm * _emscale_for_decision), bitid)        # noqa: F821
                alg.addvariable('MaxMSqr', (d.maxInvm * _emscale_for_decision)*(d.maxInvm * _emscale_for_decision), bitid)        # noqa: F821

            tm.registerAlgo(alg)
            
            
        # VBF deta    

        algoList = [
            {"minDeta": 0,  "maxDeta": 20, "otype" : "J",  "ocut1" : 50,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 0, "nleading2": 2}, #0DETA20-J50s1-Js2
        ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDETA%i-%s%s%s%s-%s%s%s%s"  % (d.minDeta, d.maxDeta,  # noqa: F821
                                                        d.otype, str(d.ocut1) if d.ocut1 > 0 else "", d.olist, str(d.nleading1) if d.olist=="s" else "", # noqa: F821
                                                        d.otype, str(d.ocut2) if d.ocut2 > 0 else "", d.olist, str(d.nleading2) if d.olist=="s" else "") # noqa: F821
            log.info("Define %s" % toponame)
            inputList = d.otype + d.olist # noqa: F821
            alg = AlgConf.DeltaEtaIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            log.info("Current algo ID %i" % currentAlgoId )
            alg.addgeneric('InputWidth', d.inputwidth1)  # noqa: F821
            alg.addgeneric('MaxTob', d.nleading2)        # noqa: F821
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', str(d.ocut1), 0)   # noqa: F821
            alg.addvariable('MinET2', str(d.ocut2), 0)   # noqa: F821
            alg.addvariable('MinDeltaEta', d.minDeta, 0) # noqa: F821
            alg.addvariable('MaxDeltaEta', d.maxDeta, 0) # noqa: F821
            tm.registerAlgo(alg)



            
        # ZH Trigger
        supportedalgolist = [
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 30 }, #10MINDPHI-J20s2-XE30
            {"minDPhi": 10, "otype" : "J", "ocut" : 20, "olist" : "s", "nleading" : 2, "inputwidth": HW.OutputWidthSortJET, "ocut2": 50 }, #10MINDPHI-J20s2-XE50
        ] 
        for x in supportedalgolist:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iMINDPHI-%s%s%s%s-XE%i"  % (d.minDPhi, d.otype, str(d.ocut) if d.ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "",d.ocut2) # noqa: F821
            log.info("Define %s" % toponame)
            inputList = d.otype + d.olist # noqa: F821
            alg = AlgConf.MinDeltaPhiIncl2( name = toponame, inputs = [inputList, 'XE'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
            log.info("Current algo ID %i" % currentAlgoId )
            alg.addgeneric('InputWidth1', d.inputwidth) # noqa: F821
            alg.addgeneric('InputWidth2', 1)  
            alg.addgeneric('MaxTob1', d.nleading)       # noqa: F821
            alg.addgeneric('MaxTob2', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut)           # noqa: F821
            alg.addvariable('MinET2', d.ocut2)          # noqa: F821
            alg.addvariable('DeltaPhiMin', d.minDPhi, 0)# noqa: F821
            tm.registerAlgo(alg)

            
        # LFV DETA ATR-14282 
        algoList = [
            {"minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DETA04-EM8abi-MU10ab
            {"minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DETA04-EM15abi-MUab
        ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%sDETA%s-%s%s%s-%s%s%s"  % (d.minDeta, d.maxDeta, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2) # noqa: F821
            log.info("Define %s at %i", toponame, currentAlgoId)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2] # noqa: F821
            alg = AlgConf.DeltaEtaIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('InputWidth1', d.nleading1)   # noqa: F821
            alg.addgeneric('InputWidth2', d.nleading2)   # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2)       # noqa: F821
            alg.addvariable('MinET1', d.ocut1, 0)        # noqa: F821
            alg.addvariable('MinET2', d.ocut2, 0)        # noqa: F821
            alg.addvariable('MinDeltaEta', d.minDeta, 0) # noqa: F821
            alg.addvariable('MaxDeltaEta', d.maxDeta, 0) # noqa: F821
            tm.registerAlgo(alg)

  
        #EMU dphi
        algoList = [
            {"minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 8, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DPHI03-EM8abi-MU10ab
            {"minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "EM", "ocut1": 15, "olist1" : "abi", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DPHI03-EM15abi-MUab
        ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%sDPHI%s-%s%s%s-%s%s%s"  % (d.minDphi, d.maxDphi, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2) # noqa: F821
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2] # noqa: F821
            alg = AlgConf.DeltaPhiIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('InputWidth1', d.nleading1)   # noqa: F821
            alg.addgeneric('InputWidth2', d.nleading2)   # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2)       # noqa: F821
            alg.addvariable('MinET1', d.ocut1, 0)        # noqa: F821
            alg.addvariable('MinET2', d.ocut2, 0)        # noqa: F821
            alg.addvariable('MinDeltaPhi', d.minDphi, 0) # noqa: F821
            alg.addvariable('MaxDeltaPhi', d.maxDphi, 0) # noqa: F821
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
        toponame = 'NOT-0MATCH-TAU30si1-EMall'
        alg = AlgConf.NotMatch( name = toponame, inputs = [ 'TAUsi', 'EMall'], outputs = [ toponame ], algoId = currentAlgoId ); currentAlgoId += 1
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
        tm.registerAlgo(alg)        

        # MULT-BIT
        for x in [
            {"otype1" : "CMU" ,"ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU}, #MULT-CMU4ab
            {"otype1" : "CMU" ,"ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU}, #MULT-CMU6ab
            ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])

            
                  
            toponame = "MULT-%s%s%s" % (d.otype1, str(d.ocut1), d.olist1) # noqa: F821
            toponames = [toponame+"[0]", toponame+"[1]"]
            log.info("Define %s" % toponames)
            
            inputList = [d.otype1 + d.olist1]  # noqa: F821
            alg = AlgConf.Multiplicity( name = toponame,  inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1
            
            alg.addgeneric('InputWidth', d.inputwidth1) # noqa: F821
            alg.addgeneric('NumResultBits', 2)
            alg.addvariable('MinET', d.ocut1-1) # for MU threshold -1   # noqa: F821
            tm.registerAlgo(alg)        


        # DISAMB 2 lists
        algolist=[
            {"disamb": 2, "otype1" : "TAU", "ocut1": 12, "olist1" : "abi", "nleading1": HW.OutputWidthSelectTAU, "otype2" : "J", "ocut2": 25, "olist2": "ab", "nleading2": HW.OutputWidthSelectJET}, #2DISAMB-TAU12abi-J25ab
        ]            
        for x in algolist :

            class d : pass
            for k in x:
                setattr (d, k, x[k])

     
 
            obj1 = "%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1) # noqa: F821
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2) # noqa: F821
            toponame = "%sDISAMB-%s%s"  % ( d.disamb if d.disamb>0 else "", obj1, obj2) # noqa: F821
            
            log.info("Define %s" % toponame)
            
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2] # noqa: F821
            alg = AlgConf.DisambiguationIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.nleading1 if d.olist1.find("ab")>=0 else -1000) # noqa: F821
            alg.addgeneric('InputWidth2', d.nleading2 if d.olist2.find("ab")>=0 else -1000)  # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('ClusterOnly', 1 if (d.otype1=="EM" and d.otype2=="TAU") or (d.otype1=="TAU" and d.otype2=="EM") else 0 ) # noqa: F821
            alg.addgeneric('ApplyDR', 0)
            alg.addvariable('MinET1', d.ocut1) # noqa: F821
            alg.addvariable('MinET2', d.ocut2) # noqa: F821
            alg.addvariable('DisambDRSqrMin', d.disamb*d.disamb) # noqa: F821
            tm.registerAlgo(alg)
        

        # DISAMB 3 lists with DR cut to 2nd and 3rd lists
        algolist=[
            { "disamb": 2, "otype1" : "EM",  "ocut1": 15, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12,
              "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab",
              "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET, "drcutmin": 0, "drcutmax": 28}, #2DISAMB-J25ab-0DR28-EM15his2-TAU12abi
            { "disamb": 2, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J", "ocut3": 25, "olist3": "ab", 
              "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET, "drcutmin": 0, "drcutmax": 28}, # 2DISAMB-J25ab-0DR28-TAU20abi-TAU12abi
            { "disamb": 2, "otype1" : "TAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "TAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU, "otype3" : "J",
              "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET, "drcutmin": 0, "drcutmax": 25 }, # 2DISAMB-J25ab-0DR25-TAU20abi-TAU12abi
        ]
        for x in algolist:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "-%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1.replace('shi','his') + (str(d.nleading1) if d.olist1.find('s')>=0 else ""))  # noqa: F821
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2.replace('shi','his') + (str(d.nleading2) if d.olist2.find('s')>=0 else "")) # noqa: F821
            obj3 = "%s%s%s" % (d.otype3, str(d.ocut3), d.olist3) # noqa: F821
            toponame = "%sDISAMB-%s-%dDR%d%s%s"  % ( str(d.disamb) if d.disamb>0 else "", obj3, d.drcutmin, d.drcutmax, obj1, obj2)  # noqa: F821
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2, d.otype3 + d.olist3]  # noqa: F821
            alg = AlgConf.DisambiguationDRIncl3( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', d.inputwidth2) # noqa: F821
            alg.addgeneric('InputWidth3', d.inputwidth3) # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2) # noqa: F821
            alg.addgeneric('MaxTob3', d.nleading3) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', d.ocut2, 0) # noqa: F821
            alg.addvariable('MinET3', d.ocut3, 0) # noqa: F821
            alg.addvariable('DisambDRSqrMin', d.drcutmin*d.drcutmin, 0) # noqa: F821
            alg.addvariable('DisambDRSqrMax', d.drcutmax*d.drcutmax, 0) # noqa: F821
            alg.addvariable('DisambDRSqr', d.disamb*d.disamb, 0) # noqa: F821
            tm.registerAlgo(alg)            


        # KF-XE-AJall
        xemap = [{"etcut": 0, "Threlist": [ 40, 50, 55, 60, 65, 75 ]}]
        for x in xemap:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            log.info("Define %s" % toponame)
            inputList = ['XENoSort', 'AJall']
            toponames=[]
            for minxe in d.Threlist:  # noqa: F821
                toponames.append("KF-XE%s-AJall"  % (minxe))
            alg = AlgConf.KalmanMETCorrection( name = "KF-XE-AJall", inputs = inputList, outputs = toponames, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('NumResultBits', len(toponames))
            alg.addvariable('MinET', 0)
            for bitid,minxe in enumerate(d.Threlist):  # noqa: F821
                alg.addvariable('KFXE', str(minxe), bitid)
            tm.registerAlgo(alg)


        # LATE MUON : LATE-MU10s1
        for x in [     
             {"otype" : "LATE-MU", "ocut" : 10, "inputwidth": HW.NumberOfDelayedMuons},
            ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%s%ss1"  % ( d.otype, str(d.ocut) ) # noqa: F821
            log.info("Define %s" % toponame)
            inputList = 'LMUs'
            alg = AlgConf.EtCut( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth)  # noqa: F821
            alg.addgeneric('MaxTob', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(d.ocut)) # noqa: F821
            tm.registerAlgo(alg)
            

        # (ATR-12748) fat jet trigger with Simple Cone algo
        algoList = [
            {"minHT": 111, "otype" : "CJ", "ocut" : 15, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "oeta" : 26}, #SC111-CJ15ab.ETA26
        ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "SC%d-%s%s%s%s.ETA%s" % (d.minHT, d.otype, str(d.ocut), d.olist, str(d.nleading) if d.olist=="s" else "", str(d.oeta)) # noqa: F821
            log.info("Define %s" % toponame)
            inputList = d.otype + d.olist # noqa: F821
            alg = AlgConf.SimpleCone( name = toponame, inputs = inputList, outputs = [toponame], algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth) # noqa: F821
            alg.addvariable('MinET', d.ocut)           # noqa: F821       
            alg.addvariable('MinSumET', d.minHT)       # noqa: F821
            alg.addvariable('MaxRSqr', 10*10)                        
            tm.registerAlgo(alg)  


        # DISAMB-INVM
        for x in [
                {"disamb": 0, "minInvm": 30, "maxInvm": 9999,"otype1" : "EM",  "ocut1": 20, "olist1": "shi","nleading1": 2, "inputwidth1": HW.OutputWidthSortEM, "otype2" : "TAU", "ocut2": 12, "olist2": "ab", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # DISAMB-30INVM-EM20his2-TAU12ab
        ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s"  % (d.otype1, str(d.ocut1), d.olist1.replace('shi','his') + (str(d.nleading1) if d.olist1.find('s')>=0 else "")) # noqa: F821
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2) # noqa: F821
            toponame = "%sDISAMB-%iINVM%s-%s%s"  % ( d.disamb if d.disamb>0 else "", d.minInvm, str(d.maxInvm) if d.maxInvm<9999 else "", obj1, obj2)       # noqa: F821    
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]      # noqa: F821         
            #alg = AlgConf.DisambiguationInvariantMass2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1
            alg = AlgConf.DisambiguationInvmIncl2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', d.inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', d.inputwidth2) # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2) # noqa: F821
            alg.addgeneric('NumResultBits', 1)            
            alg.addvariable('MinET1', d.ocut1) # noqa: F821
            alg.addvariable('MinET2', d.ocut2) # noqa: F821
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm) # noqa: F821
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm) # noqa: F821        
            tm.registerAlgo(alg)


        #
        for x in [
            {  "minInvm": 400, "maxInvm": 9999, "otype1" : "AJ", "ocut1": 30, "olist1" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET, "otype2" : "AJ", "ocut2": 20, "olist2" : "s", "nleading2" : 6, "inputwidth2": HW.OutputWidthSortJET, "applyEtaCut":1, "minEta1": 0 ,"maxEta1": 31 , "minEta2": 31 ,"maxEta2": 49 , }, #400INVM9999-AJ30s6.ETA31-AJ20s6.31ETA49
        ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "%s%s%s.%sETA%i"  % (d.otype1, str(d.ocut1), d.olist1 + (str(d.nleading1) if d.olist1.find('s')>=0 else ""),str(d.minEta1) if d.minEta1>0 else "", d.maxEta1) # noqa: F821
            obj2 = "-%s%s%s.%sETA%i"  % (d.otype2, str(d.ocut2), d.olist2 + (str(d.nleading2) if d.olist2.find('s')>=0 else ""),str(d.minEta2) if d.minEta2>0 else "", d.maxEta2) # noqa: F821
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2] # noqa: F821
            toponame = "%iINVM%i-%s%s"   % (d.minInvm, d.maxInvm, obj1, obj2) # noqa: F821
            alg = AlgConf.InvariantMassInclusive2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1    
            alg.addgeneric('InputWidth1', d.inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', d.inputwidth2) # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1) # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2) # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            if (d.applyEtaCut>0): # noqa: F821
                alg.addgeneric('ApplyEtaCut', d.applyEtaCut) # noqa: F821            
            alg.addvariable('MinET1', d.ocut1) # noqa: F821
            alg.addvariable('MinET2', d.ocut2) # noqa: F821
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm ) # noqa: F821
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm ) # noqa: F821
            if (d.applyEtaCut>0): # noqa: F821
                alg.addvariable('MinEta1', d.minEta1) # noqa: F821
                alg.addvariable('MaxEta1', d.maxEta1) # noqa: F821
                alg.addvariable('MinEta2', d.minEta2) # noqa: F821
                alg.addvariable('MaxEta2', d.maxEta2) # noqa: F821            
            tm.registerAlgo(alg)


        #  0INVM9-EM7ab-EMab 
        for x in [
            {"minInvm" : 0, "maxInvm": 9, "otype" : "EM", "ocut1" : 7, "olist" : "ab", "inputwidth": HW.OutputWidthSelectEM, "ocut2" : 0},
            ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            inputList = d.otype + d.olist # noqa: F821
            toponame = "%iINVM%i-%s%s%s-%s%s"  % (d.minInvm, d.maxInvm,                                                    # noqa: F821
                                                  d.otype, str(d.ocut1) , d.olist,     # noqa: F821
                                                  d.otype, d.olist)     # noqa: F821 
            alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId); currentAlgoId += 1  # noqa: F821
            alg.addgeneric('InputWidth', d.inputwidth)      # noqa: F821
            alg.addgeneric('MaxTob', HW.OutputWidthSelectEM)            # noqa: F821
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1)    # noqa: F821
            alg.addvariable('MinET2', d.ocut2)    # noqa: F821
            alg.addvariable('MinMSqr', (d.minInvm * _emscale_for_decision)*(d.minInvm * _emscale_for_decision))        # noqa: F821
            alg.addvariable('MaxMSqr', (d.maxInvm * _emscale_for_decision)*(d.maxInvm * _emscale_for_decision))        # noqa: F821
            tm.registerAlgo(alg)




        # added for b-phys, 0DR03-EM7ab-CJ15ab
        for x in [  
            {"minDr": 0, "maxDr": 3, "otype1" : "EM" ,"ocut1": 7,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"} 
            ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (d.minDr, d.maxDr, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2), d.olist2) # noqa: F821
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2] # noqa: F821
            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectEM)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', d.ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', d.ocut2, 0) # noqa: F821
            alg.addvariable('DeltaRMin', d.minDr*d.minDr, 0) # noqa: F821
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr, 0) # noqa: F821
            tm.registerAlgo(alg)


        # VBF items INVM_NFF
        invm_nff_map = { "algoname": 'INVM_NFF', "Threlist": [ 500 ], "maxInvm": 9999, "otype1" : "J", "ocut1" : 30, "olist1" : "s", "nleading1" : 6,
                         "inputwidth": HW.OutputWidthSortJET,  "otype2" : "AJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }
        for x in [ invm_nff_map ]:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]      # noqa: F821       
            toponames=[]
            for minInvm in d.Threlist:  # noqa: F821
                toponame = "%iINVM%i-%s%s%s%s-%s%s%s%s"  % (minInvm, d.maxInvm,                          
                                                            d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                            d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else "") 
                toponames.append(toponame)
            alg = AlgConf.InvariantMassInclusive2( name = d.algoname, inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1     # noqa: F821
            alg.addgeneric('InputWidth1', d.inputwidth) # noqa: F821
            alg.addgeneric('InputWidth2', d.inputwidth) # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2)# noqa: F821
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, minInvm in enumerate(d.Threlist): # noqa: F821
                alg.addvariable('MinET1', d.ocut1, bitid)# noqa: F821
                alg.addvariable('MinET2', d.ocut2, bitid)# noqa: F821
                alg.addvariable('MinMSqr', minInvm*minInvm , bitid)        # noqa: F821         
                alg.addvariable('MaxMSqr', d.maxInvm *d.maxInvm , bitid)        # noqa: F821
            tm.registerAlgo(alg)        


        # Axion 2EM DPHI  
        #27DPHI32-EMs1-EMs6
        algoList = [
            {"minDphi": 27,  "maxDphi": 32, "otype" : "EM",  "ocut1" : 0,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortEM, "ocut2" : 0, "nleading2": 6},
        ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDPHI%i-%s%s%s%s-%s%s%s%s"  % (d.minDphi, d.maxDphi,  # noqa: F821
                                                        d.otype, str(d.ocut1) if d.ocut1 > 0 else "", d.olist, 
                                                        str(d.nleading1) if d.olist=="s" else "", # noqa: F821
                                                        d.otype, str(d.ocut2) if d.ocut2 > 0 else "", d.olist, 
                                                        str(d.nleading2) if d.olist=="s" else "") # noqa: F821
            log.info("Define %s" % toponame)
            inputList = d.otype + d.olist # noqa: F821
            alg = AlgConf.DeltaPhiIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
            alg.addgeneric('InputWidth', d.inputwidth1) # noqa: F821
            alg.addgeneric('MaxTob', d.nleading2)       # noqa: F821 
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1', d.ocut1 if d.ocut1 > 0 else 3, 0) # noqa: F821   
            alg.addvariable('MinET2', d.ocut2 if d.ocut2 > 0 else 3, 0) # noqa: F821  
            alg.addvariable('MinDeltaPhi', d.minDphi, 0) # noqa: F821
            alg.addvariable('MaxDeltaPhi', d.maxDphi, 0) # noqa: F821
            tm.registerAlgo(alg)


        # VBF items INVM_DPHI_NFF
        NFFDphimap = [{ "minInvm": 400 , "maxInvm": 9999, "minDphi": 0, "maxDphiList": [26, 24, 22, 20], "otype1" : "J", "ocut1" : 30, "olist1" : "s",
                        "nleading1" : 6, "inputwidth": HW.OutputWidthSortJET,  "otype2" : "AJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }]
        for x in NFFDphimap:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]      # noqa: F821       
            toponames=[]
            for maxDphi in d.maxDphiList:  # noqa: F821
                toponames.append ("%iINVM%i-%iDPHI%i-%s%s%s%s-%s%s%s%s"  % (d.minInvm, d.maxInvm, d.minDphi, maxDphi,
                                                                 d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "", 
                                                                 d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else ""))
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'INVM_DPHI_NFF', inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1     # noqa: F821
            alg.addgeneric('InputWidth1', d.inputwidth) # noqa: F821
            alg.addgeneric('InputWidth2', d.inputwidth) # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2)# noqa: F821
            alg.addgeneric('NumResultBits',  len(toponames))
            for bitid,maxDphi in enumerate(d.maxDphiList):  # noqa: F821
                alg.addvariable('MinET1', d.ocut1, bitid)# noqa: F821
                alg.addvariable('MinET2', d.ocut2, bitid)# noqa: F821
                alg.addvariable('MinMSqr', d.minInvm*d.minInvm , bitid)        # noqa: F821         
                alg.addvariable('MaxMSqr', d.maxInvm*d.maxInvm , bitid)        # noqa: F821
                alg.addvariable('MinDeltaPhi', d.minDphi, bitid) # noqa: F821
                alg.addvariable('MaxDeltaPhi', maxDphi, bitid) # noqa: F821
            tm.registerAlgo(alg)


        #ATR-19355  
        toponame = "0INVM10-3MU4ab"      # noqa: F821
        log.info("Define %s" % toponame)    
        inputList = 'MUab'        
        alg = AlgConf.InvariantMassThreeTOBsIncl( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr', 0)
        alg.addvariable('MaxMSqr', 10*10)
        alg.addvariable('MinET1', 4)
        tm.registerAlgo(alg)


        # added for muon-jet:
        algoList = [
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"}, #0DR04-MU4ab-CJ15ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"}, #0DR04-MU6ab-CJ20ab
        ]
        for x in algoList:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (d.minDr, d.maxDr, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2), d.olist2) # noqa: F821
            log.info("Define %s" % toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2] # noqa: F821
            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ], algoId = currentAlgoId); currentAlgoId += 1
            log.info("Current algo ID %i" % currentAlgoId )
            alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1, 0) # noqa: F821
            alg.addvariable('MinET2', d.ocut2, 0) # noqa: F821
            alg.addvariable('DeltaRMin', d.minDr*d.minDr, 0) # noqa: F821
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr, 0) # noqa: F821
            tm.registerAlgo(alg)
            
            


        #ATR-18815
 #       toponame = "0INVM10-0DR15-EM8abi-MU10ab"
 #       log.info("Define %s" % toponame)
        
#        inputList = ['EMabi','MUab']
        
#        alg = AlgConf.InvariantMassInclusive2DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
#        alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob1', HW.OutputWidthSortEM)
#        alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 0)
#        alg.addvariable('MaxMSqr', 10*10*_emscale_for_decision) # Fix to add emscale
#        alg.addvariable('MinET1', 8)
#        alg.addvariable('MinET2', 10)
#        alg.addgeneric('ApplyEtaCut', 0)
#        alg.addvariable('MinEta1', 0)
#        alg.addvariable('MinEta2', 0)
#        alg.addvariable('MaxEta1', 9999)
#        alg.addvariable('MaxEta2', 9999)
#        alg.addvariable('DeltaRMin', 0)
#        alg.addvariable('DeltaRMax', 15*15)
#        tm.registerAlgo(alg)
#        
#        #ATR-18815
#        toponame = "0INVM10-0DR15-EM12abi-MU6ab"
#        log.info("Define %s" % toponame)
#        
#        inputList = ['EMabi','MUab']
#        
#        alg = AlgConf.InvariantMassInclusive2DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
#        alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob1', HW.OutputWidthSortEM)
#        alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 0)
#        alg.addvariable('MaxMSqr', 10*10*_emscale_for_decision) # Fix to add emscale
#        alg.addvariable('MinET1', 12)
#        alg.addvariable('MinET2', 6)
#        alg.addgeneric('ApplyEtaCut', 0)
#        alg.addvariable('MinEta1', 0)
#        alg.addvariable('MinEta2', 0)
#        alg.addvariable('MaxEta1', 9999)
#        alg.addvariable('MaxEta2', 9999)
#        alg.addvariable('DeltaRMin', 0)
#        alg.addvariable('DeltaRMax', 15*15)
#        tm.registerAlgo(alg)
#        


        #ATR-18824 ZAFB-DPHI
        
        ZAFBDphimap = [
            {"minInvm": 60 ,"maxInvm": 9999, "minDphiList": [4, 25], "maxDphi": 32, "minEta2": 23, "maxEta2": 49, "inputwidth1": HW.OutputWidthSortEM, "otype1" : "EM", "ocut1" : 15, "olist1" : "abhi", "nleading1" : HW.OutputWidthSortEM, "inputwidth2": HW.OutputWidthSortJET,  "ocut2" : 15,"nleading2" : 6}
        ]
        for x in ZAFBDphimap:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, 'FJjs23ETA49']      # noqa: F821       
            toponames=[]
            for minDphi in d.minDphiList:  # noqa: F821
                toponames.append ("%iINVM%i-%02dDPHI%i-%s%s%s%s-FJj%ss%s%iETA%i"  % (d.minInvm, d.maxInvm, minDphi, d.maxDphi, d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",  str(d.ocut2) , str(d.nleading2) , d.minEta2, d.maxEta2))  
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'ZAFB_DPHI', inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1     # noqa: F821
            alg.addgeneric('InputWidth1', d.inputwidth1) # noqa: F821
            alg.addgeneric('InputWidth2', d.inputwidth2) # noqa: F821
            alg.addgeneric('MaxTob1', d.nleading1)       # noqa: F821
            alg.addgeneric('MaxTob2', d.nleading2)# noqa: F821
            alg.addgeneric('NumResultBits',  len(toponames))
            alg.addgeneric('ApplyEtaCut', 1)
            alg.addvariable('MinEta1', 0)
            alg.addvariable('MaxEta1', 9999)
            alg.addvariable('MinEta2', 23)
            alg.addvariable('MaxEta2', 49)
            for bitid,minDphi in enumerate(d.minDphiList):  # noqa: F821
                alg.addvariable('MinET1', d.ocut1, bitid)# noqa: F821
                alg.addvariable('MinET2', d.ocut2, bitid)# noqa: F821
                alg.addvariable('MinMSqr', d.minInvm*d.minInvm*_emscale_for_decision , bitid)        # noqa: F821 #ATR-18824 only one factor is needed as there is one EM object         
                alg.addvariable('MaxMSqr', d.maxInvm*d.maxInvm*_emscale_for_decision , bitid)        # noqa: F821 #ATR-18824 only one factor is needed as there is one EM object
                alg.addvariable('MinDeltaPhi', minDphi, bitid) # noqa: F821
                alg.addvariable('MaxDeltaPhi', d.maxDphi, bitid) # noqa: F821
            tm.registerAlgo(alg)


        #ATR-19302: not included for now
        toponame = "0INVM70-27DPHI32-EM10his1-EM10his6"
        log.info("Define %s" % toponame)
        inputList = ['EMshi','EMshi']
        alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', 6)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr', 0)
        alg.addvariable('MaxMSqr', (70*_emscale_for_decision)*(70*_emscale_for_decision))
        alg.addvariable('MinET1', 10)
        alg.addvariable('MinET2', 10)
        alg.addgeneric('ApplyEtaCut', 1)
        alg.addvariable('MinEta1', 0)
        alg.addvariable('MaxEta1', 9999)
        alg.addvariable('MinEta2', 0)
        alg.addvariable('MaxEta2', 9999)
        alg.addvariable('MinDeltaPhi', 27)
        alg.addvariable('MaxDeltaPhi', 32)
        tm.registerAlgo(alg)

        
        toponame = "0INVM70-27DPHI32-EM12his1-EM12his6"
        log.info("Define %s" % toponame)
        inputList = ['EMshi','EMshi']
        alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth1', HW.OutputWidthSortEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', 6)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr', 0)
        alg.addvariable('MaxMSqr', (70*_emscale_for_decision)*(70*_emscale_for_decision))
        alg.addvariable('MinET1', 10)
        alg.addvariable('MinET2', 12)
        alg.addgeneric('ApplyEtaCut', 1)
        alg.addvariable('MinEta1', 0)
        alg.addvariable('MaxEta1', 9999)
        alg.addvariable('MinEta2', 0)
        alg.addvariable('MaxEta2', 9999)
        alg.addvariable('MinDeltaPhi', 27)
        alg.addvariable('MaxDeltaPhi', 32)
        tm.registerAlgo(alg)


        #ATR-19720, L1_BPH-8M15-0DR22-2MU6
#        toponame = "8INVM15-0DR22-2MU6ab"
#        log.info("Define %s" % toponame)
#        
#        inputList = ['MUab']
#        
#        alg = AlgConf.InvariantMassInclusive1DeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 8*8)
#        alg.addvariable('MaxMSqr', 15*15)
#        alg.addvariable('MinET1', 6)
#        alg.addvariable('MinET2', 6)
#        alg.addvariable('DeltaRMin', 0)
#        alg.addvariable('DeltaRMax', 22*22)
#        tm.registerAlgo(alg)
#        
#        #ATR-19720, L1_BPH-2M9-2DR15-2MU6
#        toponame = "2INVM9-2DR15-2MU6ab"
#        log.info("Define %s" % toponame)
#    
#        inputList = ['MUab']
#    
#        alg = AlgConf.InvariantMassInclusive1DeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 2*2)
#        alg.addvariable('MaxMSqr', 9*9)
#        alg.addvariable('MinET1', 6)
#        alg.addvariable('MinET2', 6)
#        alg.addvariable('DeltaRMin', 2*2)
#        alg.addvariable('DeltaRMax', 15*15)
#        tm.registerAlgo(alg)
#
#        #ATR-19720, L1_BPH-2M9-0DR15-MU6MU4
#        toponame = "2INVM9-0DR15-MU6ab-MU4ab"
#        log.info("Define %s" % toponame)
#        
#        inputList = ['MUab']
#        
#        alg = AlgConf.InvariantMassInclusive1DeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 2*2)
#        alg.addvariable('MaxMSqr', 9*9)
#        alg.addvariable('MinET1', 6)
#        alg.addvariable('MinET2', 4)
#        alg.addvariable('DeltaRMin', 0)
#        alg.addvariable('DeltaRMax', 15*15)
#        tm.registerAlgo(alg)
#        
#        #ATR-19720, L1_BPH-8M15-0DR22-MU6MU4-BO
#        toponame = "8INVM15-0DR22-MU6ab-MU4ab"
#        log.info("Define %s" % toponame)
#        
#        inputList = ['MUab']
#        
#        alg = AlgConf.InvariantMassInclusive1DeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 8*8)
#        alg.addvariable('MaxMSqr', 15*15)
#        alg.addvariable('MinET1', 6)
#        alg.addvariable('MinET2', 4)
#        alg.addvariable('DeltaRMin', 0)
#        alg.addvariable('DeltaRMax', 22*22)
#        tm.registerAlgo(alg)
#
#        #ATR-19720, L1_BPH-2M9-0DR15-2MU4
#        
#        toponame = "2INVM9-0DR15-2MU4ab"
#        log.info("Define %s" % toponame)
#   
#        inputList = ['MUab']
#
#        alg = AlgConf.InvariantMassInclusive1DeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
#        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
#        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
#        alg.addgeneric('NumResultBits', 1)
#        alg.addvariable('MinMSqr', 2*2)
#        alg.addvariable('MaxMSqr', 9*9)
#        alg.addvariable('MinET1', 4)
#        alg.addvariable('MinET2', 4)
#        alg.addvariable('DeltaRMin', 0)
#        alg.addvariable('DeltaRMax', 15*15)
#        tm.registerAlgo(alg)
#


        currentAlgoId = 47

        #ATR-20174, L1BPH-8M15-2MU4-BO
        toponame = "8INVM15-2CMU4ab"
        log.info("Define %s" % toponame)        
        inputList = ['CMUab']        
        alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame, algoId = currentAlgoId ); currentAlgoId += 1
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr', 8*8)
        alg.addvariable('MaxMSqr', 15*15)
        alg.addvariable('MinET1', 4)
        alg.addvariable('MinET2', 4)        
        tm.registerAlgo(alg)
        

        #CEP algorithms
        CEPmap = [{"algoname": 'CEP_CJ', "minETlist": [50, 60]}]
        for x in CEPmap:
            class d : pass
            for k in x:
                setattr (d, k, x[k])
            inputList = ['CJs']
            toponames=[]            
            for minET in d.minETlist:  # noqa: F821
                toponames.append ("CEP-CJ%is6" % (minET))     # noqa: F821 
            alg = AlgConf.ExclusiveJets( name = d.algoname, inputs = inputList, outputs = toponames, algoId = currentAlgoId); currentAlgoId += 1 # noqa: F821
            alg.addgeneric('InputWidth', HW.InputWidthJET) # noqa: F821
            alg.addgeneric('MaxTob', HW.InputWidthJET)       # noqa: F821
            alg.addgeneric('NumResultBits',  len(toponames)) # noqa: F821
            for bitid,minET in enumerate(d.minETlist):  # noqa: F821
                alg.addvariable('MinET1', minET, bitid)# noqa: F821
            alg.addvariable('MinXi', 13000.0*0.02, bitid) # noqa: F821
            alg.addvariable('MaxXi', 13000.0*0.05, bitid) # noqa: F821
            tm.registerAlgo(alg)

        
