package atlantis;

import java.awt.Toolkit;
import java.io.File;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Node;
import org.w3c.dom.Document;

import atlantis.canvas.ACanvas;
import atlantis.config.ADefaultValues;
import atlantis.config.AConfigUpdater;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;

import atlantis.geometry.AAtlasDetector;
import atlantis.geometry.ADetectorSystem;
import atlantis.globals.AGlobals;
import atlantis.graphics.colormap.AColorMap;
import atlantis.gui.ADemoDialog;
import atlantis.gui.AEventQueue;
import atlantis.gui.AGUI;
import atlantis.gui.AStartupWindow;
import atlantis.gui.AEventLoopDialog;
import atlantis.output.AExceptionHandler;
import atlantis.parameters.APar;
import atlantis.projection.AProjectionsManager;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AUtilities;
import atlantis.utils.xml.AXMLErrorHandler;
import atlantis.utils.xml.AXMLEntityResolver;
import atlantis.utils.xml.AXMLUtils;
import atlantis.event.AImageProducer;
import atlantis.event.AEventSource;
import atlantis.utils.ALogger;
import atlantis.event.xmlrpc.AServerXMLRPC;
import atlantis.event.xmlrpc.AServerXMLRPCEventSource;
import atlantis.gui.ACrashReporter;
import atlantis.gui.ADemoLoop;
import atlantis.interactions.AInteractionsConfigReader;
import atlantis.utils.ANewIdHelper;
import atlantis.utils.xml.AXMLEntityResolver;

import java.util.Arrays;
import java.util.Vector;
import javax.swing.JOptionPane;


/**
 * Main class of Atlantis application.
 * Initialises the application's main components and provides access to them.
 */
public class Atlantis
{    
    // The main class logger gets initialized after command line options
    // are processed. All other classes will just do
    // logger = ALogger.getLogger(Class.class) in their attribute definitions
    private static ALogger logger = null;
    
    /** Store for Atlantis global state. */
    private static AGlobals globals = AGlobals.instance();
 
    // expands into SVN tag
    private static String versionAtlantisJava = "$HeadURL: file:///atlas/scratch0/graemes/ao-mirror/graphics/AtlantisJava/tags/AtlantisJava-09-16-05-18/src/atlantis/Atlantis.java $";

    private static final String FILE_SEPAR = System.getProperty("file.separator");
    private static final String USER_HOME_DIR = System.getProperty("user.home");

    // This is the URL that is used as default for "Read events from URL"
    // The stream is set to "Default" as this is the only one guaranteed to be there.
    private static final String LIVE_POINT_1_EVENTS_URL =
        "http://atlas-live.cern.ch/event_files/Default";
    
    // expected file name of the user customised configuration file
    // (stored in the user's home directory)
    private static final String USER_CONFIG_FILE_PATH =
        USER_HOME_DIR + FILE_SEPAR + ".Atlantis-config.xml";
    
    // configuration file from the Atlantis distribution (default config)
    private static final String DISTRIB_DEFAULT_CONFIG_FILE = 
        "configuration" + FILE_SEPAR + "config.xml";
    
    // this is checksum file of the distribution's main configuration file
    // if it doesn't match, Atlantis tries to update user's configuration
    // or the --config <configfile> configuration 
    // it is an indicator that the master config.xml changed
    private static final String CONFIG_CHECKSUM_FILE_PATH =
        USER_HOME_DIR + FILE_SEPAR + ".Atlantis-config.chk";

    // limit the total width of Atlantis to this number of pixels (0 = screen width)
    private static int MAX_WIDTH = 0;

    /**
     * The list of parameters that can be set from the command line
     */
    // force Atlantis to update the personal configuration file
    protected static boolean updateConfig = false;
    // configuration file specified as command line argument
    protected static String configCommandLine = null;
    // colormap file specified as command line argument
    protected static String colormapCommandLine = null;
    // selected projections specified as command line argument for demo mode
    protected static String selectedProjections = null;
    // timer delay from command line for reading events in a loop
    protected static Integer loopInterval = null;
    // navigation mode from command line
    protected static AEventSource.NavigationMode initialMode = null;
    // initial event source, may be specified from command line
    protected static String initialEventSource = "events"+FILE_SEPAR+"test_events.zip";
    // Object for saving event images in a directory
    protected static AImageProducer imageProducer = null;
    // Port number for image server from command line
    protected static int imageServerPort = -1;
    
