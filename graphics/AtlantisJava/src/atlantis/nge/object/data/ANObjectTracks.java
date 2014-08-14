package atlantis.nge.object.data;

import atlantis.data.ALArData;
import atlantis.data.ATrackData;
import atlantis.nge.ANColor;
import atlantis.nge.object.ANObjectVL;
import javax.media.opengl.GL;

/**
 *
 * @author Adam Davison
 */
public class ANObjectTracks extends ANObjectVL {

    private float[][] m_v;
    private int[] m_pt;
    private int[] m_color;

    public ANObjectTracks(ATrackData td) {
        float[][] x = td.getX();
        float[][] y = td.getY();
        float[][] z = td.getZ();

        m_pt = new int[x.length];
        m_v = new float[x.length][];
        m_color = new int[x.length];

        for (int i = 0; i < x.length; i++) {
            m_pt[i] = GL.GL_LINE_STRIP;
            m_v[i] = new float[x[i].length * 3];
            m_color[i] = td.getColor(i);
            for (int p = 0; p < x[i].length; p++) {
                m_v[i][p * 3] = x[i][p];
                m_v[i][p * 3 + 1] = y[i][p];
                m_v[i][p * 3 + 2] = z[i][p];
            }
        }
    }

    public ANObjectTracks(ATrackData td, int[] dl) {
        float[][] x = td.getX();
        if (x == null) {
            System.out.println("ARGH!!");
            m_pt = null;
            m_v = null;
            m_color = null;
            return;
        }
        float[][] y = td.getY();
        float[][] z = td.getZ();

        m_pt = new int[dl.length];
        m_v = new float[dl.length][];
        m_color = new int[dl.length];

        for (int i = 0; i < dl.length; i++) {
            m_pt[i] = GL.GL_LINE_STRIP;
            int index = td.getIndexFromId(dl[i]);
            m_v[i] = new float[x[index].length * 3];
            m_color[i] = td.getColor(index);
            for (int p = 0; p < x[index].length; p++) {
                m_v[i][p * 3] = x[index][p];
                m_v[i][p * 3 + 1] = y[index][p];
                m_v[i][p * 3 + 2] = z[index][p];
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

    public ANColor getColor(int i) {
        return ANColor.getMapColor(m_color[i]);
    }
}
