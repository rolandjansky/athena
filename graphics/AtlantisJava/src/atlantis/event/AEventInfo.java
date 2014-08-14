package atlantis.event;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Comparator;
import java.util.Date;

/**
 * The AEventInfo class summarizes the information that uniquely identifies a event.
 * It also provides an "equals" function allowing to compare if two events are identical.
 * @author sboeser
 */
public class AEventInfo {
  
  // number of the run of the event
  private final long runNumber;
  // event number in a run
  private final long eventNumber;
   // Athena time when XML event file was made in JiveXML
  private final String dateTime;
  // the name of the source this event was coming from (filename, servername, ...)
  private final String sourceName;
  // the lumiBlock number
  private final String lumiBlock;
  // Any additional eventProperties that may be important
  private final String eventProperties;
  // any additional info that is needed
  
  //the date format to use with the event info
  public final static SimpleDateFormat dateTimeFormat = new SimpleDateFormat("yyyy.MM.dd HH:mm:ss");

  /**
   * Fully qualified constructor
   * @param run the run number
   * @param event the event number
   * @param time the date and time of the event
   */
  AEventInfo(long event, long run, String time, String source, String lumiBlock, String eventProperties){
    this.runNumber = run;
    this.eventNumber = event;
    this.dateTime = time != null ? time : "n/a";
    this.sourceName = source != null ? source : "n/a";
    this.lumiBlock = checkString(lumiBlock);
    this.eventProperties = checkString(eventProperties);
  }
  
  /**
   * Copy constructor
   * @param the evenInfo object to copy
   */
  AEventInfo(AEventInfo event){
    this(event.getEventNumber(),event.getRunNumber(),
         event.getDateTime(),event.getSourceName(),
	 event.getLumiBlock(),event.getEventProperties());
  }
  
  /**
   * @return the run number
   */
  public long getRunNumber() {   
    return runNumber;
  } // getRunNumber() -----------------------------------------------------

  /**
   * @return the event number
   */
  public long getEventNumber(){
    return eventNumber;
  } // getEventNumber() ---------------------------------------------------


  /**
   * @return the date and time of the event
   */
  public String getDateTime() {
    return dateTime;
  } // getEventNumber() ---------------------------------------------------
  
  /**
   * @return the name of the event source
   */
  public String getSourceName() {
    return sourceName;
  } // getSourceName() ----------------------------------------------------------

  /**
   * @return the lumiBlock
   */
  public String getLumiBlock() {
    return lumiBlock;
  } // getSourceName() ----------------------------------------------------------

 /**
   * @return the eventProperties
   */
  public String getEventProperties() {
    return eventProperties;
  } // getSourceName() ----------------------------------------------------------



  /**
   * Performs test to see if there is a valid value for send string
   */
  private String checkString(String receivedString){
    String temp;
    if(receivedString == null || receivedString.endsWith("-1")){
	temp = "default";
    }else{
	temp = receivedString;
    }
    return temp;
  }
 



  /**
   * Check whether this event is identical to the event described by info
   * @param info the event information for the event to compare to
   * @return true if identical
   */
  // FIXME: Should take Object as argument. Need to consider how to deal with
  //        subclasses like AEvent.
  public boolean equals(AEventInfo info){
    //check if info is valid
    if (info == null) return false;
    //Only compare run- and event number and time, source may be different
    return ((info.getRunNumber() == runNumber)&&
            (info.getEventNumber() == eventNumber));
  }

// Need to override hashCode() to guarantee correct behaviour of equals()
@Override
public int hashCode() {
	final int prime = 31;
	int result = 1;
	result = prime * result + (int) (eventNumber ^ (eventNumber >>> 32));
	result = prime * result + (int) (runNumber ^ (runNumber >>> 32));
	return result;
}


/**
   * Implementation of Comparable interface - sort events by run- and event number
   */
  public static class RunEventNumberComparator implements Comparator {
 
   /**
    * @param o1 , o2 the two eventInfo objects to compare
    * @return negative/zero/positive int if o1 is less/equal/larger than o2
    */
    public int compare(Object o1, Object o2) {
   
      /**
       * Cast to an event info object - if this fails, someone has mixed this with
       * another class in a collection and we better crash with ClassCastException than catch
       */ 
      AEventInfo info1 = (AEventInfo)o1;
      AEventInfo info2 = (AEventInfo)o2;

      //compare by run number first
      Long run = info1.getRunNumber();
      if (run.compareTo(info2.getRunNumber()) != 0)
        return run.compareTo(info2.getRunNumber());

      //otherwise return by event number
      Long event = info2.getEventNumber();
      return event.compareTo(info2.getEventNumber());
    }
  }
  
  /**
   * Implementation of Comparable interface - sort events by date and time
   */
  public static class DateTimeComparator implements Comparator {
 
   /**
    * @param o1 , o2 the two eventInfo objects to compare
    * @return negative/zero/positive int if o1 is less/equal/larger than o2
    */
    public int compare(Object o1, Object o2) {
   
      /**
       * Cast to an event info object - if this fails, someone has mixed this with
       * another class in a collection and we better crash with ClassCastException than catch
       */ 
      AEventInfo info1 = (AEventInfo)o1;
      AEventInfo info2 = (AEventInfo)o2;
      
      //return zero for events that are equal
      if (info1.equals(info2)) return 0;      
      
      //Now get date and time objects
      Date date1 = null; Date date2=null;
      try {
        date1 = dateTimeFormat.parse(info1.getDateTime());
        date2 = dateTimeFormat.parse(info2.getDateTime());
      } catch (ParseException pex) {
        throw new ClassCastException("Parse exception when comparing dates");
      }
      
      //If there is not date assoicated with any of these, throw an exception
      if ((date1==null)||(date2==null))
        throw new ClassCastException("Could not parse valid date comparing "
                +info1.getDateTime()+" to "+info2.getDateTime());
      
      //Finally, compare the two dates
      return date1.compareTo(date2);
    }
  }
}
