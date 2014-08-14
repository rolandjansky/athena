package atlantis.utils;

import java.util.HashMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * Class representing a single field in an identifier. Each field has a parent, 
 * a range of allowed values and child fields that may depend on the value of the 
 * current field. The possible values of a field will determine the number of bits
 * needed to store it in the compact identifier. The AIdField tree is capable of 
 * encoding and decoding these compact identifiers.
 *
 * @author Eric Jsnsen
 */
class AIdField {
    
    /** 
     * Dictionary that this field belongs to. The dictionary is used to keep track 
     * of the possible values of a field, 
     */
    protected AIdDictionary dictionary = null;
    
    /** Parent field in the identifier tree. */
    private AIdField parent = null;
    
    /** Level of nesting this field is at. */
    private int level = 0;
    
    /** Field name. */
    private String name = null;
    
    /** All known values found for this field in the dictionary. */
    private AIdRange fieldRange = null;

    /** Map of value ranges to child AIdField objects, inverse of parent. */
    private HashMap<AIdRange, AIdField> children = new HashMap<AIdRange, AIdField>();
    
    /** Map of string labels in case of an enumerated field. */
    private HashMap<Integer, String> labels = null;
    
    /** Reference to idHelper for retrieving dictionaries. */
    protected static ANewIdHelper idHelper = ANewIdHelper.instance();
    
    /**
     * Empty constructor.
     */
    protected AIdField() {}
    
    /**
     * Constructor, setting dictionary and parent field.
     * @param dict dictionary the field belongs to
     * @param parentField parent field
     */
    protected AIdField(AIdDictionary dict, AIdField parentField) {
        
        dictionary = dict;
        if (parentField != null) {
            parent = parentField;
            level = parent.getLevel()+1;
        }
    }
    
    /**
     * Get the name of the field.
     * @return field name
     */
    String getName() {
        return name;
    }
    
    /**
     * Get the nesting level of the field.
     * @return nesting level
     */
    int getLevel() {
        return level;
    }
    
    /**
     * Obtain the value of the named field from a compact identifier.
     * @param fieldName field name
     * @param id compact identifier
     * @return value of the given field in the identifier
     * @throws AAtlantisException 
     */
    public int get(String fieldName, long id) throws AAtlantisException {
        if (id > Integer.MAX_VALUE) {
            return get(fieldName, id, 64);
        } else {
            return get(fieldName, id, 32);
        }
    }
    
    /**
     * Obtain the string value of the named field from a compact identifier.
     * @param fieldName field name
     * @param id compact identifier
     * @return value of the given field in the identifier
     * @throws AAtlantisException 
     */
    public String getString(String fieldName, long id) throws AAtlantisException {
        if (id > Integer.MAX_VALUE) {
            return getString(fieldName, id, 64);
        } else {
            return getString(fieldName, id, 32);
        }
    }
    
    /**
     * Internal version of get(String, long), propagating the bit offset in 
     * the compact identifier.
     * @param fieldName field name
     * @param id compact identifier
     * @param offset bit offset accumulated from parent fields
     * @return value of the given field in the identifier
     * @throws AAtlantisException 
     */
    private int get(String fieldName, long id, int offset) throws AAtlantisException {
        
        if (fieldRange == null) {
            throw new AAtlantisException("No range known for identifier field"
                    + " '" + name + "' while decoding '" + fieldName + "'");
        }
        
        // The following code is duplicated a few times in this class. This is 
        // necessary to extract the value but also update offset at the same time.
        int bits = fieldRange.bits();
        offset -= bits;
        int mask = (int)(Math.pow(2, bits)) - 1;

        int value;
        try {
            value = fieldRange.get((int)(id >> offset & mask));
        } catch (AAtlantisException e) {
            throw new AAtlantisException("Unable to decode identifier field"
                    + " '" + name + "': " + e);
        }
        
        if (fieldName.equals(name)) {
            // This is me, return value
            return value;
        } else {
            // Find the correct child to delegate this ID to
            for(AIdRange range : children.keySet()) {
                if (range.contains(value)) {
                    return children.get(range).get(fieldName, id, offset);
                }
            }
        }
        
        throw new AAtlantisException("Invalid identifier '" + id + "'");
    }

