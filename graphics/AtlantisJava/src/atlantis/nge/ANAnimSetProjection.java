package atlantis.nge;

/**
 *
 * @author Adam Davison
 */
public class ANAnimSetProjection extends ANAnimSegment {
    private ANFrameManager m_fm;
    private ANProjection m_p;
    private double m_when;

    public ANAnimSetProjection(ANFrameManager fm, ANProjection p, double when) {
        m_fm = fm;
        m_p = p;
        m_when = when;
    }
    
    @Override
    public void evaluate(double now) {
        //System.out.println("Evaluate in SetProjection: " + now);
        if (now > m_when) {
            //System.out.println("CHANGE!");
            m_fm.setCurrentProjection(m_p);
        }
    }

    @Override
    public double getStart() {
        return m_when;
    }

    @Override
    public double getEnd() {
        return m_when;
    }
    
    
}
