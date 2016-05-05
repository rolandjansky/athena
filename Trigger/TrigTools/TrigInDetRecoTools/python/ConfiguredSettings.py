# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import *

class CommonSettings() :
    def __init__(self):         
        self.allowedInstanceNames = ['Muon', 'eGamma', 'muonIso', 'Tau', 'TauCore', 'TauIso',
                                     'Jet', 'Bphysics', 'FullScan', 'BeamSpot', 'Tile', 'FullScan_ZF_Only']
        self.db = {}
        ptmin = {}
        for i in self.allowedInstanceNames :
            ptmin[i] = 1.*GeV # in MeV
        ptmin['Muon']= 1.0 * GeV
        self.db['pTmin']=ptmin
    
    def __getitem__(self, (quantity, slice)):
        v = None
        try:
            q = self.db[quantity]
            try:
                v = q[slice]
            except:
                print 'Settings has no instance %s ' % slice
        except:
            print 'Settings has no setting %s ' % quantity
        return v

    def __setitem__(self, (quantity, slice), value):
        try:
            q = self.db[quantity]
            try:
                q[slice] = value
            except:
                print 'Settings has no instance %s ' % slice
        except:
            print 'Settings has no setting %s ' % quantity



class SettingsForStrategyB(CommonSettings) :
    def __init__(self):
        CommonSettings.__init__(self)
        # strategy-specific settings defined below
        extension_SpacePoints      = {}
        extension_SpacePointsMiss1 = {}
        extension_SpacePointsMiss2 = {}
        extension_TRTExtension     = {}
        map_LayerOneMultiplicity   = {}
        map_LayerTwoMultiplicity   = {}
        reco_D0Max                 = {}
        vertexing_CutRPhi          = {}
        vertexing_CutRZ            = {}
        vertexing_Enable           = {}
        vertexing_WindowSize       = {}
        
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import L2IDTrackingCuts

        for i in self.allowedInstanceNames :
            extension_SpacePoints[i]      = 5
            extension_SpacePointsMiss1[i] = 5
            extension_SpacePointsMiss2[i] = 5
            extension_TRTExtension[i]     = True            
            map_LayerOneMultiplicity[i]   = 3
            map_LayerTwoMultiplicity[i]   = 3
            reco_D0Max[i]                 = 3.0*mm            
            vertexing_CutRPhi[i]          = 0.5
            vertexing_Enable[i]           = False
            vertexing_WindowSize[i]       = 3.0
        
        # customize setting here for BeamSpot instance
        vertexing_Enable['BeamSpot']     = True
        vertexing_CutRPhi['BeamSpot']    = 0.00025
        vertexing_WindowSize['BeamSpot'] = 2.0

        # customize setting here for Bphysics instance
        vertexing_Enable['Bphysics']     = True
        vertexing_CutRPhi['Bphysics']    = 0.00025
        vertexing_WindowSize['Bphysics'] = 2.0
        
        # customize setting here for FullScan instance
        vertexing_Enable['FullScan']     = True
        vertexing_CutRPhi['FullScan']    = 0.00025
        vertexing_WindowSize['FullScan'] = 2.0
        
        # customize setting here for Jet instance
        extension_SpacePoints['Jet']     = 4
        extension_TRTExtension['Jet']    = False        
                
        # customize setting here for Tau instance
        map_LayerOneMultiplicity['Tau'] = 2
        map_LayerTwoMultiplicity['Tau'] = 2

        # extend settings database
        self.db['Extension_SpacePoints']      = extension_SpacePoints
        self.db['Extension_SpacePointsMiss1'] = extension_SpacePointsMiss1
        self.db['Extension_SpacePointsMiss2'] = extension_SpacePointsMiss2    
        self.db['Extension_TRTExtension']     = extension_TRTExtension    
        self.db['Map_LayerOneMultiplicity']   = map_LayerOneMultiplicity
        self.db['Map_LayerTwoMultiplicity']   = map_LayerTwoMultiplicity
        self.db['Reco_D0Max']                 = reco_D0Max        
        self.db['Vertexing_CutRPhi']          = vertexing_CutRPhi
        self.db['Vertexing_CutRZ']            = vertexing_CutRZ        
        self.db['Vertexing_Enable']           = vertexing_Enable
        self.db['Vertexing_WindowSize']       = vertexing_WindowSize



