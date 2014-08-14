package atlantis.parameters;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * A supergroup of parameters, i.e. a group of groups, corresponding to one of the
 * top-level tabs in the GUI, e.g. "Projections" or "Data".
 * 
 * @author waugh
 *
 */
public class AParameterSuperGroup implements AParameterChangeListener {
	private String name;
	private String toolTip;
	private int userLevel;
	
	private Map<String,AParametersGroup> parameterGroups = new HashMap<String,AParametersGroup>();
	private List<AParametersGroup> uiGroups = new ArrayList<AParametersGroup>();
    /** Keep track of enumerator parameters so we can tell them about new events. */
    private final List<AEnumeratorParameter> collectionParameters = new ArrayList<AEnumeratorParameter>();

    private List<AParameterChangeListener> parameterChangeListeners = new ArrayList<AParameterChangeListener>();

	private AParameterSuperGroup() {}
	
	static AParameterSuperGroup readSuperGroup(Node node, int applicationUserLevel) {
		AParameterSuperGroup superGroup = new AParameterSuperGroup();
		superGroup.name = node.getAttributes().getNamedItem("name").getNodeValue();
		superGroup.toolTip = node.getAttributes().getNamedItem("toolTip").getNodeValue();
		String sUserLevel = node.getAttributes().getNamedItem("userLevel").getNodeValue();
		superGroup.userLevel = Integer.parseInt(sUserLevel);
		superGroup.readGroups(node,applicationUserLevel);
		return superGroup;
	}
	
	/**
	 * Populate the lists of parameter groups and UI groups.
	 * @param superGroupNode
	 */
	private void readGroups(Node superGroupNode, int applicationUserLevel) {
		NodeList groups = superGroupNode.getChildNodes();
		for (int i = 0; i < groups.getLength(); i++) 
		{
			Node group = groups.item(i);
			if (group.getNodeType() == Node.ELEMENT_NODE) 
			{
				// here we have a <Group> element and create an AParametersGroup object from it
				AParametersGroup g = readGroup(group);

				String gName = g.getGroupName();
				if (parameterGroups.containsKey(gName))
					throw new Error("Duplicate definition of group " + gName);
				parameterGroups.put(gName, g);
				int guiUsage = g.getUIUsage();

				if ( ( (guiUsage == AParametersGroup.TABLE) ||
						(guiUsage == AParametersGroup.PROJECTION)
						|| (guiUsage == AParametersGroup.BRAIN_TEST)
						) && (g.getUserLevel() <= userLevel))
					uiGroups.add(g);
			}
		}

	}
	
