package atlantis.parameters;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPopupMenu;
import javax.swing.LookAndFeel;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.gui.ACheckBox;
import atlantis.gui.AParametersTable;
import atlantis.output.AExceptionHandler;
import atlantis.utils.AUtilities;

/**
 * The class is used as a default implementation of the AParameter interface.
 * All parameters extend this class except ALinkParameter which implements
 * AParameter directly.
 */
abstract class AAbstractParameter implements AParameter
{
    // range constants
    protected final static int MIN = 0;
    protected final static int MAX = 1;
    protected final static int SVAL = 2;
    
    // unitsOf constants
    protected final static int NOTHING = 0;
    protected final static int LENGTH = 1;
    protected final static int ENERGY = 2;
    protected final static int ENERGYMEV = 3;
    
    /**
     * List of listeners/observers to be informed of changes to parameters.
     * Currently this is just the GUI, which repaints its tables when a parameter
     * changes.
     */
    private List<AParameterChangeListener> parameterChangeListeners = new ArrayList<AParameterChangeListener>();
    
    /** The internal name of the parameter. */
    protected String name;
    /** The name which is displayed in the user interface. */
    protected String screenName;
    /** Tool tip, i.e. text that appears when mouse hovers over control. */
    protected String toolTip;
    
    protected boolean hasStatus;
    protected int valueType;
    protected int scope;
    protected int defaultScope;
    protected boolean isModulus;
    protected int userLevel;
    protected int unitsOf;
    protected String[] operators;
    /** Allowed values for this parameter:
     *    range[MIN][i] and range[MAX][i] are the limits of the i'th range;
     *    range[SVAL][j] is the j'th allowed single value. */
    protected double[][] range;
    
    private ACheckBox nameLabel;
    private JLabel label;
    private boolean isInitialized = false;

    /** The number of windows in the canvas. */
    protected int wCount = 0; // initialised later in constructor

    /** Contains the values for each window. */
    protected AParameterData data;

	@Override
	public void addParameterChangeListener(AParameterChangeListener listener) {
		parameterChangeListeners.add(listener);		
	}

	public AAbstractParameter(String name, String screenName, String toolTip, 
            int valueType, double value, String op, String dop, 
            boolean haseStatus, boolean status, boolean isMod, int userLevel, 
            int scope, int unitsOf)
    {
        // initialising wCount
        // for running without Canvas (e.g. calling geometry methods from GraXML),
        // this may run without Canvas initialised
        // check if Canvas actually exists
        // dependency of AAbstractParameter on ACanvas should be revised, this
        // current solution is not very elegant
        ACanvas canvas = ACanvas.getCanvas();
        this.wCount = canvas == null ? 0 : canvas.getWindowsCount();
        
        this.isModulus = isMod;
        this.unitsOf = unitsOf;
        
        if (!screenName.trim().equals(""))
            this.screenName = screenName;
        else
            this.screenName = name;
        
        if (isMod)
            this.screenName = "|" + this.screenName + "|";
        
        // removing spaces from the parameter name
        StringBuffer strBuffer = new StringBuffer(name);
        int i = 0;
        
        while (i < strBuffer.length())
        {
            if (Character.isWhitespace(strBuffer.charAt(i)))
                strBuffer.deleteCharAt(i);
            else
                i++;
        }
        this.name = strBuffer.toString();
        
        if ((toolTip == null) || (toolTip.trim().length() == 0))
            this.toolTip = this.screenName;
        else
            this.toolTip = toolTip;
        
        this.hasStatus = haseStatus;
        this.valueType = valueType;
        
        // set up the local window copies
        this.data = new AParameterData(wCount);
        
        for (i = 0; i < wCount; i++)
        {
            this.data.setValue(i, value);
            this.data.setStatus(i, status);
        }
        
        // decompose supportedOperators
        if ((op == null) || (op.trim().equals("")))
        {
            this.operators = new String[1];
            operators[0] = "=";
        }
        else
        {
            String[] tempOperators = new String[10];
            
            int operatorsCount = 0;
            StringTokenizer st = new StringTokenizer(op, ",");
            
            while (st.hasMoreTokens())
            {
                String oper = st.nextToken().trim();
                
                if (isKnownOperator(oper))
                {
                    tempOperators[operatorsCount] = oper;
                    operatorsCount++;
                }
                else
                    throw new Error("Unknown operator: " + oper);
            }
            
            this.operators = new String[operatorsCount];
            for (i = 0; i < operatorsCount; i++)
                this.operators[i] = tempOperators[i];
        }
        
        if ((dop == null) || (dop.trim().equals("")))
            for (i = 0; i < wCount; i++)
                this.data.setOperator(i, operators[0]);
        else
            for (i = 0; i < wCount; i++)
                this.data.setOperator(i, dop);
        
        this.userLevel = userLevel;
        setScope(scope);
        saveDefaults();
    }
    
