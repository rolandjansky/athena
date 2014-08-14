
package atlantis.event.xmlrpc;

import atlantis.event.*;
import atlantis.data.AEventFromXML;
import atlantis.Atlantis;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;

import atlantis.utils.ALogger;

import java.io.ByteArrayInputStream;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Vector;


/**
 * Implements an event source retrieving events from an XMLRCP server.
 * By implementing a ABufferedEventSource, a number of previous events will be
 * available to the user. Care is taken to clear the buffer in case we run into
 * memory problems.
 * 
 * @author sboeser
 */

public class AXMLRPCEventSource extends ABufferedEventSource {

  //The logger
  private static final ALogger logger = ALogger.getLogger(AXMLRPCEventSource.class);
  
  //The XMLRPC client
  private AClientXMLRPC XMLRPCClient = null;
  //The Method to call on the server side
  private static final String GETEVENTDATA = "getEventData";
  //The default timeout value in milliseconds for calling the server
  private static final int DEFAULT_TIMEOUT = 2000;
  //The name of the local machine (send along with event request)
  private String localMachineName = "<unknown>";
  //The name and port number of the remote host
  private String remoteMachineName = null;
  //The port number to which we should connect
  private Integer portNumber = null;
  //The timeout for a request
  private Integer timeOut = null;  

  /**
   * Constructor with the server name and port as a string value
   * @param sourceName
   * @throws InvalidEventSourceException 
   */
  public AXMLRPCEventSource(String sourceName) throws InvalidEventSourceException {
    
   //Remove any leading "xmlrpc://" indicator
   sourceName = sourceName.replaceFirst("xmlrpc://", "");
   //try to split sourceName in serverName:port pair
   String[] sourceParams = sourceName.split(":");
   //Do some checks
   if (sourceParams.length != 2)
     throw new InvalidEventSourceException("Invalid <server:port> specification: "+sourceName);
     
   //Try to get the port number
   int port = -1;
   try {
     port = Integer.decode(sourceParams[1]);
   } catch (NumberFormatException nfe){
     throw new InvalidEventSourceException("Invalid port number: "+sourceParams[1]);
   }
   
   if (port < 0)
     throw new InvalidEventSourceException("Invalid port number: "+port);  
   
   //Now call the fully qualified constructor with the defaul timeout value
   CreateXMLRPCEventSource(sourceParams[0],port,DEFAULT_TIMEOUT);
 
  }
 
  /**
   * Constructor with server name, port number and timeout
   * @param serverName the name or IP-address of the remote server 
   * @param port the port number to which the request is send
   * @param timeout a timeout value in milliseconds
   * @throws InvalidEventSourceException 
   */
  public AXMLRPCEventSource(String serverName, int port, int timeout)
          throws InvalidEventSourceException {
    //Call common constructor entry
    CreateXMLRPCEventSource(serverName,port,timeout);
  }
 
  /**
   * @return a string of the form xmlrpc://servername:port
   */
  public String getSourceName() {
    return "xmlrpc://"+remoteMachineName+":"+portNumber;
  }

  /**
   * @param mode event navigation mode
   * @return true if the requested mode is supported by this source
   */
  public boolean supportsNavigationMode(NavigationMode mode) {
    if(mode == NavigationMode.SEQUENTIAL)
      return true;
    return false;
  }

  /**
   * Common entry point for all constructors
   * @param serverName the name or IP-address of the remote server 
   * @param port the port number to which the request is send
   * @param timeout a timeout value in milliseconds
   * @throws InvalidEventSourceException
   */
  private void CreateXMLRPCEventSource(String serverName, int port, int timeout)
         throws InvalidEventSourceException {

    //Store the input values
   remoteMachineName = serverName;
   portNumber = port;
   timeOut = timeout;
    
   //try to obtain the name of the local machine
   try {
    localMachineName = InetAddress.getLocalHost().getHostName();
   } catch (UnknownHostException ex) {
     //Can't get name of local host
     String msg = "Unable to obtain local machine name, using \""
                + localMachineName +"\" instead";
     logger.error(msg);
   } catch(Exception ex) {
     //Rethrow everything else as invalid source exception
     throw new InvalidEventSourceException("Exception trying to obtain local machine name",ex);
   }
   
   //Finally create a new AClientXMLRPC object
   XMLRPCClient = new AClientXMLRPC(GETEVENTDATA);
   
  }
    
  /**
   * Read the next event from the server
   * @param currentEvent the current event
   * @return the next event
   * @throws NoMoreEventsException
   * @throws InvalidEventSourceException
   * @throws ReadEventException
   */
  @Override
  protected AEvent readNext(AEventInfo currentEvent) throws NoMoreEventsException,
                                                      InvalidEventSourceException,
                                                      ReadEventException {
    //Be verbose
    logger.info("Call to remote server xmlrpc://" + remoteMachineName + ":" + portNumber.toString());

    //send the name of the local machine to the JiveXML server
    Vector<String> parameters = new Vector();
    parameters.add(localMachineName);

    //record calling time
    long startTime = System.currentTimeMillis();

    //The XMLRPC client will return data as a string
    String eventXMLData = null;

    // perform the remote call
    try {
      eventXMLData = XMLRPCClient.callXMLRPCServer("http://"+remoteMachineName, portNumber, parameters);
    } catch (ARemoteCallerException ex) {
      //An error occured
      String msg = "readNext(): Exception while calling remote server";
      logger.error(msg,ex);
      //rethrow
      throw new InvalidEventSourceException(msg, ex);
    }

    //do some statistics
    long callDuration = System.currentTimeMillis() - startTime;
    logger.debug("Received " + eventXMLData.length() + " bytes in " + callDuration + " ms");


    //If there is just no new event, we should get back an empty string
    if (eventXMLData.length() == 0) {
      throw new NoMoreEventsException("No event available from the server (yet)");    // something was received, going to process the result
    }
    if (!(eventXMLData.startsWith("<?xml"))) {
      String msg = "Invalid event data received from JiveXML server";
      logger.error(msg);
      throw new ReadEventException(msg);
    }

   
    
    //So it seems we got something worth processing
    logger.info("Received data from remote server, going to process it ...");

    // Wrap the string as an inputstream
    ByteArrayInputStream bais = new ByteArrayInputStream(eventXMLData.getBytes());

    //Construct a new event
    AEvent event = AEventFromXML.read(bais, remoteMachineName+":"+portNumber);

    /**
     * The XMLRPC server is not clever enough to return no event if there is no
     * new one. In order to avoid filling the buffer with identical events, we
     * therefore check here if we don't already have that event, and throw a 
     * NoMoreEventsException in case we do.
     */
    if (event.equals(currentEvent)){
      String msg = "Already have event "+event.getEventNumber()+" in run "
                   + event.getRunNumber() + " - ignored!";
      throw new NoMoreEventsException(msg);
    }
   

    //Now we are sure we have a next event, return it
    return event;
    
  }
  
/**
   * Requesting to read the previous event from the server only appears
   * if we have already reached the beginning of the event buffer.
   * Only one current events are provided by the server at any time, thus we
   * will always throw a NoMoreEventsException. Handling of the exception is left
   * to the GUI interface, and may e.g be ignored in loop mode.
   * @param currentEvent will be ignored
   * @return nothing as it will always raise an exception
   * @throws NoMoreEventsException
   */
  @Override
  protected AEvent readPrevious(AEventInfo currentEvent) throws NoMoreEventsException
  {
    throw new NoMoreEventsException("XMLRPC server does not provide a previous event");
  }

}
