package atlantis.event;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.TreeMap;
import java.util.LinkedHashMap;
import java.util.Set;

import atlantis.list.AListManager;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.AEnumeratorParameter;
import atlantis.parameters.APar;
import atlantis.parameters.AParameterUtilities;
import atlantis.projection.AProjection;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;
import atlantis.data.*;
import atlantis.globals.AGlobals;

/**
 * AEvent is the Container for an ATLAS physics event.
 * It is identified by deriving from AEventInfo and holds a map of all the
 * data that is available in the event.
 *
 */
public class AEvent extends AEventInfo
{
    private static ALogger logger = ALogger.getLogger(AEvent.class);
    
    // container with all datatypes within the event
    private Map<String,AData> data = new HashMap<String,AData>();
    // storegate keys for collections-aware datatypes
    // Use parameterized type but keep Hashtable and Vector instead of HashMap 
    // and ArrayList until I work out if thread safety is an issue. - Ben 31/1/11
    private Hashtable<String,Vector<String>> collections = new Hashtable<String,Vector<String>>(); 
 
    private AAssociationManager assocmgr = new AAssociationManager();
    
    private static APar parameterStore = APar.instance();

  /**
   * @param eventNumber the event number
   * @param runNumber the run number
   * @param dateTime  the date and time of the event
   * @param sourceName the name of the event source
   * @param lumiBlock the lumiBlock number
   */
  public AEvent(String eventNumber, String runNumber, String dateTime, String sourceName, String lumiBlock, String eventProperties) {
    
    //Initialize event information
    super(Integer.parseInt(eventNumber), Integer.parseInt(runNumber), dateTime, sourceName, lumiBlock, eventProperties);

  } // AEvent() -----------------------------------------------------------
  
  /**
   * @return the hashtable of available collections
   */ 
  public Hashtable<String,Vector<String>> getCollections(){
        return collections;
  } // getCollections() ---------------------------------------------------


    public void add(AData dataSet) throws AAtlantisException
    {
        String name = dataSet.getClass().getName();

        int index = name.lastIndexOf('.');
        if(index >= 0 && index < name.length() - 1)
        {
            name = name.substring(index + 1);
        }

        // remove the "A" and "Data"
        name = name.substring(1, name.length() - 4);
        String newKey = dataSet.getStoreGateKey();

        if(newKey == null)
        {
            // no storegatekey comes with this dataset (datatype)
            // hash data shouldn't contain entry under sourceName key, if it does
            // the old one is replaced
            data.put(name, dataSet);
        }
        else
        {
            // just checking for the same key at the datatypes in the event
            if(data.containsKey(name + newKey))
            {
                String msg = "Error - datatype " + name + " with storeGateKey\n" +
                newKey + " already exists in this event.";
                AAtlantisException aaex = new AAtlantisException(msg);
                // Stuck the trace in the logging for good measure - AD
                logger.error("\n" + msg, aaex);
                throw aaex;
            }
            else
            {
                data.put(name + newKey, dataSet);
                // saving key of a particular sourceName (datatype)
                Vector<String> keys = new Vector<String>();
                if(collections.containsKey(name))
                {
                    keys = collections.get(name);
                    if(keys.contains(newKey))
                    {
                        String msg = "Previously read datatype " + name + "\n" +
                        "(different collection) contained " +
                        "storeGateKey " + newKey + ". Error.";
                        AAtlantisException aaex = new AAtlantisException(msg);
                        logger.error(msg, aaex);
                        throw aaex;
                    }
                    else
                    {
                        keys.add(newKey);
                        collections.put(name, keys);
                    }
                }
                else
                {
                    keys.add(newKey);
                    collections.put(name, keys);
                }
            }
        }

    } // add() ---------------------------------------------------------------
 


 

    // < get<DATA_TYPE> methods > -------------------------------------------


    public AData get(String name)
    {
        return data.get(name);
    }

    public AData[] getData()
    {
        AData[] aData = new AData[data.size()];
        Collection<AData> collection = data.values();
        return collection.toArray(aData);
    }


