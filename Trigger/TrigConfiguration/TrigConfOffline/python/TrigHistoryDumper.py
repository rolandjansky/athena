###########################################################################
#    
#    <nozicka@mail.desy.de>
#
#   DESCRIPTION: TrigHistorySvc - derived configurable of the HistorySvc
#                - extends the functionality of the HistorySvc obtaining 
#                  the python class (py_name) and python module (py_package)
#                  of the algorithm
#
###########################################################################


__all__ = [ 'dump'
            ]


## Message log
from AthenaCommon.Logging import logging
log = logging.getLogger( 'TrigHistoryDumper.py' )

## Status code
from AthenaPython.PyAthena import StatusCode


class Dumper:
    @classmethod
    def getDumperInstance(cls,filename):
        if filename[-4:] == '.xml' :
            return XMLDumper(filename)
        else:
            return TextDumper(filename)

class TextDumper(Dumper):
    def __init__(self, filename):
        if filename != '<no value>' :
            if '.' in filename:
                filename = ".".join(filename.split('.')[:-1]) + "_setup." + filename.split('.')[-1]
            else:
                filename += "_setup"
            self.FH = open(filename, 'w')
            log.info("Starting to dump to file '%s'" % filename)
        else:
            import sys
            self.FH = sys.stdout
        self.FH.write("SETUP\n")

    def append(self, what, algtype, dumpChildren):
        self.FH.write(TextDumper.algToStr(what, dumpChildren=dumpChildren))

    def finalize(self):
        if type(self.FH)==file:
            self.FH.close()

    @classmethod
    def algToStr(cls, alg, dumpChildren=True) :
        py_class = str(alg.__class__)[6:].strip('\' >').split('.')[-1]
        py_package = str(alg.__module__)
        alg_name = None
        try :
            alg_name = alg.getType()
        except :
            alg_name = alg.getFullJobOptName().split('/')[0]
        alg_alias = alg.getFullJobOptName().split('/')[-1]

        if alg_name == 'ApplicationMgr':
            properties = dict([(p,v.value()) for p,v in alg.getHandle().properties().items()])
        else:
            properties = alg.getProperties()

        # Actual printout
        outputStr  = ">> " + alg_alias + "\n"
        outputStr += "Name: " + alg_alias + "\n"
        outputStr += "Type: " + alg_name + "\n"
        outputStr += "Py_Name: " + py_class + "\n"
        outputStr += "Py_Package: " + py_package + "\n"

        if len(properties) > 0:
            outputStr += "Properties: [\n"
            for k,v in properties.items() :
                if v=='<no value>': v = alg.getDefaultProperties()[k]
                outputStr += "%s\n" % (cls.propertyToStr(k,v))
            outputStr += "]\n"

        outputStr += "\n"

        if dumpChildren :
            for child in alg.getAllChildren() :
                outputStr += cls.algToStr(child, dumpChildren)

        return outputStr
       
    @classmethod
    def propertyToStr(cls, name, value) :
      """Convert property to string
      """
      outputStr = ''

      from GaudiKernel.GaudiHandles import PrivateToolHandle, PrivateToolHandleArray, PublicToolHandle, PublicToolHandleArray, ServiceHandle, ServiceHandleArray
      from AthenaCommon.Configurable import Configurable
      # PrivateTool, PublicTool, Service HandleArray property
      if isinstance(value, (PrivateToolHandleArray, PublicToolHandleArray, ServiceHandleArray)):
        newValue = []
        for item in value :
          try :
            newValue.append("%s/%s" % (item.getType(),item.getName()))
          except :
            newValue.append("%s" % (item))
        value = "%s" % (newValue)
      # PrivateTool, PublicTool, Service Handle property
      elif isinstance(value, (PrivateToolHandle, PublicToolHandle, ServiceHandle, Configurable)):
        try:
          value = "%s/%s" % (value.getType(),value.getName())
          if value.strip() == "/" : value =""
        except:
          pass

      outputStr = '\'%s\':%s' % (name, value)
      return outputStr


