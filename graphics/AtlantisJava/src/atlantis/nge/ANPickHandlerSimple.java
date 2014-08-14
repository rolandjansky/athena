package atlantis.nge;

/**
 *
 * @author Adam Davison
 */
public class ANPickHandlerSimple implements ANPickHandler {

    private String m_name;

    public ANPickHandlerSimple(String name) {
        m_name = name;
    }
    
    public ANPickResult getHitInfo(int data) {
        return new ANPickResult(m_name);
    }

}
