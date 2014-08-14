package atlantis.event;

import atlantis.data.AEventFromXML;
import java.io.File;
import java.io.FilenameFilter;
import java.io.InputStream;

import java.util.Arrays;
import java.util.zip.GZIPInputStream;
import java.util.Random;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;


/**
 * Read a single event from an XML file in a directory
 * @author sboeser
 */
public class AFileEventSource extends ABufferedEventSource
{

    //The current directory
    private File directory = null;
    
    //A filter for valid files in the current directory
    private final XMLFilenameFilter filter = new XMLFilenameFilter();

    // needed for random mode
    private Random randomGenerator = new Random();

    /**
     * Constructor with file name given
     * @param sourceName
     * @throws AEventSource.InvalidEventSourceException
     */
    public AFileEventSource(String sourceName) throws AEventSource.InvalidEventSourceException
    {
      // check if it's a well-formed event path
      if(sourceName.startsWith("file://")) {
        // strip file:// from the sourceName
        sourceName = sourceName.substring(7);
        //Check if the current file name is valid
        directory = new File(sourceName);
   
        //Check that we can read that file or directory
        if (!directory.canRead())
          throw new InvalidEventSourceException("Can not read from "+sourceName);
      } else throw new InvalidEventSourceException(sourceName+" is not in the form file://path");
    }

