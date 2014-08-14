package atlantis.parameters;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JComponent;

import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.gui.ATextField;

public class ANumberParameter extends AAbstractParameter {
  private ATextField textField;

  public ANumberParameter(
      String name, String screenName, String toolTip, String pv, int valueType, double value,
      boolean haseStatus, boolean status, boolean isMod, int userLevel, int scope, int unitsOf) {

    super(name, screenName, toolTip, valueType, value, null, null, haseStatus, status, isMod,
        userLevel, scope, unitsOf);
    range=resolvePossibleValues(pv, valueType);
  }

  public void setD(double v) {
    if(valueType!=FLOAT)
      throw new IllegalArgumentException("setD acces on not FLOAT parameter");

    _setD(v);
    refresh();
  }

  public void setI(int v) {
    if(valueType!=INT)
      throw new IllegalArgumentException("setI acces on not INT parameter");

    if(textField!=null) textField.setText(""+v);
    _setI(v);
  }

  public void initialize() {
    super.initialize();

    textField=new ATextField();
    textField.setEditable(true);
    textField.setToolTipText(toolTip);

    if(valueType==FLOAT)
      textField.setText(""+getD());
    else
      textField.setText(""+getI());

    textField.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        applyInput();
      }
    });

    /*
     textField.addFocusListener(new FocusAdapter() {
     public void focusLost(FocusEvent e) {
     applyInput();
     }
     });
     */
  }

  private void applyInput() {
    boolean error=false;

    try {
      double val=parseUnitsDouble(textField.getText());

      if(validateValue(val)) {
        if(valueType==FLOAT) {
          ACommandProcessor.receive(name+getOperator()+val);
          apply();
        } else {
          ACommandProcessor.receive(name+getOperator()+(int)val);
          apply();
        }
      } else
        error=true;
    } catch(NumberFormatException nfe) {
      error=true;
    }
    if(error)
      AOutput.append("value: "+textField.getText()+", is not correct!", ALogInterface.BAD_COMMAND);
  }

  public JComponent getValueComponent() {
    return textField;
  }

  public void refresh() {
    _refresh();
    if(textField!=null) {
      if(valueType==FLOAT)
        textField.setText(parseUnits(getD()));
      else
        textField.setText(""+getI());
    }
    fireParameterChanged();
  }

}
