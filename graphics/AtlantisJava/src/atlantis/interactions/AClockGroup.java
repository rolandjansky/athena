package atlantis.interactions;

import atlantis.canvas.ACanvas;
import atlantis.parameters.APar;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import javax.swing.JCheckBox;
import javax.swing.JPanel;

public class AClockGroup extends AInteractionGroup {

  private AClockInteraction clockInteraction;
  private JCheckBox checkbox;
  // the panel that will contain the additional controls for the interactions
  private JPanel additionalControls;

  public AClockGroup(AInteractionsManager iManager) {
    super(AInteractionGroup.WINDOW_GROUP, iManager);
    
    clockInteraction=new AClockInteraction();
    addInteraction(clockInteraction);
    
    // create the additional controls panel
    additionalControls = new JPanel();

    checkbox=new JCheckBox("on/off", false);
    checkbox.addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        switch(e.getStateChange()) {
        case ItemEvent.SELECTED:
          parameterStore.get("YX", "Clock").setStatus(true);
          break;

        case ItemEvent.DESELECTED:
          parameterStore.get("YX", "Clock").setStatus(false);
          break;
        }
        ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
      }
    });
    additionalControls.add(checkbox);
  }

  public void connect() {
    checkbox.setSelected(parameterStore.get("YX", "Clock").getStatus());
  }
  
  /**
   * this interaction group has additional controls, so return true
   */
  
  public boolean hasAdditionalControls() {
	  return true;
  }
  /**
   * returns the additional controls
   */
  public JPanel getAdditionalControls() {
	  return additionalControls;
  }

}