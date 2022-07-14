#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def JfexMonitoringConfig(inputFlags):
    '''Function to configure LVL1 Jfex algorithm in the monitoring system.'''

    import math 
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'JfexMonitoringCfg')

    # get any algorithms
    JfexMonAlg = helper.addAlgorithm(CompFactory.JfexMonitorAlgorithm,'JfexMonAlg')

    # add any steering
    groupName = 'JfexMonitor' # the monitoring group name is also used for the package name
    JfexMonAlg.PackageName = groupName

    mainDir = 'L1Calo'
    trigPath = 'Jfex/'

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(JfexMonAlg, groupName , mainDir)

    # define gfex histograms
    
    ######  jJ  ######  
    myGroup.defineHistogram('jJ_jFexNumber;h_jJ_jFexNumber', title='jFex SRJet Module;Module number;Counts',
                            type='TH1I', path=trigPath+'jJ/', xbins=6,xmin=0,xmax=6)

    myGroup.defineHistogram('jJ_fpgaNumber;h_jJ_fpgaNumber', title='jFex SRJet FPGA;FPGA number;Counts',
                            type='TH1F', path=trigPath+'jJ/',xbins=4,xmin=0,xmax=4)
                            
    myGroup.defineHistogram('jJ_Et;h_jJ_Et', title='jFex SRJet Transverse Energy;tobEt [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jJ/', xbins=512,xmin=0,xmax=2048)

    myGroup.defineHistogram('jJ_Eta;h_jJ_Eta', title='jFex SRJet #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jJ/',xbins=100,xmin=-5.0,xmax=5.0)

    myGroup.defineHistogram('jJ_Phi;h_jJ_Phi', title='jFex SRJet #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jJ/',xbins=66,xmin=-math.pi,xmax=math.pi)
                            
    myGroup.defineHistogram('jJ_Eta,jJ_Phi;h_jJ_EtaPhiMap', title="jFex SRJet #eta vs #phi;#eta;#phi",
                            type='TH2F',path=trigPath+'jJ/', xbins=1000,xmin=-5.0,xmax=5.0,ybins=1000,ymin=-math.pi,ymax=math.pi)                            

    myGroup.defineHistogram('jJ_GlobalEta;h_jJ_GlobalEta', title='jFex SRJet Global #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jJ/',xbins=100,xmin=-50,xmax=50)

    myGroup.defineHistogram('jJ_GlobalPhi;h_jJ_GlobalPhi', title='jFex SRJet Global #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jJ/',xbins=67,xmin=-1,xmax=65)
                            
    myGroup.defineHistogram('jJ_GlobalEta,jJ_GlobalPhi;h_jJ_GlobalEtaPhiMap', title="jFex SRJet Global #eta vs #phi;(int) #eta;(int) #phi",
                            type='TH2F',path=trigPath+'jJ/', xbins=1000,xmin=-50,xmax=50,ybins=1000,ymin=-1,ymax=65)                            
                            
    ######  jLJ  ######  
    myGroup.defineHistogram('jLJ_jFexNumber;h_jLJ_jFexNumber', title='jFex LRJet Module;Module number;Counts',
                            type='TH1I', path=trigPath+'jLJ/', xbins=6,xmin=0,xmax=6)

    myGroup.defineHistogram('jLJ_fpgaNumber;h_jLJ_fpgaNumber', title='jFex LRJet FPGA;FPGA number;Counts',
                            type='TH1F', path=trigPath+'jLJ/',xbins=4,xmin=0,xmax=4)
                            
    myGroup.defineHistogram('jLJ_Et;h_jLJ_Et', title='jFex LRJet Transverse Energy;tobEt [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jLJ/',xbins=512,xmin=0,xmax=2048)

    myGroup.defineHistogram('jLJ_Eta;h_jLJ_Eta', title='jFex LRJet #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jLJ/',xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('jLJ_Phi;h_jLJ_Phi', title='jFex LRJet #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jLJ/',xbins=66,xmin=-math.pi,xmax=math.pi)
                            
    myGroup.defineHistogram('jLJ_Eta,jLJ_Phi;h_jLJ_EtaPhiMap', title="jFEX LRJet #eta vs #phi;#eta;#phi",
                            type='TH2F',path=trigPath+'jLJ/', xbins=1000,xmin=-5.0,xmax=5.0,ybins=1000,ymin=-math.pi,ymax=math.pi)                             

    myGroup.defineHistogram('jLJ_GlobalEta;h_jLJ_GlobalEta', title='jFex LRJet Global #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jLJ/',xbins=100,xmin=-50,xmax=50)

    myGroup.defineHistogram('jLJ_GlobalPhi;h_jLJ_GlobalPhi', title='jFex LRJet Global #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jLJ/',xbins=67,xmin=-1,xmax=65)
                            
    myGroup.defineHistogram('jLJ_GlobalEta,jLJ_GlobalPhi;h_jLJ_GlobalEtaPhiMap', title="jFex LRJet #eta vs #phi;(int) #eta; (int) #phi",
                            type='TH2F',path=trigPath+'jLJ/', xbins=1000,xmin=-50,xmax=50,ybins=1000,ymin=-1,ymax=65)                                  
    ######  jTau  ######   
    myGroup.defineHistogram('jTau_jFexNumber;h_jTau_jFexNumber', title='jFex Tau Module;Module number;Counts',
                            type='TH1I', path=trigPath+'jTau/', xbins=6,xmin=0,xmax=6)

    myGroup.defineHistogram('jTau_fpgaNumber;h_jTau_fpgaNumber', title='jFex Tau FPGA;FPGA number;Counts',
                            type='TH1F', path=trigPath+'jTau/',xbins=4,xmin=0,xmax=4)
                            
    myGroup.defineHistogram('jTau_Et;h_jTau_Et', title='jFex Tau Transverse Energy;tobEt [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jTau/',xbins=512,xmin=0,xmax=2048)
    
    myGroup.defineHistogram('jTau_Iso;h_jTau_Iso', title='jFex Tau Isolation;tobIso [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jTau/',xbins=512,xmin=0,xmax=2048)

    myGroup.defineHistogram('jTau_Eta;h_jTau_Eta', title='jFex Tau #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jTau/',xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('jTau_Phi;h_jTau_Phi', title='jFex Tau #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jTau/',xbins=66,xmin=-math.pi,xmax=math.pi)
                            
    myGroup.defineHistogram('jTau_Eta,jTau_Phi;h_jTau_EtaPhiMap', title="jFex Tau #eta vs #phi;#eta;#phi",
                            type='TH2F',path=trigPath+'jTau/', xbins=1000,xmin=-5.0,xmax=5.0,ybins=1000,ymin=-math.pi,ymax=math.pi)          

    myGroup.defineHistogram('jTau_GlobalEta;h_jTau_GlobalEta', title='jFex Tau Global #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jTau/',xbins=100,xmin=-50,xmax=50)

    myGroup.defineHistogram('jTau_GlobalPhi;h_jTau_GlobalPhi', title='jFex Tau Global #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jTau/',xbins=67,xmin=-1,xmax=65)
                            
    myGroup.defineHistogram('jTau_GlobalEta,jTau_GlobalPhi;h_jTau_GlobalEtaPhiMap', title="jFex Tau Global #eta vs #phi;(int) #eta; (int) #phi",
                            type='TH2F',path=trigPath+'jTau/', xbins=1000,xmin=-50,xmax=50,ybins=1000,ymin=-1,ymax=65)                               
    ######  jEM  ######  
    myGroup.defineHistogram('jEM_jFexNumber;h_jEM_jFexNumber', title='jFex EM Module;Module number;Counts',
                            type='TH1I', path=trigPath+'jEM/', xbins=6,xmin=0,xmax=6)

    myGroup.defineHistogram('jEM_fpgaNumber;h_jEM_fpgaNumber', title='jFex EM FPGA;FPGA number;Counts',
                            type='TH1F', path=trigPath+'jEM/',xbins=4,xmin=0,xmax=4)
                            
    myGroup.defineHistogram('jEM_Et;h_jEM_Et', title='jFex EM Transverse Energy;tobEt [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jEM/',xbins=512,xmin=0,xmax=2048)

    myGroup.defineHistogram('jEM_Eta;h_jEM_Eta', title='jFex EM #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jEM/',xbins=60,xmin=-3.0,xmax=3.0)

    myGroup.defineHistogram('jEM_Phi;h_jEM_Phi', title='jFex EM #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jEM/',xbins=66,xmin=-math.pi,xmax=math.pi)

    em_labels = ['None','loose','medium','tight']
    myGroup.defineHistogram('jEM_Iso;h_jEM_Iso', title='jFex EM Isolation;tobIso;Counts',
                            type='TH1I', path=trigPath+'jEM/',xbins=4,xmin=0,xmax=4,xlabels=em_labels)

    myGroup.defineHistogram('jEM_f1;h_jEM_f1', title='jFex EM Frac1;EM Frac1;Counts',
                            type='TH1I', path=trigPath+'jEM/',xbins=4,xmin=0,xmax=4,xlabels=em_labels)

    myGroup.defineHistogram('jEM_f2;h_jEM_f2', title='jFex EM Frac2;EM Frac2;Counts',
                            type='TH1I', path=trigPath+'jEM/',xbins=4,xmin=0,xmax=4,xlabels=em_labels)
                            
    myGroup.defineHistogram('jEM_Eta,jEM_Phi;h_jEM_EtaPhiMap', title="jFex EM #eta vs #phi;#eta;#phi",
                            type='TH2F',path=trigPath+'jEM/', xbins=1000,xmin=-5.0,xmax=5.0,ybins=1000,ymin=-math.pi,ymax=math.pi) 

    myGroup.defineHistogram('jEM_GlobalEta;h_jEM_GlobalEta', title='jFex EM Global #eta;#eta;Counts',
                            type='TH1F', path=trigPath+'jEM/',xbins=100,xmin=-50,xmax=50)

    myGroup.defineHistogram('jEM_GlobalPhi;h_jEM_GlobalPhi', title='jFex EM Global #phi;#phi;Counts',
                            type='TH1F', path=trigPath+'jEM/',xbins=67,xmin=-1,xmax=65)
                            
    myGroup.defineHistogram('jEM_GlobalEta,jEM_GlobalPhi;h_jEM_GlobalEtaPhiMap', title="jFex EM Global #eta vs #phi;(int) #eta; (int) #phi",
                            type='TH2F',path=trigPath+'jEM/', xbins=1000,xmin=-50,xmax=50,ybins=1000,ymin=-1,ymax=65)                                
    ######  jXE  ######                          
    myGroup.defineHistogram('jXE_X;h_jXE_X', title='jFex MET X component;tobEx [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jXE/',xbins=1000,xmin=-500,xmax=500)
    
    myGroup.defineHistogram('jXE_Y;h_jXE_Y', title='jFex MET Y component;tobEy [200 MeV Scale];Counts',
                            type='TH1I', path=trigPath+'jXE/',xbins=1000,xmin=-500,xmax=500)

    myGroup.defineHistogram('jXE_MET;h_jXE_MET', title='jFex MET ;Total jXE [200 GeV Scale];Counts',
                            type='TH1F', path=trigPath+'jXE/',xbins=500,xmin=0,xmax=1000)

    myGroup.defineHistogram('jXE_phi;h_jXE_phi', title='jFex MET phi ;#phi=atan(Ey/Ex) values;Counts',
                            type='TH1F', path=trigPath+'jXE/',xbins=66,xmin=-math.pi,xmax=math.pi)

                            
    ######  jTE  ######                          
    myGroup.defineHistogram('jTE_low;h_jTE_low', title='jFex SumEt low #eta;tob Et_lower [GeV/200];Counts',
                            type='TH1I', path=trigPath+'jTE/',xbins=700,xmin=0,xmax=700)
    
    myGroup.defineHistogram('jTE_high;h_jTE_high', title='jFex SumEt high #eta;tob Et_upper [GeV/200];Counts',
                            type='TH1I', path=trigPath+'jTE/',xbins=700,xmin=0,xmax=700)

    myGroup.defineHistogram('jTE_SumEt;h_jTE_SumEt', title='jFex SumEt total ;Total jTE [200 GeV Scale];Counts',
                            type='TH1F', path=trigPath+'jTE/',xbins=1000,xmin=0,xmax=4000)


    acc = helper.result()
    result.merge(acc)
    return result


if __name__=='__main__':
    # set input file and config options
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob
    inputs = glob.glob('/eos/user/t/thompson/ATLAS/LVL1_mon/MC_ESD/mc21.601189.PhPy8EG_AZNLO_Zee.recon.ESD.e8392_e7400_s3775_r13614_r13689/*root*')

    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'ExampleMonitorOutput_LVL1.root'

    ConfigFlags.Exec.MaxEvents=10

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    JfexMonitorCfg = JfexMonitoringConfig(ConfigFlags)
    cfg.merge(JfexMonitorCfg)
    cfg.run()
