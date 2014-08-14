package atlantis.graphics.layout;


import java.awt.Container;
import java.awt.Window;
import javax.swing.SwingUtilities;


/**
 * Used by the Layout Managers to invoke another layout operation.
 * Needed to resize the canvas preserving aspect ratio.
 */

public class ARunAnotherLayout implements Runnable {
  private Container parent;

  public ARunAnotherLayout(Container parent) {
    this.parent=parent;
  }

  public void run() {
    Window window=SwingUtilities.getWindowAncestor(parent);

    if(window!=null) window.pack();
  }
}