    /**
     * Internal version of get(String, long), propagating the bit offset in 
     * the compact identifier.
     * @param fieldName field name
     * @param id compact identifier
     * @param offset bit offset accumulated from parent fields
     * @return value of the given field in the identifier
     * @throws AAtlantisException 
     */
    private String getString(String fieldName, long id, int offset) throws AAtlantisException {
        
        if (fieldRange == null) {
            throw new AAtlantisException("No range known for identifier field"
                    + " '" + name + "' while decoding '" + fieldName + "'");
        }
        
        // The following code is duplicated a few times in this class. This is 
        // necessary to extract the value but also update offset at the same time.
        int bits = fieldRange.bits();
        offset -= bits;
        int mask = (int)(Math.pow(2, bits)) - 1;

        int value;
        try {
            value = fieldRange.get((int)(id >> offset & mask));
        } catch (AAtlantisException e) {
            throw new AAtlantisException("Unable to decode identifier field"
                    + " '" + name + "': " + e);
        }
        
        if (fieldName.equals(name)) {
            // This is me, return value
            return getLabel(value);
        } else {
            // Find the correct child to delegate this ID to
            for(AIdRange range : children.keySet()) {
                if (range.contains(value)) {
                    return children.get(range).getString(fieldName, id, offset);
                }
            }
        }
        
        throw new AAtlantisException("Invalid identifier '" + id + "'");
    }
    
    /**
     * Convert a compact identifier into a "/" delimited expanded identifier.
     * @param id compact identifier
     * @return expanded identifier, fields delimited by "/"
     */
    public String getFullIdentifier(long id) throws AAtlantisException {
        
        StringBuilder s = new StringBuilder();
        if (id > Integer.MAX_VALUE) {
            getFullIdentifier(id, 64, s);
        } else {
            getFullIdentifier(id, 32, s);
        }
        return s.toString();
    }
    
    /**
     * Internal version of getFullIdentifier(long), passing the bit offset a 
     * StringBuilder object. The output is appended to the StringBuilder object.
     * @param id compact identifier
     */
    private void getFullIdentifier(long id, int offset, StringBuilder s) throws AAtlantisException {
        
        // The following code is duplicated a few times in this class. This is 
        // necessary to extract the value but also update offset at the same time.
        int bits = fieldRange.bits();
        offset -= bits;
        int mask = (int)(Math.pow(2, bits)) - 1;

        int value;
        try {
            value = fieldRange.get((int)(id >> offset & mask));
        } catch (AAtlantisException e) {
            throw new AAtlantisException("Unable to decode identifier field"
                    + " '" + name + "': " + e.getMessage());
        }
        
        if (hasLabels()) {
            s.append(getLabel(value));//.append("=").append(value);
        } else {
            s.append(value);
        }

        if (children != null) {
            for (AIdRange range : children.keySet()) {
                if (range.contains(value)) {
                    AIdField child = children.get(range);
                    if (child != null && !child.isEmpty()) {
                        s.append("/");
                        child.getFullIdentifier(id, offset, s);
                    }
                    return;
                }
            }

            throw new AAtlantisException("Invalid identifier '" + id + "', "
                    + "field '" + name +"' does not allow value " + value + " "
                    + "(partially decoded: '" + s.toString() + "')");
        }
    }
    
