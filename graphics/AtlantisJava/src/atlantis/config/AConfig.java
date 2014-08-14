package atlantis.config;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import atlantis.Atlantis;
import atlantis.utils.xml.AXMLErrorHandler;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;

/**
 * AConfig acts as a container of the configuration file.
 * The whole file can be accessed through a tree structure.
 * 
 * @author ql
 * 
 */

// by Zdenek, remark, 2008-08-14
// since the attributes are stored in something like HashMap
// their order is not ensured?, e.g. having in WindowCorners source
// configuration (master copy according to which synchronization is done)
// <CORNERS windowName="W" x0="-1320.0" y0="1320.0" x1="1320.0" y1="1320.0" x2="1320.0" y2="-1320.0"/>
// line order this way, after updating it result in something like this
// <CORNERS x2="1320.0" windowName="W" y0="1320.0" x1="1320.0" y2="-1320.0" y1="1320.0" x0="-1320.0"/>
// the same data, but the order of attributes is not the same
public class AConfig
{
    private static ALogger logger = ALogger.getLogger(AConfig.class);
    
    // A map defining the key attribute for each modifiable element
    static Hashtable keyMap;
    
    // Define modifiable attributes
    public static String[] CANVAS_ATT = {"startupLayout", "aspectRatio"}; //<Canvas>
    public static String[] WINDOW_ATT = {"projection", "group"};          //<UsedWindow>
    public static String[] LAYOUT_ATT = {"startup", "startupWindow"};     //<Layout>
    public static String[] PAR_ATT = {"va", "st", "dop", "scope", "ul"};  //<Others>
    public static String[] GROUP_ATT = {"userLevel"};                     //<Group>
    
    // Define used greek letters in configuration file
    static String[] GREEK_LETTER = {"\u03B7", "\u03C1", "\u03A6", "\u0394", "\u03BB", "\u03C3"};
    static String[] GREEK_ENTITY = {"&eta;", "&rho;", "&phi;", "&delta;", "&lambda;", "&sigma;"};
    
    // The name of the configuration file 
    private String filename;
    
    // If isCustomised is true, the configuration file contains two exclusive
    // blocks, <ParameterDifferences> and <WindowCorners>.
    private boolean isCustomised;
    
    // This field contains the whole DTD section of the configuration file.
    private String dtdSection = null;
    
    // Root node of the configuration tree
    private AConfigNode rootNode;
    
    // A list containing all element nodes in the file
    private ArrayList nodeList;
    
    // A list containing all user exclusive element nodes in the file
    // <ParameterDifferences> and <WindowCorners>
    private ArrayList userExclusiveNodeList;
    
    public AConfig(String configFilename, boolean isCustomised) throws Exception
    {
        if(AConfig.keyMap == null)
            AConfig.createKeyMap();
        this.filename = configFilename;
        this.isCustomised = isCustomised;
        parseXML();
    }
    
    /**
     * @return Returns the dtdSection.
     */
    public String getDtdSection() throws Exception
    {
        if(this.dtdSection == null)
            createDTDSection(this.filename);
        return this.dtdSection;
    }
    
    public Iterator getIterator()
    {
        return nodeList.iterator();
    }
    
    void addNode(AConfigNode node)
    {
        nodeList.add(node);
    }
    
    void addUserExclusiveNode(AConfigNode node)
    {
        userExclusiveNodeList.add(node);
    }
    
    /**
     * initialise <code>keyMap</code>.
     */
    private static void createKeyMap()
    {
        keyMap = new Hashtable();
        keyMap.put("Canvas", "startupLayout");
        keyMap.put("UsedWindow", "name");
        keyMap.put("Layout", "name");
        keyMap.put("ENUM", "fn");
        keyMap.put("SENUM", "fn");
        keyMap.put("ICUT", "fn");
        keyMap.put("SICUT", "fn");
        keyMap.put("FCUT", "fn");
        keyMap.put("SFCUT", "fn");
        keyMap.put("INT", "fn");
        keyMap.put("SINT", "fn");
        keyMap.put("FLOAT", "fn");
        keyMap.put("SFLOAT", "fn");
        keyMap.put("LINT", "fn");
        keyMap.put("SLINT", "fn");
        keyMap.put("COLOR", "fn");
        keyMap.put("SCOLOR", "fn");
        keyMap.put("STATUS", "fn");
    }
    
