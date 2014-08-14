package atlantis.nge;

/**
 *
 * @author Adam Davison
 */
public abstract class ANAnimSegment {

    public abstract double getStart();
    public abstract double getEnd();
    
    public abstract void evaluate(double now);
}
