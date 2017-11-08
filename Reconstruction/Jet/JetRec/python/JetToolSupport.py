# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetToolSupport.py
#
# David Adams
# March 2014
#
# Defines the jet tool manager (class JetToolManager, JTM for short)
# which can be used to configure jet reconstruction tools.
#
# Use add or "+=" to register a tool both with this an instance of this
# class and with ToolSvc. The advantage of doing it here is there is a
# check to make sure the same name is not used more than once.
#
# Tools are locked when registered. The method setOutputLevel may
# be used to override this lock to change the tool's output level.
#
# Methods are provide to configure a jetrec tool along with its
# associated finder or groomer. The member "jetrecs" holds the full
# list of offline jetrec tools and can be used to configure the tool or
# algorithm that loops over such tools. The member trigjetrecs holds
# the same for the trigger.  The members "finders" and
# "groomers" hold the finding and grooming tools used by the jetrec
# tools.
#
# The prefix for messages from this tool is held by the member "prefix"
# and the member "debug" may be set to 1 to obtain more messages.
#
# The members "jetBuilderWithArea" and "jetBuilderWithoutArea" hold the
# jet builders used by all jetrec tools.
#
# The member "ghostArea" specifies the area to use for the ghosts
# generated for jet finding.

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

# import the jet flags.
from JetRec.JetRecFlags import jetFlags

