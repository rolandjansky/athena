#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
 

def same( val , tool):
  return [val]*( len( tool.EtaBins ) - 1 )


#
# Create the hypo alg with all selectors
#
def createTrigEgammaForwardPrecisionElectronHypoAlg(name, sequenceOut):
    # make the Hypo
    #from TriggerMenuMT.HLT.Egamma.TrigEgammaDefs import createTrigEgammaForwardPrecisionElectronLHSelectors

    from TrigEgammaForwardHypo.TrigEgammaForwardHypoConf import TrigEgammaForwardPrecisionElectronHypoAlg
    thePrecisionElectronHypo = TrigEgammaForwardPrecisionElectronHypoAlg(name)
    thePrecisionElectronHypo.Electrons = sequenceOut
    thePrecisionElectronHypo.RunInView = True
    #thePrecisionElectronHypo.FwdElectronLHSelectorTools = createTrigEgammaPrecisionElectronLHSelectors()
    #thePrecisionElectronHypo.LHNames = ["lhtight", "lhmedium", "lhloose"] # just like the pidnames

    from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    MonTool = GenericMonitoringTool("MonTool_"+name)
    MonTool.Histograms = [ 
          defineHistogram('TIME_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo Algtime; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=8000.0),
          defineHistogram('TIME_LH_exec', type='TH1F', path='EXPERT', title="Precision Electron Hypo LH Algtime; time [ us ] ; Nruns", xbins=30, xmin=0.0, xmax=3000.0),
    ]

    MonTool.HistPath = 'PrecisionElectronHypo_FWD/'+name
    thePrecisionElectronHypo.MonTool=MonTool

    return thePrecisionElectronHypo



class TrigEgammaForwardPrecisionElectronHypoToolConfig:


  __operation_points  = [  
                           'lhtight'  ,
                           'lhmedium' ,
                           'lhloose'  ,
                           'lhvloose' ,
                           ]


  def __init__(self, name, threshold, sel, iso):

    from AthenaCommon.Logging import logging
    self.__log = logging.getLogger('TrigEgammaForwardPrecisionElectronHypoTool')
    self.__name = name
    self.__threshold = float(threshold)
    self.__sel = sel
    self.__iso = iso
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaPrecisionElectronHypoTool
    tool = TrigEgammaPrecisionElectronHypoTool( name )

    tool.EtaBins        = []
    tool.PidName        = ""
    self.__tool         = tool    

    self.__log.debug( 'Electron_Chain     :%s', name )
    self.__log.debug( 'Electron_Threshold :%s', threshold )
    self.__log.debug( 'Electron_Pidname   :%s', sel )
    self.__log.debug( 'Electron_iso       :%s', iso )

  def chain(self):
    return self.__name

  def pidname( self ):
    return self.__sel

  def etthr(self):
    return self.__threshold

  def isoInfo(self):
    return self.__iso

  def tool(self):
    return self.__tool

  def etcut(self):
    self.__log.debug( 'Configure etcut' )


  def nominal(self):
    if not self.pidname() in self.__operation_points:
      self.__log.fatal("Bad selection name: %s" % self.pidname())
    self.tool().PidName = self.pidname()


  #
  # Compile the chain
  #
  def compile(self):

    if 'etcut' == self.pidname():
      self.etcut()
    else:
      self.nominal()





def _IncTool( name, threshold, sel, iso ):
    config = TrigEgammaForwardPrecisionElectronHypoToolConfig(name, threshold, sel, iso)
    config.compile()
    return config.tool()



def TrigEgammaForwardPrecisionElectronHypoToolFromDict( d ):
    """ Use menu decoded chain dictionary to configure the tool """
    cparts = [i for i in d['chainParts'] if ((i['signature']=='Electron') or (i['signature']=='Electron'))]

    def __mult(cpart):
        return int( cpart['multiplicity'] )

    def __th(cpart):
        return cpart['threshold']
    
    def __sel(cpart):
        return cpart['addInfo'][0] if cpart['addInfo'] else cpart['IDinfo']

    def __iso(cpart):
        return cpart['isoInfo']

    name = d['chainName']
    return _IncTool( name, __th( cparts[0]),  __sel( cparts[0] ), __iso ( cparts[0])  )
                   
    
def TrigEgammaForwardPrecisionElectronHypoToolFromName(name, conf):
    from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName
    decodedDict = dictFromChainName(conf)
    return  TrigEgammaForwardPrecisionElectronHypoToolFromDict( decodedDict )