    /**
     * AJetData getJetData()
     * returns collection of Jet datatype according to selected
     * collection (Jet storeGateKey) in ATLAS -> Jet -> Jet Collection
     * if 'all' (or 'none') is selected - returns null (no association will
     * be taken into account)
     * @return AJetData
     */
    public AJetData getJetData()
    {
        AEnumeratorParameter listBox =
             (AEnumeratorParameter) parameterStore.get("Jet", "JetCollections");
        String selectedKey = listBox.getCurrentText();
        if("All".equals(selectedKey) || "None".equals(selectedKey))
{
            logger.debug("AEvent.getJetData(): " + selectedKey +
                    " is selected in Jet->JetCollection, no association " +
                    " to Jet is taken into account, null is returned");
            return null;
        }
        else
        {
            AJetData jet = (AJetData) data.get("Jet" + selectedKey);
            return jet;
        }
    }


    // can combine with the other methods (e.g. getClusterData)
    public AData getData(String type)
    {
        AEnumeratorParameter listBox =
             (AEnumeratorParameter) parameterStore.get(type, type+"Collections");
        String selectedKey = (listBox == null) ? "" : listBox.getCurrentText();
        if("All".equals(selectedKey) || "None".equals(selectedKey)) {

            logger.debug("AEvent.getData(String type): " + selectedKey +
                    " is selected in " + type + "->" + type + " Collections, " +
                    "no association to " + type + " is taken into account.");

            return null;
        }
        else
        {
            return data.get(type + selectedKey);
        }
    }

    /**
     * AClusterData getClusterData()
     * returns collection of Cluster datatype according to selected
     * collection (Cluster storeGateKey) in ATLAS -> Cluster -> Cluster Collection
     * if 'all' (or 'none') is selected - returns null (no association will
     * be taken into account)
     * @return AClusterData
     */
    public AClusterData getClusterData()
    {
        AEnumeratorParameter listBox =
             (AEnumeratorParameter) parameterStore.get("Cluster", "ClusterCollections");
        String selectedKey = listBox.getCurrentText();
        if ("All".equals(selectedKey) || "None".equals(selectedKey)) {

            logger.debug("AEvent.getClusterData(): " + selectedKey +
                    " is selected in Cluster->ClusterCollection, no association " +
                    " to Cluster is taken into account, null is returned");

            return null;
        }
        else
        {
            AClusterData cluster = (AClusterData) data.get("Cluster" + selectedKey);
            return cluster;
        }
    }


    /**
     * ATrackData getTrackData(String type)
     * type is InDetTrack, MuonTrack - the internal datatype sourceName for Track
     * returns collection of Track datatype according to selected
     * collection (Track storeGateKey) in InDet -> Track -> TrackCollection
     * if 'all' (or 'none') is selected - returns null (no association will
     * be taken into account)
     * @param type String
     * @return ATrackData
     */
    public ATrackData getTrackData(String type)
    {
        AEnumeratorParameter listBox =
             (AEnumeratorParameter) parameterStore.get(type, type + "Collections");
        String selectedKey = listBox.getCurrentText();
        if("All".equals(selectedKey) || "None".equals(selectedKey))
        {
            logger.debug("AEvent.getTrackData(" + type + "): " + selectedKey +
                    " is selected in Track->TrackCollection, no association " +
                    " to Track is taken into account, null is returned");
            return null;
        }
        else
        {
            ATrackData track = (ATrackData) data.get(type + selectedKey);
            return track;
        }
    }


    /**
     * ATrackData getTrackData(String type, String key)
     * returns specified collection of Track datatype if it exists
     * @param type String
     * @param key String
     * @return ATrackData
     */
    public ATrackData getTrackData(String type, String key)
    {
        Vector<String> keys = collections.get(type);
        if(keys == null) {
            logger.warn("AEvent: " + type + " datatype doesn't exist " +
                    "in current event");
            return null;
        }
        else
        {
            if(keys.contains(key))
            {
                return (ATrackData) data.get(type + key);
            }
            else {

                logger.warn("AEvent: " + type + " datatype: storeGateKey " +
                        key + " doesn't exist in current event");
                return null;
            }
        }
    }




