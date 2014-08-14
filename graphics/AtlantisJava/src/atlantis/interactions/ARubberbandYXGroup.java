package atlantis.interactions;

import atlantis.canvas.ACanvas;
import atlantis.output.AExceptionHandler;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;
import javax.swing.JMenu;
import javax.swing.JComboBox;
import javax.swing.JPanel;

import java.lang.reflect.Constructor;
import java.util.HashMap;

public class ARubberbandYXGroup extends AInteractionGroup {

  private final static String[] RUBBERBAND_NAMES= {"RectangleYX", "RotatedRectangle", "Parallelogram",
    "Square", "XSkew", "YSkew", "XSlice", "YSlice"};

  private HashMap rubberbands;
  private JComboBox rbComboBox;
  // the panel that will contain the additional controls for the interactions
  private JPanel additionalControls;

  public ARubberbandYXGroup(AInteractionsManager manager) {
    super(AInteractionGroup.WINDOW_GROUP, manager);
    
    // create the additional controls panel
    additionalControls = new JPanel();

    rubberbands=new HashMap();
    rbComboBox=new JComboBox();

    for(int i=0; i<RUBBERBAND_NAMES.length; i++) {
      rbComboBox.addItem(RUBBERBAND_NAMES[i]);
      try {
        Constructor[] c=Class.forName("atlantis.interactions.A"+RUBBERBAND_NAMES[i]+"Selection").getDeclaredConstructors();

        rubberbands.put(RUBBERBAND_NAMES[i], c[0].newInstance(new Object[] {}));
      } catch(Exception e) {
        AExceptionHandler.processException("Cannot make selection "+"A"+RUBBERBAND_NAMES[i]
            +"Selection",
            e);
      }
    }

    rbComboBox.addActionListener(new RuberbandListener());
    additionalControls.add(rbComboBox);

    changeRubberband();
  }
  
  /**
   * rubberband has additional controls, so return true
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
  
  private void changeRubberband() {
    String item=(String)rbComboBox.getSelectedItem();

    removeAllInteractions();
    addInteraction((AInteraction)rubberbands.get(item));
    iManager.setContext(this);
  }

  // generating the menu
  public JMenuItem getPopupItem() {
    JMenu menu=new JMenu(name);
    ActionListener l=new PopupListener();

    for(int i=0; i<RUBBERBAND_NAMES.length; i++)
      menu.add(RUBBERBAND_NAMES[i]).addActionListener(l);

    return menu;
  }

  public void setGroupState(AInteractionGroup givenGroup) {
    if(givenGroup instanceof ARubberbandYXGroup) {
      ARubberbandYXGroup panel=(ARubberbandYXGroup)givenGroup;

      this.rbComboBox.setSelectedIndex(panel.rbComboBox.getSelectedIndex());
    }
  }

  class RuberbandListener implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      changeRubberband();
    }
  }


  class PopupListener implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      ACanvas.getCanvas().getCurrentWindow().getInteractionToolBar().setSelectedGroup(name);
      rbComboBox.setSelectedItem(e.getActionCommand());
    }
  }

}
