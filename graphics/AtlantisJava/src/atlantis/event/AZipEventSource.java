package atlantis.event;

import atlantis.data.AEventFromXML;

import java.io.IOException;
import java.io.File;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.AAtlantisException;
import atlantis.utils.AUtilities;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.zip.ZipException;
import java.util.Random;

/**
 * Reading zip archives from local or network files or from zip files stored within .jar file 
 */
public class AZipEventSource implements AEventSource
{
  
  //define the source type
  private enum SourceType {
    JAR_FILE,
    ZIP_FILE,
    ZIP_NETWORK_FILE
  };

  // current event navigation mode
  private NavigationMode eventNavigationMode = NavigationMode.SEQUENTIAL;

  // needed for random mode
  private Random randomGenerator = new Random();

  // A list of entries in the zip file
  ArrayList<ZipEntry> ZipEntries = new ArrayList<ZipEntry>();
  // pointer to the current zip entry
  int eventIndex = -1;
  // flag if zip file is read from jar file (as stream)
  SourceType sourceType = null;
  // event file name (or event archive name)
  String eventSource = null;

  /**
   * @return current event navigation mode
   */
  public NavigationMode getNavigationMode() {
    return eventNavigationMode;
  }

  /**
   * @param mode sets the event navigation mode
   */
  public void setNavigationMode(NavigationMode mode) throws InvalidEventSourceException {
    if(supportsNavigationMode(mode)) {
      eventNavigationMode = mode;
      // go to the first event
      eventIndex = -1;
    }
    else
      throw new InvalidEventSourceException("Mode '"+mode+"' not supported by current source");
  }

  /**
   * @param mode event navigation mode
   * @return true if the requested mode is supported by this source
   */
  public boolean supportsNavigationMode(NavigationMode mode) {
    if(mode == NavigationMode.SEQUENTIAL ||
        mode == NavigationMode.RANDOM ||
        mode == NavigationMode.LOOP)
      return true;
    return false;
  }

  /**
   * Constructor with event source
   * @param sourceName the name of the source
   * @throws InvalidEventSourceException 
   */
  public AZipEventSource(String sourceName) throws InvalidEventSourceException
  {
    //Locally opened things
    ZipFile zipfile = null;
    ZipInputStream stream = null;
    
    try {
      // first check if it is an network file
      if (sourceName.startsWith("http://")) {
        
        //open network zip file input stream
        stream = openNetworkStream(sourceName);
        //try to read entries from network file
        readEntriesFromStream(stream);
        //set the source type
        sourceType = SourceType.ZIP_NETWORK_FILE;
        
      } else if (sourceName.startsWith("file://")) {
        
        // strip the file:// from the source name
        sourceName = sourceName.substring(7);

        // Check if this is a regular file
        File file = new File(sourceName);
        
        //Read entries from regular file
        if( (file.exists() && file.canRead())){
      
          //Now try to open as a zipfile
          zipfile = new ZipFile(sourceName);
          //Try to read the entries from that file
          readEntriesFromFile(zipfile);
          //set the source type
          sourceType = SourceType.ZIP_FILE;
          
          
        } else {
          //might still be a zip file in a jar archive
          stream = openJarStream(sourceName);
          //try to read entries from jar file
          readEntriesFromStream(stream);
          //set the source type
          sourceType = SourceType.JAR_FILE;
        }
      }
    } catch(AAtlantisException ae) {
      //Rethrow
      throw new InvalidEventSourceException("Atlantis exception reading from: "+eventSource,ae);
    } catch(MalformedURLException mue) {
      //Rethrow
      throw new InvalidEventSourceException("An unknown protocol is specified: "+eventSource,mue);
    } catch(ZipException zfe) {
      //Rethrow
      throw new InvalidEventSourceException("ZIP format error has occurred: "+eventSource,zfe);
    } catch(IOException ioe) {
      //Rethrow
      throw new InvalidEventSourceException("I/O error has occurred reading: "+eventSource,ioe);
    } finally {
      //Try to close everthing that has been opened 
      try {
        if (zipfile != null) zipfile.close();
        if (stream != null) stream.close();
      } catch (Throwable t){
        //Do nothing if closing fails
      }
    }

    //If we have not got a valid source type, throw an error
    if (sourceType == null)
      throw new InvalidEventSourceException("Could not identify source type of "+sourceName);
    
    //Make sure there are valid events in the file
    if (ZipEntries.size() == 0)
      throw new InvalidEventSourceException("No entries in ZIP archive: "+sourceName);
    
    //Store source name
    eventSource=sourceName;
    //Otherwise set counter to first event
    eventIndex = -1;
  }
  
  /**
   * @return the name of the event file or archive
   */
  public String getSourceName() {
    return eventSource;
  }
   
  /**
   * Try to open a zip file in a jar file as input stream
   * @param sourceName the name of the zip file
   * @return the zip stream
   */
  private ZipInputStream openJarStream(String sourceName) throws AAtlantisException
  {
    //Open as zip input stream
    return new ZipInputStream(AUtilities.getFileAsStream(sourceName));
  }
  
