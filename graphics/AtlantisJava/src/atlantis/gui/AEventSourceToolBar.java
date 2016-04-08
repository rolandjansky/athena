
package atlantis.gui;

import atlantis.config.ADefaultValues;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.event.AEventSource;
import atlantis.event.ANewEventSourceListener;
import atlantis.event.AURLEventSource;
import atlantis.event.AEventSource.InvalidEventSourceException;
import atlantis.event.AEventSource.NavigationMode;
import atlantis.event.AEventSource.NoMoreEventsException;
import atlantis.event.AEventSource.ReadEventException;
import atlantis.event.AStreamedEventSource;
import atlantis.event.AStreamedEventSource.InvalidStreamException;
import atlantis.globals.AGlobals;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Vector;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.DefaultComboBoxModel;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.JRadioButton;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

/**
 * This toolbar contains all the elements to set a new event source or stream,
 * browse through the events or set the event navigation mode
 * @author maillard
 * @author sboeser
 */
class AEventSourceToolBar extends JToolBar implements ANewEventSourceListener {

  /** The text field that allows us to enter a new event source */
  private JTextField addressBar;

  /** The event stream selection combo box */
  private JComboBox eventStreamComboBox;

  /** The radio buttons that allow to select the event navigation mode */
  private JRadioButton randomButton, sequentialButton, loopButton;
  
  /** The logger */
  private final static ALogger logger = ALogger.getLogger(AEventSourceToolBar.class);

  private static AEventManager eventManager = AEventManager.instance();
  
  /** Spacing between components in the toolbar */
  private static int SPACE = 4;

