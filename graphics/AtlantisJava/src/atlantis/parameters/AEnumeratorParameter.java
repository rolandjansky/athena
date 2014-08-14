package atlantis.parameters;

import java.util.StringTokenizer;
import java.util.Hashtable;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import javax.swing.JComponent;

import atlantis.output.AExceptionHandler;
import atlantis.gui.AComboBox;


/**
 * A parameter that displays a finite list of possible values.
 * Used to contain items in pTexts String[], reimplemented to contain items
 * in Hash in order to be able to change the contents from event to event
 * (because of the multiple collections cuts).
 */
public class AEnumeratorParameter extends AAbstractParameter
{
    private AComboBox comboBox;
    private Hashtable<Integer, String> pTexts;



    public AEnumeratorParameter(String name, String screenName, String toolTip,
                                String possibleValues, double value,
                                boolean haseStatus, boolean status,
                                int userLevel, int scope)
    {
        super(name, screenName, toolTip, INT, value, null, null, haseStatus,
              status, false, userLevel, scope, NOTHING);

        StringTokenizer st, tokenSt;
        st = new StringTokenizer(possibleValues, ",");
        int num = st.countTokens();
        if(num <= 0)
        {
            throw new Error("Empty \"pv\" attribute for ENUM: " + name);
        }
        pTexts = new Hashtable<Integer, String>();
        comboBox = new AComboBox();
        comboBox.setName(screenName);

        range = new double[3][];
        range[MIN] = null;
        range[MAX] = null;
        range[SVAL] = new double[num];

        for(int i = 0; i < num; i++)
        {
            tokenSt = new StringTokenizer(st.nextToken(), "=");
            if(tokenSt.countTokens() != 2)
            {
                throw new Error("Incomplete \"text = value\" declaration " +
                                "for ENUM: " + name);
            }
            pTexts.put(new Integer(i), tokenSt.nextToken().trim());

            try
            {
                if(valueType == FLOAT)
                {
                    range[SVAL][i] =
                        Double.parseDouble(tokenSt.nextToken().trim());
                }
                else if(valueType == INT)
                {
                    range[SVAL][i] =
                        Integer.parseInt(tokenSt.nextToken().trim());
                }
            }
            catch(NumberFormatException nfe)
            {
                String msg = "Number error for ENUM: " + name + possibleValues;
                AExceptionHandler.processException(msg, nfe);
            }
        }

    } // AEnumeratorParameter() ---------------------------------------------



    public void setD(double v)
    {
        if(valueType != FLOAT)
        {
            throw new Error("setD access on not FLOAT parameter");
        }
        if(comboBox != null)
        {
            comboBox.setGUISelectedItem(getText(v));
        }
        _setD(v);
        refresh();

    } // setD() -------------------------------------------------------------



    public void setI(int v)
    {
        if(valueType != INT)
        {
            throw new Error("setI access on not INT parameter");
        }
        if(comboBox != null)
        {
            comboBox.setGUISelectedItem(getText(v));
        }
        _setI(v);
        refresh();

    } // setI() -------------------------------------------------------------



    public String getCurrentText()
    {
        return getText(getI());

    } // getCurrentText() ---------------------------------------------------



    public void clear()
    {
        pTexts.clear();

    } // clear() ------------------------------------------------------------



    public Hashtable<Integer, String> getValuesHashtable()
    {
        return pTexts;

    } // getValuesHashtable() -----------------------------------------------



    public int size()
    {
        return pTexts.size();

    } // size() -------------------------------------------------------------



    public String getText(double v)
    {
        for(int i = 0; i < pTexts.size(); i++)
        {
            if(range[SVAL][i] == v)
            {
                return pTexts.get(new Integer(i));
            }
        }
        return null;

    } // getText() ----------------------------------------------------------



    public double getValue(String text)
    {
        for(int i = 0; i < pTexts.size(); i++)
        {
            if(text.equals(pTexts.get(new Integer(i))))
            {
                return range[SVAL][i];
            }
        }
        return 0;

    } // getValue() ---------------------------------------------------------



    public void addItem(int key, String item)
    {
        pTexts.put(new Integer(key), item);
        range[SVAL] = new double[pTexts.size()];
        for(int i = 0; i < pTexts.size(); i++)
        {
            range[SVAL][i] = i;
        }

    } // addItem() ----------------------------------------------------------


    public void removeItem(String itemToRemove)
    {
        Hashtable<Integer, String> pTextsNew = new Hashtable<Integer, String>();
        for(int i = 0; i < pTexts.size(); i++)
        {
            String item = pTexts.get(new Integer(i));
            if(itemToRemove.equals(item))
            {
                continue;
            }
            else
            {
                pTextsNew.put(new Integer(pTextsNew.size()), item);
            }
        }
        pTexts = pTextsNew;

        range[SVAL] = new double[pTexts.size()];
        for(int i = 0; i < pTexts.size(); i++)
        {
            range[SVAL][i] = i;
        }

    } // removeItem() -------------------------------------------------------



    /**
     * Initialize the contained AComboBox with the list of items stored in this AEnumeratorParameter.
     * Also sets a listener to send the appropriate command when an item is selected.
     */
    public void initialize()
    {
        super.initialize();
        // remove all existing items from, in the constructor created, comboBox
        // rather than create a new instance here, because after the first
        // call of this initialize() method is not possible to update items
        // in the comboBox
        comboBox.removeAllItems();
        comboBox.setToolTipText(toolTip);
        for(int i = 0; i < pTexts.size(); i++)
        {
            String item = pTexts.get(new Integer(i));
            comboBox.addItem(item);
        }
        comboBox.setGUISelectedItem(getText(getI()));
        if (!comboBox.hasGUIItemListener()) {
        	comboBox.setGUIItemListener(new ItemListener()
        	{
        		public void itemStateChanged(ItemEvent e)
        		{
        			if(e.getStateChange() == ItemEvent.SELECTED)
        			{
        				int index = comboBox.getSelectedIndex();
        				String command =  name + "=" + range[SVAL][index];
        				ACommandProcessor.receive(command);
        				apply();
        			}
        		}
        	});
        }
        refresh();
    } // initialize() -------------------------------------------------------



    public JComponent getValueComponent()
    {
        return comboBox;

    } // getValueComponent() ------------------------------------------------



    public void refresh()
    {
        _refresh();
        if(comboBox != null)
        {
            comboBox.setGUISelectedItem(getText(getI()));
        }
        fireParameterChanged();
    } // refresh() ----------------------------------------------------------



} // class AEnumeratorParameter =============================================
