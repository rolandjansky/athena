package atlantis.nge.object;

import atlantis.nge.*;

/**
 * An ANObject is the superclass of anything which knows how to draw itself
 * using an ANRenderer
 * 
 * Specifically these will tend to represent some kind of visual onscreen
 * element and will contain all the information required to draw the
 * representation
 * 
 * @author Adam Davison
 */
public abstract class ANObject {

    public abstract void render(ANRenderer r, ANRenderHints hint);
    
    private ANPickHandler m_ph = null;
    protected void setPickHandler(ANPickHandler ph) {
        m_ph = ph;
    }
    
    public ANPickHandler getPickHandler() {
        return m_ph;
    }
}
