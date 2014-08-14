package atlantis;

import atlantis.event.AEpsImageProducer;
import atlantis.event.AImageProducer;
import atlantis.event.AEventSource;
import atlantis.event.APngImageProducer;
import atlantis.event.ASvgImageProducer;
import atlantis.globals.AGlobals;
import atlantis.gui.AGUI;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;
import java.awt.Dimension;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.GnuParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.ParseException;

/**
 * This package-private class holds all the command line options
 * as well as providing verification
 * @author Sebastian Boeser
 */
class ACommandLine {

  //A help formatter
  private final HelpFormatter formatter = new HelpFormatter();
  
  private AGlobals globals = AGlobals.instance();

  //The set of command-line options
  private final Options options = new Options();

  //The usage line
  private static final String usage = "Atlantis [OPTIONS] [[-s]SOURCE]";

  //The help message header and footer
  private final String headerMsg;
  private final String footerMsg;


  /**
   * Create all the options in the constructor. Options are sorted alphabetically in here.
   */
  ACommandLine(){

    //show HLTAutokey tracks
    options.addOption(new Option("a", false, "show HLTAutoKey tracks"));

    //configuration file
    options.addOption(OptionBuilder.withArgName("file")
      .hasArg()
      .withDescription("read configuration from specified file")
      .withLongOpt("config")
      .create("c")
    );
 

    //debug
    String debugDescr = "log messages of level to " +
                        "'stdout', optionally to file destination. " +
                        "Message levels are "+ ALogger.getStringLevels() + " (INFO is default)";
    options.addOption(OptionBuilder
      .hasArg()
      .hasOptionalArg()
      .withArgName("level [destination]")
      .withDescription(debugDescr)
      .withLongOpt("debug")
      .create('d')
    );

 
    //help/usage
    options.addOption(new Option("h", "help", false, "display this help message"));

    //demo mode
    options.addOption(OptionBuilder.withArgName("projections")
      .hasOptionalArg()
      .withDescription("colon seperated list of projections for the demo mode (e.g \"-j YX:VP\"). "
          + "Available projections/actions are YX, "
          + "VP(" + AMath.PHI + AMath.ETA + "), "
          + "RZ(" + AMath.RHO + "Z), "
          + "FR(" + AMath.PHI + AMath.RHO + "), "
          + "FZ(" + AMath.PHI + "Z), "
          + "XZ(X'Z), "
          + "YZ(Y'Z), "
          + "LegoPlot, "
          + "NE(next event)")
      .withLongOpt("projection")
      .create("j")
    );

    //loop mode
    String loopDescr = "Set event navigation mode, supported values are "
                      +AEventSource.NavigationMode.LOOP.toString()+", "
                      +AEventSource.NavigationMode.RANDOM.toString()+", "
                      +AEventSource.NavigationMode.SEQUENTIAL.toString();
    options.addOption(OptionBuilder.withArgName("mode")
      .hasArg()
      .withDescription(loopDescr)
      .withLongOpt("loop")
      .create("l")
    );

    //color map
    options.addOption(OptionBuilder.withArgName("file")
      .hasArg()
      .withDescription("read colourmap from specified file")
      .withLongOpt("colormap")
      .create("m")
    );

    // output image files
    String outputDescr = "Output event image " +
    "into directory dir (default: current directory) "+
    "of dimensions width x height (default: 800x828). "+
    "If height is not specified the aspect ratio of the canvas is used. " +
    "If scale is given, images will be scaled by that factor (use for thumbnails). "+
    "If a filename is given, that file will be overwritten, "+
    "otherwise a new file will be created for each event. "+
    "The output format may be specified as PNG (the default) or EPS using" +
    "the --outputformat option.";
    options.addOption(OptionBuilder.hasOptionalArgs(4)
     .withArgName("[dir] [width[xheight]] [scale] [filename]")
     .withDescription(outputDescr)
     .withLongOpt("output")
     .create('o')
    );
    
    String outputFormatDescr = "output format for -o option (eps or png, default is png)";
    options.addOption(OptionBuilder.hasArg()
    		.withArgName("format")
    		.withDescription(outputFormatDescr)
    		.withLongOpt("outputformat")
    		.create('f')
    );

    //event loop intervall
    options.addOption(OptionBuilder.withArgName("seconds")
      .hasArg()
      .withDescription("pause inbetween events and/or projections options")
      .withLongOpt("pause")
      .create("p")
    );

    //default source
    String sourceDescr = "Set initial event source, e.g\n"+
                         "*    [file://]<filename>\n"+
                         "*     http://<url>\n"+
                         "*   xmlrpc://<server>:<port>\n"+
                         "*   oncrpc://<server>[:port][:stream]";
    options.addOption(OptionBuilder.withArgName("source")
      .hasArg()
      .withDescription(sourceDescr)
      .withLongOpt("source")
      .create("s")
    );

    //update config
    options.addOption(new Option("x", "updateconfig", false, "update user's configuration file"));

    //max GUI width
    options.addOption(OptionBuilder.withArgName("maxwidth")
      .hasArg()
      .withDescription("maximum screen width used by Atlantis")
      .withLongOpt("maxwidth")
      .create("w")
    );


    //image server
    options.addOption(OptionBuilder.hasArgs(1)
      .withArgName("port")
      .withDescription("Run a server on port <port> which accepts " +
                        "event data from a client and returns images")
      .withLongOpt("imgserver")
      .create('I')
    );

   
    /**
     * Now create header and footer Messages
     */
    headerMsg = "\nMandatory arguments to long options are " +
                 "mandatory for short options too.\n";
    footerMsg = "\nAtlFast objects were supported up to " +
                "AtlantisJava-09-05-28 via \"Fast\" option.\n" +
                "Beginner simplified GUI was supported up to " +
                "AtlantisJava-09-05-28 via \"Beginner\" option.\n" +
                "CTB 2004 geometry was supported up to " +
                "AtlantisJava-09-07-42 via -t, --testbeam option.\n";

  }
  
