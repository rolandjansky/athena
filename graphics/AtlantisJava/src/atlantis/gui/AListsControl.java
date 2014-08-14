package atlantis.gui;

import atlantis.list.AListManager;
import atlantis.utils.AUtilities;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 * The preferences menu.
 */
public class AListsControl extends AMenuButton {

  public AListsControl() {
    super("Lists");
    this.setToolTipText("List control dialog" );
    this.setBorder(null);
    this.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if (AUtilities.isRightMouseButton(e)) {
          AHelpSystem.getInstance().showPage("Lists");
        } else {
          AListManager.getInstance().showLists();
        }
      }
    });
  }

}
