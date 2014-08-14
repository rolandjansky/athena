package atlantis.nge;

import atlantis.canvas.AWindow;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;

/**
 *
 * @author Adam Davison
 */
public class ANPickDrawer implements GLEventListener {

    private AWindow m_window;
    private int m_w;
    private int m_h;
    private int m_x;
    private int m_y;
    
    private ANPickResult m_pr;
    private boolean m_pickdone;
    
    private ANPickListener m_pl;
    
    public ANPickDrawer(AWindow window, int w, int h, int x, int y, ANPickListener pl) {
        m_window = window;
        m_w = w;
        m_h = h;
        m_x = x;
        m_y = y;
        m_pickdone = false;
        m_pl = pl;
    }
    
    public void doPick() {
        ANManager.getManager().requestBufferDraw(m_w, m_h, this);
    }
    
    public void init(GLAutoDrawable arg0) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void display(GLAutoDrawable arg0) {
        m_pr = ANManager.getManager().getFrameManager(m_window).pick(arg0, m_w, m_h, m_x, m_y);
        m_pickdone = true;
        
        if (m_pl != null) {
            m_pl.pickCompleted(m_pr);
        }
    }

    public void reshape(GLAutoDrawable arg0, int arg1, int arg2, int arg3, int arg4) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

}