  /**
   * Try to open a zip file on the net as input stream
   * @param sourceName the URL of the zip file
   * @return the zip stream
   */
  private ZipInputStream openNetworkStream(String sourceName) throws ZipException,
                                                                     MalformedURLException,
                                                                     IOException
  {
    //Open a network zip file as input stream
    return new ZipInputStream(new URL(sourceName).openStream());
  }  
  
  
  /**
   * Check if this is a valid name of an entry in a zip file or stream
   * @param entryName the name of the entry
   * @return true if valid
   */
  private boolean checkZipEntry(ZipEntry entry){
    //Make sure it is not a directory
    if(entry.isDirectory()) return false;
    //Make sure it ends with .xml
    if (! entry.getName().toLowerCase().endsWith(".xml")) return false;
    //everthing fine
    return true;
  }
  
  /**
   * Get a list of all valid entries from a zip stream
   * @param zis the input stream
   */
  private void readEntriesFromStream(ZipInputStream zis) throws InvalidEventSourceException
  {
      try {
        //Loop over zip entries
    		ZipEntry next = null;
    		while((next = zis.getNextEntry()) != null){
          //If it is valid add it to our list fo entries
          if (checkZipEntry(next)) ZipEntries.add(next);
        }
      } catch(ZipException ze) {
        //Rethrow
        throw new InvalidEventSourceException("ZIP exception reading entry from zip stream",ze);
      } catch(IOException ioe){
        //rethrow
        throw new InvalidEventSourceException("I/O exception reading entry from zip stream",ioe);
      }
  }
  
  /**
   * Get a list of all valid XML file entries from a zip file
   * @param file the zip input file
   * @throws InvalidEventSourceException
   */
  private void readEntriesFromFile(ZipFile file) throws InvalidEventSourceException{
    
    //Seems to be a regular file
    try{
      //Get the zip file entries
      Enumeration<? extends ZipEntry> myenum = file.entries();
      //Loop over zip entries
      while(myenum.hasMoreElements()){
        //get the entry 
        ZipEntry next = (ZipEntry) myenum.nextElement();
          //If it is valid add it to our list fo entries
          if (checkZipEntry(next)) ZipEntries.add(next);
      }
    } catch(IllegalStateException ise){
      //rethrow
      throw new InvalidEventSourceException("Zip file was cleased while trying to read entries: "+file.getName());
    }
 }   
    /**
     * Read a particular event from a stream.
     * Since the file is read as stream, it's not possible to reference a
     * particular zip entry. However, we know the number of a zip entry
     * we want to get (eventIndex), so we seek (getNextEntry()) to the
     * zip entry of interest and then read it.
     * @return the event
     */
    private AEvent readEventFromStream() throws ReadEventException
    {
      //Things we need to close in the end
      ZipInputStream zis = null;
      AEvent event = null;
       
      try{
        //Check which kind of stream we have and open it   
        if (sourceType == SourceType.JAR_FILE) 
          zis = openJarStream(eventSource);
        else if (sourceType == SourceType.ZIP_NETWORK_FILE)
          zis = openNetworkStream(eventSource);
        else 
          throw new ReadEventException("Trying to read stream from neither jar file nor network file:" +eventSource);
    
        //Now seek through file to current entry
        ZipEntry entry = null;
        for(int i = 0; i <= eventIndex; i++)
          entry = zis.getNextEntry();
        
        //Check if we have reached EOF while skipping 
        if (entry == null)
          throw new ReadEventException("Reached EOF while skippeng to entry #"+eventIndex
              +" with name "+ZipEntries.get(eventIndex).getName()+ " in "+eventSource);
        
        //Next check if this is the entry we wanted to have
        if (! entry.getName().equals(ZipEntries.get(eventIndex).getName()))
          throw new ReadEventException("Content of zip stream changed since opening it:"+eventSource);
         
        //Finally read-in this entry
        event = AEventFromXML.read(zis, entry.getName());
      } catch (AAtlantisException ae) {
        throw new ReadEventException("Atlantis exception reading from: " + eventSource, ae);
      } catch (ZipException ze) {
        throw new ReadEventException("A ZIP format error has occurred reading entry " +
                ZipEntries.get(eventIndex).getName() + " from " + eventSource, ze);
      } catch (IllegalStateException ise) {
        throw new ReadEventException("The zip file has been closed while reading entry" +
                ZipEntries.get(eventIndex).getName() + " from " + eventSource, ise);
      } catch (IOException ioe) {
        throw new ReadEventException("I/O exception reading from: " + eventSource, ioe);

      } finally {
        try {
          //Always close the files in the end;
          if (zis != null) {
            zis.close();
          }
        } catch (Throwable t) {
          //Do nothing if closing the stream fails
          }
      }
        
      //Evertthing is fine, return the event
      return event;
    }
        