    public ASTrData getSTrData()
    {
        return (ASTrData) data.get("STr");
    }

    public ASNPData getSNPData()
    {
        return (ASNPData) data.get("SNP");
    }

    public APixelClusterData getPixelClusterData()
    {
        return (APixelClusterData) data.get("PixelCluster");
    }
    
    public ASiClusterData getSiClusterData()
    {
        return (ASiClusterData) data.get("SiCluster");
    }

    public AS3DData getS3DData()
    {
        return (AS3DData) data.get("S3D");
    }

    public ATRTData getTRTData()
    {
        return (ATRTData) data.get("TRT");
    }

    public ALArData getLArData()
    {
        return (ALArData) data.get("LAr");
    }

    public ATILEData getTILEData()
    {
        return (ATILEData) data.get("TILE");
    }

    public AHECData getHECData()
    {
        return (AHECData) data.get("HEC");
    }

    public AFCALData getFCALData()
    {
        return (AFCALData) data.get("FCAL");
    }
    
    public AMBTSData getMBTSData()
    {
        return (AMBTSData) data.get("MBTS");
    }

    //Because some things need to access the data before the parameters are filled, another method
    //for accessing the data has been added. This method first checks the list of available storegate keys
    //and see's if there is one starting with MDT. As the dataType + storegatekey is hard coded into how
    //the hash map keys are created this should be a stable way of doing it. If there are multiple 
    //results for the search, it then checks which one is currently selected by the user. 

    public AMDTData getMDTData()
    {
	Vector<String> storeGateKeys = new Vector<String>();
	Set<String> keys = data.keySet();
	for(String entry : keys){
	    if(entry.startsWith("MDT")){
		storeGateKeys.add(entry);
	    }
	}
	if(storeGateKeys.size() == 1){
	    return (AMDTData) data.get(storeGateKeys.get(0));
	}else{

	    AEnumeratorParameter listBox =
		(AEnumeratorParameter) parameterStore.get("MDT", "MDTCollections");
	    String selectedKey = listBox.getCurrentText();
	    if("All".equals(selectedKey) || "None".equals(selectedKey))
		{
		    logger.debug("AEvent.getMDTata(): " + selectedKey +
				 " is selected in MDT->MDTCollection, no association " +
				 " to MDT is taken into account, null is returned");
		    return null;

		}
	    else{
		    AMDTData mdt = (AMDTData) data.get("MDT" + selectedKey);
		    return mdt;
	    }


	}

    }

    //See description for getMDTData

    public ARPCData getRPCData()
    {

	Vector<String> storeGateKeys = new Vector<String>();
	Set<String> keys = data.keySet();
	for(String entry : keys){
	    if(entry.startsWith("RPC")){
		storeGateKeys.add(entry);
	    }
	}
	if(storeGateKeys.size() == 1){
	    return (ARPCData) data.get(storeGateKeys.get(0));
	}else{

	    AEnumeratorParameter listBox =
		(AEnumeratorParameter) parameterStore.get("RPC", "RPCCollections");
	    String selectedKey = listBox.getCurrentText();
	    if("All".equals(selectedKey) || "None".equals(selectedKey))
		{
		    logger.debug("AEvent.getRPCData(): " + selectedKey +
				 " is selected in RPC->RPCCollection, no association " +
				 " to RPC is taken into account, null is returned");
		    return null;

		}
	    else{
		    ARPCData rpc = (ARPCData) data.get("RPC" + selectedKey);
		    return rpc;
	    }


	}
    }
    //See description for getMDTData

