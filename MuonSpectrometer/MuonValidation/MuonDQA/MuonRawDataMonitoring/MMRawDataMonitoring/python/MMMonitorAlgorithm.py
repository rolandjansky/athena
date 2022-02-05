#
#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from MMMonUtils import getMMLabel,getMMLabelY

def MMMonitoringConfig(inputFlags):
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

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'MMAthMonitorCfg')
    
    # Adding an algorithm to the helper. 

    mmMonAlg = helper.addAlgorithm(CompFactory.MMRawDataMonAlg,'MMMonAlg')
    mmMonAlg.DoMMESD = True    
    
    # Add a generic monitoring tool (a "group" in old language). The returned                                
    # object here is the standard GenericMonitoringTool. 

    mmGroup = helper.addGroup(mmMonAlg,'mmMonitor','Muon/MuonRawDataMonitoring/MM/')
    
    # Configure histograms

    # Overview histograms
    mmGroup.defineHistogram('residual;Residuals',  type='TH1F',  title='Residuals;res[mm];Number of Entries',
                        path='Overview',   xbins=200, xmin=-10, xmax=10.
    )


    mmGroup.defineHistogram('residual,eta_trk;Res_vs_eta', type='TH2F', title="Res vs Eta;Res;Eta;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=100, ymin=-3.,ymax=3.)
    mmGroup.defineHistogram('residual,phi_trk;Res_vs_phi', type='TH2F', title="Res vs Eta;Res;Phi;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=16, ymin=-3.14,ymax=3.14)
    mmGroup.defineHistogram('residual,stPhi_mon;Res_vs_stPhi', type='TH2F', title="Res vs Eta;Res;stPhi;", path='Overview',xbins=100, xmin=-10, xmax=10., ybins=16, ymin=0,ymax=16)
    
    mmGroup.defineHistogram('charge_all;Charge',  type='TH1F',
                            title='Charge;Charge[fC];Number of Entries',
                        path='Overview',   xbins=120, xmin=0., xmax=1200.
                       )

    mmGroup.defineHistogram('x_mon,y_mon;Posy_vs_Posx', type='TH2F', title="Posy vs Posx;MM-GlobalX [mm];MM-GlobalY [mm];", path='Overview',xbins=500, xmin=-5000, xmax=5000., ybins=500, ymin=-5000.,ymax=5000.)
    
    mmGroup.defineHistogram('R_mon,z_mon;Posz_vs_R', type='TH2F', title="Posz vs R;MM-GlobalR [mm];MM-GlobalZ [mm];", path='Overview',xbins=500, xmin=0, xmax=5000., ybins=1000, ymin=-8000 ,ymax=8000)
    
    mmGroup.defineHistogram('numberofstrips_percluster;Number_of_strips_percluster',  type='TH1F',
                        title='Number of strips per cluster;Number of strips;Number of Entries',
                            path='Overview',   xbins=20, xmin=0., xmax=20.)
    
    mmGroup.defineHistogram('mu_TPC_angle;uTPC_angle',  type='TH1F',
                            title='#mu TPC angle;#mu TPC angle [degrees];Number of Entries',
                            path='Overview',   xbins=2000, xmin=-100, xmax=100)

    mmGroup.defineHistogram('mu_TPC_chi2;uTPC_chi2',  type='TH1F',
                        title='#mu TPC #chi2; #mu TPC #chi2;Number of Entries',
                        path='Overview',   xbins=100, xmin=0., xmax=1.)

    mmGroup.defineHistogram('time_all;Time',  type='TH1F',
                        title='Time;Time[ns];Number of Entries',
                        path='Overview',   xbins=1000, xmin=0., xmax=10000.)

    mmGroup.defineHistogram('statEta_strip,strip_number;Strip_Numbers_vs_StationEta',    type='TH2F',
                            title='Strip Numbers vs Station Eta;; Strip Numbers;',
                            path='Overview',   xbins=5, xmin=-2, xmax=3., xlabels=['#eta-2','#eta-1','','#eta1','#eta2'], ybins=5120, ymin=0., ymax=5120.)

    thisLabelx,thisLabely=getMMLabel("x_lab_occ_etaminus1","y_lab_occ_etaminus1")

    mmGroup.defineHistogram('sector_CSide_eta1,stationPhi_CSide_eta1;Occupancy_CSide_eta1_PCB', type='TH2F', title='Occupancy CSide eta1 PCB; ; ;', path='Occupancy', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx, ylabels=thisLabely)

    mmGroup.defineHistogram('sector_CSide_eta1_ontrack,stationPhi_CSide_eta1_ontrack;Occupancy_CSide_eta1_PCB_ontrack', type='TH2F', title='Occupancy CSide eta1 PCB ontrack; ; ;', path='Occupancy_ontrack', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx, ylabels=thisLabely)

    thisLabelx1,thisLabely1=getMMLabel("x_lab_occ_etaminus2","y_lab_occ_etaminus2")

    mmGroup.defineHistogram('sector_CSide_eta2,stationPhi_CSide_eta2;Occupancy_CSide_eta2_PCB', type='TH2F', title='Occupancy CSide eta2 PCB; ; ;', path='Occupancy', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx1, ylabels=thisLabely1)

    mmGroup.defineHistogram('sector_CSide_eta2_ontrack,stationPhi_CSide_eta2_ontrack;Occupancy_CSide_eta2_PCB_ontrack', type='TH2F', title='Occupancy CSide eta2 PCB ontrack; ; ;', path='Occupancy_ontrack', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx1, ylabels=thisLabely1)

    thisLabelx2,thisLabely2=getMMLabel("x_lab_occ_eta1","y_lab_occ_eta1")

    mmGroup.defineHistogram('sector_ASide_eta1,stationPhi_ASide_eta1;Occupancy_ASide_eta1_PCB', type='TH2F', title='Occupancy ASide eta1 PCB; ; ;', path='Occupancy', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx2, ylabels=thisLabely2)

    mmGroup.defineHistogram('sector_ASide_eta1_ontrack,stationPhi_ASide_eta1_ontrack;Occupancy_ASide_eta1_PCB_ontrack', type='TH2F', title='Occupancy ASide eta1 PCB ontrack; ; ;', path='Occupancy_ontrack', xbins=40, xmin=0, xmax=40., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx2, ylabels=thisLabely2)

    thisLabelx3,thisLabely3=getMMLabel("x_lab_occ_eta2","y_lab_occ_eta2")

    mmGroup.defineHistogram('sector_ASide_eta2,stationPhi_ASide_eta2;Occupancy_ASide_eta2_PCB', type='TH2F', title='Occupancy ASide eta2 PCB; ; ;', path='Occupancy', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx3, ylabels=thisLabely3)

    mmGroup.defineHistogram('sector_ASide_eta2_ontrack,stationPhi_ASide_eta2_ontrack;Occupancy_ASide_eta2_PCB_ontrack', type='TH2F', title='Occupancy ASide eta2 PCB ontrack; ; ;', path='Occupancy_ontrack', xbins=24, xmin=0, xmax=24., ybins=16, ymin=1, ymax=17,xlabels=thisLabelx3, ylabels=thisLabely3)

    thisLabely4=getMMLabelY("y_lab_lb_CSide_eta2")

    mmGroup.defineHistogram('lb_mon,sector_lb_CSide_eta2;Occupancy_lb_CSide_eta2_PCB', type='TH2F', title="Occupancy lb CSide eta2 PCB; ; ;", path='Occupancy', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=49,ylabels=thisLabely4,opt='kAddBinsDynamically')

    mmGroup.defineHistogram('lb_ontrack,sector_lb_CSide_eta2_ontrack;Occupancy_lb_CSide_eta2_PCB_ontrack', type='TH2F', title="Occupancy lb CSide eta2 PCB ontrack; ; ;", path='Occupancy_ontrack', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=49, ylabels=thisLabely4, opt='kAddBinsDynamically')

    thisLabely5=getMMLabelY("y_lab_lb_CSide_eta1")

    mmGroup.defineHistogram('lb_mon,sector_lb_CSide_eta1;Occupancy_lb_CSide_eta1_PCB', type='TH2F', title="Occupancy lb CSide eta1 PCB; ; ;", path='Occupancy', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=81,ylabels=thisLabely5,opt='kAddBinsDynamically')

    mmGroup.defineHistogram('lb_ontrack,sector_lb_CSide_eta1_ontrack;Occupancy_lb_CSide_eta1_PCB_ontrack', type='TH2F', title="Occupancy lb CSide eta1 PCB ontrack; ; ;", path='Occupancy_ontrack', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=81,ylabels=thisLabely5,opt='kAddBinsDynamically')

    thisLabely6=getMMLabelY("y_lab_lb_ASide_eta1")

    mmGroup.defineHistogram('lb_mon,sector_lb_ASide_eta1;Occupancy_lb_ASide_eta1_PCB', type='TH2F', title="Occupancy lb ASide eta1 PCB; ; ;", path='Occupancy', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=81,ylabels=thisLabely6,opt='kAddBinsDynamically')

    mmGroup.defineHistogram('lb_ontrack,sector_lb_ASide_eta1_ontrack;Occupancy_lb_ASide_eta1_PCB_ontrack', type='TH2F', title="Occupancy lb ASide eta1 PCB ontrack; ; ;", path='Occupancy_ontrack', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=81,ylabels=thisLabely6,opt='kAddBinsDynamically')

    thisLabely7=getMMLabelY("y_lab_lb_ASide_eta2")

    mmGroup.defineHistogram('lb_mon,sector_lb_ASide_eta2;Occupancy_lb_ASide_eta2_PCB', type='TH2F', title="Occupancy lb ASide eta2 PCB; ; ;", path='Occupancy', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=49,ylabels=thisLabely7,opt='kAddBinsDynamically')

    mmGroup.defineHistogram('lb_ontrack,sector_lb_ASide_eta2_ontrack;Occupancy_lb_ASide_eta2_PCB_ontrack', type='TH2F', title="Occupancy lb ASide eta2 PCB ontrack; ; ;", path='Occupancy_ontrack', xbins=100,xmin=-0.5,xmax=99.5,ymin=1, ymax=49,ylabels=thisLabely7,opt='kAddBinsDynamically')

    side = ["CSide","ASide"]
    sector = ["MMS","MML"]
    etasector  = ["1","2"]
    for iside in side:

        if iside=="ASide":
            thisLabelx11,thisLabely11=getMMLabel("x_lab_occ_ASide","y_lab_occ_ASide")
        if iside=="CSide":
            thisLabelx11,thisLabely11=getMMLabel("x_lab_occ_CSide","y_lab_occ_CSide")

        MM_SideGroup="MM_sideGroup{0}".format(iside)
        mmSideGroup=helper.addGroup(mmMonAlg, MM_SideGroup, "Muon/MuonRawDataMonitoring/MM/"+iside)
        
        # Histograms for each sector
        phimax=8
        multipletmin=1
        multipletmax=2
        for isector in sector:
            for phi in range(1, phimax+1):
                phi16=0
                if (isector=="MMS"):
                    phi16=2*phi-1
                elif (isector=="MML"):
                    phi16=2*phi

                title_MMSummary="Number of strips per cluster,"+iside+" "+isector+" stPhi "+str(phi)   
                var="sector_strip_"+iside+"_"+isector+"_phi"+str(phi)+",strip_number_"+iside+"_"+isector+"_phi"+str(phi)+";Strip_number_pergap_"+iside+"_"+isector+"stPhi"+str(phi)
                mmSideGroup.defineHistogram(var, type='TH2F', title=title_MMSummary+"; ;Strip Number",      
                                            path='Number_of_strips_percluster_perPhiSector',   xbins=16, xmin=0, xmax=16, xlabels=thisLabelx11, ybins=5120, ymin=0., ymax=5120.)
                    
                for eta in etasector:
                    maxpcb=5
                    if(eta == "2"):
                        maxpcb=3
                    for multi in range(multipletmin, multipletmax+1):
                        for gas_gap in range(1,5):
                            #efficiency per pcb      
                            title_eff="Efficiency_per_PCB"+"Eta"+str(eta)+"_"+iside+"_phi"+str(phi16-1)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                            var_pcb="hitcut,pcb_eta"+str(eta)+"_"+iside+"_phi"+str(phi16-1)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                            title_allphi_eff="Efficiency_per_PCB"+"Eta"+str(eta)+"_"+iside+"_allphi"+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                            var_pcb_allphi="hitcut,pcb_eta"+str(eta)+"_allphi_"+iside+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)

                            mmSideGroup.defineHistogram(var_pcb,  type='TEfficiency', title=title_eff+"; pcb ;Efficiency Eta1",path='Efficiency',    xbins=maxpcb, xmin=0, xmax=maxpcb)
                            mmSideGroup.defineHistogram(var_pcb_allphi,  type='TEfficiency', title=title_allphi_eff+"; pcb ;Efficiency Eta1",path='Efficiency',    xbins=maxpcb, xmin=0, xmax=maxpcb) 
                            # Histograms for each layer
                            title_MMSummary_charge="Charge "+iside+" "+isector+" stPhi"+str(phi)+" stEta"+str(eta)+" multiplet"+str(multi)+" gap"+str(gas_gap)
                            var1="charge_"+iside+"_sector_"+isector+"_phi"+str(phi)+"_stationEta"+str(eta)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)+";Charge_"+iside+"_"+isector+"_stPhi"+str(phi)+"_stEta"+str(eta)+"_multiplet"+str(multi)+"_gap"+str(gas_gap)
                            mmSideGroup.defineHistogram(var1,  type='TH1F', title=title_MMSummary_charge+';Charge [fC];Number of Entries',path='Charge_perLayer',   xbins=120, xmin=0., xmax=1200.)
                            title_MMSummary_angle="uTPC angle "+iside+" "+isector+" stPhi"+str(phi)+" stEta"+str(eta)+" multiplet"+str(multi)+" gap"+str(gas_gap)
                            var3="mu_TPC_angle_"+iside+"_sector_"+isector+"_phi"+str(phi)+"_stationEta"+str(eta)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)+";uTPCangle_"+iside+"_"+isector+"_stPhi"+str(phi)+"_stEta"+str(eta)+"_multiplet"+str(multi)+"_gap"+str(gas_gap)
                            mmSideGroup.defineHistogram(var3,  type='TH1F', title=title_MMSummary_angle+"; #muTPC angle [degrees];Number of Entries",path='uTPC_angle_perLayer',    xbins=2000, xmin=-100, xmax=100)
                            
                            var_residual="residuals_"+iside+"_phi"+str(phi)+"_stationEta"+str(eta)+"_multiplet"+str(multi)+"_gas_gap"+str(gas_gap)
                            title_residual = "residuals "+iside+" "+isector+" stPhi"+str(phi)+" stEta"+str(eta)+" multiplet"+str(multi)+" gap"+str(gas_gap)
                            mmSideGroup.defineHistogram(var_residual,  type='TH1F', title=title_residual+"; res [mm];Number of Entries",path='Residuals',    xbins=200, xmin=-10, xmax=10)

        for gas1 in range(1, 5):
            for multi1 in range(1, 3):
                title_ontrack="Posy vs Posx "+iside+" multiplet"+str(multi1)+" gap"+str(gas1)+" ontrack"
                var_ontrack="x_"+iside+"_multiplet"+str(multi1)+"_gas_gap_"+str(gas1)+"_ontrack,y_"+iside+"_multiplet"+str(multi1)+"_gas_gap_"+str(gas1)+"_ontrack;Posy_vs_Posx_"+iside+"_multiplet"+str(multi1)+"_gas_gap_"+str(gas1)+"_ontrack"
                mmSideGroup.defineHistogram(var_ontrack, type='TH2F', title=title_ontrack+";MM-GlobalX [mm];MM-GlobalY [mm];", path='PosY_vs_Posx_perLayer_ontrack',xbins=500, xmin=-5000, xmax=5000., ybins=500, ymin=-5000.,ymax=5000.)


    acc = helper.result()
    result.merge(acc)
    return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaCommon.Constants import DEBUG
    
    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuon10_100GeV.ESD.r22.0.36_DigiCut1MeV_v4_EXT0/group.det-muon.26193643.EXT0._000001.ESD.pool.root",
                               "/afs/cern.ch/user/b/bigliett/myeos/DQ/group.det-muon.DiMuon10_100GeV.ESD.r22.0.36_DigiCut1MeV_v4_EXT0/group.det-muon.26193643.EXT0._000002.ESD.pool.root"
                           ]
                               
    #from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    ConfigFlags.Output.HISTFileName = 'monitor.root'

    ConfigFlags.Detector.GeometryMM=True
    ConfigFlags.Muon.doMicromegas = True
    ConfigFlags.DQ.useTrigger=False

    ConfigFlags.lock()
    ConfigFlags.dump()
    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    mmMonitorAcc  =  MMMonitoringConfig(ConfigFlags)
    mmMonitorAcc.OutputLevel=DEBUG
    cfg.merge(mmMonitorAcc)
    #cfg.printConfig(withDetails=True, summariseProps = True)  
    # number of events selected in the ESD
    cfg.run(-1)
