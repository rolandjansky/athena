# File: AthenaCommon/JobOptTraceInclude.py
# Author: Massimo Marino (Massimo.Marino@lbl.gov)
# Modified: Wim Lavrijsen (WLavrijsen@lbl.gov) [Mar,20,2005]

__version__ = "$Revision: 1.46 $"
__author__  = "Massimo Marino"
#
#
import traceback
import sys

def exception():
   try:
      type, value, tb = sys.exc_info()
      info = traceback.extract_tb(tb)
      filename, lineno, function, text = info[-1] # last line only
      print "%s:%d: %s: %s (in %s)" %\
            (filename, lineno, type.__name__, str(value), function)
   finally:
      type = value = tb = None # clean up

            
### property change to traceback association -------------------------------
class Change( object ):
   """Property value change and its traceback.Internal use only."""

   def __init__( self, traceback, property, attribute, value ):
      self.traceback = traceback
      self.property = property
      self.attribute = attribute
      self.value = value


### doctor the given class for change collection ----------------------------
def _instrument( Class ):
   """Internal function."""

   original__setattr__ = Class.__setattr__

   def getProperty( self, name ):
      ip = self.getInterface()
      if ip:
         return ip.getProperty( name )
    # raise "Property %s not found on %s" % (name, self.name())

   def __setattr__( self, attrname, value ):
      original__setattr__( self, attrname, value )

      history = self.__dict__.setdefault( 'history', {} )
      changes = history.setdefault( attrname, [] )
      changes.append( Change( traceback.extract_stack(), self, attrname, value ) )

      if len(changes) > 1:
         changed = self.name()+"."+attrname
         JOT._multiChanged.setdefault(changed, len(changes))
         JOT._multiChanged[changed]=(len(changes))

      JOT._changesTrace.append(Change(traceback.extract_stack(), self, attrname, value))

   Class.__setattr__ = __setattr__


