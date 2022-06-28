#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration #
#

from AthenaConfiguration.ComponentFactory import CompFactory

def sTgcMonitoringConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''
    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    
    # Make sure muon geometry is configured
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags, 'StgcAthMonitorCfg')

    # Adding an algorithm to the helper.
    sTgcMonAlg = helper.addAlgorithm(CompFactory.sTgcRawDataMonAlg,'sTgcMonAlg')
    sTgcMonAlg.dosTgcESD = True  

    # Add a generic monitoring tool (a "group" in old language). The returned      
    # object here is the standard GenericMonitoringTool. 

    sTgcGroup = helper.addGroup(sTgcMonAlg,'sTgcMonitor','Muon/MuonRawDataMonitoring/sTgc/')

    # Configure histograms
    # Overview histograms
    sTgcGroup.defineHistogram('strip_times;Strip_Time', type = 'TH1F', title = 'Strip Time; Strip Time [ns];Number of Entries', path = 'Overview', xbins = 20, xmin = 0., xmax = 100.) 
    sTgcGroup.defineHistogram('strip_charges;Strip_Charge', type = 'TH1F', title = 'Strip Charge; Strip Charge [fC];Number of Entries', path = 'Overview', xbins = 200, xmin = 0., xmax = 1000.) 
    sTgcGroup.defineHistogram('strip_number;Strip_Number', type = 'TH1F', title = 'Strip Number; Strip number;Number of Entries', path = 'Overview', xbins = 20, xmin = 0., xmax = 400.) 
    sTgcGroup.defineHistogram('charge_all;Charge', type = 'TH1F', title = 'Charge;Charge[fC];Number of Entries', path = 'Overview', xbins = 100, xmin = 0., xmax = 1000.) 
    sTgcGroup.defineHistogram('x_mon,y_mon;Posx_vs_Posy', type = 'TH2F', title="Posx vs Posy;sTgc-GlobalX [mm];sTgc-GlobalY [mm];", path = 'Overview', xbins = 500, xmin = -5000, xmax = 5000., ybins = 500, ymin = -5000., ymax = 5000.) 
    sTgcGroup.defineHistogram('R_mon,z_mon;R_vs_Posz', type = 'TH2F', title = "R vs Posz; sTgc-GlobalR [mm]; sTgc-GlobalZ [mm];", path = 'Overview', xbins = 500, xmin = 0., xmax = 5000., ybins = 1000, ymin = -10000., ymax = 10000.) 
    sTgcGroup.defineHistogram('numberofstrips_percluster;Number_of_strips_percluster', type = 'TH1F', title = 'Number of strips per cluster;Number of strips;Number of Entries', path = 'Overview', xbins = 12, xmin = 0., xmax = 12.) 
    sTgcGroup.defineHistogram('time_all;Time', type = 'TH1F', title = 'Time;Time[ns];Number of Entries', path = 'Overview', xbins = 5, xmin = 0., xmax = 5.) 

    side = ["CSide", "ASide"]
    stationPhiMax = 16
    stationEtaMax = 3

    for iside in side:
        sTgc_SideGroup = "sTgc_sideGroup{0}".format(iside)
        stgcSideGroup    = helper.addGroup(sTgcMonAlg, sTgc_SideGroup, "Muon/MuonRawDataMonitoring/sTgc/" + iside)
        for multip in range(1, 3):
            for gasgap in range(1, 5):
                title_chargePad_phi_vs_eta = f'Charge (pad): {iside} Multiplet {multip} Gas gap {gasgap}; stationPhi; stationEta; Total charge [fC]'
                var_chargePad_phi_vs_eta = f'sector_{iside}_phi_multiplet_{multip}_gasgap_{gasgap}, sector_{iside}_eta_multiplet_{multip}_gasgap_{gasgap};ChargePad_vs_phi_vs_eta_{iside}_multiplet_{multip}_gasgap_{gasgap}'

                title_chargeStrip_phi_vs_eta = f'Charge (strip): {iside} Multiplet {multip} Gas gap {gasgap}; stationPhi; stationEta; Total charge [fC]'
                var_chargeStrip_phi_vs_eta = f'sector_{iside}_phi_multiplet_{multip}_gasgap_{gasgap}, sector_{iside}_eta_multiplet_{multip}_gasgap_{gasgap};ChargeStrip_vs_phi_vs_eta_{iside}_multiplet_{multip}_gasgap_{gasgap}'

                title_chargeWire_phi_vs_eta = f'Charge (wire): {iside} Multiplet {multip} Gas gap {gasgap}; stationPhi; stationEta; Total charge [fC]'
                var_chargeWire_phi_vs_eta = f'sector_{iside}_phi_multiplet_{multip}_gasgap_{gasgap}, sector_{iside}_eta_multiplet_{multip}_gasgap_{gasgap};ChargeWire_vs_phi_vs_eta_{iside}_multiplet_{multip}_gasgap_{gasgap}'
                if (f'{iside}' == 'ASide'):
                    stgcSideGroup.defineHistogram(var_chargePad_phi_vs_eta, type = 'TH2F', title = title_chargePad_phi_vs_eta, path = 'Summary', xbins = stationPhiMax, xmin = 1., xmax = float(stationPhiMax + 1), ybins = stationEtaMax, ymin = 1., ymax = float(stationEtaMax + 1), opt = 'kAlwaysCreate', weight = f'charge_pad_{iside}_multiplet_{multip}_gasgap_{gasgap}')
                    stgcSideGroup.defineHistogram(var_chargeStrip_phi_vs_eta, type = 'TH2F', title = title_chargeStrip_phi_vs_eta, path = 'Summary', xbins = stationPhiMax, xmin = 1., xmax = float(stationPhiMax + 1), ybins = stationEtaMax, ymin = 1., ymax = float(stationEtaMax + 1), opt = 'kAlwaysCreate', weight = f'charge_strip_{iside}_multiplet_{multip}_gasgap_{gasgap}')
                    stgcSideGroup.defineHistogram(var_chargeWire_phi_vs_eta, type = 'TH2F', title = title_chargeWire_phi_vs_eta, path = 'Summary', xbins = stationPhiMax, xmin = 1., xmax = float(stationPhiMax + 1), ybins = stationEtaMax, ymin = 1., ymax = float(stationEtaMax + 1), opt = 'kAlwaysCreate', weight = f'charge_wire_{iside}_multiplet_{multip}_gasgap_{gasgap}')
                else:
                    stgcSideGroup.defineHistogram(var_chargePad_phi_vs_eta, type = 'TH2F', title = title_chargePad_phi_vs_eta, path = 'Summary', xbins = stationPhiMax, xmin = 1., xmax = float(stationPhiMax + 1), ybins = stationEtaMax, ymin = -float(stationEtaMax + 1), ymax = -1., opt = 'kAlwaysCreate', weight = f'charge_pad_{iside}_multiplet_{multip}_gasgap_{gasgap}')
                    stgcSideGroup.defineHistogram(var_chargeStrip_phi_vs_eta, type = 'TH2F', title = title_chargeStrip_phi_vs_eta, path = 'Summary', xbins = stationPhiMax, xmin = 1., xmax = float(stationPhiMax + 1), ybins = stationEtaMax, ymin = -float(stationEtaMax + 1), ymax = -1., opt = 'kAlwaysCreate', weight = f'charge_strip_{iside}_multiplet_{multip}_gasgap_{gasgap}')
                    stgcSideGroup.defineHistogram(var_chargeWire_phi_vs_eta, type = 'TH2F', title = title_chargeWire_phi_vs_eta, path = 'Summary', xbins = stationPhiMax, xmin = 1., xmax = float(stationPhiMax + 1), ybins = stationEtaMax, ymin = -float(stationEtaMax + 1), ymax = -1., opt = 'kAlwaysCreate', weight = f'charge_wire_{iside}_multiplet_{multip}_gasgap_{gasgap}')
               
                for phiStation in range(1, stationPhiMax + 1):
                    title_stationEta_vs_stripNumber_vs_chargeStrip_eachPhi = f'Station eta vs strip number vs charge (strip): {iside} Multiplet {multip} Gas gap {gasgap} stationPhi {phiStation}; stationEta; Strip number; Total charge [fC]'
                    var_stationEta_vs_stripNumber_vs_chargeStrip_eachPhi = f'sector_{iside}_eta_multiplet_{multip}_gasgap_{gasgap}_stationPhi_{phiStation}, stripNumber_strip_{iside}_multiplet_{multip}_gasgap_{gasgap}_stationPhi_{phiStation};StationEta_vs_stripNumber_vs_chargeStrip_{iside}_multiplet_{multip}_gasgap_{gasgap}_stationPhi_{phiStation}'
                    
                    if (f'{iside}' == 'ASide'):
                        stgcSideGroup.defineHistogram(var_stationEta_vs_stripNumber_vs_chargeStrip_eachPhi, type = 'TH2F', title = title_stationEta_vs_stripNumber_vs_chargeStrip_eachPhi, path = 'Summary', xbins = stationEtaMax, xmin = 1., xmax = float(stationEtaMax + 1), ybins = 400, ymin = 0., ymax = 400., opt = 'kAlwaysCreate', weight = f'charge_strip_{iside}_multiplet_{multip}_gasgap_{gasgap}_stationPhi_{phiStation}')
                    else:
                        stgcSideGroup.defineHistogram(var_stationEta_vs_stripNumber_vs_chargeStrip_eachPhi, type = 'TH2F', title = title_stationEta_vs_stripNumber_vs_chargeStrip_eachPhi, path = 'Summary', xbins = stationEtaMax, xmin = -float(stationEtaMax + 1), xmax = -1., ybins = 400, ymin = 0., ymax = 400., opt = 'kAlwaysCreate', weight = f'charge_strip_{iside}_multiplet_{multip}_gasgap_{gasgap}_stationPhi_{phiStation}')

    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import argparse
    
    # If you want to run the package in a standalone way you have to execute the command: 
    # python3 -m StgcRawDataMonitoring.StgcMonitorAlgorithm --events NumberOfEvents --samples PathToInputSamples --output OutputROOTFile
    # in the run folder
    
    parser = argparse.ArgumentParser()
    parser.add_argument("--events", default = 100, type = int, help = 'Number of events that you want to run.')
    parser.add_argument("--samples", nargs = "+", default = None, help = 'Path to the input samples. If you want to run multiple samples at once you have to introduce them separated by blank spaces.')
    parser.add_argument("--output", default = "monitor_sTgc.root", help = 'Name of the output ROOT file.')
    args = parser.parse_args()

    ConfigFlags.Input.Files = []
    ConfigFlags.Input.Files += args.samples 
    
    ConfigFlags.Output.HISTFileName = args.output

    ConfigFlags.Detector.GeometrysTGC = True
    ConfigFlags.DQ.useTrigger = False

    ConfigFlags.lock()
    ConfigFlags.dump()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    sTgcMonitorAcc  =  sTgcMonitoringConfig(ConfigFlags)
    sTgcMonitorAcc.OutputLevel = 2
    cfg.merge(sTgcMonitorAcc)           
    
    # number of events selected in the ESD
    cfg.run(args.events)


