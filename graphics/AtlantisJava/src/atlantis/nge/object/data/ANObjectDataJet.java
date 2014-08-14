package atlantis.nge.object.data;

import atlantis.nge.object.*;
import atlantis.nge.*;
import atlantis.data.AJetData;
import atlantis.utils.AMath;

import javax.media.opengl.GL;

/**
 * Object that draws geometry of the muon system
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANObjectDataJet extends ANObjectVL {

    private float[][] m_v;
    private int[] m_pt;
    private int[] m_color;
    
    public ANObjectDataJet() {

    }
    
public ANObjectDataJet(AJetData ajd) {
         
        
        m_pt = new int[2*ajd.getNumDraw()];
        m_v = new float[2*ajd.getNumDraw()][];
        m_color = new int[2*ajd.getNumDraw()];
        
        for (int j = 0; j < ajd.getNumDraw(); j++) {
            int i= ajd.getDrawIndex(j);
            float theta = (float) (2.0f * Math.atan(Math.exp(-ajd.getEta(i))));
            
            m_pt[2*j] = GL.GL_TRIANGLE_FAN;
            //m_v[2*j] = getCone(ajd.getET(i), ajd.getPhi(i), theta);
            m_v[2*j] = getCone(1300.0f, ajd.getPhi(i), theta);
            m_color[2*j]=ajd.getColor(i);
            m_pt[2*j+1] = GL.GL_LINES;
            //m_v[2*i+1] = getCore(ajd.getET(i), ajd.getPhi(i), theta);
            m_v[2*j+1] = getCore(1300.0f, ajd.getPhi(i), theta);
            m_color[2*j+1]=DRAW_LINE;
        }
    }
    
    public ANObjectDataJet(AJetData ajd, float phi, float eta, float dr) {
        
        int num=0;
        for (int j = 0; j < ajd.getNumDraw(); j++) {
            int i= ajd.getDrawIndex(j);
            if(AMath.deltaR(phi,eta,(float)ajd.getPhi(i),(float)ajd.getEta(i))<dr){
                num++;
            }
        }
        
        m_pt = new int[2*num];
        m_v = new float[2*num][];
        m_color = new int[2*num];
        num=0;
        for (int j = 0; j < ajd.getNumDraw(); j++) {
            int i= ajd.getDrawIndex(j);
            if(AMath.deltaR(phi,eta,(float)ajd.getPhi(i),(float)ajd.getEta(i))>dr)
                continue;
            float theta = (float) (2.0f * Math.atan(Math.exp(-ajd.getEta(i))));
            
            m_pt[2*num] = GL.GL_TRIANGLE_FAN;
            //m_v[2*num] = getCone(ajd.getET(i), ajd.getPhi(i), theta);
            m_v[2*num] = getCone(1300.0f, ajd.getPhi(i), theta);
            m_color[2*num]=ajd.getColor(i);
            m_pt[2*num+1] = GL.GL_LINES;
            //m_v[2*num+1] = getCore(ajd.getET(i), ajd.getPhi(i), theta);
            m_v[2*num+1] = getCore(1300.0f, ajd.getPhi(i), theta);
            m_color[2*num+1]=DRAW_LINE;
            num++;
        }
    }
    

    
    private float[] getCore(float len, float phi, float theta){

        // Jets are in detector eta so primary vertex set to 0 is sensible for now
        float[] core = {0.0f, 0.0f, 0.0f,
                (float) (len * Math.cos(phi) * Math.sin(theta)),
                (float) (len * Math.sin(phi) * Math.sin(theta)),
                (float) (len * Math.cos(theta))
        };
        return core;
    }
    
    private float[] getCone(float len, float phi, float theta){
        int segs = 8;

        float[] cone = new float[3 * (2 + segs)];
        cone[0] = 0.0f;
        cone[1] = 0.0f;
        cone[2] = 0.0f;

        //temporary jet radius
        float jetr = 100.0f;

        for (int i = 0; i < segs; i++) {
            float lphi = (float) (i * 2.0f * Math.PI / segs);
            float ax = (float) (jetr * Math.cos(lphi));
            float ay = (float) (jetr * Math.sin(lphi));
            float az = len;

            float bx = (float) (ax * Math.cos(phi) * Math.cos(theta) +
                    az * Math.sin(theta) * Math.cos(phi) - ay * Math.sin(phi));
            float by = (float) (ax * Math.cos(theta) * Math.sin(phi) +
                    az * Math.sin(theta) * Math.sin(phi) + ay * Math.cos(phi));
            float bz = (float) (-ax * Math.sin(theta) + az * Math.cos(theta));
            
            cone[i*3+3] = bx;
            cone[i*3+4] = by;
            cone[i*3+5] = bz;
        }
        
        cone[(3*(2+segs)) - 3] = cone[3];
        cone[(3*(2+segs)) - 2] = cone[4];
        cone[(3*(2+segs)) - 1] = cone[5];
        
        return cone;
    }
    
    @Override
    public float[][] getVertices() {
        return m_v;
    }

    @Override
    public int[] getPrimTypes() {
        return m_pt;
    }

    @Override
    public ANColor getColor(int i) {
        
        if(m_color[i]==DRAW_LINE)
            return new ANColor(0.0f, 0.0f, 0.0f, 1.0f);
        else
            return ANColor.getMapColor(m_color[i], 0.5f);

    }
}