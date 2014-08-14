package atlantis.event;

import atlantis.utils.ALogger;

/**
 * Representation of the association between two data types. The association is 
 * stored using the string name+storeGateKey for both data types.
 * 
 * The actual association is a an array of arrays of integers. The first index 
 * of this array is the index of an element in the first datatype. The array 
 * then contains the Athena IDs of the element in the second datatype that are 
 * associated to it. This is necessary because the second datatype might not 
 * have been read from the XML file yet, so the indices are unknown.
 * 
 * @todo Modify this class to perform a translation of Athena ID to Atlantis ID 
 * for the identifiers of "b". This can be done together with purgeIDNotInData()
 * and avoids the need for continuously looking up Athena IDs when drawing.
 */
public class AAssociation {

    /** Logger */
    private static final ALogger logger = ALogger.getLogger(AAssociation.class);
    
    /** Event this association belongs to */
    private final AEvent event;
    
    /** Identifying the datatype this association if from */
    private final String a;
    
    /** Identifying the datatype this association is to */
    private final String b;
    
    /** 
     * Association, the first index is the index of an element of "a", the 
     * second index then loops over the Athena IDs in "b" to which the element 
     * of "a" is associated.
     */   
    private final int[][] assoc;

     /**
     * Constructor, taking as input a flat array (btoa) and an array that 
     * specifies how many entries correspond to each object in "a".
     * 
     * For example:
     * 
     *   from = "Jet";
     *   to = "Track";
     *   numbtoa = {2, 0, 3};
     *   btoa = {1, 5, 3, 4, 6};
     * 
     * This means that the first jet in the event (index=0) is associated
     * to 2 tracks, namely index 1 and 5. The second jet has no associated 
     * tracks and the third jet (index=2) is associated to 3 tracks, indices 
     * 3, 4 and 6.
     * 
     * Alternatively, numbtoa can be null. In that case the length of btoa has 
     * to be equal to the number of entries in "a". Every object in "a" will be 
     * associated to exactly one object in "b".
     * 
     * @param from name+key this association is from, "a"
     * @param to name+key this association is to, "b"
     * @param numbtoa number of elements in "b" that each element in "a" is associated to 
     * @param btoa contents of the association in flat array
     * @param e event object this association belongs to
     */
    public AAssociation(String from, String to, int[] numbtoa, int[] btoa, AEvent e) {
        
        // Association to STr is done using barcodes, this is the only exception 
        // here: for barcodes ID=0 means no association. So we skip those entries.
        boolean skipZero = to.startsWith("STr") && numbtoa == null;
        
        event = e;
        a = from;
        b = to;
        
        if (btoa == null) {
            
            assoc = null;
            
        } else {
            
            if (numbtoa != null) {
                
                // Multiple "b" entries associated to an "a" entry
                assoc = new int[numbtoa.length][];

                int num = 0;
                for (int i = 0; i < numbtoa.length; ++i) {
                    if (numbtoa[i] <= 0) {
                        assoc[i] = new int[0];
                    } else {
                        assoc[i] = new int[numbtoa[i]];
                    }

                    for (int j=0; j<numbtoa[i]; ++j) {
                        assoc[i][j] = btoa[num++];
                    }
                }

            } else {

                // Simple 1-on-1 association
                assoc = new int[btoa.length][];
                
                for (int i=0; i<btoa.length; i++) {
                    if (skipZero && btoa[i] <= 0) {
                        assoc[i] = new int[0];
                    } else {
                        assoc[i] = new int[] { btoa[i] };
                    }
                }
            }

        }
    }

    /**
     * Copy constructor
     * 
     * @param from source datatype
     * @param to target datatype
     * @param associations association index arrays
     * @param e event the association belongs to
     */
    public AAssociation(String from, String to, int[][] associations, AEvent e) {
        event = e;
        a = from;
        b = to;
        assoc = associations;
    }
    
    /**
     * Get the name of association
     * @return association name, concatenation of the two data types it associates
     */
    public String getName() {
        return a + b;
    }

    /**
     * Get source data type of association
     * @return name of source data type
     */
    public String getFrom() {
        return a;
    }

