###########################################################################
#                                          
#    <nozicka@mail.desy.de>                                                             
#
###########################################################################


class TrigHistorySvc(object) :
  def __init__(self, *args, **kwargs) :
    for k,v in kwargs.items() :
      self.__setattr__(k,v)
      
  def __setattr__(self, name, value) :
    _properties = {'OutputFile':str('L2EF_Setup.txt'),'Dump':True,'Activate':True}
    if name in (_properties.keys()) :
      if type(value) == type(_properties[name]) :
        self.__dict__[name] = value
      else :
        print 'Unsupported type of value %s - type %s expected' % (type(value),type(_properties[name]))
    else :
      print 'Unsupported property name %s' % (name)
    
  def dump(self) :
    # Set the output file
    document = None
    file = None
    if self.__dict__.has_key('OutputFile') and self.OutputFile != '' :
      # Dump the algorithms to the file
      file = open(self.OutputFile, 'w')
      
      # Test the xml type of the output
      if self.OutputFile[-4:] == '.xml' :
        import xml.dom.minidom
        impl = xml.dom.minidom.getDOMImplementation()
        document = impl.createDocument(None, "setup", None)
    else:
      import sys
      file = sys.stdout
    
    # Get the algorithms
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp
    from AthenaCommon.AlgSequence import AlgSequence
    #toolSvc = CfgMgr.ToolSvc()
    svcMgr = theApp.serviceMgr()
    toolSvc = svcMgr.ToolSvc
    topAlgs = theApp.TopAlg
    auditors = svcMgr.AuditorSvc
    
    # Dump the ApplicationMgr
    if document :
      topElement = document.documentElement
      topElement.appendChild(self.algtoXML(theApp, document, dumpChildren=False))
    else :
      file.write(self.algtoStr(theApp, ntabs=1, dumpChildren=False))
    
    # Dump the Tools
    if document :
      topElement = document.documentElement
      topElement.appendChild(self.algtoXML(toolSvc, document,'Service',dumpChildren=False))
    else :
      file.write(self.algtoStr(toolSvc, ntabs=1, dumpChildren=False))
    for alg in toolSvc.getAllChildren():
      if document:
        topElement = document.documentElement
        topElement.appendChild(self.algtoXML(alg, document, 'PublicTool'))
      else :
        file.write(self.algtoStr(alg))

    # Dump the Auditors
    if document :
      topElement = document.documentElement
      topElement.appendChild(self.algtoXML(auditors, document,'Service',dumpChildren=False))
    else :
      file.write(self.algtoStr(auditors, ntabs=1, dumpChildren=False))
      
    for alg in auditors.getAllChildren():
      if document:
        topElement = document.documentElement
        topElement.appendChild(self.algtoXML(alg, document, 'Auditor'))
      else :
        file.write(self.algtoStr(alg))
    
    # Dump the Services
    if document :
      topElement = document.documentElement
      topElement.appendChild(self.algtoXML(svcMgr, document,'Service',dumpChildren=False))
    else :
      file.write(self.algtoStr(svcMgr, ntabs=1, dumpChildren=False))
    for alg in svcMgr.getAllChildren():
      if alg.getName() not in ('AuditorSvc','ToolSvc') :
        if document:
          topElement = document.documentElement
          topElement.appendChild(self.algtoXML(alg, document, 'Service'))
        else :
          file.write(self.algtoStr(alg))
        
    # Dump the top Algorithms
    from AthenaCommon.ConfigurableDb import getConfigurable, cfgDb
    from AthenaCommon import CfgMgr
    from AthenaCommon.Configurable import Configurable
    for algNameClass in topAlgs:
      alg = None
      algName = algNameClass.split('/')[1]
      algClass = algNameClass.split('/')[0]
      if Configurable.allConfigurables.has_key(algName) :
        alg = Configurable.allConfigurables[algName]
        if alg.getFullName() != algNameClass: alg = None
      if not alg :
        cfg_class = getConfigurable(algClass, assumeCxxClass=True)
        Configurable.allConfigurables.has_key('TrigSteer_L2')
        stmt = 'alg = CfgMgr.%s("%s")' % (cfg_class.__name__,algName)
        try :
          exec stmt
        except Exception, exc:
          print "ERROR:\t\tTop algorithm: %s/%s was not assigned: %s" % (algClass,algName)
          print exc
      
      if alg :
        if document:
          topElement = document.documentElement
          topElement.appendChild(self.algtoXML(alg, document, 'TopAlg'))
        else :
          file.write(self.algtoStr(alg))
      else :
        print "Top algorithm: %s/%s was not assigned" % (algClass,algName)
    
    if document :
      document.writexml(file, addindent='  ', newl='\n')
      
    file.close()
    
  def algtoStr(self, alg, ntabs=1, dumpChildren=True) :
    py_class = str(alg.__class__).strip('<class \' \'>').split('.')[-1]
    py_package = str(alg.__module__)
    
    py_info = ' (Python: %s/%s) ' % (py_package, py_class)
    max_length = 100
    nendchar = max_length - (ntabs + 5 + len(alg.getPrintTitle()) + len(py_info))
    output  = '|'*(ntabs-1) + '/' + '*'*5 + alg.getPrintTitle() + py_info + '*'*nendchar + '\n'
    
    # Formating of the properties
    maxproplength = 0
    for k,v in alg.getProperties().items() :
      if len('%s\t%s' %(k,type(v))) > maxproplength : maxproplength = len('%s\t%s' %(k,type(v)))
    
    for k,v in alg.getProperties().items():
      output += '|'*ntabs 
      output += '-%s\t%s  ' % (k, type(v)) 
      output += ' '*(maxproplength-len('%s\t%s' %(k,type(v))))
      output += '= %s' % (v)
      output += '\n'
      
    if dumpChildren :
      for child in alg.getAllChildren() :
        output += self.algtoStr(child, ntabs+1)
    
    nendchar = nendchar - len('(End of )')
    output  += '|'*(ntabs-1) + '\\' + '*'*5 + '(End of %s%s)' % (alg.getPrintTitle(), py_info) + '*'*nendchar + '\n'
    return output
    
  def algtoXML(self, alg, document, algtype='Algorithm', dumpChildren=True) :
    py_class = str(alg.__class__)[6:].strip('\' >').split('.')[-1]
    py_package = str(alg.__module__)
    alg_name = alg.getType()
    alg_alias = alg.getFullJobOptName().split('/')[-1]
    
    algorithm='1'
    topalg=steeralg=svc=pubT=privT=aud='0'
    if algtype == 'TopAlg': topalg='1'
    if algtype == 'SteerAlg': steeralg='1'
    if algtype == 'Service': svc='1'
    if algtype == 'PublicTool': pubT='1'
    if algtype == 'PrivateTool': privT='1'
    if algtype == 'Auditor': aud='1'
    
    component = document.createElement('component')
    component.setAttribute('name', str(alg_name))
    component.setAttribute('alias', str(alg_alias))
    component.setAttribute('py_name', py_class)
    component.setAttribute('py_package', py_package)
    component.setAttribute('topalg', topalg)
    component.setAttribute('alg', algorithm)
    component.setAttribute('steeralg', steeralg)
    component.setAttribute('svc', svc)
    component.setAttribute('pubT', pubT)
    component.setAttribute('privT', privT)
    component.setAttribute('aud', aud)

    # Append the properties
    for k,v in alg.getProperties().items() :
      if v=='<no value>': v = alg.getDefaultProperties()[k]
      component.appendChild(self.propertytoXML(document,k,v))
    
    if dumpChildren :
      # Append the child algorithms
      if alg_name == 'HLT::TrigSteer': algtype = 'SteerAlg'
      for child in alg.getAllChildren() :
        if algtype != 'SteerAlg' : algtype='PrivateTool'
        component.appendChild(self.algtoXML(child, document, algtype, True))
        
    return component
  
  def propertytoXML(self, document, name, value) :
    prop_type = str(type(value)).strip('<type >').strip('\'')
    if value == '<no value>' : value =''
    if prop_type[:6] ==  'class ':
      propertyClass = prop_type[6:].strip('\'')
      if propertyClass in ('GaudiKernel.GaudiHandles.ServiceHandleArray',
          'GaudiKernel.GaudiHandles.PrivateToolHandleArray',
          'GaudiKernel.GaudiHandles.PublicToolHandleArray') :
        new_value = []
        for item in value :
          try:
            new_value.append(item.getFullName())
          except:
            pass
        value = '%s' % (new_value)
      else :
        try :
          value = value.getFullJobOptName()
        except :
          pass
    parameter = document.createElement('parameter')
    parameter.setAttribute('name', str(name))
    parameter.setAttribute('value', str(value))
    parameter.setAttribute('type', prop_type)
    parameter.setAttribute('op','set')

    return parameter
      
