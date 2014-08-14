package atlantis.config;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.jar.Attributes;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class AConfigNode
{
    private String nodeName;
    
    // use fullNodeName combined with keyValue to uniquely identify a parameter
    // in the configuration file
    // NB: keyValue is not empty only if attributes is not empty
    private String fullNodeName;
    private String keyValue = null;
    
    // Layer 0 represents the root node of the xml file,
    // and layer 1 reprensents the layer inside layer 0,
    // and so on.
    private int layer;
    
    private Attributes attributes = null;
    private ArrayList children = null;
    private AConfigNode parent = null;
    
    // if parent is null, elementNode is the root node
    AConfigNode(AConfigNode parent, Node elementNode, int layer, AConfig configObject)
    {
        // add this node into the node list
        configObject.addNode(this);
        this.layer = layer;
        
        // set the full name
        this.parent = parent;
        String parentName = (this.parent != null) ? this.parent.getFullNodeName() + "-" : "";
        this.nodeName = elementNode.getNodeName();
        if("ParameterDifferences".equals(this.nodeName)
                || "WindowCorners".equals(this.nodeName))
            configObject.addUserExclusiveNode(this);
        this.fullNodeName = parentName + this.nodeName;
        
        // set the attributes map
        if(elementNode.hasAttributes())
        {
            NamedNodeMap attMap = elementNode.getAttributes(); 
            int numAttributes = attMap.getLength();
            this.attributes = new Attributes(numAttributes);
            for(int i=0; i<numAttributes; ++i)
            {
                // each node returned from item(i) is an Attr
                String name = attMap.item(i).getNodeName();
                String value = attMap.item(i).getNodeValue();
                // replace special math operator characters with entity references
                if("=".equals(value))
                    value = "&intEqual;";
                else if(">".equals(value))
                    value = "&gt;";
                else if("<".equals(value))
                    value = "&lt;";
                else if("<,>".equals(value))
                    value = "&real;";
                else if("<=".equals(value))
                    value = "&le;";
                else if(">=".equals(value))
                    value = "&ge;";
                else if("=,!=,<,<=,>,>=".equals(value))
                    value = "&int;";
                
                // replace greek letters with entity references
                value = AConfig.replaceGreekLetter(value);
                    
                this.attributes.putValue(name, value);
            }
        }
        
        // set the key value
        String keyAttr = (String) AConfig.keyMap.get(this.nodeName);
        if(keyAttr != null)
        {
            String groupName = this.parent.getAttValue("name");
            this.keyValue = ((groupName == null) ? "" : (groupName + "-")) + this.attributes.getValue(keyAttr);
        }
        
        // set the children list
        if(elementNode.hasChildNodes())
        {
            NodeList childList = elementNode.getChildNodes();
            int numChildren = childList.getLength()/2;
            this.children = new ArrayList(numChildren);
            int idx = 0;
            for(int i=0; i<childList.getLength(); ++i)
            {
                // each node returned from item(i) is an Attr
                Node childNode = childList.item(i);
                if (childNode.getNodeType() == Node.ELEMENT_NODE)
                {
                    this.children.add(idx++, new AConfigNode(this, childNode, this.layer+1, configObject));
                }
            }
        }
    }
    
    /**
     * @return Returns the nodeName.
     */
    public String getNodeName()
    {
        return this.nodeName;
    }
    
    /**
     * @return Returns the fullNodeName.
     */
    public String getFullNodeName()
    {
        return this.fullNodeName;
    }

    /**
     * @return Returns the parent.
     */
    public AConfigNode getParent()
    {
        return this.parent;
    }
    
    /**
     * @return Returns the children.
     */
    public List getChildren()
    {
        return this.children;
    }

    /**
     * @return Returns the keyValue.
     */
    public String getKeyValue()
    {
        return this.keyValue;
    }

    /**
     * @return Returns the string value of the specified attribute name,
     *         or null if not found.
     */
    public String getAttValue(String name)
    {
        if(this.attributes != null)
            return this.attributes.getValue(name);
        else
            return null;
    }
    
    public String setAttValue(String name, String value)
    {
        return this.attributes.putValue(name, value);
    }
    
    // the start line of this xml node
    // e.g. <Nodename att1="value1" att2="value2">
    private String selfStartString()
    {
        StringBuffer str = new StringBuffer();
        for(int i=0; i<this.layer; ++i)
        {
            str.append("\t");
        }
        str.append("<");
        str.append(this.nodeName);
        if(this.attributes != null)
        {
            Set attSet = this.attributes.keySet();
            Iterator it = attSet.iterator();
            while(it.hasNext())
            {
                String attName = it.next().toString();
                String attValue = this.attributes.getValue(attName);
                str.append(" ");
                str.append(attName);
                str.append("=\"");
                str.append(attValue);
                str.append("\"");
            }
        }
        if(this.children == null)
        {
            str.append("/>");
        }
        return str.toString();
    }
    
    private String selfEndString()
    {
        StringBuffer str = new StringBuffer();
        if(this.children == null)
        {
            // if no children, use simplified format (<.../>)
            // so selfStartString contains everything about this node
            return "";
        }
        else
        {
            for(int i=0; i<this.layer; ++i)
            {
                str.append("\t");
            }
            str.append("</");
            str.append(this.nodeName);
            str.append(">");
        }
        return str.toString();
    }
    
    private String childrenString()
    {
        StringBuffer str = new StringBuffer();
        if(this.children == null)
        {
            return "";
        }
        else
        {
            str.append(">\n");
            for(int i=0; i<this.children.size(); ++i)
            {
                AConfigNode child = (AConfigNode) this.children.get(i);
                str.append(child.toString());
                str.append("\n");
            }
        }
        return str.toString();
    }
    
    /**
     *  Converts to a string representing the xml data containing this node and
     *  all its children.
     */
    public String toString()
    {
        return selfStartString()+childrenString()+selfEndString();
    }
    
    // userExclusiveSection is noly under root node (layer 0)
    public String toString(String userExclusiveSection)
    {
        if(this.layer == 0)
        {
            return selfStartString()+childrenString()+userExclusiveSection+selfEndString();
        }
        else
        {
            return toString();
        }
    }
    
}
