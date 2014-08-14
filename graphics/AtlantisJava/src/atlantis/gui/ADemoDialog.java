package atlantis.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import javax.swing.WindowConstants;
import javax.swing.border.Border;

import java.util.Arrays;
import java.util.Vector;

import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.projection.AProjectionsManager;
import atlantis.parameters.ACommandProcessor;
import atlantis.utils.ALogger;





/**
 * A Dialog in which user can set parameters for demo mode, as well as start and
 * stop the demo mode. This class holds and controls the demo loop thread. 
 * This is a singleton, but there is no point why it needs to be.
 *
 * @author Sebastian Boeser (with traces from Qiang Lu)
 */
public class ADemoDialog extends JFrame implements ActionListener
{
    //Get a logger
    private final static ALogger logger = ALogger.getLogger(ADemoDialog.class);

    //Some GUI elements
    private JSpinner timerSpinner;
    private SpinnerNumberModel timerSpinnerModel;
    private static final Integer TIMER_MIN = new Integer(1);
    private static final Integer TIMER_MAX = new Integer(120);
    private static final Integer TIMER_INIT = new Integer(3);

    //The text label showing the projection sequence
    private JTextField sequenceTextField = null;
    //And the panel housing these
    private JPanel projControlPanel=null;
    
    //The action buttons
    private JButton startButton = null;
    private JButton stopButton = null;
    private JButton closeButton = null;


    //The thread that is runing the loop
    private ADemoLoop demo = null;

    private static final AGlobals globals = AGlobals.instance();

    //The singleton instance
    private static ADemoDialog instance = null;

    /**
     * Public access-instantiation accessor
     * @return the singleton instance
     */
    public static ADemoDialog getInstance(){
      //Create instance if needed
      if (instance == null) instance = new ADemoDialog();
      //and return it
      return instance;
    }

    /**
     * Private Constructor
     */
    private ADemoDialog() {
      super("Demo");
      createGUI();
      pack();
    }

