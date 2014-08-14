
package atlantis.event;

/**
 * All exceptions occuring while attempting to read events shall be chaught
 * and converted into one of the three generic event source exceptions:
 * 
 *  -NoMoreEventsException: there is no next/previous event
 *  -InvalidEventSourceException: the specified event source is invalid
 *  -ReadEventException: there was an error reading in the event
 * 
 * All three of them derive from AEventSourceException which provides printing methods.
 * 
 * These three exceptions are then handed up to the GUI level, where they are treated 
 * differently, depending on the current reading mode (e.g. NoMoreEvents can be ignored
 * when waiting for events to appear on a server, web-dir, etc..)
 * 
 * @author sboeser
 */
public class AEventSourceException extends Exception{
  
    //Default constructor
    public AEventSourceException() { super(); }
    //Constructor with a string
    public AEventSourceException( String message ){ super( message ); }
    //Constructor with a description and cause 
    public AEventSourceException( String message, Throwable cause ){ super( message, cause ); }
    
    /**
     * Generates a string with the cause of this message and all causing ones
     * @return the generated string
     */
    public String getCauseMessages(){
      
      //Create the string buffer
      StringBuffer msg = new StringBuffer();
      
      //Add all cause messages starting from this exception
      addCauseMessage(this,msg);
      
      //return string 
      return msg.toString();
    }
    
    /**
     * Recursively add cause messages to string buffer
     * @param t the exception
     * @param msg the string buffer
     */
    private void addCauseMessage(Throwable t, StringBuffer msg){
      
      //Add the name of the exception class
      msg.append(t.getClass().getName());
      //Add the description of the exception
      msg.append(": ");
      msg.append(t.getMessage());
      
      //Check if there has been a cause given
      if (t.getCause() != null){
        //Add cause messages in next line
        msg.append("\n caused by ");
        addCauseMessage(t.getCause(),msg);
      }
   }
}
