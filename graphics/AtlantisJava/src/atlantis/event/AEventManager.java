package atlantis.event;

import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.event.AEventSource.NavigationMode;
import atlantis.event.oncrpc.AONCRPCEventSource;
import atlantis.event.xmlrpc.AXMLRPCEventSource;
import java.util.Vector;

import atlantis.utils.ALogger;

/**
 * All new physics events come into Atlantis via the AEventManager
 * The AEventManager may hold a list of event, has the index of the
 * current event int the list and informs its listeners about new events.
 * Only one event can be active at one time.
 * 
 * @author sboeser
 */
public class AEventManager
{
    private final static ALogger logger = ALogger.getLogger(AEventManager.class);
    private static AEventManager instance;
    
    /** Private constructor for singleton. */
    private AEventManager() {}
    
    /** Get the singleton instance. */
    public synchronized static AEventManager instance() {
    	if (instance==null) instance = new AEventManager();
    	return instance;
    }
    
    //The current event source
    private AEventSource eventSource = null;
    
    //The current event - i.e. the last delivered by any event source
    private AEvent currentEvent = null;
 
    //A list of listeners for new events
    private Vector<ANewEventListener> NewEventListeners = new Vector<ANewEventListener>();
    
    //A list of listeners for new events sources
    private Vector<ANewEventSourceListener> NewEventSourceListeners = new Vector<ANewEventSourceListener>();
    
    /**
     * Return the currently active event
     * @return AEvent
     */
    public synchronized AEvent getCurrentEvent()
    {
        //simply return the current event
        return currentEvent;
    }


    /**
     * Set the event with the given index as the current event
     * @param index the index of the new event
     */
    private synchronized void setCurrentEvent(AEvent event)
    {
        //Now simply set the current event
        currentEvent = event;
        //Inform all listeners
        fireNewEvent(getCurrentEvent());
    }
      

    /**
     * @return the current active event source
     */
    public synchronized AEventSource getEventSource(){
      return eventSource;
    }
    
    /**
     * Set a new already constructed event source. Note that this constructor
     * does not throw InvalidEventSource exception as the AEventSource object
     * is already constructed. 
     * @param source the AEventSource object
     */
    public synchronized void setEventSource( AEventSource source )
    {      
      //Simply set the source and we are done
      eventSource = source;
      fireNewEventSource(getEventSource()); 
    }

    
    /**
     * Set the source given by name as new event source. If the event source is
     * not valid (e.g invalid file name) we rely on the constructor to fail, throwing
     * an InvalidEventSourceException. Thus, the old source is retained if the new one
     * can not be created.
     * @param sourceName the name of the source (e.g. file name, url, server name,...)
     * @throws InvalidEventSourceException
     */
    public synchronized void setEventSource(String sourceName) throws InvalidEventSourceException
    {
      //Make a nice string out of it
      String theSourceName = sourceName.toLowerCase().trim();
      
      // XMLRPC Server source given as "xmlrpc://server:port"
      if(theSourceName.startsWith("xmlrpc://")) {
        eventSource = new AXMLRPCEventSource(sourceName);
      // ONCRPC Server source given as "oncrpc://server[:port]"
      } else if(theSourceName.startsWith("oncrpc://")) {
        eventSource = new AONCRPCEventSource(sourceName);
      // Metwork sources possibilities
      // .xml, .zip or web directory containing event files
      } else if(theSourceName.startsWith("http://")) {
        // normal xml file
        if(theSourceName.endsWith(".xml")){
          eventSource = new AURLEventSource(sourceName);
        // zip file on the web
        } else if(theSourceName.endsWith(".zip")) {
           eventSource = new AZipEventSource(sourceName);
        //Anything else should be a web directory
        } else {
          eventSource = new AURLEventSource(sourceName);
        }

      // local disk access possibilities (.xml, .zip, .gz, .gzip)
      // event source starts with file://
      } else if(theSourceName.startsWith("file://")) {
        // normal xml file
        if (theSourceName.endsWith(".xml")) {
          eventSource = new AFileEventSource(sourceName);
        // normal zip file
        } else if (theSourceName.endsWith(".zip")) {
          eventSource = new AZipEventSource(sourceName);
        // compressed single xml file
        } else if (theSourceName.endsWith(".gz") || theSourceName.endsWith(".gzip")) {
          eventSource = new AFileEventSource(sourceName);
        } else {
          String msg = "Could not identify event source: "+sourceName;
          logger.error(msg);
          throw new InvalidEventSourceException(msg);
        }
      } else {
          String msg = "Could not identify event source: "+sourceName;
          logger.error(msg);
          throw new InvalidEventSourceException(msg);
      }
      fireNewEventSource(getEventSource()); 
    }

