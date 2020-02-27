
#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigEgammaMonitoringConfigRun3.py
@author D. Maximov
@date 2019-07-08
@brief Run 3 configuration builder. Histograms definitions taken from TrigEgammaPlotTool
'''






class  THWrapper(object):
    def __init__(self, name):
        self.name = name
        self.kwargs = {}

    def __setitem__(self, key, value):
        self.kwargs[key] = value




# Emulate multiple ROOT C++ constructors to minimize retyping of histograms definitions
#    TH1F (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup) -- signature 1
#    TH1F (const char *name, const char *title, Int_t nbinsx, const Double_t *xbins)       -- signature 2
def TH1F(name, title, nxbins, bins_par2, bins_par3=None, path='', **kwargs):

    res = THWrapper(name)
    res['type'] = 'TH1F'
    res['title'] = title

    if isinstance(bins_par2, (list, tuple) ): # signature 2
        res['xbins'] = bins_par2
    else: # signature 1
        res['xbins'] = nxbins
        res['xmin'] = bins_par2
        res['xmax'] = bins_par3

    res['path'] = path
    res.kwargs.update(kwargs)
    return res




def TProfile(*args, **kwargs):
    res = TH1F(*args, **kwargs)
    res.name = res.name+','+res.name+'_total' # Temporary emulate two monitored variables
    res['type'] = 'TProfile'
    return res




def TH2F(name, title, nxbins, bins_par2, bins_par3, bins_par4, bins_par5=None, bins_par6=None, path='/', **kwargs):

    res = THWrapper(name+','+name+'_2') # Temporary emulate two monitored variables
    #res = THWrapper(name) # Temporary emulate two monitored variables
    res['type'] = 'TH2F'
    res['title'] = title

    if isinstance(bins_par2, (list, tuple) ): # signature 2 or 4
        res['xbins'] = bins_par2
        if isinstance(bins_par4, (list, tuple) ): # signature 4
            res['ybins'] = bins_par4
        else: # signature 2
            res['ybins'] = bins_par3
            res['ymin'] = bins_par4
            res['ymax'] = bins_par5

    else: # signature 1 or 3
        res['xbins'] = nxbins
        res['xmin'] = bins_par2
        res['xmax'] = bins_par3
        if isinstance(bins_par5, (list, tuple) ): # signature 3
            res['ybins'] = bins_par5
        else: # signature 1
            res['ybins'] = bins_par4
            res['ymin'] = bins_par5
            res['ymax'] = bins_par6

    res['path'] = path
    res.kwargs.update(kwargs)
    return res


def TProfile2D(name, *args, **kwargs):
    res = TH2F(name, *args, **kwargs)
    res.name = res.name+','+name # Temporary emulate three monitored variables
    res['type'] = 'TProfile2D'
    return res






class TrigEgammaAnalysisBuilder( object ):



    def __init__(self, helper, monAlg, monname, basepath ,
                 detailLevel=False,
                 ):

        self.helper=helper
        self.basepath = basepath
        self.detailLevel = detailLevel
        self.monAlg = monAlg
        self.monname=monname
        self._book_dists=False



    def bookTriggerHistograms(self, trigger, doJpsiee=False ):
        self.setBinning(doJpsiee)



    def bookEventHistograms( self, tool ):

      analysis = tool.Analysis
      nTriggers = len(tool.TriggerList)
      basepath = self.basepath+'/Expert'

      labels = tool.TriggerList
      cutLabels = ["Events","LAr","RetrieveElectrons","TwoElectrons","PassTrigger","EventWise","Success"]
      probeLabels=["Electrons","NotTag","OS","SS","ZMass","HasTrack","HasCluster","Eta","Et","IsGoodOQ","GoodPid","NearbyJet","Isolated"]
      
      
      # Create mon group.  The group name should be the path name for map
      group = self.helper.addGroup( self.monAlg, (self.basepath+'/Expert').replace('/','_'), self.basepath+'/Expert/' )


      # If yes, this tool has inheritance from TrigEgammaNavTPBaseTool
      if hasattr( tool, 'ZeeMassMin' ):
        zeemin = tool.ZeeMassMin
        zeemax = tool.ZeeMassMax

        # Book all histograms
        group.defineHistogram(analysis+"_ProbeCutCounter", type='TH1F', path='', title="Number of Probes; Cut ; Count",xbins=12, xmin=0, xmax=12)
        group.defineHistogram(analysis+"_Mee", type='TH1F', path='/', title="Offline M(ee); m_ee [GeV] ; Count",xbins=50, xmin=zeemin, xmax=zeemax)
        group.defineHistogram(analysis+"_CutCounter", type='TH1I', path='/', title="Event Selection; Cut ; Count",xbins=6, xmin=0, xmax=6)

        group.defineHistogram(analysis+"_trigger_counts", type='TH1F', path='', title="Trigger Counts; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_nProbes", type='TH1F', path='', title="Number of Probes; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_nProbesL1", type='TH1F', path='', title="Number of Probes L1; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_nProbesL2", type='TH1F', path='', title="Number of Probes L2; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_nProbesL2Calo", type='TH1F', path='', title="Number of Probes L2Calo; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_nProbesEFCalo", type='TH1F', path='', title="Number of Probes EFCalo; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_nProbesHLT", type='TH1F', path='', title="Number of Probes; Trigger HLT; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_EffL1", type='TProfile', path='', title="Average L1 Efficiency; Trigger ; #epsilon",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_EffL2", type='TProfile', path='', title="Average L2 Efficiency; Trigger ; #epsilon",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_EffL2Calo", type='TProfile', path='', title="Average L2Calo Efficiency; Trigger ; #epsilon",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_EffEFCalo", type='TProfile', path='', title="Average EFCalo Efficiency; Trigger ; #epsilon",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)
        group.defineHistogram(analysis+"_EffHLT", type='TProfile', path='', title="Average HLT Efficiency; Trigger ; #epsilon",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)

      else: # If not, has inheritance from TrigEgammaNavBaseTool
              
        group.defineHistogram(analysis+"_trigger_counts", type='TH1F', path='/', title="Trigger Counts; Trigger ; Count",xbins=nTriggers, xmin=0, xmax=nTriggers,
                              labels=labels)






    def setBinning(self, doJpsiee=False):

        """ Method to define all binning required that is variable"""

        # Binning as defined in TP framework
        coarse_eta_bins = [-2.47,-1.52,-1.37,-0.60,0.00,0.60,1.37,1.52,2.47] # 9 items

        coarse_et_bins = [4.,20.,30.,40.,50.,60.,150.] # 7 items

        default_eta_bins = [-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.80,-0.60,-0.10,0.00,
            0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] # 21 items

        default_et_bins = [4.,7.,10.,15.,20.,25.,30.,35.,40.,45.,50.,60.,80.,150.] # 14 items

        etabins = [-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.8,-0.6,-0.1,
            0.0,0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47] # 21 items

        # TH2 with variable bin x-Axis, but constant bin y-Axis takes only Double_t arrays
        etbins_Zee = [0.,2.,4.,6.,8.,10.,
            12.,14.,16.,18.,20.,22.,24.,26.,28.,
            30.,32.,34.,36.,38.,40.,42.,44.,46.,48.,50.,55.,60.,65.,70.,100.] # 31 items

        etbins_Jpsiee = [ 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5,
            5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5,
            10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,
            15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,
            20,20.5,21,21.5,22,22.5,23,23.5,24,24.5,
            25,25.5] # 52 items


        if doJpsiee:
            self._nEtbins=51;
            self._etbins = etbins_Jpsiee[0:self._nEtbins+1]
        else:
            self._nEtbins=30;
            self._etbins = etbins_Zee[0:self._nEtbins+1]

        # Define the binning
        self._nEtabins=20;
        self._ndefaultEtbins=13;
        self._ndefaultEtabins=20;
        self._ncoarseEtbins=6;
        self._ncoarseEtabins=8;

        #Fill the arrays
        self._etabins = etabins[0:self._nEtabins+1]
        self._defaultEtbins = default_et_bins[0:self._ndefaultEtbins+1]
        self._defaultEtabins = default_eta_bins[0:self._ndefaultEtabins+1]
        self._coarseEtbins = coarse_et_bins[0:self._ncoarseEtbins+1]
        self._coarseEtabins = coarse_eta_bins[0:self._ncoarseEtabins+1]



    def getTrigInfo( self, trigger ):

      class TrigEgammaInfo(object):

        def __init__(self, trigger):
          self.__chain = trigger

        def chain(self):
          return self.__chain
        
        def isL1Item(self):
          return True if self.chain().startswith('L1') else False

        def isElectron(self):
          return True if (self.isL1Item() or self.chain().startswith('HLT_e')) else False
        
        def isPhoton(self):
          return True if (self.chain().startswith('HLT_g')) else False

        def pidname(self):
          return self.chain().split('_')[2]

      return TrigEgammaInfo(trigger)