  /**
   * Parse and validate the command line options
   */
  void process(String[] args){
  
    //Create GNU style parse
    CommandLineParser parser = new GnuParser();  
    
    //Now try parsing the options
    try {
      
      //invoke GNU parser
      CommandLine cmdLine = parser.parse(options, args);

      /**
       * Now validate and process all options
       */
       
      //auto-keys
      if(cmdLine.hasOption('a')) globals.setHLTAutoKeys(true);
       
      //config
      if(cmdLine.hasOption('c')){
         Atlantis.configCommandLine = cmdLine.getOptionValue('c', null);
      }

      //debug
      if(cmdLine.hasOption('d')){
        //Get option values
        String[] vals = cmdLine.getOptionValues('d');
        //this arg will always be there
        String level = vals[0];
			  if (! ALogger.getStringLevels().contains(level))
          throw new ParseException("Invalid logging level "+level);
        //Check if there was also another argument
        String destination = null;
        if(vals.length == 2) destination = vals[1];
        
        //Finally initialize the logger
        ALogger.initialize(level,destination);
      }

      //help
      if(cmdLine.hasOption('h')) {
        formatter.printHelp(usage, headerMsg, options, footerMsg);
        System.exit(0);
      }

      //image server
      if(cmdLine.hasOption('I')){
        Atlantis.imageServerPort = Integer.parseInt(cmdLine.getOptionValue('I'));
        if (Atlantis.imageServerPort < 0)
          throw new ParseException("Port number "+cmdLine.getOptionValue('I')+
                             " is not valid for -I option");
         //Check for other mutual exclusive options
        if (cmdLine.hasOption('j') || cmdLine.hasOption('p'))
          throw new ParseException("Can not use option -I with -j or -p options");
      }

      //demo mode projections
      if(cmdLine.hasOption('j')){
        //NOTE: default is not null but "", so we can distinguish inbetween
        //this flag being set or not w/o introducing more parameters
        Atlantis.selectedProjections = cmdLine.getOptionValue('j', "");
      }

      //navigation mode
      if(cmdLine.hasOption('l')){
        //Get arguments
        String loopMode = cmdLine.getOptionValue('l',null);
        if (loopMode == null)
          throw new ParseException("Option -l needs an argument");
        Atlantis.initialMode = AEventSource.NavigationMode.valueOf(loopMode);
      }

                
      //color map
      if(cmdLine.hasOption('m')){
        // retrieve the argument for options or assign default value
        Atlantis.colormapCommandLine = cmdLine.getOptionValue('m', null);
      }
  
      // generate PNG/EPS output files
      if(cmdLine.hasOption('o'))
      {
        String format = "PNG";
        if (cmdLine.hasOption('f')) {
        	String formatVal = cmdLine.getOptionValue('f');
        	format = formatVal.toUpperCase();
        	if (!("EPS".equals(format)||"PNG".equals(format))) {
                throw new ParseException("Invalid output format "+formatVal);
        	}
        }
        String[] vals = cmdLine.getOptionValues('o');
        this.processImageOutputOptions(vals,format);
      }
      
      // generate PNG output files
      if(cmdLine.hasOption('e'))
      {
        String[] vals = cmdLine.getOptionValues('e');
        this.processImageOutputOptions(vals,"EPS");
      }
      
      //source
      if (cmdLine.hasOption('s')){
        //Simply set this as default event source
        Atlantis.initialEventSource = cmdLine.getOptionValue('s');
      }

      //pause
      if (cmdLine.hasOption('p')){
        //Check for integer value
        String val = cmdLine.getOptionValue('p', null);
        try {
          Atlantis.loopInterval = Integer.parseInt(val);
        } catch (NumberFormatException nfe) {
            throw new ParseException("Invalid seconds value "+val+" for -p option");
        }
      }
         
      //GUI width
      if(cmdLine.hasOption('w')){
        String val = cmdLine.getOptionValue('w', null);
         try {
            int maxWidth = Integer.parseInt(val);
        	AGUI.getGUI().setMaxWidth(maxWidth);
         } catch (NumberFormatException e) {
            throw new ParseException("Invalid width value "+val+" for -w option");
         }
      }

      //update config
      if(cmdLine.hasOption('x')) Atlantis.updateConfig = true;

      //finally check for positional options (i.e w/o --opt)
      String[] unspecifiedOpts=cmdLine.getArgs();
      if (unspecifiedOpts.length > 0){
        //We accept only one, which is a single string for the event source
        if ((unspecifiedOpts.length != 1)
            || cmdLine.hasOption('s')
            || cmdLine.hasOption('I')){
          throw new ParseException("Can only specify one event source");
        }
        Atlantis.initialEventSource = unspecifiedOpts[0];
      }

    } catch(ParseException ex) {
      //Every error above should have thrown a parse exception
      //Show help message and exit
      System.err.println("\n" + ex.getMessage() + "\n");
      formatter.printHelp(usage, headerMsg, options, footerMsg);
      System.exit(1);
    }

  }
  