    private static final boolean atlantisWithOpenGL = false;

    private static final String INIT_ERROR_MSG = 
        	"Error during Atlantis initialization.\n" +
        	"(configuration file related issues might be\n" +
        	"corrected by running with -x (updating customised configuration)).\n" +
        	"See console output for more information.";
    
    // main Atlantis components
    private static APar parameterStore = null;
    private static AEventManager eventManager = null;
    private static ADetectorSystem detector = null;
    private static ACanvas canvas = null;
    private static AGUI gui = null;

    /**
     * Returns the delay for timer for automatic event reading from server
     * when this option was specified as command line argument 
     * @return the delay in seconds
     */
    private static Integer getLoopInterval()
    {
    	return loopInterval;
    }
 
    
    /**
     * Set the default java look and feel ("metal")
     */
    private static void setLookAndFeel()
    {
      try {
         UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
      } catch(Exception e) {
          AExceptionHandler.processException("Error setting default Look and Feel", e);
      }
    }
     

    /**
     * Check if the checksum file CONFIG_CHECKSUM_FILE_PATH is up-to-date.
     * Create one if it does not exist or update it if it is not up-to-date.
     */
    private static void checkAndUpdateConfigCheckSumFile(String distribFile)
    throws AAtlantisException
    {
        if(Atlantis.updateConfig)
        {
            logger.info("Checksum not checked, run with -x, configuration update " +
            		    "forced anyway.");
            return;
        }

        logger.info("Checking if the used configuration is up-to-date ...");
        
        logger.debug("Checking checksum of the configuration file from the " +
                     "Atlantis distribution: " + distribFile);
        logger.debug("Checksum file: " + CONFIG_CHECKSUM_FILE_PATH);
        
        boolean checkSumExists = new File(CONFIG_CHECKSUM_FILE_PATH).exists();
        
        if(checkSumExists)
        {
            boolean comparison = AConfigUpdater.compareChecksums(distribFile,
                                                CONFIG_CHECKSUM_FILE_PATH);
            if(comparison)
            {
                logger.debug("Checksum file - checksum match, no update necessary.");
                Atlantis.updateConfig = false;
                return;
            }
        }

        // doesn't match, force update of custom configuration
        // and re-generate checksum
        AConfigUpdater.createChecksumFile(distribFile, CONFIG_CHECKSUM_FILE_PATH);
        logger.debug("Checksum didn't match, updating customised " +
                     "configuration forced.");
        Atlantis.updateConfig = true;

    } // checkAndUpdateConfigCheckSumFile() ---------------------------------
    
    
    
