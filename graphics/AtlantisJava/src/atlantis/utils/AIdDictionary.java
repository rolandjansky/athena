package atlantis.utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * Class representing an identifier dictionary, used for decoding of compact IDs.
 *
 * @author Eric Jansen
 */
class AIdDictionary extends AIdField {

    private HashMap<String, Node> subregions = null;
    private HashMap<String, Node> labels = null;
    private HashMap<String, List<AIdRange>> ranges = new HashMap<String, List<AIdRange>>();
    
    /**
     * Empty constructor, used to create a placeholder dictionary that 
     * can be initialized later.
     */
    AIdDictionary() {}
    
    /**
     * Construct and initialize dictionary from an &lt;IdDictionary&gt; XML node.
     * @param dict dictionary XML node
     * @throws AAtlantisException 
     */
    AIdDictionary(Node dict) throws AAtlantisException {
        initialize(dict);
    }
    
    /**
     * Initializes a dictionary from an &lt;IdDictionary&gt; XML node.
     * @param dict dictionary XML node
     * @throws AAtlantisException 
     */
    final void initialize(Node dict) throws AAtlantisException {
        
        this.dictionary = this;
        
        NodeList nodes = dict.getChildNodes();
        
        for (int i=0; i<nodes.getLength(); i++) {
            Node node = nodes.item(i);

            if (node.getNodeType() == Node.ELEMENT_NODE) {
                
                if ("field".equals(node.getNodeName())) {
                    String fieldName = node.getAttributes().getNamedItem("name").getNodeValue();
                    if (labels == null) {
                        labels = new HashMap<String, Node>();
                    }
                    labels.put(fieldName, node);
                
                } else if ("subregion".equals(node.getNodeName())) {
                    String subregionName = node.getAttributes().getNamedItem("name").getNodeValue();
                    if (subregions == null) {
                        subregions = new HashMap<String, Node>();
                    }
                    subregions.put(subregionName, node);
                
                } else if ("region".equals(node.getNodeName())) {
                    parseRegion(node, null, 0);
                
                } else if ("alternate_regions".equals(node.getNodeName())) {
                    NodeList altnodes = node.getChildNodes();
 
                    for (int j=0; j<altnodes.getLength(); j++) {
                        Node altnode = altnodes.item(j);
                        if (altnode.getNodeType() == Node.ELEMENT_NODE
                                && "region".equals(altnode.getNodeName())) {
                            parseRegion(altnode, null, 0);
                        }
                    }
                }
            }
        }
        
        // Clean up the HashMaps, they are only needed during initialization.
        ranges = null;
        labels = null;
        subregions = null;
    }
    
    /**
     * Returns true if the dictionary contains labels for the named field.
     * @param fieldName name of the field
     * @return true if labels are present
     */
    boolean hasLabels(String fieldName) {
        return labels.containsKey(fieldName);
    }
    
    /**
     * Returns the &lt;field&gt; node containing the labels for the named field.
     * @param fieldName name of the field
     * @return &lt;field&gt; XML node
     * @throws AAtlantisException 
     */
    Node getLabels(String fieldName) throws AAtlantisException {
        if (labels.containsKey(fieldName)) {
            return labels.get(fieldName);
        } else {
            throw new AAtlantisException("Labels for field '"+fieldName+"' not found");
        }        
    }
    
    /**
     * Returns the &lt;subregion&gt; node containing the named subregion.
     * @param subRegionName name of the subregion
     * @return &lt;subregion&gt; XML node
     * @throws AAtlantisException 
     */
    Node getSubRegion(String subRegionName) throws AAtlantisException {
        if (subregions.containsKey(subRegionName)) {
            return subregions.get(subRegionName);
        } else {
            throw new AAtlantisException("Subregion '"+subRegionName+"' not found");
        }
    }
    
    /**
     * One AIdRange object for every unique field range is kept inside the dictionary. 
     * It serves to collect all possible values for the named field. Due to the tree 
     * structure of the identifiers multiple AIdFields with the same name might be 
     * created, but they will always have the same range. The first AIdField with a 
     * certain name will be the parent of the range, but it is only used for decoding 
     * of labelled fields. The field name has to be set before creating the range in the 
     * dictionary.
     * @param field named identifier field
     * @return global range corresponding to the named field
     */
    AIdRange getRange(String group, int level) throws AAtlantisException {

        List<AIdRange> range;
        if (ranges.containsKey(group)) {
            range = ranges.get(group);
        } else {
            range = new ArrayList<AIdRange>();
            ranges.put(group, range);
        }
        
        while (range.size() <= level) {
            range.add(new AIdRange());
        }

        return range.get(level);
    }
}
