# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Filters for roughly mapping algorithms to domains. Originally based
# on "job subtraction" of python/DomainCalc_RAWtoESDTrf.py, but
# several was added by hand. This is not a very maintainable mapping,
# but it is very very useful.
# Todo: Make it easy to update the filters.
#
# Author: Thomas Kittelmann
# First version: April 2011

def filter_alg_to_pmbdomains_rawtoesd(a):
    if a.startswith("ToolSvc."):
        a=a[8:]
    s,e=a.startswith,a.endswith
    def c(substr): return substr in a
    if a=='EMTopo35ClusterMaker': return 'egamma'
    if s('InDetTrig'): return 'trigef'
    if s('MuonCombinedFit'): return 'mucomb'
    if s('MuPat'):
        return 'muonex'
    if a in ['ComposeMuonCollection','BuildCaloMuonCollection','MuonCombinedPhysicsDressing',
             'MuonCrossCollectionLinker','CloseMuonCollections','StatCombination','StatCombMaster']: return 'mucomb'
    if c('Mon') and c('Man') or a in ['InDetGlobalManager','HadCell2TopoClusterMapper','HadTopoCluster']:
        if s('HLT') or s('L1') or s('CT'): return 'trigmon'
        return 'mon'
    if s('tau') or e('tau') or s('Tau') or s('PhotonConversionPID') or c('ForTau'): return 'tau'
    if s('StreamESD') or s('StreamAOD') or e('NtupleWriter'): return 'out'
    if s('InDet'): return 'idex'
    if s('Tile') or s('Calo'):
        if s('CaloTrk'): return 'mucomb'
        return 'caloex'
    if a in ['MuonRdoToMuonDigit','JetElementMaker','ROD','StreamBS']: return 'trigbase'
    if a=='MuonSpShowerBuilder': return 'jet'
    if c('Moore') or s('Mboy') or s('Muon') or s('Moo'): return 'muonex'
    if s('InitializeMuon'): return 'mucomb'
    if s('MET'): return 'etmiss'
    if s('Muid') or s('MuGirl') or s('Staco') or s('MuTag'): return 'mucomb'
    if s('egamma') or s('softe'): return 'egamma'
    if s('AntiKt'): return 'jet'
    if e('TowerBldr'):
        if s('Em'): return 'caloex'
        return 'jet'
    if s('Ath') and e('Seq'): return 'core'
    if a in ['EventCounter','BackgroundWordFiller','CreateLumiBlockCollectionFromFile']: return 'core'
    if s('L2') or s('T2') or e('L2'): return 'triglvl2'
    if s('L1'): return 'trigbase'
    if s('RoI'): return 'trigbase'
    if e('RoI'): return 'triglvl2'
    if c('_'):
        if s('EF') or c('_EF'): return 'trigef'
        if s('L1'): return 'trigbase'
        if c('Stream_') and (e('Filter') or e('Prescaler')): return 'trigout'
        if c('_TriggerFilter_') or (c('Stream') and e('AcceptEvent')): return 'trigout'
        if s('StreamDESD_'): return 'trigout'
        if c('_Prescale'): return 'trigout'
    if c('Trig'): return 'trigmisc'
    if s('CTPSim'): return 'trigbase'
    if a=='LArCollisionTimeAlg': return 'mon'#special (in multiple domains)
    if s('LAr'): return 'caloex'
    if s('Csc') or s('Tgc') or s('Rpc') or s('Mdt'): return 'muonex'
    if a=='JetGlobalEventSetup': return 'core'#seems wrong, should make a bug report.
    if a=='MBTSTimeDiffEventInfoAlg' or s('EMCell2TopoCluster'): return 'caloex'
    if a in ['RawInfoSummaryForTagWriter']: return 'trigbase'
    if a in ['MakeAODMuons','MergeMuonCollections','FinalizeMuonClusters']: return 'mucomb'
    if e('Maker'): return 'trigbase'
    #Preliminary, I did not make these with python/DomainCalc_RAWtoESDTrf.py:
    if e('Cnv') or a=='AllExecutedEvents': return 'out'
    if s('Stream') or s('DESD'): return 'out'
    if s('Zdc'): return 'caloex'
    if s('MBTSTimeFilterInCollisionStream'): return 'core'
    if s('DPD_') or c('D3PD'): return 'out'
    if e('Filter') or c('_MuFilter_'): return 'out'
    if a in ['SkimmingCycleDefiner',
             'ThinMuonTracksInEGammaStream',
             'MUONCommStream_MuonFilterTrackID']: return 'out'
    if a in ['KinkedTrack_MultiJetMetFilter']: return 'out'
    if e('MuonTrackSlimmer'): return 'muonex'
    if c('Stream_') or e('Stream'): return 'out'
    if a=='nonalg': return None
    if a=='SlimTrackParticles': return 'idex'
    if s('LumiBlockMuWriter'): return 'core'
    if s('eflow'): return 'tau'
    if c('Muid') or c('MuGirl') or c('Staco') or c('MuTag'): return 'mucomb'
    if a in ['BuildCaloCollection']: return 'mucomb'
    if s('RecoTimer') or a=='BeamBackgroundFiller': return 'core'
    if s('Bcm'): return 'idex'
    #DR some more for HI
    if c('CSC'): return 'mucomb'
    if c('Csc'): return 'mucomb'
    if c('CombMuon'): return 'mucomb'
    if c('Ppm'): return 'core'
    if s('HIGlobalEt'): return 'caloex'
    if s('empid'): return 'egamma'
    if s('MagField'): return 'core'
    if c('InnerStations'): return 'muonex'
    if c('SCT'): return 'idex'
    if c('Mboy'): return 'muonex'
    if c('CellBkg'): return 'caloex'
    if c('SoftElectronTag'): return 'jet'
    if s('TRT'): return 'idex'
    if s('Pixel'): return 'idex'
    if s('PRD'): return 'idex'
    if c('Jet'): return 'jet'
    if c('Tower'): return 'caloex'
    if c('Analysis'): return 'other'
    if s('eg'): return 'egamma'
    if c('EF'): return 'trigef'
    if c('HLT'): return 'trigef'
    if c('LVL1'): return 'trigbase'
    if c('Mon'): return 'mon'
    if s('Comb'): return 'mucomb'
    if c('InDet'): return 'idex'
    if c('SVF'): return 'jet'
    if c('EM'): return 'egamma'
    if c('Leibler'): return 'caloex'
    if c('EnergyInCalo'): return 'mucomb'
    if c('eV_STRATEGY_'): return 'triglvl2'
    if c('PT_STRATEGY_'): return 'triglvl2'
    if c('Strategy'): return 'caloex'
    if c('IP1'): return 'jet'
    if c('IP2'): return 'jet'
    if c('IP3'): return 'jet'
    if c('SV2'): return 'jet'
    if c('SV1'): return 'jet'
    if c('ByteStream'): return 'trigbase'
    if c('CoolHist'): return 'core'
    if c('muFast'): return 'trigef'
    if c('TrackExtrapolatorToCalo'): return 'egamma'
    if c('CloseComponent'): return 'caloex'
    if c('Jem'): return 'jet'
    if c('CrossDistancesSeed'): return 'caloex'
    if c('EventSelector'): return 'core'
    if c('BeamCond'): return 'core'
    if c('TrackSplitter'): return 'idex'
    if c('SVTag'): return 'jet'
    if c('Mdt'): return 'muonex'
    if c('AtlasHole'): return 'idex'
    if c('DQ'): return 'mon'
    if c('brem'): return 'egamma'
    if c('GSF'): return 'egamma'
    if c('AtlasMagnetic'): return 'core'
    if c('softe'): return 'egamma'
    if c('Softe'): return 'egamma'
    if c('KalmanVertex'): return 'idex'
    if c('EventCnv'): return 'core'
    if c('SoftMuon'): return 'jet'
    if c('DataModel'): return 'core'
    if c('MCTBEntry'): return 'other'
    if c('Muonboy'): return 'muonex'
    if c('SeedNewton'): return 'idex'
    if c('TrackCounting'): return 'idex'
    if c('MultipleScatt'): return 'idex'
    if c('Val'): return 'mon'
    if c('caloIso'): return 'mucomb'
    if c('RawInfo'): return 'other'
    if c('TBCalo'): return 'caloex'
    if c('TGC'): return 'muonex'
    if c('TRT'): return 'idex'
    if c('LV1'): return 'trigbase'
    if c('RPC'): return 'muonex'
    if c('mdt'): return 'muonex'
    if c('MDT'): return 'muonex'
    if c('Perf'): return 'mon'
    if c('CombinedMuon'): return 'mucomb'
    if c('exToCalo'): return 'egamma'
    if c('AppMgr'): return 'core'
    if c('bbNN'): return 'jet'
    if c('Track'): return 'idex'
    if c('Trk'): return 'idex'
    if c('UserDataSvc'): return 'core'
    if c('JobID'): return 'core'
    if c('em'): return 'egamma'
    if c('Cell'): return 'caloex'
    if c('IOVSvc'): return 'core'
    if c('Electron'): return 'egamma'
    if c('Ath'): return 'core'
    if c('BTag'): return 'jet'
    if c('CaloDepth'): return 'egamma'
    if c('NnCl'): return 'jet'
    if c('myUtils'): return 'other'
    if c('Stream'): return 'other'
    if c('CPHelper'): return 'other'
    if c('OQ'): return 'caloex'
    if c('iso'): return 'egamma'
    if c('Intersector'): return 'idex'
    if c('Truth'): return 'core'
    if c('Io'): return 'core'
    if c('Gsf'): return 'egamma'
    if c('Lumi'): return 'core'
    if c('Tag'): return 'other'
    if c('Pixel'): return 'idex'
    if c('EnergyLoss'): return 'muonex'
    if c('CutFlow'): return 'core'
    if c('RndmGenSvc'): return 'core'
    if c('Topo'): return 'caloex'
    if c('topo'): return 'caloex'
    if c('PyComponentMgr'): return 'core'
    if c('FinderName'): return 'mon'
    if c('CTB'): return 'idex'
    if c('ZDC'): return 'caloex'
    if c('BeamPipe'): return 'core'
    if c('ToT'): return 'idex'
    if c('IssueLogger'): return 'core'
    if c('Eloss'): return 'idex'
    if c('Vx'): return 'idex'
    if c('StoreGate'): return 'core'
    if c('Photon'): return 'egamma'
    if c('electron'): return 'egamma'
    if c('Store'): return 'core'
    if c('PartPropSvc'): return 'core'
    if c('WeightBasic'): return 'core'
    if c('VKal'): return 'idex'
    if c('Vtx'): return 'idex'
    if c('ThinningSvc'): return 'core'
    if c('Muon'): return 'muonex'
    if c('MetaData'): return 'core'
    if c('IMO'): return 'mucomb'
    if c('ROB'): return 'trigbase'
    if c('Stau'): return 'mucomb'
    if c('DAF'): return 'egamma'
    if c('Material'): return 'idex'
    if c('Event'): return 'out'
    if c('McAodBuilder'): return 'core'
    if c('Extrapolor'): return 'idex'
    if c('Extrapolat'): return 'idex'
    if c('MuPat'): return 'muonex'
    if c('GlobalFit'): return 'idex'
    if c('RDB'): return 'core'
    if c('Navigator'): return 'idex'
    if c('Histogram'): return 'core'
    if c('HVCorrTool'): return 'caloex'
    if c('Trajectory'): return 'idex'
    if c('Segments'): return 'muonex'
    if c('Stations'): return 'muonex'
    if c('Flow'): return 'caloex'
    if c('FilterTool'): return 'core'
    if c('Decision'): return 'core'
    if c('DSConfig'): return 'core'
    if c('AtlasGeometry'): return 'core'
    if c('DetStatus'): return 'core'
    if c('NSi'): return 'idex'
    if c('DBReplicaSvc'): return 'core'
    if c('Propagator'): return 'idex'
    if c('JobInfo'): return 'core'
    if c('DetDescr'): return 'core'
    if c('Forwards'): return 'other'
    if c('CondProxy'): return 'core'
    if c('IOVDb'): return 'core'
    if c('MuLUT'): return 'trigbase'
    if c('NeuralNetworkToHisto'): return 'other'
    if c('iPat'): return 'idex'
    if c('T0'): return 'muonex'
    if c('Cosmics_MagField'): return 'muonex'
    if c('GoodRun'): return 'core'
    if c('MultiComponent'): return 'muonex'
    if c('ToolConstants'): return 'caloex'
    if c('PedestalTool'): return 'other'
    if c('PosteriorWeights'): return 'other'
    if c('ResidualPull'): return 'other'
    if c('LayerArray'): return 'other'
    if c('BadChan'): return 'caloex'
    if c('CpmMapping'): return 'other'
    if c('BunchCrossing'): return 'core'
    if c('convUtils'): return 'core'
    if c('BadChan'): return 'caloex'
    if c('MPV'): return 'idex'
    if c('KOL'): return 'other'
    if c('StateMode'): return 'other'
    if c('MGM'): return 'caloex'
    if c('Central'): return 'caloex'
    if c('IDSCAN'): return 'triglvl2'
    if s('HLT'): return 'trigef'
    if s('MucombHypo') or s('EtaHypo') or s('MufastHypo'): return 'trigef'
    if s('terminateAlgo'): return 'triglvl2'
    if c('Sf'): return 'other'
    if c('Moo'): return 'muonex'
    if c('StatComb'): return 'mucomb'
