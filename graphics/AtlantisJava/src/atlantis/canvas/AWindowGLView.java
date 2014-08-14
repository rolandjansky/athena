package atlantis.canvas;

import atlantis.interactions.AInteractionsManager;
import atlantis.utils.ALogger;
import java.awt.BorderLayout;
import java.awt.BufferCapabilities;
import java.awt.Canvas;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.GraphicsEnvironment;
import java.awt.event.MouseListener;
import java.lang.ref.WeakReference;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import javax.media.opengl.AWTGraphicsDevice;
import javax.media.opengl.DefaultGLCapabilitiesChooser;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLCapabilitiesChooser;
import javax.media.opengl.GLContext;
import javax.media.opengl.GLDrawableFactory;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLJPanel;
import javax.media.opengl.glu.GLU;
import javax.swing.JPanel;
import javax.swing.border.Border;

/**
 * This class provides an OpenGL context. When the parent AWindow repaints
 * this is propagated down to this view component, which then calls back to
 * the parent AWindow requesting everything be copied in.
 * 
 * @author Adam Davison
 */
public class AWindowGLView extends JPanel implements GLEventListener {
    
    private static ALogger logger = ALogger.getLogger(AWindowGLView.class);
    
    private GLCanvas m_panel;
    //private JPanel m_children;
    private long m_start = 0;
    private int m_count = 0;
    private AWindow m_parent;
    private Graphics m_g = null;

    private static GLContext m_ocontext;
    
    // Keep track of all initialized OpenGL contexts without forcing them
    // to stay in memory when we aren't using them any more...
    // This is a debugging thing
    private static ArrayList<WeakReference> m_all = new ArrayList<WeakReference>();
    
    private static int m_maxFSAA = -1;
    private static int m_FSAA = 0;
    
    public static int getCurrentFSAA() {
        return m_FSAA;
    }
    
    public static void setCurrentFSAA(int FSAA) {
        logger.info("Changing anti-aliasing to " + FSAA + "x");
        m_FSAA = FSAA;
        for (int i = 0; i < m_all.size(); i++) {
            AWindowGLView v = (AWindowGLView)(m_all.get(i).get());
            if (v != null) { 
                v.restartDisplay();
            }
        }
    }
    
    public static int getMaxFSAA() {
        return m_maxFSAA;
    }
    
    public void restartDisplay() {
        System.out.println("RESTART DISPLAY!");
        remove(m_panel);
        m_panel = null;
        startDisplay();
        validate();
        m_panel.setBounds(0, 0, getWidth(), getHeight());
    }
    
    private void startDisplay() {
        GLCapabilities cap = new GLCapabilities();
        
        if (m_FSAA > 0) {
            cap.setSampleBuffers(true);
            cap.setNumSamples(m_FSAA);
            logger.info("Requesting " + m_FSAA + "x");
        }
        
        cap.setDoubleBuffered(true);
        
        if (m_ocontext != null) {
            m_panel = new GLCanvas(cap, null, m_ocontext, null);
        } else {
            m_panel = new GLCanvas(cap);
        }
        
        m_ocontext = m_panel.getContext();
        
        m_panel.addGLEventListener(this);
        
        //m_FSAA = m_panel.getChosenGLCapabilities().getNumSamples();
        
        //m_children = new JPanel();
        
        add(m_panel, BorderLayout.CENTER);
        
    }
    
    public AWindowGLView(AWindow parent) {
        m_parent = parent;
        setLayout(null);
        
        startDisplay();
        //m_panel.add(m_but);
        //add(m_but, BorderLayout.NORTH); 
        
        
        m_all.add(new WeakReference(this));

    }
    
    private void checkListeners() {
        AInteractionsManager lt = m_parent.getInteractionManager();
        MouseListener[] lr = m_panel.getMouseListeners();
        boolean found = false;
        for (int i = 0; i < lr.length; i++) {
            if (lr[i] == lt) {
                found = true;
            }
        }
        
        if (!found) {
            m_panel.addMouseListener(lt);
            m_panel.addMouseMotionListener(lt);
        }
    }
    