    /**
     * Returns InputStream to read XML application configuration from.
     * 
     * Evaluates sources from which Atlantis XML configuration is
     * possibly loaded from (prioritised):
     * 1) -c, --config <configfile.xml> specified on the command line
     *    here available in configCommandLine variable
     * 2) if USER_CONFIG_FILE_PATH exists, read that one
     * 3) or eventually read <ATLANTIS_DIRECTORY>/DISTRIB_DEFAULT_CONFIG_FILE
     * 
     * User's customised (in the home directory) or --config <configfile> are
     * also tried to be updated when the checksum configuration file doesn't
     * match. Atlantis calculates checksum of the main distribution
     * configuration file and stores that checksum in CONFIG_CHECKSUM_FILE_PATH.
     * If this checksum doesn't match when Atlantis is run on the user's
     * home directory configuration or --config <configfile> configuration,
     * automatic configuration file update process is executed.
     * 
     * Updating means that config.xml (main, base) contains some changes
     * which are not consistent with the user's config or custom config
     * and they need to be propagated there.
     * Update process:
     * AConfigUpdater.updateConfigurationFile(distribFilePath, configToUse)
     * is then executed.
     * 
     * Do not try to update custom configuration which is located in the same
     * directory as the base config file - it means that such custom config
     * comes with AtlantisJava distribution - no need updating this one.
     */
    private static InputStream getXMLConfigAsStream() throws Exception
    {
        String configToUse = null;
        
        logger.info("Reading the XML configuration ...");
        
        // configuration file from the Atlantis distribution
        String distribFilePath = getHomeDirectory() + DISTRIB_DEFAULT_CONFIG_FILE;
        
        // distribution config file checksum only taken into account when having
        // --config command line argument or when running from user's (home
        // directory) configuration
        
        // option / source 1) - command line argument - highest priority
        if(configCommandLine != null)
        {
            logger.info("Using configuration file specified on the command " +
                        "line: " + configCommandLine);
            // sets Atlantis.updateConfig = true; accordingly
            checkAndUpdateConfigCheckSumFile(distribFilePath);
            configToUse = configCommandLine;
        }
        else
        {
            // check if we can read the user configuration file (in home dir.)
            // option / source 2)
            if(new File(USER_CONFIG_FILE_PATH).exists())
            {
                logger.info("Using user's configuration file: " +
                             USER_CONFIG_FILE_PATH);
                // sets Atlantis.updateConfig = true; accordingly
                checkAndUpdateConfigCheckSumFile(distribFilePath);
                configToUse = USER_CONFIG_FILE_PATH;
            }
            else
            {
                // // option / source 3)
                logger.info("Using distribution configuration file: " +
                             distribFilePath);
                if(Atlantis.updateConfig)
                {
                    logger.warn("Update custom configuration flag is set, " +
                                "that doesn't make sence - no custom config " +
                                "file available, ignored.");
                    Atlantis.updateConfig = false;
                }
                configToUse = distribFilePath;
            }
        }

        
        try
        {
            // try to update custom configuration before loading it
            if(Atlantis.updateConfig)
            {
                logger.info("Updating customised configuration file ...");
                // check if configToUse comes from atlantis home directory, if
                // so, then configToUse is from AtlantisJava distribution, should
                // already be up-to-date and no automatic update will be performed
                String atlantisHome = Atlantis.getHomeDirectory();
                String customConfigDir = new File(configToUse).getAbsolutePath();
                
                logger.info("Updating configuration - customised config directory: " +
                             customConfigDir);
                // if configToUse comes from AtlantisJava distribution, customConfigDir
                // path start with atlantisHome path plus 'configuration' directory
                // (does not filter off webstart since .getHomeDirectory() 
                // returns user's home directory when running via webstart,
                // the condition below should be improved to cover webstart situation
                // as well ...)
                if(customConfigDir.startsWith(atlantisHome))
                {
                    logger.debug("Updating configuration - Atlantis home: " + atlantisHome);
                    logger.info("Updating configuration - customised config directory " +
                                "is in the Atlantis home, no need updating it.");
                }
                else
                {
                    AConfigUpdater.updateConfigurationFile(distribFilePath, 
                                                           configToUse);
                }
            }
        }
        catch(AAtlantisException aae)
        {
            logger.error(aae.getMessage());
        }

        
        InputStream isConfig = null; // configuration file input stream
        try 
        {
            isConfig = AUtilities.getFileAsStream(configToUse); 
        }
        catch(AAtlantisException aae)
        {
            String m = "Could not read " + configToUse + " reason: " +
                       aae.getMessage();
            throw new Exception(m);
        }
        
        logger.info("Going to parse XML configuration: " + configToUse);
        return isConfig;
              
    } // getXMLConfigAsStream() ---------------------------------------------

        
    
