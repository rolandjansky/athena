package atlantis.utils.xml;


import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


/**
 * Provides some basic XML utilities which are not present in the XML library.
 */
public class AXMLUtils
{
    public static Node getChild(Node node, String name)
    {
        NodeList childs = node.getChildNodes();

        for (int i = 0; i < childs.getLength(); i++)
        {
            Node child = childs.item(i);

            if (child.getNodeType() == Node.ELEMENT_NODE)
            {
                if (child.getNodeName().equals(name))
                    return child;
            }
        }
        return null;
    }

    public static String tryAttribut(NamedNodeMap attributes, String attName)
    {
        Node node = attributes.getNamedItem(attName);

        return (node != null) ? node.getNodeValue() : null;
    }

}
