package atlantis.nge.object.geometry;

import atlantis.nge.object.*;
import atlantis.nge.*;
import atlantis.geometry.ABoxDetector;
import java.util.ArrayList;
import javax.media.opengl.GL;

/**
 * Object that draws geometry of the muon system
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANObjectGeomMuon extends ANObjectVL {

    private float[][] m_v;
    private int[] m_pt;
    private int[] m_color;
    
    public ANObjectGeomMuon() {
        setPickHandler(new ANPickHandlerSimple("Muon Spectrometer"));
    }
    
    public ANObjectGeomMuon(ArrayList<ABoxDetector> m_boxes) {
        setPickHandler(new ANPickHandlerSimple("Muon Spectrometer")); 
        
        m_pt = new int[2*m_boxes.size()];
        m_v = new float[2*m_boxes.size()][];
        m_color = new int[2*m_boxes.size()];
        
        for (int i = 0; i < m_boxes.size(); i++) {
            ABoxDetector b = m_boxes.get(i);
            m_pt[2*i] = GL.GL_TRIANGLE_STRIP;
            m_v[2*i] = getBoxFaces((float)b.getZMin(), (float)b.getZMax(),
                    (float)b.getRMin(), (float)b.getRMax(), (float)b.getPhi(),
                    (float)b.getSize());
            m_color[2*i]=b.getFillColor();
            m_pt[2*i+1] = GL.GL_LINES;
            m_v[2*i+1] = getBoxEdges((float)b.getZMin(), (float)b.getZMax(),
                    (float)b.getRMin(), (float)b.getRMax(), (float)b.getPhi(),
                    (float)b.getSize());
            m_color[2*i+1]=DRAW_LINE;
        }
    }
    
        // This code answers the question, how do you wrap a strip of 14 triangles
    // to form a cube?
    private float[] getBoxFaces(float zmin, float zmax, float rmin, float rmax, float phi, float w) {
        float[] v = new float[3*14];
        
        float sinPhi = (float)Math.sin(phi);
        float cosPhi = (float)Math.cos(phi);
        
        float xmi = rmin*cosPhi;
        float ymi = rmin*sinPhi;
        float xmo = rmax*cosPhi;
        float ymo = rmax*sinPhi;
        float dx = -sinPhi*w/2.0f;
        float dy = cosPhi*w/2.0f;
        
        v[0] = xmi + dx;
        v[1] = ymi + dy;
        v[2] = zmax;
        
        v[3] = xmi - dx;
        v[4] = ymi - dy;
        v[5] = zmax;
        
        v[6] = xmo + dx;
        v[7] = ymo + dy;
        v[8] = zmax;
        
        v[9] = xmo - dx;
        v[10] = ymo - dy;
        v[11] = zmax;
        
        v[12] = v[9];
        v[13] = v[10];
        v[14] = zmin;
        
        v[15] = v[3];
        v[16] = v[4];
        v[17] = v[5];
        
        v[18] = v[3];
        v[19] = v[4];
        v[20] = zmin;
        
        v[21] = v[0];
        v[22] = v[1];
        v[23] = zmin;
        
        v[24] = v[12];
        v[25] = v[13];
        v[26] = v[14];
        
        v[27] = v[6];
        v[28] = v[7];
        v[29] = zmin;
        
        v[30] = v[6];
        v[31] = v[7];
        v[32] = v[8];
        
        v[33] = v[21];
        v[34] = v[22];
        v[35] = v[23];
        
        v[36] = v[0];
        v[37] = v[1];
        v[38] = v[2];
        
        v[39] = v[3];
        v[40] = v[4];
        v[41] = v[5];
        
        return v;
    }
    
    private float[] getBoxEdges(float zmin, float zmax, float rmin, float rmax, float phi, float w) {
        float[] v = new float[3*12*2]; // 3d*12edges*2ends
        
        float sinPhi = (float)Math.sin(phi);
        float cosPhi = (float)Math.cos(phi);
        
        float xmi = rmin*cosPhi;
        float ymi = rmin*sinPhi;
        float xmo = rmax*cosPhi;
        float ymo = rmax*sinPhi;
        float dx = -sinPhi*w/2.0f;
        float dy = cosPhi*w/2.0f;
        
        v[0] = xmi + dx;
        v[1] = ymi + dy;
        v[2] = zmin;
        
        v[3] = xmi - dx;
        v[4] = ymi - dy;
        v[5] = zmin;
        
        v[6] = v[3];
        v[7] = v[4];
        v[8] = v[5];
        
        v[9] = xmo - dx;
        v[10] = ymo - dy;
        v[11] = zmin;
        
        v[12] = v[9];
        v[13] = v[10];
        v[14] = v[11];
        
        v[15] = xmo + dx;
        v[16] = ymo + dy;
        v[17] = zmin;
        
        v[18] = v[15];
        v[19] = v[16];
        v[20] = v[17];
        
        v[21] = v[0];
        v[22] = v[1];
        v[23] = v[2];
        
        for (int i = 24; i < 48; i++) {
            if (i % 3 == 2) {
                v[i] = zmax;
            } else {
                v[i] = v[i - 24];
            }
            
        }
        
        v[48] = v[0];
        v[49] = v[1];
        v[50] = v[2];
        
        v[51] = v[0];
        v[52] = v[1];
        v[53] = zmax;
        
        v[54] = v[3];
        v[55] = v[4];
        v[56] = v[5];
        
        v[57] = v[3];
        v[58] = v[4];
        v[59] = zmax;
        
        v[60] = v[9];
        v[61] = v[10];
        v[62] = v[11];
        
        v[63] = v[9];
        v[64] = v[10];
        v[65] = zmax;
        
        v[66] = v[15];
        v[67] = v[16];
        v[68] = v[17];
        
        v[69] = v[15];
        v[70] = v[16];
        v[71] = zmax;
        
        return v;
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
            return ANColor.getMapColor(m_color[i], 1.0f);

    }
    
}