package atlantis.nge;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.glu.GLU;

/**
 *
 * @author Adam Davison
 */
public class ANRendererBasic extends ANRenderer {

    private GLAutoDrawable m_drawable;
    private GL m_gl;

    public ANRendererBasic(GLAutoDrawable drawable) {
        m_drawable = drawable;
        m_gl = drawable.getGL();
    }

    @Override
    public void begin(int type) {
        m_gl.glBegin(type);
    }

    @Override
    public void end() {
        m_gl.glEnd();
    }

    @Override
    public void drawVertices(float[] vert) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    @Override
    public void vertex3f(float x, float y, float z) {
    //    if (m_btesting) {
    //        doBoundTest(x, y, z);
    //    }
        
        m_gl.glVertex3f(x, y, z);
    }

    @Override
    public void color3f(float r, float g, float b) {
        m_gl.glColor3f(r, g, b);
    }

    @Override
    public void lineWidth(float w) {
        m_gl.glLineWidth(w);
    }

    @Override
    public void color4f(float r, float g, float b, float a) {
        m_gl.glColor4f(r, g, b, a);
    }
    
/*    private float[] m_tmat = new float[16];
    private float m_minx;
    private float m_maxx;
    private float m_miny;
    private float m_maxy;
    
    private boolean m_btesting = false;
    
    public void beginBoundsTest() {
        // This function assumes no new transformations take place
        // So bear that in mind...
        
        m_gl.glGetFloatv(GL.GL_MODELVIEW_MATRIX, m_tmat, 0);
        m_minx = Float.MAX_VALUE;
        m_maxx = Float.MIN_VALUE;
        m_miny = Float.MAX_VALUE;
        m_maxy = Float.MIN_VALUE;
        
        m_btesting = true;
    }
    
    public void stopBoundsTest() {
        m_btesting = false;
    }
    
    private void doBoundTest(float x, float y, float z) {

    }*/
}
