package atlantis.utils;

import java.io.IOException;
import java.io.File;
import java.util.Properties;

import org.apache.log4j.Logger;
import org.apache.log4j.Level;
import org.apache.log4j.PatternLayout;
import org.apache.log4j.PropertyConfigurator;
import org.apache.log4j.FileAppender;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Layout;



/**
 * The class is a common wrapper for logging facility, uses
 * apache-logging library log4j (more complex and powerful than Java
 * native java.util.logging (e.g. flushing is automated, allows customised
 * message logs layout, different levels for different sources, etc).
 * Because it is subclassing from Logger, it needs LoggerFactory mechanism
 * to be in place. Reference to it is given via Properties, if more
 * complex properties are to be defined, it will be put in a file (such
 * properties file must be accessed generically so that it is load in
 * correctly no matter whether Atlantis is run locally (and have distribution
 * directories available) or from a .jar file (webstart execution).
 *
 * @author Zdenek Maxa
 */
public final class ALogger extends Logger
{		
	// the class is a singleton, reference to itself
	private static Logger root = null;
	
	// it's enough to instantiate a factory once and for all
	private static ALoggerFactory loggerFactory = new ALoggerFactory();
	
	// logging layouts definition (format characters explained at PatternLayout class)
	private static final String CONSOLE_LOGGING_LAYOUT =
		"%-5p %d{yyyy-MM-dd HH:mm:ss,SSS}  %m%n";
	// %c - cathegory (must be specified at .getLogger(Class.class))
	private static final String FILE_LOGGING_LAYOUT =
		"%-5p %d{yyyy-MM-dd HH:mm:ss,SSS}  %m%n  (%c thread:%t)%n";
	// contains some format characters which are slow - *only* for debugging
	private static final String FILE_LOGGING_LAYOUT_FULL =
		"%-5p %d{yyyy-MM-dd HH:mm:ss,SSS}  %m%n  (%c.%M() [line %L], thread:%t)%n";
	
	
	
	// logging levels as defined in org.apache.log4j.Level
	// this variable is used via a getter method to print out command
	// line options help
	private static String stringLevels = 
		Level.TRACE.toString()  + ", " + // int value: 10000
    Level.DEBUG.toString()  + ", " + // int value: 10000
		Level.INFO.toString()   + ", " + // int value: 20000
		Level.WARN.toString()   + ", " + // int value: 30000
		Level.ERROR.toString() + ", " +  // int value: 30000
		Level.FATAL.toString();          // int value: 50000

	// default logging level (if it wasn't specified or was incorrect)
	private static Level defaultLevel = Level.INFO;
	
	
	/**
	 * The constructor must be protected so that it is visible
	 * from ALoggerFactory. Should not be instantiated directly, but via
	 * .getInstance() 
	 * @param name
	 */
	protected ALogger(String name) 
	{
		super(name);
		
	} // ALogger() ----------------------------------------------------------

	
	
	/**
	 * initialize(String[] options)
	 * Initializes the logger
   * @param level severity level as read from the command line option
   * @param destination destination to write logs to, as read from the command line
   */
	public static void initialize(String level, String destination)
	{

		// set logging level, if level contains nonsense, set default
		Level currentLevel = Level.toLevel(level, defaultLevel);
		

		// set properties - LogggerFactory
		// log4j.loggerFactory=atlantis.utils.ALoggerFactory
		// this may also be done via properties file (if necessary for
		// more complex logging configuration without necessity to
		// recompile). properties file is contains pairs key=value
		// caution: must be accessed generically (so that it also
		// works when atlantis is packed in a .jar file)
		// without setting LoggerFactory - ClassCastException when getting
		// Logger via ALogger.getLogger("Atlantis");
		Properties up = new Properties();
		up.setProperty("log4j.loggerFactory",
				       "atlantis.utils.ALoggerFactory");
		PropertyConfigurator.configure(up);
		
		// if instantiated this way
		// logger = new ALogger();
		// -> NullPointerException when adding appender
		
		// all loggers created afterwards will have properties of this root logger
		root = Logger.getRootLogger();
		
		root.setLevel(currentLevel);

		
		// logging will always be done to console (System.out - stdout)

		Layout consoleLayout = new PatternLayout(CONSOLE_LOGGING_LAYOUT);
		ConsoleAppender ca = new ConsoleAppender(consoleLayout,
				ConsoleAppender.SYSTEM_OUT);
		ca.setImmediateFlush(true);
		root.addAppender(ca);

		root.info("Logging to console (System.out) initialised");
	
		// if a file was specified as logging destination, then logs
		// will be duplicated to that file as well
		if(destination != null)
		{
			// check if the file already exists
			File f = new File(destination);
			boolean fileExists = false;
			if(f.exists())
			{
				fileExists = true;
			}
			try
			{
				// some file was specified on the command line
				Layout fileLayout = new PatternLayout(FILE_LOGGING_LAYOUT);
				// if the file exists, it will be appended to
				FileAppender fa = new FileAppender(fileLayout, destination);
				fa.setImmediateFlush(true);
				root.addAppender(fa);
				if(fileExists)
				{
					String msg = "==================================  " +
					             "Log file " + destination + " exists, " +
					             "opening it for appending";
					root.warn(msg);
				}
				root.warn("Logging to " + destination + " initialised");
			}
			catch(IOException ex)
			{
				root.warn("Can't write log messages to: "+ destination);
			}
		}
				
	} // initialize() -------------------------------------------------------

	

	public static ALogger getLogger(String name) 
	{
		if(root != null)
		{	
			return (ALogger) Logger.getLogger(name, loggerFactory);
		}
		else
		{
			// logger hasn't been initialised yet, initialise with
			// default values (i.e. logging only to console with INFO level)			
			try
			{
				initialize(defaultLevel.toString(),null);
			}
			catch(Exception ex)
			{
				// this exception should never occur
				ex.printStackTrace();
			}
			return (ALogger) Logger.getLogger(name, loggerFactory);
		}
		
	} // getLogger() --------------------------------------------------------

	

	public static ALogger getLogger(Class clazz)
	{
		return ALogger.getLogger(clazz.getName());
		
	} // getLogger() --------------------------------------------------------

	
	
	public void forcedLog(String fqcn, Object msg, Throwable t)
	{
		super.forcedLog(fqcn, Level.ALL, msg, t);
		
	} // forcedLog() --------------------------------------------------------

	
	
	public static String getStringLevels()
	{
		return stringLevels;
		
	} // getStringLevels() --------------------------------------------------
	
} // class ALogger ==========================================================
