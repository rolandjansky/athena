package atlantis.nge;

/**
 * This class encapsulates all the information you might want to get out of a 
 * pick interaction and back into Atlantis
 * 
 * @author Adam Davison
 */
public class ANPickResult {

    private String m_result;
    
    public ANPickResult(String result) {
        m_result = result;
    }
    
    public String getResult() {
        return m_result;
    }
    
}
