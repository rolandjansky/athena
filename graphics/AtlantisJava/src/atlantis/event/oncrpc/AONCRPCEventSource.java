
package atlantis.event.oncrpc;

import atlantis.data.AEventFromXML;
import atlantis.utils.ALogger;

import atlantis.event.ABufferedEventSource;
import atlantis.event.AEvent;
import atlantis.event.AEventInfo;
import atlantis.event.AStreamedEventSource;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;

import org.acplt.oncrpc.XdrVoid;
import org.acplt.oncrpc.OncRpcTcpClient;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.OncRpcProgramNotRegisteredException;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.net.ConnectException;
import java.net.InetAddress;
import java.util.Arrays;
import java.util.Vector;

/**
 * Implements an event source retrieving events from an ONC/RPC server.
 * By implementing a ABufferedEventSource, a number of previous events will be
 * available to the user. Care is taken to clear the buffer in case we run into
 * memory problems. This event source also provides event streams by implementing
 * AStreamedEventSource.
 * 
 * @author sboeser
 */

public class AONCRPCEventSource extends ABufferedEventSource implements AStreamedEventSource {

  //The logger
  private static final ALogger logger = ALogger.getLogger(AONCRPCEventSource.class);
  
  //The ONC/RPC client
  private OncRpcTcpClient client = null;
 
  //The default timeout value in milliseconds for calling the server
  private static final int DEFAULT_TIMEOUT = 2000;
  //The name of the stream to read events from
  private String currentStream = null;

  /**
   * Constructor with the server name and port as a string value
   * @param sourceName
   * @throws InvalidEventSourceException 
   */
  public AONCRPCEventSource(String sourceName) throws InvalidEventSourceException {
    
   //Remove any leading "oncrpc://" indicator
   sourceName = sourceName.replaceFirst("oncrpc://", "");
   
   //Check if we can split the sourceName in hostName:port pair
   String[] sourceParams = sourceName.split(":");
   //Do some sanity check
   if (sourceParams.length > 3)
     throw new InvalidEventSourceException("Invalid <server[:port][:stream]> specification: "+sourceName);
     
   //Now try to get the port number if it was given,
   //Otherwise get port from portmapper by using 0;
   int port = 0;
   if ((sourceParams.length > 1)&&(sourceParams[1].length() > 0)){
     try {
       port = Integer.decode(sourceParams[1]);
     } catch (NumberFormatException nfe){
       throw new InvalidEventSourceException("Invalid port number: "+sourceParams[1]);
     }
   }

   //More sanity checks
   if (port < 0)
     throw new InvalidEventSourceException("Invalid port number: "+port);  

   //If a stream name was given, set it now
   if (sourceParams.length > 2) currentStream = sourceParams[2];


   //Now call the fully qualified constructor with the defaul timeout value
   createClient(sourceParams[0],port,DEFAULT_TIMEOUT);
 
  }
 
  /**
   * Constructor with server name, port number and timeout
   * @param hostName the name or IP-address of the remote server
   * @param port the port number to which the request is send - can be 0
   * @param timeout a timeout value in milliseconds
   * @throws InvalidEventSourceException 
   */
  public AONCRPCEventSource(String hostName, int port, int timeout) throws InvalidEventSourceException {
    //Call common constructor entry
    createClient(hostName,port,timeout);
  }
 
  /**
   * @return a string of the form oncrpc://hostName:port:stream
   */
  public String getSourceName() {
    return "oncrpc://"+client.getHost().getHostName()+":"+ client.getPort()+
      ((currentStream != null) ? ":"+currentStream : "");
  }

  /**
   * This source only supports sequential mode.
   * @param mode event navigation mode to test
   * @return true if the requested mode is supported by this source
   */
  public boolean supportsNavigationMode(NavigationMode mode) {
    if(mode == NavigationMode.SEQUENTIAL)
      return true;
    return false;
  }