### tracer implementation ---------------------------------------------------
class JobOptTraceInclude( Include ):
   """Tracing of changes to properties and jobOptions inclusion tree reports.
   
      This Python tool builds a history tree for property changes in Athena.
      The current state of the tree can be queried at any point in time.

 Usage:
      Apply it as the first script to the normal run, then pick up the
      results either at the end of your script or interactively as in
      the following example:

 $ athena.py -i AthenaCommon/JobOptTraceInclude.py AthExHelloWorld/HelloWorldOptions.py

 Alternative Use:
      You can add the reporting in another options file and run this lasts
      after your normal configuration, as in:

 $ cat report.py
   JOT.propReport( HelloWorld, tb=True )
 $ athena.py AthenaCommon/JobOptTraceInclude.py AthExHelloWorld/HelloWorldOptions.py report.py
"""

   def _helpOnJOT(self):
      """On Python prompt (>>>) issue help(JobOptTraceInclude) to access help on
      this module through Python help system. The tool is nicknamed JOT.
      
      This Python tool builds a history tree for property changes in Athena.
      The current state of the tree can be queried at any point in time.
      
      Usage: apply it as the first script to the normal run, then pick up the
      results either at the end of your script, via a GUI or interactively as in
      the following examples:
      
      $ athena.py -i AthenaCommon/JobOptTraceInclude.py AthExHelloWorld/HelloWorldOptions.py
      
      athena> JOT.propReport( HelloWorld )
      ------------------------------------------------------------
      [..]/InstallArea/jobOptions/AthExHelloWorld/HelloWorldOptions.py at line 43
      HelloWorld.MyBool = 1
      ------------------------------------------------------------
      [..]/InstallArea/jobOptions/AthExHelloWorld/HelloWorldOptions.py at line 42
      HelloWorld.MyInt = 42
      
      athena> JOT.propReport( HelloWorld, tb = True )  # request traceback as well
      
      athena> JOT.propReport( HelloWorld, 'MyStringVec' )
      [..]/InstallArea/jobOptions/AthExHelloWorld/HelloWorldOptions.py at line 45
      HelloWorld.MyStringVec = [ "Welcome", "to", "Athena", "Framework", "Tutorial" ]

      athena> JOT.propReport( HelloWorld, 'MyStringVec', tb = True )
      
      
      Alternatively, you can add the reporting in another options file and run
      this after your normal configuration, like so:
      
      $ cat report.py
      JOT.propReport( HelloWorld, tb=True )
      $ athena.py AthenaCommon/JobOptTraceInclude.py AthExHelloWorld/HelloWorldOptions.py report.py
      
      ------------------------------------------------------------
      [..]/InstallArea/jobOptions/AthExHelloWorld/HelloWorldOptions.py at line 43
      HelloWorld.MyBool = 1
      [...]
      
      You can trace down the includes (blocked ones will not appear, but
      those that employ an internal block guard will) for a specific joboption file:
      
      $ athena.py -i AthenaCommon/JobOptTraceInclude.py myTopOptions.py
      
      athena> JOT.joTrace( 'LArCondCnv/LArCondCnv_Config_jobOptions.py' )
      ------------------------------------------------------------
      [..]/InstallArea/share/bin/athena.py at line 303
      include( script )
      
      ../run/myTopOptions.py at line 39
      include ("RecExCommon/RecExCommon_topOptions.py")
      
      [..]/InstallArea/jobOptions/LArCondCnv/LArCondCnv_DetDescr_jobOptions.py at line 4
      include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
      ------------------------------------------------------------
      
      [..]/InstallArea/jobOptions/LArCondCnv/LArCondCnv_G4_jobOptions.py at line 4
      include( "LArCondCnv/LArCondCnv_Config_jobOptions.py" )
      
      By default include trace follows a FIFO policy.
      If you want to have the immediate *includer* jobOption spat out first (LIFO)
      just say so, as in:
      
      $ athena> JOT.joTrace('RecExCommon/CBNT_config.py', LIFO=True)
      ------------------------------------------------------------
      ./RecExCommon_topOptions.py at line 401
      include( "RecExCommon/CBNT_config.py" )
      ------------------------------------------------------------
      ./myTopOptions.py at line 40
      include ("RecExCommon_topOptions.py")
      ------------------------------------------------------------
      /afs/cern.ch/atlas/software/dist/10.0.0/InstallArea/share/bin/athena.py at line 303
      include( script )
      
      
      You may get the include cascade from a specific jobOption down to specified
      depth level. For example:
      
      athena> JOT.joTree('iPatRecExample/iPatRec_jobOptions.py')
      |
      --- iPatRecAlgs/iPatRec_jobOptions.py
      --- iPatRecAlgs/iPatTrackTruthAssociator_jobOptions.py
      --- iPatRecAlgs/iPatStatistics_jobOptions.py
      
      or:
      
      athena> JOT.joTree('iPatRecExample/iPatRec_jobOptions.py',1)
      |
      --- iPatRecAlgs/iPatRec_jobOptions.py
      |
      --- iPatRecAlgs/TrajectorySvc_jobOptions.py
      --- iPatTrackFinder/TrackFinder_jobOptions.py
      --- iPatGeometry/AlignmentTolerances_jobOptions.py
      --- TRT_Rec/TRT_Rec_jobOptions.py
      --- iPatRecAlgs/iPatTrackTruthAssociator_jobOptions.py
      |
      --- iPatTruthTrajectory/TruthSelector_jobOptions.py
      --- iPatRecAlgs/iPatStatistics_jobOptions.py
      |
      --- iPatTruthTrajectory/TruthSelector_jobOptions.py
      
      You get a report of jobOptions files that have been included multiple
      times (counts those with internal block guards as well). For example:
      
      athena> JOT.joWarns()
      The following jobOptions have been processed more than once:
      ------------------------------------------------------------
      LArCondCnv/LArCondCnv_Config_jobOptions.py included 2 times
      CaloIdCnv/CaloIdCnv_joboptions.py included 4 times
      LArClusterRec/LArCluster_G4_corrections.py included 2 times
      TileConditions/TileConditions_jobOptions.py included 2 times
      iPatTruthTrajectory/TruthSelector_jobOptions.py included 2 times
      TileIdCnv/TileIdCnv_jobOptions.py included 2 times
      InDetTrackSummaryHelperTool/InDetTrackSummaryHelperTool_jobOptions.py included 2 times
      LArClusterRec/LArCluster_corrections_class.py included 2 times
      ------------------------------------------------------------
      Check them with calls to joTrace and joTree to trace down the above multiple inclusions.
      
      athena> JOT.propWarns()
      
      propWarns is the corresponding method for Properties as joWarns()
      is for jobOptions. It reports properties that have been changed more than
      once during the configuration process.
      
      athena> JOT.joDumps()
      joDumps creates a flat job option file. This is done with a two-fold purpose:
      as a debug aid and as a reproducibility feature.
      The flat job option file should allow you to freeze various configurations
      pertaining to specific purposes.
      
      athena> JOT.gui() starts the GUI.
"""


   def __init__( self, org ):
      """Initialize the dictionaries used for traceback."""
      Include.__init__( self, org._show )
      for name, value in org.__dict__.items():
         if type(value) == str:
            value = '"%s"' % value
         exec 'self.%s = %s' % (name,value)
         
      self._filetrace = {}
      self._includeTree = {}
      self._multiInclude = {}
      self._multiChanged = {}
      self._athenaGlobals = {}
      self._dclSvcAlg = {}
      self._changesTrace = []
      self._msg = ""
      self._msgpieces = []
      self.jomenu    = None # hacks to detect gui has started
      self.joframe   = None
      self.propframe = None  
      
   def __call__( self, filename, *args, **kw ):
      """Include filename in the current scope by executing it globally."""
      Include.__call__( self, filename, *args, **kw )

    # TODO: don't use base class private member.Get a method to provide this info.
      if not filename in self._once:
         incltrace = self._filetrace.setdefault( filename, [] )
         incltrace.append( traceback.extract_stack() )

    # Counts multiple inclusions of this jobOption
         if len(incltrace)>1:   
            self._multiInclude.setdefault(filename,len(incltrace))
            self._multiInclude[filename]=len(incltrace)

      # Internal method
      def buildTree(filename):
         tb = traceback.extract_stack()
         # Last first
         tb.reverse()
         # Determine caller of filename joboption and store it as
         # Package/jobOption names combo
         for t in tb:
            if t[3] and t[3].startswith('include'):
               jopath = t[0].split('/')
               
               # hackish detection of user local joboption
               if jopath[0] is ".": jopath.remove(".")
               
               jocaller = '/'.join(jopath[len(jopath)-2:len(jopath)])
               tree = self._includeTree.setdefault(jocaller,[])
               tree.append(filename)
               # not interested in the rest of full backtrace to build the tree 
               break
            
      buildTree(filename)

   def _printMsg(self, recursive=False):
      tmpmsg = "".join(self._msgpieces)
      if recursive:
         # cumulate messages
         self._msg += tmpmsg
      else:
         # single message
         self._msg = tmpmsg

      # shell scrolls hence we may print each tmpmsg in sequence
      if self.jomenu and self.jomenu.cliON():
         print tmpmsg
      elif self.jomenu and not self.jomenu.guiAct():
         print tmpmsg
      elif not self.jomenu:
         print tmpmsg
      # this operation msg result can then be cleared
      self._msgpieces = []

   def _printDict(self, key, sort=False):
      try:
         dict = theApp.properties()[key]
      except KeyError, e:
         #exception()
         self._msgpieces = ["Application Manager not available: " + str(e)]
         self._printMsg()
         return
      if sort: dict.sort()
      self._msgpieces = ["\n" + item for item in dict]
      self._printMsg()

   def extServices(self,sort=False):
      """Lists the external services known to Application Manager."""
      self._printDict('ExtSvc',sort)
      
   def topAlgs(self,sort=False):
      """Lists the top algorithms known to Application Manager."""
      self._printDict('TopAlg',sort)

   def Dlls(self,sort=False):
      """Lists the Dlls known to Application Manager."""
      self._printDict('Dlls',sort)
      

   def joTree( self, filename, depth=0, _level=0):
      """Reports the include tree from a given jobOption, with optional
         depth of inclusion.
      
      Usage example:
      
      $ JOT.joTree('iPatRecExample/iPatRec_jobOptions.py',#)

      Default depth is 0 to only report those includes the examined
      jobOption is directly responsible for.
      """
      try:
         root = self._includeTree[filename]
         self._msgpieces.append("   "*_level+"|\n")
      except KeyError:
         # exception()
         # Check all known jobOptions (with/out tree) for a typo on filename
         if _level == 0: # but not while recursively constructing the tree
            # ... and they do not appear in the gui
            self._msgpieces = ["%s is not a known jobOption. A typo?" % filename]
            for inclusions in self._includeTree.values():
               if filename in inclusions:
                  self._msgpieces = ["No include tree was generated for: %s" % filename]
                  break
            self._printMsg()
            return
         
      # Visually print the tree ( a la 'tree' in Unix )
      for jo in root:
         self._msgpieces.append("   "*_level+("--- %s") % jo + "\n")
         if depth>0 and self._includeTree.has_key(jo):
            _level=_level+1
            self.joTree(jo,depth-_level,_level)
            _level=_level-1
      if _level == 0:
         # end of recursion and print     
         self._printMsg()

   def joTrace( self, filename, LIFO = False ):
      """Reports the include cascade into the given jobOption file.

      Usage example:

      $ JOT.joTrace('RecExCommon/CBNT_config.py')

      By default the trace direction is top-bottom (direct *includer* last).
      To reverse trace direction (direct *includer* first) use:

      $ JOT.joTrace('RecExCommon/CBNT_config.py', True)
      """
      try:
         incltraces = self._filetrace[ filename ][0]
      except KeyError:
      	 # exception()
         # Check whether we face a typo on filename
         self._msgpieces = ["%s is not a known jobOption. A typo?" % filename]
         for traces in self._filetrace.values():
            if filename in traces:
               self._msgpieces = ["No traceback available for %s" % filename]
               break
         self._printMsg()
         return

      # Show include trace by having the immediate *includer* spat out first
      inclusions = incltraces[:] #reverse below happens IN PLACE!
      if LIFO:
         inclusions.reverse() 

      for trace in inclusions:
         # To do: formatting should become a method
         if trace[3] and trace[3].startswith( 'include' ):
            self._msgpieces.append("-"*60+"\n")
            self._msgpieces.append("   %s at line %d\n    %s\n" % (trace[0],trace[1],trace[3]))
      self._printMsg()
      
   def joWarns(self):
      """Reports jobOption files included more than once during configuration process.

      The reported jobOptions though might contain internal logic to
      prevent multiple executions.

      Usage example:

      $ JOT.joWarns()
      """
      loop = "\n"
      for jo,times in self._multiInclude.iteritems():
         loop += "%s included %d times\n" % (jo, times)
      if len(loop) >1:
         self._msgpieces = ["The following jobOptions have been processed more than once:\n",
                            "-"*60,
                            loop,
                            "-"*60,
                            "\nCheck them with calls to joTrace and joTree to trace down the above multiple inclusions."]
      else:
         self._msgpieces = ["No jobOptions have been processed more than once."]
         
      self._printMsg()

   def joDumps(self):
      """Dumps the collected configuration changes into a flat jobOption file named myFlatOptions.py.
      
      Usage example:

      $ JOT.joDumps()
      """
      # Output to file. Will evolve.
      filename = "myFlatOptions.py"
      try:
         # Open file stream
         file = open(filename, "w")
      except IOError:
         #exception()
         self._msgpieces = ["There was an error writing to %s" % filename]
         self._printMsg()
         sys.exit()
      
      for change in JOT._changesTrace:
         newline = ""
         joLine = change.traceback[-2][3]
         if type(change.property.name) is str:
            propName = change.property.owner.name()+"."+change.property.name
         else:
            propName = change.property.name()
            
         if propName == "ApplicationMgr": propName = "theApp"
         try:
            value = change.property.properties()[change.attribute]
         except:
            #exception()
            value = change.value
         if joLine:
            # There is indeed a recorded property change.
            # Do not report setattr changes though
            if "setattr" not in joLine:
               # Tried different more simple solutions.
               # Unfortunately they do not cover all possible cases
               if type(change.value) != str:
                  # the property value should be changed thusly
                  newline = "%s.%s = %s\n" % (propName,
                                              change.attribute,
                                              value)
               else:
                  newline = '%s.%s = "%s"\n' % (propName,
                                                change.attribute,
                                                change.value)
                                                
            # Sequences can be tricky as developers play with them.
            # Preserve "+=" if possible, otherwise keep above general case.
            if joLine.find("+=")>0:
               # and sequence is complete
               if joLine.rfind(']')+1 == len(joLine) :
                  newline = joLine + "\n"
                  # cover local variable computations
                  if newline.find("%")>0:
                     newline = "%s.%s = %s\n" % (propName,
                                                 change.attribute,
                                                 value)
                  
            # Some property names are bogus: contain "::".
            # Make no sense, hence get the actual line:
            if propName.find("::")>0:
               newline = joLine + "\n"
               
         # Very rare but happens: missing line but property
         # has a tracedbacked change anyway
         else:
            if type(change.value) != str:
               newline = "%s.%s = %s\n" % (propName,
                                           change.attribute,
                                           value)
            else:
               newline = '%s.%s = "%s"\n' % (propName,
                                             change.attribute,
                                             change.value)
                  
         # Define locally named properties as Algs/Svcs.
         # Only first time and for main Properties only (not "prop.prop" Svcs/Algs)
         if propName.find(".")>0:
            propName = propName[:propName.find(".")]
         if propName.find("::")>0:
            propName = propName[propName.find("::")+2:]
         # and there are non-pythonic names as well????? ::
         if not self._dclSvcAlg.has_key(propName):
            if type(change.property) is iAlgorithm:
               dcl_as = ' = Algorithm("%s")\n' % propName
               doDefine(dcl_as)
            elif type(change.property) is iService:
               dcl_as = ' = Service("%s")\n' % propName
               doDefine(dcl_as)

            def doDefine(as):   
               propdef = self._dclSvcAlg.setdefault(propName,as)
               declaration = propName + propdef
               # Output local property definition
               self._msgpieces.append(declaration)
               file.write(declaration)
         # actual lines - debug only
         #actline = "#DEBUG %s at line %d\n" % (change.traceback[-2][0] , change.traceback[-2][1])
         #file.write(actline)

         # Output configuration change
         self._msgpieces.append(newline)
         file.write(newline)
      
   
      self._printMsg()
      file.close()

   def propWarns(self):
      """Reports properties that got changed more than once during configuration process.

      The report list that might be just properly growing.

      Usage example:

      $ JOT.propWarns()
      """
      listprops = []
      for prop, changes in self._multiChanged.iteritems():
         listprops += ["%s property has been updated %d times\n" % (prop,changes)]

      self._msgpieces = ["No property have been processed more than once."]
      if len(listprops) >1:
         strlist = "\n"
         listprops.sort()
         for prop in listprops: strlist += prop
         self._msgpieces = ["The following properties have been changed more than once:\n",
                            "-"*60,
                            strlist,
                            "-"*60,
                            "\nCheck them with calls to propReport to trace the above multiple changes."]
         
      self._printMsg()
              
   def propReport( self, obj, attribute=None, tb=False , recursive=False):
      """Reports history of the changes for the given property object and
      of one of its specific attribute, if specified.
      
      If tb == True, a full traceback is provided.

      Usage example:

      $ JOT.propReport(CaloCellMaker)
      $ JOT.propReport(CaloCellMaker, tb=True)
      $ JOT.propReport(CaloCellMaker,'CaloCellsOutputName')
      $ JOT.propReport(CaloCellMaker,'CaloCellsOutputName', tb=True)

      Wildcard (*) is supported on the property objects (as string)
      and attributes, as in:

      $ JOT.propReport(CaloCellMaker,'CaloCell*')
      $ JOT.propReport('CaloSW*','Clus*')   
"""
      def format( changes ):
         for change in changes:
            formatsingle(change)

      def formatsingle(change):
         line = None
         if type(change.property.name) is str:
            propName = change.property.name
         else:
            propName = change.property.name()
         # get the traceback line modifying the property value
         tb = change.traceback[:]
         tb.reverse()
         for tbline in tb:
            for el in tbline:
               if propName+"." in str(el):
                  line = tbline
                  break
         if line:
            self._msgpieces.append("  %s at line %d\n    %s\n" % (line[0],line[1],line[3] or ("%s.%s" % (obj.name(),change.attribute))))
                                   
            # In case of multi line list the value is not available from the extracted line
            if not line[3] or ( line[3].find( '[' ) > 0 and line[3].find( ']' ) < 0 ):
               self._msgpieces.append("    Actual value is: %s\n" % change.value)
               self._msgpieces.append("-"*60+"\n")
         else:
            if (change.value) is str:
               self._msgpieces.append(' changed by user\n %s.%s = "%s"\n' % (obj.name(), change.attribute, change.value))
            else:
               self._msgpieces.append(" changed by user\n %s.%s = %s\n" % (obj.name(), change.attribute, change.value))
               
      def backtraceit( changes ):
         for change in changes:
            for t in change.traceback:
               if t[3] and t[3].startswith( 'include' ):
                  self._msgpieces.append("  %s at line %d\n    %s\n" % (t[0],t[1],t[3]))
            formatsingle ( change )

      def reportChanges(attribute):
         try:
            changes = history[attribute]
         except KeyError:
            #exception
            self._msgpieces = ["%s not known or never modified after creation." % attribute]
            self._printMsg()
            return
         if tb:
            backtraceit( changes )
         else:
            format ( changes )
         
      def printreport( history, attribute, recursive=False):
         if attribute:
            if attribute.endswith("*"): # Wildcard attribute name with ending "*"
               wildcard = attribute[:-1]
               for attrkey in history.iterkeys():
                  if attrkey.startswith(wildcard):
                     reportChanges(attrkey)
            else:
               reportChanges(attribute)
               
         else: # for all changed attributes
            for attrkey in history.iterkeys():
               reportChanges(attrkey)

         self._printMsg(recursive)

      def getHistory(obj):      
         try:
            history = obj.__dict__[ 'history' ]
            return history
         except AttributeError: pass
         except KeyError:
         #exception()
            self._msgpieces = ["Unknown or unmodified since its construction."]
            self._msgpieces.extend(obj.properties())
            self._printMsg()
            return

      if type(obj) == str:
         if obj.endswith("*"): # Wildcard on Property object
            wildcard = obj[:-1]
            for globj in globals().iterkeys():
               if globj.startswith(wildcard):
                  obj = globals()[globj]
                  if isinstance(obj, iProperty) or \
                         isinstance(obj, _PropertyProxy):
                     self._msgpieces.append("*** Report on %s ***\n" % globj)
                     self._msgpieces.append("+"*60+"\n")
                     history = getHistory(obj)
                     if history: printreport( history, attribute )
            return
         
         else:
            if globals().has_key(obj):
               obj = globals()[obj]
            elif globals().has_key(obj[:obj.find(".")]) and globals()[obj[:obj.find(".")]].__dict__.has_key(obj[obj.find(".")+1:]):
               obj = globals()[obj[:obj.find(".")]].__dict__[obj[obj.find(".")+1:]]
            
            # Throw up
            else:
               if obj in theApp.properties()['TopAlg']:
                  self._msgpieces = ["TopAlg %s not changed from creation." % obj]
               elif obj in theApp.properties()['ExtSvc']:
                  self._msgpieces = ["Service %s not changed from creation." % obj]
               else:
                  self._msgpieces = ["The string value %s is not an Algorithm nor a Service." % obj]

               self._printMsg()
               return

      history = getHistory(obj)
      if history:
         printreport( history, attribute , recursive)

         if not attribute:
            # If asking for complete report take into account that
            # obj may contain other Svcs and Algs.
            # Report them recursively.
            for subobj in obj.__dict__.values():
               if type(subobj) is iService or type(subobj) is iAlgorithm:
                  self.propReport(subobj, recursive=True)

   def gui(self):

      def _importCode(code, name, add_to_sys_modules=0):
         """Internal. code can be any object containing code -- string, file object,
         or compiled code object. Returns a new module object initialized by
         dynamically importing the given code and optionally adding it to
         sys.modules under the given name.
         """
         import imp
         module = imp.new_module(name)
         
         if add_to_sys_modules:
            sys.modules[name] = module
            
         exec code in module.__dict__
         return module

      self._athenaGlobals = globals()
      JOTGUI = _importCode(self.GUI, "JOTGUI",1)
      JOTGUI.buildMain()
      
      return


