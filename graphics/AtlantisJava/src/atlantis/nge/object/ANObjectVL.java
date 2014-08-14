package atlantis.nge.object;

import atlantis.nge.*;

/**
 * An object which draws from a vertex list
 * 
 * @author Adam Davison
 */
public abstract class ANObjectVL extends ANObject {
    
    public abstract float[][] getVertices();
    
    public abstract int[] getPrimTypes();

    // What the hell is this?
    public final int DRAW_LINE=-9;
    
    public ANColor getColor(int i) {
        return new ANColor(1.0f, 0.0f, 0.0f, 1.0f);
    }

    /**
     * This function is like the software equivalent of just dumping a block
     * of vertex information in GL 3.1...
     * @param r
     * @param hint
     */
    @Override
    public void render(ANRenderer r, ANRenderHints hint) {
        r.lineWidth(1.5f);
        
        int pid = -1;
        ANPickHelper ph = null;
        if (hint.getPick()) {
            ph = hint.getPickHelper();
            pid = ph.getIndex(this);
        }
        
        int[] pt = getPrimTypes();
        float[][] v = getVertices();
        
        if (pt == null) {
            return;
        }
        
        for (int i = 0; i < pt.length; i++) {
            if (hint.getPick()) {
                ph.startPick(pid, i);
            }
            
            ANColor c = getColor(i);
            r.color4f(c.r, c.g, c.b, c.a);
            r.begin(pt[i]);
            for (int j = 0; j < v[i].length; j+=3) {
                r.vertex3f(v[i][j], v[i][j+1], v[i][j+2]);
            }
            r.end();
            
            if (hint.getPick()) {
                ph.endPick();
            }
        }
    }
    
}