    /**
     * Sets event navigation mode: random, push, loop or sequential.
     * Throws InvalidEventSourceException if there is no source
     * or if it doesn't support the requested mode.
     * @throws InvalidEventSourceException
     */
    public synchronized void setNavigationMode(NavigationMode mode) throws InvalidEventSourceException
    {
      if(eventSource == null) throw new InvalidEventSourceException("Current event source is NULL");

      NavigationMode oldMode = eventSource.getNavigationMode();

      // check if we're really setting a new mode, or if
      // the user just clicked on the same button twice
      if(oldMode != mode) {
        if(eventSource.supportsNavigationMode(mode)) {
          eventSource.setNavigationMode(mode);
          fireNewEventSource(getEventSource());
          // go to the next event
          try {
            nextEvent();
          } catch (NoMoreEventsException nme) {
            String msg = "No more events from current source";
            logger.error(msg);
          } catch (ReadEventException re) {
            String msg = "Error while reading the event";
            logger.error(msg);
          }
        } else throw new InvalidEventSourceException("Current event source does not support the selected display mode");
      }
    }

    /**
     * Gets the current event navigation mode.
     * @return event navigation mode
     */
    public synchronized NavigationMode getNavigationMode() throws InvalidEventSourceException
    {
      if(eventSource == null) throw new InvalidEventSourceException("Current event source is NULL");
      return eventSource.getNavigationMode();
    }

    /**
     * Check if the current source supports a specific event navigation mode.
     * @param mode the requested event navigation mode
     * @return true if the event navigation mode is supported
     */
    public boolean supportseventNavigationMode(NavigationMode mode) throws InvalidEventSourceException
    {
      if(eventSource == null) throw new InvalidEventSourceException("Current event source is NULL");
      return eventSource.supportsNavigationMode(mode);
    }

    /**
     * Read the next event from the current event source.
     * Throws NoMoreEvents if there is none.
     * @throws NoMoreEventsException
     * @throws InvalidEventSourceException
     * @throws ReadEventException
     */
    public void nextEvent() throws NoMoreEventsException,
                                   InvalidEventSourceException,
                                   ReadEventException
    {

    	// Check for valid event source.
      if (eventSource==null) 
        throw new InvalidEventSourceException("Current event source is NULL");
      
      // Simply read next event from it.
      AEvent event = eventSource.nextEvent();
      setCurrentEvent(event);
   
    }


    /**
     * Read previous event from current event source.
     * Throws NoMoreEvents if there is none.
     * @throws NoMoreEventsException
     * @throws InvalidEventSourceException
     * @throws ReadEventException 
     */
    public void previousEvent() throws NoMoreEventsException,
                                       InvalidEventSourceException,
                                       ReadEventException
    {
      
      // Check for valid event source.
      if (eventSource==null) 
        throw new InvalidEventSourceException("Current event source is NULL");
      
      // Simply read previous event from it. 
      setCurrentEvent(eventSource.previousEvent());
       
    } 
    
    /**
     * Add a new listener for newEvent incidents
     * @param listener to be added to list
     */
    public void addNewEventListener(ANewEventListener listener)
    {
      //Add this listener
      NewEventListeners.addElement(listener);
      //If there are already some events
      if (getCurrentEvent() == null) return;
      //Make the new listener aware of the current event
      listener.newEvent(getCurrentEvent());
    }

    /**
     * Remove a listener from the list.
     * @param listener to be removed from list
     */
    public void removeNewEventListener(ANewEventListener listener)
    {
      //Remove this listener
      NewEventListeners.removeElement(listener);
    }

    /**
     * Call stateChanges of all newEvent listeners
     * @param event the new event passed on to all listeners
     */
    private synchronized void fireNewEvent(AEvent event)
    {
      
      // Loop over all listeners
      for ( ANewEventListener listener : NewEventListeners )
         // give them the new event
         listener.newEvent(event);
    }

    /**
     * Add a new listener for newEventSource incidents
     * @param listener to be added to list
     */
    public void addNewEventSourceListener(ANewEventSourceListener listener)
    {
      //Add this listener
      NewEventSourceListeners.addElement(listener);
      //If there are already some events
      if (getEventSource() == null) return;
      //Make the new listener aware of the current event
      listener.newEventSource(getEventSource());
    }

    /**
     * Remove a listener from the list
     * @param listener to be removed from list
     */
    public void removeNewEventSourceListener(ANewEventSourceListener listener)
    {
      NewEventSourceListeners.removeElement(listener);
    }

    /**
     * Call eventSourceChanged of all event source changes listeners
     * @param eventSource the new event source
     */
    private synchronized void fireNewEventSource(AEventSource eventSource)
    {
      // Loop over all listeners
      for ( ANewEventSourceListener listener : NewEventSourceListeners )
         // give them the new event
         listener.newEventSource(eventSource);
    }

}
