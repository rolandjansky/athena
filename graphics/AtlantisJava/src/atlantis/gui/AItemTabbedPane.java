package atlantis.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

// Gary- DnD facility commented out as it does not fully work
//  ( Window control is left in funny state)
// and is not something i have ever used.....

/**
 * A TabbedPane which generates events when a tab is selected
 * and when a tab is deselected.
 * Provides DnD support.
 */
public class AItemTabbedPane extends JTabbedPane
  implements // for Item Selectability
      ItemSelectable, ChangeListener {
//      // for drag and drop
//      DragSourceListener, DragGestureListener, ACallBack {

  private Component current;
  private Component previous;
  private ItemListener itemListener;
  private ItemEvent itemEvent;

//  private DragSource dragSource=null;
//  private Vector dragListeners;

  public AItemTabbedPane(int tabPlacement) {
    super(tabPlacement);
    // tabs don't change on right click rather help appears.
    setUI(new ATabbedPaneUI());

    current=null;
    previous=null;
    this.addChangeListener(this);

//    // initializing drag and drop
//    dragSource=new DragSource();
//    dragSource.createDefaultDragGestureRecognizer(this, DnDConstants.ACTION_MOVE, this);

//    dragListeners=new Vector();
  }

  public Component getPreviousSelectedComponent() {
    if(previous==null)
      return getSelectedComponent();
    else
      return previous;
  }

  public void stateChanged(ChangeEvent e) {
    previous=current;

    if((current!=null)&&(itemListener!=null)) {
      itemEvent=new ItemEvent(this, 0, current, ItemEvent.DESELECTED);
      itemListener.itemStateChanged(itemEvent);
    }

    current=getSelectedComponent();

    if(itemListener!=null) {
      itemEvent=new ItemEvent(this, 0, current, ItemEvent.SELECTED);
      itemListener.itemStateChanged(itemEvent);
    }
  }

  // Add a listener to recieve item events when the state of an tab changes.
  public void addItemListener(ItemListener l) {
    itemListener=l;
  }

  // Returns the selected items or null if no items are selected.
  public Object[] getSelectedObjects() {
    return null;
  }

  // Removes an item listener.
  public void removeItemListener(ItemListener l) {
    if(itemListener.equals(l))
      itemListener=null;
  }

  public Dimension getPreferredSize() {
    this.getLayout().layoutContainer(this);
    return super.getPreferredSize();
  }

/*
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
    Point p=dge.getDragOrigin();

    if(getSelectedIndex()==getUI().tabForCoordinate(this, p.x, p.y)) {
      AObjectTransferable cbt=new AObjectTransferable(this);

      dragSource.startDrag(dge, ADnDLabel.DROP_INVALID, cbt, this);
    }
  }

  public void callBack(Object to) {
    for(int i=0; i<dragListeners.size(); i++)
      ((ADragListener)dragListeners.get(i)).dragPerformed(this, to, -1);
  }

  public void addDragListener(ADragListener l) {
    dragListeners.addElement(l);
  }

  public void addChangeListener(ChangeListener l) {
   super.addChangeListener(l);
  }

  public void removeChangeListener(ChangeListener l) {
   super.removeChangeListener(l);
  }
*/
}
