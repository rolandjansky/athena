package atlantis.nge;

import java.nio.IntBuffer;

/**
 *
 * @author Adam Davison
 */
public class ANSelectionList {

    private long[] m_zmax;
    private long[] m_zmin;
    private int[][] m_names;
    private int m_ent;
    
    public ANSelectionList(IntBuffer ib) {
        int s = ib.limit();
        // Shortest possible entry is 4 ints long, could two pass to get
        // memory requirements right but better to just spend a few bytes
        int maxent = s/4;
        m_zmax = new long[maxent];
        m_zmin = new long[maxent];
        m_names = new int[maxent][];
        
        /*for (int i = 0; i < 20; i++) {
            System.out.print(":" + ib.get(i));
        }
        System.out.println();*/
        
        m_ent = 0;
        int pos = 0;
        while((pos = processEntry(ib, pos, m_ent)) != -1) {
            m_ent++;
        }
        //System.out.println("Got " + m_ent + " entries in selection list");
    }

    private int processEntry(IntBuffer ib, int pos, int ent) {
        //System.out.println("Processing at " + pos + " entry " + ent);
        int nn = ib.get(pos);
        //System.out.println("nn: " + nn);
        if (nn == 0) {
            // At the end
            return -1;
        }
        m_zmin[ent] = getUnsigned(ib.get(pos+1));
        m_zmax[ent] = getUnsigned(ib.get(pos+2));
        m_names[ent] = new int[nn];
        for (int i = 0; i < nn; i++) {
            m_names[ent][i] = ib.get(pos+3+i);
            //System.out.println("Name " + i + " is " + m_names[ent][i]);
        }
        pos += 3 + nn;
        return pos;
    }
    
    public long getUnsigned(int a) {
        long b = a;
        if (b < 0) { b -= 2L*(long)Integer.MIN_VALUE; }
        return b;
    }
    
    public int[] getFrontID() {
        int found = -1;
        long max = Long.MIN_VALUE;
        for (int i = 0; i < m_ent; i++) {
            System.out.println(m_zmax[i] + ":" + max);
            if (m_zmax[i] > max) {
                max = m_zmax[i];
                found = i;
            }
        }
        
        System.out.println(found);
        if (found == -1) {
            return null;
        } else {
            return m_names[found];
        }
    }
}