    public ATGCData getTGCData()
    {
	Vector<String> storeGateKeys = new Vector<String>();
	Set<String> keys = data.keySet();
	for(String entry : keys){
	    if(entry.startsWith("TGC")){
		storeGateKeys.add(entry);
	    }
	}
	if(storeGateKeys.size() == 1){
	    return (ATGCData) data.get(storeGateKeys.get(0));
	}else{

	    AEnumeratorParameter listBox =
		(AEnumeratorParameter) parameterStore.get("TGC", "TGCCollections");
	    String selectedKey = listBox.getCurrentText();
	    if("All".equals(selectedKey) || "None".equals(selectedKey))
		{
		    logger.debug("AEvent.getTGCData(): " + selectedKey +
				 " is selected in TGC->TGCCollection, no association " +
				 " to TGC is taken into account, null is returned");
		    return null;

		}
	    else{
		    ATGCData tgc = (ATGCData) data.get("TGC" + selectedKey);
		    return tgc;
	    }


	}
    }
    //See description for getMDTData

    public ACSCDData getCSCDData()
    {
	Vector<String> storeGateKeys = new Vector<String>();
	Set<String> keys = data.keySet();
	for(String entry : keys){
	    if(entry.startsWith("CSC")){
		storeGateKeys.add(entry);
	    }
	}
	if(storeGateKeys.size() == 1){
	    return (ACSCDData) data.get(storeGateKeys.get(0));
	}else{

	    AEnumeratorParameter listBox =
		(AEnumeratorParameter) parameterStore.get("CSC", "CSCCollections");
	    String selectedKey = listBox.getCurrentText();
	    if("All".equals(selectedKey) || "None".equals(selectedKey))
		{
		    logger.debug("AEvent.getCSCData(): " + selectedKey +
				 " is selected in CSC->CSCCollection, no association " +
				 " to CSC is taken into account, null is returned");
		    return null;

		}
	    else{
		    ACSCDData csc = (ACSCDData) data.get("CSC" + selectedKey);
		    return csc;
	    }


	}
    }

    public ASVxData getSVxData()
    {
        return (ASVxData) data.get("SVx");
    }

    public ARVxData getRVxData()
    {     
        AEnumeratorParameter listBox =
             (AEnumeratorParameter) parameterStore.get("RVx", "RVxCollections");
        String selectedKey = listBox.getCurrentText();
        if ("All".equals(selectedKey)) {

            logger.debug("AEvent.getRVxData(): " + selectedKey +
                    " is selected in InDet->RecVertex, no association " +
                    " to vertex is taken into account, null is returned");

            return null;
        } else if ("None".equals(selectedKey)) {
            // Return  collection without for backwards compatibility
            return (ARVxData) data.get("RVx");
        } else
        {
            ARVxData rvx = (ARVxData) data.get("RVx" + selectedKey);
            return rvx;
        }
    }

    public ATrigS3DData getTrigS3DData()
    {
        return (ATrigS3DData) data.get("TrigS3D");
    }

    public ALVL1TriggerTowerData getLvl1TriggerTowerData()
    {
        return (ALVL1TriggerTowerData) data.get("LVL1TriggerTower");
    }

    public ALVL1ResultData getLvl1ResultData()
    {
        return (ALVL1ResultData) data.get("LVL1Result");
    }

    public ALVL1JetElementData getLvl1JetElementData()
    {
        return (ALVL1JetElementData) data.get("LVL1JetElement");
    }
    
    public ATriggerInfoData getTriggerInfoData()
    {
        return (ATriggerInfoData) data.get("TriggerInfo");
    }

    /**
     * This function returns the names currently selected for a certain collection
     * which has its data draw status set to true 
     * @param collectionType String sourceName of the collection
     * @return string of collection names length 0 if none in event
     */
    public String[] getActiveCollectionNames(String collectionType)
    {
    //LVL1Result is not on the Data list as it is always on hence check this first
    boolean collectionStatus = collectionType.equals("LVL1Result");
    if(!collectionStatus)
        collectionStatus=parameterStore.get("Data", collectionType).getStatus();
    if (collectionStatus)
    {
    	return getCollectionNames(collectionType);
    }
    else
        return new String[0];
    }

