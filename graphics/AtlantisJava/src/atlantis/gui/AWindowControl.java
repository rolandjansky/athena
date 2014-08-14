package atlantis.gui;

import atlantis.canvas.ALayoutChangeListener;
import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.canvas.ALayout;
import atlantis.graphics.layout.AFlowLayout;
import atlantis.graphics.layout.AGridLayout;
import atlantis.graphics.dnd.ADnDLabel;
import atlantis.graphics.dnd.ADragListener;
import atlantis.parameters.ACommandProcessor;
import atlantis.utils.*;
import java.util.*;
import javax.swing.BorderFactory;
import javax.swing.SwingUtilities;
import javax.swing.JPanel;
import java.awt.Color;
import java.awt.Rectangle;
import java.awt.Dimension;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.event.*;

/**
 * Allows user to select the current window and to
 * copy between windows.
 */
public class AWindowControl extends JPanel
  implements ALayoutChangeListener, ADragListener {

  private String selectedWindowName;
  private Vector pages=new Vector(10);

  public AWindowControl getPanel() {
    return this;
  }

  public AWindowControl() {
    super();
    setLayout(new AFlowLayout(10, 10));

    addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if(AUtilities.isRightMouseButton(e))
          AHelpSystem.getInstance().showPage("WindowControl");
      }
    });

    // registering as a listener to of window lock changes
    ACanvas.getCanvas().addLockChangeListener(new ChangeListener() {

      public void stateChanged(ChangeEvent e) {
        for(int i=0; i<pages.size(); i++) {
          Hashtable page=(Hashtable)pages.elementAt(i);
          Enumeration labels=page.elements();
          while(labels.hasMoreElements()) {
            ADnDLabel label=(ADnDLabel)labels.nextElement();
            if(ACanvas.getCanvas().getWindow(label.getName()).isLocked())
              label.setForeground(Color.red);
            else
              label.setForeground(Color.black);
          }
        }
      }
    });

    // registering as a listener to APar
    ACanvas.getCanvas().addLayoutChangeListener(this);

    ACanvas.getCanvas().addWindowChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        ADnDLabel label;

        if(selectedWindowName!=null)
          for(int i=0; i<pages.size(); i++) {
            Hashtable page=(Hashtable)pages.elementAt(i);

            label=(ADnDLabel)page.get(selectedWindowName);
            if(label!=null)
              label.setBackground(new Color(207, 207, 207));
          }

        selectedWindowName=ACanvas.getCanvas().getCurrentWindow().getName();
        for(int i=0; i<pages.size(); i++) {
          Hashtable page=(Hashtable)pages.elementAt(i);

          label=(ADnDLabel)page.get(selectedWindowName);
          if(label!=null)
            label.setBackground(Color.white);
        }
      }
    });
  }

  // function gets called when the current layout is changed in APar
  public void layoutChanged(ACanvas c) {
    removeAll();
    ALayout layout = c.getCurrentLayout();
    Dimension d=layout.getSize();
    WindowMouseListener windowMouseListener=new WindowMouseListener();

    // adding new set of pages
    pages.clear();
    String[] pageNames=layout.getPageNames();
    Hashtable pageHashtable;

    for(int i=0; i<pageNames.length; i++) {
      String[] wNames=layout.getPageContent(pageNames[i]);

      JPanel page=new JPanel(new AGridLayout(d, 15));

      pageHashtable=new Hashtable();
      for(int k=0; k<wNames.length; k++) {
        Rectangle constraint=layout.getWindowConstraints(wNames[k]);
        ADnDLabel w=new ADnDLabel(wNames[k], wNames, true);

        w.setBorder(BorderFactory.createLineBorder(new Color(150, 150, 150)));
        pageHashtable.put(wNames[k], w);
        page.add(w, constraint);
        w.addMouseListener(windowMouseListener);
        w.addDragListener(this);
      }

      pages.addElement(pageHashtable);
      add(page);
      setToolTipText("Current Layout - "+layout.getName());
      revalidate();
      repaint();
    }
  }

  public void dragPerformed(Object from, Object to, int action) {
    if((from instanceof ADnDLabel)&&(to instanceof ADnDLabel))
      ACanvas.getCanvas().copyWindowSettings(((ADnDLabel)from).getText(), ((ADnDLabel)to).getText());
  }

  private Hashtable findPage(String[] wNames)
  {
    for (int i = 0; i < pages.size(); i++)
    {
      int matches = 0;
      Hashtable page = (Hashtable)pages.get(i);

      for (int j = 0; j < wNames.length; j++)
      {
        if (page.containsKey(wNames[j]))
          matches++;
        else break;
      }
      if (matches == wNames.length)
        return page;
    }
    return null;
  }

  class WindowMouseListener extends MouseAdapter {

    public void mousePressed(MouseEvent e) {
      ADnDLabel label = (ADnDLabel) e.getSource();

      if (AUtilities.isRightMouseButton(e)) {
        if (AEventQueue.getKeyboardState() == KeyEvent.VK_A) {

          String[] pageName = label.returnPageName();
          Hashtable page = findPage(pageName);
          if(page==null)
            return;

          for (Enumeration labels = page.elements(); labels.hasMoreElements(); ) {
            label = (ADnDLabel) labels.nextElement();
            if (label != null)
              ACommandProcessor.receive("W" + label.getName());
          }
        }
        else {
          AWindow.getPopupMenu().show(label, e.getX(), e.getY());
          ACommandProcessor.receive("W" + label.getName());
        }
      }
    }

    public void mouseReleased(MouseEvent e) {
      ADnDLabel label=(ADnDLabel)e.getSource();

      if(SwingUtilities.isLeftMouseButton(e)) {
        ACommandProcessor.receive("W"+label.getName());
      }
    }
  }
}