class XMLDumper(Dumper):
    def __init__(self, filename):
        import xml.dom.minidom

        filename = ".".join(filename.split('.')[:-1]) + "_setup." + filename.split('.')[-1]
        self.FH = open(filename, 'w')
        log.info("Starting to dump to file '%s'" % filename)

        impl = xml.dom.minidom.getDOMImplementation()
        self.document = impl.createDocument(None, "setup", None)

    def append(self, what, algtype, dumpChildren):
        topElement = self.document.documentElement
        topElement.appendChild(XMLDumper.algToXmlElement(what, document=self.document, algtype=algtype, dumpChildren=dumpChildren))

    def finalize(self):
        document.writexml(self.FH, addindent='  ', newl='\n')

    @classmethod
    def algToXmlElement(cls, alg, document, algtype='Algorithm', dumpChildren=True) :
        py_class = str(alg.__class__)[6:].strip('\' >').split('.')[-1]
        py_package = str(alg.__module__)
        alg_name = alg.getType()
        alg_alias = alg.getFullJobOptName().split('/')[-1]

        algorithm='1'
        topalg=steeralg=svc=pubt=privt=aud='0'
        if algtype == 'TopAlg': topalg='1'
        if algtype == 'SteerAlg': steeralg='1'
        if algtype == 'Service': svc='1'
        if algtype == 'PublicTool': pubt='1'
        if algtype == 'PrivateTool': privt='1'
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
        component.setAttribute('pubt', pubt)
        component.setAttribute('privt', privt)
        component.setAttribute('aud', aud)

        # append the properties
        for k,v in alg.getproperties().items() :
            if v=='<no value>': v = alg.getDefaultProperties()[k]
            component.appendChild(cls.propertyToXmlElement(document,k,v))

        if dumpChildren :
            # append the child algorithms
            if alg_name == 'HLT::TrigSteer': algtype = 'steeralg'
            for child in alg.getAllChildren() :
                if algtype != 'steeralg' : algtype='privatetool'
                component.appendChild(cls.algToXmlElement(child, document=document, algtype=algtype, dumpChildren=dumpChildren))

        return component


    @classmethod
    def propertyToXmlElement(cls, document, name, value) :
      """Convert property to the xml element
      """
      from GaudiKernel.GaudiHandles import PrivateToolHandle, PrivateToolHandleArray, PublicToolHandle, PublicToolHandleArray, ServiceHandle, ServiceHandleArray

      # PrivateTool, PublicTool, Service HandleArray property
      if isinstance(value, (PrivateToolHandleArray, PublicToolHandleArray, ServiceHandleArray)):
        newValue = []
        for item in value :
          try :
            newValue.append("%s/%s" % (item.getType(),item.getName()))
          except :
            newValue.append("%s" % (item))
        value = "%s" % (newValue)

      elif isinstance(value, (PrivateToolHandle, PublicToolHandle, ServiceHandle)):
        try:
          value = "%s/%s" % (value.getType(),value.getName())
        except:
          pass

      parameter = document.createelement('parameter')
      parameter.setAttribute('name', str(name))
      parameter.setAttribute('value', str(value))
      parameter.setAttribute('type', prop_type)
      parameter.setAttribute('op','set')

      return parameter




def dump(outputFilename):
    log.info("Start dumping configuration for further use in the trigger configuration database")

    dumper = Dumper.getDumperInstance(outputFilename)

    from AthenaCommon.AppMgr import ToolSvc, ServiceMgr, theApp
    svcMgr = theApp.serviceMgr()
    toolSvc = svcMgr.ToolSvc
    topAlgs = theApp.TopAlg
    auditorSvc = svcMgr.AuditorSvc

    # Dump the ApplicationMgr
    dumper.append(theApp, algtype='Algorithm', dumpChildren=False)

    # Dump the Tools
    dumper.append(toolSvc, algtype='Service', dumpChildren=False)
    for alg in toolSvc.getAllChildren():
        dumper.append(alg, algtype='PublicTool', dumpChildren=True)
    
    # Dump the Auditors
    dumper.append(auditorSvc, algtype='Service', dumpChildren=False)
    for alg in auditorSvc.getAllChildren():
        dumper.append(alg, algtype='Auditor', dumpChildren=True)

    # Dump the Services
    dumper.append(svcMgr, algtype='Service', dumpChildren=False)
    for alg in svcMgr.getAllChildren():
        if alg.getName() in ('AuditorSvc','ToolSvc'): continue
        dumper.append(alg, algtype='Service', dumpChildren=True)

    # Dump the top Algorithms
    from AthenaCommon.ConfigurableDb import getConfigurable
    from AthenaCommon.Configurable import Configurable

    for algNameClass in topAlgs:
        algClass, algName = algNameClass.split('/')[0:2]
        alg = None
        if algName in Configurable.allConfigurables:
            alg = Configurable.allConfigurables[algName]
            if alg.getFullName() != algNameClass:
                alg = None

        if alg==None:
            from AthenaCommon import CfgMgr
            cfg_class = getConfigurable(algClass, assumeCxxClass=True)
            stmt = 'alg = CfgMgr.%s("%s")' % (cfg_class.__name__,algName)
            try:
                exec stmt
            except Exception, exc:
                print "ERROR:\t\tTop algorithm: %s/%s was not assigned: %s" % (algClass,algName)
                print exc
        if alg:
            dumper.append(alg, algtype='TopAlg', dumpChildren=True)
        else:
            print "Top algorithm: %s/%s was not assigned" % (algClass,algName)

    # Dump user defined Configurables
    for c in Configurable.allConfigurables.itervalues():
        if c.getGaudiType()!='User': continue
        dumper.append(c, algtype='User', dumpChildren=True)

    # closing the files
    dumper.finalize()

    log.info("Finished dumping the configuration")
    return StatusCode.Success


