package atlantis.graphics.dnd;

import java.awt.Point;
import java.awt.Graphics;
import javax.swing.JButton;
import java.awt.image.BufferedImage;
import java.awt.dnd.DragSourceDropEvent;
import java.awt.dnd.DragSourceListener;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragSource;
import java.awt.dnd.DragSourceDragEvent;
import java.awt.dnd.DragSourceEvent;
import java.util.Vector;

/**
 * A button wich has Drag and Drop support ( see java.awt.dnd.*)
 * Used by the rubberbands.
 */

public class ADnDButton extends JButton
  implements DragSourceListener, DragGestureListener, ACallBack {

  private DragSource dragSource;
  private Vector dragListeners;

  public ADnDButton(String name) {
    super(name);
    setName(name);

    dragSource=DragSource.getDefaultDragSource();
    dragSource.createDefaultDragGestureRecognizer(this, DnDConstants.ACTION_MOVE, this);

    dragListeners=new Vector();
  }

  // implementation of DragSourceListener

  // This method is invoked to signify that the Drag and Drop operation is complete.
  public void dragDropEnd(DragSourceDropEvent dsde) {}

  // Called as the hotspot enters a platform dependent drop site.
  public void dragEnter(DragSourceDragEvent dsde) {
    dsde.getDragSourceContext().setCursor(ADnDLabel.DROP_VALID);
  }

  // Called as the hotspot exits a platform dependent drop site.
  public void dragExit(DragSourceEvent dse) {
    dse.getDragSourceContext().setCursor(ADnDLabel.DROP_INVALID);
  }

  // Called as the hotspot moves over a platform dependent drop site.
  public void dragOver(DragSourceDragEvent dsde) {
    dsde.getDragSourceContext().setCursor(ADnDLabel.DROP_VALID);
  }

  // Called when the user has modified the drop gesture.
  public void dropActionChanged(DragSourceDragEvent dsde) {}

  // implementation of DragGestureListener

  public void dragGestureRecognized(DragGestureEvent dge) {
    BufferedImage resizeCursorImage=new BufferedImage(64, 64, BufferedImage.TYPE_4BYTE_ABGR);

    Graphics g=resizeCursorImage.getGraphics();

    g.drawLine(0, 0, 64, 64);
    g.dispose();

    // return Toolkit.getDefaultToolkit().createCustomCursor(resizeCursorImage, center, "resize")


    AObjectTransferable cbt=new AObjectTransferable(this);

    dragSource.startDrag(dge, ADnDLabel.DROP_INVALID, resizeCursorImage, new Point(0, 0), cbt, this);
  }

  public void callBack(Object to) {
    for(int i=0; i<dragListeners.size(); i++)
      ((ADragListener)dragListeners.get(i)).dragPerformed(this, to, -1);
  }

  public void addDragListener(ADragListener l) {
    dragListeners.addElement(l);
  }

}