    /**
     * This function returns the names currently selected for a certain collection
     * @param collectionType String sourceName of the collection
     * @return string of collection names length 0 if none in event
     */
    public String[] getCollectionNames(String collectionType)
    {
    String[] col;
    Vector<String> keys = collections.get(collectionType);
    if (keys != null)
    {
        String[] collec = keys.toArray(new String[keys.size()]);
        AEnumeratorParameter listBox = (AEnumeratorParameter) parameterStore.get(collectionType, collectionType + "Collections");
        String currentSelection = listBox.getCurrentText();
        int count=0;
        //count how many items in list
        for (int i = 0; i < collec.length; i++)
        {
            if ("All".equals(currentSelection) || collec[i].equals(currentSelection))
                count++;
        }
        col=new String[count];
        count=0;
        //now save items into string array
        for (int i = 0; i < collec.length; i++)
        {
            if ("All".equals(currentSelection) || collec[i].equals(currentSelection))
            {
                col[count] = collectionType + collec[i];
                count++;
            }
        }
        return col;
    }
    else
        return new String[0];
    }

    // </ get<DATA_TYPE> methods > --------------------------------------------

    public List<ACalorimeterData> getCalorimeters()
    {
        List<ACalorimeterData> v = new ArrayList<ACalorimeterData>();
        Iterator<AData> i = data.values().iterator();
        while(i.hasNext())
        {
            AData data = i.next();
            if(data instanceof ACalorimeterData)
            {
                ACalorimeterData calorimeter = (ACalorimeterData) data;
                if(parameterStore.get("Data", calorimeter.getCalorimeterName()).getStatus() &&
                        (parameterStore.getUnknown("Det", calorimeter.getCalorimeterName() + "Fill") == null
                                || parameterStore.get("Det", calorimeter.getCalorimeterName() + "Fill").getStatus()))
                {
                    v.add(calorimeter);
                }
            }
        }
        return v;

    } // getCalorimeters() ---------------------------------------------------



