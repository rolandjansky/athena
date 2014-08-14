package atlantis.parameters;

import atlantis.utils.*;
import atlantis.gui.AComboBox;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import javax.swing.JComponent;

/**
 * A parameter which shows a list of possible integer values.
 */
public class AListIntegerParameter extends AAbstractParameter {
    
  private static ALogger logger = ALogger.getLogger(AListIntegerParameter.class);
    
  private AComboBox comboBox;
  int[] displayableValues;

  public AListIntegerParameter(
      String name, String screenName, String toolTip, String pv, int value,
      boolean haseStatus, boolean status, int userLevel, int scope) {

    super(name, screenName, toolTip, INT, value, null, null, haseStatus, status, false, userLevel,
        scope, NOTHING);

    range=resolvePossibleValues(pv, INT);
    displayableValues=getValues(pv);
  }

  public void setD(double v) {
    throw new IllegalArgumentException("is not allowed to use setD on AListIntegerParameter");
  }

  public void setI(int v) {
    if(comboBox!=null) comboBox.setGUISelectedItem(""+v);
    _setI(v);
    refresh();
  }

  public void initialize() {
    super.initialize();
    comboBox=new AComboBox();
    comboBox.setEditable(true);
    comboBox.setToolTipText(toolTip);

    for(int i=0; i<displayableValues.length; i++)
      if(validateValue(displayableValues[i]))
        comboBox.addItem(""+displayableValues[i]);
      else throw new Error("Displayable Value: "+displayableValues[i]+" cannot be validated"
            +toString());

    comboBox.setGUISelectedItem(""+getI());

    comboBox.setGUIItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        if(e.getStateChange()==ItemEvent.SELECTED)
          applyInput();
      }
    });

    /*
     comboBox.addFocusListener(new FocusAdapter() {
     public void focusLost(FocusEvent e) {
     applyInput();
     }
     });
     */
  }

  private void applyInput() {
    boolean error=false;

    try {
      int val=Integer.parseInt((String)comboBox.getSelectedItem());

      if(validateValue(val)) {
        ACommandProcessor.receive(name+"="+val);
        apply();
      } else
        error=true;
    } catch(NumberFormatException nfe) {
      error=true;
    }
    if(error)
      logger.error("value: "+comboBox.getSelectedItem()+", is not correct!");
  }

  public JComponent getValueComponent() {
    return comboBox;
  }

  public void refresh() {
    _refresh();
    if(comboBox!=null) comboBox.setGUISelectedItem(""+getI());
    fireParameterChanged();
  }

}
