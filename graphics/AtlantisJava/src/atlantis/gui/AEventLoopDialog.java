package atlantis.gui;

import java.util.Date;

import java.awt.Color;
import java.awt.Frame;
import java.awt.event.WindowListener;
import java.awt.event.WindowEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.BorderLayout;
import java.awt.Dimension;

import java.text.SimpleDateFormat;

import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.border.Border;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.Timer;

import atlantis.event.AEventManager;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.output.ALogInterface;
import atlantis.utils.ALogger;


/**
 * Read events in a loop using a given timer intervall
 * This is a singleton class using the getInstance pattern.
 *
 * @author Sebastian Boeser
 */
public class AEventLoopDialog extends JFrame
        implements WindowListener, ActionListener, ChangeListener
{
	// the logger 
  private static ALogger logger = ALogger.getLogger(AEventLoopDialog.class);

  private static final AGlobals globals = AGlobals.instance();

  //create singleton instance
  private static AEventLoopDialog instance;
	
    // some GUI attributes
    private static final String TITLE = "Event loop";
    private static final Integer SPINNER_INIT = new Integer(4);
    private static final Integer SPINNER_MIN = new Integer(1);
    private static final Integer SPINNER_MAX = new Integer(120);
    private static final Integer SPINNER_STEP = new Integer(2);
    
    // We don't need more than one button
    private static JButton button = null;

    //The update time in seconds
    private static JSpinner updateIntervalSpinner = null;
    //A logging pane
    private static ALogPane log = null;
 
    // The update timer
    private static final Timer timer = new Timer(SPINNER_INIT*1000,null);
   

    private AEventLoopDialog()
    {
      //Create dialog with title
      super(TITLE);
      
      //Construct GUI
      createGUI();
  
    } // AReadEventFromServerDialog() ---------------------------------------

    /**
     * @return the singleton instance
     */
    public static AEventLoopDialog getInstance(){
      if (instance == null) instance = new AEventLoopDialog();
      return instance;
    } // getInstance() ------------------------------------------------------
    
    
    /**
     * Non-static set functions so user is forced to obtain an instance first
     * @param UpdateInterval the update intervall
     */
    public void setUpdateInterval(int UpdateInterval) {
      try {
        updateIntervalSpinner.setValue(UpdateInterval);
      } catch (IllegalArgumentException iae){
        //Just ignore in case of an invalid argument
      }
    }
        
   /**
   * Design of the dialog - panels, buttons, etc
   */
   private void createGUI() {

    //Definde default component gap
    final int defaultGap = 5;
    final Border defaultBorder = BorderFactory.createEmptyBorder(5, 10, 5, 10);

    /**
     * Looping panel
     */
    //Create a spinner for the update time
    SpinnerNumberModel numberModel = new SpinnerNumberModel(SPINNER_INIT, SPINNER_MIN, SPINNER_MAX, SPINNER_STEP);
    updateIntervalSpinner = new JSpinner(numberModel);
    updateIntervalSpinner.addChangeListener(this);

    // looping panel
    JPanel loopingPanel = new JPanel();
    loopingPanel.setLayout(new BorderLayout(defaultGap, 0));
    loopingPanel.setBorder(defaultBorder);
    loopingPanel.add(updateIntervalSpinner, BorderLayout.CENTER);
    loopingPanel.add(new JLabel("[secs]"), BorderLayout.EAST);


    /**
     * Main action buttons
     */
    // action button panel
    JPanel buttonsPanel = new JPanel();
    buttonsPanel.setLayout(new BorderLayout(0, 0));
    buttonsPanel.setBorder(defaultBorder);
    button = new JButton("Start");
    //Set a preferred size so it doesn't change if we change its label
    button.setPreferredSize(new Dimension(100,25));
    buttonsPanel.add(button, BorderLayout.CENTER);
    //React when button is pressed
    button.addActionListener(this);

    //the top row panel
    JPanel topRowPanel = new JPanel();
    topRowPanel.setLayout(new BorderLayout(defaultGap, 0));
    topRowPanel.add(loopingPanel, BorderLayout.CENTER);
    topRowPanel.add(buttonsPanel, BorderLayout.EAST);

 

    /**
     * Logging panel
     */
    // the logging panel at the bottom
    JPanel logPanel = new JPanel();
    logPanel.setLayout(new BorderLayout(0, 0));
    logPanel.setBorder(defaultBorder);
    //Add a logpane to it
    log = new ALogPane();
    log.setEnabled(true);
    log.setBorder(BorderFactory.createLineBorder(Color.black));
    log.setRequestFocusEnabled(false);
    log.setPreferredSize(new Dimension(350, 200));
    logPanel.add(log, BorderLayout.CENTER);


    // main panel
    JPanel mainPanel = new JPanel();
    mainPanel.setLayout(new BorderLayout(0, 0));
    // logPanel must be CENTER to be resisable vertically (otherwise,
    // it keeps its size), hence upper part must be extra panel for
    // NORTH position to accommodate topRowPanel and loopingPanel
    JPanel upperMainPanel = new JPanel();
    upperMainPanel.setLayout(new BorderLayout(0, 0));
    upperMainPanel.add(topRowPanel, BorderLayout.NORTH);
    mainPanel.add(upperMainPanel, BorderLayout.NORTH);
    mainPanel.add(logPanel, BorderLayout.CENTER);

    // Finally add this MainPanel
    getContentPane().add(mainPanel, BorderLayout.CENTER);

    //arrange all GUI items
    pack();

    //Set location to top left GUI corner so it does not hide Canvas
    setLocation(globals.getGuiFrame().getLocation());

    //give it a nice icon when minimized
    AIcon.setIconImage(this);


    /**
     *  end of dialog design, set listeners and handlers
     */    //add ourselfs as state listeners to catch closing
    addWindowListener(this);

    //also set ourselfs as action handler for the timer
    timer.addActionListener(this);
    //And have the timer start immediately on the first event
    timer.setInitialDelay(0);



  } // createGUI() --------------------------------------------------------
  
  /**
   * State change events are only generated by the updateIntervalSpinner
   * @param e the Change event
   */
  public void stateChanged(ChangeEvent e) {
    //Set timer delay to new value
    timer.setDelay((Integer)updateIntervalSpinner.getValue()*1000);
  }

  /**
   * Stop event loop when closing the window
   * @param e the Window event
   */  
  public void windowClosing(WindowEvent e){
    
    //Make sure the timer is not running anymore
    if (timer.isRunning()) stopEventLoop();
  
  } // closeActionsAndDispose() ------------------------------------------

  /**
   * Small helper to print a message with timestamp to the log
   * @param msg the message
   */
  private void timeStampedLog(String msg){
   
    //Record the time of the incident
    SimpleDateFormat dateForm = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    log.append("\n"+dateForm.format(new Date())+" ",ALogInterface.NORMAL_BOLD);
    log.append(msg);
  }
  
  
  /**
   * Try to read the next event from the server, handle all exceptions
   */
  private void getNextEvent() {
    
    //try reading the next event
    try{
      //tell the user
      timeStampedLog("Reading next event...");
      //Then get next event
      AEventManager.instance().nextEvent();
    } catch (NoMoreEventsException ex) {
      //Just wait for next event
      log.append("\n -- No next event available");
      logger.info("No next event available:\n"+ex.getCauseMessages());
    } catch (InvalidEventSourceException ex) {
      //Do not break loop on invalid event source -- good for ACR
      log.append("\n -- Invalid event source");
      logger.warn("Invalid event source:\n"+ex.getCauseMessages());
    } catch (ReadEventException ex) {
      //Don't break loop on invalid events
      log.append("\n -- Received invalid event");
      logger.warn("Received invalid event:\n"+ex.getCauseMessages());
    }
  }
  
  /**
   * Stop the event loop, reenabling all GUI elements
   */
  private void stopEventLoop(){
  
    //Stop the timer loop
    timer.stop();
    
    //Reset button label and states
    button.setText("Start");
    updateIntervalSpinner.setEnabled(true);
  
    //tell the user
    timeStampedLog("Stopped event loop...");
  }
  
  /**
   * Start the event loop, disable all GUI elements - is public to facilitate
   * automated start of event loop (e.g with command-line options)
   */
  public void startEventLoop(){
   
    //tell the user
    timeStampedLog("Starting event loop...");
 
    //enable/disable buttons
    updateIntervalSpinner.setEnabled(false);
    button.setText("Stop");
  
    //Stop the timer loop
    timer.start();
   
  }
  
  /**
   * Called every time the button is hit or we get a callback from the timer
   * @param e the action event
   */
  public void actionPerformed(ActionEvent e) {
    
    //Check if we got called from the timer loop
    if (e.getSource() == timer){
      //If so just read next event and return
      getNextEvent();
      return;
    }
    
    //Being called because button was hit.
    //Check if event loop is running - stop it
    if (timer.isRunning()) {
      stopEventLoop();
    } else { 
      startEventLoop();
    }
  }
  

  /**
   * Show the dialog
   */
  public void showDialog() {

    // clear the iconified bit (no matter whether or not it is iconified)
    setExtendedState(getExtendedState() & ~Frame.ICONIFIED);
    setVisible(true);

  } // show() ----------------------------------------------------

  
  /**
   * Empty default implementation of WindowListeners
   * @param e
   */
  public void windowOpened(WindowEvent e) {}
  public void windowClosed(WindowEvent e) {}
  public void windowIconified(WindowEvent e) {}
  public void windowDeiconified(WindowEvent e) {}
  public void windowActivated(WindowEvent e) {}
  public void windowDeactivated(WindowEvent e) {}

}
