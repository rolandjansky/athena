package atlantis.event;

/**
 * Basic interface class to be implemented by all event sources 
 * @author sboeser
 */

public interface AEventSource
{

    /***
     * Exception that is thrown if an event can not be read from this source
     */
    public class ReadEventException extends AEventSourceException  
    {
       //Default constructor
       public ReadEventException() { super(); }
       //Constructor with a string
       public ReadEventException( String message ){ super( message ); }
       //Constructor with a description and cause 
       public ReadEventException( String message, Throwable cause ){ super( message, cause ); }
    }
    
    /***
     * Exception that is thrown if there are no more events from this source
     */
    public class NoMoreEventsException extends AEventSourceException  
    {
       //Default constructor
       public NoMoreEventsException() { super(); }
       //Constructor with a string
       public NoMoreEventsException( String message ){ super( message ); }
       //Constructor with a description and cause 
       public NoMoreEventsException( String message, Throwable cause ){ super( message, cause ); }
    }
    
    /**
     * Exception that is thrown if the event source is invalid 
     */
    public class InvalidEventSourceException extends AEventSourceException
    {
       //Default constructor
       public InvalidEventSourceException() { super(); }
       //Constructor with a string
       public InvalidEventSourceException( String message ){ super( message ); }
       //Constructor with a description and cause 
       public InvalidEventSourceException( String message, Throwable cause ){ super( message, cause ); }
    }
    
    /**
     * Read the next event from the source, throws NoMoreEvents
     * if no next event is available
     * @return the next event
     * @throws NoMoreEventsException 
     * @throws InvalidEventSourceException
     * @throws ReadEventException 
     */
    AEvent nextEvent() throws NoMoreEventsException,
                              InvalidEventSourceException,
                              ReadEventException;
    
    /**
     * Read the previous event from the source, throws NoMoreEvents
     * if no previous event is available
     * @return the previous event
     * @throws NoMoreEventsException
     * @throws InvalidEventSourceException
     * @throws ReadEventException 
     */
    AEvent previousEvent() throws NoMoreEventsException,
                                  InvalidEventSourceException,
                                  ReadEventException;
    /**
     * Get a string uniquly identifying the event source, e.g the URL, the zip
     * archive name or the server:port string
     * @return a unique name describing the source
     */
    String getSourceName();

    /***
     * Enum that stores the possible event navigation modes
     */
    public enum NavigationMode {
        SEQUENTIAL, //go through events in their natural order
        LOOP,       //start from beginning when reaching the end
        RANDOM,     //take a random event
        PUSH        //event loop is driven from outside (see AServerXMLRPC)
    }

    /**
     * Get the event navigation mode for the current source.
     * @return current navigation mode
     */
    NavigationMode getNavigationMode();

    /**
     * Set the event navigation mode for the current source.
     * Throws InvalidEventSourceException if the current
     * source does not support the requested mode
     * @param mode requested event navigation mode
     * @throws InvalidEventSourceException if the mode is not supported
     */
    void setNavigationMode(NavigationMode mode) throws InvalidEventSourceException;

    /**
     * Checks whether the current event source supports
     * a particular display mode.
     * @return true if the mode is supported
     * @param mode requested event navigation mode
     */
    boolean supportsNavigationMode(NavigationMode mode);

}
    
