package atlantis.nge;

import atlantis.canvas.AWindow;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import javax.media.opengl.DebugGL;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.glu.GLU;

/**
 * Decides which projection should be onscreen and in what state.
 * Handles animation, performance and probably all sorts of other things.
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANFrameManager {
    
    // Targ is what we're aiming to display but proj is what we're really
    // displaying, could be the same but could also be some transition into
    // targ
    
    private ANProjection m_targ;
    private ANProjection m_proj;
    
    private ANAnimationManager m_am;
    
    public ANFrameManager(AWindow w) {
        // Default, if not told anything else construct 3d projection
        m_proj = null;
        m_am = new ANAnimationManager(w);
        
    }

    public ANPickResult pick(GLAutoDrawable drawable, int w, int h, int x, int y) {
        
        if (m_proj == null) { return null; }
        
        long before = System.nanoTime();
        
        GL gl = drawable.getGL();
        gl.glViewport(0, 0, w, h);
        
        GLU glu = new GLU();
        int maxsize = 100000;
        
        ByteBuffer bb = ByteBuffer.allocateDirect(4*maxsize);
        bb.order(ByteOrder.nativeOrder());
        // FIXME: Reuse this object for performance
        IntBuffer ib = bb.asIntBuffer();
        gl.glSelectBuffer(maxsize, ib);
        gl.glRenderMode(GL.GL_SELECT);
        gl.glMatrixMode(GL.GL_PROJECTION);
        int[] viewport = new int[4];
        gl.glLoadIdentity();
        gl.glGetIntegerv(GL.GL_VIEWPORT, viewport, 0);
        y = h - y;
        glu.gluPickMatrix(x - 1, y - 1, 3, 3, viewport, 0);
        
        ANRenderHints rh = new ANRenderHints();
        rh.setPick(true);
        rh.setPickHelper(new ANPickHelper(gl));
        
        //gl.glPushName(0);
        m_proj.display(drawable, w, h, rh);
        //gl.glPopName();
        
        int hits = gl.glRenderMode(GL.GL_RENDER);
    
        System.out.println("HITS: " + hits);
        ANSelectionList sl = new ANSelectionList(ib);
        //System.out.println("FRONT: " + sl.getFrontID());
        int[] front = sl.getFrontID();
        
        ANPickResult ret = null;
        
        if (front != null) {
            int id = front[0];
            System.out.println(id);
            int data = front[1];
            System.out.println(data);
            ANPickHandler ph = rh.getPickHelper().getPickHandler(id);
            System.out.println(ph);
            if (ph != null) {
                ret = ph.getHitInfo(data);
            }
        }
        
        long after = System.nanoTime();
        System.out.println("Pick took: " + (after - before) / 1000);
        
        System.out.println(ret);
        return ret;
    }
    
    public void display(GLAutoDrawable drawable, int w, int h) {
        System.out.println("Asked to display!");
        
        if (m_proj == null) { return; }
        
        long before = System.nanoTime();

        GL gl = new DebugGL(drawable.getGL());
        gl.glViewport(0, 0, w, h);

        m_am.animate();
        
        m_proj.display(drawable, w, h, new ANRenderHints());

        long after = System.nanoTime();
        System.out.println("Frame took: " + (after - before) / 1000);
    }
    
    public double timeToWait() {
        return m_am.timeToWait();
    }

    public ANProjection getTargetProjection() {
        return m_targ;
    }
    
    public ANProjection getCurrentProjection() {
        return m_proj;
    }

    public void setCurrentProjection(ANProjection proj) {
        m_proj = proj;
    }

    public void scheduleTransition(ANProjection p) {
        //(new Exception()).printStackTrace();
        
        m_targ = p;
        
        if (m_proj == null || m_proj == p) { m_proj = p; return; }
        
        //System.out.println(m_proj);
        //System.out.println(p);
        
        if (m_proj instanceof ANLinearProjection &&
                p instanceof ANLinearProjection) {

            //System.out.println("HERE!");
            
            ANLinearTransition trans = new ANLinearTransition(
                    (ANLinearProjection) m_proj,
                    (ANLinearProjection) p);

            setCurrentProjection(trans);

            double length = 1.0;

            m_am.scheduleAnimation(trans.getTransformTimeVar(), 0.0, length, 0.0, 1.0);
            m_am.scheduleAnimation(trans.getFadeTimeVar(), 0.0, length, 0.0, 1.0);
            m_am.scheduleProjectionChange(this, p, length);

        } else {
            m_proj = p;
        }
    }

    public void checkProjectionYX() {
        //System.out.println("checkProjectionYX: m_proj: " + m_proj);
        if (!(m_targ instanceof ANProjectionYX)) {
            //System.out.println("RECREATING");
            scheduleTransition(new ANProjectionYX());
        }
    }

    public void checkProjection3D() {
        if (!(m_targ instanceof ANProjection3D)) {
            scheduleTransition(new ANProjection3D());
        }
    }

    public ANProjectionPhysics checkProjectionPhysics() {
        if (!(m_targ instanceof ANProjectionPhysics)) {
            ANProjectionPhysics pp = new ANProjectionPhysics();
            scheduleTransition(pp);
            return pp;
        } else {
            return (ANProjectionPhysics)(m_targ);
        }
    }
    
    public ANAnimationManager getAnimationManager() {
        return m_am;
    }
}