    /**
     * Read a particular event from a zip file
     * @param file the zip file
     * @return the event
     * @throws ReadEventException
     */
    private AEvent readEventFromFile() throws ReadEventException
    {
      ZipFile file = null;
      AEvent event = null;
      try {
        //Open the zip file
        file = new ZipFile(eventSource);
        //Get the input stream for the requested entry
        InputStream stream = file.getInputStream(ZipEntries.get(eventIndex));
        //Get the entry name
        String eventName = ZipEntries.get(eventIndex).getName();
        //Read the event
        event = AEventFromXML.read(stream,eventName);
      } catch (ZipException ze){
        throw new ReadEventException("A ZIP format error has occurred reading entry "+
           ZipEntries.get(eventIndex).getName() + " from "+ eventSource, ze);
      } catch (IllegalStateException ise){
        throw new ReadEventException("The zip file has been closed while reading entry"+
           ZipEntries.get(eventIndex).getName() + " from "+ eventSource, ise);
      } catch(IOException ioe){
        throw new ReadEventException("I/O error reading entry "+
           ZipEntries.get(eventIndex).getName() + " from "+ eventSource, ioe);      
      } finally {
        //Always close the input stream
        try {
          if(file != null) file.close();
        } catch (Throwable t) {
          //Ignore all errors here
        }
      }
      
      //Everything fine so we can return the event
      return event;
    }
 
    /**
     * Reads a random event from a zipped source
     * @return the event
     * @throws InvalidEventSourceException
     * @throws ReadEventException
     */
    protected AEvent randomEvent() throws InvalidEventSourceException,
                                     ReadEventException
    { 	
      //Check if we have a valid source
      if(( sourceType == null ) || ( eventSource==null ))
        throw new InvalidEventSourceException("No valid input source");
      
      //Now go to next event
      eventIndex = randomGenerator.nextInt(ZipEntries.size());

      //Now get this stream from its proper source
      switch (sourceType){
        case JAR_FILE: return readEventFromStream();
        case ZIP_NETWORK_FILE: return readEventFromStream();
        case ZIP_FILE: return readEventFromFile();
        default: throw new ReadEventException("Unknown event source type");
      }
    }

    /**
     * Read the next event from a zipped source
     * @return the event
     * @throws InvalidEventSourceException
     * @throws NoMoreEventsException
     * @throws ReadEventException
     */
    public AEvent nextEvent() throws InvalidEventSourceException,
                                     NoMoreEventsException,
                                     ReadEventException
    {
      // if random mode is selected, return a random event
      if(getNavigationMode() == NavigationMode.RANDOM) return randomEvent();

      //Check if we have a valid source
      if(( sourceType == null ) || ( eventSource==null ))
        throw new InvalidEventSourceException("No valid input source");

      // if we're in loop mode and at the last event, go to the first one
      if(getNavigationMode() == NavigationMode.LOOP && eventIndex >= ZipEntries.size() - 1)
        eventIndex = 0;
      // if we're not in loop mode and at the last event...
      else {
        // check that there are still some events
        if(eventIndex >= ZipEntries.size() - 1){
          String msg="Already at last event in archive";  
          // inform user
          AOutput.alwaysAppend("\n" + msg+"\n", ALogInterface.WARNING);
          throw new NoMoreEventsException(msg);
        }
        // go to next event.
        eventIndex++;
      }

      //Now get this stream from its proper source
      switch (sourceType){
        case JAR_FILE: return readEventFromStream();
        case ZIP_NETWORK_FILE: return readEventFromStream();
        case ZIP_FILE: return readEventFromFile();
        default: throw new ReadEventException("Unknown event source type");
      }
    }

    
    /**
     * Read the previous event from a zipped source
     * @return the event
     * @throws InvalidEventSourceException
     * @throws NoMoreEventsException
     * @throws ReadEventException
     */
    public AEvent previousEvent() throws InvalidEventSourceException,
                                        NoMoreEventsException,
                                        ReadEventException
    {

      // in random mode there's no looking back
      if(getNavigationMode() == NavigationMode.RANDOM) throw new NoMoreEventsException("Cannot go back in random mode");
    	
        //Check if we have a valid source
      if(( sourceType == null ) || ( eventSource==null ))
        throw new InvalidEventSourceException("No valid input source");

      // if we're in loop mode and we're at the first event, go to the last one
      if(getNavigationMode() == NavigationMode.LOOP && eventIndex == 0)
        eventIndex = ZipEntries.size() -1;
      // if we're not il loop mode and at the first event...
      else {
        // check that there are still some events
        if(eventIndex == 0){
          String msg="Already at first event in archive";  
          // inform user
          AOutput.alwaysAppend("\n" + msg+"\n", ALogInterface.WARNING);
          throw new NoMoreEventsException(msg);
        }
        // go to previous event.
        eventIndex--;
      }
      
      //Now get this stream from its proper source
      switch (sourceType){
        case JAR_FILE: return readEventFromStream();
        case ZIP_NETWORK_FILE: return readEventFromStream();
        case ZIP_FILE: return readEventFromFile();
        default: throw new ReadEventException("Unknown event source type");
      }
    }
}