# The tool manager
class JetToolManager:
  prefix = "JetToolManager: "
  debug = 0
  m_jetBuilder = None
  jetBuilderWithArea = None
  jetBuilderWithoutArea = None
  tools = { }
  finders = []
  groomers = []
  jetrecs = []
  trigjetrecs = []
  # Jet container names. Add input containers to this list:
  # jetcons += ["SomeExistingJetContainer"]
  jetcons = []
  # pT thresholds in MeV (for finding and filtering after calibration)
  ptminFinder = 2000
  ptminFilter = 10000
  # Map of getter lists.
  gettersMap = {}
  # Map of modifier lists
  modifiersMap = {}

  vertexContainer = "PrimaryVertices"
  trackContainer = "InDetTrackParticles"

  # Print a message to the log.
  def msg(self, lvl, mymsg):
    if lvl <= self.debug: jetlog.debug( self.prefix + str(mymsg) )

  # Add a tool.
  def add(self, mytool):
    myname = mytool.name()
    self.msg(1, "Adding tool " + myname)
    if myname in self.tools:
      self.msg(0, "Tool " + myname + " is already registered")
      raise LookupError
    else:
      jetlog.info( type(mytool) )
      self.tools[myname] = mytool
      from AthenaCommon.AppMgr import ToolSvc 
      ToolSvc += mytool
      mytool.lock()
      setattr(self, myname, mytool)
      return mytool

  # Add a tool with a local name (alias).
  # Used to provide alternative names for tools with long names.
  def addNamed(self, myname, mytool):
    if myname in self.tools:
      self.msg( "Name " + myname + " is already registered")
      raise LookupError
    self.tools[myname] = self.add(mytool)
    setattr(self, myname, mytool)
    return mytool

  # Ad op+= as alternative syntax to add.
  def __iadd__(self, mytool):
    self.add(mytool)
    return self

  # Add for a jet builder.
  # The jet builder is used by by all finders an groomers and
  # must be added with one of these methods.
  def addJetBuilderWithArea(self, mytool):
    if self.add(mytool):
      self.jetBuilderWithArea = mytool
      self.msg(1, "Jet builder with area added: " + mytool.name())
      return mytool
    else:
      self.msg(0, "Error adding jet builder with area.")
      raise LookupError
  def addJetBuilderWithoutArea(self, mytool):
    if self.add(mytool):
      self.jetBuilderWithoutArea = mytool
      self.msg(1, "Jet builder without area added: " + mytool.name())
      return mytool
    else:
      self.msg(0, "Error adding jet builder without area.")
      raise LookupError

  # Return the list of modifiers associated with a name.
  # If the argument is a list, it is returned directly.
  def getModifiers(self, modifiersin, altname =None):
    if modifiersin == None:
      if altname in ["lctopo","emtopo"]:
        return self.modifiersMap[altname+"_ungroomed"]
      elif "pflow" in altname:
        return self.modifiersMap["pflow_ungroomed"]
      else:
        return self.modifiersMap[altname]
    if type(modifiersin) == str:
        return self.modifiersMap[modifiersin]
        
    return modifiersin

  # Build the list of modifiers, replacing the string "calib:XXX:CALIB" with
  # the appropriate calibration tool.
  def buildModifiers(self, modifiersin, finder, getters, altname, output, calibOpt):
    from GaudiKernel.Proxy.Configurable import ConfigurableAlgTool
    from JetRec.JetRecConf import JetFinder
    outmods = []
    inmods = self.getModifiers(modifiersin, altname)
    ncalib = 0
    for mod in inmods:
      jetlog.info( self.prefix + "Adding modifier " + str(mod) )
      mod0 = ""
      # Split mod = a:b:c... into mod0="a" and modarr = [a, b, c, ...]
      if type(mod) == str:
        if len(mod) == 0: continue
        modarr = mod.split(":")
        mod0 = modarr[0]
      # Fully configured tool.
      if isinstance(mod, ConfigurableAlgTool):
        self.msg(2, "  Adding modifier " + mod.name())
        outmods += [mod]
      # Add jet calibration:
      #   calib:XXX:CALIB - Applies calibration sequence XXX (see JetRecCalibrationFinder)
      #                     using JetCalibrationTool with configuration (or key) CONFIG.
      elif mod0 == "calib":
        ncalib += 1
        alg = finder.JetAlgorithm
        rad = finder.JetRadius
        get = getters[0]
        inp = get.Label
        copt = calibOpt
        if type(calibOpt)==str and len(calibOpt):
          calargs = calibOpt.split(":")
        else:
          calargs = modarr[1:]
        if len(calargs) == 0:
          copt = jetFlags.defaultCalibOpt
          if type(copt)==str and len(copt):
            calargs = copt.split(":")
          else:
            jetlog.info( self.prefix + 'ERROR: If the modifier "calib" is used, then calibOpt or jetFlags.CalibOpt must be a non-blank string.' )
            jetlog.info( self.prefix + 'ERROR: Another alternative is to use the modifier string format "calib:<OPT>", e.g. "calib:a"' )
            raise Exception
        if len(calargs) == 0 or calargs[0]=="":
          jetlog.info( self.prefix + "ERROR: Calibration requested without option." )
          jetlog.info( self.prefix + "       Add calibOpt to tool string, jet build command or to jetFlags.defaultCalibOpt" )
          raise Exception
        seq = calargs[0]
        if seq == "none":
          jetlog.info( self.prefix + "Skipping calibration." )
          continue
        config = ""
        evsprefix = "Kt4"
        if len(calargs) > 1:
          config = calargs[1]
        if len(calargs) > 2:
          evsprefix = calargs[2]
        self.msg(0, "  Adding " + seq + " calibration for " + alg + " R=" + str(rad) + " " + inp)
        self.msg(0, "  Configuration key/file: " + config)
        self.msg(0, "  Event shape prefix: " + evsprefix)
        from JetRec.JetRecCalibrationFinder import jrcf
        calmod = jrcf.find(alg, rad, inp, seq, config, evsprefix)
        jetlog.info( self.prefix + "Adding calib modifier " + str(calmod) )
        outmods += [calmod]
      # truthassoc - Does truth jet association replacing the input name with "Truth"
      elif mod == "truthassoc":
        sinp = getters[0].Label.split("Origin")[0]
        salg = finder.JetAlgorithm
        srad = str(int(10*finder.JetRadius))
        cname = output.replace(sinp, "Truth")
        if cname == output:
            jetlog.info( sinp, cname, output )
            raise TypeError
        # Check that the building of the association tool has been scheduled.
        if not cname in self.jetcons:
          jetlog.info( self.prefix + "Truth association skipped because container is missing: " + cname )
          jetlog.info( self.prefix + "Add to jetcons if input stream is expected to have this." )
        else:
          tname = mod + "_" + salg + srad
          if not tname in self.tools:
            from JetMomentTools.JetMomentToolsConf import JetPtAssociationTool
            self += JetPtAssociationTool(tname, InputContainer=cname, AssociationName="GhostTruth")
          outmods += [self.tools[tname]]
      # trackassoc - Does track jet association replacing the input name with "Track"
      elif mod == "trackassoc":
        sinp = getters[0].Label.split("Origin")[0]
        salg = finder.JetAlgorithm
        srad = str(int(10*finder.JetRadius))
        cname = output.replace(sinp, "PV0Track")
        if cname == output:
            jetlog.info( sinp, cname, output )
            raise TypeError
        # Check that the building of the association tool has been scheduled.
        if not cname in self.jetcons:
          jetlog.info( self.prefix + "Track association skipped because container is missing: " + cname )
          jetlog.info( self.prefix + "Add to jetcons if input stream is expected to have this." )
        else:
          tname = mod + "_" + salg + srad
          if not tname in self.tools:
            from JetMomentTools.JetMomentToolsConf import JetPtAssociationTool
            self += JetPtAssociationTool(tname, InputContainer=cname, AssociationName="GhostTrack")
          outmods += [self.tools[tname]]
      # jetfilter - Filter to remove jets with pT < self.ptminFilter
      elif mod == "jetfilter":
        if self.ptminFilter <= 0:
          jetlog.info( self.prefix + "Jet filter requested without a threshold." )
          raise Exception
        tname = "jetpt" + str(self.ptminFilter)
        if not tname in self.tools:
          from JetRec.JetRecConf import JetFilterTool
          self.add( JetFilterTool(tname, PtMin=self.ptminFilter) ) 
        outmods += [self.tools[tname]]
      # btag - btagging
      elif mod == "btag":
        from BTagging.BTaggingConfiguration import getConfiguration
        ConfInstance = getConfiguration()
        from AthenaCommon.AppMgr import ToolSvc 
        sinp = getters[0].Label
        salg = finder.JetAlgorithm
        srad = str(int(10*finder.JetRadius))
        bspec = salg + srad + sinp
        jetlog.info( self.prefix + "Scheduling btagging for " + bspec )
        btagger = ConfInstance.setupJetBTaggerTool(ToolSvc, bspec)
        jetlog.info( btagger )
        self.add(btagger)
        outmods += [btagger]
      elif mod == "largeR":
        outmods += jtm.modifiersMap["largeR"]
      else:
        raise TypeError
    # Check calibration.
    if calibOpt != "":
      if ncalib==0:
        jetlog.info( self.prefix + "Calibration option (" + calibOpt + ") provided without any calibration modifiers." )
      elif ncalib > 1:
        jetlog.info( self.prefix + "Calibration option (" + calibOpt + ") provided with multiple calibration modifiers." )
        raise Exception

        
    return outmods

  # Create a jet finder without a JetRecToosl.
  #   alg = akgorithm name (Kt, CamKt, AntiKt)
  #   radius = size parameter
  #   rndseed: RandomOption for the finder
  #   gettersin = array of input pseudojet builders (or name in gettersMap)
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  #                 If absent, the gettersin name is used.
  #   consumers = sequence of jet consumers to run after reco
  #   ivtx = None: ordinary jet finding
  #            -1: Separate finding for each vertex
  #           >=0: Finding only for vertex ivtx.
  #   ghostArea: if > 0, then ghosts are found with this inverse density
  #   rndseed: RandomOption for the finder
  #   variableRMinRadius: Min radius for variable-R finding
  #   variableRMassScale: Mass scale for variable-R finding
  #   calibOpt: Calibration option, e.g. "ar". See JetRecCalibrationFinder.py.
  # returns lofinder, hifinder where finder is the lowest-level finder and hifinder
  #         is the highest.
  def addJetFinderTool(self, toolname, alg, radius, ivtx =None,
                       ghostArea =0.0, ptmin =0.0, rndseed =1,
                       variableRMinRadius =-1.0, variableRMassScale =-1.0):
    myname = "JetToolManager:addJetFinderTool: "
    if toolname in self.tools:
      self.msg(0, "Tool " + myname + " is already registered")
      raise LookupError
    self.msg(2, "Adding finder tool")
    if ghostArea == 0.0:
      self.m_jetBuilder = self.jetBuilderWithoutArea
    else:
      self.m_jetBuilder = self.jetBuilderWithArea
    if self.m_jetBuilder == None:
      self.msg(0, "Jet builder must be specified")
      raise Error
    from JetRec.JetRecConf import JetFinder
    areaSuffix= "Area" if ghostArea>0.0 else ""
    finder = JetFinder(toolname);
    finder.JetAlgorithm = alg
    finder.JetRadius = radius
    finder.VariableRMinRadius = variableRMinRadius
    finder.VariableRMassScale = variableRMassScale
    finder.RandomOption = rndseed
    finder.GhostArea = ghostArea
    if ptmin > 0.0:
      finder.PtMin = ptmin
    else:
      finder.PtMin = self.ptminFinder
    finder.JetBuilder = self.m_jetBuilder
    self += finder
    self.finders += [finder]
    hifinder = finder;
    if type(ivtx) is int:
      from JetRec.JetRecConf import JetByVertexFinder
      vfinder = JetByVertexFinder(
        toolname + "ByVertex",
        JetFinder = finder,
        Vertex = ivtx
      )
      self += vfinder
      self.finders += [vfinder]
      hifinder = vfinder;
    return (finder, hifinder)

  # Create a jet finder and rectool.
  #   output = name for output container (and JetRecTool)
  #   alg = akgorithm name (Kt, CamKt, AntiKt)
  #   radius = size parameter
  #   rndseed: RandomOption for the finder
  #   isTrigger: If true, the trigger store is used and the tools is added to
  #              trigjetrecs instead of jetrecs
  #   useTriggerStore: If true, the trigger store is used.
  #   gettersin = array of input pseudojet builders (or name in gettersMap)
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  #                 If absent, the gettersin name is used.
  #   consumers = sequence of jet consumers to run after reco
  #   ivtx = None: ordinary jet finding (unless gettersin = ztrack or pv0track)
  #            -1: Separate finding fore each vertex
  #           >=0: Finding only for vertex ivtx.
  #   ghostArea: if > 0, then ghosts are found with this inverse density
  #   ptminFilter: If > 0, then this is used as the pT threhold for filtering jets.
  #   rndseed: RandomOption for the finder
  #   isTrigger: If true, the trigger store is used and the tools is added to
  #              trigjetrecs instead of jetrecs
  #   useTriggerStore: If true, the trigger store is used (only for testing)
  #   variableRMinRadius: Min radius for variable-R finding
  #   variableRMassScale: Mass scale for variable-R finding
  #   calibOpt: Calibration option, e.g. "ar". See JetRecCalibrationFinder.py.
  def addJetFinder(self, output, alg, radius, gettersin, modifiersin =None,
                   consumers =None, ivtxin =None,
                   ghostArea =0.0, ptmin =0.0, ptminFilter =0.0, rndseed =1,
                   isTrigger =False, useTriggerStore =False,
                   variableRMinRadius =-1.0, variableRMassScale =-1.0,
                   calibOpt ="", jetPseudojetCopier ="",
                   warnIfDuplicate=True,
                   overwrite=False):
    self.msg(2, "Adding finder")
    from JetRec.JetRecConf import JetRecTool
    if type(gettersin) == str:
      getters = self.gettersMap[gettersin]
    else:
      getters = gettersin
    # If jet finding by vertex is not specified, check for special input type names
    ivtx = ivtxin
    if ivtx == None:
      if gettersin == "ztrack": ivtx = -1        # Find tracs separatesly for each vertex
      elif gettersin == "pv0track": ivtx = 0     # Find tracks only for 1st vertex
    # Retrieve/build the jet finder.
    lofinder,hifinder = self.addJetFinderTool(output+"Finder", alg, radius, ivtx, ghostArea, ptmin, rndseed, 
                                            variableRMinRadius, variableRMassScale)
    jetrec = JetRecTool(output)
    jetrec.PseudoJetGetters = getters
    jetrec.JetFinder = hifinder
    jetrec.OutputContainer = output
    ptminSave = self.ptminFilter
    if ptminFilter > 0.0: self.ptminFilter = ptminFilter
    jetrec.JetModifiers = self.buildModifiers(modifiersin, lofinder, getters, gettersin, output, calibOpt)
    if consumers != None:
      jetrec.JetConsumers = consumers
    self.ptminFilter = ptminSave
    jetrec.Trigger = isTrigger or useTriggerStore
    jetrec.Timer = jetFlags.timeJetRecTool()
    jetrec.WarnIfDuplicate = warnIfDuplicate
    jetrec.Overwrite = overwrite

    self += jetrec
    if isTrigger:
      self.trigjetrecs += [jetrec]
    else:
      self.jetrecs += [jetrec]
    self.jetcons += [output]
    return jetrec

  # Create a mass-drop filter and rectool.
  #   output = name for output container (and JetRecTool)
  #   mumax = MuMax used in the filter
  #   ymin = YMin used in the filter
  #   input = name of the input jet container
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  #   doArea = whether to write jet areas (default false because work is needed to 
  #            recover this for reclustered jets).
  def addJetSplitter(self, output, mumax, ymin, input, modifiersin ="groomed",
                     isTrigger =False, useTriggerStore =False, doArea =True):
    from JetRec.JetRecConf import JetSplitter
    from JetRec.JetRecConf import JetRecTool
    groomer = JetSplitter(output + "Groomer")
    groomer.MuMax = mumax
    groomer.YMin = ymin
    groomer.BDRS = False
    groomer.NSubjetMax = 3
    if doArea:
      groomer.JetBuilder = self.jetBuilderWithArea
    else:
      groomer.JetBuilder = self.jetBuilderWithoutArea
    self += groomer
    jetrec = JetRecTool(output)
    jetrec.JetGroomer = groomer
    jetrec.InputContainer = input
    jetrec.OutputContainer = output
    jetrec.JetModifiers = self.getModifiers(modifiersin)
    jetrec.Trigger = isTrigger or useTriggerStore
    jetrec.Timer = jetFlags.timeJetRecTool()
    self += jetrec
    if isTrigger:
      self.trigjetrecs += [jetrec]
    else:
      self.jetrecs += [jetrec]
    self.jetcons += [output]
    return jetrec

  # Create a Trimmer and rectool.
  #   output = name for output container (and JetRecTool)
  #   rclus = RClus used in the trimming
  #   ptfrac = PtFrac used in the trimming
  #   input = name of the input jet container
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  def addJetTrimmer(self, output, rclus, ptfrac, input, modifiersin ="groomed",
                    pseudojetRetriever ="jpjretriever",
                    isTrigger =False, useTriggerStore =False, doArea =True):
    from JetRec.JetRecConf import JetTrimmer
    from JetRec.JetRecConf import JetRecTool
    groomer = JetTrimmer(output + "Groomer")
    groomer.RClus = rclus
    groomer.PtFrac = ptfrac
    if doArea:
      groomer.JetBuilder = self.jetBuilderWithArea
    else:
      groomer.JetBuilder = self.jetBuilderWithoutArea
    self += groomer
    jetrec = JetRecTool(output)
    jetrec.JetGroomer = groomer
    jetrec.InputContainer = input
    jetrec.OutputContainer = output
    jetrec.JetModifiers = self.getModifiers(modifiersin)
    jetrec.Trigger = isTrigger or useTriggerStore
    jetrec.Timer = jetFlags.timeJetRecTool()
    if pseudojetRetriever in self.tools:
      jetrec.JetPseudojetRetriever = self.tools[pseudojetRetriever]
    else:
      jetlog.info( "Requested jet pseudojet retriever is not a registered tool: " \
            + pseudojetRetriever )
      raise KeyError
    self += jetrec
    if isTrigger:
      self.trigjetrecs += [jetrec]
    else:
      self.jetrecs += [jetrec]
    self.jetcons += [output]
    return jetrec

  # Create a Pruner and rectool.
  #   output = name for output container (and JetRecTool)
  #   rcut = RCut used in the pruning
  #   zcut = ZCut used in the pruning
  #   input = name of the input jet container
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  def addJetPruner(self, output, rcut, zcut, input, modifiersin ="groomed",
                   isTrigger =False, useTriggerStore =False, doArea =True):
    from JetRec.JetRecConf import JetPruner
    from JetRec.JetRecConf import JetRecTool
    groomer = JetPruner(output + "Groomer")
    groomer.RCut = rcut
    groomer.ZCut = zcut
    if doArea:
      groomer.JetBuilder = self.jetBuilderWithArea
    else:
      groomer.JetBuilder = self.jetBuilderWithoutArea
    self += groomer
    jetrec = JetRecTool(output)
    jetrec.JetGroomer = groomer
    jetrec.InputContainer = input
    jetrec.OutputContainer = output
    jetrec.JetModifiers = self.getModifiers(modifiersin)
    jetrec.Trigger = isTrigger or useTriggerStore
    jetrec.Timer = jetFlags.timeJetRecTool()
    self += jetrec
    if isTrigger:
      self.trigjetrecs += [jetrec]
    else:
      self.jetrecs += [jetrec]
    self.jetcons += [output]
    return jetrec

  # Create a jet reclusterer and rectool.
  #   output = name for output container (and JetRecTool)
  #   alg = akgorithm name (Kt, CamKt, AntiKt)
  #   radius = size parameter
  #   input = name for the input jet collection
  #   rndseed: RandomOption for the finder
  #   isTrigger: If true, the trigger store is used and the tools is added to
  #              trigjetrecs instead of jetrecs
  #   useTriggerStore: If true, the trigger store is used.
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  #                 If absent, the gettersin name is used.
  #   consumers = sequence of jet consumers to run after reco
  #   ivtx = None: ordinary jet finding (unless gettersin = ztrack or pv0track)
  #            -1: Separate finding fore each vertex
  #           >=0: Finding only for vertex ivtx.
  #   ghostArea: if > 0, then ghosts are found with this inverse density
  #   ptminFilter: If > 0, then this is used as the pT threhold for filtering jets.
  #   rndseed: RandomOption for the finder
  #   isTrigger: If true, the trigger store is used and the tools is added to
  #              trigjetrecs instead of jetrecs
  #   useTriggerStore: If true, the trigger store is used (only for testing)
  #   variableRMinRadius: Min radius for variable-R finding
  #   variableRMassScale: Mass scale for variable-R finding
  #   calibOpt: Calibration option, e.g. "ar". See JetRecCalibrationFinder.py.
  def addJetReclusterer(self, output, alg, radius, input, modifiersin =None,
                        consumers =None, ivtx =None,
                        ghostArea =0.0, ptmin =0.0, ptminFilter =0.0, rndseed =1,
                        isTrigger =False, useTriggerStore =False,
                        variableRMinRadius =-1.0, variableRMassScale =-1.0,
                        calibOpt ="", jetPseudojetCopier =""):
    self.msg(2, "Adding reclusterer")
    from JetRec.JetRecConf import JetRecTool
    from JetRec.JetRecConf import JetReclusterer
    # Retrieve/build the jet finder.
    lofinder,hifinder = self.addJetFinderTool(output+"Finder", alg, radius, ivtx, ghostArea, ptmin, rndseed, 
                                              variableRMinRadius, variableRMassScale)
    reclname = output + "Reclusterer"
    groomer = JetReclusterer(
      reclname,
      JetConstituentsRetriever = self.tools["jconretriever"],
      JetFinder = hifinder
    )
    self += groomer
    jetrec = JetRecTool(output)
    jetrec.InputContainer = input
    jetrec.OutputContainer = output
    jetrec.JetGroomer = groomer
    jetrec.JetModifiers = self.getModifiers(modifiersin)
    if consumers != None:
      jetrec.JetConsumers = consumers
    jetrec.Trigger = isTrigger or useTriggerStore
    jetrec.Timer = jetFlags.timeJetRecTool()
    self += jetrec
    if isTrigger:
      self.trigjetrecs += [jetrec]
    else:
      self.jetrecs += [jetrec]
    self.jetcons += [output]
    return jetrec

  # Create a copying rectool.
  #   output = name for output container (and JetRecTool)
  #   output = name for input container
  #   modifiersin = list of modifier tools (or name of such in modifiersMap)
  def addJetCopier(self, output, input, modifiersin, ptminFilter =0.0, radius =0.0, alg ="", inp ="",
                   isTrigger=False, useTriggerStore=False, calibOpt ="", shallow =True):
    from JetRec.JetRecConf import JetRecTool
    jetrec = JetRecTool(output)
    jetrec.InputContainer = input
    jetrec.OutputContainer = output
    ptminSave = self.ptminFilter
    if ptminFilter > 0.0: self.ptminFilter = ptminFilter
    class finder:
      JetRadius = radius
      JetAlgorithm = alg
    class get:
      Label = inp
    getters = [get]
    jetrec.JetModifiers = self.buildModifiers(modifiersin, finder, getters, None, output, calibOpt)
    self.ptminFilter = ptminSave
    jetrec.Trigger = isTrigger or useTriggerStore
    jetrec.Timer = jetFlags.timeJetRecTool()
    jetrec.ShallowCopy = shallow
    self += jetrec
    self.jetrecs += [jetrec]
    self.jetcons += [output]
    return jetrec

  # Ad op[] to return a tool.
  def __getitem__(self, toolname):
    return self.tools[toolname]

  # Change the output level of a tool.
  def setOutputLevel(self, toolid, level):
    from GaudiKernel.Proxy.Configurable import ConfigurableAlgTool
    if type(toolid) == str:
      tool = self[toolid]
    else:
      tool = toolid
    locked = tool.isLocked()
    oldlevel = "undefined"
    if locked:
      dbglvl = 0
      slock = "locked"
      tool.unlock()
    else:
      dbglvl = 1
      slock = "unlocked"
    aname = "OutputLevel"
    if hasattr(tool, aname):
      oldlevel = getattr(tool, aname)
    setattr(tool, aname, level)
    if locked:
      tool.lock()
    self.msg(0, "Changed output level of " + slock + " tool " + tool.name() + \
             " from " + str(oldlevel) + " to " + str(level) + ".")