    public List<AData> getHitsAndTracks(AProjection projection)
    {
        int mode = parameterStore.get(projection.getName(), "Mode").getI();
        String[][] det = new String[0][0];
        String[] simTr = new String[0];
        String[] recTr = new String[0];
        String[] hits = new String[0];
        String[] aod = new String[0];

        if(mode == 0 || projection instanceof AProjectionVP
                || projection instanceof AProjectionLegoPlot)
        {
            simTr = new String[] { "STr", "SNP", "SMTr" };
            recTr = new String[] { "InDetSegment", "MuonSegment", "G4Step",
                                   "InDetTrack", "MuonTrack" };

            aod = new String[] {"CompositeParticle", "BJet", "TauJet", "Photon", "Electron", "Muon" };

            if(parameterStore.get("InDetDrawingOrder", "SpacePoints").getI() == 0)
            {
                hits = new String[] {"UserHit", "RVx", "TRT", "SiCluster",
                        "SiClusterRDO", "S3D", "PixelCluster", "PixelRDO", "TrigS3D",
                        "MDT", "CSCD", "CSC", "RPC", "R3D", "TGC", "T3D", "SVx",
                        "Jet", "EmTauROI", "Particle", "Cluster", "ETMis",
                        "JetROI", "MuonROI", "LVL1TriggerTower", "LVL1JetElement"};
            }
            else
            {
                hits = new String[] {"UserHit", "RVx", "TRT", "SiCluster",
                        "SiClusterRDO", "TrigS3D", "S3D", "PixelCluster", "PixelRDO",
                        "MDT", "CSCD", "CSC", "RPC", "R3D", "TGC", "T3D", "SVx",
                        "Jet", "EmTauROI", "Particle", "Cluster", "ETMis",
                        "JetROI", "MuonROI", "LVL1TriggerTower", "LVL1JetElement"};
            }
        }
        else if(projection instanceof AProjectionYX
                || projection instanceof AProjectionFR)
        {
            if(mode >= AProjectionYX.MODE_MDT_INNER && mode <= AProjectionYX.MODE_MDT_OUTER)
            {
                simTr = new String[] { "SMTr", "SMTr" };
                recTr = new String[] { "MuonTrack", "MuonSegment" };
                if(mode == AProjectionYX.MODE_MDT_INNER)
                {
                    hits = new String[] { "MDT", "CSCD", "CSC" };
                }
                else
                {
                    hits = new String[] { "MDT" };
                }
            }
            else if(mode < AProjectionYX.MODE_MDT_INNER)
            {
                simTr = new String[] { "SMTr", "SMTr" };
                recTr = new String[] { "MuonTrack", "MuonSegment" };
                hits = new String[] { "TGC", "T3D" };
            }
        }
        else if(projection instanceof AProjectionFZ)
        {
            if(mode >= 4)
            {
                simTr = new String[] { "SMTr", "SMTr" };
                recTr = new String[] { "MuonTrack", "MuonSegment" };
                hits = new String[] { "MDT" };
            }
            else
            {
                simTr = new String[] { "SMTr", "SMTr" };
                recTr = new String[] { "MuonTrack", "MuonSegment" };
                hits = new String[] { "RPC", "R3D" };
            }
        }

        if(parameterStore.get("InDetDrawingOrder", "SpacePointsTracks").getI() == 0)
        {
            // Simulated tracks, reconstructed tracks, hits
            det = new String[][] { simTr, recTr, hits, aod };
        }
        else if(parameterStore.get("InDetDrawingOrder", "SpacePointsTracks").getI() == 1)
        {
            // Simulated tracks, hits, reconstructed tracks
            det = new String[][] { simTr, hits, recTr, aod };
        }
        else
        {
            // Reconstructed tracks, hits, simulated tracks
            det = new String[][] { recTr, hits, simTr, aod };
        }

        AEnumeratorParameter listBox = null;
        List<AData> v = new ArrayList<AData>();
        for(int i = 0; i < det.length; ++i)
        {
            for(int j = 0; j < det[i].length; ++j)
            {
                if(collections.containsKey(det[i][j]))
                {
                    Vector<String> keys = collections.get(det[i][j]);
                    String[] array;
                    array = keys.toArray(new String[keys.size()]);
                    // GUI listbox sourceName datatype + "Collections"
                    listBox = (AEnumeratorParameter) parameterStore.get(det[i][j],
                              det[i][j] + "Collections");
                    String currentSelection = listBox.getCurrentText();
                    for(int c = 0; c < array.length; c++)
                    {
                        if("All".equals(currentSelection) ||
                           array[c].equals(currentSelection))
                        {
                            AData a = data.get(det[i][j] + array[c]);
                            processDataSet(v, a);
                        }
                    }
                }
                else
                {
                    AData a = data.get(det[i][j]);
                    processDataSet(v, a);
                }
            }
        }
        return v;

    } // getHitsAndTracks() -------------------------------------------------


    private static void processDataSet(List<AData> v, AData a)
    {
        if(a != null)
        {
            if(parameterStore.getUnknown("Data", a.getName()) != null &&
                    parameterStore.get("Data", a.getName()).getStatus() &&
                    (parameterStore.getUnknown("Det", a.getName() + "Fill") == null ||
                            parameterStore.get("Det", a.getName() + "Fill").getStatus()))
            {
                v.add(a);
            }
        }

    } // processDataSet() ---------------------------------------------------



    public List<ACalorimeterData> getElectromagneticCalorimeters()
    {
        List<ACalorimeterData> v = new ArrayList<ACalorimeterData>();
        Iterator<AData> i = data.values().iterator();
        while(i.hasNext())
        {
            AData a = i.next();
            if(a instanceof ALArData || a instanceof AFCALData)
            {
                v.add((ACalorimeterData)a);
            }
        }
        return v;

    } // getElectromagneticCalorimeters() ------------------------------------



