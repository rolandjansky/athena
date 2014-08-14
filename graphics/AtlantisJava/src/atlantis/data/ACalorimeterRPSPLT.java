package atlantis.data;

import java.util.HashMap;

import atlantis.utils.ALogger;
import atlantis.utils.AAtlantisException;

/**
 *
 *    Calorimeter Real Pulse Shape Plot Lookup
 *    Tables. The class accommodates lookup tables (arrays) for the Tile and LAr,
 *    FCAL and HEC calorimeters. If the real pulse shape plots data are found in
 *    the event files, the corresponding arrays within this class are initialised
 *    from A[TILE|LAr|FCAL|HEC]Data class(es). The arrays are initialised from
 *    CSV (comma separated values files) just once per Atlantis session.
 *   
 * @author Zdenek Maxa
 */
public final class ACalorimeterRPSPLT
{
    private static ALogger logger = ALogger.getLogger(ACalorimeterRPSPLT.class);

    // TILE calorimeter, lookup table time values
    protected static float[] TILE_TIME = null;
    // TILE calorimeter, lookup table amplitude values
    protected static float[] TILE_AMPLITUDE = null;
    

    // FCAL calorimeter, lookup table time values
    protected static float[] FCAL_TIME = null;
    // FCAL calorimeter, lookup table layer 0, amplitude values
    protected static float[] FCAL_LAYER0_AMPLITUDE = null;
    // FCAL calorimeter, lookup table layer 1, amplitude values
    protected static float[] FCAL_LAYER1_AMPLITUDE = null;
    // FCAL calorimeter, lookup table layer 2, amplitude values
    protected static float[] FCAL_LAYER2_AMPLITUDE = null;


    // HEC calorimeter, lookup table time values
    protected static float[] HEC_TIME = null;
    // HEC calorimeter, lookup table amplitude values
    protected static float[] HEC_AMPLITUDE = null;




    // LAr calorimeter, lookup tables 
    // for time (the same array for both
    // barrel/endcap, all layer and regions and eta ranges
    // currently (2009-02-11) there are 58 various arrays for LAr, it
    // is not sustainable to have all these in separate arrays - for LAr
    // there will be defined a container - keys are lookup tables names
    // as defined in the lookup table file and values are float[] arrays
    private static HashMap<String, float[]> larTables = null;
    
    // set the LAr table keys in a static block
    // explanation of the LAr lookup tables names (the names are identical
    // to comma separated values array names as in the look up table file):
    //     LAR_TIME - time constants - used for barrel/endcap, all layers,
    //                all regions, all eta ranges
    //     LAR_BARREL_L1_R0_AMPLITUDE - barrel, layer 1, region 0, etc, etc
    //     LAR_ENDCAP_L0_R0_ETA0_AMPLITUDE - endcap, layer 0, region 0, eta
    //                                       range 0 - very many of these
    // bother to define the array names here to be really safe ... otherwise
    // could just rely on the CSV file ...
    static
    {
        larTables = new HashMap<String, float[]>(70); // not more arrays than 70
        
        larTables.put("LAR_TIME", null);
        larTables.put("LAR_BARREL_LAYER1_REGION0_AMPLITUDE", null);
        larTables.put("LAR_BARREL_LAYER1_REGION1_AMPLITUDE", null);
        larTables.put("LAR_BARREL_LAYER2_REGION0_AMPLITUDE", null);
        larTables.put("LAR_BARREL_LAYER2_REGION1_AMPLITUDE", null);
        larTables.put("LAR_BARREL_LAYER3_REGION0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER0_REGION0_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA3_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA4_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA5_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA6_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION0_ETA7_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION1_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION2_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION2_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION2_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION2_ETA3_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION3_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION3_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION3_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION4_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION4_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION4_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION4_ETA3_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION4_ETA4_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER1_REGION5_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA3_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA4_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA5_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA6_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION0_ETA7_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA3_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA4_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA5_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA6_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA7_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA8_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA9_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER2_REGION1_ETA10_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA0_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA1_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA2_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA3_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA4_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA5_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA6_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA7_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA8_AMPLITUDE", null);
        larTables.put("LAR_ENDCAP_LAYER3_REGION0_ETA9_AMPLITUDE", null); 
    }
    
    


    public static boolean areFCALLookupTablesInitialized()
    {
        if(FCAL_LAYER0_AMPLITUDE != null &&
           FCAL_LAYER1_AMPLITUDE != null &&
           FCAL_LAYER2_AMPLITUDE != null &&
           FCAL_TIME != null)
        {
            return true;
        }
        else
        {
            return false;
        }

    } // areFCALLookupTablesInitialized() -----------------------------------



    public static boolean areHECLookupTablesInitialized()
    {
        if(HEC_TIME != null && HEC_AMPLITUDE != null)
        {
            return true;
        }
        else
        {
            return false;
        }

    } // areHECLookupTablesInitialized() ------------------------------------



    public static boolean areTileLookupTablesInitialized()
    {
        if(TILE_TIME != null && TILE_AMPLITUDE != null)
        {
            return true;
        }
        else
        {
            return false;
        }

    } // areTileLookupTablesInitialized() -----------------------------------



    /**
     * Method is called at LAr constructor, i.e. at the beginning of each
     * event to check if all lookup tables for LAr are properly initialised.
     * The check is performed only when other necessary LAr are is available
     * in a given event file. 
     * @return true if all float arrays were properly initialised
     */
    public static boolean areLarLookupTablesInitialized()
    {
        // iterate over all keys in the container and check if all
        // float arrays were properly initialised
        for(String key : larTables.keySet())
        {
            if(larTables.get(key) == null)
            {
                logger.error("LAr real pulse shapes plots lookup table " +
                             "\"" + key + "\" was not initialised.");
                return false;
            }
        }
        
        return true;
        
    } // areLarLookupTablesInitialized() ------------------------------------
    
    
    
    public static void setLarTable(String key, float[] array) throws AAtlantisException
    {
        logger.debug("Setting LAr pulse shapes lookup table \"" + key + "\"");
        
        if(larTables.containsKey(key))
        {
            // it certainly already should contain the key (defined above)
            if(larTables.get(key) != null)
            {
                throw new AAtlantisException("LAr pulse shapes lookup table \"" +
                          key + "\" has already been defined.");
            }
            else
            {
                larTables.put(key, array);
            }
        }
        else
        {
            throw new AAtlantisException("LAr pulse shapes lookup table - " +
                      "entry \"" + key + "\" has not been created.");
        }
        
    } // setLarTable() ------------------------------------------------------
    
    
    
    public static float[] getLarTable(String key) throws AAtlantisException
    {
        logger.debug("Getting LAr pulse shapes lookup table \"" + key + "\"");
        if(larTables.containsKey(key))
        {
            float[] r = larTables.get(key);
            if(r == null)
            {
                throw new AAtlantisException("LAr pulse shape lookup table " +
                          "key \"" + key + "\" is null.");
            }
            else
            {
                return r;
            }
        }
        else
        {
            throw new AAtlantisException("LAr pulse shape lookup table " +
                      "entry \"" + key + "\" has not been created.");
        }
        
    } // getLarTable() ------------------------------------------------------


} // class ACalorimeterRPSPLT ===============================================
