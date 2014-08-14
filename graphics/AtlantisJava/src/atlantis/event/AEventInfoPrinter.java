package atlantis.event;

import atlantis.globals.AGlobals;
import atlantis.output.ALogInterface;

/**
 * Print event information for each new event to the log pane
 * @author sboeser
 */
public class AEventInfoPrinter implements ANewEventListener {
  
  private ALogInterface logPane = null;
  
  /**
   * Constructor with the log pane where the information shall be printed
   * @param output the output pane to which to write
   */
  public AEventInfoPrinter(ALogInterface output){
    logPane = output;
  }

  private void printEventInfo(AEvent event, ALogInterface dest)
  {
	//For Minerva just output name and coded event and run numbers
	//Not on canvas title, but teachers may want to see the number to help
	//with recognising difficult events 
	if(AGlobals.instance().getSimpleOutput()>0)
	 {
	  String r = "\n" + event.getSourceName() + 
	  " (" + event.getRunNumber() + "00"+ event.getEventNumber() + ")\n";
	  dest.append(r + "\n", ALogInterface.NORMAL);
	  return;
    } 

    int lineLen = 39;
    String r = "\n" + event.getSourceName() + "\n" +
    "run number: " + event.getRunNumber() +
    " event number: " + event.getEventNumber() + "\n" +
    "--------------------------------------\n";
    
    String[][] s = event.getInfo();
    
    for(int i = 0; i < s.length; i++)
    {
    r += String.format("%-"+lineLen+"s", s[i][0]) + ": " + s[i][1] + "\n";
    }
    dest.append(r + "\n", ALogInterface.NORMAL);
    
    } // printEventInfo() ---------------------------------------------------*/

  /**
   * For each new event print log information to the log pane
   * @param event the change event
   */
  public void newEvent(AEvent event) {
    //simply print it
    printEventInfo(event,logPane);
  }

}