    private static void initAtlantis() {
    	
    String homeDirectory = Atlantis.getHomeDirectory();
    globals.setHomeDirectory(homeDirectory);

    //Create startup window and set how often
    //the progress bar will get update below
    AStartupWindow startupWin = new AStartupWindow(9);
    
    try {
      System.setProperty("sun.awt.exception.handler",
              "AExceptionHandler");

      startupWin.updateText("Atlantis initialisation");
      
      //Show atlantis home
      logger.info("Atlantis home: "+homeDirectory);

      //Create an event manager
      eventManager = AEventManager.instance();
      
      // reading Atlantis runtime configuration (text file in the home directory)
      logger.info("Reading Atlantis runtime configuration, file: " +
                  ADefaultValues.CONFIGFILE);
      ADefaultValues.readRuntimeValues();
      
      
      // read the configuration xml files -------------------------
      startupWin.updateText("Reading Atlantis configuration");
      
      // evaluate configuration sources options and get the configuration
      InputStream isConfig = getXMLConfigAsStream();
      
      // prepare XML parser for parsing the XML configuration
      DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
      factory.setValidating(true);
      DocumentBuilder parser = factory.newDocumentBuilder();
      parser.setErrorHandler(new AXMLErrorHandler());
      parser.setEntityResolver(new AXMLEntityResolver());

      // parse the XML configuration
      String configBase = homeDirectory + FILE_SEPAR + "configuration" + FILE_SEPAR;
      Document xmlConfigDoc = parser.parse(isConfig, configBase);
      Node rootNode = xmlConfigDoc.getDocumentElement();
      // read the XML initialization node, and performs initialization
      Node initializationNode = AXMLUtils.getChild(rootNode, "Initialization");

      // parse the ID dictionary
      String geometryBase = homeDirectory + FILE_SEPAR + "geometry" + FILE_SEPAR;
      InputStream isIdDict = AUtilities.getFileAsStream(geometryBase+"ATLAS_IDS.xml");
      Document xmlDictDoc = parser.parse(isIdDict, geometryBase);
      ANewIdHelper.construct(xmlDictDoc.getDocumentElement());
      
      
      // read colour map (colormap.xml) configuration -----------------
      startupWin.updateText( "Reading colour map");
      AColorMap.readColorMap(Atlantis.colormapCommandLine);
      
      
      // creating canvas ----------------------------------------------
      startupWin.updateText("Building canvas and projections");
      Node canvasNode = AXMLUtils.getChild(initializationNode, "Canvas");
      canvas = ACanvas.construct(canvasNode);

      // reading parameters ------------------------------------------
      logger.info("Building parameters");
      parameterStore = APar.construct(AXMLUtils.getChild(rootNode, "Parameters"));
      parameterStore.update(AXMLUtils.getChild(rootNode, "ParameterDifferences"));
      eventManager.addNewEventListener(parameterStore);
      // creating the Projections Manager (reading the projections)
      Node interactionsNode =
              AXMLUtils.getChild(initializationNode, "Interactions");
      AInteractionsConfigReader.readInteractions(interactionsNode);
      AProjectionsManager.initialise();
      
      //set the output flag: 0 normal 1 eta/phi/pt 2 px/py/pz 3 eta/phi/pt/px/py/pz
      if(parameterStore.get("Minerva","alternativesimpleoutput").getStatus()){
    	  if(parameterStore.get("Minerva","simpleoutput").getStatus()){
    		  globals.setSimpleOutput(3);
    	  }else{
    		  globals.setSimpleOutput(2);
    	  }
      }else if(parameterStore.get("Minerva","simpleoutput").getStatus()){
    	  globals.setSimpleOutput(1);
      }

      // reading  the geometry ----------------------------------------
      startupWin.updateText("Reading geometry");
      String geomPrefix = Atlantis.getHomeDirectory() + "geometry" +
              Atlantis.FILE_SEPAR;
      String geomName = ADefaultValues.get("GeometryName");
      if (!"".equals(geomName)) {
        // name of the geometry considered after underscore
        geomName = "_" + geomName;

        // do the geometry files exist - just try loading the files
        String geomNameFull1 = geomPrefix + "AGeometry" + geomName + ".xml";
        String geomNameFull2 = geomPrefix + "AMuonGeometry" + geomName + ".xml";
        try {
          // just accessibility test
          @SuppressWarnings("unused")
		  InputStream i = AUtilities.getFileAsStream(geomNameFull1);
          i = AUtilities.getFileAsStream(geomNameFull2);
          i = null;
        } catch (AAtlantisException ae) {
          geomName = ""; // will fall back to default geometry
        }
      }
      String geomIdCalo = geomPrefix + "AGeometry" + geomName + ".xml";
      String geomMuon = geomPrefix + "AMuonGeometry" + geomName + ".xml";
      detector = new AAtlasDetector(geomIdCalo, geomMuon);
      AAtlasDetector.setDetector(detector);
      
      canvas.finalizeConstruction();
      canvas.readCorners(AXMLUtils.getChild(rootNode, "WindowCorners"));
      // This is needed here, design should be changed to remove this problem
      canvas.setCurrentWindow(canvas.getCurrentWindowName());
     
      //Add the canvas as a NewEvent event listener
      eventManager.addNewEventListener(canvas);

      // reading the first event --------------------------------------
      startupWin.updateText("Reading initial event");
      getInitialEvent();
     
      // displaying GUI, Canvas ---------------------------------------
      startupWin.updateText("Displaying GUI and Canvas");

      // update colors to those selected in the config file
      AColorMap.setColorMap(parameterStore.get("Prefs", "ColorMap").getI());
      // NB: must calculate the size of GUI after calling
      // canvas.setVisible(true), in which the real final size of Canvas
      // will be calculated
      canvas.setCanvasVisible(true);

      // Now the canvas is on the screen and have a size, check if we need
      // to rework the aspect ratio on YX projections at all as is the case
      // when going to control room projectors for example - AD
      canvas.correctAspectRatios();
      
      //Create the GUI
      gui = AGUI.getGUI();
      
      //If we got one, show it (e.g. not in headless mode)
      if (gui != null){
    	gui.setMaxWidth(MAX_WIDTH);
        gui.setName("Atlantis GUI"); // Name is used by JUnit/FEST tests
        // Tell the parameter store about the GUI so it can inform the GUI of parameter changes
        parameterStore.addParameterChangeListener(gui);
        //Display it
        gui.setVisible(true);
        // and register as a layout change listener
        canvas.addOverlay();
        canvas.addLayoutChangeListener(gui);
      }

      //Before starting any loops, inform user we are ready to go
      startupWin.updateText("Atlantis Ready");

      // Set the navigation mode if given on the command line.
      // Use SwingUtilities.invokeLater() to "defer until all pending events
      // have been processed", avoiding the deadlock that otherwise occurs.
      if (Atlantis.initialMode != null){
    	  SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
		    	  try {
					eventManager.setNavigationMode(initialMode);
				} catch (InvalidEventSourceException e) {
                    logger.error("Invalid event source", e);
				}
			}
    	  });
      }

      //Set up event loop if requested
      if ((Atlantis.loopInterval != null) 
       && (Atlantis.selectedProjections == null )
       && (!AGlobals.isAtlantisHeadless()) ) {
        // A loop interval has been given, but no projections -
        // so create an event loop dialog, but not in headless mode
        AEventLoopDialog dialog =  AEventLoopDialog.getInstance();

        //set all properties
        dialog.setUpdateInterval(getLoopInterval());
        dialog.showDialog();
        dialog.startEventLoop();
      }

      //setup demo loop if requested
      if (Atlantis.selectedProjections != null) {
        //If user just gave -j, use default settings
        if (Atlantis.selectedProjections.equals(""))
          ADemoDialog.getInstance().setDefaultSequence();
        //User may also give the update interval
        if (getLoopInterval() != null)
          ADemoDialog.getInstance().setTimerInterval(getLoopInterval());
        ADemoDialog.getInstance().setVisible(true);
        ADemoDialog.getInstance().startDemo();
      }
 
      //Add an image server if requested on the command line
      if (imageServerPort >= 0) {
        AServerXMLRPC serv = new AServerXMLRPC(imageServerPort, AServerXMLRPCEventSource.class);
        serv.start();
      }
      
      //If we have a image saver, add it now as new event listener
      if (imageProducer != null){
    	eventManager.addNewEventListener(imageProducer);
        //if we are also running in headless mode, add an automatic event loop
        if (AGlobals.isAtlantisHeadless()){
          //Check whether the event source is a file or an ONCRPC server
          //For a server, start the loop in a "try hard" mode, i.e only stop on ReadEventException
          boolean stopOnFailure = !(eventManager.getEventSource() instanceof atlantis.event.oncrpc.AONCRPCEventSource );
          //If a loop interval is given on the command line, use it,
          //otherwise, run in "as fast as can mode"
          int sleepTime = (Atlantis.loopInterval != null) ? Atlantis.loopInterval*1000 : 0;
          //Show warning in case of "as fast as we can mode"
          if (sleepTime == 0) logger.info("No loop intervall given - processing events as fast as possible");
          //Now create the loop
          ADemoLoop loop = new ADemoLoop(sleepTime,new Vector<String>(Arrays.asList("NE")),stopOnFailure);
          //And start it right away
          loop.startDemoLoop();
          //also wait for it to finish before the program finishes
          synchronized (loop) { loop.wait(); }
        }
      }

    } catch (Exception e) {
      logger.fatal(INIT_ERROR_MSG);
      logger.fatal(e.getMessage(), e);
      AExceptionHandler.processException(INIT_ERROR_MSG, e);
    } finally {
      if (startupWin != null) {
        startupWin.dispose();
      }
    }

  } // initAtlantis() -----------------------------------------------------


    protected static String getHomeDirectory()
    {
        // where was this class loaded from
        String temp = Atlantis.class.getResource("Atlantis.class").getFile();

        // remove "file://" or "file:"
        if (temp.startsWith("file://")) {
            temp = temp.substring(7);
        } else if (temp.startsWith("file:")) {
            temp = temp.substring(5);
        }

        if(temp.indexOf("!") > -1)
        {
            // atlantis is in a .jar file
            temp = temp.substring(0, temp.indexOf("!"));
            temp = temp.replaceAll("%20", " ");
            return temp.substring(0, temp.lastIndexOf('/') + 1);
        }
        // must be the current working directory of the user
        return System.getProperty("user.dir") + FILE_SEPAR;

    } // getHomeDirectory() -------------------------------------------------

    /**
     * Process the options passed on the command line
     * @param args the command line arguments
     */
    private static void processCommandLineParameters(String args[])
    {

      //echo the given command line options
      System.out.println("Command line arguments:");
      for(String arg : args) System.out.print(arg + " ");
      System.out.println("\n");

      //Then go and process the command line
      (new ACommandLine()).process(args);
      
    } // processCommandLineParameters() -------------------------------------


    /**
     * Try to read the initial event
     *  - from the given event source
     *  - from the default event
     */
    private static void getInitialEvent()
    {
        //Get the file where the default event is stored
        //if not a special source
        String eventPath = Atlantis.initialEventSource;
        
        // if this is not a special source then consider it
        // a) to be a file with relative or absolute path
        // b) to be a file relative to the Atlantis directory
        if (! eventPath.contains("://") ) {
          //This should be a file, see if it exists
          if(! new File(eventPath).exists()) {
            //try prepending atlantis home directory
            logger.info("File "+eventPath+" does not exist - trying in "+Atlantis.getHomeDirectory());
            eventPath = Atlantis.getHomeDirectory()+eventPath;
          }

          //All non specified sources are assumed to be files
          eventPath = "file://" + eventPath;
        }
      
        //be verbose
        logger.info("Reading the default event from " + eventPath);
        //set the event source and read an event
        try {
          eventManager.setEventSource(eventPath);
          eventManager.nextEvent();
        } catch (InvalidEventSourceException ies){
          //Write error message
          String msg = "Can not access "+eventPath+": "+ies.getMessage();
          logger.error(msg);
          //Show error dialog
          if (AGlobals.isAtlantisHeadless()) return;
          JOptionPane.showMessageDialog(ACanvas.getCanvas(), msg,
                "Invalid Event Source",JOptionPane.ERROR_MESSAGE);
        } catch (NoMoreEventsException nme){
          String msg = "No events in "+eventPath+": "+nme.getMessage();
          logger.error(msg);
          //Show error dialog
          if (AGlobals.isAtlantisHeadless()) return;
          JOptionPane.showMessageDialog(ACanvas.getCanvas(), msg,
                "No Events found",JOptionPane.ERROR_MESSAGE);
        } catch (ReadEventException rex){
          String msg = "Can not read events from "+eventPath+": "+rex.getMessage();
          logger.error(msg);
          //Show error dialog
          if (AGlobals.isAtlantisHeadless()) return;
          JOptionPane.showMessageDialog(ACanvas.getCanvas(), msg,
                "Can not read event",JOptionPane.ERROR_MESSAGE);
        }
    }

    /**
     * Get the SVN tag version from the HeadURL, which is automatically
     * substituted by SVN.
     */
    private static void setSVNTagInfo()
    {
        // SVN expands into $HeadURL: file:///atlas/scratch0/graemes/ao-mirror/graphics/AtlantisJava/tags/AtlantisJava-09-16-05-18/src/atlantis/Atlantis.java $,
        // we only want the tag name
        String s = Atlantis.versionAtlantisJava;
        // 0) Check we are in a tagged version
        if (!s.contains("tags")){
          Atlantis.versionAtlantisJava = "AtlantisJava (SVN tag n/a)";
          return;
        }
        // 1) Replace anything after src
        s=s.substring(0,s.indexOf("/src"));
        // 2) Next find the "tags" subdir and cut after that
        s=s.substring(s.indexOf("tags/")+5);
        //this should be the version
        Atlantis.versionAtlantisJava = s;
    } 

    public static void main(String[] args)
    {

        /**
         * First thing always: say hello with a version number
         */
        //Retrieve the SVN version tag number
        setSVNTagInfo();
        globals.setVersion(versionAtlantisJava);
        globals.setLivePoint1EventsURL(LIVE_POINT_1_EVENTS_URL);
        globals.setUserConfigFile(USER_CONFIG_FILE_PATH);
        globals.setUseOpenGL(atlantisWithOpenGL);
        System.out.println("AtlantisJava version (SVN tag): " + Atlantis.versionAtlantisJava);

        // Any uncaught exception should be reported to the developers
        Thread.setDefaultUncaughtExceptionHandler(new ACrashReporter());

        /**
         * Now run all the checks for system properties
         */

        //Now set the default look and feel
        setLookAndFeel();

        // Disable rename file/make new directory from the file chooser
        UIManager.put("FileChooser.readOnly", Boolean.TRUE);

        /**
         * Process the command line parameters
         */
        //process args
        processCommandLineParameters(args);

        //Logger can be created only after cmdl opts to catch settings
        logger = ALogger.getLogger(Atlantis.class);

        //Do some OGL checks
        if (Atlantis.atlantisWithOpenGL) {
          logger.info("Testing for OpenGL");
          logger.info(AOpenGLTools.isOpenGLAvailable() ? "Open GL Available" : "Open GL Unavailable");
        }

        /**
         * Now go and create the manifold Atlantis classes and singletons.
         * Has to run on main thread so display is updated during initialization.
         */
        initAtlantis();
        // Replace event queue by AEventQueue, but only after initialization.
        try {
			SwingUtilities.invokeAndWait(new Runnable() {
				@Override
				public void run() {
					logger.debug("Waiting for AWT event queue to be empty.");
				}
			});
		} catch (InterruptedException e) {
			logger.error("Interrupted while waiting for event queue.", e);
		} catch (InvocationTargetException e) {
			logger.error("Error while waiting for event queue.",e);
		}
        Toolkit.getDefaultToolkit().getSystemEventQueue().push(new AEventQueue());
    }
}