    public void init(GLAutoDrawable drawable) {
        
        System.out.println("INIT CALLED!");
        System.out.println(getWidth() + ":" + getHeight());
        //(new Exception()).printStackTrace();
        
        checkListeners();
        
        //drawable.setGL(new TraceGL(drawable.getGL(), System.err));
        GL gl = drawable.getGL();
        logger.debug("INIT GL IS: " + gl.getClass().getName());
        
        int[] buffer = new int[2];
        gl.glGetIntegerv(GL.GL_MAX_SAMPLES_EXT, buffer, 0);
        m_maxFSAA = buffer[0];
        logger.debug("Graphics card supports max anti-alias of: " + buffer[0] + "x");
        
        BufferCapabilities bc = m_panel.getGraphicsConfiguration().getBufferCapabilities();
        boolean frontacc = bc.getFrontBufferCapabilities().isAccelerated();
        boolean backacc = bc.getBackBufferCapabilities().isAccelerated();
        logger.info("OpenGL Acceleration Status: Front - " + frontacc + " and Back - " + backacc);

        // All polygons in Atlantis are uniformly coloured, this speeds things up in theory at least...
        gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gl.glShadeModel(GL.GL_FLAT);

        // Old style antialiasing like this is apparently not supported by 
        // modern graphics cards although we may want to enable it on older
        // hardware where people do not have FSAA but still want AA
        /*gl.glHint(gl.GL_POLYGON_SMOOTH_HINT, gl.GL_NICEST);
        gl.glEnable(gl.GL_POLYGON_SMOOTH);
        gl.glHint(gl.GL_LINE_SMOOTH_HINT, gl.GL_NICEST);
        gl.glEnable(gl.GL_LINE_SMOOTH);*/

        // We do our own Z-ordering
        gl.glDisable(gl.GL_DEPTH_TEST);
        
        // Enable transparency, even if nothing in Atlantis is actually
        // transparent yet...
        gl.glEnable(gl.GL_BLEND);
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA);

    }

    private static int countContexts() {
        int count = 0;
        for (int i = 0; i < m_all.size(); i++) {
            if (m_all.get(i).get() != null) {
                count++;
            }
            // Could throw away dead weak refs here...
        }
        return count;
    }
    
    public void display(GLAutoDrawable d) {
        
        /*if (m_parent.isValid()) {
            System.out.println("VALID PARENT - RETURNING");
            return;
        }*/
        // Perhaps we could not do this and turn off buffer swaps?
        // Currently we're probably doing some unnecessary repaints when
        // you change window
        m_parent.invalidateQuietly();
        
        GL gl = d.getGL();
        //System.out.println("DISPLAY " + this);
        //System.out.println("ACTIVE: " + countContexts());
        gl.glMatrixMode(GL.GL_MODELVIEW);
        gl.glLoadIdentity();

        // Can we avoid this? We probably fill with bg colour later anyway...
        gl.glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        gl.glClear(gl.GL_COLOR_BUFFER_BIT);

        if (m_parent != null) {
            AGLGraphics glg = new AGLGraphics(d);
            m_parent.paintWindow(glg, false);
            super.paintBorder(glg);
            /*for (int i = 0; i < m_panel.getComponentCount(); i++) {
                m_panel.getComponent(i).paint(glg);
            }*/
        }

        gl.glFlush();

        m_count++;
        long tnow = System.currentTimeMillis();
        if (tnow - 1000 > m_start) {
            logger.info("OpenGL FPS: " + m_count);
            m_count = 0;
            m_start = tnow;
        }
    }

    /*@Override
    public Graphics getGraphics() {
    if (m_g == null) {
    System.out.println("PAINT OUT OF VALID");
    return new ADummyGraphics2D();
    } else {
    return m_g;
    }
    }*/
    public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
        //System.out.println("GLRESIZE: " + x + ":" + y + ":" + width + ":" + height);
        GL gl = drawable.getGL();
        GLU glu = new GLU();

        if (height <= 0) // avoid a divide by zero error!
        {
            height = 1;
        }
        final float h = (float) width / (float) height;
        gl.glViewport(0, 0, width, height);
        gl.glMatrixMode(GL.GL_PROJECTION);
        gl.glLoadIdentity();
        gl.glOrtho(x, x + width, y + height, y, -100.0, 100.0);
        //gl.glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);

        gl.glMatrixMode(GL.GL_MODELVIEW);
        gl.glLoadIdentity();

        m_count = 0;
        m_start = System.currentTimeMillis();
    }

    // Should implement this for multi-screen people some time...
    public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void setBounds(int x, int y, int width, int height) {
        //Border b = getBorder();
        //setBorder(null);
        super.setBounds(x, y, width, height);
        m_panel.setBounds(0, 0, getWidth(), getHeight());
        //setBorder(b);
        //System.out.println("RESIZE " + x + ":" + y + ":" + width + ":" + height);
    }
    
    // Need to paint our own border to make sure it gets drawn in GL
    public void paintBorder(Graphics g) {
        
    }
    
    // Need to make sure our heavyweight child is invalidated
    public void repaint() {
        super.repaint();
        if (m_panel != null) {
            m_panel.repaint();
        }
    }
    
    public Component getRealPanel() {
        return m_panel;
    }
}

class OurGLCapChooser implements GLCapabilitiesChooser {
    
    private int m_maxAA;
    private boolean m_canAA;
    
    public OurGLCapChooser() {
    }
    
    public int chooseCapabilities(GLCapabilities req, GLCapabilities[] all, int recommended) {
        System.out.println("CALLED!");
        m_canAA = false;
        m_maxAA = 0;
        
        for (int i = 0; i < all.length; i++) {
            System.out.println(all[i]);
            if (all[i].getSampleBuffers()) {
                m_canAA = true;
                if (all[i].getNumSamples() > m_maxAA) {
                    m_maxAA = all[i].getNumSamples();
                }
            }
        }
        
        return (new DefaultGLCapabilitiesChooser()).chooseCapabilities(req, all, recommended);
    }
    
    public boolean getCanAA() {
        return m_canAA;
    }
    
    public int getMaxAA() {
        return m_maxAA;
    }
}