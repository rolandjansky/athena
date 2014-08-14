package atlantis.data;

import atlantis.event.*;
import atlantis.projection.AProjectionYX;
import atlantis.utils.AHashMap;

/**
 *
 * @author Eric Jansen
 */
public class AMuonSegmentData extends ASegmentData {
    
    /** Creates a new instance of AMuonSegmentData */
    AMuonSegmentData(AHashMap p, AEvent e) {
        super(p,e);
        String assocKey = getName() + getStoreGateKey();
        event.getAssociationManager().add(new AAssociation(assocKey, "MDT", numHits, hits, event));
        event.getAssociationManager().add(new AAssociation(assocKey, "RPC", numHits, hits, event));
        event.getAssociationManager().add(new AAssociation(assocKey, "TGC", numHits, hits, event));
        event.getAssociationManager().add(new AAssociation(assocKey, "CSC", numHits, hits, event));
    }

    public void makeDrawList() {
        super.makeDrawList();

        if (currentProjection instanceof AProjectionYX) {
            if (parameterStore.get("YX", "Mode").getI() != AProjectionYX.MODE_STANDARD) {
                numDraw = 0;
            } else {
                int num = 0;
                for (int i=0; i<numDraw; i++) {
                    int j = listdl[i];
                    
                    // Anything beyond z=13m or below r=6.5m is endcap
                    if (Math.sqrt(x[j]*x[j] + y[j]*y[j]) > 650.
                            && Math.abs(z[j]) < 1300.) {
                        listdl[num++] = j;
                    }
                }
                numDraw = num;
            }
        }
    }

    public String getParameterGroup() {
        return "MuonSegment";
    }
    
    public String getName() {
        return "MuonSegment";
    }
    
    public String getNameScreenName() {
        return "MuonSegment";
    }    
}
