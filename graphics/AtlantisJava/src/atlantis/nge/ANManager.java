package atlantis.nge;

import atlantis.canvas.AWindow;
import atlantis.utils.ALogger;
import java.util.Hashtable;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLDrawableFactory;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLPbuffer;

/**
 * This class keeps handy resources for you...
 * 
 * @author Adam Davison
 */
public class ANManager implements GLEventListener {

    private static ALogger logger = ALogger.getLogger(ANManager.class);
    private static ANManager manager = new ANManager();

    public static ANManager getManager() {
        return manager;
    }
    private Hashtable<AWindow, ANFrameManager> m_fms = null;
    private GLPbuffer m_buf = null;
    private GLEventListener m_gll;

    public ANManager() {
        m_fms = new Hashtable<AWindow, ANFrameManager>();
    }

    public ANFrameManager getFrameManager(AWindow w) {
        if (w == null) {
            return null;
        }
        ANFrameManager fm = m_fms.get(w);
        if (fm == null) {
            fm = new ANFrameManager(w);
            m_fms.put(w, fm);
        }
        return fm;
    }

    public static int getNextPower2(int v) {
        int p = 1;
        for (int i = 0; i < 16; i++) {
            p *= 2;
            if (p >= v) {
                return p;
            }
        }

        logger.warn("Returning non power of 2 buffer size!");
        return v;
    }

    public void requestBufferDraw(int w, int h, GLEventListener gll) {

        if (m_buf != null && m_buf.getWidth() >= w && m_buf.getHeight() >= h) {
            //System.out.println("BUFFER OK");
            // Do nothing
        } else {
            //System.out.println("BUFFER CREATE/REPLACE");
            if (m_buf != null) {
                m_buf.destroy();
            }

            GLCapabilities pbcap = new GLCapabilities();
            pbcap.setHardwareAccelerated(true);
            pbcap.setSampleBuffers(true);
            pbcap.setNumSamples(4);
            //pbcap.setPbufferRenderToTexture(true);
            //pbcap.setDoubleBuffered(false);

            //logger.info("w: " + m_w + " 2: " + getNextPower2(m_w));

            m_buf = GLDrawableFactory.getFactory().createGLPbuffer(pbcap, null,
                    getNextPower2(w), getNextPower2(h), null);

            if (m_buf == null) {
                logger.error("Got null when trying to create pbuffer!");
                return;
            }
            
            m_buf.addGLEventListener(this);

            logger.info("Created pbuffer w: " + m_buf.getWidth() + " h: " + m_buf.getHeight());
            
            if (m_buf.getChosenGLCapabilities() == null) {
                logger.warn("Your implementation returns null for pbuffer capabilities...");
            } else {
                logger.info("Pbuffer hardware accelleration: " + m_buf.getChosenGLCapabilities().getHardwareAccelerated());
            }
        }

        // Set up events to go back to the projection
        m_gll = gll;
        
        //System.out.println("CALLING DISPLAY");
        m_buf.display();
    }
    
    public void init(GLAutoDrawable arg0) {
        m_gll.init(arg0);
    }

    public void display(GLAutoDrawable arg0) {
        //System.out.println("HIT DISPLAY CALLBACK");
        m_gll.display(arg0);
    }

    public void reshape(GLAutoDrawable arg0, int arg1, int arg2, int arg3, int arg4) {
        m_gll.reshape(arg0, arg1, arg2, arg3, arg4);
    }

    public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
        m_gll.displayChanged(arg0, arg1, arg2);
    }
}