class SettingsForStrategyC(CommonSettings) :
    def __init__(self):
        CommonSettings.__init__(self)
        
        # strategy-specific settings defined below
        width = {}
        usezvtool = {}
        nclustersmin = {}
        nholesmax = {}
        nholesgapmax = {}

        for i in self.allowedInstanceNames :
            width[i] = 10.0
            usezvtool[i] = True
            nclustersmin[i] = 7
            nholesmax[i] = 2
            nholesgapmax[i] = 2
            
        
        
        # customize setting here: for example width['Tau'] = 20.
        usezvtool['Tau'] = False
        # extend settings database
        self.db['RoadWidth']=width
        self.db['useZvertexTool'] = usezvtool
        self.db['nClustersMin'] = nclustersmin
        self.db['nHolesMax'] = nholesmax
        self.db['nHolesGapMax'] = nholesgapmax
        
class SettingsForStrategyF(CommonSettings) :
    def __init__(self):
        CommonSettings.__init__(self)
        # strategy-specific settings defined below
        chi2cut = {}
        for i in self.allowedInstanceNames :
            chi2cut[i] = 1000.0
        
        # customize setting here: for example width['Tau'] = 20.
        # extend settings database
        self.db['Chi2Cut']=chi2cut

class SettingsForStrategyA(CommonSettings) :
    def __init__(self):
        CommonSettings.__init__(self)
        
        # strategy-specific settings defined below
        
        FindMultipleZ       = {}
      
        phihalfwidth = {}
      
        etahalfwidth = {}
        minhits = {}
        # z-finder parameters 
        namezf            = {}
        fullscanmode        = {}
        roiphiwidth         = {}
        numberofpeaks       = {}
     
        phibinsizez          = {}
        useonlypixel        = {}
        minzbinsize         = {}
        zbinsizeetacoeff    = {}
        dphideta            = {}
        chargeaware         = {}
        zhistoperphi        = {}
        maxz                = {}
        prefercentralz      = {}
        vrtxdistcut         = {}
        nvrtxseparation     = {}
        tripletmode         = {}
        neighbormultiplier  = {}
        vrtxmixing          = {}
        tripletdz           = {}
        tripletdk           = {}
        tripletdp           = {}
        zfindermode         = {}
        useroidescriptorwidths = {}
        nfirstlayers        = {}
        dozfonly     = {}
        weightthreshold = {}
        
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import L2IDTrackingCuts

        for i in self.allowedInstanceNames :
            
            FindMultipleZ[i]          = True
            phihalfwidth[i]           = 0.2
            etahalfwidth[i]           = 0.2
            minhits[i]                =  5
            namezf[i]                 ="unknown"
            fullscanmode[i]           = False    
            roiphiwidth[i]            = 0.2  
            numberofpeaks[i]          = 1
            phibinsizez[i]             = 0.2            
            useonlypixel[i]           = False
            minzbinsize[i]            = 0.2
            zbinsizeetacoeff[i]       = 0.1            
            dphideta[i]               = -0.02
            chargeaware[i]            = True
            zhistoperphi[i]           = True
            prefercentralz[i]         = False
            vrtxdistcut[i]            = 0.
            nvrtxseparation[i]        = 0
            tripletmode[i]         = 0
            neighbormultiplier[i]  = 1
            vrtxmixing[i]          = 0.
            tripletdz[i]           = 25
            tripletdk[i]           = 0.005
            tripletdp[i]           = 0.05
            zfindermode[i]        = 0
            useroidescriptorwidths[i] = False
            nfirstlayers[i]       = 3
            dozfonly[i]        = False
            weightthreshold[i] = 0.
        
        
     
        # customize setting here for BeamSpot instance
        etahalfwidth['BeamSpot']           = 0.3
        phihalfwidth['BeamSpot']           = 3.14159265358
        
        namezf['BeamSpot']                 ="IDScanZFinderA_BeamSpot"  
        fullscanmode['BeamSpot']           = True
        numberofpeaks['BeamSpot']          = 3
        phibinsizez['BeamSpot']             = 1.0            
        minzbinsize['BeamSpot']            = 0.4
        zbinsizeetacoeff['BeamSpot']       = 0.2            
        dphideta['BeamSpot']               = -0.025
        chargeaware['BeamSpot']            = False
        zhistoperphi['BeamSpot']           = False
        prefercentralz['BeamSpot']         = True
       
        # customize setting here for Bphysics instance
        etahalfwidth['Bphysics']           = 0.75
        phihalfwidth['Bphysics']           = 0.75
        
        namezf['Bphysics']               ="IDScanZFinderA_Bphysics"  
        numberofpeaks['Bphysics']          = 3
        phibinsizez['Bphysics']             = 1.0            
        dphideta['Bphysics']               = -0.025
        chargeaware['Bphysics']            = False
        zhistoperphi['Bphysics']           = False
        nfirstlayers['Bphysics']          = 4
        
        #customize setting here for FullScan instance
        FindMultipleZ['FullScan']          = True
        etahalfwidth['FullScan']           = 3.0
        phihalfwidth['FullScan']           = 3.14159265358
        namezf['FullScan']               ="IDScanZFinderA_FullScan"
        fullscanmode['FullScan']           = True
        numberofpeaks['FullScan']          = 3
        phibinsizez['FullScan']             = 1.0            
        useonlypixel['FullScan']           = False
        minzbinsize['FullScan']            = 0.4
        zbinsizeetacoeff['FullScan']       = 0.2            
        dphideta['FullScan']               = -0.025
        chargeaware['FullScan']            = False
        zhistoperphi['FullScan']           = False
        prefercentralz['FullScan']         = True


        FindMultipleZ['FullScan_ZF_Only']          = True
        etahalfwidth['FullScan_ZF_Only']           = 3.0
        phihalfwidth['FullScan_ZF_Only']           = 3.14159265358
        namezf['FullScan_ZF_Only']                 = "IDScanZFinderA_FullScan_ZF_Only"  
        fullscanmode['FullScan_ZF_Only']           = True
        numberofpeaks['FullScan_ZF_Only']          = 4
        phibinsizez['FullScan_ZF_Only']             = 1.0            
        useonlypixel['FullScan_ZF_Only']           = False
        minzbinsize['FullScan_ZF_Only']            = 2.0
        zbinsizeetacoeff['FullScan_ZF_Only']       = 0.2            
        dphideta['FullScan_ZF_Only']               = -0.025
        chargeaware['FullScan_ZF_Only']            = False
        zhistoperphi['FullScan_ZF_Only']           = False
        prefercentralz['FullScan_ZF_Only']         = True
        weightthreshold['FullScan_ZF_Only']        = 500.
        dozfonly['FullScan_ZF_Only']               = True
        
                
        # customize setting here for Jet instance
        etahalfwidth['Jet']           = 0.2
        phihalfwidth['Jet']           = 0.2
        
        namezf['Jet']               ="IDScanZFinderA_Jet"  
        numberofpeaks['Jet']          = 3
        phibinsizez['Jet']             = 0.8
        useonlypixel['Jet']           = False
        minzbinsize['Jet']            = 0.4
        zbinsizeetacoeff['Jet']       = 0.1
        dphideta['Jet']               = -0.02
        chargeaware['Jet']            = False
        zhistoperphi['Jet']           = False
        vrtxdistcut['Jet']            = 0.01
        tripletmode['Jet']            = 1
        neighbormultiplier['Jet']    = 1
        tripletdz['Jet']             = 6
        tripletdk['Jet']             = 0.00005
        tripletdp['Jet']             = 0.005

        # customize setting here for eGamma instance
        etahalfwidth['eGamma']           = 0.1
        phihalfwidth['eGamma']           = 0.1
        
        namezf['eGamma']               ="IDScanZFinderA_eGamma"  
        numberofpeaks['eGamma']          = 4
        phibinsizez['eGamma']             = 0.1            
        useonlypixel['eGamma']           = False
        minzbinsize['eGamma']            = 0.3
        zbinsizeetacoeff['eGamma']       = 0.15            
        dphideta['eGamma']               = -0.035
        chargeaware['eGamma']            = True
        zhistoperphi['eGamma']           = False
        prefercentralz['eGamma']         = True
        vrtxdistcut['eGamma']            = 0.02
        nvrtxseparation['eGamma']        = 5
        tripletmode['eGamma']            = 1
        neighbormultiplier['eGamma']    = 1
        tripletdz['eGamma']             = 10
        tripletdk['eGamma']             = 0.00001
        tripletdp['eGamma']             = 0.05
        nfirstlayers['eGamma']          = 4
         
        # customize setting here for Muon instance
        etahalfwidth['Muon']           = 0.1
        phihalfwidth['Muon']           = 0.1
        
        namezf['Muon']               ="IDScanZFinderA_Muon"  
        numberofpeaks['Muon']          = 4
        phibinsizez['Muon']             = 0.15            
        useonlypixel['Muon']           = False
        minzbinsize['Muon']            = 0.3
        zbinsizeetacoeff['Muon']       = 0.15            
        dphideta['Muon']               = -0.045
        chargeaware['Muon']            = True
        zhistoperphi['Muon']           = False
        prefercentralz['Muon']         = True
        vrtxdistcut['Muon']            = 0.1
        nvrtxseparation['Muon']        = 5
        tripletmode['Muon']         = 1
        neighbormultiplier['Muon']  = 1
        tripletdz['Muon']           = 15
        tripletdk['Muon']           = 0.0001
        tripletdp['Muon']           = 0.1
        nfirstlayers['Muon']          = 4
       # useroidescriptorwidths['Muon'] = True
       # customize setting here for muonIso instance
        etahalfwidth['muonIso']           = 0.2
        phihalfwidth['muonIso']           = 0.2
        
        namezf['muonIso']               ="IDScanZFinderA_muonIso"  
        numberofpeaks['muonIso']          = 4
        phibinsizez['muonIso']             = 0.1            
        useonlypixel['muonIso']           = False
        minzbinsize['muonIso']            = 0.3
        zbinsizeetacoeff['muonIso']       = 0.15            
        dphideta['muonIso']               = -0.035
        chargeaware['muonIso']            = True
        zhistoperphi['muonIso']           = False
        prefercentralz['muonIso']         = True
        vrtxdistcut['muonIso']            = 0.05
        nvrtxseparation['muonIso']        = 5
        tripletmode['muonIso']         = 1
        neighbormultiplier['muonIso']  = 1
        tripletdz['muonIso']           = 10
        tripletdk['muonIso']           = 0.00005
        tripletdp['muonIso']           = 0.005
       # useroidescriptorwidths['muonIso'] = True

        # customize setting here for Tau instance
        etahalfwidth['Tau']           = 0.3
        phihalfwidth['Tau']           = 0.3
        
        namezf['Tau']               ="IDScanZFinderA_Tau"  
        numberofpeaks['Tau']          = 4
        phibinsizez['Tau']             = 0.3            
        useonlypixel['Tau']           = False
        minzbinsize['Tau']            = 0.2
        zbinsizeetacoeff['Tau']       = 0.2            
        dphideta['Tau']               = -0.035
        chargeaware['Tau']            = True
        zhistoperphi['Tau']           = False
        prefercentralz['Tau']         = True
        vrtxdistcut['Tau']            = 0.01
        nvrtxseparation['Tau']        = 5
        tripletmode['Tau']         = 1
        neighbormultiplier['Tau']  = 1
        vrtxmixing['Tau']          = 0
        tripletdz['Tau']           = 10
        tripletdk['Tau']           = 0.00005
        tripletdp['Tau']           = 0.05
       
        
        # customize setting here for Tile instance
        etahalfwidth['Tile']           = 0.1
        phihalfwidth['Tile']           = 0.2
        
        namezf['Tile']               ="IDScanZFinderA_Tile"  
        numberofpeaks['Tile']          = 4
        phibinsizez['Tile']             = 0.3            
        useonlypixel['Tile']           = False
        minzbinsize['Tile']            = 0.4
        zbinsizeetacoeff['Tile']       = 0.2            
        dphideta['Tile']               = -0.025
        chargeaware['Tile']            = False
        zhistoperphi['Tile']           = False
       
        # extend settings database
        
        self.db['namezf']                   =          namezf
        self.db['FullScanMode']             =          fullscanmode           
        self.db['NumberOfPeaks']            =          numberofpeaks          
        self.db['PhiBinSizeZ']               =          phibinsizez             
        self.db['UseOnlyPixels']            =          useonlypixel           
        self.db['MinZBinSize']              =          minzbinsize            
        self.db['ZBinSizeEtaCoeff']         =          zbinsizeetacoeff       
        self.db['DPhiDEta']                 =          dphideta               
        self.db['ChargeAware']              =          chargeaware            
        self.db['ZHistoPerPhi']             =          zhistoperphi
        self.db['MaxZ']                     =          maxz
        self.db['PreferCentralZ']           =          prefercentralz         
        self.db['VrtxDistCut']              =          vrtxdistcut            
        self.db['nVrtxSeparation']          =          nvrtxseparation        
        self.db['TripletMode']              =          tripletmode            
        self.db['NeighborMultiplier']       =          neighbormultiplier     
        self.db['VrtxMixing']               =          vrtxmixing             
        self.db['TripletDZ']                =          tripletdz              
        self.db['TripletDK']                =          tripletdk              
        self.db['TripletDP']                =          tripletdp              
        self.db['ZFinderMode']              =          zfindermode
        self.db['nFirstLayers']              =          nfirstlayers
        self.db['phiHalfWidth']             =          phihalfwidth 
        self.db['etaHalfWidth']             =          etahalfwidth 
        self.db['MinHits']                  =          minhits 
        self.db['FindMultipleZ']            =          FindMultipleZ
        self.db['DoZF_Only']                =          dozfonly
        self.db['WeightThreshold']          =          weightthreshold
        
        # hit-filter parameters        

        namehit = {}
        phihalfwidth = {}
        etahalfwidth = {}
        phibinsize = {}
        etabinsize = {}
        layerthreshold = {}
        loosetightboundary = {}
        enhancelayer0 = {}
        removeduplicates = {}
        cloneremovalcut = {}
        useptsign = {}
        d0cut_loose = {}
        d0cut_tight = {}
        dphidrbinsize_tight = {}
        dphidrbinsize_loose = {}
        etacutouter_tight = {}
        etacutinner_tight = {}
        etacutouter_loose = {}
        etacutinner_loose = {}
        phi0bins_tight = {}
        phi0bins_loose = {}
        deltadzdrcut_tight = {}
        deltadzdrcut_loose = {}
        #ptcutinmev = {}
                   
        for i in self.allowedInstanceNames :
            namehit[i] = "unknown"
            phihalfwidth[i] = 0.1
            etahalfwidth[i] = 0.1
            phibinsize[i] = 2.4 
            etabinsize[i] = 0.005
            layerthreshold[i] = 3.5
            loosetightboundary[i] = 10.5 
            enhancelayer0[i] = True
            removeduplicates[i] = True
            cloneremovalcut[i] = 3
            useptsign[i] = True
            d0cut_loose[i] = 4.0
            d0cut_tight[i] = 1.0
            dphidrbinsize_tight[i] =  0.0000024 
            dphidrbinsize_loose[i] = 0.0000030 
            etacutouter_tight[i] = 0.01
            etacutinner_tight[i] = 0.004
            etacutouter_loose[i] = 0.02
            etacutinner_loose[i] = 0.008
            phi0bins_tight[i] = 20000
            phi0bins_loose[i] = 16000
            deltadzdrcut_tight[i] = 0.12
            deltadzdrcut_loose[i] = 0.12
            #ptcutinmev[i] = 1000.0
        
        # customize setting here for BeamSpot instance
            
        namehit['BeamSpot'] = "IDScanHitFilterA_BeamSpot"
        phibinsize['BeamSpot'] = 3.5 
        etabinsize['BeamSpot'] = 0.02
        cloneremovalcut['BeamSpot'] = 2
        d0cut_loose['BeamSpot'] = 4.0
        d0cut_tight['BeamSpot'] = 4.0
        dphidrbinsize_tight['BeamSpot'] =  0.00000300
        dphidrbinsize_loose['BeamSpot'] = 0.00000375
        etacutouter_tight['BeamSpot'] = 0.02
        etacutinner_tight['BeamSpot'] = 0.04
        etacutouter_loose['BeamSpot'] = 0.05
        etacutinner_loose['BeamSpot'] = 0.1
        layerthreshold['BeamSpot'] = 4.5
        #ptcutinmev['BeamSpot'] = 500.0
        
        
        # customize setting here for Bphysics instance
        
        namehit['Bphysics'] = "IDScanHitFilterA_Bphysics"
        phibinsize['Bphysics'] = 3.5 
        etabinsize['Bphysics'] = 0.02
        cloneremovalcut['Bphysics'] = 2
        d0cut_tight['Bphysics'] = 4.0
        dphidrbinsize_tight['Bphysics'] =  0.00000300
        dphidrbinsize_loose['Bphysics'] = 0.00000375
        etacutouter_tight['Bphysics'] = 0.02
        etacutinner_tight['Bphysics'] = 0.04
        etacutouter_loose['Bphysics'] = 0.05
        etacutinner_loose['Bphysics'] = 0.1
        layerthreshold['Bphysics'] = 4.5
        
        # customize setting here for FullScan instance
        
        namehit['FullScan'] = "IDScanHitFilterA_FullScan"
        phibinsize['FullScan'] = 3.5 
        etabinsize['FullScan'] = 0.02
        cloneremovalcut['FullScan'] = 2
        d0cut_loose['FullScan'] = 4.0
        d0cut_tight['FullScan'] = 4.0
        dphidrbinsize_tight['FullScan'] =  0.00000300
        dphidrbinsize_loose['FullScan'] = 0.00000375
        etacutouter_tight['FullScan'] = 0.02
        etacutinner_tight['FullScan'] = 0.04
        etacutouter_loose['FullScan'] = 0.05
        etacutinner_loose['FullScan'] = 0.1
        layerthreshold['FullScan'] = 4.5
        
        # customize setting here for Jet instance
        
        namehit['Jet'] = "IDScanHitFilterA_Jet"
        phibinsize['Jet'] = 2.0 
        etabinsize['Jet'] = 0.02
        cloneremovalcut['Jet'] = 2
        useptsign['Jet'] = False
        dphidrbinsize_loose['Jet'] = 0.00000375
        etacutouter_loose['Jet'] = 0.05
        etacutinner_loose['Jet'] = 0.1
        etacutouter_tight['Jet'] = 0.004
        d0cut_tight['Jet'] = 0.4
        
        # customize setting here for Tau instance
        
        namehit['Tau'] = "IDScanHitFilterA_Tau"
        phibinsize['Tau'] = 2.4 
        etabinsize['Tau'] = 0.005
        cloneremovalcut['Tau'] = 2
        
        # customize setting here for eGamma instance
        
        namehit['eGamma'] = "IDScanHitFilterA_eGamma"
        phibinsize['eGamma'] = 2.4
        etabinsize['eGamma'] = 0.005
        cloneremovalcut['eGamma'] = 3
        useptsign['eGamma'] = False
        dphidrbinsize_tight['eGamma'] =  0.00000300
        d0cut_loose['eGamma'] = 10
        d0cut_tight['eGamma'] = 10

        # customize setting here for Muon instance
        
        namehit['Muon'] = "IDScanHitFilterA_Muon"
        phibinsize['Muon'] = 2.4
        etabinsize['Muon'] = 0.005
        cloneremovalcut['Muon'] = 3
        useptsign['Muon'] = False
        d0cut_loose['Muon'] = 10
        d0cut_tight['Muon'] = 10
         
        # customize setting here for muonIso instance
        
        namehit['muonIso'] = "IDScanHitFilterA_muonIso"
        phibinsize['muonIso'] = 2.4 
        etabinsize['muonIso'] = 0.005
        cloneremovalcut['muonIso'] = 3
        d0cut_loose['muonIso'] = 10
        d0cut_tight['muonIso'] = 10
        
        
        # customize setting here for Tile instance
        
        namehit['Tile'] = "IDScanHitFilterA_Tile"
        phibinsize['Tile'] = 2.4 
        etabinsize['Tile'] = 0.005
        cloneremovalcut['Tile'] = 3
        
        # extend settings database
        
        self.db['namehit']              =                  namehit          
        self.db['PhiBinSize']           =               phibinsize           
        self.db['EtaBinSize']           =               etabinsize           
        self.db['LayerThreshold']       =           layerthreshold       
        self.db['LooseTightBoundary']   =       loosetightboundary   
        self.db['EnhanceLayer0']        =            enhancelayer0        
        self.db['RemoveDuplicates']     =         removeduplicates     
        self.db['CloneRemovalCut']      =          cloneremovalcut      
        self.db['UsePtSign']            =                useptsign            
        self.db['d0Cut_loose']          =              d0cut_loose          
        self.db['d0Cut_tight']          =              d0cut_tight          
        self.db['dPhidRBinSize_tight']  =      dphidrbinsize_tight  
        self.db['dPhidRBinSize_loose']  =      dphidrbinsize_loose  
        self.db['etaCutOuter_tight']    =        etacutouter_tight    
        self.db['etaCutInner_tight']    =        etacutinner_tight    
        self.db['etaCutOuter_loose']    =        etacutouter_loose    
        self.db['etaCutInner_loose']    =        etacutinner_loose    
        self.db['phi0Bins_tight']       =           phi0bins_tight       
        self.db['phi0Bins_loose']       =           phi0bins_loose       
        self.db['deltadzdrCut_tight']   =       deltadzdrcut_tight   
        self.db['deltadzdrCut_loose']   =       deltadzdrcut_loose   
        #self.db['pTcutInMeV']           =               ptcutinmev           
                    
