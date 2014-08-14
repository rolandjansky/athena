package atlantis.event;

import atlantis.data.AEventFromXML;

import java.util.ArrayList;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.URL;
import java.net.MalformedURLException;

import java.util.regex.Pattern;
import java.util.regex.Matcher;
import java.util.Random;

import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.ALogger;

import java.util.Collections;


/**
 * This class obtains an event from an XML file which is read from
 * URL address (from a remote source).
 * 
 * Once the file is read the classes tries to create an internal
 * list of XML event files from this remote source which is utilised
 * when the user accesses next, resp. previous event from this address.
 * 
 * It is dependent on the fact that the web server supports directory
 * listings - i.e. it returns an HTML page which this class parses
 * and gets all files conforming ".*\.xml" regular expression.
 * 
 * It is possible to specify only URL with a directory where the event
 * files are.
 * 
 * @author sboeser
 */
public class AURLEventSource extends ABufferedEventSource
{
    // get the logger
    private static final ALogger logger = ALogger.getLogger(AURLEventSource.class);   
    // the name of the event source
    private String directoryURL = null;
    
    // needed for random mode
    private Random randomGenerator = new Random();

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
     * Constructor with the URL of the directory
     * @param sourceName
     * @throws atlantis.event.AEventSource.InvalidEventSourceException
     */
    public AURLEventSource(String sourceName) throws InvalidEventSourceException
    {   
      
      //Make sure the event source starts with http://
      if( ! sourceName.toLowerCase().startsWith("http://"))
        throw new InvalidEventSourceException("URL event source name does"+
                                              "not start with \"http://\"");
      
      //Store event source
      directoryURL = sourceName;
    }
    
    /**
     * @return the directory URL string
     */
    public String getSourceName() {
      return directoryURL;
    }
    
    /**
     * Get a list of files in a directory given by a URL
     * @param url the url of the directory
     * @return a list of urls to the individual files
     * @throws atlantis.event.AEventSource.InvalidEventSourceException
     */
    private ArrayList<String> getEventListForURL(String url) throws InvalidEventSourceException {

    //the url of the directory we are searching  
    String baseUrl = url;
    //trim any file endings from url
    if (url.endsWith(".xml"))
      baseUrl = url.substring(0, url.lastIndexOf('/'));
    
    
    //The input stream - need to be closed in the end
    InputStreamReader urlStream = null;
    BufferedReader br = null;

    //be a bit verbose
    logger.info("Trying to get directory listing from " + baseUrl);

    //Create an array to return
    ArrayList<String> eventList = new ArrayList<String>();

    //Now try reading stuff from the web
    try {
      //Get an input stream reader
      urlStream = new InputStreamReader(new URL(baseUrl).openStream());
      //get a buffered reader for that stream
      br = new BufferedReader(urlStream);

      // input is the whole line from HTML directory listing
      // example to match: <a href="atlantis_event_70417_93.xml">
      // We use regular expressions patterns to catch the lines of interest
      // ( ) defines group, group 0 is the whole match and group 1 is
      // just the desired file name within the parentheses 
      Pattern pattern = Pattern.compile("<a href=\"(.*\\.xml)\">");

      //Loop over all lines
      String line = null;
      while ((line = br.readLine()) != null) {
        //try to match the patter
        Matcher matcher = pattern.matcher(line);
        if (matcher.find()) // not .match() (!)
        {
          //Get the first group from the pattern
          //i.e the file name and add to our event list
          eventList.add(matcher.group(1));
        }
      }
 
      //be a bit verbose
      logger.debug("List of " + eventList.size() + " events loaded from " + baseUrl);


    } catch (MalformedURLException mue) {
      String msg = "Incorrect URL address: " + baseUrl;
      logger.error(msg);
      AOutput.alwaysAppend("\n" + msg + "\n", ALogInterface.WARNING);
      throw new InvalidEventSourceException(msg, mue);
    } catch (IOException ioe) {
      String msg = "IO error while reading URL: " + baseUrl;
      logger.error(msg);
      AOutput.alwaysAppend("\n" + msg + "\n", ALogInterface.WARNING);
      throw new InvalidEventSourceException(msg, ioe);
    } finally {
      //In any case close everything
      try {
        br.close();
        urlStream.close();
      } catch (Throwable t) {
        //do nothing here
      }
    }

   //check if there were at least some events
   if (eventList.size()==0)
     throw new InvalidEventSourceException("No events found at "+baseUrl);

    //In the end sort the event list
    Collections.sort(eventList);

    //and return it
    return eventList;

  }
    
    /**
     * Read in an event from a fully specified path
     * @param fileName the URL to the file
     * @param directoryURL the name of the event source
     * @return the event
     */
    private AEvent readEventFromURL(String fileName, String directoryURL)
          throws ReadEventException {
    //Things we need to close in the end
    InputStream urlStream = null;
    BufferedInputStream buffStream = null;
    AEvent event = null;

    try {
      //Open the url as stream
      String fullURL = directoryURL + "/" + fileName;
      URL eventURL = new URL(fullURL);
      urlStream = eventURL.openStream();
      buffStream = new BufferedInputStream(urlStream);
      //then read the event from there
      logger.debug("Reading event data stream from " + eventURL.toString() + "...");
      event = AEventFromXML.read(buffStream, fileName);

    } catch (FileNotFoundException fnfe) {
      //Show as a warning and rethrow
      String msg = "File not found at " + directoryURL+"/"+fileName;
      logger.error(msg);
      AOutput.alwaysAppend("\n" + msg + "\n", ALogInterface.WARNING);
      throw new ReadEventException(msg, fnfe);
    } catch (IOException ioe) {
      //Show as an error and rethrow
      String msg = "IO error when reading " + directoryURL;
      logger.error(msg);
      AOutput.alwaysAppend("\n" + msg + "\n", ALogInterface.WARNING);
      throw new ReadEventException(msg, ioe);
    } finally {
      try {
        if (urlStream != null) {
          urlStream.close();
        }
        if (buffStream != null) {
          buffStream.close();
        }
      } catch (Throwable t) {
        //Ignore all errors
      }
    }
    return event;
  }
    
