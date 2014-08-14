package atlantis.gui;


import java.awt.event.ItemListener;
import javax.swing.JComboBox;


/**
 * This class does not generate an ItemEvent
 * when the setSelectedItem() method is called.
 */

public class AComboBox extends JComboBox {
  private ItemListener listener;

  /**
   * Just calls the superclass constructor.
   */
  public AComboBox() {
    super();
  }

  /**
   * Set the GUIItemListener to be informed of ItemEvents.
   * Only one listener is allowed.
   * @param aListener
   */
  public void setGUIItemListener(ItemListener aListener) {
	// Remove any existing listeners.
    for (ItemListener listener : this.getItemListeners()) this.removeItemListener(listener);
    super.addItemListener(aListener);
    listener=aListener;
  }
  
  /**
   * Check whether the GUIItemListener has already been set.
   * @return true if this AComboBox has a GUIItemListener
   */
  public boolean hasGUIItemListener() {return (listener!=null);}

  /**
   * Set the selected item without telling any item listeners.
   * @param item
   */
  public void setGUISelectedItem(Object item) {
    if(listener!=null) super.removeItemListener(listener);
    super.setSelectedItem(item);
    if(listener!=null) super.addItemListener(listener);
  }

}