    private AParametersGroup readGroup(Node group) 
    {
        // first we read all the attributes
        NamedNodeMap attributes=group.getAttributes();
        String groupName=attributes.getNamedItem("name").getNodeValue();
        String screenName=attributes.getNamedItem("sname").getNodeValue();
        String guiUsageAttribut=attributes.getNamedItem("guiUsage").getNodeValue();
        String scopeAttribut=attributes.getNamedItem("scope").getNodeValue();
        int ul=Integer.parseInt(attributes.getNamedItem("userLevel").getNodeValue());
        String groupToolTip=attributes.getNamedItem("toolTip").getNodeValue();
        
        // set guiUsage value for this group
        int guiUsage;
        if(guiUsageAttribut.equals("TABLE"))
            guiUsage=AParametersGroup.TABLE;
        else if(guiUsageAttribut.equals("PROJECTION"))
            guiUsage=AParametersGroup.PROJECTION;
        else if(guiUsageAttribut.equals("BRAIN_TEST"))
            guiUsage=AParametersGroup.BRAIN_TEST;
        else
            guiUsage=AParametersGroup.NO_USE;
        
        // set scope value for this group
        int scope;
        if(scopeAttribut.equals("LOCAL"))
            scope=AParameter.LOCAL;
        else if(scopeAttribut.equals("GLOBAL"))
            scope=AParameter.GLOBAL;
        else
            scope=AParameter.GLOBAL;
        
        // create an AParametersGroup object based on the attributes
        AParametersGroup thisGroup=new AParametersGroup(groupName, screenName, guiUsage, scope, ul, groupToolTip);
        
        // report error if no parameters in this group
        NodeList childrens=group.getChildNodes();
        if(childrens==null) throw new Error("this group contains no parameters");
        int childrensCount=childrens.getLength();
        if(childrensCount==0) throw new Error("this group contains no parameters");
        
        // read all the parameters of this group recursively
        for(int i=0; i<childrensCount; i++) 
        {
            Node child=childrens.item(i);
            if(child.getNodeType()==Node.ELEMENT_NODE) 
            {
                // for each parameter, create an object (either ACommnad or AParameter)
                // and add it into the to be returned AParametersGroup object
                String childName=child.getNodeName();
                if(childName.equals("Command")) {
                    ACommand c=readCommand(child);
                    if(c!=null) thisGroup.add(c);
                } 
                else {
                	AParameter parameter = AParameterConfigReader.readParameter(child, scope);
                	parameter.addParameterChangeListener(this);
                    thisGroup.add(parameter);
                    if (parameter instanceof AEnumeratorParameter &&
                    	parameter.getScreenName().endsWith("Collections")) {
                    		collectionParameters.add((AEnumeratorParameter)parameter);
                    }
                }
            }
        }
        
        return thisGroup;
    }
    

    
    private ACommand readCommand(Node commandNode) 
    {
        NamedNodeMap attributes=commandNode.getAttributes();
        
        String name=attributes.getNamedItem("name").getNodeValue();
        String command=attributes.getNamedItem("command").getNodeValue();
        String toolTip=attributes.getNamedItem("toolTip").getNodeValue();
        String userLevel=attributes.getNamedItem("ul").getNodeValue();
        if(Integer.parseInt(userLevel)<=this.userLevel)
            return new ACommand(name, command, toolTip);
        else
            return null;
    }
    
	/**
	 * Get the name of the supergroup.
	 * @return the name
	 */
	public String getName() {return name;}
	
	/**
	 * Get the tool tip for this supergroup.
	 * @return the tool tip
	 */
	public String getToolTip() {return toolTip;}
	
	/**
	 * Get the user level of this supergroup.
	 * @return the user level
	 */
	public int getUserLevel() {return userLevel;}

	/**
	 * Get parameter group with given name.
	 * @param name name of group
	 * @return parameter group
	 */
	public AParametersGroup getGroup(String name) {return parameterGroups.get(name);}
	
	/**
	 * Get the parameter groups that are displayed in the UI in the current user level.
	 * @return list of parameter groups
	 */
	public List<AParametersGroup> getUIGroups() {return uiGroups;}

	/**
	 * Tell the listeners that a parameter has changed.
	 */
	@Override
	public void parameterChanged(AParameter parameter) {
		for (AParameterChangeListener listener : parameterChangeListeners) {
			listener.parameterChanged(parameter);
		}
	}

	public Map<? extends String, ? extends AParametersGroup> getParameterGroups() {
		return parameterGroups;
	}

	public Collection<? extends AEnumeratorParameter> getCollectionParameters() {
		return collectionParameters;
	}

	/**
	 * Add a listener to be informed of parameter changes.
	 * 
	 * @param listener
	 */
	public void addParameterChangeListener(AParameterChangeListener listener) {
		this.parameterChangeListeners.add(listener);
	}

	/**
	 * Restore all groups in supergroup to defaults.
	 */
	public void restoreDefaults() {
		for (AParametersGroup group : parameterGroups.values()) {
			group.restoreDefaults();
		}
	}

    /**
     * Copy parameters from one window to another.
     * @param sourceName name of source window
     * @param destName name of destination window
     */
	public void copyParameters(String sourceName, String destName) {
		for (AParametersGroup group : parameterGroups.values()) {
			group.copyParameters(sourceName, destName);
		}
	}

	public void resolveLinkParameters() {
		for (AParametersGroup group : parameterGroups.values()) {
			group.resolveLinkParameters();
		}
	}
}
