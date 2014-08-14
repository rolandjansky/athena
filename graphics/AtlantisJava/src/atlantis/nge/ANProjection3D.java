package atlantis.nge;

import atlantis.nge.object.ANObjectTestAxes;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

import atlantis.parameters.APar;

/**
 * The plain 3d projection, simple for debugging
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANProjection3D extends ANLinearProjection {

    
    private float m_phi;
    private float m_elev;
    private float m_distance;
    private float m_tx,  m_ty,  m_tz;

    public ANProjection3D() {

        m_phi = -45.0f;
        m_elev = 20.0f;
        m_distance = 3000.0f;
        m_tx = 0.0f;
        m_ty = 0.0f;
        m_tz = 0.0f;

    // create the object generator here
    }

    @Override
    public void applyLinearTransform(GLAutoDrawable drawable, int width, int height) {
        GL gl = drawable.getGL();
        
        float w = 1000.0f;
        float h = (w * drawable.getHeight()) / drawable.getWidth();

        gl.glFrustum(-w, w, -h, h, 1000.0f, 10000.0f);

        // Remember, these are in reverse so essentially:
        // 0. GL default, start at 0,0,-1 looking at 0,0,0
        // 1. Spin around y axis to give phi
        // 2. Spin around x axis to give elevation
        // 3. Pull back to give distance
        gl.glTranslatef(0.0f, 0.0f, -m_distance);
        gl.glRotatef(m_elev, 1.0f, 0.0f, 0.0f);
        gl.glRotatef(m_phi, 0.0f, 1.0f, 0.0f);
        gl.glTranslatef(-m_tx, -m_ty, -m_tz);
    }

    @Override
    public void drawScene(GLAutoDrawable drawable, ANRenderHints hint) {
        ANRenderer r = new ANRendererBasic(drawable);
        if(APar.instance().get("N3D","Axis").getStatus()){
            ANObjectTestAxes ota = new ANObjectTestAxes();
            ota.render(r, hint);
        }

        if (m_ogm == null) {
            //m_ogm = new ANObjectGeom();
            createGeometry();
        }

        m_ogm.render(r, hint);
        m_ogc.render(r, hint);
        m_ogetp.render(r, hint);
        m_ogecp.render(r, hint);
        m_ogtsp.render(r, hint);
        m_ogetm.render(r, hint);
        m_ogecm.render(r, hint);
        m_ogtsm.render(r, hint);
        
        createData();
        m_jet.render(r, hint);
        
    }
    
    public void dPhi(float phi) {
        m_phi += phi;
    }
    
    public void dElev(float elev) {
        m_elev += elev;
        if (m_elev < -90.0f) { m_elev = -90.0f; }
        if (m_elev > 90.0f) { m_elev = 90.0f; }
    }
}