    /**
     * Parses a &lt;region&gt; element in the XML file. This method should be 
     * called on an AIdField instance that represents the first field in the 
     * region. Children will be added by recursively calling this method.
     * @param region XML element to parse
     * @param start Starting child element
     * @return Last child parsed
     * @throws AAtlantisException 
     */
    AIdField parseRegion(Node region, String group, int start) throws AAtlantisException {
        NodeList nodes = region.getChildNodes();
        
        if (start == 0) {
            // In a new region, first set the group all parsed elements belong to (if any).
            Node groupNode = region.getAttributes().getNamedItem("group");
            if (groupNode != null) {
                group = groupNode.getNodeValue();
            }
        }
        
        for (int i=start; i<nodes.getLength(); i++) {
            Node node = nodes.item(i);
            
            /* Select only elements, not the text in between. */
            if (node.getNodeType() == Node.ELEMENT_NODE) {
                
                if ("range".equals(node.getNodeName())) {
                    
                    // The first <range> encountered is for the current AIdFied. For the 
                    // given values of this field, the next will be a child. For example, 
                    // when called with:
                    // 
                    //   <range field="subdet" value="2" />
                    //   <range field="part" value=1" />
                    //   ...
                    // 
                    // This field will become "subdet". In case the value of this field 
                    // in the identifier is "2", the next field in the chain is "part".
                    // More values of "part" will probably be added later, plus other 
                    // values of "subdet" for which the next field is not "part" but 
                    // something different.
                    if (name == null) {
                        name = node.getAttributes().getNamedItem("field").getNodeValue();
                        if (dictionary.hasLabels(name)) {
                            parseLabels(dictionary.getLabels(name));
                        }
                        
                        fieldRange = dictionary.getRange(group, level);
                    }

                    AIdRange range = new AIdRange(this, group, node);
                    
                    // Fields are sometimes repeated without specifying any allowed values,
                    // in that case we allow all existing values (so we copy fieldRange).
                    if (range.isEmpty() && !fieldRange.isEmpty()) {
                        range.add(fieldRange);
                    }
                    
                    // If this particular range is already present, give the next 
                    // node (i+1) to the correct child and tell it to parse it.
                    if (children.containsKey(range) && children.get(range) != null) {
                        
                        return children.get(range).parseRegion(region, group, i+1);
                    }
                    
                    // If there is another range that fully contains this range, we add the 
                    // current node there. This is a reasonable shortcut, since we are not 
                    // interested in fully validating IDs (i.e. raise an exception when an 
                    // RPC identifier is presented for a muon station that should not contain 
                    // RPCs), this is a reasonable shortcut. 
                    for (AIdRange childRange : children.keySet()) {
                        if (childRange.contains(range)) {
                            return children.get(childRange).parseRegion(region, group, i+1);
                        }
                    }

                    if (group != null && !group.equals("")) {
                        
                        // Two ranges within the same group means that they should be 
                        // considered to lead to the same child. In that case expand 
                        // the range using AIdRange.add().
                        for (AIdRange childRange : children.keySet()) {
                            if (group.equals(childRange.group())) {
                                AIdField child = children.get(childRange);
                                
                                if (child != null) {
                                    fieldRange.add(range);
                                    children.remove(childRange);
                                    childRange.add(range);
                                    children.put(childRange, child);
                                    return child.parseRegion(region, group, i+1);
                                }
                            }
                        }
                    }

                    fieldRange.add(range);

                    AIdField child = new AIdField(dictionary, this);
                    AIdField retfield = child.parseRegion(region, group, i+1);
                    if (retfield instanceof AIdDictionary) {
                        children.put(range, retfield);
                    } else {
                        children.put(range, child);
                    }

                    return retfield;

                } else if ("dictionary".equals(node.getNodeName())) {
                    
                    // A dictionary is a reference to another file. The AIdDictionary class
                    // inherits from AIdField and can will act as the first field in the 
                    // dictionary. Dictionaries are initialized one by one, so when a dictionary
                    // does not exist yet, it is created empty so that it can be initialized later.
                    String dictName = node.getAttributes().getNamedItem("name").getNodeValue();
                    if (idHelper.hasDictionary(dictName)) {
                        return idHelper.getDictionary(dictName);
                    } else {
                        AIdDictionary dict = new AIdDictionary();
                        idHelper.addDictionary(dictName, dict);
                        return dict;
                    }
                    
                } else if ("reference".equals(node.getNodeName())) {

                    // A reference is a link to a <subregion> element. The elements within the 
                    // subregion are parsed as if they were present instead of the reference.
                    String reference = node.getAttributes().getNamedItem("subregion").getNodeValue();                    
                    AIdField child = parseRegion(dictionary.getSubRegion(reference), group, 0);

                    // After parsing the subregion, we need to continue with the current region.
                    AIdField retfield = child.parseRegion(region, group, i+1);
                    return retfield;
                }
            }
        }
        
        // Reached the end of a region or subregion, 
        // return the last object parsed (which is us).
        return this;
    }

