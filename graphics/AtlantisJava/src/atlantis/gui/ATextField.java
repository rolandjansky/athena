package atlantis.gui;


import java.awt.event.ActionListener;
import javax.swing.JTextField;


/**
 * This textfield will not generate an event when the setText() function is called.
 */

public class ATextField extends JTextField {
  ActionListener listener;

  public ATextField() {
    super();
  }

  public void addActionListener(ActionListener l) {
    if(listener==null) {
      super.addActionListener(l);
      listener=l;
    }
  }

  public void setText(String t) {
    if(listener!=null) super.removeActionListener(listener);
    super.setText(t);
    if(listener!=null) super.addActionListener(listener);
  }
}
