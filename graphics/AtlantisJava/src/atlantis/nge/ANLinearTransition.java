package atlantis.nge;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

/**
 *
 * @author Adam Davison
 */
public class ANLinearTransition extends ANLinearProjection {

    private ANLinearProjection m_a;
    private ANLinearProjection m_b;
    private ANAnimVar m_vtttime = new ANAnimVar();
    private ANAnimVar m_fadetime = new ANAnimVar();
    
    public ANLinearTransition(ANLinearProjection a, ANLinearProjection b) {
        m_a = a;
        m_b = b;
    }

    public ANAnimVar getTransformTimeVar() { return m_vtttime; }
    public ANAnimVar getFadeTimeVar() { return m_fadetime; }
    
    @Override
    public void applyLinearTransform(GLAutoDrawable drawable, int w, int h) {
        GL gl = drawable.getGL();
        
        double vtttime = m_vtttime.getValue();
        
        if (vtttime < 0.0) { vtttime = 0.0; }
        
        if (vtttime <= 0.0f) {
            m_a.applyLinearTransform(drawable, w, h);
        } else if (vtttime >= 1.0f) {
            m_b.applyLinearTransform(drawable, w, h);
        } else {

            float[] ma = new float[16];
            float[] mb = new float[16];

            // TODO: Make this whole thing better
            // Maybe this is ok. If getFloatv is problematic we may have trouble
            // but generally this allows us to transform between moving projections!
            gl.glMatrixMode(GL.GL_PROJECTION);
            gl.glPushMatrix();
            gl.glLoadIdentity();
            m_a.applyLinearTransform(drawable, w, h);
            gl.glGetFloatv(GL.GL_PROJECTION_MATRIX, ma, 0);
            gl.glLoadIdentity();
            m_b.applyLinearTransform(drawable, w, h);
            gl.glGetFloatv(GL.GL_PROJECTION_MATRIX, mb, 0);
            gl.glPopMatrix();

            float[] matrix = new float[16];

            // Scaling the matrix linearly means that x/w moves in a highly
            // non-linear way, especially where large scale changes in w are
            // involved, try to compensate this by using a highly non-linear
            // time coordinate

            // Final solution involves inverting the formula for the transformation
            // to calculate what time coordinate should be applied to ensure
            // linear motion of a test eye coordinate between it's clip
            // coordinates under transformation by ma and mb
            // Should document the maths at some point for sanity...

            // Operating on normalised time here, length 1
            double[] zerosums = new double[4];
            double[] gradsums = new double[4];

            for (int y = 0; y < 4; y++) {
                zerosums[y] = 0.0f;
                gradsums[y] = 0.0f;
                for (int x = 0; x < 4; x++) {
                    zerosums[y] += ma[(4 * x) + y];
                    gradsums[y] += mb[(4 * x) + y] - ma[(4 * x) + y];
                }
            }

            double w0 = ma[3] + ma[7] + ma[11] + ma[15];
            double wT = mb[3] + mb[7] + mb[11] + mb[15];

            double[] timeest = new double[3];
            for (int i = 0; i < 3; i++) {
                // Target value of coord/w
                double c0w = (ma[0 + i] + ma[4 + i] + ma[8 + i] + ma[12 + i]) / w0;
                double cTw = (mb[0 + i] + mb[4 + i] + mb[8 + i] + mb[12 + i]) / wT;
                // Beta is the target value for this component, this could be
                // a non-linear function for smoother transitions
                // i.e. something with 0 first derivative at times 0 and 1
                double beta = (c0w + ((cTw - c0w) * vtttime));
                timeest[i] = (zerosums[i] - zerosums[3] * beta) / (gradsums[3] * beta - gradsums[i]);
            }

            // They seem to tend to scale similarly at the moment but this
            // may fall apart for some really wierd transformation matrix...
            // ... just take the mean for now and look out for problems
            double meantime = (timeest[0] + timeest[1] + timeest[2]) / 3.0;

            for (int i = 0; i < 16; i++) {
                matrix[i] = (float) (ma[i] + ((mb[i] - ma[i]) * meantime));
            }

            gl.glMultMatrixf(matrix, 0);
        }
    }

    @Override
    public void drawScene(GLAutoDrawable drawable, ANRenderHints hint) {        
        GL gl = drawable.getGL();
        
        
        
        if (m_fadetime.getValue() > 0.0) {
            m_a.drawScene(drawable, hint);
        
            gl.glMatrixMode(GL.GL_PROJECTION);
            gl.glPushMatrix();
            gl.glLoadIdentity();
            gl.glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
            ANColor bgc = ANColor.getBGColor();
            gl.glColor4f(bgc.r, bgc.g, bgc.b, (float)(m_fadetime.getValue()));
            gl.glDisable(GL.GL_DEPTH_TEST);
            gl.glBegin(GL.GL_QUADS);
            gl.glVertex3f(-1.0f, -1.0f, 0.0f);
            gl.glVertex3f(1.0f, -1.0f, 0.0f);
            gl.glVertex3f(1.0f, 1.0f, 0.0f);
            gl.glVertex3f(-1.0f, 1.0f, 0.0f);
            gl.glEnd();
            // FIXME: Don't always want this back on here potentially
            gl.glEnable(GL.GL_DEPTH_TEST);
            gl.glPopMatrix();
            gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
        }
            
        m_b.drawScene(drawable, hint);
        
    }
}