    /**
     * Parse a &lt;field&gt; object in the XML file.
     * @param field XML element to parse
     * @throws AAtlantisException 
     */
    private void parseLabels(Node field) throws AAtlantisException {

        labels = new HashMap<Integer, String>();
        int lastValue = 0;
        
        NodeList nodes = field.getChildNodes();
        for (int i=0; i<nodes.getLength(); i++) {
            Node node = nodes.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE) {
                
                if (node.getNodeName().equals("label")) {
                    String label = node.getAttributes().getNamedItem("name").getNodeValue();
                    Integer value;
                    
                    Node valueNode = node.getAttributes().getNamedItem("value");
                    if (valueNode == null) {
                        value = lastValue+1;
                    } else {
                        value = Integer.parseInt(valueNode.getNodeValue().replaceAll("\\+", ""));
                    }

                    labels.put(value, label);
                    lastValue = value.intValue();
                }
            }
        }
    }
    
    /**
     * Check if this field has string labels.
     * @return true if string labels are present
     */
    boolean hasLabels() {
        return labels != null && labels.size() > 0;
    }

    /**
     * Check if this field is empty/uninitialized. Fields at the end of the tree 
     * will remain in this state, during construction all newly initialized fields 
     * will be in this state.
     * @return 
     */
    boolean isEmpty() {
        return name == null;
    }
    
    /**
     * Return label corresponding to an integer value.
     * @param value field value
     * @return label
     */
    String getLabel(Integer value) {
        if (labels.containsKey(value)) {
            return labels.get(value);
        } else {
            return "?";
        }
    }

    /**
     * Convert a field value as it appears in the XML file into a true integer.
     * For example "+2" is converted into 2 and labelled fields are converted 
     * into their integer equivalents.
     * @param fieldName field name (for label lookup)
     * @param value "raw" field value in the XML
     * @return integer value
     * @throws AAtlantisException 
     */
    protected int valueToInt(String fieldName, String value) throws AAtlantisException {

        try {
            return Integer.parseInt(value.replaceAll("\\+", ""));
        } catch (NumberFormatException e) {
            return labelToInt(fieldName, value);
        }
    }
    
    /**
     * Convert a field label into an integer value.
     * @param fieldName field name
     * @param label value of the labelled field
     * @return integer label
     * @throws AAtlantisException 
     */
    protected int labelToInt(String fieldName, String label) throws AAtlantisException {
        if (labels != null) {
            if (labels.containsValue(label)) {
                for (Integer value : labels.keySet()) {
                    if (label.equals(labels.get(value))) {
                        return value;
                    }
                }
            }
        }
        
        throw new AAtlantisException("Field/label not found in"
                +" dictionary: " + fieldName + "/" + label);
    }
    
    /**
     * Convert the field structure into a printable form, starting from this node.
     * @return string representation of the identifier tree
     */
    @Override
    public String toString() {
        return toString("");
    }

    /**
     * Private version of toString(), used recursively and passing an indentation
     * string to print the entire tree structure.
     * @param indent identation string, appended to all lines
     * @return string representation of the identifier tree
     */
    private String toString(String indent) {
        StringBuilder s = new StringBuilder();

        s.append(indent).append(name).append(" ").append(fieldRange).append("\n");

        for (AIdRange range : children.keySet()) {
            s.append(indent).append(range).append("\n");
            AIdField child = children.get(range);
            if (child != null) {
                s.append(child.toString(indent + "  "));
            }
        }

        return s.toString();
    }
}
