package atlantis.nge;

import atlantis.event.AEventManager;
import atlantis.data.ACalorimeterData;
import atlantis.event.AEvent;
import atlantis.data.AJetData;
import atlantis.data.ATrackData;
import atlantis.geometry.ABoxDetector;
import atlantis.nge.object.ANObject;
import atlantis.nge.object.ANObjectList;
import atlantis.nge.object.data.ANObjectPointing;
import atlantis.nge.object.data.ANObjectTracks;
import atlantis.nge.object.geometry.ANObjectGeomMuon;
import atlantis.utils.ALogger;
import atlantis.utils.AMath;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * These things get arrays of ANObjects out of the Atlantis data store
 * 
 * The general idea is to separate the selection of objects from the way
 * they're represented in the projection. But I suspect that'll break down
 * 
 * For now it's a static object but if we ever want to unstatic it then it's
 * only accessible via getGenerator() so it should be easy to refactor
 * 
 * A lot of the functionality should one day be pushed back into the detector
 * objects themselves
 * 
 * @author Adam Davison
 */
public class ANObjectGenerator {

    private static ALogger logger = ALogger.getLogger(ANObjectGenerator.class);
    private static AEventManager eventManager = AEventManager.instance();
    
    private static ANObjectGenerator thegenerator = new ANObjectGenerator();

    private ANObjectGenerator() {
    }

    public static ANObjectGenerator getGenerator() {
        return thegenerator;
    }

    public ANObject getGeometryMuon() {
        return new ANObjectGeomMuon(ANTerribleHacks.getMuonDetectors());
    }
    
    // Ideally change layout of detector stuff so this can be more generic
    // not like one search per type, since basically everything has eta/phi
    public ANObject getGeometryMuon(float phi, float eta, float dr) {
        ArrayList<ABoxDetector> boxes = ANTerribleHacks.getMuonDetectors();
        ArrayList<ABoxDetector> hits = new ArrayList<ABoxDetector>();

        for (int i = 0; i < boxes.size(); i++) {
            ABoxDetector bd = boxes.get(i);
            // TODO - This formula is wrong, it's only checking one corner
            float etabd = (float) AMath.etaAbs(bd.getZMin(), bd.getRMin());
            if (AMath.deltaR(phi, eta, (float) bd.getPhi(), etabd) < dr) {
                hits.add(bd);
            }
        }

        return new ANObjectGeomMuon(hits);
    }
    
    public ANObject getDataCaloForJet(int j, String key) {
        
        ANObjectList ol = new ANObjectList();
        
        AEvent ev = eventManager.getCurrentEvent();
        AJetData jd = ev.getJetData();
        String assockey = jd.getFullName();
        int[][] assoc = ev.getAssociationManager().get(assockey, key);
        if (assoc == null) { return ol; }
        if (assoc[j] == null) { return ol; }
        int[] dl = new int[assoc[j].length];
        List calolist = ev.getCalorimeters();
        ACalorimeterData calo = null;
        for (int i = 0; i < calolist.size(); i++) {
            ACalorimeterData tc = (ACalorimeterData)(calolist.get(i));
            if (tc.getName().equals(key)) {
                calo = tc;
            }
        }
        if (calo == null) {
            logger.error("Requested unavailable calo key!");
            return ol;
        }
        
        calo.makeDrawList();
        int[] calodl = calo.getDrawList();
        
        IntBuffer ib = IntBuffer.allocate(assoc[j].length);
        for (int i = 0; i < assoc[j].length; i++) {
            int id = assoc[j][i];
            int index = calo.getIndexFromId(id);
            boolean found = false;
            for (int d = 0; d < calo.getNumDraw(); d++) {
                if (calodl[d] == index) {
                    found = true;
                }
            }
            if (found) {
                ib.put(index);
            }
        }
        
        ol.addObject(new ANObjectPointing(calo, ib.array(), ib.position(), true));
        ol.addObject(new ANObjectPointing(calo, ib.array(), ib.position(), false));
        
        return ol;
    }

    public ANObject getInDetTracksForJet(int j) {
        AEvent ev = eventManager.getCurrentEvent();
        AJetData jd = ev.getJetData();
        int jetid = jd.getIdFromIndex(j);
        ATrackData tracks = ev.getTrackData("InDetTrack");
        int[] trackdl = tracks.getDrawList();
        String jetassockey = jd.getFullName();
        String trackassockey = tracks.getName() + tracks.getStoreGateKey();
        int[][] assoc = ev.getAssociationManager().get(trackassockey, jetassockey);
        
        IntBuffer ib = IntBuffer.allocate(assoc.length);
        for (int i = 0; i < assoc.length; i++) {
            for (int s = 0; s < assoc[i].length; s++) {
                if (assoc[i][s] == jetid) {
                    boolean display = false;
                    for (int d = 0; d < trackdl.length; d++) {
                        if (trackdl[d] == i) {
                            display = true;
                        }
                    }
                    if (display) {ib.put(i);}
                }
            }
        }
        
        return new ANObjectTracks(tracks, ib.array());
    }
    
    public ANObject getInDetTracks() {
        AEvent ev = eventManager.getCurrentEvent();
        ATrackData td = ev.getTrackData("InDetTrack");
        return new ANObjectTracks(td);
    }
}