  /**
   * Constructor
   */
  public AEventSourceToolBar() {

    //call parent constructor
    super();

    setFloatable(false);

    //Get the path to the icon files
    String iconPath = AGlobals.instance().getHomeDirectory()+"img"+System.getProperty("file.separator");

    // setting up the 'open file' button
    ImageIcon openIcon = AUtilities.getFileAsImageIcon(iconPath+"toolbar_open.png");
    JButton openFile = new JButton(openIcon);
    openFile.setVerticalTextPosition(JButton.BOTTOM);
    openFile.setHorizontalTextPosition(JButton.CENTER);
    openFile.setMargin(new Insets(2, 2, 2, 2));
    openFile.setToolTipText("Open local file");
    // And add an action listener for it
    openFile.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        openFileButtonAction();
      }
    });
    // adding the open file button to the toolbar

    add(Box.createHorizontalStrut(SPACE));
    add(openFile);
    add(Box.createHorizontalStrut(SPACE));

    // setting up the address bar
    addressBar = new JTextField(30);
    // and an action listener
    addressBar.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        addressBarAction((JTextField)e.getSource());
      }
    });
    // adding the address bar to the toolbar
    add(addressBar);

    //Set up previous and  next buttons
    ImageIcon nextIcon = AUtilities.getFileAsImageIcon(iconPath+"toolbar_next.png");
    ImageIcon previousIcon = AUtilities.getFileAsImageIcon(iconPath+"toolbar_previous.png");

    JButton nextButton = new JButton(nextIcon);
    JButton previousButton = new JButton(previousIcon);
    
    //customize and add action listener for next
    nextButton.setName("nextButton");
    nextButton.setVerticalTextPosition(JButton.BOTTOM);
    nextButton.setHorizontalTextPosition(JButton.CENTER);
    nextButton.setMargin(new Insets(2, 2, 2, 2));
    nextButton.setToolTipText("Next event (PageDown)");
    nextButton.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e){
        nextButtonAction();
      }
    });

    //customize and add action listener for previous
    previousButton.setName("previousButton");
    previousButton.setVerticalTextPosition(JButton.BOTTOM);
    previousButton.setHorizontalTextPosition(JButton.CENTER);
    previousButton.setMargin(new Insets(2, 2, 2, 2));
    previousButton.setToolTipText("Previous event (PageUp)");
    previousButton.addActionListener(new ActionListener(){
      public void actionPerformed(ActionEvent e){
        previousButtonAction();
      }
    });
    //Add both to toolbar
    add(Box.createHorizontalStrut(SPACE));
    add(previousButton);
    add(nextButton);
    add(Box.createHorizontalStrut(SPACE));

    //Next add a seperator
    addSeparator();

    //Add a combo box, but set it to invisble
    String[] streamNames = { "--n/a--" };
    eventStreamComboBox = new JComboBox(streamNames);
    eventStreamComboBox.setVisible(false);
    add(eventStreamComboBox);

    // set up the navigation mode button icons
    ImageIcon sequentialIcon = AUtilities.getFileAsImageIcon(iconPath+"mode_sequential.png");
    ImageIcon loopIcon = AUtilities.getFileAsImageIcon(iconPath+"mode_loop.png");
    ImageIcon randomIcon = AUtilities.getFileAsImageIcon(iconPath+"mode_random.png");
    ImageIcon sequentialIconOn = AUtilities.getFileAsImageIcon(iconPath+"mode_sequential_on.png");
    ImageIcon loopIconOn = AUtilities.getFileAsImageIcon(iconPath+"mode_loop_on.png");
    ImageIcon randomIconOn = AUtilities.getFileAsImageIcon(iconPath+"mode_random_on.png");

    //put these buttons in a group
    ButtonGroup group = new ButtonGroup();
    // sequential mode
    sequentialButton = new JRadioButton(sequentialIcon);
    sequentialButton.setOpaque(false);
    sequentialButton.setSelectedIcon(sequentialIconOn);
    sequentialButton.setToolTipText("Sequential mode");
    sequentialButton.setName("Sequential mode");
    // loop mode
    loopButton = new JRadioButton(loopIcon);
    loopButton.setSelectedIcon(loopIconOn);
    loopButton.setOpaque(false);
    loopButton.setToolTipText("Loop mode");
    loopButton.setName("Loop mode");
    // random mode
    randomButton = new JRadioButton(randomIcon);
    randomButton.setOpaque(false);
    randomButton.setSelectedIcon(randomIconOn);
    randomButton.setToolTipText("Random mode");
    randomButton.setName("Random mode");
    // the mode buttons are initially disabled, since we
    // don't know which event source will be loaded first
    sequentialButton.setEnabled(false);
    loopButton.setEnabled(false);
    randomButton.setEnabled(false);

    // when one of the buttons is pressed, set the required display mode
    sequentialButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        navigationModeAction(NavigationMode.SEQUENTIAL);
      }
    });

    loopButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        navigationModeAction(NavigationMode.LOOP);
      }
    });

    randomButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        navigationModeAction(NavigationMode.RANDOM);
      }
    });
    //Add all buttons to the group
    group.add(sequentialButton);
    group.add(loopButton);
    group.add(randomButton);

    add(sequentialButton);
    add(loopButton);
    add(randomButton);
  }
  
  /**
   * Called whenever the event text field changed
   */
  private synchronized void addressBarAction(JTextField tf){

    //Get the new text
    String sourceName = tf.getText();

    // if sourceName is a local path but doesn't provide an [type]://,
    // prepend file:// as default
    if(! sourceName.contains("://")) sourceName = "file://" + sourceName;
    try {
      eventManager.setEventSource(sourceName);
      eventManager.nextEvent();
    } catch (NoMoreEventsException nme) {
      JOptionPane.showMessageDialog(this, "No more events found from this source!", "No more events", JOptionPane.ERROR_MESSAGE);
      logger.warn(nme.getCauseMessages());
    } catch (InvalidEventSourceException ies) {
      JOptionPane.showMessageDialog(this, "Not a valid event source: "+sourceName, "Invalid event source", JOptionPane.ERROR_MESSAGE);
      logger.error(ies.getCauseMessages());
    } catch (ReadEventException ree ){
      JOptionPane.showMessageDialog(this, "Cannot read event(s).", "Read error", JOptionPane.ERROR_MESSAGE);
      logger.warn(ree.getCauseMessages());
    }
  }

  /**
   * Called whenever the previous button is pressed
   */
  private synchronized void previousButtonAction(){
	  
    try {
      eventManager.previousEvent();
    } catch (NoMoreEventsException nme) {
      String msg = "Already at first event from this source!";
      JOptionPane.showMessageDialog(this, msg, "No more events", JOptionPane.ERROR_MESSAGE);
      logger.warn(nme.getCauseMessages());
    } catch (InvalidEventSourceException ies ){
      String msg = "Not a valid event source - please select one!";
      JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
      logger.warn(ies.getCauseMessages());
    } catch (ReadEventException ree ){
      String msg = "Can not read event(s)!";
      JOptionPane.showMessageDialog(this, msg, "Read error", JOptionPane.ERROR_MESSAGE);
      logger.warn(ree.getCauseMessages());
    }
	refreshAddressBar();
  }

  /**
   * Called whenever the next button is pressed
   */
  private synchronized void nextButtonAction(){
    try {
      eventManager.nextEvent();
    } catch (NoMoreEventsException nme) {
      String msg = "Already at last event from this source!";
      JOptionPane.showMessageDialog(this, msg, "No more events", JOptionPane.ERROR_MESSAGE);
      logger.warn(nme.getCauseMessages());
    } catch (InvalidEventSourceException ies ){
      String msg = "Not a valid event source - please select one!";
      JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
      logger.warn(ies.getCauseMessages());
    } catch (ReadEventException ree ){
      String msg = "Can not read event(s)!";
      JOptionPane.showMessageDialog(this, msg, "Read error", JOptionPane.ERROR_MESSAGE);
      logger.warn(ree.getCauseMessages());
    }
	refreshAddressBar();
  }
  
  /**
   * Update the address bar to show the current event source
   */
  private synchronized void refreshAddressBar(){  
      AEventSource source = eventManager.getEventSource();
      AEvent event = eventManager.getCurrentEvent();
      if (source != null && event != null) {
          // if the source is a URL, we should always use '/' as the separator
          // else we'll use the system file separator
          if (source instanceof AURLEventSource) {
              addressBar.setText(source.getSourceName() + "/" + event.getSourceName());
          } else {
              addressBar.setText(source.getSourceName() + System.getProperty("file.separator") + event.getSourceName());
          }
      }
  }

  /**
   * Open a file dialog and read the selected event or directory
   */
  private synchronized void openFileButtonAction(){
    //get the last location from the default values
    String lastLoc = ADefaultValues.get("LastEventFilesSourceDir");
    //Create a file chooser dialog
    AReadFileChooser fileDialog = new AReadFileChooser(lastLoc);
    //Check if the user pressed ok in the dialog
    if ( fileDialog.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
      //Get the selected file path, prepend file://
      String fullFilePath = "file://" + fileDialog.getSelectedFile().getPath();

      try {
        //Now set this as a new event source
        eventManager.setEventSource(fullFilePath);
        eventManager.nextEvent();
      } catch (NoMoreEventsException nme) {
        String msg = "No events found from this source!";
        JOptionPane.showMessageDialog(this, msg, "No more events", JOptionPane.ERROR_MESSAGE);
        logger.warn(nme.getCauseMessages());
      } catch (InvalidEventSourceException ies ){
        String msg = "Not a valid event source";
        JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
        logger.warn(ies.getCauseMessages());
      } catch (ReadEventException ree ){
        String msg = "Can not read event(s)";
        JOptionPane.showMessageDialog(this, msg, "Read error", JOptionPane.ERROR_MESSAGE);
        logger.warn(ree.getCauseMessages());
      }
    } // okay pressed - otherwise do nothing
  } // openFileButtonAction() ----------------------------------------------------

  /**
   * Called whenever the navigation mode is changed in the GUI
   */
  private synchronized void navigationModeAction( NavigationMode mode ){
    try {
      eventManager.setNavigationMode(mode); }
    catch (InvalidEventSourceException ies) {
      JOptionPane.showMessageDialog(this, ies.getCauseMessages(), "Invalid navigation mode", JOptionPane.ERROR_MESSAGE);
    }
  }

  /**
   * Called whenever the event stream is changed in the GUI
   */
  private synchronized void selectedStreamAction(AStreamedEventSource source){

    //Get the newly selected stream
    String stream = (String) eventStreamComboBox.getSelectedItem();
    
    try {
      //Try to set a new stream
      source.setStream(stream);
      //Also get a new event
      eventManager.nextEvent();
      //And update the event source field
      refreshAddressBar();

    } catch (NoMoreEventsException nme){
      //In case we already have the latest event from this stream - ignore
    } catch (InvalidStreamException ise) {
      //This can happen if the stream list changes inbetween
      //opening the popup and selecting an item
      String msg = "Cannot set stream '"+stream+"' for event source";
      JOptionPane.showMessageDialog(this, msg, "Invalid stream", JOptionPane.ERROR_MESSAGE);
      logger.error(msg+"\n"+ise.getCauseMessages());
      //Also clear the stream list
      eventStreamComboBox.removeAllItems();
    } catch (InvalidEventSourceException ies) {
      //This can happen if the source turned invalid inbetween
      //opening the popup and selecting an item
      String msg = "Invalid event source when setting new stream";
      JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
      logger.error(msg+"\n"+ies.getCauseMessages());
      //Also clear the stream list
      eventStreamComboBox.removeAllItems();
    } catch (ReadEventException ree){
      //Do nothing special with stream list in this case
      String msg = "Can not read event from new stream '"+stream+"'";
      JOptionPane.showMessageDialog(this, msg, "Invalid event", JOptionPane.ERROR_MESSAGE);
      logger.error(msg+"\n"+ree.getCauseMessages());
    }

  }

  /**
   * This is called whenever the event stream combo is poped up
   * Use this to update the event list every time
   */
  private synchronized void updateEventStreamCombo(AStreamedEventSource eventSource){
    /**
     * NOTE: We are NOT using "removeAllItems" + "addItems" to update the
     *       combo box as this will always fire a new ActionEvent with the first
     *       added Item. The proper way to do is to change the data model, as the
     *       new data model will only fire AFTER the user selected from the open
     *       ComboBox.
     */

    //Create an empty list
    Vector<String> streamNames = new Vector<String>();

    try {
      //Get the stream names
      streamNames = eventSource.getAvailableStreams();
    } catch (InvalidEventSourceException ies) {
      //If we fail to update the list show some messages
      //NOTE: we can not show dialog here, as this will steal the focus
      //      from the combo box, so it does not pop-down later on
      String msg = "Invalid event source when getting stream list";
      //JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
      logger.error(msg+"\n"+ies.getCauseMessages());
    }

    //In any case update the stream list by setting a new data model
    // -- even if it is empty
    finally {
      //Create a new data model
      DefaultComboBoxModel newDataModel = new DefaultComboBoxModel(streamNames);
      //Select the same stream we had before, if its still there
      newDataModel.setSelectedItem(eventStreamComboBox.getSelectedItem());
      //Now install the new DataModel
      eventStreamComboBox.setModel(newDataModel);
    }
  }

  /**
   * This implementation of the ANewEventSourceListener gets called whenever
   * the event source changes in AEventManager
   * @param eventSource the new event source (final as being bound by combo action listener)
   */
  public synchronized void newEventSource(final AEventSource eventSource) {

    //Stream selection is off by default
    eventStreamComboBox.setVisible(false);

    //Check if this event source supports streams
    if (eventSource instanceof AStreamedEventSource){
      
      AStreamedEventSource streamedSource = (AStreamedEventSource) eventSource;

      //Update the list in the event stream combo box
      updateEventStreamCombo(streamedSource);

      //Temporarily remove all the update handler
      for (ActionListener al : eventStreamComboBox.getActionListeners())
         eventStreamComboBox.removeActionListener(al);
      for (PopupMenuListener pml : eventStreamComboBox.getPopupMenuListeners())
         eventStreamComboBox.removePopupMenuListener(pml);

      try {
        //Now right away select the stream given by the event source
        eventStreamComboBox.setSelectedItem(((AStreamedEventSource) eventSource).getStream());
      } catch (InvalidEventSourceException ies) {
        //If we fail to update the list show some messages
        String msg = "Invalid event source when getting stream list";
        JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
        logger.error(msg+"\n"+ies.getCauseMessages());
      } catch (NoMoreEventsException nme) {
         //If we fail to update the list show some messages
        String msg = "No streams (yet) available for this source";
        JOptionPane.showMessageDialog(this, msg, "Invalid source", JOptionPane.ERROR_MESSAGE);
        logger.error(msg+"\n"+nme.getCauseMessages());
      }
   
      //(Re-)set the update handler
      eventStreamComboBox.addActionListener( new ActionListener () {
        public void actionPerformed(ActionEvent e) {
          selectedStreamAction((AStreamedEventSource)eventSource);
        }
      });

      //Also have the combobox update its item list every time the
      //popup is selected
      eventStreamComboBox.addPopupMenuListener(new PopupMenuListener(){
        //update stream list when popup appears
        public void popupMenuWillBecomeVisible(PopupMenuEvent e) {
          updateEventStreamCombo((AStreamedEventSource)eventSource);
        }
        //Do nothing in other cases
        public void popupMenuWillBecomeInvisible(PopupMenuEvent e) {}
        public void popupMenuCanceled(PopupMenuEvent e) {}
        }
      );

      //Finally make it visible
      eventStreamComboBox.setVisible(true);
    }
      
    //And repaint to show/hide the combo box
    repaint();

    // disable all the mode buttons
    sequentialButton.setEnabled(false);
    loopButton.setEnabled(false);
    randomButton.setEnabled(false);
  
    // enable the modes that are supported by the current event source
    // and mark the current display mode as active
    if(eventSource.supportsNavigationMode(NavigationMode.SEQUENTIAL)) {
      if(eventSource.getNavigationMode() == NavigationMode.SEQUENTIAL)
        sequentialButton.setSelected(true);
      sequentialButton.setEnabled(true);
    }

    if(eventSource.supportsNavigationMode(NavigationMode.LOOP)) {
      if(eventSource.getNavigationMode() == NavigationMode.LOOP)
        loopButton.setSelected(true);
      loopButton.setEnabled(true);
    }

    if(eventSource.supportsNavigationMode(NavigationMode.RANDOM)) {
      if(eventSource.getNavigationMode() == NavigationMode.RANDOM)
        randomButton.setSelected(true);
      randomButton.setEnabled(true);
    }

    // change the address in the address bar to the current event source
    refreshAddressBar();

  } // newEventSource() -----------------------------------------------------
}

