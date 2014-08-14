package atlantis.nge;

/**
 *
 * @author Adam Davison
 */
public class ANRenderHints {

    private boolean pick;
    private ANPickHelper pickHelper;
    
    public ANRenderHints() {
        pick = false;
        pickHelper = null;
    }
    
    public ANPickHelper getPickHelper() {
        return pickHelper;
    }

    public void setPickHelper(ANPickHelper pickHelper) {
        this.pickHelper = pickHelper;
    }
    
    public boolean getPick() {
        return pick;
    }

    public void setPick(boolean pick) {
        this.pick = pick;
    }
    
}
