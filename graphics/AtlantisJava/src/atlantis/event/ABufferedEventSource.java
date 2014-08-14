package atlantis.event;

import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.ALogger;
import java.util.Vector;

/**
 * Extension class to AEventSource providing buffering for a fixed number of events.
 * @author sboeser
 */
public abstract class ABufferedEventSource implements AEventSource {

  //Logging interface
  private final static ALogger logger = ALogger.getLogger(ABufferedEventSource.class);
  
  //The index of the currently selected event in the list
  private static int currentEvent = -1;
  //The maximum number of events to save
  public static int maxNumberOfEvents = 1;
  //A vector of available events
  private static Vector<AEvent> eventContainer = new Vector<AEvent>();
  // the current event navigation mode
  private NavigationMode eventNavigationMode = NavigationMode.SEQUENTIAL;

  /**
   * Abstract read next event method to be implemented by all subclasses
   * @return the next event
   * @throws NoMoreEventsException
   * @throws InvalidEventSourceException
   * @throws ReadEventException
   */
  protected abstract AEvent readNext(AEventInfo currentEvent) throws NoMoreEventsException,
                                           InvalidEventSourceException,
                                           ReadEventException;
  /**
   * Abstract read previous event method to be implemented by all subclasses
   * @return the previous event
   * @throws NoMoreEventsException
   * @throws InvalidEventSourceException
   * @throws ReadEventException
   */
  protected abstract AEvent readPrevious(AEventInfo currentEvent) throws NoMoreEventsException,
                                               InvalidEventSourceException,
                                               ReadEventException;

  /**
   * Get the next event, either from the buildin buffer or by reading it in
   * @return the next event
   * @throws atlantis.event.AEventSource.NoMoreEventsException
   * @throws atlantis.event.AEventSource.InvalidEventSourceException
   * @throws atlantis.event.AEventSource.ReadEventException
   */
  public AEvent nextEvent() throws NoMoreEventsException,
                                   InvalidEventSourceException,
                                   ReadEventException
  {
    //Check if we are already at the last event
    if (currentEvent == eventContainer.size()-1){
      
      //if so, try to get another event
      AEvent event = null;
      try{
         event = readNext(getCurrentEventInfo());
      } catch ( OutOfMemoryError oom ) {
         
        //throw an error message
        logger.error("Ran out of memory while reading event data");
        
        //See if we can clear memory by removing events
        if ( getNumberOfEvents() > 0) {
          //be verbose
          logger.info("Clearing event cache and retry...");
          //remove events
          clearEventContainer();
          //Call garbage collector to free resources
          System.gc();
          //Now retry (no current event)
          event = readNext(null);
        }
      }
         
      //Add it to the eventContainer as last one
      addEvent(event,false);
    
    } else {
      // simply increase event counter
      setCurrentEvent(currentEvent+1);
    } 
 
    //Now return the new current event
    return eventContainer.get(currentEvent);
  }

  public AEvent previousEvent() throws NoMoreEventsException,
                                       InvalidEventSourceException,
                                       ReadEventException {
   
    //Check if we are already at the first event
    if (currentEvent <= 0 ){
                 
      //if so, try to get another event
      AEvent event = null;
      try{
         event = readPrevious(getCurrentEventInfo());
      } catch ( OutOfMemoryError oom ) {
         
        //throw an error message
        logger.error("Ran out of memory while reading event data");
        
        //See if we can clear memory by removing events
        if ( getNumberOfEvents() > 0) {
          //be verbose
          logger.info("Clearing event cache and retry...");
          //remove events
          clearEventContainer();
          //Call garbage collector to free resources
          System.gc();
          //Now retry (no current event)
          event = readPrevious(null);
        }
      }

      //Add it to the eventContainer as first one
      addEvent(event,true);

    } else {
      // simply decrease event counter
      setCurrentEvent(currentEvent-1);
    } 

    //Now return the new current event
    return eventContainer.get(currentEvent);

  }

  /**
   * Get the event navigation mode for the current source.
   * @return current event navigation mode
   */
  public NavigationMode getNavigationMode() {
    return eventNavigationMode;
  }

  /**
   * Set the event navigation mode for the current source,
   * clears the buffer and reads the first event.
   * Throws InvalidEventSourceException if the current
   * source does not support the requested mode
   * @param mode requested event navigation mode
   */
  public void setNavigationMode(NavigationMode mode) throws InvalidEventSourceException
  {
    if(supportsNavigationMode(mode)) {
      NavigationMode oldMode = getNavigationMode();
      eventNavigationMode = mode;

      // empty the buffer
      clearEventContainer();
    } else
      throw new InvalidEventSourceException("Mode '"+mode.name()+"' not supported by current source");
  }

    /**
     * Checks whether the current event source supports
     * a particular display mode.
     * @return true if the mode is supported
     * @param mode requested event navigation mode
     */
    public abstract boolean supportsNavigationMode(NavigationMode mode);
 
    /**
     * Returns number of events saved in the eventContainer
     * @return int
     */
    public int getNumberOfEvents()
    {
        return eventContainer != null ? eventContainer.size() : 0;

    }

        /**
     * Add the given even to the event container. By default, events are added
     * to the end of the container. If skipping backward, it may be necessary to
     * add to the beginning of the container, as indicated by the asFirst flag
     * The new event will also be set as default event
     * 
     * @param event the event to add
     * @param asFirst whether to add the event at the beginning (default is end)
     */
    private synchronized void addEvent(AEvent event, boolean asFirst)
    {
       //create event container if it does not exist
       if(eventContainer == null) eventContainer = new Vector<AEvent>();
  
       //Make sure container does not get too large
       while (eventContainer.size() >= maxNumberOfEvents){
          // remove at the side at which we are not adding event
           if (asFirst) eventContainer.remove(eventContainer.lastElement());
           else eventContainer.remove(0);
       }
       
       //At the end or at the beginning
       int index = (asFirst) ? 0 : eventContainer.size();
       
       //add the event       
       eventContainer.add(index,event);
       
       //be verbose
       logger.debug(eventContainer.size() + " event(s) in memory");
    
       //set as current event
       setCurrentEvent(index);
   }

  
      /**
     * Clear the event container, reset the current event
     */
    protected synchronized void clearEventContainer()
    {
        //Make sure the event container exits before clearing it
        if(eventContainer != null){
           // let the user know
          String msg = "Clearing event container with "+getNumberOfEvents()+" events.";
          logger.warn(msg);
          AOutput.append(msg, ALogInterface.WARNING);
          
          //Clear the container
          eventContainer.clear();
        }
        
        //Alse reset current event
        currentEvent = -1;
        
        //Run the garbage collector to free space immediately
        System.gc();

    }

  private void setCurrentEvent(int index) {
    //Check for out-of-bounds
    if ((index < 0) || ( index >= eventContainer.size()))
      throw new ArrayIndexOutOfBoundsException("Cannot set index "+index+" in event container of size "+eventContainer.size());
    currentEvent = index;
  }

  /**
   * @return the curent event or null if there is none
   */
  private AEventInfo getCurrentEventInfo(){
   
    //Check if the current event is there
    try {
      //get the current events name
      return eventContainer.get(currentEvent);
    } catch (ArrayIndexOutOfBoundsException ex){
      //return an empty string if there is no current event
      return null;
    }
  }
  
}
