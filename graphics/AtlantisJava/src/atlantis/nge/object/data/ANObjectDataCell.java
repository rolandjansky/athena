package atlantis.nge.object.data;

import atlantis.nge.object.*;
import atlantis.nge.*;
import atlantis.data.ACalorimeterData;
import atlantis.geometry.AEndcapCalorimeterDetector;
import atlantis.utils.AMath;

import javax.media.opengl.GL;

/**
 * Object that draws cells
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANObjectDataCell extends ANObjectVL {

    private float[][] m_v;
    private int[] m_pt;
    private int[] m_color;

    public ANObjectDataCell() {
    }

    public ANObjectDataCell(ACalorimeterData ald) {

        m_pt = new int[ald.getNumDraw()];
        m_v = new float[ald.getNumDraw()][];
        m_color = new int[ald.getNumDraw()];

        for (int j = 0; j < ald.getNumDraw(); j++) {
            int i = ald.getDrawIndex(j);

            m_pt[j] = GL.GL_TRIANGLE_STRIP;
            m_color[j] = ald.getColor(i);
            m_v[j] = getVert(ald, i);

        }
    }

    public ANObjectDataCell(ACalorimeterData ald, int dl[]) {
        
        m_pt = new int[dl.length];
        m_v = new float[dl.length][];
        m_color = new int[dl.length];

        for (int j = 0; j < dl.length; j++) {
            int i = dl[j];

            m_pt[j] = GL.GL_TRIANGLE_STRIP;
            m_color[j] = ald.getColor(i);
            m_v[j] = getVert(ald, i);

        }
    }
    
    // This logic should be in ANObjectGenerator
    public ANObjectDataCell(ACalorimeterData ald, float phi, float eta, float dr) {

        int num = 0;
        for (int j = 0; j < ald.getNumDraw(); j++) {
            int i = ald.getDrawIndex(j);
            if (AMath.deltaR(phi, eta, (float) ald.getPhi(i), (float) ald.getEta(i)) < dr) {
                num++;
            }
        }

        m_pt = new int[num];
        m_v = new float[num][];
        m_color = new int[num];
        num = 0;
        for (int j = 0; j < ald.getNumDraw(); j++) {
            int i = ald.getDrawIndex(j);
            if (AMath.deltaR(phi, eta, (float) ald.getPhi(i), (float) ald.getEta(i)) > dr) {
                continue;
            }
            m_pt[num] = GL.GL_TRIANGLE_STRIP;
            m_color[num] = ald.getColor(i);
            m_v[num] = getVert(ald, i);
            num++;

        }
    }

    private float[] getVert(ACalorimeterData ald, int index) {

        float etamin = (float) (ald.getEta(index) - ald.getdEta(index));
        float etamax = (float) (ald.getEta(index) + ald.getdEta(index));

        float thetamin = (float) (2.0f * Math.atan(Math.exp(-etamin)));
        float thetamax = (float) (2.0f * Math.atan(Math.exp(-etamax)));

        float phimin = (float) (ald.getPhi(index) - ald.getdPhi(index));
        float phimax = (float) (ald.getPhi(index) + ald.getdPhi(index));

        float rmin = (float) ald.getDetector(index).getRMin();
        float rmax = (float) ald.getDetector(index).getRMax();

        // TODO: Push this up to the getZ??? functions in this detector
        // class.
        if (ald.getDetector(index) instanceof AEndcapCalorimeterDetector) {
            //correction taken from AEndcapCalorimeterDetector
            String name = ald.getDetector(index).getName();
            float zmin = (float) (ald.getDetector(index).getZMin());
            //This shouldn't be 0, but just to be on the safe side.
            int side = ald.getSide(index);
            if (side != 0) {
                side /= Math.abs(side);
            }
            zmin *= side;
            if (name.indexOf("Inner") >= 0 || name.indexOf("Outer") >= 0 || name.indexOf("Presampler") >= 0) {
                rmax = (float) (zmin / Math.sinh(etamin));
            }
            rmin = (float) (zmin / Math.sinh(etamax));
        }

        float[][] v = new float[8][3];

        fillVert(v[0], rmin, thetamin, phimin);
        fillVert(v[1], rmin, thetamax, phimin);
        fillVert(v[2], rmax, thetamax, phimin);
        fillVert(v[3], rmax, thetamin, phimin);
        fillVert(v[4], rmin, thetamin, phimax);
        fillVert(v[5], rmin, thetamax, phimax);
        fillVert(v[6], rmax, thetamax, phimax);
        fillVert(v[7], rmax, thetamin, phimax);

        float[] vert = new float[3 * 14];
        int[] wind = {6, 5, 7, 4, 0, 5, 1, 2, 0, 3, 7, 2, 6, 5};
        for (int i = 0; i < 14; i++) {
            copyVert(v[wind[i]], vert, i);
        }

        return vert;
    }

    private void copyVert(float[] v, float[] t, int ti) {
        t[(ti * 3) + 0] = v[0];
        t[(ti * 3) + 1] = v[1];
        t[(ti * 3) + 2] = v[2];

    }

    private void fillVert(float[] v, float r, float theta, float phi) {

        v[0] = (float) (r * Math.cos(phi));
        v[1] = (float) (r * Math.sin(phi));
        v[2] = (float) (r / Math.tan(theta));
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

        if (m_color[i] == DRAW_LINE) {
            return new ANColor(0.0f, 0.0f, 0.0f, 1.0f);
        } else {
            return ANColor.getMapColor(m_color[i]);
        }
    }
}