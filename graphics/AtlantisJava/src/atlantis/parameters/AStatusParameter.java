package atlantis.parameters;

import javax.swing.JLabel;
import javax.swing.LookAndFeel;
import javax.swing.JComponent;

/**
 * A parameter representing an on/off value. Corresponds to an ACheckBox in the GUI.
 */
public class AStatusParameter extends AAbstractParameter
{
    private JLabel valueLabel;

    public AStatusParameter(String name, String screenName, String toolTip,
                            boolean status, int userLevel, int scope)
    {
        super(name, screenName, toolTip, NO_VALUE, 0, null, null, true,
              status, false, userLevel, scope, NOTHING);
    }


    public void setD(double v)
    {
        throw new IllegalArgumentException("StatusParameter has no value");
    }


    public void setI(int v)
    {
        throw new IllegalArgumentException("StatusParameter has no value");
    }


    public void initialize()
    {
        super.initialize();
        valueLabel = new JLabel();
        valueLabel.setOpaque(true);
        valueLabel.setToolTipText(toolTip);
        LookAndFeel.installColorsAndFont(valueLabel, "CheckBox.background",
                                         "CheckBox.foreground",
                                         "CheckBox.font");
    }


    public JComponent getValueComponent()
    {
        return valueLabel;
    }


    public void refresh()
    {
        _refresh();
        fireParameterChanged();
    }

} // class AStatusParameter =================================================