    /**
     * @return the full name of the file or directory
     */
    public String getSourceName() {
      return directory.getAbsolutePath();
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
     * Read an event from the current file and directory
     * @return the event
     * @throws ReadEventException 
     * @throws InvalidEventSourceException
     */
    protected AEvent readEventFromFile(File file) throws ReadEventException,
                                             InvalidEventSourceException
    {
      //Reassemble full file name
      String FullFileName = file.getAbsolutePath();
      
      try{
        
        //Use full path and name to specify the file
        InputStream is = new FileInputStream(FullFileName);

        //Check if this might be a gzip stream
        if ((file.getName().toLowerCase().endsWith(".gz"))||
           (file.getName().toLowerCase().endsWith(".gzip"))){
            
          //Run through GZIPInput for decoding
          is = new GZIPInputStream(is);
        }
        //Read file from stream
        return AEventFromXML.read(is, file.getName());
       
      } catch( OutOfMemoryError oom ) {
       
        //Check if we can clear the event container
        if( getNumberOfEvents() > 0) clearEventContainer();
    
        //Run garbage collector
        System.gc();
      
        //Retry reading the event
        return readEventFromFile(file);
       
      } catch(FileNotFoundException fnfe){
        //rethrow
        throw new InvalidEventSourceException("File not found: "+FullFileName,fnfe);
      } catch(IOException ioe){
        //rethrow
        throw new ReadEventException("I/O error readding file: "+FullFileName,ioe);
      }
    }
    
    /**
     *Get a sorted list of files in the directory of the current file
     */
    private String[] getListOfFiles(){
    
      //get a list of files in the same directory using the filter
      String[] FileList = directory.list(filter);
       
      //Sort the list
      Arrays.sort(FileList);
      
      return FileList; 
   
    }

    protected AEvent readRandom() throws ReadEventException, InvalidEventSourceException {

      //The file we want to pass on to the reading routine
      File file = null;
      //On the first encounter "directory" might be a file
      if (directory.isFile()){
        //set this as the file we want to read
        file = directory.getAbsoluteFile();
        //store its parent directory
        directory = file.getParentFile();
      } else {
        //Get a list of files
        String[] FileList = getListOfFiles();
        //Check there are more files
        if (FileList.length == 0) 
          throw new InvalidEventSourceException("No valid files in directory "
                                           +directory.getAbsolutePath());
        //Loop over file list
        int iName = randomGenerator.nextInt(FileList.length);
        
        //Now get the new file
        file = new File(directory,FileList[iName]);
      }
   
      //Should now have a file in any case, try to read from it
      return readEventFromFile(file);

    }


    /**
     * Read the next event from the next XML file in the same directory
     * @return the next event
     * @throws NoMoreEventsException
     * @throws ReadEventException
     * @throws InvalidEventSourceException 
     */
    public AEvent readNext(AEventInfo currentEvent) throws NoMoreEventsException,
                                    ReadEventException,
                                    InvalidEventSourceException
    { 
      // if random mode is set, return a random event
      if(getNavigationMode() == NavigationMode.RANDOM) return readRandom();

      //The file we want to pass on to the reading routine
      File file = null;
      //On the first encounter "directory" might be a file
      if (directory.isFile()){
        //set this as the file we want to read
        file = directory.getAbsoluteFile();
        //store its parent directory
        directory = file.getParentFile();
      } else {
        //Get a list of files
        String[] FileList = getListOfFiles();
        //Check there are more files
        if (FileList.length == 0) 
          throw new NoMoreEventsException("No valid files in directory "
                                           +directory.getAbsolutePath());
        //Loop over file list
        int iName = 0;
        //search for next entry that is lexicographically larger
        //taking first one if there is no current event to compare to
        while ((currentEvent != null ) &&
               (currentEvent.getSourceName().compareTo(FileList[iName]) >= 0)){
          //Go to next file
          ++iName;
          // if we have reached the end of the list
          if (iName == FileList.length) {
            // if in loop mode, go back to the first file
            if(getNavigationMode() == NavigationMode.LOOP) {
              iName = 0;
              break;
            }
            // if not in loop mode, abort
            else throw new NoMoreEventsException("No more files after"
                + currentEvent.getSourceName() + " in directory "+directory.getAbsolutePath());
          }
        }
        
        //Now get the new file
        file = new File(directory,FileList[iName]);
      }
   
      //Should now have a file in any case, try to read from it
      return readEventFromFile(file);

    } 
    
    /**
     * Read the next event from the previous XML file in the same directory
     * @return the previous event
     * @throws NoMoreEventsException
     * @throws ReadEventException
     * @throws InvalidEventSourceException 
     */
    public AEvent readPrevious(AEventInfo currentEvent) throws NoMoreEventsException,
                                        ReadEventException,
                                        InvalidEventSourceException
    {
      // in random mode there's no looking back
      if(getNavigationMode() == NavigationMode.RANDOM) throw new NoMoreEventsException("No more random events left in the buffer");
      //The file we want to pass on to the reading routine
      File file = null;
      //On the first encounter "directory" might be a file
      if (directory.isFile()){
        //set this as the file we want to read
        file = directory.getAbsoluteFile();
        //store its parent directory
        directory = file.getParentFile();
      } else {
        //Get a list fo files
        String[] FileList = getListOfFiles();
        //Check there are more files
        if (FileList.length == 0) 
          throw new NoMoreEventsException("No valid files in directory "
                                           +directory.getAbsolutePath());
        //Loop over file list
        int iName = FileList.length-1;
        //search for next entry that is lexicographically smaller
        //taking last one if there is no current one to compare to
        while ((currentEvent != null) &&
               (currentEvent.getSourceName().compareTo(FileList[iName]) <= 0)){
          //Go to next file
          --iName;
          // if we have reached the end of the list
          if (iName < 0) {
            // if in loop mode, go to the last event
            if(getNavigationMode() == NavigationMode.LOOP) {
              iName = FileList.length - 1;
              break;
            }
            // if not in loop mode, abort
            else throw new NoMoreEventsException("No more files before"
                + currentEvent.getSourceName() + " in directory "+directory.getName());
          }
        }
          
        //Now get the new file
        file = new File(directory,FileList[iName]);
      }
   
      //Should now have a file in any case, try to read from it
      return readEventFromFile(file);
  }
}
/**
 * Filter helper class only accepting file names ending in .xml, .gz or .gzip
 * @author sboeser
 */
class XMLFilenameFilter implements FilenameFilter
{

    // accept all readable .xml .gz and .gzip files, no directories
    public boolean accept(File dir, String name)
    {
        //First check the name
        if ( ! name.toLowerCase().endsWith(".xml"))
          if ( ! name.toLowerCase().endsWith(".gzip"))
           if ( ! name.toLowerCase().endsWith(".gz"))
             //invalid ending
             return false;
        
        //Check file properties
        File f = new File(dir, name);
        
        //Reject directories
        if(! f.isFile()) return false;
        //Reject unreadable files
        if(! f.canRead()) return false;

        //Finally accept
        return true;

    }
}