    public void saveDefaults()
    {
    	this.data.saveDefaults();
        this.defaultScope = this.scope;
    }
    
    public void restoreDefaults()
    {
    	this.data.restoreDefaults();
        changeScope(this.defaultScope);
    }
    
    /**
     * This method is called to apply the change in the value of the parameter
     */
    public void apply()
    {
        if (getScope() == LOCAL)
            ACommandProcessor.receive("DO");
        else
            ACanvas.getCanvas().repaintAllFromScratch();
    }
    
    /**
     * Called by the User Interface in order to initialize the graphics of the
     * parameter (if it will be displayed in the UI).
     */
    public void initialize()
    {
        if (hasStatus)
        {
            nameLabel = new ACheckBox(screenName);
            nameLabel.setSelected(getStatus());
            nameLabel.setBackground(new Color(204, 204, 204));
            if (getScope() == LOCAL)
                setForeground(Color.black);
            else
                setForeground(Color.blue);
            
            nameLabel.setToolTipText(toolTip);
            nameLabel.addActionListener(new ActionListener()
                    {
                public void actionPerformed(ActionEvent e)
                {
                    ACommandProcessor.receive(name);
                    apply();
                }
                    });
            
            // after the first left click at the table in the parameter tabbed
            // pane, all the followingmouse mouse event will be catched by the
            // mouselisteners of those components inside the table cells, not 
            // the table itself, so in order for the local/global
            // switch (right click to change) to always work, have to add mouse
            // listeners to these components as well.
            nameLabel.addMouseListener(new MouseAdapter()
                    {
                public void mousePressed(MouseEvent e)
                {
                    if (AUtilities.isRightMouseButton(e))
                    {
                        JPopupMenu popupMenu = new JPopupMenu();
                        if (getScope() == AParameter.LOCAL)
                        {
                            popupMenu.add(AParametersTable.SET_GLOBAL).addActionListener(new ActionListener()
                                    {
                                public void actionPerformed(ActionEvent e)
                                {
                                    changeScope(AParameter.GLOBAL);
                                    refresh();
                                }
                                    });
                        }
                        else
                        {
                            popupMenu.add(AParametersTable.SET_LOCAL).addActionListener(new ActionListener()
                                    {
                                public void actionPerformed(ActionEvent e)
                                {
                                    changeScope(AParameter.LOCAL);
                                    refresh();
                                }
                                    });
                        }
                        popupMenu.show(nameLabel, e.getX(), e.getY());
                    }
                }
                    });
        }
        else
        {
            label = new JLabel(screenName);
            if (getScope() == LOCAL)
                setForeground(Color.black);
            else
                setForeground(Color.blue);
            
            // after the first left click at the table in the parameter tabbed
            // pane, all the followingmouse mouse event will be catched by the
            // mouselisteners of those components inside the table cells, not 
            // the table itself, so in order for the local/global
            // switch (right click to change) to always work, have to add mouse
            // listeners to these components as well.
            label.addMouseListener(new MouseAdapter()
                    {
                public void mousePressed(MouseEvent e)
                {
                    if (AUtilities.isRightMouseButton(e))
                    {
                        JPopupMenu popupMenu = new JPopupMenu();
                        if (getScope() == AParameter.LOCAL)
                        {
                            popupMenu.add(AParametersTable.SET_GLOBAL).addActionListener(new ActionListener()
                                    {
                                public void actionPerformed(ActionEvent e)
                                {
                                    changeScope(AParameter.GLOBAL);
                                    refresh();
                                }
                                    });
                        }
                        else
                        {
                            popupMenu.add(AParametersTable.SET_LOCAL).addActionListener(new ActionListener()
                                    {
                                public void actionPerformed(ActionEvent e)
                                {
                                    changeScope(AParameter.LOCAL);
                                    refresh();
                                }
                                    });
                        }
                        popupMenu.show(label, e.getX(), e.getY());
                    }
                }
                    });
            label.setOpaque(true);
            label.setToolTipText(toolTip);
            LookAndFeel.installColorsAndFont(label, "CheckBox.background", "CheckBox.foreground", "CheckBox.font");
        }
        
        isInitialized = true;
    }
    
