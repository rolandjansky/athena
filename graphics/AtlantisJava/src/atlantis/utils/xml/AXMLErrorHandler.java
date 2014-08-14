package atlantis.utils.xml;

import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXParseException;

import atlantis.utils.ALogger;

/**
 * Used in the XML processing code to catch and store the possible exceptions and errors.
 */
public class AXMLErrorHandler implements ErrorHandler {
  
  //the logger
  private final ALogger logger =  ALogger.getLogger(AXMLErrorHandler.class);
  /**
   * Thee different error states encountered in XML parsing 
   */
  public enum ErrorState {

    NO_ERROR,         // no error
    IGNORED_DATA,     // the given data will be ignored
    UNKNOWN_TAG,      // unknown tag
    INVALID_DATA,     // the given data is not valid (e.g. string instead of int) 
    OTHER_ERROR,      // some other error
    FATAL_ERROR       // some fatal error - event has to be aborted
  }
  
  //Store the current error state
  private ErrorState errorState = ErrorState.NO_ERROR;
  //Store the exception causing  the error
  private Throwable cause = null;
  
  /**
   * @return the current error state of the parser
   */
  public ErrorState getErrorState() {
    return errorState;
  }
  
  /**
   * @return the cause of the last error
   */
  public Throwable getErrorCause(){
    return cause;
  }

  /**
   * Set the error state of the parser
   * @param errorState The errorState to set.
   * @param cause the cause of the error
   */
  public void setError(ErrorState errorState, Throwable cause) {
    
    //check that the user had actually given a cause for all errors
    if (cause == null && (errorState != ErrorState.NO_ERROR ))
      throw new IllegalArgumentException("Cause can not be NULL"); 
    
    this.errorState = errorState;
    
    //force cause null for NO_ERROR
    if (errorState == ErrorState.NO_ERROR) 
      this.cause = null;
    else 
      this.cause = cause;
  }

  /**
   * Handles all errors occurding during parsing
   * @param exception the SAXParseException causing the error
   */
  public void error(SAXParseException exception) {

    //Only keep the first error, ignore subsequents
    if ( getErrorState() != ErrorState.NO_ERROR ) return;

    //Decode error from exception message (is there no better way?!
    String errorMessage = exception.getMessage();
    
    //Check for undeclared elements
    if ( errorMessage.startsWith("Element type") &&
         errorMessage.indexOf("must be declared") >= 0) {
      // an undeclared element is found in event file
      // errorMessage - Element type "elementName" must be declared.
      setError(ErrorState.UNKNOWN_TAG,exception);
      return;
      
    } else if (errorMessage.startsWith("The content of element type") &&
               errorMessage.indexOf("must match") >= 0) {
      /* errorMessage - The content of element type "elementName" must
      match ... -
      this error will occur if any undeclared element is found.
       */
      setError(ErrorState.INVALID_DATA,exception);
      return;
    } else {
      // an untreated error (e.g. undeclared attributes, etc.)
      // supposed to be corrected by modifying event.dtd or retrievers
      logger.error("Parsing XML in line " + exception.getLineNumber() + ": " + errorMessage);
      setError(ErrorState.OTHER_ERROR,exception);
      return;
    }

  }

  /**
   * Handle warning exceptions
   * @param exception the SAX exception
   */
  public void warning(SAXParseException exception) {
    //Write a warning output, otherwise ignore
    logger.warn("Parsing XML at position " + exception.getLineNumber() + ", " + exception.getColumnNumber() + " :: " + exception.getMessage());
  }
  
  /**
   * Handle fatal exceptions
   * @param exception the SAX exception
   */
  public void fatalError(SAXParseException exception) {
      //Only fatal exceptions are handled by global exception handler
      String m = "Fatal error while parsing XML, line: " +
                 exception.getLineNumber() + " column: " +
                 exception.getColumnNumber() + " reason: " +
                 exception.getMessage();
      logger.fatal(m);
      System.exit(1);
  }
}
