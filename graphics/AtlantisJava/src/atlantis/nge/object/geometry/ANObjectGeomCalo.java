package atlantis.nge.object.geometry;

import atlantis.nge.object.*;
import atlantis.nge.*;
import atlantis.geometry.ACalorimeterDetector;

import java.util.ArrayList;
import javax.media.opengl.GL;

/**
 * Object that draws geometry of the muon system
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANObjectGeomCalo extends ANObjectVL {

    private float[][] m_v;
    private int[] m_pt;
    
    private int[] m_color;
    
    public ANObjectGeomCalo(ArrayList<ACalorimeterDetector> m_calo, int nv_scale, boolean duplicate) {

        m_pt = new int[7*m_calo.size()];
        m_v = new float[7*m_calo.size()][];
        m_color = new int[7*m_calo.size()]; 

        //all endcaps and extended tile only has one end in geom file and not relicated like others
        for (int j = 0; j < m_calo.size(); j++) {
            int i=7*j;
            ACalorimeterDetector cd = m_calo.get(j);
            m_pt[i]=GL.GL_TRIANGLE_STRIP;
            m_pt[i+1]=GL.GL_TRIANGLE_STRIP;
            m_pt[i+2]=GL.GL_TRIANGLE_STRIP;
            m_pt[i+3]=GL.GL_LINE_LOOP;
            m_pt[i+4]=GL.GL_LINE_LOOP;
            m_pt[i+5]=GL.GL_LINE_LOOP;
            m_pt[i+6]=GL.GL_LINE_LOOP;
            m_color[i]=cd.getFillColor();
            m_color[i+1]=cd.getFillColor();
            m_color[i+2]=cd.getFillColor();
            m_color[i+3]=DRAW_LINE;
            m_color[i+4]=DRAW_LINE;
            m_color[i+5]=DRAW_LINE;
            m_color[i+6]=DRAW_LINE;
            
            float zMin=(float) cd.getZMin();
            float zMax=(float) cd.getZMax();
            if(duplicate){
                zMin*=-1.;
                zMax*=-1.;
            }
            m_v[i] = getEndTris(zMin, (float) cd.getRMin(), (float) cd.getRMax(), cd.getNumPhi() / nv_scale);
            m_v[i+1] = getEndTris(zMax, (float) cd.getRMin(), (float) cd.getRMax(), cd.getNumPhi() / nv_scale);
            m_v[i+2] = getBarrelTris(zMin, zMax, (float) cd.getRMin(), (float) cd.getRMax(), cd.getNumPhi() / nv_scale);
            m_v[i+3] = getEdges(zMin, (float) cd.getRMin(), cd.getNumPhi() / nv_scale, true);
            m_v[i+4] = getEdges(zMax, (float) cd.getRMin(), cd.getNumPhi() / nv_scale, true);
            m_v[i+5] = getEdges(zMin, (float) cd.getRMax(), cd.getNumPhi() / nv_scale, false);
            m_v[i+6] = getEdges(zMax, (float) cd.getRMax(), cd.getNumPhi() / nv_scale, false);
        }
    }
    
   
        private float[] getEdges(float z, float r, int nv, boolean inner) {
            float[] v = new float[3 * nv];

            float dphi = (2 * (float) Math.PI) / nv;
            float hdphi = 0.5f * dphi;
            //if (inner) { hdphi = 0.0f; }

            for (int i = 0; i < nv; i++) {
                int ie = i * 3;
                v[ie] = r * (float) Math.cos(i * dphi + hdphi);
                v[ie + 1] = r * (float) Math.sin(i * dphi + hdphi);
                v[ie + 2] = (float) z;
            }

            return v;
        }
        
        private float[] getBarrelTris(float zmin, float zmax, float rmin, float rmax, int nv) {
            float[] v = new float[3 * (nv * 2 + 2)];

            float dphi = (2 * (float) Math.PI) / nv;
            float hdphi = 0.5f * dphi;

            float r = (float) rmax;

            for (int i = 0; i < nv; i++) {
                int ie = i * 2 * 3;
                int oe = (i * 2 + 1) * 3;
                v[ie] = r * (float) Math.cos(i * dphi + hdphi);
                v[ie + 1] = r * (float) Math.sin(i * dphi + hdphi);
                v[ie + 2] = (float) zmin;
                v[oe] = r * (float) Math.cos(i * dphi + hdphi);
                v[oe + 1] = r * (float) Math.sin(i * dphi + hdphi);
                v[oe + 2] = (float) zmax;
            }

            // Close the strip
            for (int i = 0; i < 6; i++) {
                v[i + v.length - 6] = v[i];
            }

            return v;
        }

        private float[] getEndTris(float z, float rmin, float rmax, int nv) {
            float[] v = new float[3 * (nv * 2 + 2)];

            float dphi = (2 * (float) Math.PI) / nv;
            float hdphi = 0.5f * dphi;

            for (int i = 0; i < nv; i++) {
                int ie = i * 2 * 3;
                int oe = (i * 2 + 1) * 3;
                v[ie] = (float) rmin * (float) Math.cos(i * dphi);
                v[ie + 1] = (float) rmin * (float) Math.sin(i * dphi);
                v[ie + 2] = z;
                v[oe] = (float) rmax * (float) Math.cos(i * dphi + hdphi);
                v[oe + 1] = (float) rmax * (float) Math.sin(i * dphi + hdphi);
                v[oe + 2] = z;
            }

            // Close the strip
            for (int i = 0; i < 6; i++) {
                v[i + v.length - 6] = v[i];
            }

            return v;
        }
    
        
        
    public ANObjectGeomCalo() {

    }

    @Override
    public int[] getPrimTypes()
    {
        return m_pt;
    }

    @Override
    public float[][] getVertices()
    {
        return m_v;
    }
    
    @Override
    public ANColor getColor(int i) {
        
        if(m_color[i]==DRAW_LINE)
            return new ANColor(0.0f, 0.0f, 0.0f, 1.0f);
        else
            return ANColor.getMapColor(m_color[i], 1.0f);
    }
}