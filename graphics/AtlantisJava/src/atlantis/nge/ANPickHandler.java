package atlantis.nge;

/**
 *
 * @author Adam Davison
 */
public interface ANPickHandler {

    // Turn your 32 bits of data into a result somehow
    public ANPickResult getHitInfo(int data);
}
