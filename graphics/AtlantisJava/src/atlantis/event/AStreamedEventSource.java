package atlantis.event;

import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import java.util.Vector;

/**
 * This interface shall be implemented by all event sources that support event
 * streams. Only provides simple get and set methods.
 * @author sboeser
 */
public interface AStreamedEventSource extends AEventSource {

  /**
   * Exception that is thrown if the stream is invalid
   */
  public class InvalidStreamException extends AEventSourceException
  {
     //Default constructor
     public InvalidStreamException() { super(); }
     //Constructor with a string
     public InvalidStreamException( String message ){ super( message ); }
     //Constructor with a description and cause
     public InvalidStreamException( String message, Throwable cause ){ super( message, cause ); }
  }

  /**
   * Get an array of names of all available streams
   * @return the array of stream names
   * @throws InvalidEventSourceException if the list of streams can not be obtained
   */
  Vector<String> getAvailableStreams() throws InvalidEventSourceException;

  /**
   * Set the stream from which to provide events
   * @param streamName the stream from which to get events
   * @throws InvalidStreamException if the given name does not specify a valid stream
   * @throws InvalidEventSourceException if the server cannot be contacted to get the stream names
   */
  void setStream(String streamName) throws InvalidStreamException,
                                           InvalidEventSourceException;

  /**
   * Get the currently selected stream, or the default stream if none is selected
   * @return the name of the currently selected stream
   * @throws InvalidEventSourceException if the list of streams can not be obtained
   * @throws NoMoreEventsException if no stream is available
   *
   */
  String getStream() throws InvalidEventSourceException,
                            NoMoreEventsException;
}
