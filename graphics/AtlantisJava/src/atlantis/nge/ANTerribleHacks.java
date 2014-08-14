package atlantis.nge;

import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.data.AJetData;
import atlantis.data.ALArData;
import atlantis.geometry.AAtlasDetector;
import atlantis.geometry.ABarrelCalorimeterDetector;
import atlantis.geometry.ABoxDetector;
import atlantis.geometry.ACalorimeterDetector;
import atlantis.geometry.ADetector;
import atlantis.geometry.ADetectors;
import atlantis.geometry.AEndcapCalorimeterDetector;
import atlantis.geometry.AEndcapCryostatDetector;
import atlantis.utils.A3Vector;
import java.util.ArrayList;

/**
 * This class encapsulates everything that we shouldn't have to do in one place
 * 
 * Mainly it relates to the fact that there's no way to ask the detector
 * store for something like the muon geometry only at the moment
 * 
 * One day if we really use this code then we need to add sensible access
 * methods like that to the detector store
 * 
 * This class should definitely be deleted one day
 * 
 * @author Adam Davison
 */
public class ANTerribleHacks {

    private static ArrayList<ABoxDetector> m_boxes = null;
    private static ArrayList<ACalorimeterDetector> m_calo = null;
    private static ArrayList<ACalorimeterDetector> m_extt = null;
    private static ArrayList<ACalorimeterDetector> m_mbts = null;
    private static ArrayList<ACalorimeterDetector> m_endc = null;

    public static ArrayList<ABoxDetector> getMuonDetectors() {
        if (m_boxes == null) {
            fillArrays();
        }
        return m_boxes;
    }

    private static void fillArrays() {
        m_boxes = new ArrayList<ABoxDetector>();
        m_calo = new ArrayList<ACalorimeterDetector>();
        m_extt = new ArrayList<ACalorimeterDetector>();
        m_mbts = new ArrayList<ACalorimeterDetector>();
        m_endc = new ArrayList<ACalorimeterDetector>();

        ADetectors[] ds = AAtlasDetector.getDetector().getDetectors();
        for (int i = 0; i < ds.length; i++) {
            ADetector[] d = ds[i].getDetectors();
            for (int j = 0; j < d.length; j++) {
                if (d[j] instanceof ABoxDetector) {
                    ABoxDetector bd = (ABoxDetector) d[j];
                    //TODO temporary code to show cut away detector
                    //if((bd.getPhi()>1.2 && bd.getPhi()<5.1) || !ACanvas.getCanvas().getPaintingWindow().getProjection().getName().equals("N3D"))
                    m_boxes.add(bd);
                } else if (d[j] instanceof ABarrelCalorimeterDetector) {
                    ACalorimeterDetector cd = (ACalorimeterDetector) d[j];
                    //extended tile only has one end in geom file and not relicated like others
                    if (d[j].getName().equals("Extended TILE")) {
                        m_extt.add(cd);
                    } else {
                        m_calo.add(cd);
                    }

                } else if (d[j] instanceof AEndcapCryostatDetector) {
                    //adds MBTS but only has one end in geom file
                    AEndcapCryostatDetector ecd = (AEndcapCryostatDetector) d[j];
                    m_mbts.add(ecd);
                } else if (d[j] instanceof AEndcapCalorimeterDetector) {
                    //adds LAr endcap + HEC but only has one end in geom file
                    AEndcapCalorimeterDetector ecd = (AEndcapCalorimeterDetector) d[j];
                    m_endc.add(ecd);

                }

            }
        }
    }

    public static A3Vector getCellCentreLArForJet(int j) {
        float xsum = 0.0f;
        float ysum = 0.0f;
        float zsum = 0.0f;
        int count = 0;
        AEvent ev = AEventManager.instance().getCurrentEvent();
        AJetData jd = ev.getJetData();
        String assockey = jd.getFullName();
        int[][] assoc = ev.getAssociationManager().get(assockey, "LAr");
        int[] dl = new int[assoc[j].length];
        ALArData calo = ev.getLArData();
        for (int i = 0; i < assoc[j].length; i++) {
            int index = calo.getIndexFromId(assoc[j][i]);
            float eta = (float) (calo.getEta(index));

            float theta = (float) (2.0f * Math.atan(Math.exp(-eta)));

            float phi = (float) (calo.getPhi(index));

            float rmin = (float) calo.getDetector(index).getRMin();
            float rmax = (float) calo.getDetector(index).getRMax();
            float r = 0.5f * (rmin + rmax);

            xsum += (float) (r * Math.cos(phi));
            ysum += (float) (r * Math.sin(phi));
            zsum += (float) (r / Math.tan(theta));
            count++;
        }
        
        
        
        return new A3Vector(xsum / count, ysum / count, zsum / count);
    }
}
