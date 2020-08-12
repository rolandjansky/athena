#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigBjetMonitorAlgorithm.py
@author E. Nagy
@author T. Bold
@date 2020-05-27
@brief Example trigger python configuration for the Run III AthenaMonitoring package, based on the example by C Burton and P Onyisi
'''

def TrigBjetMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # from AthenaCommon.Logging import logging
    # log = logging.getLogger( 'TrigBjetMonitorAlgorithm.py' )

    ### STEP 1 ###
    # Define one top-level monitoring algorithm. The new configuration 
    # framework uses a component accumulator.
    # EN: Not needed for the moment
    # from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    # result = ComponentAccumulator()

    # The following class will make a sequence, configure algorithms, and link
    # them to GenericMonitoringTools
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigBjetAthMonitorCfg')


    ### STEP 2 ###
    # Adding an algorithm to the helper. Here, we will use the example 
    # algorithm in the AthenaMonitoring package. Just pass the type to the 
    # helper. Then, the helper will instantiate an instance and set up the 
    # base class configuration following the inputFlags. The returned object 
    # is the algorithm.
    #The added algorithm must exist as a .h file 

    from AthenaConfiguration.ComponentFactory import CompFactory
    trigBjetMonAlg = helper.addAlgorithm(CompFactory.TrigBjetMonitorAlgorithm,'TrigBjetMonAlg')

    # You can actually make multiple instances of the same algorithm and give 
    # them different configurations
    #shifterTrigBjetMonAlg = helper.addAlgorithm(TrigBjetMonitorAlgorithm,'ShifterTrigBjetMonAlg')

    # # If for some really obscure reason you need to instantiate an algorithm
    # # yourself, the AddAlgorithm method will still configure the base 
    # # properties and add the algorithm to the monitoring sequence.
    # helper.AddAlgorithm(myExistingAlg)


    ### STEP 3 ###
    # Edit properties of a algorithm
    # some generic property
    # trigBjetMonAlg.RandomHist = True
    # to enable a trigger filter, for example:
    #trigBjetMonAlg.TriggerChain = 'HLT_mu26_ivarmedium'
    #trigBjetMonAlg.TriggerChain = 'HLT_e24_lhtight_nod0'
    trigBjetMonAlg.TriggerChain = ''

    ### STEP 4 ###
    # Read in the Bjet trigger chain names

    # Directly from TrigBjetMonitCategory
    #from TrigBjetMonitoring import TrigBjetMonitCategory
    #bjet_triglist = TrigBjetMonitCategory.monitoring_bjet
    #bjet_triglist += TrigBjetMonitCategory.monitoring_mujet
    #print " ==> bjet_triglist: ", bjet_triglist

    # From the hltmonList where TrigHLTMonitoring filtered the run type
    from TrigHLTMonitoring.HLTMonTriggerList import hltmonList
    bjet_triglist = hltmonList.monitoring_bjet
    bjet_triglist += hltmonList.monitoring_mujet
    # print " ==> bjet_triglist: ", bjet_triglist

    # Add some tools. N.B. Do not use your own trigger decion tool. Use the
    # standard one that is included with AthMonitorAlgorithm.

    # # First, add a tool that's set up by a different configuration function. 
    # # In this case, CaloNoiseToolCfg returns its own component accumulator, 
    # # which must be merged with the one from this function.
    # from CaloTools.CaloNoiseToolConfig import CaloNoiseToolCfg
    # caloNoiseAcc, caloNoiseTool = CaloNoiseToolCfg(inputFlags)
    # result.merge(caloNoiseAcc)
    # trigBjetMonAlg.CaloNoiseTool = caloNoiseTool

    # # Then, add a tool that doesn't have its own configuration function. In
    # # this example, no accumulator is returned, so no merge is necessary.
    # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
    # trigBjetMonAlg.MyDomainTool = MyDomainTool()

    # Add a generic monitoring tool (a "group" in old language). The returned 
    # object here is the standard GenericMonitoringTool.
    BjetMonGroup = helper.addGroup(trigBjetMonAlg,'TrigBjetMonitor','HLT/BjetMon/')

    # Add a GMT for the other example monitor algorithm
    #shifterGroup = helper.addGroup(shifterTrigBjetMonAlg,'TrigBjetMonitor','HLT/BjetMon/Shifter/')

    ### STEP 5 ###
    # Configure histograms
    #NB! The histograms defined here must match the ones in the cxx file exactly

    # Offline PV histograms - common for all trigger chains

    BjetMonGroup.defineHistogram('Off_NVtx', title='Number of Offline Vertices;NVtx;Events',
                                 path='Shifter/Offline',xbins=100,xmin=0.0,xmax=100.)
    BjetMonGroup.defineHistogram('Off_xVtx', title='Offline xVtx;xVtx;Events',
                                 path='Shifter/Offline',xbins=200,xmin=-1.5,xmax=+1.5)
    BjetMonGroup.defineHistogram('Off_yVtx', title='Offline yVtx;yVtx;Events',
                                 path='Shifter/Offline',xbins=200,xmin=-1.5,xmax=+1.5)
    BjetMonGroup.defineHistogram('Off_zVtx', title='Offline zVtx;zVtx;Events',
                                 path='Shifter/Offline',xbins=500,xmin=-250.0,xmax=+250.0)

    # Histograms which depend on the trigger chain

    # mu-jet histograms

    AllChains = []
    for chain in bjet_triglist :
        AllChains.append(chain[2:])
        # print " inside bjet_triglist chain[2:8] : " , chain[2:8]
        if chain[2:8] == 'HLT_mu' : # mu-jets
            # print "        mu-jet histogram is defined for ", chain[2:]

            HistName = 'jetPt_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                             path='Expert/'+chain[2:],xbins=100,xmin=0.0,xmax=750.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                             path='Shifter/'+chain[2:],xbins=100,xmin=0.0,xmax=750.0)

            continue
        else :                      # b-jets
            # print "        b-jet histogram is defined for ", chain[2:]

      # b-jet histograms

         # Primary vertex histograms

            # PV associated to jets

            HistName = 'PVz_jet_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online zPV from jets;zPV from jets;Events',
                                             path='Expert/'+chain[2:],xbins=500,xmin=-250.0,xmax=250.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online zPV from jets;zPV from jets;Events',
                                             path='Shifter/'+chain[2:],xbins=500,xmin=-250.0,xmax=250.0)

            HistName = 'PVx_jet_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online xPV from jets;xPV from jets;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online xPV from jets;xPV from jets;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)

            HistName = 'PVy_jet_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online yPV from jets;yPV from jets;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online yPV from jets;yPV from jets;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)

            # PV directly from SG

            HistName = 'nPV_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Number of online PV per event;nPV;Events',
                                             path='Expert/'+chain[2:],xbins=101,xmin=-1.0,xmax=100.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Number of online PV per event;nPV;Events',
                                             path='Shifter/'+chain[2:],xbins=101,xmin=-1.0,xmax=100.0)

            HistName = 'PVz_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online zPV;zPV;Events',
                                             path='Expert/'+chain[2:],xbins=500,xmin=-250.0,xmax=250.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online zPV;zPV;Events',
                                             path='Shifter/'+chain[2:],xbins=500,xmin=-250.0,xmax=250.0)

            HistName = 'PVx_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online xPV;xPV from jets;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online xPV;xPV from jets;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)

            HistName = 'PVy_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online yPV;yPV from jets;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of online yPV;yPV from jets;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-1.5,xmax=+1.5)

         # track histograms

            HistName = 'nTrack_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Number of tracks;nTrack;Events',
                                             path='Expert/'+chain[2:],xbins=40,xmin=0.0,xmax=40.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Number of tracks;nTrack;Events',
                                             path='Shifter/'+chain[2:],xbins=40,xmin=0.0,xmax=40.0)

            HistName = 'trkPt_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Pt of tracks;Pt;Events',
                                             path='Expert/'+chain[2:],xbins=100,xmin=0.0,xmax=50.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Pt of tracks;Pt;Events',
                                             path='Shifter/'+chain[2:],xbins=100,xmin=0.0,xmax=50.0)

            HistName = 'trkEta_' + chain[2:] + ',trkPhi_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName,type='TH2F',title='Phi vs Eta of tracks;Eta;Phi',
                                             path='Expert/'+chain[2:],xbins=20,xmin=-5.0,xmax=+5.0,ybins=20,ymin=-3.1416,ymax=+3.1416)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName,type='TH2F',title='Phi vs Eta of tracks;Eta;Phi',
                                             path='Shifter/'+chain[2:],xbins=20,xmin=-5.0,xmax=+5.0,ybins=20,ymin=-3.1416,ymax=+3.1416)

            HistName = 'd0_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of d0;d0;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-2.0,xmax=2.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of d0;d0;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-2.0,xmax=2.0)

            HistName = 'ed0_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of sig(d0);sig(d0);Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=0.,xmax=1.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of sig(d0);sig(d0);Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=0.,xmax=1.0)

            HistName = 'z0_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of z0;z0;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-250.0,xmax=250.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of z0;z0;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-250.0,xmax=250.0)

            HistName = 'ez0_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of sig(z0);sig(z0);Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=0.,xmax=5.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of sig(z0);sig(z0);Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=0.,xmax=5.0)

         # jet histograms

            HistName = 'nJet_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Number of jets;nJet;Events',
                                             path='Expert/'+chain[2:],xbins=40,xmin=0.0,xmax=40.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Number of jets;nJet;Events',
                                             path='Shifter/'+chain[2:],xbins=40,xmin=0.0,xmax=40.0)

            HistName = 'jetPt_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                             path='Expert/'+chain[2:],xbins=100,xmin=0.0,xmax=750.0)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of Pt_jet;Pt_jet;Events',
                                             path='Shifter/'+chain[2:],xbins=100,xmin=0.0,xmax=750.0)

            HistName = 'jetEta_' + chain[2:] + ',jetPhi_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName,type='TH2F',title='Phi vs Eta of jets;Eta_jet;Phi_jet',
                                             path='Expert/'+chain[2:],xbins=20,xmin=-5.0,xmax=+5.0,ybins=20,ymin=-3.1416,ymax=+3.1416)
                # print " ==> histogram ",HistName," is defined for Expert folder"
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName,type='TH2F',title='Phi vs Eta of jets;Eta_jet;Phi_jet',
                                             path='Shifter/'+chain[2:],xbins=20,xmin=-5.0,xmax=+5.0,ybins=20,ymin=-3.1416,ymax=+3.1416)

      # b-tagging quantities


            HistName = 'IP3D_pu_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='IP3D_pu probability distribution;IP3D_pu;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=0.0,xmax=1.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='IP3D_pu probability distribution;IP3D_pu;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=0.0,xmax=1.0)

            HistName = 'IP3D_pb_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='IP3D_pb probability distribution;IP3D_pb;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=0.0,xmax=1.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='IP3D_pb probability distribution;IP3D_pb;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=0.0,xmax=1.0)

            HistName = 'IP3D_pc_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='IP3D_pc probability distribution;IP3D_pc;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=0.0,xmax=1.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='IP3D_pc probability distribution;IP3D_pc;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=0.0,xmax=1.0)

            HistName = 'wIP3D_Rbu_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='LogLH IP3D_pb/IP3D_pu probability ratio distribution;LogLH IP3D_pb/IP3D_pu;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-4.0,xmax=6.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='LogLH IP3D_pb/IP3D_pu probability ratio distribution;LogLH IP3D_pb/IP3D_pu;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-4.0,xmax=6.0)

            HistName = 'wSV1_Rbu_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='LogLH SV1_pb/SV1_pu probability ratio distribution;LogLH SV1_pb/SV1_pu;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-4.0,xmax=6.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='LogLH SV1_pb/SV1_pu probability ratio distribution;LogLH SV1_pb/SV1_pu;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-4.0,xmax=6.0)

            HistName = 'wCOMB_Rbu_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='LogLH IP3D+SV1 probability ratio distribution;LogLH IP3D+SV1 probability ratio;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-4.0,xmax=6.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='LogLH IP3D+SV1 probability ratio distribution;LogLH IP3D+SV1 probability ratio;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-4.0,xmax=6.0)

            HistName = 'wMV2c10_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of MV2c10 discriminant;MV2c10;Events',
                                             path='Expert/'+chain[2:],xbins=200,xmin=-1.0,xmax=1.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of MV2c10 discriminant;MV2c10;Events',
                                             path='Shifter/'+chain[2:],xbins=200,xmin=-1.0,xmax=1.0)

            HistName = 'xMVtx_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='SV1 mass distribution;SV1 mass;Events',
                                             path='Expert/'+chain[2:],xbins=50,xmin=0.0,xmax=10.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='SV1 mass distribution;SV1 mass;Events',
                                             path='Shifter/'+chain[2:],xbins=50,xmin=0.0,xmax=10.0)

            HistName = 'xEVtx_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='SV1 E-fraction distribution;SV1 E-fraction;Events',
                                             path='Expert/'+chain[2:],xbins=50,xmin=0.0,xmax=1.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='SV1 E-fraction distribution;SV1 E-fraction;Events',
                                             path='Shifter/'+chain[2:],xbins=50,xmin=0.0,xmax=1.0)

            HistName = 'xNVtx_tr_' + chain[2:]
            if chain[0:1] == "E" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of number of 2-track SV1;Number of 2-track SV1;Events',
                                             path='Expert/'+chain[2:],xbins=40,xmin=0.0,xmax=40.0)
            if chain[0:1] == "S" :
                BjetMonGroup.defineHistogram(HistName, title='Distribution of number of 2-track SV1;Number of 2-track SV1;Events',
                                             path='Shifter/'+chain[2:],xbins=40,xmin=0.0,xmax=40.0)
            continue


    # print " ==> In TrigBjetMonitorAlgorithm.py: AllChains list: ", AllChains
    trigBjetMonAlg.AllChains = AllChains


    ### STEP 6 ###
    # Finalize. The return value should be a tuple of the ComponentAccumulator
    # and the sequence containing the created algorithms. If we haven't called
    # any configuration other than the AthMonitorCfgHelper here, then we can 
    # just return directly (and not create "result" above)
    return helper.result()
    
    # # Otherwise, merge with result object and return
    # acc = helper.result()
    # result.merge(acc)
    # return result

if __name__=='__main__':
    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)
    # from AthenaCommon.Constants import INFO
    # log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    # Input files

    # Original data input file from P.Onyisi and C.Burton:
    #nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/'
    #file = 'data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1'
    #ConfigFlags.Input.Files = [nightly+file]
    #ConfigFlags.Input.isMC = False

    # AOD file to be run w/ MT access
    file = '/afs/cern.ch/work/e/enagy/public/ARTfiles/MCtest310720.AOD.pool.root'

    ConfigFlags.Input.Files = [file]
    ConfigFlags.Input.isMC = True

    # Output file (root)

    ConfigFlags.Output.HISTFileName = 'TrigBjetMonitorOutput.root'

    # ConfigFlags.Trigger.triggerMenuSetup="Physics_pp_v7_primaries"
    
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    trigBjetMonitorAcc = TrigBjetMonConfig(ConfigFlags)
    cfg.merge(trigBjetMonitorAcc)

    # If you want to turn on more detailed messages ...
    #trigBjetMonitorAcc.getEventAlgo('TrigBjetMonAlg').OutputLevel = 2 # DEBUG
    cfg.printConfig(withDetails=True) # set True for exhaustive info

    Nevents = 25
    #cfg.run(Nevents)
    cfg.run() #use cfg.run(20) to only run on first 20 events

