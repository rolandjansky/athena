package atlantis.interactions;

import java.util.Hashtable;
import java.util.Map.Entry;
import java.util.Set;

import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import atlantis.projection.AProjectionsManager;

/**
 * Handles the "Interactions" node in the XML configuration file:
 * <ul>
 * <li>store the InteractionControl node for each projection</li>
 * <li>process the lists of available/default projections in demo mode</li>
 * </ul>
 * 
 * @author waugh
 */
public class AInteractionsConfigReader {

	/** The set of XML nodes with projection information (name : node). */
	private final static Hashtable<String,Node> projectionNodes = new Hashtable<String, Node>();

	/**
	 * Read and process the Interactions information from the XML configuration file.
	 * 
	 * @param node The XML Node which contains the <bold>Interactions</bold> node.
	 */
	public static void readInteractions(Node node) {
		initProjectionNodes(node);
		setAvailableProjectionsInDemoMode();
	}
	
	/**
	 * Get the configuration XML node corresponding to a given projection.
	 * 
	 * @param projectionName the name of the projection
	 * @return the corresponding InteractionControl node
	 */
	public static Node getNode(String projectionName) {
		return projectionNodes.get(projectionName);
	}

	private static void initProjectionNodes(Node node) {
		NodeList childNodes=node.getChildNodes();
		for(int i=0; i<childNodes.getLength(); i++) {
			Node child=childNodes.item(i);

			if(child.getNodeType()==Node.ELEMENT_NODE) {
				String name=child.getAttributes().getNamedItem("projectionName").getNodeValue();
				projectionNodes.put(name, child);
			}
		}
	}

	private static void setAvailableProjectionsInDemoMode() {
		Set<Entry<String, Node>> entries = projectionNodes.entrySet();
		for (Entry<String,Node> entry : entries) {
			String name = entry.getKey();
			Node node = entry.getValue();
			boolean availableInDemoMode = node.getAttributes().getNamedItem("availableInDemoMode").getNodeValue().equals("YES");
			boolean defaultInDemoMode = node.getAttributes().getNamedItem("defaultInDemoMode").getNodeValue().equals("YES");
			if (availableInDemoMode) AProjectionsManager.makeProjectionAvailableInDemoMode(name);
			if (defaultInDemoMode) AProjectionsManager.makeProjectionDefaultInDemoMode(name);
		}
	}
}