    /**
     * Read a random event
     * @return the random event
     * @throws InvalidEventSourceException
     * @throws ReadEventException
     */
    protected AEvent readRandom() throws InvalidEventSourceException, ReadEventException
    {
      
      String fileName = null;
    
      //Check if we have been given a fully specified file url
      if(directoryURL.endsWith(".xml")){       
        //Store the file name from the url
        fileName = directoryURL.substring(directoryURL.lastIndexOf('/')+1,directoryURL.length());
        //trim file endings from url
        directoryURL = directoryURL.substring(0, directoryURL.lastIndexOf('/'));
             
      } else {
     
        //Update event list every time, so we get new files
        ArrayList<String> eventList=getEventListForURL(directoryURL);
      
        //Check there are more files
        if (eventList.size() == 0) 
          throw new InvalidEventSourceException("No valid files at "+directoryURL);
        
        //Pick a random event
        int iName = randomGenerator.nextInt(eventList.size());
        
        //Now get the new file
        fileName = eventList.get(iName);
      }
      
      //Now read exactly that event and return it
      return readEventFromURL(fileName,directoryURL);   
    }
    

    /**
     * Read the previous event from the URL directory
     * @return the previous event
     * @throws NoMoreEventsException
     * @throws InvalidEventSourceException
     * @throws ReadEventException
     */
    public AEvent readNext(AEventInfo currentEvent) throws NoMoreEventsException,
                                                           InvalidEventSourceException,
                                                           ReadEventException
    {
      // if random mode is set, return a random event
      if(getNavigationMode() == NavigationMode.RANDOM) return readRandom();

      String fileName = null;
    
      //Check if we have been given a fully specified file url
      if(directoryURL.endsWith(".xml")){       
        //Store the file name from the url
        fileName = directoryURL.substring(directoryURL.lastIndexOf('/')+1,directoryURL.length());
        //trim file endings from url
        directoryURL = directoryURL.substring(0, directoryURL.lastIndexOf('/'));
             
      } else {
     
        //Update event list every time, so we get new files
        ArrayList<String> eventList=getEventListForURL(directoryURL);
      
        //Check there are more files
        if (eventList.size() == 0) 
          throw new NoMoreEventsException("No valid files at "+directoryURL);
        //Loop over file list
        int iName = 0;          
        //search for next entry that is lexicographically larger
        //taking first one if there is no current one to compare to
        while ((currentEvent != null) && 
               (currentEvent.getSourceName().compareTo(eventList.get(iName)) >= 0)){
          //Go to next file
          ++iName;
          // if we have reached the end of the list...
          if (iName == eventList.size())
            // if in loop mode go to the first event
            if(getNavigationMode() == NavigationMode.LOOP) {
              iName = 0;
              break;
            }
            // if not in loop mode, abort
            else throw new NoMoreEventsException("No more files after "
                + currentEvent.getSourceName() + " in directory "+directoryURL);
        }
        
        //Now get the new file
        fileName = eventList.get(iName);
      }
      
      //Now read exactly that event and return it
      return readEventFromURL(fileName,directoryURL);   
    }
    
    /**
     * Read the previous event from the current URL directory
     * @return the previous event
     * @throws NoMoreEventsException
     * @throws InvalidEventSourceException
     * @throws ReadEventException
     */
    public AEvent readPrevious(AEventInfo currentEvent) throws NoMoreEventsException,
                                                             InvalidEventSourceException,
                                                             ReadEventException
    {
      // in random mode there's no looking back
      if(getNavigationMode() == NavigationMode.RANDOM) throw new NoMoreEventsException("No more random events left in the buffer");

      String fileName = null;
    
      //Check if we have been given a fully specified file url
      if(directoryURL.endsWith(".xml")){       
        //Store the file name from the url
        fileName = directoryURL.substring(directoryURL.lastIndexOf('/')+1,directoryURL.length());
        //trim file endings from url
        directoryURL = directoryURL.substring(0, directoryURL.lastIndexOf('/'));
             
      } else {
     
        //Update event list every time, so we get new files
        ArrayList<String> eventList=getEventListForURL(directoryURL);
      
        //Check there are more files
        if (eventList.size() == 0) 
          throw new NoMoreEventsException("No valid files at "+directoryURL);
        //Loop over file list
        int iName = eventList.size()-1;
        //search from the end for the first entry that is lexicographically smaller
        //taking last one if there is no previous one to compare to
        while ((currentEvent != null) &&
               (currentEvent.getSourceName().compareTo(eventList.get(iName)) <= 0)){
          //Go to next file
          --iName;
          // if we have reached the end of the list...
          if (iName < 0)
            // if in loop mode, go to the last file
            if(getNavigationMode() == NavigationMode.LOOP) {
              iName = eventList.size()-1;
              break;
            }
            // if not in loop mode, abort
            else throw new NoMoreEventsException("No more files before "
                + currentEvent.getSourceName() + " in directory "+directoryURL);
        }
        
        //Now get the new file
        fileName = eventList.get(iName);
      }
      
      //Now read exactly that event and return it
      return readEventFromURL(fileName,directoryURL);
    }

} // class AURLEventSource