### setup monitoring and activate tracer ------------------------------------
_instrument( iProperty )
_instrument( _PropertyProxy )
include = JobOptTraceInclude( include )
JOT = include
log.info( "************* JobOption Tracing ACTIVATED ****************" )


##########################  GUI stuff #######################################
JOT.GUI = """
import pydoc
from __main__ import JOT, JobOptTraceInclude
from Tkinter import *
from SimpleDialog import SimpleDialog

class StatusBar(Frame):

    def __init__(self, master):
        Frame.__init__(self, master)
        self.label = Label(self, bd=1, relief=SUNKEN, anchor=W)
        self.label.pack(fill=X)

    def set(self, format, *args):
        self.label.config(text=format % args)
        self.label.update_idletasks()

    def clear(self):
        self.label.config(text="")
        self.label.update_idletasks()

class Command:
    def __init__(self, callback, *args, **kwargs):
        self.callback = callback
        self.args = args
        self.kwargs = kwargs

    def __call__(self):
        return apply(self.callback, self.args, self.kwargs)

def createIntCB(master,label):
   master.cbVar = IntVar()
   master.fulltrace = Checkbutton(master, text=label,
                                variable=master.cbVar,
                                anchor=NE)   
   master.fulltrace.grid(row = 2)
   return master.fulltrace

def createListbox(master):   
   lbrow=3
   joYscroll  =  Scrollbar (master, orient=VERTICAL )
   joXscroll  =  Scrollbar (master, orient=HORIZONTAL )
   
   master.jolistbox = Listbox (master,selectmode="SINGLE",width=40,height=20,
                             xscrollcommand=joXscroll.set,
                             yscrollcommand=joYscroll.set)
   master.jolistbox.grid ( row=lbrow, column=0, sticky=N+S+E+W )
   joXscroll.config(command=master.jolistbox.xview)
   joYscroll.config(command=master.jolistbox.yview)
   
   joYscroll.grid ( row=lbrow, column=1, sticky=N+S )        
   joXscroll.grid ( row=lbrow+1, column=0, sticky=E+W )

   return master.jolistbox


def buildMain():
   jotMain = Tk()
   jotMain.resizable(0,0)

   # Main Frames
   JOT.propframe = propFrame(jotMain)
   JOT.joframe = joFrame(jotMain)
   JOT.jomenu = menuFrame(jotMain)

   # Statusbar
   JOT.status = StatusBar(jotMain)
   JOT.status.grid(column=0, columnspan=6, sticky="W")
 
   # Author
   me = StringVar()
   me.set("Massimo Marino - LBNL")
   lbl = Label(jotMain, textvariable=me,
               fg="blue", font="Helvetica", justify="right")
   lbl.grid(column=2,columnspan=2,sticky="NE")
   
   # Show it
   jotMain.title("JOT")


class propFrame(Frame):
    def __init__(self, master=None):
        "Property Frame"
        Frame.__init__(self, master, class_="propFrame",takefocus=1, relief="ridge")
        self.grid(row=1,column=2, columnspan=2, sticky=N+W+E)

        self.traceCheck = createIntCB(self,"Full Trace")
        self.proplistbox = createListbox(self)

        # Collect all processed Properties alphabetically sorted
        # hackish solution at the moment
        gobjdir = JOT._athenaGlobals
        gobjKeys = gobjdir.keys()
        gobjKeys.sort()
        for key in gobjKeys:
           gobj = gobjdir[key]
           if str(type(gobj)).find("gaudimodule")>0:
              self.proplistbox.insert(END,key)
              for subobj in gobj.__dict__.values():
                 if str(type(subobj)).find("gaudimodule")>0:
                    self.proplistbox.insert(END,subobj.name())

        # Entry for user selection
        self._slctdprop = StringVar()
        self.propSelection = Entry(self,bg="white",textvariable=self._slctdprop,width=40)
        self.propSelection.grid(row=6)

        # Button to trigger selection
        self.findProp = Button(self,text="Find",command=self.show)
        self.findProp.grid(row=6,column=2,sticky=W)

    def show(self):
        self.proplistbox.selection_clear(0,END)
        propName = self._slctdprop.get()
        knownProps = list(self.proplistbox.get(0,END))
        if propName in knownProps:
           # property is fully identified
           idx = knownProps.index(propName)
           self.proplistbox.see(idx)
           self.proplistbox.selection_set(idx)
        else:
           for prop in knownProps:
              if prop.startswith(propName):
                 idx = knownProps.index(prop)
                 self.proplistbox.see(idx)
                 break
              
class joFrame(Frame):
    def __init__(self, master=None):
        "jobOptions Frame"
        Frame.__init__(self, master, class_="joFrame",takefocus=1,relief="ridge")
        self.grid(row=1, column=0, columnspan=2, sticky=N+W+E)

        self.lifoCheck = createIntCB(self,"Reverse Trace")
        # Listbox of jobOptions 
        self.jolistbox = createListbox(self)

        # Collect all processed jobOption files
        joptWithTree = JOT._includeTree.keys()
        joptWithTree.sort()
        for jopt in joptWithTree:
           self.jolistbox.insert(END, jopt)

        # Separate those without an include tree
        joptNoTree = []
        self.jolistbox.insert(END,"-"*60)
        for subTree in JOT._includeTree.values():
           for elem in subTree:
              if elem not in joptWithTree: joptNoTree.append(elem)

        joptNoTree.sort()
        for jopt in joptNoTree:
           self.jolistbox.insert(END, jopt)


        # scale for tree depth
        self.depthScale = Scale(self, from_=0, to=50, label="Tree depth",
                                orient=HORIZONTAL, resolution=1, length=200)
        self.depthScale.grid()
        
class menuFrame(Frame):
    def __init__(self, master=None):
        "Costructor for the JobOptTraceInclude GUI"
        self.master = master
        self._cli = IntVar()
        self._guiAction = False
        Frame.__init__(self, self.master, class_="menuFrame",relief="raised",bd=2)
        self.grid(row=0, columnspan=5, sticky=N+W+E) 
        self.tk_menuBar(self.JOT_menu(),self.theApp_menu(),self.jo_menu(), self.prop_menu(),self.help_menu(), self.CLI(), self.quitmb())

    def cliON(self):
       return self._cli.get()

    def guiAct(self):
       return self._guiAction

    def JOT_menu(self):
        jotmb = Menubutton(self,text="JOT")
        jotmb.grid(row=0,column=0)
        jotmb.menu = Menu (jotmb, tearoff=0)
        jotmb["menu"] = jotmb.menu
        #jotmb.menu.add_separator()
        jotmb.menu.add_command(label="About...",    command=Command(self.aboutJOT))
        jotmb.menu.add_command(label="Help",        command=Command(self.helpMsg,JOT._helpOnJOT))
        return jotmb
       
    def theApp_menu(self):
        # ApplicationMgr menu
        appmb = Menubutton(self,text="View")
        appmb.grid(row=0,column=1)
        appmb.menu = Menu (appmb, tearoff=0)
        appmb["menu"] = appmb.menu

        appmb.menu.add_command(label="topAlgs",     command=Command(self.methNoArgs,JOT.topAlgs))
        appmb.menu.add_command(label="Dlls",        command=Command(self.methNoArgs,JOT.Dlls))
        appmb.menu.add_command(label="extServices", command=Command(self.methNoArgs,JOT.extServices))
        return appmb

    def jo_menu(self):
       # jobOptions methods menu
       jomb = Menubutton(self,text="jobOptions")
       jomb.grid(row=0,column=2)
       jomb.menu = Menu (jomb, tearoff=0)
       jomb["menu"] = jomb.menu
              
       # jobOptions commands
       jomb.menu.add_command(label="joDumps",
                             command=Command(self.methNoArgs, JOT.joDumps))
       jomb.menu.add_command(label="joTrace",
                             command=Command(self.traceMethod, JOT.joTrace))
       jomb.menu.add_command(label="joTree",
                             command=Command(self.treeMethod, JOT.joTree))
       jomb.menu.add_command(label="joWarns",
                             command=Command(self.methNoArgs, JOT.joWarns))

       return jomb

    def prop_menu(self):
       # properties methods menu
       propmb = Menubutton(self,text="Properties")
       propmb.grid(row=0,column=3)
       propmb.menu = Menu (propmb, tearoff=0)
       propmb["menu"] = propmb.menu
       
       propmb.menu.add_command(label="propReport",
                               command=Command(self.reportMethod, JOT.propReport))
       propmb.menu.add_command(label="propWarns",
                               command=Command(self.methNoArgs, JOT.propWarns))

       return propmb
    
    def help_menu(self):
       # help menu
       hlpmb = Menubutton(self,text="Help")
       hlpmb.grid(row=0,column=4)
       hlpmb.menu = Menu (hlpmb, tearoff=0)
       hlpmb["menu"] = hlpmb.menu

       hlpmb.menu.add_command(label="topAlgs",     command=Command(self.helpMsg,JOT.topAlgs))
       hlpmb.menu.add_command(label="Dlls",        command=Command(self.helpMsg,JOT.Dlls))
       hlpmb.menu.add_command(label="extServices", command=Command(self.helpMsg,JOT.extServices))
       hlpmb.menu.add_separator()
       hlpmb.menu.add_command(label="propReport",  command=Command(self.helpMsg,JOT.propReport))
       hlpmb.menu.add_command(label="propWarns",   command=Command(self.helpMsg,JOT.propWarns))
       hlpmb.menu.add_separator()
       hlpmb.menu.add_command(label="joDumps",     command=Command(self.helpMsg,JOT.joDumps))
       hlpmb.menu.add_command(label="joTree",      command=Command(self.helpMsg,JOT.joTree))
       hlpmb.menu.add_command(label="joTrace",     command=Command(self.helpMsg,JOT.joTrace))
       hlpmb.menu.add_command(label="joWarns",     command=Command(self.helpMsg,JOT.joWarns))
       hlpmb.menu.add_separator()
       hlpmb.menu.add_command(label="CLI",         command=Command(self.helpMsg,self.CLI))
       hlpmb.menu.add_command(label="Quit",         command=Command(self.helpMsg,self.Quit))
       return hlpmb


    def aboutJOT(self):
       SimpleDialog(self.master,
                    text="JOT - A Job Option Tracer for Athena. CLI and GUI tool for Athena framework. Builds an history tree of Property changes and jobOptions inclusion. Select JOT->Help for more details. Massimo_Marino@lbl.gov",
                    buttons=["Okie"],
                    default=0, title="JobOptTraceInclude").go()

    def popMsg(self, msg):
       self._guiAction = True
       popWin = Toplevel()
       txtScroll = Scrollbar(popWin, orient=VERTICAL)
       textWdg = Text(popWin,yscrollcommand=txtScroll.set)
       txtScroll.config(command=textWdg.yview)
       txtScroll.pack(side=RIGHT,fill=Y)
       textWdg.config(wrap="word")
       textWdg.insert(END,msg)
       textWdg.config(state=DISABLED)
       textWdg.pack(side=LEFT,fill=BOTH,expand=YES) 
       self._guiAction = False
       return popWin

    def helpMsg(self,item):
       self._guiAction = True
       helpText = pydoc.getdoc(item)
       helpWindow = self.popMsg(helpText)
       helpWindow.title("Help on %s" % item.__name__)
       self._guiAction = False
       
    def applyMeth(self,method,args,dict):
       self._guiAction = True
       apply(method,args,dict)
       msgWindow = self.popMsg(JOT._msg)
       if len(dict): args = args+(True,)
       msgWindow.title("JOT.%s%s" % (method.__name__ , args))
       self._guiAction = False
       
    def methNoArgs(self,method):
       self._guiAction = True
       apply(method)
       msgWindow = self.popMsg(JOT._msg)
       msgWindow.title("JOT.%s()" % method.__name__)
       self._guiAction = False
       
    def treeMethod(self,method,args=(),dict={}):
       # collect current selections
       selectIdx = JOT.joframe.jolistbox.curselection()
       treeDepth = JOT.joframe.depthScale.get()
       try:
          args = (JOT.joframe.jolistbox.get(selectIdx), treeDepth)
          self._guiAction = True
       except TclError, e:
          #exception()
          self.popMsg("No jobOption selected! " + str(e))
          return

       self.applyMeth(method,args,dict)
       self._guiAction = False
       
    def traceMethod(self,method,args=(),dict={}):
       # collect current selection
       selectIdx = JOT.joframe.jolistbox.curselection()
       LIFO = JOT.joframe.cbVar.get()
       try:
          args = (JOT.joframe.jolistbox.get(selectIdx), LIFO)
          self._guiAction = True
       except TclError, e:
          #exception()
          self.popMsg("No jobOption selected! " + str(e))
          return

       self.applyMeth(method,args,dict)
       self._guiAction = False
       
    def reportMethod(self,method,args=(),dict={}):
       # collect current selection
       selectIdx = JOT.propframe.proplistbox.curselection()
       if (JOT.propframe.cbVar.get()):
          dict = {"tb":True}
       try:   
          args = (JOT.propframe.proplistbox.get(selectIdx),)
          self._guiAction = True
       except TclError, e:
          #exception()
          self.popMsg("No Property selected! " + str(e))
          return
          
       self.applyMeth(method,args,dict)
       self._guiAction = False       

    def Quit(self):
       "Do'h!"

    def CLI(self):
       "Output on the interactive shell as well if selected"
       cliButton = Checkbutton (self, text="CLI", relief="flat",
                                variable=self._cli, command=self.stateCLI)
       cliButton.grid(row=0, column=5)
       return cliButton

    def quitmb(self):
       # quit
       quitButton = Button (self, text="Quit", activeforeground="red",
                            relief="flat",command=self.tata)
       quitButton.grid(row=0, column=6)
       return quitButton

    def stateCLI(self):
       if self._cli.get():
          args = "CLI active"
       else:
          args = ""
       JOT.status.set("%s",args)

    def tata(self):
       self._cli.set(1)
       self.master.destroy()
"""
