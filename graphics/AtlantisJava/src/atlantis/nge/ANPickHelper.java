package atlantis.nge;

import atlantis.nge.object.ANObject;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import javax.media.opengl.GL;

/**
 *
 * @author Adam Davison
 */
public class ANPickHelper {
    
    private ArrayList<ANObject> m_all = new ArrayList<ANObject>();
    private GL m_gl;
    
    public ANPickHelper(GL gl) {
        m_gl = gl;
    }
    
    public ANPickHandler getPickHandler(int id) {
        System.out.println(m_all.get(id));
        return m_all.get(id).getPickHandler();
    }
            
    public int getIndex(ANObject p) {
        int id = m_all.indexOf(p);
        
        if (id == -1) {
            m_all.add(p);
            id = m_all.indexOf(p);
        }
        
        return id;
    }
    
    // Worry about it being fast much later...
    public void startPick(ANObject p, int data) {
        startPick(getIndex(p), data);
    }
    
    public void startPick(int pickid, int data) {
        m_gl.glPushName(pickid);
        m_gl.glPushName(data);
    }
    
    public void endPick() {
        m_gl.glPopName();
        m_gl.glPopName();
    }
}
