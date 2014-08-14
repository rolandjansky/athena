package atlantis.parameters;

import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import javax.swing.JComponent;

import atlantis.gui.AColorComboBoxRenderer;
import atlantis.gui.AComboBox;

/**
 * A parameter representing a selectable colour.
 */
public class AColorParameter extends AAbstractParameter {
  private AComboBox colorComboBox;

  public AColorParameter(
      String name, String screenName, String toolTip, String pv, double value,
      boolean haseStatus, boolean status, int userLevel, int scope) {

    super(name, screenName, toolTip, INT, value, null, null, haseStatus, status, false, userLevel,
        scope, NOTHING);

    range=resolvePossibleValues(pv, INT);
    if((range[SVAL]==null)&&(range[MIN]==null)) {
      range[MIN]=new double[] {0};
      range[MAX]=new double[] {29};
    }

  }

  public void setD(double v) {
    throw new IllegalArgumentException("is not allowed to use setD on ColorParameter");
  }

  public void setI(int v) {
    if(colorComboBox!=null)
      colorComboBox.setGUISelectedItem(new Integer(v));

    _setI(v);
    refresh();
    fireParameterChanged();
  }

  public void initialize() {
    super.initialize();
    colorComboBox=new AComboBox();
    colorComboBox.setRenderer(new AColorComboBoxRenderer());
    colorComboBox.setToolTipText(toolTip);

    int i, j;

    for(i=0; i<range[SVAL].length; i++) {
      int color=(int)range[SVAL][i];

      if((color>=0)&&(color<=29))
        colorComboBox.addItem(new Integer(color));
    }

    for(i=0; i<range[MIN].length; i++) {
      int color1=(int)range[MIN][i];
      int color2=(int)range[MAX][i];

      if((color1>=0)&&(color1<=29)&&(color2>=0)&&(color2<=29))
        for(j=color1; j<=color2; j++)
          colorComboBox.addItem(new Integer(j));
    }

    colorComboBox.setGUIItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        if(e.getStateChange()==ItemEvent.SELECTED) {
          int color=((Integer)colorComboBox.getSelectedItem()).intValue();

          ACommandProcessor.receive(name+"="+color);
          apply();
        }
      }
    });

    colorComboBox.setGUISelectedItem(new Integer(getI()));
  }

  public JComponent getValueComponent() {
    return colorComboBox;
  }

  public void refresh() {
    _refresh();
    if(colorComboBox!=null)
      colorComboBox.setGUISelectedItem(new Integer(getI()));
  }

}
