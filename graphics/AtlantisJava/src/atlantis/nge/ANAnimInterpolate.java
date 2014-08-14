package atlantis.nge;

/**
 *
 * @author Adam Davison
 */
public class ANAnimInterpolate extends ANAnimSegment {

    private ANAnimVar m_var;
    private double m_t1;
    private double m_t2;
    private double m_v1;
    private double m_v2;
    private int m_interp;
    
    public ANAnimInterpolate(ANAnimVar var, double t1, double t2,
            double v1, double v2, int interp) {
        m_var = var;
        m_t1 = t1;
        m_t2 = t2;
        m_v1 = v1;
        m_v2 = v2;
        m_interp = interp;
    }
    
    public void evaluate(double now) {
        if (now < m_t1) { return; }
        switch(m_interp) {
            case (ANAnimationManager.INTERP_LINEAR):
                //System.out.println("now: " + now + " m_t1: " + m_t1 + " m_t2: " + m_t2);
                double t = (now - m_t1) / (m_t2 - m_t1);
                //System.out.println("t: " + t);
                m_var.setValue((t * (m_v2 - m_v1)) + m_v1);
                break;
        }
    }

    @Override
    public double getStart() {
        return m_t1;
    }

    @Override
    public double getEnd() {
        return m_t2;
    }
}