    // initialize the GUI
    private void createGUI()
    {
        JPanel timerControlPanel = createTimeControlPanel();
        projControlPanel = createProjControlPanel();
        JPanel buttonControlPanel = createButtonControlPanel();

        JPanel contentPane = new JPanel(new GridBagLayout());
        contentPane.setOpaque(true);
        GridBagConstraints c = new GridBagConstraints();
        c.weightx = 1;
        c.weighty = 1;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill=GridBagConstraints.HORIZONTAL;
        c.anchor = GridBagConstraints.CENTER;
        contentPane.add(timerControlPanel, c);
        contentPane.add(projControlPanel, c);
        contentPane.add(buttonControlPanel, c);
        this.setResizable(false);
        this.setContentPane(contentPane);
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        this.setAlwaysOnTop(true);

        // set the initial location
        Frame gui = globals.getGuiFrame();
        int guiWidth = gui.getWidth();
        int guiHeight = gui.getHeight();
        int guiX = gui.getX();
        int dialogWidth = (int) this.getPreferredSize().getWidth();
        int dialogHeight = (int) this.getPreferredSize().getHeight();
        int screenWidth = Math.round((float) Toolkit.getDefaultToolkit().getScreenSize().getWidth());
        if(guiX + guiWidth + (dialogWidth - guiWidth) / 2 > screenWidth)
            this.setLocation(Math.max(0, screenWidth - dialogWidth), 
                    Math.max(0, (guiHeight - dialogHeight) / 3));
        else
            this.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2), 
                    Math.max(0, (guiHeight - dialogHeight) / 3));
        AIcon.setIconImage(this);
    }

     private JPanel createTimeControlPanel()
    {

        JPanel aPanel = new JPanel(new GridBagLayout());
        GridBagConstraints c = new GridBagConstraints();
        aPanel.setOpaque(true);
        //aPanel.setPreferredSize(new Dimension(FRAME_WIDTH, PANEL_HEIGHT));
        aPanel.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));

        //Add timer label
        JLabel timerLabel= new JLabel("Interval");
        c.gridwidth = 1;
        c.fill = GridBagConstraints.HORIZONTAL;
        c.weightx=1;
        c.gridx=0;
        aPanel.add(timerLabel,c);
        //Add timer spinner
        timerSpinner = new JSpinner();
        timerSpinner.setEnabled(true);
        timerSpinnerModel = new SpinnerNumberModel(TIMER_INIT, TIMER_MIN,
                TIMER_MAX, new Integer(1));
        timerSpinner.setModel(timerSpinnerModel);
        c.gridwidth = 1;
        c.weightx=1;
        c.fill = GridBagConstraints.HORIZONTAL;
        c.gridx=1;
        aPanel.add(timerSpinner,c);
        //Add units label
        c.weightx = 0.5;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridx=2;
        aPanel.add(new JLabel(" sec"),c);


        //Add the labels for th selected projections
        c.gridx=GridBagConstraints.RELATIVE;
        c.weightx=1;
        c.gridwidth=1;
        aPanel.add(new JLabel("Sequence:"), c);

        //Now generate the sequence label and make it span the remaining columns
        sequenceTextField = new JTextField();
        c.gridwidth=GridBagConstraints.REMAINDER;
        c.fill=GridBagConstraints.HORIZONTAL;
        aPanel.add(sequenceTextField,c);

        return aPanel;
    }

    private JPanel createProjControlPanel()
    {

        JPanel aPanel = new JPanel(new GridLayout(3,4));
        aPanel.setOpaque(true);
        Border outerBorder = BorderFactory.createEmptyBorder(5, 5, 5, 5);
        Border innerBorder = BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.GRAY),
                "Add Projection / Action");
        aPanel.setBorder(BorderFactory.createCompoundBorder(outerBorder, innerBorder));

        //Create the list of check boxes
        for (String projName : AProjectionsManager.getKnownProjectionNames()){

          //Check wether this projection name should be used in the demo dialog
          if (AProjectionsManager.useProjectionInDemoMode(projName)){

            //Create the respective button with the screen name of the projection
            String projScreenName = AProjectionsManager.getProjection(projName).getScreenName();
            JButton projButton = new JButton(projScreenName);
            //Store the name of the projection as the name of the JCheckBox
            //Where we can retrieve it once the demo dialog is started
            projButton.setName(projName);
            projButton.setFocusPainted(false);

            //Add ourselves as action listener of this button
            projButton.addActionListener(this);
            aPanel.add(projButton);
          }
       }
       //Also add a "next event" button
        JButton nextEvtBut = new JButton("Next event");
        nextEvtBut.setName("NE");
        nextEvtBut.addActionListener(this);
        aPanel.add(nextEvtBut);

       return aPanel;
    }

    private JPanel createButtonControlPanel()
    {
        //final int PANEL_HEIGHT = 40;
        Dimension buttonDimension = new Dimension(100,25);

        JPanel aPanel = new JPanel(new GridBagLayout());
        aPanel.setOpaque(true);
        //aPanel.setPreferredSize(new Dimension(FRAME_WIDTH, PANEL_HEIGHT));
        aPanel.setBorder(BorderFactory.createEmptyBorder(5,5,10,5));

        GridBagConstraints c = new GridBagConstraints();
        startButton = new JButton("Start");
        //startButton.setFocusPainted(false);
        startButton.setEnabled(true);
        startButton.setPreferredSize(buttonDimension);
        startButton.addActionListener(this);
        c.weightx = 1;
        c.weighty = 1;
        c.anchor = GridBagConstraints.CENTER;
        aPanel.add(startButton, c);
        stopButton = new JButton("Stop");
        stopButton.setFocusPainted(false);
        stopButton.setEnabled(false);
        stopButton.setPreferredSize(buttonDimension);
        stopButton.addActionListener(this);
        aPanel.add(stopButton, c);
        JSeparator aSeparator = new JSeparator(SwingConstants.VERTICAL);
        //aSeparator.setPreferredSize(new Dimension(5, PANEL_HEIGHT));
        aPanel.add(aSeparator, c);
        closeButton = new JButton("Close");
        closeButton.setFocusPainted(false);
        closeButton.setEnabled(true);
        closeButton.setPreferredSize(buttonDimension);
        closeButton.addActionListener(this);
        c.gridwidth = GridBagConstraints.REMAINDER;
        aPanel.add(closeButton, c);

        return aPanel;
    }

    /**
     * @return true if demo thread is running, otherwise false
     */
    boolean isRunning(){
      //no thread -> false
      if (demo == null) return false;
      //otherwise use thread status
      return demo.isRunning();
    }
   

    // return interval in millisecond
    int getTimerInterval()
    {
        return 1000 * timerSpinnerModel.getNumber().intValue();
    }

    public void setTimerInterval(Integer intervalInSec)
    {
      try {
        timerSpinnerModel.setValue(intervalInSec);
      } catch (IllegalArgumentException iae){
        //Just ignore in case of illegal argument
      }
    }


    /**
     * Set the sequence of projections
     * @param projNames colon seperated list of projection names
     */
    public void setSequence(String projNames){
      //store this in our text entry
      sequenceTextField.setText(projNames);
    }

    /**
     * Set the default sequence
     */
    public void setDefaultSequence(){
      //Create default sequence
      StringBuilder sequence = new StringBuilder();
      //Add all default projections
      for (String projName : AProjectionsManager.getKnownProjectionNames()){
        if (AProjectionsManager.defaultProjectionInDemoMode(projName))
          sequence.append(projName+":");
      }
      //End sequence with next event
      sequence.append("NE");
      //And set this in the text field
      sequenceTextField.setText(sequence.toString());
    }

    /**
     * @return a vector with the names of the projections
     */
    private Vector<String> getSequence()
    {
      return new Vector<String>(Arrays.asList(sequenceTextField.getText().split(":")));
    }

    /**
     * Validate whether the given sequence is valid
     * @return the number of selected projections
     */
    private boolean validateSequence()
    {
      //Get the projections sequence
      Vector<String> sequence = getSequence();

      //Check on number of projections
      if(sequence.size() < 1){
        JOptionPane.showMessageDialog(this,
         "No projection/event is selected, please select at least select one",
         "Warning", JOptionPane.WARNING_MESSAGE);
        return false;
      }

      //Now check each item of the sequence for validity
      for (String projName : sequence){
        //The "new event" action is an exception here
        if (projName.equals("NE")) continue;
        //check wether this is a known and allowed projection
        if ((! Arrays.asList(AProjectionsManager.getKnownProjectionNames()).contains(projName)) ||
            (! AProjectionsManager.useProjectionInDemoMode(projName)))
        {
         JOptionPane.showMessageDialog(this,
           "Not a valid projection name: '"+projName+"'",
           "Warning", JOptionPane.WARNING_MESSAGE);
         return false;
        }

      }

      //Seems all okay
      return true;
    }

   

    /**
     * Start the demo thread
     */
    public void startDemo()
    {

      //First make sure some projections have been selected
      if (!validateSequence()) return;

      //Set single full window mode
      ACommandProcessor.receive("WW");

      //Start the demo, setting "stopOnNoMoreEvents" to false
      //TODO: make stopOnNoMoreEvents a checkbox in the Dialog
      demo = new ADemoLoop(getTimerInterval(),getSequence(),false);
      demo.startDemoLoop();
      //Now disable all GUI elements
      timerSpinner.setEnabled(false);
      sequenceTextField.setEnabled(false);
      for (Component comp: projControlPanel.getComponents()) comp.setEnabled(false);
      startButton.setEnabled(false);
      stopButton.setEnabled(true);
      closeButton.setEnabled(false);
      //Do not allowe the user to close the window, so he doesn't loose access
      this.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    }

    /**
     * Stop the demo thread
     */
    synchronized void stopDemo()
    {
      //exit the demo
      demo.stopDemoLoop();
      //Now reenable all GUI elements
      timerSpinner.setEnabled(true);
      sequenceTextField.setEnabled(true);
      for (Component comp: projControlPanel.getComponents()) comp.setEnabled(true);
      stopButton.setEnabled(false);
      startButton.setEnabled(true);
      closeButton.setEnabled(true);

      this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    }

    /**
     * Implementation of the Action listener
     * - gets called for every button that is pressed
     * @param e the action event
     */
    public void actionPerformed(ActionEvent e) {
      //Check for action buttons first
      if (e.getSource() == startButton ) startDemo();
      else if (e.getSource() == stopButton) stopDemo();
      else if (e.getSource() == closeButton ) dispose();
      //this should be one of the projection buttons
      else {
        //get the projectio name to append
        String projName = ((JButton)e.getSource()).getName();
        //get the existing sequence
        String sequence = sequenceTextField.getText();
        //append new projection
        sequence += ((sequence.length()!=0) ? ":" : "") + projName;
        //Set this as new sequence
        sequenceTextField.setText(sequence);
      }

    }
}
