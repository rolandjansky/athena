package atlantis.gui;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JComponent;
import javax.swing.plaf.ComponentUI;
import javax.swing.plaf.metal.MetalTabbedPaneUI;

import atlantis.utils.AUtilities;

/**
 * Once installed on a TabbedPane this UI will allow changing the tabs only with
 * the left mouse button. By default this is possible with any button.
 * In Atlantis the right mouse button provides help
 */

public class ATabbedPaneUI extends MetalTabbedPaneUI {

  public static ComponentUI createUI(JComponent c) {
    return new ATabbedPaneUI();
  }

  protected MouseListener createMouseListener() {
    return new myMouseHandler();
  }

  public class myMouseHandler extends MouseHandler {
    public void mousePressed(MouseEvent e) {
      if(!AUtilities.isRightMouseButton(e))
        super.mousePressed(e);
    }
  }

}
