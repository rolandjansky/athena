package atlantis.output;

import atlantis.globals.AGlobals;
import atlantis.utils.ALogger;

/**
 * AOutput provides access to the specific static instance of AMainLogPane which
 * is used for displaying general information within Atlantis
 * @author Adam Davison
 */
public class AOutput
{
  //Handle to the main log pane  
  private static ALogInterface outputPane;

  /** Logger for when not outputting to the GUI (Minerva). */
  private final static ALogger logger = ALogger.getLogger(AOutput.class);
  
  /** Append text to output pane/logger depending on output level. */
  public static void append(String s, String style) { 
	  if (outputPane==null) return; // in case not set yet, or no GUI
	  if(AGlobals.instance().getSimpleOutput()>0){
		  if (style.equals(ALogInterface.BAD_COMMAND) || style.equals(ALogInterface.WARNING)) {
			  logger.warn(s);
		  }else{
			  logger.info(s);
		  }
	  }else{
		  outputPane.append(s, style); 
	  }
  }
  
  /** Append text to output pane - only for pick info (and select event/debug info). */
  public static void alwaysAppend(String s, String style) {
	  if (outputPane==null) return; // in case not set yet, or no GUI
	  outputPane.append(s, style);
  }

  /**
   * Provide interface to the log pane in the GUI.
   * @param outputDisplay the log pane
   */
  public static void setOutput(ALogInterface outputDisplay) {
	  AOutput.outputPane = outputDisplay;
  }

  /**
   * Get interface to the log pane in the GUI, if any.
   * @param outputDisplay the log pane
   */
  public static ALogInterface getOutput() {
	  return outputPane;
  }   
}
