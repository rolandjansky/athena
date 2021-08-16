# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


# algorithm python base classes generated from C++ code
import L1TopoAlgorithms.L1TopoAlgConfig as AlgConf
import L1TopoHardware.L1TopoHardware as HW


from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class TopoAlgoDef:

    @staticmethod
    def registerTopoAlgos(tm):

        # constants and conversions:
        _no_m_upper_threshold = 1024*1024*10*10*10 # a check against this number in L1Topo FW and sim will disable the upper threshold on MaxMSqr
        _dr_conversion = 4   # factor 10 already included to remove . from name
        _et_conversion = 10  # 1 GeV -> 100 MeV. This is to be applied also on the pt threholds for muons. A correction is then done L1/Base/TopoAlgos.py 
        _eta_conversion = 4  # factor 10 already included to remove . from name
        _phi_conversion = 2  # factor 10 already included to remove . from name

        # eEM inputs
        # ALL
        alg = AlgConf.ClusterNoSort( name = 'eEMall', inputs = 'ClusterTobArray', outputs = 'eEMall' ) #TODO: change ClusterNoSort into...? still old tobs inputs
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('OutputWidth', HW.InputWidthEM)
        tm.registerTopoAlgo(alg)  

        # SORT
        alg = AlgConf.eEmSort( name = 'eEMs', inputs = 'eEmTobs', outputs = 'eEMs' )
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('REtaMin',   0)
        alg.addvariable('RHadMin',   0)
        alg.addvariable('WsTotMin',  0)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        tm.registerTopoAlgo(alg)

        alg = AlgConf.eEmSort( name = 'eEMsl', inputs = 'eEmTobs', outputs = 'eEMsl' )
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('REtaMin',   1)
        alg.addvariable('RHadMin',   1)
        alg.addvariable('WsTotMin',  1)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        tm.registerTopoAlgo(alg)

        alg = AlgConf.eEmSort( name = 'eEMsm', inputs = 'eEmTobs', outputs = 'eEMsm' )
        alg.addgeneric('InputWidth', HW.InputWidthEM)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortEM)
        alg.addvariable('REtaMin',   2)
        alg.addvariable('RHadMin',   2)
        alg.addvariable('WsTotMin',  2)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        tm.registerTopoAlgo(alg)

        # SELECT
        alg = AlgConf.eEmSelect( name = 'eEMab', inputs = 'eEmTobs', outputs = 'eEMab' )
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET',     5*_et_conversion)
        alg.addvariable('REtaMin',   0)
        alg.addvariable('RHadMin',   0)
        alg.addvariable('WsTotMin',  0)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        tm.registerTopoAlgo(alg)

        alg = AlgConf.eEmSelect( name = 'eEMabl', inputs = 'eEmTobs', outputs = 'eEMabl' )
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET',     8*_et_conversion)
        alg.addvariable('REtaMin',   1)
        alg.addvariable('RHadMin',   1)
        alg.addvariable('WsTotMin',  1)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        tm.registerTopoAlgo(alg)

        alg = AlgConf.eEmSelect( name = 'eEMabm', inputs = 'eEmTobs', outputs = 'eEMabm' )
        alg.addgeneric('InputWidth',  HW.InputWidthEM)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectEM )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectEM)
        alg.addvariable('MinET',     8*_et_conversion)
        alg.addvariable('REtaMin',   2)
        alg.addvariable('RHadMin',   2)
        alg.addvariable('WsTotMin',  2)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        tm.registerTopoAlgo(alg)

        # eTAU inputs
        # all
        alg = AlgConf.ClusterNoSort( name = 'eTAUall', inputs = 'ClusterTobArray', outputs = 'eTAUall') #TODO: change ClusterNoSort into...? still old tob inputs
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('OutputWidth', HW.InputWidthTAU)
        alg.addvariable('IsoMask', 0)
        tm.registerTopoAlgo(alg)

        # SORT
        alg = AlgConf.eTauSort( name = 'eTAUsi', inputs = 'eTauTobs', outputs = 'eTAUsi' )
        alg.addgeneric('InputWidth', HW.InputWidthTAU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSortTAU)
        alg.addvariable('IsoMask',   2)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        alg.addgeneric('DoIsoCut',   1)
        tm.registerTopoAlgo(alg)

        # SELECT
        alg = AlgConf.eTauSelect( name = 'eTAUab', inputs = 'eTauTobs', outputs = 'eTAUab' )
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)
        alg.addvariable('MinET',    12*_et_conversion)
        alg.addvariable('IsoMask',   0)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        alg.addgeneric('DoIsoCut',   0)
        tm.registerTopoAlgo(alg) 

        alg = AlgConf.eTauSelect( name = 'eTAUabi', inputs = 'eTauTobs', outputs = 'eTAUabi' )
        alg.addgeneric('InputWidth',  HW.InputWidthTAU)
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectTAU)
        alg.addvariable('MinET',    12*_et_conversion)
        alg.addvariable('IsoMask',   2)
        alg.addvariable('MinEta',    0*_eta_conversion)
        alg.addvariable('MaxEta',   49*_eta_conversion)
        alg.addgeneric('DoIsoCut',   1)
        tm.registerTopoAlgo(alg) 

        # Muon inputs # TODO: change to new Phase-1 thresholds
        # all
        alg = AlgConf.MuonNoSort( name = 'MUall', inputs = 'MuonTobArray', outputs = 'MUall')
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('OutputWidth', HW.InputWidthMU)
        tm.registerTopoAlgo(alg)

        # SORT
        alg = AlgConf.MuonSort( name = 'MUs', inputs = 'MuonTobArray', outputs = 'MUs' )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortMU)
        alg.addvariable('MinEta',  0*_eta_conversion)
        alg.addvariable('MaxEta', 25*_eta_conversion)
        tm.registerTopoAlgo(alg)
        
        # SELECT
        alg = AlgConf.MuonSelect( name = 'MUab', inputs = 'MuonTobArray', outputs = 'MUab' )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinET',   4*_et_conversion)
        alg.addvariable('MinEta',  0*_eta_conversion)
        alg.addvariable('MaxEta', 25*_eta_conversion)
        tm.registerTopoAlgo(alg)

        alg = AlgConf.MuonSelect( name = 'CMUab', inputs = 'MuonTobArray', outputs = 'CMUab' )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSelectMU)
        alg.addvariable('MinET',   4*_et_conversion)
        alg.addvariable('MinEta',  0*_eta_conversion)
        alg.addvariable('MaxEta', 10*_eta_conversion)
        tm.registerTopoAlgo(alg)
    
        #LATE
        alg = AlgConf.MuonSort_1BC( name = 'LMUs', inputs = 'LateMuonTobArray', outputs = 'LMUs' )
        alg.addgeneric('InputWidth', HW.InputWidthMU)
        #alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortMU )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortMU)
        alg.addgeneric('nDelayedMuons', 1)
        alg.addvariable('MinEta',  0*_eta_conversion)
        alg.addvariable('MaxEta', 25*_eta_conversion)
        tm.registerTopoAlgo(alg)

        #jJets inputs # TODO: all lists below still to be fixed
        alg = AlgConf.JetNoSort( name = 'AJall', inputs = 'JetTobArray', outputs = 'AJall' ) 
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', HW.DefaultJetSize)
        tm.registerTopoAlgo(alg)

        alg = AlgConf.JetNoSort( name = 'AJjall', inputs = 'JetTobArray', outputs = 'AJjall' ) 
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)
        tm.registerTopoAlgo(alg)


        # for 0MATCH-4AJ20-4AJj15
        alg = AlgConf.JetNoSortMatch( name = 'AJMatchall', inputs = 'JetTobArray', outputs = 'AJMatchall' ) 
        alg.addgeneric('InputWidth', HW.InputWidthJET)
        alg.addgeneric('OutputWidth', HW.InputWidthJET)
        alg.addgeneric('JetSize', 2 if HW.DefaultJetSize.value==2 else 1)
        alg.addvariable('MinET1', 15) # 4x4       
        alg.addvariable('MinET2', 20) # 8x8
        tm.registerTopoAlgo(alg)        

        # ab J lists:
        #legacy
        for jet_type in ['J', 'CJ', 'FJ']:
            jetabseta = 49
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
            alg = AlgConf.JetSelect( name = jet_type+'ab', inputs = 'JetTobArray', outputs = jet_type+'ab' )
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSelectJET )            
            alg.addgeneric('OutputWidth', HW.OutputWidthSelectJET)
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinET', _minet)  
            alg.addvariable('MinEta', _mineta)
            alg.addvariable('MaxEta', jetabseta)
            alg.addgeneric('DoEtaCut', 1)
            tm.registerTopoAlgo(alg) 
        #phase1
        for jet_type in ['jJ']:
            jetabseta = 49*_eta_conversion
            minet = 25*_et_conversion
            mineta=0
            if jet_type=='J':
                jetabseta = 31*_eta_conversion
                minet = 20*_et_conversion
            elif jet_type=='CJ':
                jetabseta = 26*_eta_conversion
                minet = 15*_et_conversion
            elif jet_type=='FJ':
                mineta = 31*_eta_conversion
                minet = 15*_et_conversion
            alg = AlgConf.JetSelect( name = jet_type+'ab', inputs = 'JetTobArray', outputs = jet_type+'ab' )
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('OutputWidth', HW.OutputWidthSelectJET)
            alg.addvariable('MinET', minet)
            alg.addvariable('MinEta', mineta)
            alg.addvariable('MaxEta', jetabseta)
            tm.registerTopoAlgo(alg)


        alg = AlgConf.JetSort( name = 'AJjs', inputs = 'JetTobArray', outputs = 'AJjs')
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)                
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 49)
        alg.addgeneric('DoEtaCut', 0)
        tm.registerTopoAlgo(alg) 


        #input list needed for ATR-18824
        alg = AlgConf.JetSort( name = 'FJjs23ETA49', inputs = 'JetTobArray', outputs = 'FJjs23ETA49')
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', 1 if HW.DefaultJetSize.value==2 else 2)
        alg.addvariable('MinEta', 23)
        alg.addvariable('MaxEta', 49)
        alg.addgeneric('DoEtaCut', 1)
        tm.registerTopoAlgo(alg)


        alg = AlgConf.JetSort( name = 'CJsETA21', inputs = 'JetTobArray', outputs = 'CJsETA21')
        alg.addgeneric('InputWidth',  HW.InputWidthJET)
        alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
        alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
        alg.addgeneric('JetSize', HW.DefaultJetSize.value)
        alg.addvariable('MinEta', 0)
        alg.addvariable('MaxEta', 21)
        tm.registerTopoAlgo(alg)

        # Sorted J lists:
        #legacy
        for jet_type in ['AJ', 'FJ']:
            jetabseta = 49
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
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s' )
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
        #phase1
        for jet_type in ['AjJ']:
            jetabseta = 49*_eta_conversion
            mineta = 0*_eta_conversion
            if jet_type=='J':
                jetabseta = 31*_eta_conversion
            elif jet_type=='CJ':
                jetabseta = 26*_eta_conversion
            elif jet_type=='FJ':
                mineta = 31*_eta_conversion
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s' )
            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addvariable('MinEta', mineta)
            alg.addvariable('MaxEta', jetabseta)
            tm.registerTopoAlgo(alg)


        #legacy
        for jet_type in ['J','CJ']:
            jetabseta = 49
            _minet = 25
            if jet_type=='J':
                jetabseta = 31
                _minet = 20
            elif jet_type=='CJ':
                jetabseta = 26
                _minet = 15
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s' )
            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('InputWidth1stStage', HW.InputWidth1stStageSortJET )
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addgeneric('JetSize', HW.DefaultJetSize.value)
            alg.addvariable('MinEta', 0)
            alg.addvariable('MaxEta', jetabseta)
            alg.addgeneric('DoEtaCut', 1)
            tm.registerTopoAlgo(alg)
        #phase1
        for jet_type in ['jJ']:
            jetabseta = 49*_eta_conversion
            if jet_type=='J':
                jetabseta = 31*_eta_conversion
            elif jet_type=='CJ':
                jetabseta = 26*_eta_conversion
            alg = AlgConf.JetSort( name = jet_type+'s', inputs = 'JetTobArray', outputs = jet_type+'s' )
            alg.addgeneric('InputWidth',  HW.InputWidthJET)
            alg.addgeneric('OutputWidth', HW.OutputWidthSortJET )
            alg.addvariable('MinEta', 0*_eta_conversion)
            alg.addvariable('MaxEta', jetabseta)
            tm.registerTopoAlgo(alg)


        alg = AlgConf.METNoSort( name = 'XENoSort', inputs = 'MetTobArray', outputs = 'XENoSort' )

        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerTopoAlgo(alg)

                
        alg = AlgConf.MetSort( name = 'XE', inputs = 'MetTobArray', outputs = 'XE' )
        alg.addgeneric('InputWidth', HW.InputWidthMET)
        alg.addgeneric('OutputWidth', HW.OutputWidthMET)
        tm.registerTopoAlgo(alg)

        
                
        # Decision algorithms

        # LAR  ZEE
        algoList = [
            {"otype" : "eEM", "ocut1" : 20,  "ocut2" : 20, "olist" : "sm", "nleading1" : 2, "minInvm" : 60, "maxInvm" : 100, "inputwidth": HW.OutputWidthSortEM},
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = 'ZEE-eEM20sm2'
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame)
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', d.nleading1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1',  d.ocut1*_et_conversion )
            alg.addvariable('MinET2',  d.ocut2*_et_conversion )
            alg.addvariable('MinMSqr', d.minInvm*d.minInvm*_et_conversion*_et_conversion )
            alg.addvariable('MaxMSqr', d.maxInvm*d.maxInvm*_et_conversion*_et_conversion )
            tm.registerTopoAlgo(alg)

        

        # dimu DR items
        # TODO: udpate with phase1 muons
        listofalgos=[
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR15-2MU6ab 
            {"minDr": 0, "maxDr": 15, "mult": 2, "otype1" : "MU", "ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR15-2MU4ab 
            {"minDr": 0, "maxDr": 24, "mult": 2, "otype1" : "MU", "ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR24-2MU4ab 
            {"minDr": 0, "maxDr": 15, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0}, #0DR15-MU6ab-MU4ab
            {"minDr": 0, "maxDr": 22, "mult": 1, "otype1" : "MU", "ocut1": 6,  "olist" : "ab", "otype2" : "MU", "ocut2": 4, "onebarrel": 0}, #0DR22-MU6ab-MU4ab
            {"minDr": 2, "maxDr": 15, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #2DR15-2MU6ab
            {"minDr": 0, "maxDr": 22, "mult": 2, "otype1" : "MU" ,"ocut1": 6,  "olist" : "ab", "otype2" : "",   "ocut2": 6, "onebarrel": 0}, #0DR22-2MU6ab
            {"minDr": 2, "maxDr": 99, "mult": 2, "otype1" : "MU" ,"ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, # SM Y  x, 2DR99-2MU4ab
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
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ]) 
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
            alg.addvariable('MinET1',    d.ocut1*_et_conversion)
            alg.addvariable('MinET2',    d.ocut2*_et_conversion)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr*_dr_conversion*_dr_conversion)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr*_dr_conversion*_dr_conversion)
            tm.registerTopoAlgo(alg)

            
        # deta-dphi with ab+ab
        # TODO: udpate with phase1 muons
        algolist=[
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 1, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "MU", "ocut2": 4, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-MU6ab-MU4ab
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 6, "olist2": "", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-2MU6ab           
            {"minDeta": 5, "maxDeta": 99, "minDphi": 5, "maxDphi": 99, "mult": 2, "otype1" : "MU", "ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "otype2" : "", "ocut2": 4, "olist2": "", "nleading2": HW.OutputWidthSelectMU}, #5DETA99-5DPHI99-2MU4ab
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
            alg = algoname( name = toponame, inputs = inputList, outputs = [ toponame ])
            alg.addgeneric('NumResultBits', 1)            
            if (d.mult>1 or d.otype1==d.otype2):
                alg.addgeneric('InputWidth', d.nleading1)
                alg.addgeneric('MaxTob', d.nleading1)
                alg.addvariable('MinET1',      d.ocut1*_et_conversion)
                alg.addvariable('MinET2',      d.ocut2*_et_conversion)
                alg.addvariable('MinDeltaEta', d.minDeta*_eta_conversion)
                alg.addvariable('MaxDeltaEta', d.maxDeta*_eta_conversion)
                alg.addvariable('MinDeltaPhi', d.minDphi*_phi_conversion)
                alg.addvariable('MaxDeltaPhi', d.maxDphi*_phi_conversion)
            else:
                alg.addgeneric('InputWidth1', d.nleading1)
                alg.addgeneric('InputWidth2', d.nleading2)
                alg.addgeneric('MaxTob1', d.nleading1)
                alg.addgeneric('MaxTob2', d.nleading2)
                alg.addvariable('DeltaEtaMin', d.minDeta*_eta_conversion)
                alg.addvariable('DeltaEtaMax', d.maxDeta*_eta_conversion)
                alg.addvariable('DeltaPhiMin', d.minDphi*_phi_conversion)
                alg.addvariable('DeltaPhiMax', d.maxDphi*_phi_conversion)
                alg.addvariable('MinET1',      d.ocut1*_et_conversion)
                alg.addvariable('MinET2',      d.ocut2*_et_conversion)            
            tm.registerTopoAlgo(alg)
            

        # (ATR-8194) L1Topo HT Trigger
        # TODO: double check correct implementation
        algoList = [
            {"minHT": 150, "otype" : "jJ", "ocut" : 20, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 31}, #HT150-jJ20s5pETA31
            {"minHT": 190, "otype" : "jJ", "ocut" : 15, "olist" : "s",   "nleading" : 5, "inputwidth": HW.OutputWidthSortJET, "oeta" : 21}, #HT190-jJ15s5pETA21
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "HT%d-%s%s%s%spETA%s" % (d.minHT, d.otype, str(d.ocut), d.olist, str(d.nleading) if d.olist=="s" else "", str(d.oeta))
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.JetHT( name = toponame, inputs = inputList, outputs = [toponame] )
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', d.nleading)
            alg.addgeneric('NumRegisters', 2 if d.olist=="all" else 0)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET',  d.ocut*_et_conversion)
            alg.addvariable('MinEta',      0*_eta_conversion)
            alg.addvariable('MaxEta', d.oeta*_eta_conversion)
            alg.addvariable('MinHt', d.minHT*_et_conversion)
            tm.registerTopoAlgo(alg)




        # INVM_EM for Jpsi
        invm_map = { "algoname": 'INVM_eEMs6' , "ocutlist": [ 7, 12 ], "minInvm": 1, "maxInvm": 5, "otype" : "eEM", "olist" : "s",
                     "nleading" : 1, "inputwidth": HW.OutputWidthSortEM}
        for x in [ invm_map ]:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = d.otype + d.olist
            toponames=[]
            for ocut in d.ocutlist:
                toponame = "%iINVM%i-%s%s%s%s-eEMs6" % (d.minInvm, d.maxInvm, d.otype, str(ocut) if ocut > 0 else "", d.olist, str(d.nleading) if d.olist=="s" else "")
                toponames.append(toponame)
            alg = AlgConf.InvariantMassInclusive2( name = d.algoname, inputs = [inputList, 'eEMs'], outputs = toponames)
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
            alg.addgeneric('MaxTob1', d.nleading)
            alg.addgeneric('MaxTob2', HW.OutputWidthSortEM)
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, ocut in enumerate(d.ocutlist):
                alg.addvariable('MinET1', ocut*_et_conversion, bitid)
                alg.addvariable('MinET2',    0*_et_conversion, bitid)
                alg.addvariable('MinMSqr', d.minInvm*d.minInvm*_et_conversion*_et_conversion, bitid)
                alg.addvariable('MaxMSqr', d.maxInvm*d.maxInvm*_et_conversion*_et_conversion, bitid)
            tm.registerTopoAlgo(alg)

            
        # added for muon-jet:
        # TODO: to be updated with phase1 muons and jets
        algoList = [
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"}, #0DR04-MU4ab-CJ15ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"}, #0DR04-MU4ab-CJ20ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 4,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 30, "olist2" : "ab"}, #0DR04-MU4ab-CJ30ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 20, "olist2" : "ab"}, #0DR04-MU6ab-CJ20ab
            {"minDr": 0, "maxDr": 4, "otype1" : "MU" ,"ocut1": 6,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 25, "olist2" : "ab"}, #0DR04-MU6ab-CJ25ab
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (d.minDr, d.maxDr, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2), d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ])
            alg.addgeneric('InputWidth1', HW.OutputWidthSelectMU)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectMU)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1',    d.ocut1*_et_conversion, 0)
            alg.addvariable('MinET2',    d.ocut2*_et_conversion, 0)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr*_dr_conversion*_dr_conversion, 0)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr*_dr_conversion*_dr_conversion, 0)
            tm.registerTopoAlgo(alg)


        # MULT-BIT            
        # TODO: to be updated with phase1 muons
        algoList = [
                {"otype1" : "CMU" ,"ocut1": 4, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU}, #MULT-CMU4ab
                {"otype1" : "CMU" ,"ocut1": 6, "olist1" : "ab", "nleading1": HW.OutputWidthSelectMU, "inputwidth1": HW.OutputWidthSelectMU}, #MULT-CMU6ab
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "MULT-%s%s%s" % (d.otype1, str(d.ocut1), d.olist1) 
            toponames = [toponame+"[0]", toponame+"[1]"]
            log.debug("Define %s with outputs %r", toponame, toponames)
            inputList = [d.otype1 + d.olist1]  
            alg = AlgConf.Multiplicity( name = toponame,  inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth', d.inputwidth1) 
            alg.addgeneric('NumResultBits', 2)
            alg.addvariable('MinET', d.ocut1*_et_conversion)
            tm.registerTopoAlgo(alg)        


        # dimu INVM items
        # TODO: to be updated with phase1 muons
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
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ])
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
            alg.addvariable('MinET1', d.ocut1*_et_conversion)
            alg.addvariable('MinET2', d.ocut2*_et_conversion if d.ocut2>0 else d.ocut1*_et_conversion)
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm *_et_conversion*_et_conversion)
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm *_et_conversion*_et_conversion)
            tm.registerTopoAlgo(alg)

        # TODO: to be updated with phase1 muons
        toponame = "8INVM15-2CMU4ab"
        log.debug("Define %s", toponame)        
        inputList = ['CMUab']
        alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',   8*8*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 15*15*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',      4*_et_conversion)
        alg.addvariable('MinET2',      4*_et_conversion)        
        tm.registerTopoAlgo(alg)

        # TODO: to be updated with phase1 muons
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
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ])
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
            alg.addvariable('MinET1', d.ocut1*_et_conversion)
            alg.addvariable('MinET2', d.ocut2*_et_conversion if d.ocut2>0 else d.ocut1*_et_conversion)
            alg.addvariable('MinMSqr', d.minInvm * d.minInvm *_et_conversion*_et_conversion)
            alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm *_et_conversion*_et_conversion)
            tm.registerTopoAlgo(alg)


        # dimu DR items
        # TODO: to be updated with phase1 muons
        algolist = [
            {"minDr": 0, "maxDr": 24, "mult": 2, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "",   "ocut2": 4, "onebarrel": 0}, #0DR24-2CMU4ab
            {"minDr": 0, "maxDr": 24, "mult": 1, "otype1" : "CMU","ocut1": 4,  "olist" : "ab", "otype2" : "MU","ocut2": 4, "onebarrel": 0}, #0DR24-CMU4ab-MU4ab  
            {"minDr": 10, "maxDr": 99, "mult": 2, "otype1" : "MU","ocut1": 6,  "olist" : "ab", "otype2" : "","ocut2": 4, "onebarrel": 0}, #10DR99-2MU6ab
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
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ])
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
            alg.addvariable('MinET1', d.ocut1*_et_conversion)
            alg.addvariable('MinET2', d.ocut2*_et_conversion)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr *_et_conversion*_et_conversion)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr *_et_conversion*_et_conversion)
            tm.registerTopoAlgo(alg)

        # LFV DETA ATR-14282
        # TODO: update with phase1 muons (also algo below)
        algoList = [
            { "minDeta": 0, "maxDeta": "04", "mult": 1, "otype1" : "eEM", "ocut1": 8, "olist1" : "abl",
              "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab",
              "nleading2": HW.OutputWidthSelectMU}, #0DETA04-eEM8abl-MU10ab
            { "minDeta": 0, "maxDeta": "04", "mult": 1,
              "otype1" : "eEM", "ocut1": 15, "olist1" : "abl", "nleading1": HW.OutputWidthSelectEM,
              "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DETA04-eEM15abl-MUab
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%sDETA%s-%s%i%s-%s%s%s"  % (d.minDeta, d.maxDeta, d.otype1, d.ocut1, d.olist1, d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaEtaIncl2( name = toponame, inputs = inputList, outputs = [ toponame ])
            alg.addgeneric('NumResultBits', 1)
            alg.addgeneric('InputWidth1', d.nleading1)
            alg.addgeneric('InputWidth2', d.nleading2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addvariable('MinET1',      d.ocut1*_et_conversion, 0)
            alg.addvariable('MinET2',      d.ocut2*_et_conversion, 0)
            alg.addvariable('MinDeltaEta', d.minDeta*_eta_conversion, 0)
            alg.addvariable('MaxDeltaEta', d.maxDeta*_eta_conversion, 0)
            tm.registerTopoAlgo(alg)

        algoList = [
            {"minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "eEM", "ocut1": 8, "olist1" : "abl", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 10, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DPHI03-eEM8abl-MU10ab
            {"minDphi": 0, "maxDphi": "03", "mult": 1, "otype1" : "eEM", "ocut1": 15, "olist1" : "abl", "nleading1": HW.OutputWidthSelectEM, "otype2" : "MU", "ocut2": 0, "olist2": "ab", "nleading2": HW.OutputWidthSelectMU}, #0DPHI03-eEM15abl-MUab
            ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])            
            toponame = "%sDPHI%s-%s%s%s-%s%s%s"  % (d.minDphi, d.maxDphi, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2) if d.ocut2>0 else "", d.olist2) 
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaPhiIncl2( name = toponame, inputs = inputList, outputs = [ toponame ])
            alg.addgeneric('NumResultBits', 1)            
            alg.addgeneric('InputWidth1', d.nleading1)
            alg.addgeneric('InputWidth2', d.nleading2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addvariable('MinET1',      d.ocut1*_et_conversion, 0)
            alg.addvariable('MinET2',      d.ocut2*_et_conversion, 0)
            alg.addvariable('MinDeltaPhi', d.minDphi*_phi_conversion, 0)
            alg.addvariable('MaxDeltaPhi', d.maxDphi*_phi_conversion, 0)
            tm.registerTopoAlgo(alg)
            

        # RATIO MATCH dedicated to Exotic #TODO: are eTAU correct to use here (and below)?
        toponame = '100RATIO-0MATCH-eTAU30si2-eEMall'
        alg = AlgConf.RatioMatch( name = toponame, inputs = [ 'eTAUsi', 'eEMall'], outputs = [ toponame ] )
        alg.addgeneric('InputWidth1', HW.OutputWidthSortTAU)
        alg.addgeneric('InputWidth2', HW.InputWidthEM)      
        alg.addgeneric('MaxTob1', 2)
        alg.addgeneric('MaxTob2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1', 30*_et_conversion)
        alg.addvariable('MinET2',  0*_et_conversion)
        alg.addvariable('Ratio', 100, 0)
        tm.registerTopoAlgo(alg)        

        # NOT MATCH dedicated to Exotic
        toponame = 'NOT-0MATCH-eTAU30si1-eEMall'
        alg = AlgConf.NotMatch( name = toponame, inputs = [ 'eTAUsi', 'eEMall'], outputs = [ toponame ] )
        alg.addgeneric('InputWidth1', HW.OutputWidthSortTAU)
        alg.addgeneric('InputWidth2', HW.InputWidthEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', HW.InputWidthEM)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinET1',  30*_et_conversion)
        alg.addvariable('MinET2',   0*_et_conversion)
        alg.addvariable('EtaMin1',  0*_eta_conversion)
        alg.addvariable('EtaMax1', 49*_eta_conversion)
        alg.addvariable('EtaMin2',  0*_eta_conversion)
        alg.addvariable('EtaMax2', 49*_eta_conversion)
        alg.addvariable('DRCut', 0) #TODO: conversion needed here?
        tm.registerTopoAlgo(alg)        

        # TODO: to be updated with phase1 met
        xemap = [{"etcut": 0, "Threlist": [ 40, 50, 55, 60, 65, 75 ]}]
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
            alg = AlgConf.KalmanMETCorrection( name = "KF-XE-AJall", inputs = inputList, outputs = toponames )
            alg.addgeneric('InputWidth', HW.InputWidthJET)
            alg.addgeneric('NumResultBits', len(toponames))
            alg.addvariable('MinET', 0)
            for bitid,minxe in enumerate(d.Threlist):
                alg.addvariable('KFXE', str(minxe), bitid)            
            tm.registerTopoAlgo(alg)

                
        # LATE MUON : LATE-MU10s1
        # TODO: to be updated with phase1 muons
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

            alg = AlgConf.EtCut( name = toponame, inputs = inputList, outputs = toponame )
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', 1)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET', str(d.ocut*_et_conversion))
            tm.registerTopoAlgo(alg)
            

        # (ATR-12748) fat jet trigger with Simple Cone algo
        # TODO: to be updated with phase1 jets
        algoList = [
            {"minHT": 111, "otype" : "CJ", "ocut" : 15, "olist" : "ab", "nleading" : HW.OutputWidthSelectJET, "inputwidth": HW.OutputWidthSelectJET, "oeta" : 26}, #SC111-CJ15abpETA26
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "SC%d-%s%s%s%spETA%s" % (d.minHT, d.otype, str(d.ocut), d.olist, str(d.nleading) if d.olist=="s" else "", str(d.oeta))
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.SimpleCone( name = toponame, inputs = inputList, outputs = [toponame] )
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addvariable('MinET', d.ocut)
            alg.addvariable('MinSumET', d.minHT)
            alg.addvariable('MaxRSqr', 10*10)                        
            tm.registerTopoAlgo(alg)  


 
        #  0INVM9-eEM7ab-eEMab 
        algoList = [
            {"minInvm" : 0, "maxInvm": 9, "otype" : "eEM", "ocut1" : 7, "olist" : "ab", "inputwidth": HW.OutputWidthSelectEM, "ocut2" : 0},
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
            alg = AlgConf.InvariantMassInclusive1( name = toponame, inputs = inputList, outputs = toponame)
            alg.addgeneric('InputWidth', d.inputwidth)
            alg.addgeneric('MaxTob', HW.OutputWidthSelectEM)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1',  d.ocut1*_et_conversion)
            alg.addvariable('MinET2',  d.ocut2*_et_conversion)
            alg.addvariable('MinMSqr', d.minInvm*d.minInvm*_et_conversion*_et_conversion)
            alg.addvariable('MaxMSqr', d.maxInvm*d.maxInvm*_et_conversion*_et_conversion)
            tm.registerTopoAlgo(alg)


        # added for b-phys, 0DR03-eEM7ab-CJ15ab
        # TODO: update with phase1 jets
        algoList = [  
            {"minDr": 0, "maxDr": 3, "otype1" : "eEM" ,"ocut1": 7,  "olist1" : "ab", "otype2" : "CJ", "ocut2": 15, "olist2" : "ab"} 
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDR%02d-%s%s%s-%s%s%s"  % (d.minDr, d.maxDr, d.otype1, str(d.ocut1), d.olist1, d.otype2, str(d.ocut2), d.olist2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            alg = AlgConf.DeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = [ toponame ])
            alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
            alg.addgeneric('InputWidth2', HW.OutputWidthSelectJET)
            alg.addgeneric('MaxTob1', HW.OutputWidthSelectEM)
            alg.addgeneric('MaxTob2', HW.OutputWidthSelectJET)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1',    d.ocut1*_et_conversion, 0)
            alg.addvariable('MinET2',    d.ocut2*_et_conversion, 0)
            alg.addvariable('DeltaRMin', d.minDr*d.minDr*_dr_conversion*_dr_conversion, 0)
            alg.addvariable('DeltaRMax', d.maxDr*d.maxDr*_dr_conversion*_dr_conversion, 0)
            tm.registerTopoAlgo(alg)


        # Axion 2EM DPHI  
        #27DPHI32-eEMs1-eEMs6
        algoList = [
            {"minDphi": 27,  "maxDphi": 32, "otype" : "eEM",  "ocut1" : 0,  "olist" : "s", "nleading1" : 1, "inputwidth1": HW.OutputWidthSortEM, "ocut2" : 0, "nleading2": 6},
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
            alg = AlgConf.DeltaPhiIncl1( name = toponame, inputs = inputList, outputs = toponame )
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('MaxTob', d.nleading2)
            alg.addgeneric('NumResultBits', 1)                        
            alg.addvariable('MinET1',      d.ocut1*_et_conversion if d.ocut1 > 0 else 3, 0)
            alg.addvariable('MinET2',      d.ocut2*_et_conversion if d.ocut2 > 0 else 3, 0)
            alg.addvariable('MinDeltaPhi', d.minDphi*_phi_conversion, 0)
            alg.addvariable('MaxDeltaPhi', d.maxDphi*_phi_conversion, 0)
            tm.registerTopoAlgo(alg)

        # Tau dR chains
        # TODO: use eTAU lists with or without isolation selection?
        algolist=[
            { "minDr": 0, "maxDr": 28, "otype1" : "eTAU" ,"ocut1": 20, "olist1" : "abi",
              "nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,"otype2" : "eTAU", "ocut2": 12, "olist2" : "abi",
              "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # 0DR28-eTAU20abi-eTAU12abi
            { "minDr": 0, "maxDr": 25, "otype1" : "eTAU" ,"ocut1": 20, "olist1" : "abi",
              "nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,"otype2" : "eTAU", "ocut2": 12, "olist2" : "abi",
              "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU}, # 0DR25-eTAU20abi-eTAU12abi
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
            alg = algoname( name = toponame,  inputs = inputList, outputs = [ toponame ])
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
                alg.addvariable('MinET1',    d.ocut1*_et_conversion )
                alg.addvariable('MinET2',    d.ocut2*_et_conversion )
                alg.addvariable('DeltaRMin', d.minDr*d.minDr*_dr_conversion*_dr_conversion)
                alg.addvariable('DeltaRMax', d.maxDr*d.maxDr*_dr_conversion*_dr_conversion)
            else:
                alg.addvariable('MinET1',    d.ocut1*_et_conversion , 0)
                alg.addvariable('MinET2',    d.ocut2*_et_conversion , 0)
                alg.addvariable('DeltaRMin', d.minDr*d.minDr*_dr_conversion*_dr_conversion, 0)
                alg.addvariable('DeltaRMax', d.maxDr*d.maxDr*_dr_conversion*_dr_conversion, 0)
            tm.registerTopoAlgo(alg)

        # DISAMB 3 lists with DR cut to 2nd and 3rd lists
        algolist=[
            { "disamb": 2,
              "otype1" : "eTAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "eTAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "jJ", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET,
              "drcutmin": 0, "drcutmax": 28}, # 2DISAMB-jJ25ab-0DR28-eTAU20abi-eTAU12abi
            { "disamb": 2,
              "otype1" : "eTAU",  "ocut1": 20, "olist1": "abi","nleading1": HW.OutputWidthSelectTAU, "inputwidth1": HW.OutputWidthSelectTAU,
              "otype2" : "eTAU", "ocut2": 12, "olist2": "abi", "nleading2": HW.OutputWidthSelectTAU, "inputwidth2": HW.OutputWidthSelectTAU,
              "otype3" : "jJ", "ocut3": 25, "olist3": "ab", "nleading3": HW.OutputWidthSelectJET, "inputwidth3": HW.OutputWidthSelectJET,
              "drcutmin": 0, "drcutmax": 25}, # 2DISAMB-jJ25ab-0DR25-eTAU20abi-eTAU12abi
        ]
        for x in algolist:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            obj1 = "-%s%s%s" % (d.otype1, str(d.ocut1), d.olist1)
            obj2 = "-%s%s%s" % (d.otype2, str(d.ocut2), d.olist2)
            obj3 = "%s%s%s"  % (d.otype3, str(d.ocut3), d.olist3)
            toponame = "%sDISAMB-%s-%dDR%d%s%s"  % ( str(d.disamb) if d.disamb>0 else "", obj3, d.drcutmin, d.drcutmax, obj1, obj2)
            log.debug("Define %s", toponame)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2, d.otype3 + d.olist3]
            alg = AlgConf.DisambiguationDRIncl3( name = toponame, inputs = inputList, outputs = [ toponame ])
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('InputWidth3', d.inputwidth3)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('MaxTob3', d.nleading3)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', d.ocut1*_et_conversion, 0)
            alg.addvariable('MinET2', d.ocut2*_et_conversion, 0)
            alg.addvariable('MinET3', d.ocut3*_et_conversion, 0)
            alg.addvariable('DisambDRSqrMin', d.drcutmin*d.drcutmin*_dr_conversion*_dr_conversion, 0)
            alg.addvariable('DisambDRSqrMax', d.drcutmax*d.drcutmax*_dr_conversion*_dr_conversion, 0)
            alg.addvariable('DisambDRSqr', d.disamb*d.disamb*_dr_conversion*_dr_conversion, 0)
            tm.registerTopoAlgo(alg)


        # TLA deta
        # TODO: update to phase1 jets
        algoList = [
            { "minDeta": 0,  "maxDeta": 20, "otype" : "J",  "ocut1" : 50,  "olist" : "s",
              "nleading1" : 1, "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 0, "nleading2": 2}, #0DETA20-J50s1-Js2
        ]
        for x in algoList:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            toponame = "%iDETA%i-%s%s%s%s-%s%s%s%s"  % (d.minDeta, d.maxDeta,
                                                        d.otype, d.ocut1 if d.ocut1 > 0 else "", d.olist, d.nleading1 if d.olist=="s" else "",
                                                        d.otype, d.ocut2 if d.ocut2 > 0 else "", d.olist, d.nleading2 if d.olist=="s" else "")
            log.debug("Define %s", toponame)
            inputList = d.otype + d.olist
            alg = AlgConf.DeltaEtaIncl1( name = toponame, inputs = inputList, outputs = toponame )
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('MaxTob', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            alg.addvariable('MinET1', str(d.ocut1), 0)
            alg.addvariable('MinET2', str(d.ocut2), 0)
            alg.addvariable('MinDeltaEta', d.minDeta, 0)
            alg.addvariable('MaxDeltaEta', d.maxDeta, 0)
            tm.registerTopoAlgo(alg)

        # jINVM + DPHI
        NFFDphimap = [
            { "minInvm": 400 , "minDphi": 0, "maxDphiList": [26, 24, 22, 20],
                         "otype1" : "AjJ", "ocut1" : 30, "olist1" : "s", "nleading1" : 6, "inputwidth": HW.OutputWidthSortJET,
                         "otype2" : "AjJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }
        ]
        for x in NFFDphimap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]
            toponames=[]
            for maxDphi in d.maxDphiList:
                toponames.append ("%iINVM-%iDPHI%i-%s%s%s%s-%s%s%s%s"  % (d.minInvm, d.minDphi, maxDphi,
                                                                 d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                                 d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else ""))
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'jINVM_DPHI', inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', d.inputwidth)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits',  len(toponames))
            for bitid,maxDphi in enumerate(d.maxDphiList):
                alg.addvariable('MinET1',      d.ocut1*_et_conversion , bitid)
                alg.addvariable('MinET2',      d.ocut2*_et_conversion , bitid)
                alg.addvariable('MinMSqr',     d.minInvm*d.minInvm *_et_conversion*_et_conversion , bitid)
                alg.addvariable('MaxMSqr',     _no_m_upper_threshold , bitid)  # no upper threshold
                alg.addvariable('MinDeltaPhi', d.minDphi*_phi_conversion , bitid)
                alg.addvariable('MaxDeltaPhi', maxDphi*_phi_conversion, bitid)
            tm.registerTopoAlgo(alg)


        # jINVM_NFF + DPHI
        NFFDphimap = [
            { "minInvm": 400 , "minDphi": 0, "maxDphiList": [26, 24, 22, 20],
                         "otype1" : "jJ", "ocut1" : 30, "olist1" : "s", "nleading1" : 6, "inputwidth": HW.OutputWidthSortJET,
                         "otype2" : "AjJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }
        ]
        for x in NFFDphimap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]
            toponames=[]
            for maxDphi in d.maxDphiList:
                toponames.append ("%iINVM-%iDPHI%i-%s%s%s%s-%s%s%s%s"  % (d.minInvm, d.minDphi, maxDphi,
                                                                 d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                                 d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else ""))
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'jINVM_DPHI_NFF', inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', d.inputwidth)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits',  len(toponames))
            for bitid,maxDphi in enumerate(d.maxDphiList):
                alg.addvariable('MinET1',      d.ocut1*_et_conversion , bitid)
                alg.addvariable('MinET2',      d.ocut2*_et_conversion , bitid)
                alg.addvariable('MinMSqr',     d.minInvm*d.minInvm *_et_conversion*_et_conversion , bitid)
                alg.addvariable('MaxMSqr',     _no_m_upper_threshold , bitid)  # no upper threshold
                alg.addvariable('MinDeltaPhi', d.minDphi*_phi_conversion , bitid)
                alg.addvariable('MaxDeltaPhi', maxDphi*_phi_conversion, bitid)
            tm.registerTopoAlgo(alg)

        # all-jets items
        # legacy
        invm_aj_highmass_map = { "algoname": 'INVM_AJ_HighMass', "Threlist": [ 700], "maxInvm": 9999,
                                 "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6,
                                 "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6}

        invm_aj_lowmass_map = { "algoname": 'INVM_AJ_LowMass',  "Threlist": [ 300], "maxInvm": 9999,
                                "otype" : "AJ", "ocut1" : 30, "olist" : "s", "nleading1" : 6,
                                "inputwidth1": HW.OutputWidthSortJET, "ocut2" : 20, "nleading2" : 6}

        for x in [invm_aj_highmass_map, invm_aj_lowmass_map ]:
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
            alg = AlgConf.InvariantMassInclusive1( name = d.algoname, inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth', d.inputwidth1)
            alg.addgeneric('MaxTob', d.nleading1)
            alg.addgeneric('NumResultBits', len(toponames))
            for bitid, minInvm in enumerate(d.Threlist):
                alg.addvariable('MinET1', d.ocut1, bitid)
                alg.addvariable('MinET2', d.ocut2, bitid)
                alg.addvariable('MinMSqr', minInvm * minInvm, bitid)
                alg.addvariable('MaxMSqr', d.maxInvm * d.maxInvm, bitid)
            tm.registerTopoAlgo(alg)

        # CF
        algoList = [
            {  "minInvm": 400, "otype1" : "AjJ", "ocut1": 30, "olist1" : "s", "nleading1" : 6, "inputwidth1": HW.OutputWidthSortJET,
               "otype2" : "AjJ", "ocut2": 20, "olist2" : "s", "nleading2" : 6, "inputwidth2": HW.OutputWidthSortJET, "applyEtaCut":1,
               "minEta1": 0 ,"maxEta1": 31 , "minEta2": 31 ,"maxEta2": 49 , }, #400INVM-AjJ30s6pETA31-AjJ20s6p31ETA49
        ]
        for x in algoList:
            class d: 
                pass     
            for k in x:  
                setattr (d, k, x[k])
            obj1 = "%s%s%sp%sETA%i"  % (d.otype1, str(d.ocut1), d.olist1 + (str(d.nleading1) if d.olist1.find('s')>=0 else ""),str(d.minEta1) if d.minEta1>0 else "", d.maxEta1)
            obj2 = "-%s%s%sp%sETA%i"  % (d.otype2, str(d.ocut2), d.olist2 + (str(d.nleading2) if d.olist2.find('s')>=0 else ""),str(d.minEta2) if d.minEta2>0 else "", d.maxEta2)
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist2]
            toponame = "%iINVM-%s%s"   % (d.minInvm, obj1, obj2)
            alg = AlgConf.InvariantMassInclusive2( name = toponame, inputs = inputList, outputs = toponame)
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits', 1)
            if (d.applyEtaCut>0):
                alg.addgeneric('ApplyEtaCut', d.applyEtaCut)
            alg.addvariable('MinET1',  d.ocut1*_et_conversion )
            alg.addvariable('MinET2',  d.ocut2*_et_conversion )
            alg.addvariable('MinMSqr', d.minInvm*d.minInvm*_et_conversion*_et_conversion )
            alg.addvariable('MaxMSqr', _no_m_upper_threshold )
            if (d.applyEtaCut>0):
                alg.addvariable('MinEta1', d.minEta1*_eta_conversion )
                alg.addvariable('MaxEta1', d.maxEta1*_eta_conversion )
                alg.addvariable('MinEta2', d.minEta2*_eta_conversion )
                alg.addvariable('MaxEta2', d.maxEta2*_eta_conversion )
            tm.registerTopoAlgo(alg)

        # jINVM
        NFFmap = [
            { "minInvmList": [300,400,500,700] ,
                         "otype1" : "AjJ", "ocut1" : 30, "olist1" : "s", "nleading1" : 6, "inputwidth": HW.OutputWidthSortJET,
                         "otype2" : "AjJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }
        ]
        for x in NFFmap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]
            toponames=[]
            for minInvm in d.minInvmList:
                toponames.append ("%iINVM-%s%s%s%s-%s%s%s%s"  % (minInvm,
                                                                 d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                                 d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else ""))
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'jINVM', inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', d.inputwidth)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits',  len(toponames))
            for bitid,minInvm in enumerate(d.minInvmList):
                alg.addvariable('MinET1',  d.ocut1*_et_conversion , bitid)
                alg.addvariable('MinET2',  d.ocut2*_et_conversion , bitid)
                alg.addvariable('MinMSqr', minInvm*minInvm*_et_conversion*_et_conversion , bitid)
                alg.addvariable('MaxMSqr', _no_m_upper_threshold , bitid)  # no upper threshold
            tm.registerTopoAlgo(alg)

        # jINVM_NFF
        NFFmap = [
            { "minInvmList": [300,400,500,700] ,
                         "otype1" : "jJ", "ocut1" : 30, "olist1" : "s", "nleading1" : 6, "inputwidth": HW.OutputWidthSortJET,
                         "otype2" : "AjJ", "ocut2" : 20, "olist2" : "s", "nleading2" : 6 }
        ]
        for x in NFFmap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, d.otype2 + d.olist1]
            toponames=[]
            for minInvm in d.minInvmList:
                toponames.append ("%iINVM-%s%s%s%s-%s%s%s%s"  % (minInvm, 
                                                                 d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                                 d.otype2, str(d.ocut2) , d.olist2, str(d.nleading2) if d.olist2=="s" else ""))
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'jINVM_NFF', inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth1', d.inputwidth)
            alg.addgeneric('InputWidth2', d.inputwidth)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits',  len(toponames))
            for bitid,minInvm in enumerate(d.minInvmList):
                alg.addvariable('MinET1',  d.ocut1*_et_conversion , bitid)
                alg.addvariable('MinET2',  d.ocut2*_et_conversion , bitid)
                alg.addvariable('MinMSqr', minInvm*minInvm*_et_conversion*_et_conversion , bitid)
                alg.addvariable('MaxMSqr', _no_m_upper_threshold , bitid)  # no upper threshold
            tm.registerTopoAlgo(alg)

        #ATR-19355  
        # TODO: to be updated with phase1 muons
        toponame = "0INVM10-3MU4ab"
        log.debug("Define %s", toponame)
        inputList = 'MUab'
        alg = AlgConf.InvariantMassThreeTOBsIncl( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     0*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 10*10*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',      4*_et_conversion)
        tm.registerTopoAlgo(alg)


        #ATR-18815
        # TODO: to be updated with phase1 muons
        toponame = "0INVM10-0DR15-eEM8abl-MU10ab"
        log.debug("Define %s", toponame)
        inputList = ['eEMabl','MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob1', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     0*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 10*10*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',      8*_et_conversion)
        alg.addvariable('MinET2',     10*_et_conversion)
        alg.addgeneric('ApplyEtaCut',  0)
        alg.addvariable('MinEta1',     0*_eta_conversion)
        alg.addvariable('MinEta2',     0*_eta_conversion)
        alg.addvariable('MaxEta1',    49*_eta_conversion)
        alg.addvariable('MaxEta2',    49*_eta_conversion)
        alg.addvariable('DeltaRMin',     0*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 15*15*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)


        #ATR-18815
        # TODO: to be updated with phase1 muons
        toponame = "0INVM10-0DR15-eEM12abl-MU6ab"
        log.debug("Define %s", toponame)
        inputList = ['eEMabl','MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl2( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth1', HW.OutputWidthSelectEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob1', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob2', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     0*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 10*10*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',     12*_et_conversion)
        alg.addvariable('MinET2',      6*_et_conversion)
        alg.addgeneric('ApplyEtaCut',  0)
        alg.addvariable('MinEta1',     0*_eta_conversion)
        alg.addvariable('MinEta2',     0*_eta_conversion)
        alg.addvariable('MaxEta1',    49*_eta_conversion)
        alg.addvariable('MaxEta2',    49*_eta_conversion)
        alg.addvariable('DeltaRMin',     0*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 15*15*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)


        #ATR-18824 ZAFB-DPHI
        # TODO: update with fwd electrons
        ZAFBDphimap = [
            { "minInvm": 60 , "minDphiList": [4, 25], "maxDphi": 32, "minEta2": 23, "maxEta2": 49,
              "inputwidth1": HW.OutputWidthSortEM, "otype1" : "eEM", "ocut1" : 15, "olist1" : "abm",
              "nleading1" : HW.OutputWidthSortEM, "inputwidth2": HW.OutputWidthSortJET,  "ocut2" : 15, "nleading2" : 6 }
        ]
        for x in ZAFBDphimap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = [d.otype1 + d.olist1, 'FJjs23ETA49']
            toponames=[]
            for minDphi in d.minDphiList:
                toponames.append ("%iINVM-%02dDPHI%i-%s%s%s%s-FJj%ss%s%iETA%i"  % (d.minInvm, minDphi, d.maxDphi,
                                                                                     d.otype1, str(d.ocut1) , d.olist1, str(d.nleading1) if d.olist1=="s" else "",
                                                                                     str(d.ocut2) , str(d.nleading2) , d.minEta2, d.maxEta2))
            alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = 'ZAFB_DPHI', inputs = inputList, outputs = toponames)
            alg.addgeneric('InputWidth1', d.inputwidth1)
            alg.addgeneric('InputWidth2', d.inputwidth2)
            alg.addgeneric('MaxTob1', d.nleading1)
            alg.addgeneric('MaxTob2', d.nleading2)
            alg.addgeneric('NumResultBits',  len(toponames))
            alg.addgeneric('ApplyEtaCut', 1)
            alg.addvariable('MinEta1',  0*_eta_conversion)
            alg.addvariable('MaxEta1', 49*_eta_conversion)
            alg.addvariable('MinEta2', 23*_eta_conversion)
            alg.addvariable('MaxEta2', 49*_eta_conversion)
            for bitid,minDphi in enumerate(d.minDphiList):
                alg.addvariable('MinET1',  d.ocut1*_et_conversion, bitid)
                alg.addvariable('MinET2',  d.ocut2*_et_conversion, bitid)
                alg.addvariable('MinMSqr', d.minInvm*d.minInvm*_et_conversion*_et_conversion, bitid)
                alg.addvariable('MaxMSqr', _no_m_upper_threshold, bitid)
                alg.addvariable('MinDeltaPhi', minDphi*_phi_conversion, bitid)
                alg.addvariable('MaxDeltaPhi', d.maxDphi*_phi_conversion, bitid)
            tm.registerTopoAlgo(alg)


        #ATR-19302:
        toponame = "0INVM70-27DPHI32-eEM10sm1-eEM10sm6"
        log.debug("Define %s", toponame)
        inputList = ['eEMsm','eEMsm']
        alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth1', HW.OutputWidthSortEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', 6)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     0*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 70*70*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',     10*_et_conversion)
        alg.addvariable('MinET2',     10*_et_conversion)
        alg.addgeneric('ApplyEtaCut',  1)
        alg.addvariable('MinEta1',     0*_eta_conversion)
        alg.addvariable('MaxEta1',    49*_eta_conversion)
        alg.addvariable('MinEta2',     0*_eta_conversion)
        alg.addvariable('MaxEta2',    49*_eta_conversion)
        alg.addvariable('MinDeltaPhi', 27*_phi_conversion)
        alg.addvariable('MaxDeltaPhi', 32*_phi_conversion)
        tm.registerTopoAlgo(alg)

        toponame = "0INVM70-27DPHI32-eEM12sm1-eEM12sm6"
        log.debug("Define %s", toponame)
        inputList = ['eEMsm','eEMsm']
        alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth1', HW.OutputWidthSortEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', 6)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     0*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 70*70*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',     12*_et_conversion)
        alg.addvariable('MinET2',     12*_et_conversion)
        alg.addgeneric('ApplyEtaCut',  1)
        alg.addvariable('MinEta1',     0*_eta_conversion)
        alg.addvariable('MaxEta1',    49*_eta_conversion)
        alg.addvariable('MinEta2',     0*_eta_conversion)
        alg.addvariable('MaxEta2',    49*_eta_conversion)
        alg.addvariable('MinDeltaPhi', 27*_phi_conversion)
        alg.addvariable('MaxDeltaPhi', 32*_phi_conversion)
        tm.registerTopoAlgo(alg)

        #ATR-21637
        toponame = "0INVM70-27DPHI32-eEM12s1-eEM12s6"
        log.debug("Define %s", toponame)
        inputList = ['eEMs','eEMs']
        alg = AlgConf.InvariantMassDeltaPhiInclusive2( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth1', HW.OutputWidthSortEM)
        alg.addgeneric('InputWidth2', HW.OutputWidthSortEM)
        alg.addgeneric('MaxTob1', 1)
        alg.addgeneric('MaxTob2', 6)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     0*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr', 70*70*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',     12*_et_conversion)
        alg.addvariable('MinET2',     12*_et_conversion)
        alg.addgeneric('ApplyEtaCut',  1)
        alg.addvariable('MinEta1',     0*_eta_conversion)
        alg.addvariable('MaxEta1',    49*_eta_conversion)
        alg.addvariable('MinEta2',     0*_eta_conversion)
        alg.addvariable('MaxEta2',    49*_eta_conversion)
        alg.addvariable('MinDeltaPhi', 27*_phi_conversion)
        alg.addvariable('MaxDeltaPhi', 32*_phi_conversion)
        tm.registerTopoAlgo(alg)


        #ATR-19720, L1_BPH-8M15-0DR22-2MU6
        # TODO: update with phase1 muons
        toponame = "8INVM15-0DR22-2MU6ab"
        log.debug("Define %s", toponame)
        inputList = ['MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     8*8*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr',   15*15*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',        6*_et_conversion)
        alg.addvariable('MinET2',        6*_et_conversion)
        alg.addvariable('DeltaRMin',     0*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 22*22*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)

        #ATR-19720, L1_BPH-2M9-2DR15-2MU6
        # TODO: update with phase1 muons
        toponame = "2INVM9-2DR15-2MU6ab"
        log.debug("Define %s", toponame)
        inputList = ['MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     2*2*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr',     9*9*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',        6*_et_conversion)
        alg.addvariable('MinET2',        6*_et_conversion)
        alg.addvariable('DeltaRMin',   2*2*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 15*15*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)

        #ATR-19720, L1_BPH-2M9-0DR15-MU6MU4
        # TODO: update with phase1 muons
        toponame = "2INVM9-0DR15-MU6ab-MU4ab"
        log.debug("Define %s", toponame)
        inputList = ['MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     2*2*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr',     9*9*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',        6*_et_conversion)
        alg.addvariable('MinET2',        4*_et_conversion)
        alg.addvariable('DeltaRMin',     0*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 15*15*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)

        #ATR-19720, L1_BPH-8M15-0DR22-MU6MU4-BO
        # TODO: update with phase1 muons
        toponame = "8INVM15-0DR22-MU6ab-MU4ab"
        log.debug("Define %s", toponame)
        inputList = ['MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     8*8*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr',   15*15*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',        6*_et_conversion)
        alg.addvariable('MinET2',        4*_et_conversion)
        alg.addvariable('DeltaRMin',     0*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 22*22*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)

        #ATR-19720, L1_BPH-2M9-0DR15-2MU4
        # TODO: update with phase1 muons
        toponame = "2INVM9-0DR15-2MU4ab"
        log.debug("Define %s", toponame)
        inputList = ['MUab']
        alg = AlgConf.InvariantMassInclusiveDeltaRSqrIncl1( name = toponame, inputs = inputList, outputs = toponame )
        alg.addgeneric('InputWidth', HW.OutputWidthSelectMU)
        alg.addgeneric('MaxTob', HW.OutputWidthSelectMU)
        alg.addgeneric('NumResultBits', 1)
        alg.addvariable('MinMSqr',     2*2*_et_conversion*_et_conversion)
        alg.addvariable('MaxMSqr',     9*9*_et_conversion*_et_conversion)
        alg.addvariable('MinET1',        4*_et_conversion)
        alg.addvariable('MinET2',        4*_et_conversion)
        alg.addvariable('DeltaRMin',     0*_dr_conversion*_dr_conversion)
        alg.addvariable('DeltaRMax', 15*15*_dr_conversion*_dr_conversion)
        tm.registerTopoAlgo(alg)
        
        # CEP_CJ
        # TODO: update with phase1 jets, what conversion for Xi?
        CEPmap = [
            {"algoname": 'CEP_CJ', "minETlist": [50, 60]}
        ]
        for x in CEPmap:
            class d:
                pass
            for k in x:
                setattr (d, k, x[k])
            inputList = ['CJs']
            toponames=[]
            for minET in d.minETlist:  # noqa: F821
                toponames.append ("CEP-CJ%is6" % (minET))     # noqa: F821 
            alg = AlgConf.ExclusiveJets( name = d.algoname, inputs = inputList, outputs = toponames) # noqa: F821
            alg.addgeneric('InputWidth', HW.InputWidthJET) # noqa: F821
            alg.addgeneric('MaxTob', HW.InputWidthJET)       # noqa: F821
            alg.addgeneric('NumResultBits',  len(toponames)) # noqa: F821
            for bitid,minET in enumerate(d.minETlist):  # noqa: F821
                alg.addvariable('MinET1', minET, bitid)# noqa: F821
            alg.addvariable('MinXi', 13000.0*0.02, bitid) # noqa: F821
            alg.addvariable('MaxXi', 13000.0*0.05, bitid) # noqa: F821
            tm.registerTopoAlgo(alg)
