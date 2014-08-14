package atlantis.nge;

import atlantis.event.AData;
import atlantis.data.AJetData;
import atlantis.nge.object.ANObjectTestAxes;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

import atlantis.parameters.APar;
import atlantis.utils.A3Vector;
import atlantis.utils.AMath;
import javax.media.opengl.glu.GLU;

/**
 * The plain 3d projection, simple for debugging
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANProjectionPhysics extends ANLinearProjection {

    private float m_focus_phi;
    private float m_focus_eta;
    private ANAnimVar m_phi;
    private ANAnimVar m_elev;
    private float m_distance;
    private float m_tx,  m_ty,  m_tz;
    private AData m_data;
    private int m_data_index;

    public ANProjectionPhysics() {

        m_focus_phi = 0;
        m_focus_eta = 0;
        m_phi = new ANAnimVar(-45.0);
        m_elev = new ANAnimVar(20.0);
        m_distance = 2000.0f;
        m_tx = 0.0f;
        m_ty = 0.0f;
        m_tz = 0.0f;


    // create the object generator here
    }

    @Override
    public void applyLinearTransform(GLAutoDrawable drawable, int w, int h) {
        GL gl = drawable.getGL();

        //float w = 1000.0f;
        //float h = (w * drawable.getHeight()) / drawable.getWidth();

        //gl.glFrustum(-w, w, -h, h, 1000.0f, 10000.0f);
        GLU glu = new GLU();
        glu.gluPerspective(7.0f, w / h, 200.0f, 10000.0f);

        /*gl.glTranslatef(0.0f, 0.0f, -2000.0f);
        gl.glRotatef(m_elev, 1.0f, 0.0f, 0.0f);
        gl.glRotatef(m_phi, 0.0f, 1.0f, 0.0f);
        gl.glRotatef(m_focus_phi, 0.0f, 0.0f, 1.0f);
        gl.glRotatef(-theta, 0.0f, 1.0f, 0.0f);
        gl.glTranslatef(0.0f, 0.0f, 500.0f);
        gl.glRotatef(theta, 0.0f, 1.0f, 0.0f);
        gl.glRotatef(-m_focus_phi, 0.0f, 0.0f, 1.0f);*/
        // Remember, these are in reverse so essentially:
        // 0. GL default, start at 0,0,-1 looking at 0,0,0
        // 1. Spin around y axis to give phi
        // 2. Spin around x axis to give elevation
        // 3. Pull back to give distance
        gl.glTranslatef(0.0f, 0.0f, -m_distance);
        gl.glRotatef((float) m_elev.getValue(), 1.0f, 0.0f, 0.0f);
        gl.glRotatef((float) m_phi.getValue(), 0.0f, 1.0f, 0.0f);
        gl.glTranslatef(-m_tx, -m_ty, -m_tz);


    }

    @Override
    public void drawScene(GLAutoDrawable drawable, ANRenderHints hint) {
        ANRenderer r = new ANRendererBasic(drawable);

        GL gl = drawable.getGL();
        gl.glColor3f(1.0f, 0.0f, 0.0f);
        gl.glPointSize(2.0f);
        gl.glBegin(GL.GL_POINTS);
        gl.glVertex3f(m_tx, m_ty, m_tz);
        gl.glEnd();

        if (APar.instance().get("NPhysics", "Axis").getStatus()) {
            ANObjectTestAxes ota = new ANObjectTestAxes();
            ota.render(r, hint);
        }

        //ANObjectGenerator.getGenerator()
        //        .getGeometryMuon(m_focus_phi, m_focus_eta, 1.0f).render(r, pick);
        //.getGeometryMuon().render(r, pick);

        ANObjectGenerator.getGenerator().getDataCaloForJet(m_data_index, "LAr").render(r, hint);
        ANObjectGenerator.getGenerator().getDataCaloForJet(m_data_index, "TILE").render(r, hint);
        ANObjectGenerator.getGenerator().getDataCaloForJet(m_data_index, "HEC").render(r, hint);
        //ANObjectGenerator.getGenerator().getDataCaloForJet(m_data_index, "FCAL").render(r, pick);
        ANObjectGenerator.getGenerator().getInDetTracksForJet(m_data_index).render(r, hint);

    //createGeometry(m_focus_phi, m_focus_eta, 1.0f);
    //m_ogm.render(r, pick);
        /*m_ogc.render(r);
    m_ogetp.render(r);
    m_ogecp.render(r);
    m_ogtsp.render(r);
    m_ogetm.render(r);
    m_ogecm.render(r);
    m_ogtsm.render(r);*/

    //createData(m_focus_phi, m_focus_eta, 2.0f);
    //m_jet.render(r, pick);
    //m_lar.render(r, pick);

    }

    public void dPhi(float phi) {
        m_phi.setValue(m_phi.getValue() + phi);
    }

    public void dElev(float elev) {
        m_elev.setValue(m_elev.getValue() + elev);
        if (m_elev.getValue() < -90.0) {
            m_elev.setValue(-90.0);
        }
        if (m_elev.getValue() > 90.0) {
            m_elev.setValue(90.0);
        }
    }

    public void lookAt(AJetData jd, int jet_index) {

        m_data = jd;
        m_data_index = jet_index;

        A3Vector centre = ANTerribleHacks.getCellCentreLArForJet(jet_index);
        m_tx = (float) centre.x;
        m_ty = (float) centre.y;
        m_tz = (float) centre.z;

        float phi = jd.getPhi(jet_index);
        float eta = jd.getEta(jet_index);

        m_focus_phi = phi;
        m_focus_eta = eta;

        // m_elev is Spin around y axis to give phi
        // m_phi is Spin around x axis to give elevation   
        float theta = (float) (2.0f * Math.atan(Math.exp(eta)));
        theta = (float) Math.toDegrees(theta);
        phi = (float) Math.toDegrees(phi);

        //produce coords to try to look perpendicular to jet
        //needs improving
        if (eta < 0) {
            eta = -1;
        } else {
            eta = 1;
        }
        float x = (float) (m_distance * Math.cos(phi) * Math.sin(theta));
        float z = (float) (m_distance * Math.cos(theta));

        float rot = (float) Math.toDegrees(Math.atan(z / x));

        if (theta < 90) {
            m_elev.setValue(eta * -theta);
            m_phi.setValue(rot);
        } else {
            m_elev.setValue(eta * (180. - theta));
            m_phi.setValue(rot - 45.);
        }
        //actually dont need to set melev
        m_elev.setValue(0);

    }
    
    public ANAnimVar getElevVar() { return m_elev; }
    public ANAnimVar getPhiVar() { return m_phi; }
}