    public List<ACalorimeterData> getHadronicCalorimeters()
    {
        List<ACalorimeterData> v = new ArrayList<ACalorimeterData>();
        Iterator<AData> i = data.values().iterator();
        while(i.hasNext())
        {
            AData a = i.next();
            if(a instanceof ATILEData || a instanceof AHECData
                    || a instanceof AFCALData || a instanceof AMBTSData)
            {
                v.add((ACalorimeterData)a);
            }
        }
        return v;

    } // getHadronicCalorimeters() -------------------------------------------



    /**
     * setPrimaryVertex() called during event finalisation
     * sets Event: XVtx, YVtx, ZVtx in the internal parameter store
     * (in GUI: Projection -> eta-phi -> XVtx, YVtx, ZVtx)

     * 1) gets values from RecVertex (RVx) - see conditions in ARVxData class
     * 2) gets values from simulated vertices with highest pt sum (STr)
     * 3) if above fail, sets 0.0, 0.0, 0.0
     *
     * (taking primary vertex from reconstructed tracks was removed)
     *
     */
    public void setPrimaryVertex()
    {
        double[] vtx = null;
        
        // (1)
        if(getRVxData() != null)
        {
            // RVx data exists, try to retrieve primary vertex information
            vtx = getRVxData().getPrimaryVertex();
        }
        // (2)
        if(vtx == null && getSTrData() != null)
        {
          // STr data exists, try to get primary vertex information
          int vtxIndex = getSTrData().getMaxSumPtVertex();
          
          //retrieve that vertex
          vtx = getSVxData().getVertex(vtxIndex);

          //Completely unclear to me... why 4? 
          if(vtx[0] * vtx[0] + vtx[1] * vtx[1] > 4.)
          {
            logger.info("replacing primary with first");
            logger.info("  was " + vtx[0] + " " + vtx[1] + " " + vtx[2]);
            vtx = getSVxData().getVertex(0);
            if(vtx[0] * vtx[0] + vtx[1] * vtx[1] > 4.)
            {
                vtx[0] = 0.;
                vtx[1] = 0.;
                vtx[2] = 0.;
            }
            logger.info("  now is " + vtx[0] + " " + vtx[1] + " " + vtx[2]);
          }
        }
        
        if(vtx == null)
        {
            vtx = new double[] { 0.0, 0.0, 0.0 };
        }

        parameterStore.get("Event", "XVtx").setD(vtx[0]);
        parameterStore.get("Event", "YVtx").setD(vtx[1]);
        parameterStore.get("Event", "ZVtx").setD(vtx[2]);

        AOutput.append("\n\nPrimary vertex set for projection " +
                       AMath.PHI + AMath.ETA + ":\n" +
                       "  XVtx = " + String.format("%.5f",vtx[0]) + " cm\n" +
                       "  YVtx = " + String.format("%.5f",vtx[1]) + " cm\n" +
                       "  ZVtx = " + String.format("%.5f",vtx[2]) + " cm\n", ALogInterface.NORMAL);

    } // setPrimaryVertex() -------------------------------------------------



    /**
     * Finalize event construction:
     * - finalize all data objects
     * - set primary vertex
     * - calculate the number of hits on a track
     * - create associations
     * - update collections
     * @return the event
     */
    public AEvent finalizeEvent()
    {

        // setting primary vertex must be done before finalizeConstruction()
        // happens on datatypes!
        setPrimaryVertex();
     
        Iterator<AData> iter = data.values().iterator();
        while(iter.hasNext())
        {
            AData data = iter.next();
            if(data instanceof AData)
            {
              try {
                ((AData) data).finalizeConstruction();
              } catch ( Exception e ){
                  //Get sourceName of the object where the error occured
                  String ObjName = ((AData)data).getFullName();
                  logger.error("Exception while finalizing construction of "+ObjName,e);
                  AOutput.append("\n\nError while constructing " + ObjName +"\n", ALogInterface.WARNING);
                  AOutput.append(" - object will not be shown!\n",ALogInterface.WARNING);
                  iter.remove();

              }
            }
        }

        if (!AGlobals.isAtlantisHeadless()) {
            AListManager.getInstance().resetAndPreserveInvisible();
        }

        // are there any RVx read in from the event file? if so, cut the tracks
        // which form vertices (when drawn as helices) to the reconstructed
        // vertices.
        ARVxData rvxData = this.getRVxData();
        if(rvxData != null)
        {
            rvxData.cutTracksToRVx(this);
        }

        assocmgr.correct();

        return this;

    } // AEvent finalizeEvent() ---------------------------------------------


