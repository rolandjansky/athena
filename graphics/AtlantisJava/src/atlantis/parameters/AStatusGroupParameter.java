package atlantis.parameters;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import javax.swing.JComponent;
import java.util.ArrayList;
import java.awt.Color;
import hep.wired.util.TristateCheckBox;

import atlantis.gui.ACheckNode;
import atlantis.gui.ACheckBox;

/**
 * Created on 16 March 2005, 16:15
 *
 * @author Qiang Lu
 */
public class AStatusGroupParameter extends AAbstractParameter
{
    private ArrayList<AParameter> parameterNodeList;

    private ACheckNode statusGroupNode;

    /** Creates a new instance of AStatusGroupParameter */
    AStatusGroupParameter(Node statusGroup, String name, String screenName, String toolTip, int userLevel, int scope)
    {
        super(name, screenName, toolTip, AParameter.NO_VALUE, 0, null, null, false, false, 
                false, userLevel, scope, AAbstractParameter.NOTHING);

        // get child element of statusGroup
        NodeList childrens = statusGroup.getChildNodes();
        // report error if no parameters
        if (childrens == null)
            throw new Error("StatusGroup element contains no parameters");
        int childrensCount = childrens.getLength();
        if (childrensCount == 0)
            throw new Error("StatusGroup element contains no parameters");

        // create an AParameter object for each node under statusGroup
        // and add them into parameterNodeList
        parameterNodeList = new ArrayList<AParameter>();
        for (int i = 0; i < childrensCount; i++)
        {
            Node child = childrens.item(i);

            if (child.getNodeType() == Node.ELEMENT_NODE)
            {
                // here we have a <StatusGroup> or <Status> element
                parameterNodeList.add(AParameterConfigReader.readParameter(child, scope));
            }
        }
    }

    public void initialize()
    {
        super.initialize();

        // create the tree node using the <StatusGroup> element
        TristateCheckBox groupCheckBox = new TristateCheckBox(screenName);
        groupCheckBox.setBackground(new Color(204, 204, 204));
        if (getScope() == LOCAL)
            groupCheckBox.setForeground(Color.black);
        else
            groupCheckBox.setForeground(Color.blue);

        groupCheckBox.setToolTipText(toolTip);
        statusGroupNode = new ACheckNode(groupCheckBox, false, ACheckNode.MULTI_SELECTION, this);

        for (AParameter aNodeParameter : parameterNodeList)
        {
            aNodeParameter.initialize();
            if (aNodeParameter instanceof AStatusParameter)
            {
                if (aNodeParameter.getUserLevel() <= APar.instance().getUserLevel())
                {
                    ACheckBox statusCheckBox = ((AStatusParameter) aNodeParameter).getCheckBox();
                    ACheckNode aNode = new ACheckNode(statusCheckBox, statusCheckBox.isSelected(), ACheckNode.SINGLE_SELECTION, aNodeParameter);
                    statusGroupNode.add(aNode);
                }
            }
            else if (aNodeParameter instanceof AStatusGroupParameter)
            {
                statusGroupNode.add(((AStatusGroupParameter) aNodeParameter).getCheckNode());
            }
        }
    }

    public JComponent getValueComponent()
    {
        return null;
    }

    public ACheckNode getCheckNode()
    {
        return statusGroupNode;
    }

    ArrayList<AParameter> getParameterList()
    {
        return parameterNodeList;
    }

    public AParameter getParameter(String name)
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            if (aNodeParameter instanceof AStatusParameter)
            {
                if (aNodeParameter.getName().equals(name))
                    return aNodeParameter;
            }
            else if (((AStatusGroupParameter) aNodeParameter).getParameter(name) != null)
            {
                return ((AStatusGroupParameter) aNodeParameter).getParameter(name);
            }
        }
        return null;
    }
    
    public AParameter getGroupParameter(String name)
    {
        if(getName().equals(name))
            return this;
        for (AParameter aNodeParameter : parameterNodeList)
        {
            if (aNodeParameter instanceof AStatusGroupParameter)
            {
                if (((AStatusGroupParameter) aNodeParameter).getGroupParameter(name) != null)
                {
                    return ((AStatusGroupParameter) aNodeParameter).getGroupParameter(name);
                }
            }
        }
        return null;
    }

    public boolean getStatus()
    {
        return statusGroupNode.isSelected();
    }
    
    public void setStatus(boolean status)
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            aNodeParameter.setStatus(status);
        }
    }
    
    public void refresh()
    {
        _refresh();
        fireParameterChanged();
    }

    public void setD(double v)
    {
        throw new IllegalArgumentException("StatusGroupParameter hase no value");
    }

    public void setI(int v)
    {
        throw new IllegalArgumentException("StatusGroupParameter hase no value");
    }

    public void saveChildrenDefaults()
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            if(aNodeAbstractParameter instanceof AStatusGroupParameter)
                ((AStatusGroupParameter)aNodeAbstractParameter).saveChildrenDefaults();
            else
                ((AStatusParameter)aNodeAbstractParameter).saveDefaults();
        }
    }

    public void restoreDefaults()
    {
        super.restoreDefaults();
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            aNodeAbstractParameter.restoreDefaults();
        }
    }

    public void copy(int from, int to) 
    {
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            aNodeAbstractParameter.copy(from, to);
        }
    }

    public String getDifferences(String groupName)
    {
        StringBuffer ret = new StringBuffer();
        
        for (AParameter aNodeParameter : parameterNodeList)
        {
            AAbstractParameter aNodeAbstractParameter = (AAbstractParameter) aNodeParameter;
            if(aNodeAbstractParameter instanceof AStatusGroupParameter)
                ret.append(((AStatusGroupParameter)aNodeAbstractParameter).getDifferences(groupName));
            else
                ret.append(aNodeAbstractParameter.getDifferences(groupName));
        }
        
        return ret.toString();
    }

}