#remaining services must be core
    if c('Svc'): return 'core'

    print "WARNING: Could not assign algdomain for",a

map_pmbdomain2plotname={'out':'Persistency','jet':'Jet+Btag','mon': 'DQ monitoring','muonex': 'Muon',
                        'idex': 'Inner detector','mucomb': 'Combined muon','caloex': 'Calo',
                        'core':'Other','tau':'Tau','etmiss':'Etmiss','egamma':'EGamma',
                        'trigout':'Persistency','trigmon':'DQ monitoring',
                        'trigmisc':'Trigger','trigef':'Trigger','triglvl2':'Trigger','trigbase':'Trigger','other':'Other'}
domains_for_plots=list(set(map_pmbdomain2plotname.values()))
domains_for_plots.remove('Other')#should we really remove this?

def filter_alg_to_public_plot_domain(alg,stepname=None):
    d=filter_alg_to_pmbdomains_rawtoesd(alg)
    if d==None: return 'Other'
    x= map_pmbdomain2plotname.get(d)
    if x==None:
        x="Unknown"
        raise Exception,"WARNING unknown category %s for %s" % (d,alg)

    return x

def generate_public_plot_filter(domain):
    #call e.g. with domain='Persistency' to get a filter which returns
    #False if the alg is not in Persistency, and true otherwise
    def algfilter(name,stepname=None):
        d=filter_alg_to_public_plot_domain(name)
        if d!=domain: return False
        return name
    return algfilter

if __name__=='__main__':
    import gzip, cPickle
    res=cPickle.load(gzip.open('domain_info_algs.dat.gz'))
    for a,doms in res['alg2doms'].items():
        domguess=filter_alg_to_pmbdomains_rawtoesd(a)
        if not domguess in doms:
            if domguess!='trigmisc' and not 'triglvl2' in doms and not 'trigout' in doms and not 'trigef' in doms:
                print "WARNING: Domain %s assigned to %s not in allowed list: %s"%(a,domguess,' '.join(doms))
