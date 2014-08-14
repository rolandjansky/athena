package atlantis.nge;

import com.sun.opengl.util.Screenshot;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.Random;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLDrawableFactory;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLPbuffer;
import javax.swing.JComponent;
import javax.swing.JFrame;

/**
 * The ubiquitous test window class...
 * 
 * @author Adam Davison
 */
public class ANTestWindow implements GLEventListener {

    private Random m_r = new Random();
    
    private int m_h = 400;
    private int m_w = 400;
    
    private JFrame m_f;
    private BufferedImage m_img;
    
    private ANFrameManager m_fm;
    
    private GLPbuffer m_context;
    
    public ANTestWindow() {
        m_f = new JFrame("Happy Time!");
        m_f.setBounds(100, 100, m_w, m_h);
        
        TestComponent tc = new TestComponent(this);
        
        m_f.add(tc);
        
        GLCapabilities pbcap = new GLCapabilities();
        pbcap.setSampleBuffers(true);
        pbcap.setNumSamples(4);
        //pbcap.setPbufferRenderToTexture(true);
        //pbcap.setDoubleBuffered(false);
        m_context = GLDrawableFactory.getFactory().createGLPbuffer(pbcap, null, m_w, m_h, null);
        m_context.addGLEventListener(this);
        
        m_fm = new ANFrameManager(null);
        
        m_f.setVisible(true);
        
        //(new Thread(new TimingThread(tc))).start();
    }

    public void causeRepaint(Graphics g) {
        m_context.display();
        g.drawImage(m_img, 0, 0, null);
    }
    
    public void init(GLAutoDrawable arg0) {
    }

    public void display(GLAutoDrawable arg0) {
        //m_fm.display(arg0);
        m_img = Screenshot.readToBufferedImage(m_w, m_h);
    }

    public void reshape(GLAutoDrawable arg0, int arg1, int arg2, int arg3, int arg4) {
    }

    public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
}

class TestComponent extends JComponent {
    private ANTestWindow m_parent;

    public TestComponent(ANTestWindow parent) {
        m_parent = parent;
    }
    
    @Override
    protected void paintComponent(Graphics g) {
        m_parent.causeRepaint(g);
    }
    
}

class TimingThread implements Runnable {
    private JComponent m_parent;
    
    private long m_sum;
    private int m_count;
    
    public TimingThread(JComponent parent) {
        m_parent = parent;
    }

    public void run() {
        m_sum = 0;
        m_count = 0;
        try {
            Thread.sleep(5000);
        } catch (Exception e) {}
        for (int i = 0; i < 100; i++) {
            long before = System.nanoTime();
            m_parent.paintImmediately(0, 0, m_parent.getWidth(), m_parent.getHeight());
            long after = System.nanoTime();
            m_sum += after - before;
            m_count++;
        }
        System.out.println("Mean time: " + m_sum/m_count + "ns");
    }
}