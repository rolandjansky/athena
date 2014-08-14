package atlantis.interactions;

import atlantis.canvas.ACanvas;
import java.awt.event.*;
import javax.swing.JCheckBox;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import java.awt.BorderLayout;
import java.text.NumberFormat;

public class AFishEyeGroup extends AInteractionGroup implements AFishEyeChangeListener {

  private final AFishEyeInteraction fishEyeInteraction;
  private final JCheckBox checkbox;
  private final JFormattedTextField valueField;
  // the panel that will contain the additional controls for the interactions
  private JPanel additionalControls;

//Format, label and label string for value field
  private NumberFormat valueFormat;
  private JLabel valueLabel;
  private static String valueString = "Value: ";

  public AFishEyeGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);

    fishEyeInteraction=new AFishEyeInteraction();
    addInteraction(fishEyeInteraction);

    // create the additional controls panel
    additionalControls = new JPanel();

    checkbox=new JCheckBox("on/off", false);
    checkbox.addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        switch(e.getStateChange()) {
        case ItemEvent.SELECTED:
          fishEyeInteraction.setStatus(true);
          //fishEyeInteraction.getParameter("FishEye").setStatus(true);
          break;

        case ItemEvent.DESELECTED:
          fishEyeInteraction.setStatus(false);          
          //fishEyeInteraction.getParameter("FishEye").setStatus(false);
          break;
        }
        ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
      }
    });

//Create the label
    valueLabel = new JLabel(valueString);
    valueFormat = NumberFormat.getNumberInstance();
    valueFormat.setMaximumIntegerDigits(2);
    valueField=new JFormattedTextField(valueFormat);
    valueField.setToolTipText("Enter fish eye value between 0 and 25");
    valueField.setValue(new Double(0));
    valueField.setColumns(3);
    valueField.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e)
      {
        Number value =(Number)valueField.getValue();
        double dvalue = value.doubleValue();
        if(dvalue>0 && dvalue<25)
        {
          fishEyeInteraction.setValue(dvalue);
          ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
        }
        else
        {
          Double val = new Double(fishEyeInteraction.getValue());
          valueField.setValue(val);
        }
      }
    });

    JPanel nestedPanel = new JPanel();
    nestedPanel.add(valueLabel);
    nestedPanel.add(valueField);

    JPanel borderPanel = new JPanel(new BorderLayout());
    borderPanel.add(checkbox, BorderLayout.WEST);
    borderPanel.add(nestedPanel, BorderLayout.EAST);

    additionalControls.add(borderPanel);
    
  }

  public void connect() {
    super.connect();
    
    //Only after we are connected to a window, add ourselves as update listener
    fishEyeInteraction.addFishEyeChangeListener(this);
  }

  public void fishEyeChange(boolean status, double value) {
    valueField.setValue(value);
    checkbox.setSelected(status);
  }
  
  /**
   * FishEye has additional controls, so return true
   */
  public boolean hasAdditionalControls() {
    return true;
  }
  
  /**
   * return the additional controls 
   */
  public JPanel getAdditionalControls() {
    return additionalControls;
  }
  
}
