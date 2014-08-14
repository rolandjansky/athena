package atlantis.event;

/**
 * Interface for all NewEventSourceListener classes. Each time the event source 
 * is changed, the newEventSourcemethod is called.
 * @author maillard
 */
public interface ANewEventSourceListener {

  /**
   * This method gets called each time the layout is changed.
   * @param eventSource the new event source
   */
  abstract public void newEventSource(AEventSource eventSource);  

}