  private void processImageOutputOptions(String[] vals, String fileFormat) throws ParseException {
	  try
	  {
		  //vals[0] - directory to store events and png into
		  //Use current directory as default
		  String dir=System.getProperty("user.dir");
		  if ((vals!=null)&&(vals.length > 0)) dir = vals[0];
		  // vals[1] - WidthxHeight string
		  Dimension dim=new Dimension(800,828);
		  if ((vals!=null)&&(vals.length > 1)){
			  short width = Short.parseShort(vals[1].split("x")[0]);
			  short height =-1;
			  if (vals[1].split("x").length > 1)
				  height = Short.parseShort(vals[1].split("x")[1]);
			  dim = new Dimension(width,height);
		  }
		  // vals[2] - scale factor, default is one
		  double scale = 1;
		  if ((vals!=null)&&(vals.length > 2)) scale = Double.parseDouble(vals[2]);
		  // vals[3] - fixed filename, default is null
		  String fixedFile = null;
		  if ((vals!=null)&&(vals.length > 3)) fixedFile = vals[3];
		  //Now create a new image generator
		  if ("EPS".equals(fileFormat)) {
			  Atlantis.imageProducer = new AEpsImageProducer(dir,dim,scale,fixedFile);
		  }
		  else if ("PNG".equals(fileFormat)) {
			  Atlantis.imageProducer = new APngImageProducer(dir,dim,scale,fixedFile);
		  }
		  else if ("SVG".equals(fileFormat)) {
			  Atlantis.imageProducer = new ASvgImageProducer(dir,dim,scale,fixedFile);
		  }
		  else {
			  throw new Error("Invalid file format for output: "+fileFormat);
		  }
	  } catch (NumberFormatException nfe){
		  throw new ParseException("Invalid parameters to option -o");
	  } catch (IndexOutOfBoundsException iob){
		  throw new ParseException("Invalid dimension parameter to option -o");
	  } catch (AImageProducer.InstantiationException iex){
		  throw new ParseException("Invalid configuration for option -o: "+iex.getMessage());
	  }
  }

}
