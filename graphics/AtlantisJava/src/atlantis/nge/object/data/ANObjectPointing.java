package atlantis.nge.object.data;

import atlantis.data.ACalorimeterData;
import atlantis.data.ATILEData;
import atlantis.geometry.AEndcapCalorimeterDetector;
import atlantis.nge.ANColor;
import atlantis.nge.object.ANObjectVL;
import atlantis.utils.AMath;
import javax.media.opengl.GL;

/**
 *
 * @author Adam Davison
 */
public class ANObjectPointing extends ANObjectVL {

    private float[][] m_v;
    private int[] m_pt;
    private ANColor[] m_c;

    public ANObjectPointing(ACalorimeterData ald, int dl[], int dllen, boolean outline) {
        m_pt = new int[dllen];
        m_v = new float[dllen][];
        m_c = new ANColor[dllen];

        double[] area = new double[dl.length];
        for (int i = 0; i < dllen; i++) {
            area[i] = 1.0;
        }

        ald.calibrateLocalDensity(dl, area);
        double[] scale = ald.getScaleFactors(dl, area);

        if (outline) {
            for (int j = 0; j < dllen; j++) {
                int i = dl[j];
                m_pt[j] = GL.GL_LINES;
                m_c[j] = ANColor.getMapColor(ald.getColor(i));
                m_c[j].mult(0.5f);
                m_v[j] = getVert(ald, i, (float)scale[j], true);
                //m_v[j] = getVert(ald, i, 1.0f, true);
            }
        } else {
            
            for (int j = 0; j < dllen; j++) {
                int i = dl[j];
                m_pt[j] = GL.GL_TRIANGLE_STRIP;
                m_c[j] = ANColor.getMapColor(ald.getColor(i));
                m_v[j] = getVert(ald, i, (float)scale[j], false);
            }
        }

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
        return m_c[i];
    }

    private float[] getVert(ACalorimeterData ald, int index, float scale, boolean outline) {

        // Possibly should apply scale to theta not eta?
        float etamin = (float) (ald.getEta(index) - 0.5f*scale*ald.getdEta(index));
        float etamax = (float) (ald.getEta(index) + 0.5f*scale*ald.getdEta(index));

        float thetamin = (float) (2.0f * Math.atan(Math.exp(-etamin)));
        float thetamax = (float) (2.0f * Math.atan(Math.exp(-etamax)));

        float phimin = (float) (ald.getPhi(index) - 0.5f*scale*ald.getdPhi(index));
        float phimax = (float) (ald.getPhi(index) + 0.5f*scale*ald.getdPhi(index));

        float rmin = (float) ald.getDetector(index).getRMin();
        float rmax = (float) ald.getDetector(index).getRMax();
        float dr = 0.5f*(rmax - rmin);
        rmin += (1.0f - scale)*dr;
        rmax -= (1.0f - scale)*dr;

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

        int[] wind = null;
        if (!outline) {
            int[] inwind = {6, 5, 7, 4, 0, 5, 1, 2, 0, 3, 7, 2, 6, 5};
            wind = inwind;
        } else {
            int[] outwind = {0, 3, 0, 1, 0, 4, 2, 1, 2, 3, 2, 6,
                            7, 4, 7, 6, 7, 3, 5, 6, 5, 4, 5, 1};
            wind = outwind;
        }
        float[] vert = new float[3 * wind.length];
        for (int i = 0; i < wind.length; i++) {
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
}
