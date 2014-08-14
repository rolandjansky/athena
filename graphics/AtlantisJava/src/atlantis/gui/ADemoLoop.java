package atlantis.gui;

import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.util.ArrayList;

import atlantis.canvas.ACanvas;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.globals.AGlobals;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjectionsManager;
import atlantis.parameters.ACommandProcessor;
import atlantis.utils.ALogger;
import java.awt.event.ActionListener;
import java.util.Iterator;
import java.util.Vector;
import javax.swing.JOptionPane;

import javax.swing.Timer;

/**
 * A class to automatically loop over different projections given in a
 * colon-seperated list. The special projection name "NE" stands for next event.
 * This is also used in headless mode to generate an event loop. The stopOnNoMoreEvents
 * allows to control wether the loop ends if no more events can be obtained.
 *
 * NOTE: The previous implementation of this was using java.awt.Thread - we are
 *       far from providing the standards of a thread-safe application.
 *      
 * @author Sebastian Boeser
 */
public class ADemoLoop extends Timer implements ActionListener
{
  //The logger
  private static ALogger logger = ALogger.getLogger(ADemoLoop.class);

  //whether to stop the loop if there are no more events
  private boolean stopOnNoMoreEvents = false;
  
  //A list of projections to show
  private final ArrayList<String> projectionList = new ArrayList<String>();
  
  //An iterator that loops through the list above
  private Iterator<String> projIter = null;

  private static final AGlobals globals = AGlobals.instance();

  /**
   * Fully specified Constructor
   * @param sleepTime time to sleep inbetween projections in milliseconds
   * @param projectionList list of projections to loop through
   * @param stopOnNoMoreEvents wether to stop loop if there are no more events
   */
  public ADemoLoop(int sleepTime, Vector<String> projectionList, boolean stopOnNoMoreEvents){

    //Initialize the timer
    super(sleepTime,null);
    addActionListener(this);

    //Initialize the timer to start immediately only on first event
    setInitialDelay(0);

    //Store the list of projections
    for (String projName : projectionList)
      this.projectionList.add(projName);

    //Set the stop on no more events flag
    this.stopOnNoMoreEvents = stopOnNoMoreEvents;

  }


  /***
   * prepare the demo loop and start the timer thread
   */
  public void startDemoLoop(){
	AGlobals.instance().setDemoMode(true);
    
    //be verbose
    logger.trace("Starting demo loop with interval "+Integer.toString(getDelay())+
                 " ms and projections "+projectionList.toString());

    //Generate the list iterator to point to the start of the list
    projIter = projectionList.listIterator(0);

    //Finally start the loop
    start();
  }

  /**
   * stop the demo loop
   */
  public void stopDemoLoop(){
    stop();
	globals.setDemoMode(true);
  }

  /**
   * This gets fired by the timer every time after it returns from sleep
   * @param event the action event
   */
  public void actionPerformed(ActionEvent event) {

    //If we have reached the end, start from beginning
    if (! projIter.hasNext()) projIter = projectionList.listIterator(0);

    //proceed to next element in list
    String projName = projIter.next();

    //add debugging output
    logger.trace("Next in demo loop: "+projName);

    //Check if this is a next event tag
    if (projName.equals("NE")) {
      //Change the event or stop loop if this did not succeed
      if (!changeEvent()) {
        //stop the loop
        stop();
        //notify everyone that we are done (so the application can finish)
        synchronized(this) { notifyAll(); }
      }

      //Immediatly go to the next projection if this is not just a nextEvent again
      Iterator<String> nextProjIter = projIter;
      if (nextProjIter.hasNext() && (!nextProjIter.next().equals("NE"))) restart();
      //be verbose
      logger.trace("Timer is"+(isRunning()?" running":" stopped")+" with "+getDelay()+" ms delay");
    }
    //else just change the projection
    else changeProjection(projName);

  }
 

  /**
   * Change the current projection to the one given by projName
   * and select that as active projeciton in the GUI
   * @param projName the name of the projection
   */
  private void changeProjection(String projName)
  {
       //First get the projection object
       AProjection p = AProjectionsManager.getProjection(projName);
       //And set the projection
       ACanvas.getCanvas().getCurrentWindow().setProjection(p);
       //Make sure aspect ratio is always okay
       if(projName.equals("YX")||projName.equals("RZ")||projName.equals("XZ")||projName.equals("YZ"))
          ((AProjection2D) p).setAspectRatio1(ACanvas.getCanvas().getCurrentWindow());
       //Finally Change the current GUI selection by signaling the command processor
       ACommandProcessor.receive(projName + ".");
  }

  /**
   * Switch to the next event
   * @return if the return value is true, the loop will be continued, otherwise not
   */
  private boolean changeEvent(){
	Frame gui = globals.getGuiFrame();
    //try reading the next event
    try{
      //Then get next event
      AEventManager eventManager = AEventManager.instance();
      eventManager.nextEvent();
      return true;
    } catch (NoMoreEventsException ex) {
      //Just wait for next event
      logger.info("No next event for demo - "+((stopOnNoMoreEvents)?"stopping loop":"keeping same"));
      //return wether loop shall be stopped due to this or not.
      return (!stopOnNoMoreEvents);
    } catch (InvalidEventSourceException ex) {
      //log warning
      logger.warn("Invalid event source for demo - "
                  +((stopOnNoMoreEvents)?"stopping loop":"will try again")
                  +"\n"+ex.getCauseMessages());
      //quit here in headless mode
      if (AGlobals.isAtlantisHeadless()) return (!stopOnNoMoreEvents);
      //Otherwise give a user warning
      JOptionPane.showMessageDialog(gui,
        "Invalid event source for demo - stopping loop",
        "Warning", JOptionPane.WARNING_MESSAGE);
      //Always quit the loop if not in headless mode
      return false;
    } catch (ReadEventException ex) {
      //log warning
      logger.warn("Received invalid event - stopping loop:\n"+ex.getCauseMessages());
      //and tell user unless we are in headless
      if (AGlobals.isAtlantisHeadless()) return false;
      //in headless mode, don't show a dialog
       JOptionPane.showMessageDialog(gui,
         "Received invalid event for demo - stopping loop",
         "Warning", JOptionPane.WARNING_MESSAGE);
      return false;
    }
  }
}