    public boolean isInitialized()
    {
        return this.isInitialized;
    }
    
    /**
     * Copies from one window to another the value of the parameter
     */
    public void copy(int from, int to)
    {
    	data.copy(from,to);
    }
    
    /**
     * This function is called for GLOBAL parameters to apply the value in all
     * windows.
     */
    public void globalize(int window)
    {
    	data.globalize(window);
    }
    
    public void setScope(int scope)
    {
        this.scope = scope;
        if (scope == LOCAL)
            setForeground(Color.black);
        else
            setForeground(Color.blue);
    }
    
    public void changeScope(int scope)
    {
        if (scope != this.scope)
        {
            this.scope = scope;
            if (scope == LOCAL)
                setForeground(Color.black);
            else
            {
                setForeground(Color.blue);
                data.globalize();
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
    }
    
    public int getScope()
    {
        return scope;
    }
    
    public String getName()
    {
        return name;
    }
    
    public String getScreenName()
    {
        return screenName;
    }
    
    public String getToolTip()
    {
        return toolTip;
    }
    
    // public String getOperator() { return operator; }
    public boolean isModulus()
    {
        return isModulus;
    }
    
    public int getValueType()
    {
        return valueType;
    }
    
    public String getOperator()
    {
        return data.getOperator();
    }
    
    protected void _setOperator(String oper)
    {
        // if (!validateValue(v))
        // throw new Error("Value " + v + " for parameter " + screenName + " is
        // out of range ");
        data.setOperator(oper);
        if (scope == GLOBAL) data.globalize();
    }
    
    protected void _setOperator(String windowName, String oper)
    {
        if (scope == GLOBAL || windowName.equals("*")) {
            data.setOperator(oper);
            data.globalize();
        } else {
            data.setOperator(ACanvas.getCanvas().getWindow(windowName).getIndex(), oper);
        }
    }
    
    protected void _setValue(String windowName, double val)
    {
        if (scope == GLOBAL || windowName.equals("*")) {
            data.setValue(val);
            data.globalize();
        } else {
            data.setValue(ACanvas.getCanvas().getWindow(windowName).getIndex(), val);
        }
    }
    
    protected void _setStatus(String windowName, boolean stat)
    {
        if (scope == GLOBAL || windowName.equals("*")) {
            data.setStatus(stat);
            data.globalize();
        } else {
            data.setStatus(ACanvas.getCanvas().getWindow(windowName).getIndex(), stat);
        }
    }
    
    /**
     * _setD and _setI are used by subclasses when changing the values of
     * parameters.
     */
    protected void _setD(double v)
    {
        if (!validateValue(v))
            throw new Error("Value " + v + " for parameter " + screenName + " is out of range ");
        data.setValue(v);
        if (scope == GLOBAL) data.globalize();
    }
    
    protected void _setI(int v)
    {
        if (!validateValue(v))
            throw new Error("Value " + v + " for parameter " + screenName + " is out of range ");
        data.setValue(v);
        if (scope == GLOBAL) data.globalize();
    }
    
    public double getD()
    {
        if (valueType != FLOAT)
            throw new Error("getD acces on not FLOAT parameter");
        return data.getValue();
    }
    
    public int getI()
    {
        if (valueType != INT)
            throw new Error("getI acces on not INT parameter");
        return (int) data.getValue();
    }
    
    public String getValue()
    {
        if (valueType == FLOAT)
            return Double.toString(data.getValue());
        else
            return Integer.toString((int) data.getValue());
    }
    
    public void setStatus(boolean st)
    {
        if (!hasStatus)
            throw new Error("Parameter has no status");
        data.setStatus(st);
        if (scope == GLOBAL) data.globalize();
        
        refresh();
        fireParameterChanged();
    }
    
    public boolean getStatus()
    {
        if (!hasStatus)
            throw new Error("Parameter hase no status");
        
        return data.getStatus();
    }
    
    public JComponent getNameComponent()
    {
        if (hasStatus)
            return nameLabel;
        else
            return label;
    }
    
    public JComponent getValueComponent()
    {
        return new JLabel("error");
    }
    
    protected void _refresh()
    {
        if (hasStatus)
            if (nameLabel != null)
                nameLabel.setSelected(getStatus());
    }
    
    /**
     * Parses the PossibleValues string and returns the double[][] containing
     * the range of this parameter.
     */
    protected static double[][] resolvePossibleValues(String possibleValues, int type)
    {
        double[][] range = new double[3][100];
        int valCount = 0;
        int rangeCount = 0;
        StringTokenizer st = new StringTokenizer(possibleValues, ",");
        String sval;
        double val = 0;
        
        while (st.hasMoreTokens())
        {
            String token = st.nextToken().trim();
            StringTokenizer st1 = new StringTokenizer(token, ":");
            
            switch (st1.countTokens())
            {
                case 1:
                    sval = st1.nextToken().trim();
                    try
                    {
                        if (type == FLOAT)
                            val = Double.parseDouble(sval);
                        else if (type == INT)
                            val = Integer.parseInt(sval);
                        range[SVAL][valCount] = val;
                        valCount++;
                    }
                    catch (NumberFormatException nfe)
                    {
                        AExceptionHandler.processException("Corrupt Possible Value: " + sval, nfe);
                    }
                    break;
                    
                case 2:
                    for (int i = 0; i < 2; i++)
                    {
                        sval = st1.nextToken().trim();
                        try
                        {
                            if (type == FLOAT)
                                val = Double.parseDouble(sval);
                            else if (type == INT)
                                val = Integer.parseInt(sval);
                        }
                        catch (NumberFormatException nfe)
                        {
                            if (sval.equals("inf"))
                                val = Double.POSITIVE_INFINITY;
                            else if (sval.equals("-inf"))
                                val = Double.NEGATIVE_INFINITY;
                            else
                                AExceptionHandler.processException("Corrupt Possible Range: (" + token + ")", nfe);
                        }
                        range[i][rangeCount] = val;
                    }
                    rangeCount++;
                    break;
                    
                default:
                    AExceptionHandler.processException("fatal", "Corrupt \"pv\" definition: \"" + possibleValues + "\"");
                break;
            }
        }
        
        double[] min = new double[rangeCount];
        double[] max = new double[rangeCount];
        
        for (int i = 0; i < rangeCount; i++)
            if (range[MIN][i] <= range[MAX][i])
            {
                min[i] = range[MIN][i];
                max[i] = range[MAX][i];
            }
            else
                throw new Error("Bad range definiton: (" + range[0][i] + ":" + range[1][i] + ")");
        
        double[] v = new double[valCount];
        
        for (int i = 0; i < valCount; i++)
            v[i] = range[SVAL][i];
        
        range[MIN] = min;
        range[MAX] = max;
        range[SVAL] = v;
        return range;
    }
    
    protected static int[] getValues(String possibleValues)
    {
        int[] pVal = new int[100];
        int[] range = new int[2];
        int i, valCount = 0;
        String token;
        StringTokenizer st, st1;
        
        st = new StringTokenizer(possibleValues, ",");
        while (st.hasMoreTokens())
        {
            token = st.nextToken().trim();
            st1 = new StringTokenizer(token, ":");
            switch (st1.countTokens())
            {
                case 1:
                    try
                    {
                        pVal[valCount++] = Integer.parseInt(st1.nextToken().trim());
                    }
                    catch (NumberFormatException nfe)
                    {
                        AExceptionHandler.processException("Corrupt Possible Value", nfe);
                    }
                    break;
                    
                case 2:
                    for (i = 0; i < 2; i++)
                        try
                    {
                            range[i] = Integer.parseInt(st1.nextToken().trim());
                    }
                    catch (NumberFormatException nfe)
                    {
                        AExceptionHandler.processException("fatal", "error in range definition");
                    }
                    
                    if (range[0] <= range[1])
                        for (i = range[0]; i <= range[1]; i++)
                            pVal[valCount++] = i;
                    break;
                    
                default:
                    AExceptionHandler.processException("fatal", "Corrupt range definition: \"" + possibleValues + "\"");
            }
        }
        
        int[] pValFinal = new int[valCount];
        
        for (i = 0; i < valCount; i++)
            pValFinal[i] = pVal[i];
        
        return pValFinal;
    }
    
    public boolean processCommand(String oper, double v)
    {
        // first I validate the operator and the value
        if (!validateOperator(oper))
            return false;
        if (!validateValue(v))
            return false;
        
        if (valueType == FLOAT)
            _setD(v);
        else
            _setI((int) v);
        
        _setOperator(oper);
        
        refresh();
        fireParameterChanged();

        return true;
    }
    
    public boolean validateValue(double v)
    {
        if (range[SVAL] != null)
            for (int i = 0; i < range[SVAL].length; i++)
                if (v == range[SVAL][i])
                    return true;
        
        if (range[MIN] != null)
            for (int i = 0; i < range[MIN].length; i++)
                if ((v >= range[MIN][i]) && (v <= range[MAX][i]))
                    return true;
        
        return false;
    }
    
    public boolean validateOperator(String oper)
    {
        for (int i = 0; i < operators.length; i++)
            if (oper.equals(operators[i]))
                return true;
        
        return false;
    }
    
    public int getUserLevel()
    {
        return userLevel;
    }
    

    public String toString()
    {
        StringBuilder s = new StringBuilder(screenName + ": ");
        
        if (scope == GLOBAL)
            s.append("GLOBAL");
        else if (scope == LOCAL)
            s.append("LOCAL");
        
        s.append("[");
        
        // for running without Canvas (e.g. calling geometry methods from GraXML),
        // this may run without Canvas initialised
        // check if Canvas actually exists
        // dependency of AAbstractParameter on ACanvas should be revised, this
        // current solution is not very elegant
        ACanvas canvas = ACanvas.getCanvas();
        String[] wName = canvas == null ? new String[0] : canvas.getKnownWindowNames();
        
        for (int i = 0; i < wName.length; i++)
        {
            AWindow w = ACanvas.getCanvas().getWindow(wName[i]);
            
            s.append(wName[i] + ": " + data.getValue(w.getIndex()) + ": ");
        }
        
        return s.toString() + "]" + " :" + toolTip + ":";
    }

    
    private final void setForeground(Color color)
    {
        if (getNameComponent() != null)
            getNameComponent().setForeground(color);
    }
    
    public String getDifferences(String groupName)
    {
        StringBuffer b = new StringBuffer();
        String[] windowNames = ACanvas.getCanvas().getKnownWindowNames();
        int[] inverse = new int[windowNames.length];
        for (int i = 0; i < windowNames.length; i++)
            inverse[ACanvas.getCanvas().getWindow(windowNames[i]).getIndex()] = i;
        
        for (int i = 0; i < wCount; ++i)
        {
            if (data.getStatus() != data.getStatus(i) || !data.getOperator().equals(data.getOperator(i)) || data.getValue() != data.getValue(i))
            {
                
                b.append("\t\t<DIFFERENCE group=\"" + groupName + "\" name=\"" + getName() + "\" windowName=\"" + windowNames[inverse[i]] + "\"");
                
                if (data.getValue() != data.getValue(i))
                    b.append(" va=\"" + data.getValue(i) + "\"");
                if (data.getStatus() != data.getStatus(i))
                {
                    if (data.getStatus(i))
                        b.append(" st=\"ON\"");
                    else
                        b.append(" st=\"OFF\"");
                }
                if (!data.getOperator().equals(data.getOperator(i)))
                    b.append(" op=\"" + data.getOperator(i) + "\"");
                b.append("/>\n");
            }
        }
        return b.toString();
    }
    
    protected double parseUnitsDouble(String text)
    {
        // units shown are cm
        if (unitsOf == LENGTH)
        {
            if (text.endsWith("MM") || text.endsWith("mm"))
                return 0.1 * Double.parseDouble(text.substring(0, text.length() - 2));
            if (text.endsWith("CM") || text.endsWith("cm"))
                return Double.parseDouble(text.substring(0, text.length() - 2));
            if (text.endsWith("UM") || text.endsWith("um"))
                return 0.0001 * Double.parseDouble(text.substring(0, text.length() - 2));
            else if (text.endsWith("M") || text.endsWith("m"))
                return 100. * Double.parseDouble(text.substring(0, text.length() - 1));
            else
                return Double.parseDouble(text.substring(0, text.length()));
        }
        else if (unitsOf == ENERGY)
        {
            if (text.endsWith("GeV") || text.endsWith("gev") || text.endsWith("GEV"))
                return Double.parseDouble(text.substring(0, text.length() - 3));
            else if (text.endsWith("MeV") || text.endsWith("mev") || text.endsWith("MEV"))
                return .001 * Double.parseDouble(text.substring(0, text.length() - 3));
            else
                return Double.parseDouble(text);
        }
        else if (unitsOf == ENERGYMEV)
        {
            if (text.endsWith("MeV") || text.endsWith("mev") || text.endsWith("MEV"))
                return Double.parseDouble(text.substring(0, text.length() - 3));
            else if (text.endsWith("KeV") || text.endsWith("kev") || text.endsWith("KEV"))
                return .001 * Double.parseDouble(text.substring(0, text.length() - 3));
            else if (text.endsWith(" eV") || text.endsWith(" ev") || text.endsWith(" EV"))
                return .000001 * Double.parseDouble(text.substring(0, text.length() - 3));
            else
                return Double.parseDouble(text);
        }
        else
        {
            return Double.parseDouble(text);
        }
    }
    
    protected String parseUnits(double d)
    {
        // units shown are cm
        double abs = Math.abs(d);
        if (unitsOf == LENGTH)
        {
            if (abs >= 100.)
                return "" + trim(d, 100.) + " m";
            else if (abs >= 1.)
                return "" + trim(d, 1.) + " cm";
            else if (abs >= 0.1)
                return "" + trim(d, 0.1) + " mm";
            else
                return "" + trim(d, 0.0001) + " um";
        }
        else if (unitsOf == ENERGY)
        {
            if (abs >= 1.)
                return "" + trim(d, 1.) + " GeV";
            else
                return "" + trim(d, 0.001) + " MeV";
        }
        else if (unitsOf == ENERGYMEV)
        {
            if (abs >= 1.)
                return "" + trim(d, 1.) + " MeV";
            else if(abs >=0.001)
                return "" + trim(d, 0.001) + " KeV";
            else
                return "" + trim(d, 0.000001) + " eV";
        }
        else
        {
            return "" + trim(d, 1.);
        }
    }
    
    private String trim(double d, double factor)
    {
        // only display with 3 significant digits
        if (unitsOf == LENGTH && Math.abs(d) < .1)
            return "" + Math.rint(d / factor);
        else if (unitsOf == ENERGY && Math.abs(d) < 1.)
            return "" + Math.rint(d / factor);
        else if (unitsOf == ENERGYMEV && Math.abs(d) < 1.)
            return "" + Math.rint(d / factor);
        else
            return "" + Math.rint(1000. * d / factor) / 1000.;
    }
    
    public ACheckBox getCheckBox()
    {
        return this.nameLabel;
    }
    
    /**
     * Check if a given operator is one of the allowed supportedOperators.
     * @param oper the operator to check
     * @return true if this is a known operator
     */
    protected static boolean isKnownOperator(String oper) 
    {
        for(int i=0; i<AParameter.supportedOperators.length; i++)
            if(oper.equals(AParameter.supportedOperators[i]))
                return true;
        
        return false;
    }

	/**
	 * Tell the listeners that a parameter has changed.
	 */
	protected void fireParameterChanged() {
		for (AParameterChangeListener listener : parameterChangeListeners) {
			listener.parameterChanged(this);
		}
	}
}
