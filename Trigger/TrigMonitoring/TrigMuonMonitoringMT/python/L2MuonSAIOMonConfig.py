#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigMuonMonitoringMT.TrigMuonEfficiencyMonConfig import regex

def L2MuonSAIOMonConfig(helper):
    
    from AthenaConfiguration.ComponentFactory import CompFactory

    GroupName = 'L2MuonSAIO'

    monAlg = helper.addAlgorithm(CompFactory.L2MuonSAIOMonMT,'L2MuonSAIOMonMT')
    monAlg.MonitoredChains = ['HLT_2mu6_l2io_L12MU6', 'HLT_2mu14_l2io_L12MU10']
    monAlg.Group = GroupName

    for chain in monAlg.MonitoredChains:

        histGroup = helper.addGroup(monAlg, GroupName+'_'+chain, 'HLT/MuonMon/L2MuonSAIO/'+chain)
        threshold, level1 = regex('HLT_2mu([0-9]+).*_(L12MU[0-9]+)').match(chain).groups()
        monAlg.Plateaus.append( float(threshold) )
 
        histGroup.defineHistogram(chain+'_passIOchain,'+chain+'_OffdR;EffL2SAIO_OffdR',
                                  title='L2MuonSAIO Efficiency dR of offline muon pair '+chain+';dR;Efficiency', 
                                  type='TEfficiency', path='',xbins=100,xmin=0.,xmax=0.5)

        histGroup.defineHistogram(chain+'_OffdR;L2SAIO_OffdR',
                                  title='L2MuonSAIO dR of offline muon pair '+chain+';dR;Events', 
                                  type='TH1F', path='',xbins=100,xmin=0.,xmax=0.5)

    return
