package atlantis.projection;

import atlantis.canvas.AWindow;
import atlantis.nge.ANFrameManager;
import atlantis.nge.ANManager;
import atlantis.utils.ALogger;
import com.sun.opengl.util.Screenshot;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;

/**
 * This projection bridges between the traditional canvas and the new graphics
 * engine code. Essentially it looks like an old style projection but when
 * asked to paint it calls the new code and returns an image
 * 
 * @author Adam Davison
 */
public abstract class AProjectionNGE extends AProjection implements GLEventListener {

    private static ALogger logger = ALogger.getLogger(AProjectionNGE.class);
    
    //private GLPbuffer m_buf = null;
    private BufferedImage m_img = null;
    private int m_h = 0;
    private int m_w = 0;
    
    private AWindow m_currentWindow;
    
    public AProjectionNGE() {
    
    }

/*    @Override
    public String getName() {
        return "NGE";
    }

    @Override
    public String getScreenName() {
        return "NGE";
    }*/

    public abstract void configureFrame(ANFrameManager fm);
    
    @Override
    public void paint(AWindow window, Graphics g) {
       
        m_currentWindow = window;
        
        long before = System.nanoTime();
        
        m_w = window.getWidth();
        m_h = window.getHeight();

        //logger.info("Window w: " + m_w + " h: " + m_h);
        
        //System.out.println("TAG A");
        
        configureFrame(ANManager.getManager().getFrameManager(window));
        
        //System.out.println("TAG B");
        
        ANManager.getManager().requestBufferDraw(m_w, m_h, this);

        //System.out.println("TAG C");
        
        // TODO: Check m_img isn't null
        //g.drawImage(m_img, 0, 0, m_w, m_h, null);
        g.drawImage(m_img, 0, 0, m_w, m_h, 0, m_img.getHeight()-m_h, m_w, m_img.getHeight(), null);
        
        //System.out.println("TAG D copyback done");
        
        long after = System.nanoTime();
        //System.out.println("Draw took: " + (after - before)/1000);
        
        //ANManager.getManager().requestReschedule(window);
    }

/*    @Override
    public JMenuItem[] getPopupItems() {
        return null;
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public String getXLabel() {
        return "XLABEL";
    }

    @Override
    public String getYLabel() {
        return "YLABEL";
    }

    @Override
    public String getXUnits() {
        return "";
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public String getYUnits() {
        return "";
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void setScales() {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public boolean processLocalCommand(String name) {
        return false;
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void actionPerformed(ActionEvent e) {
        System.out.println(e.toString());
        //throw new UnsupportedOperationException("Not supported yet.");
    }*/

    public void init(GLAutoDrawable arg0) {
    }

    public void display(GLAutoDrawable arg0) {
        
        //System.out.println("DISPLAY IN NGE");
        ANManager.getManager().getFrameManager(m_currentWindow).display(arg0, m_w, m_h);
        //System.out.println("PAINT DONE");
        m_img = Screenshot.readToBufferedImage(ANManager.getNextPower2(m_w),
                        ANManager.getNextPower2(m_h));
        //System.out.println("COPIED BACK");
    }

    public void reshape(GLAutoDrawable arg0, int arg1, int arg2, int arg3, int arg4) {
    }

    public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
    }
}
