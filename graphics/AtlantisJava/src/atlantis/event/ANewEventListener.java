
package atlantis.event;

/**
 * Interface for all NewEventListener classes. Each time a new event is 
 * brought to the event manager, the NewEvent:isteners newEvent method is
 * called with the new event as an argument
 * @author sboeser
 */
public interface ANewEventListener {

  /**
   * This method gets called each time a new new is available.
   * @param event the new event
   */
  abstract public void newEvent(AEvent event);  

}