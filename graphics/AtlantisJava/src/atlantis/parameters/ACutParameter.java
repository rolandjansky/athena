package atlantis.parameters;

import java.awt.Color;
import java.awt.event.*;
import javax.swing.*;

import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.AUtilities;
import atlantis.gui.AMutableCheckBox;
import atlantis.gui.AParametersTable;
import atlantis.gui.ATextField;

/**
 * A parameter representing a cut on a variable.
 */
public class ACutParameter extends AAbstractParameter {
  // name component
  private AMutableCheckBox checkBox;
  
  // value component
  private ATextField textField;
  
  private ActionListener statusActionListener, valueActionListener;
  
  public ACutParameter(
          String name, String screenName, String toolTip, String pv, int valueType, double value,
          String operators, String defOperator, boolean haseStatus, boolean status, boolean isMod,
          int userLevel, int scope, int unitsOf) {
    
    super(name, screenName, toolTip, valueType, value, operators, defOperator, haseStatus, status,
            isMod, userLevel, scope, unitsOf);
    
    range=resolvePossibleValues(pv, valueType);
  }
  
  public void setD(double v) {
    if(valueType!=FLOAT) throw new IllegalArgumentException("setD acces on not FLOAT parameter");
    
    if(textField!=null) textField.setText(""+v);
    _setD(v);
    
    refresh();
  }
  
  public void setI(int v) {
    if(valueType!=INT) throw new IllegalArgumentException("setI acces on not INT parameter");
    
    if(textField!=null) textField.setText(""+v);
    _setI(v);
    
    refresh();
  }
  
  public void initialize() {
    checkBox=new AMutableCheckBox(screenName);
    checkBox.setToolTipText(toolTip);
    if(getScope()==LOCAL)
        checkBox.setForeground(Color.black);
    else
        checkBox.setForeground(Color.blue);
    
    for(int i=0; i<operators.length; i++)
      checkBox.addItem(operators[i]);
    
    checkBox.finalizeConstruction();
    checkBox.setSelectedText(getOperator());
    
    statusActionListener=new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        ACommandProcessor.receive(name);
        apply();
      }
    };
    
    valueActionListener=new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        applyInput();
      }
    };
    
    checkBox.addStatusActionListener(statusActionListener);
    checkBox.addTextActionListener(valueActionListener);
    
    // after the first left click at the table in the parameter tabbed pane,
    // all the followingmouse mouse event will be catched by the mouselisteners of those
    // components inside the table cells, not the table itself, so in order for the local/global
    // switch (right click to change) to always work, have to add mouse listeners to these components
    // as well.
    checkBox.getCheckBox().addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if(AUtilities.isRightMouseButton(e)) {
          JPopupMenu popupMenu=new JPopupMenu();
          if(getScope()==AParameter.LOCAL) {
            popupMenu.add(AParametersTable.SET_GLOBAL).addActionListener(new ActionListener() {
              public void actionPerformed(ActionEvent e) {
                changeScope(AParameter.GLOBAL);
                refresh();
              }
            });
          } else {
            popupMenu.add(AParametersTable.SET_LOCAL).addActionListener(new ActionListener() {
              public void actionPerformed(ActionEvent e) {
                changeScope(AParameter.LOCAL);
                refresh();
              }
            });
          }
          popupMenu.show(checkBox.getCheckBox(), e.getX(), e.getY());
        }
      }
    });
    
    textField=new ATextField();
    textField.setToolTipText(toolTip);
    
    if(valueType==FLOAT)
      textField.setText(""+getD());
    else
      textField.setText(""+getI());
    
    textField.addActionListener(valueActionListener);
    
    /*
     textField.addFocusListener(new FocusAdapter() {
     public void focusLost(FocusEvent e) {
     applyInput();
     }
     });
     */
  }
  
  private void applyInput() {
    try {
      if(valueType==FLOAT) {
        double v=parseUnitsDouble(textField.getText());
        
        ACommandProcessor.receive(name+checkBox.getSelectedText()+v);
        apply();
      } else {
        int v=Integer.parseInt(textField.getText());
        
        ACommandProcessor.receive(name+checkBox.getSelectedText()+v);
        apply();
      }
    } catch(NumberFormatException nfe) {
      AOutput.append("value: "+textField.getText()+", is not correct!", ALogInterface.BAD_COMMAND);
    }
  }
  
  public JComponent getValueComponent() {
    return textField;
  }
  
  public JComponent getNameComponent() {
    return checkBox;
  }
  
  public void refresh() {
    if(textField!=null) {
      if(valueType==FLOAT)
        textField.setText(parseUnits(getD()));
      else
        textField.setText(""+getI());
    }
    if(checkBox!=null) checkBox.setStatus(getStatus());
    if(checkBox!=null) checkBox.setSelectedText(getOperator());
    fireParameterChanged();
  }
  
}