    /**
     * Get target data type of association
     * @return name of target data type
     */
    public String getTo() {
        return b;
    }

    /**
     * Get association array
     * @return association array
     */
    public int[][] getData() {
        return assoc;
    }

    /**
     * Method that constructs the association b->a when only an association a->b 
     * currently exists. The complication here is due to the fact that "a" uses 
     * Atlantis indices (0, 1, ...), while "b" uses Athena IDs (1611690048,
     * 1635282976, etc.).
     * 
     * @return inverted association
     */
    public AAssociation invert() {
        
        AData aData = event.get(a);
        AData bData = event.get(b);
        
        if (aData == null || bData == null) {
            logger.debug(getClass().getName()
                    + ":  No association found between " + a + " and " + b);
            return null;
 
        } else {

            try {
                // New assoc will have the same number of entries as "b"
                int[][] inv = new int[bData.getNumData()][];
                
                // Athena IDs of object "a"
                int[] ida = aData.getID();
                
                // Loop over all entries in "a", the current association array
                for (int i=0; i<assoc.length; ++i) {
                    if (assoc[i] != null) {
                        for (int j=0; j<assoc[i].length; ++j) {
                            
                            // For each associated entry in "b", we need to find 
                            // the index from the Athena ID
                            int index = bData.getIndexFromId(assoc[i][j]);
                                                        
                            if (index != AData.NO_INVERSE) {

                                if (inv[index] == null) {

                                    // The object in "a" is the first object that
                                    // this object in "b" is associated to. Create
                                    // a new array and add this entry.
                                    inv[index] = new int[] { ida[i] };

                                } else {

                                    // The object in "b" is already associated to 
                                    // other objects in "a". Retrieve the other 
                                    // objects, increment the size and add the
                                    // current object in "a".
                                    int[] temp = new int[inv[index].length + 1];
                                    for (int k=0; k<inv[index].length; ++k) {
                                        temp[k] = inv[index][k];
                                    }
                                    temp[inv[index].length] = ida[i];
                                    inv[index] = temp;
                                }
                            }
                        }
                    }
                }
                return new AAssociation(b, a, inv, event);

            } catch (Exception e) {
                logger.error("Unable to invert association.", e);
                return null;
            }
        }
    } // invert() ---------------------------------------------------------------

    /**
     * Remove associations between items that are not present in the event data
     */
    void purgeIDNotInData() {
        // remove inconsistencies
        // TRT hits from RTr<->SiCluster
        // Association to an STr which does not exist in STr

        AData source = event.get(b);
        if (assoc == null || source == null) {
            return;
        }
        int INVALID_ID = -1; // (i hope!)
        for (int i = 0; i < assoc.length; i++) {
            if (assoc[i] != null) {
                int num = 0;
                for (int j = 0; j < assoc[i].length; j++) {
                    if (source.getIndexFromId(assoc[i][j]) == AData.NO_INVERSE) {
                        assoc[i][j] = INVALID_ID;
                        num++;
                    }
                }
                if (num > 0) {
                    if (num == assoc[i].length) {
                        assoc[i] = null;
                    } else {
                        int[] temp = new int[assoc[i].length - num];
                        num = 0;
                        for (int j = 0; j < assoc[i].length; j++) {
                            if (assoc[i][j] != INVALID_ID) {
                                temp[num++] = assoc[i][j];
                            }
                        }
                        assoc[i] = temp;
                    }
                }
            }
        }
    }

    /**
     * Convert association into an informational message for the user
     */
    @Override
    public String toString() {
        StringBuilder s = new StringBuilder();
        s.append(a).append(" to ").append(this.b);
        if (assoc == null) {
            s.append(" is empty\n");
            return s.toString();
        } else {
            s.append("\n");
            for (int i = 0; i < assoc.length; i++) {
                if (assoc[i] != null) {
                    for (int j = 0; j < assoc[i].length; j++) {
                        s.append(assoc[i][j]).append(" ");
                    }
                    s.append("\n");
                }
            }
        }
        return s.toString();
    }
}