    /**
     * initialise <code>dtdSection</code> based on the dtd part from the 
     * configuration file.
     */
    private void createDTDSection(String configFilename) throws Exception
    {
        String dtdStart = "<?xml version=\"1.0\"?>";
        String dtdEnd = "]>";
        String LINE_SEPAR = System.getProperty("line.separator");
        
        this.dtdSection = "";
        try
        {
            
            InputStream is = AUtilities.getFileAsStream(configFilename);            
            InputStreamReader isr = new InputStreamReader(is);
            BufferedReader br = new BufferedReader(isr);       
            
            String currentLine;
            boolean isInsideDTD = false;
            while(true)
            {
                currentLine = br.readLine();
                if(currentLine == null) return;
                if(dtdStart.equals(currentLine.trim()))
                {
                    isInsideDTD = true;
                    this.dtdSection += currentLine + LINE_SEPAR;
                }
                else if(dtdEnd.equals(currentLine.trim()))
                {
                    isInsideDTD = false;
                    this.dtdSection += currentLine + LINE_SEPAR;
                }
                else if(isInsideDTD)
                {
                    this.dtdSection += currentLine + LINE_SEPAR;
                }
            }
        }
        catch(FileNotFoundException e)
        {
            String m = "File " + configFilename + " not found, fatal.";
            throw new Exception(m);
        }
        catch(IOException e)
        {
            String m = "I/O exception while reading file " + configFilename;
            throw new Exception(m);
        }
    }
    
    public void createConfigTree(Element rootElement)
    {
        this.nodeList = new ArrayList();
        if(this.isCustomised)
            this.userExclusiveNodeList = new ArrayList();
        this.rootNode = new AConfigNode(null, rootElement, 0, this);
    }
    
    public void parseXML() throws Exception
    {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setValidating(true);
        DocumentBuilder parser;
        try
        {
            parser = factory.newDocumentBuilder();
            parser.setErrorHandler(new AXMLErrorHandler());
            
            InputStream isConfig = AUtilities.getFileAsStream(this.filename);
            Document xmlConfigDoc = parser.parse(isConfig);
            createConfigTree(xmlConfigDoc.getDocumentElement());
        }
        catch (Exception e)
        {
            throw e;
        }
    }

    // Find a node match the sampleNode in the configuration file
    // Return the node if found, null otherwise
    public AConfigNode find(AConfigNode sampleNode)
    {
        for(int i=0; i<nodeList.size(); ++i)
        {
            AConfigNode testNode = (AConfigNode) nodeList.get(i);
            if("Canvas".equals(sampleNode.getNodeName())
                    && (testNode.getFullNodeName().equals(sampleNode.getFullNodeName())))
            {
                return testNode;
            }
            else if((testNode.getFullNodeName().equals(sampleNode.getFullNodeName())) 
                    && (testNode.getKeyValue().equals(sampleNode.getKeyValue())))
            {
                return testNode;
            }
        }
        return null;
    }
    
    // Find a node based on the node name, attribute name and value
    // Return the node if found, null otherwise
    public AConfigNode find(String name, String attName, String attValue)
    {
        for(int i=0; i<nodeList.size(); ++i)
        {
            AConfigNode testNode = (AConfigNode) nodeList.get(i);
            if(name.equals(testNode.getNodeName())
                    && attValue.equals(testNode.getAttValue(attName)))
                return testNode;
        }
        return null;
    }

    // Find a node based on the node name, attribute name and value,
    // and its parent's node name, attribute name and value.
    // Return the node if found, null otherwise
    public AConfigNode find(String name, String attName, String attValue, 
            String parName, String parAttName, String parAttValue)
    {
        for(int i=0; i<nodeList.size(); ++i)
        {
            AConfigNode testNode = (AConfigNode) nodeList.get(i);
            if(name.equals(testNode.getNodeName())
                    && attValue.equals(testNode.getAttValue(attName)))
            {
                AConfigNode parNode = testNode.getParent();
                
                if(parName.equals(parNode.getNodeName()) 
                        && parAttValue.equals(parNode.getAttValue(parAttName)))
                    return testNode;
            }
        }
        return null;
    }

