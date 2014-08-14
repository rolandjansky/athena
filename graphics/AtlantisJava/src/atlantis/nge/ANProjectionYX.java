package atlantis.nge;

import atlantis.nge.object.ANObjectTestAxes;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

import atlantis.parameters.APar;

/**
 *
 * @author Adam Davison, Mark Stockton
 */
public class ANProjectionYX extends ANLinearProjection {


    @Override
    public void applyLinearTransform(GLAutoDrawable drawable, int w, int h) {
        GL gl = drawable.getGL();

        gl.glMatrixMode(GL.GL_PROJECTION);
        float scale = ((float) drawable.getHeight()) / ((float) drawable.getWidth());
        gl.glOrtho(-1500.0f, 1500.0f, -1500.0f * scale, 1500.0f * scale, -100000.0f, 100000.0f);
    }

    @Override
    public void drawScene(GLAutoDrawable drawable, ANRenderHints hint) {
        ANRenderer r = new ANRendererBasic(drawable);
        if(APar.instance().get("NYX","Axis").getStatus()){
            ANObjectTestAxes ota = new ANObjectTestAxes();
            ota.render(r, hint);
        }

        if (m_ogm == null) {
            //m_ogm = new ANObjectGeomMuon();
            createGeometry();
        }

        m_ogm.render(r, hint);
        
        m_ogc.render(r, hint);
        
        createData();
        m_jet.render(r, hint);
        m_lar.render(r, hint);
    }
}