  /**
   * Common entry point for all constructors
   * @param hostName the name or IP-address of the remote server
   * @param port the port number to which the request is send
   * @param timeout a timeout value in milliseconds
   * @throws InvalidEventSourceException
   */
  private void createClient(String hostName, int port, int timeout) throws InvalidEventSourceException {
 
    //Now try to create the client
    try {
      client = new OncRpcTcpClient(InetAddress.getByName(hostName),
              Server.ONCRPCSERVERPROG,Server.ONCRPCSERVERVERS,port);
    } catch ( ConnectException e ) {
      throw new InvalidEventSourceException("Could not connect to "+hostName,e);
    } catch ( OncRpcProgramNotRegisteredException e ) {
      throw new InvalidEventSourceException("Server not (yet) registered on "+hostName,e);
    } catch ( OncRpcException e ) {
      throw new InvalidEventSourceException("ONC/RPC exception while connecting to "+hostName,e);
    } catch ( IOException e ) {
      throw new InvalidEventSourceException("I/O Exception while connecting to "+hostName,e);
    }

    //Set the requested timeout
    client.setTimeout(timeout);
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
  protected synchronized AEvent readNext(AEventInfo currentEvent) throws NoMoreEventsException,
                                                         InvalidEventSourceException,
                                                         ReadEventException {

    //Be verbose
    logger.info("Call to remote server "+getSourceName());

   //Ping the server to make sure it is alive, otherwise try to recreate client
    try {
      client.call(Server.ONCRPC_NULL_PROC,XdrVoid.XDR_VOID, XdrVoid.XDR_VOID);
    } catch (OncRpcException e) {
      //Server does not respond to ping, try to recreate client
      logger.info("Failed to ping server "+getSourceName()+", trying to recreate client");
      //Use setting from old client to create new client, but allow new port number
      createClient(client.getHost().getHostName(),0,client.getTimeout());
    }

    //Create an event request - this may als set the default stream
    EventRequest eventReq = new EventRequest(-1,-1,getStream());


    //use information from existing event if there is
    if (currentEvent != null){
      eventReq.EventNumber = currentEvent.getEventNumber();
      eventReq.RunNumber = currentEvent.getRunNumber();
    }
     
    //Also prepare the return structure
    Event event = new Event();

    //record calling time
    long startTime = System.currentTimeMillis();
    
  
    // perform the remote call
    try {
      client.call(Server.ONCRPC_GETEVENT_PROC, eventReq, event);
    } catch (OncRpcException e) {
      throw new InvalidEventSourceException("Failed to get new event from server",e);
    }

    //do some statistics
    long callDuration = System.currentTimeMillis() - startTime;
    logger.debug("Received " + event.EventData.length + " bytes in " + callDuration + " ms");

    //Now analyze the event we got
    //Check if we already have the event
    if (event.isIdentical)
      //already have this event
      throw new NoMoreEventsException("Already have event "+event.EventNumber +
                                      " from run "+event.RunNumber);
    //Check if the stream was available
    if (!event.isAvailable)
      //make event source invalid
      throw new InvalidEventSourceException("No event available for stream "+eventReq.StreamName);
    
    //Check if the event is compressed
    if (event.isCompressed)
      //not supported yet
       throw new UnsupportedOperationException("Support for compressed events not yet implemented");

    //It seems we got something worth processing
    //Strip the trailing null character
    event.EventData=(new String(event.EventData)).trim().getBytes();
    //Wrap the data as an inputstream
    ByteArrayInputStream bais = new ByteArrayInputStream(event.EventData);
  
    //Finally construct a new event and return it
    return AEventFromXML.read(bais, currentStream);
  }
  
/**
   * Requesting to read the previouse event from the server only appears
   * if we have already reached the beginning of the event buffer.
   * Only one current events are provided by the server at any time, thus we
   * will always throw a NoMoreEventsException. Handling of the exception is left
   * to the GUI interface.
   * @param currentEvent will be ignored
   * @return nothing as it will always raise an exception
   * @throws NoMoreEventsException
   */
  @Override
  protected AEvent readPrevious(AEventInfo currentEvent) throws NoMoreEventsException
  {
    throw new NoMoreEventsException("ONC/RPC server does not provide a previous event");
  }

  /**
   * Get a list of available streams from the server
   * @return the list of stream names
   * @throws InvalidEventSourceException if the server cannot be contacted
   */
  public synchronized Vector<String> getAvailableStreams() throws InvalidEventSourceException {
    
    //Prepare response structure
    Streams streams = new Streams();


    //Call the server
    try {
      client.call(Server.ONCRPC_GETSTREAMS_PROC, XdrVoid.XDR_VOID, streams);
    } catch (OncRpcException e) {
      throw new InvalidEventSourceException("Failed to get new stream list from server",e);
    }
    //We got stream names, so just return them
    return new Vector(Arrays.asList(streams.StreamNames));
  }

  /**
   * Set a new stream as current stream
   * @param streamName the name of the stream
   * @throws InvalidStreamException if the stream does not exist at the server
   */
  public synchronized void setStream(String streamName) throws InvalidStreamException,
                                                               InvalidEventSourceException{
    //Check if the requested stream is available
    if (! getAvailableStreams().contains(streamName))
      throw new InvalidStreamException("Stream \""+streamName+"\" not available at server");

    //Store this as the new current stream
    currentStream = streamName;
  }

  /**
   * Get the currently selected stream
   * @return the name of the currently selected stream
   * @throws NoMoreEventsException if no streams are available
   * @throws InvalidEventSourceException if the list of streams can not be obtained
   */
  public synchronized String getStream() throws InvalidEventSourceException,
                                                NoMoreEventsException
  {

    //If stream name is not set, set it to first available by default
    if (currentStream==null){
      //Get list of streams
      Vector<String> streams = getAvailableStreams();
      //If there are none throw a "NoMoreEventsException"
      if (streams.size()<1) throw new NoMoreEventsException("No streams available");
      //Now set current stream as first one in list
      currentStream=streams.firstElement();
      //be verbose
      logger.info("Stream not set, using '"+currentStream+"'");
    }

    return currentStream;
  }
}