    // Find a parameter node based on the attribute name and value,
    // and its group's name
    // Return the node if found, null otherwise
    //
    // NB: This method is for finding a parameter node with uncertain 
    // parameter type, but with a certain parent node name
    public AConfigNode findPar(String attName, String attValue, String parAttValue)
    {
        for(int i=0; i<nodeList.size(); ++i)
        {
            AConfigNode testNode = (AConfigNode) nodeList.get(i);
            if(attValue.equals(testNode.getAttValue(attName)))
            {
                AConfigNode parNode = testNode.getParent();
                
                // <StatusRoot> and <StatusGroup> are skipped when finding parent node
                while("StatusRoot".equals(parNode.getNodeName())
                        || "StatusGroup".equals(parNode.getNodeName()))
                {
                    parNode = parNode.getParent();
                }
                
                if("Group".equals(parNode.getNodeName()) 
                        && parAttValue.equals(parNode.getAttValue("name")))
                    return testNode;
            }
        }
        return null;
    }
    
    /**
     * validate the attribute value 
     */
    public boolean validate(AConfigNode node, String attName, String attValue)
    {
        if("Canvas".equals(node.getNodeName()) && "startupLayout".equals(attName))
        {
            if(find("Layout", "name", attValue) == null)
                return false;
        }
        else if("UsedWindow".equals(node.getNodeName()) && "projection".equals(attName))
        {
            if(find("Group", "name", attValue, "SuperGroup", "name", "Projection") == null)
                return false;
        }
        else if("UsedWindow".equals(node.getNodeName()) && "group".equals(attName))
        {
            if(find("Group", "name", attValue) == null)
                return false;
        }
        else if("Layout".equals(node.getNodeName()))
        {
            int numWindows = attValue.length();
            for(int i=0; i<numWindows; ++i)
            {                
                String windowName = attValue.substring(i, i+1);
                if(find("Window", "name", windowName, "Layout", "name", node.getKeyValue()) == null)
                    return false;
            }
        }
        return true;
    }
    
    public void validateUserExclusiveNode(AConfigNode node)
    {
        if("ParameterDifferences".equals(node.getNodeName()))
        {
            // check validity of each child of <ParameterDifferences>
            Iterator it = node.getChildren().iterator();
            while(it.hasNext())
            {
                // a <DIFFERENCE> node
                AConfigNode childNode = (AConfigNode) it.next();
                String group = childNode.getAttValue("group");
                String window = childNode.getAttValue("windowName");
                String parameter = childNode.getAttValue("name");
                if(find("UsedWindow", "name", window) == null)
                    it.remove();
                else
                {
                    if(find("Group", "name", group) == null)
                        it.remove();
                    else
                    {
                        if(findPar("fn", parameter, group) == null)
                            it.remove();
                    }
                }
            }
        }
        else if("WindowCorners".equals(node.getNodeName()))
        {
            // check validity of each child of <WindowCorners>
            Iterator it = node.getChildren().iterator();
            while(it.hasNext())
            {
                // a <CORNERS> node
                AConfigNode childNode = (AConfigNode) it.next();
                String window = childNode.getAttValue("windowName");
                if(find("UsedWindow", "name", window) == null)
                    it.remove();
            }
        }
    }
    
    /**
     * Replace the greek letters in the source string with UTF entities
     * 
     * @param testStr a source string
     * @return the resulting string
     */
    static String replaceGreekLetter(String testStr)
    {
        for (int i=0; i<GREEK_LETTER.length; ++i)
        {
            if (testStr.indexOf(GREEK_LETTER[i]) >= 0)
                testStr = testStr.replaceAll(GREEK_LETTER[i], GREEK_ENTITY[i]);
        }
        
        return testStr;
    }
    
    /**
     *  Converts to a string representing the xml data in this configuration
     *  file.
     */
    public String convertToString() throws Exception
    {
        return getDtdSection() + "\n\n" + this.rootNode.toString();
    }

    /**
     *  Converts to a string representing the xml data in this configuration
     *  file plus the user exclusive section fom <code>userConfig</code>.
     */
    public String convertToString(AConfig userConfig) throws Exception
    {
        if(!this.isCustomised && userConfig.isCustomised)
        {
            StringBuffer str = new StringBuffer("\n");
            for(int i=0; i<userConfig.userExclusiveNodeList.size(); ++i)
            {
                AConfigNode userExclusiveNode = (AConfigNode) userConfig.userExclusiveNodeList.get(i);
                validateUserExclusiveNode(userExclusiveNode);
                str.append(userExclusiveNode.toString());
                str.append("\n");
            }
            return getDtdSection() + "\n\n" + this.rootNode.toString(str.toString());
        }            
        else
        {
            // only add user exclusive section to non-customized configuration
            return toString();
        }
    }

}
