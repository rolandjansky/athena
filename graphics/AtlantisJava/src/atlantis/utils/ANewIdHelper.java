package atlantis.utils;

import java.util.HashMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * Helper tool for decoding compact readout channel identifiers.
 *
 * @author Eric Jansen
 */
public final class ANewIdHelper {
    
    /** Singleton instance. */
    private static ANewIdHelper instance = null;

    /** Link to ATLAS dictionary. */
    private AIdDictionary atlas = null;
    
    /** Collection of named (sub-)dictionaries. */
    private HashMap<String, AIdDictionary> dictionaries = null;

    /** Logging facility. */
    public static ALogger logger = ALogger.getLogger(ANewIdHelper.class);
    
    /**
     * Static construct method, initializes ANewIdHelper from XML dictionaries.
     * @param rootNode XML root node of type IdDict.
     */
    public synchronized static void construct(Node rootNode) throws AAtlantisException {
        if (instance == null) instance = new ANewIdHelper();
        try {
            instance.initialize(rootNode);
        } catch (AAtlantisException e) {
            throw new AAtlantisException("Error initializing ANewIdHelper: " + e.getMessage());
            
        }
    }

    /**
     * Obtain instance of ANewIdHelper.
     * @return ANewIdHelper instance.
     */
    public synchronized static ANewIdHelper instance() {
        if (instance == null) {
            throw new Error("Attempted to access ANewIdHelper instance before initialization.");
        }
        return instance;
    }

    /**
     * Private constructor.
     */
    private ANewIdHelper() {};

    /**
     * Non-static, private initialization method.
     * @param rootNode XML root node of type IdDict.
     */
    private void initialize(Node rootNode) throws AAtlantisException {
        NodeList nodes = rootNode.getChildNodes();
        for (int i=0; i<nodes.getLength(); i++) {
            Node node = nodes.item(i);
            if (node.getNodeType() == Node.ELEMENT_NODE) {
                if ("IdDictionary".equals(node.getNodeName())) {
                    String name = node.getAttributes().getNamedItem("name").getNodeValue();
                    try {
                        if (dictionaries != null && dictionaries.containsKey(name)) {
                            AIdDictionary dictionary = dictionaries.get(name);
                            dictionary.initialize(node);
                        } else {
                            if (dictionaries == null) {
                                dictionaries = new HashMap<String,AIdDictionary>();
                            }
                            dictionaries.put(name, new AIdDictionary(node));
                        }
                    } catch (AAtlantisException e) {
                        logger.error("Exception occurred while initializing IdHelper:" + e.getMessage());
                    }
                }
            }
        }
        
        if (dictionaries.containsKey("ATLAS")) {
            atlas = dictionaries.get("ATLAS");
        } else {
            throw new AAtlantisException("No 'ATLAS' dictionary found");
        }
    }

    /**
     * Check for presence of a named dictionary.
     * @param dictName dictionary name.
     * @return true if dictionary is present, false is not.
     */
    boolean hasDictionary(String dictName) {
        return dictionaries.containsKey(dictName);
    }
    
    /**
     * Return pointer to an existing named dictionary.
     * @param dictName dictionary name
     * @return dictionary object
     */
    AIdDictionary getDictionary(String dictName) throws AAtlantisException {
        if (dictionaries != null && dictionaries.containsKey(dictName)) {
            return dictionaries.get(dictName);
        } else {
            throw new AAtlantisException("Dictionary " + dictName + " could not be found");
        }
    }
    
    /**
     * Add a named dictionary to the idHelper.
     * @param dictName dictionary name
     * @param dict dictionary
     */
    void addDictionary(String dictName, AIdDictionary dict) {
        if (dictionaries == null){
            dictionaries = new HashMap<String,AIdDictionary>();
        }
        dictionaries.put(dictName, dict);
    }
    
    /**
     * Convert a compact identifier into a "/" delimited expanded identifier.
     * @param id compact identifier
     * @return expanded identifier, fields delimited by "/"
     */
    public String getFullIdentifier(long id) throws AAtlantisException {
        return atlas.getFullIdentifier(id);
    }
    
    /**
     * Extract named field from a compact identifier.
     * @param field identifier field name
     * @param id compact identifier
     * @return value of named field
     */
    public int get(String field, long id) throws AAtlantisException {
        return atlas.get(field, id);
    }
    
    /**
     * Extract string value of a named field from a compact identifier. For 
     * example:
     *   get("stationName", id) = 49
     *   getString("stationName", id) = "EIS"
     * 
     * @param field identifier field name
     * @param id compact identifier
     * @return string value of named field
     */
    public String getString(String field, long id) throws AAtlantisException {
        return atlas.getString(field, id);
    }
}
