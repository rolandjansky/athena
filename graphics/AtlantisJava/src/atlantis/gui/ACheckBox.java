package atlantis.gui;

import java.awt.event.ActionListener;
import javax.swing.JCheckBox;

/**
 * This class does not generate an ActionEvent
 * when the setSelected() method is called.
 */
public class ACheckBox extends JCheckBox {
  ActionListener listener;

  public ACheckBox(String text) {
    super(text);
  }

  public ACheckBox() {
    super();
  }

  public void addActionListener(ActionListener l) {
    if(listener==null) {
      super.addActionListener(listener);
      listener=l;
    }
  }

  public void setSelected(boolean b) {
    if(listener!=null) super.removeActionListener(listener);
    super.setSelected(b);
    if(listener!=null) super.addActionListener(listener);
  }
}
