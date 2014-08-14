package atlantis.nge;

/**
 * A variable which is animatable, pretty much a wrapper around a double...
 * 
 * @author Adam Davison
 */
public class ANAnimVar {
    
    private double m_val;
    
    public ANAnimVar(double val) {
        m_val = val;
    }
    
    public ANAnimVar() {
        m_val = 0.0;
    }
    
    public double getValue() {
        return m_val;
    }
    
    public void setValue(double val) {
        m_val = val;
    }
}