    /**
     * Returns information about datatypes and number of items datatypes in
     * the current event
     * First item of the result array is the datatype sourceName followed
     * by ":<storeGateKey>" if that exists for a datatype and second item is
     * integer number (numData).
     * The array is alphabetically sorted.
     *
     * @return String[][]
     */
    public String[][] getInfo()
    {
        AData[] sources = getData();
        Map<String,String> m = new LinkedHashMap<String,String>();
        m.clear();
        for(int i = 0; i < sources.length; i++)
        {
            String sg = sources[i].getStoreGateKey();
            String a = sources[i].getNameScreenName();
            a += sg != null ? ":" + sg : "";
            m.put(a, String.valueOf(sources[i].getNumData()));
        }

        TreeMap<String,String> tm = new TreeMap<String,String>(String.CASE_INSENSITIVE_ORDER);
        tm.clear();
        tm.putAll(m);

        String[][] s = new String[tm.size()][2];
        int i = 0;
        for(Iterator<String> iter = tm.keySet().iterator() ; iter.hasNext() ; i++)
        {
            String key = iter.next();
            s[i][0] = key;
            s[i][1] = tm.get(key);
        }

        return s;

    } // getInfo() -----------------------------------------------------------


    /**
     * Returns information about datatypes and number of items datatypes in
     * the current event drawn for all event data, and data passing cuts
     * First item of the result array is the datatype sourceName followed
     * by ":<storeGateKey>" if that exists for a datatype and second/third items are
     * integer number (numData, numDraw).
     * The array is alphabetically sorted.
     *
     * @return String[][]
     */
    public String[][] getInfoDraw()
    {
        AData[] sources = getData();
        Map<String,String> m = new LinkedHashMap<String,String>();
        m.clear();
        Map<String,String> mDraw = new LinkedHashMap<String,String>();
        mDraw.clear();
        
        for(int i = 0; i < sources.length; i++)
        {
            String sg = sources[i].getStoreGateKey();
            String a = sources[i].getNameScreenName();
            a += sg != null ? ":" + sg : "";
            m.put(a, String.valueOf(sources[i].getNumData()));
            mDraw.put(a, String.valueOf(sources[i].getNumDraw()));
        }

        TreeMap<String,String> tm = new TreeMap<String,String>(String.CASE_INSENSITIVE_ORDER);
        tm.clear();
        tm.putAll(m);
        TreeMap<String,String> tmDraw = new TreeMap<String,String>(String.CASE_INSENSITIVE_ORDER);
        tmDraw.clear();
        tmDraw.putAll(mDraw);
        

        String[][] s = new String[tm.size()][3];
        int i = 0;
        for(Iterator<String> iter = tm.keySet().iterator() ; iter.hasNext() ; i++)
        {
            String key = iter.next();
            
            s[i][0] = key;
            s[i][1] = tm.get(key);
            s[i][2] = tmDraw.get(key);
        }

        return s;

    } // getInfoDraw() -----------------------------------------------------------
    

    /**
     * Get coordinates of event vertex.
     * 
     * The coordinates are obtained from the parameter store rather than
     * from the event itself, since the vertex position can be changed by the
     * user through the GUI.
     * 
     * @return (x,y,z) of vertex
     */
    public double[] getPrimaryVertex()
    {
        return AParameterUtilities.getPrimaryVertex();
    }
    
    public AAssociationManager getAssociationManager() {
        return assocmgr;
    }
    
    
} // class AEvent ------------------------------------------------------------
