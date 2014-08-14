package atlantis.data;

import atlantis.event.*;
import atlantis.utils.AHashMap;

/**
 *
 * @author Eric Jansen
 */
public class AInDetSegmentData extends ASegmentData {
    
    /** Creates a new instance of AInDetSegmentData */
    AInDetSegmentData(AHashMap p, AEvent e) {
        super(p,e);
        
        event.getAssociationManager().add(new AAssociation(getFullName(), "TRT", numHits, hits,event));
    }
    
    public String getParameterGroup() {
        return "InDetSegment";
    }
    
    public String getName() {
        return "InDetSegment";
    }
    
    public String getNameScreenName() {
        return "InDetSegment";
    }    
}
