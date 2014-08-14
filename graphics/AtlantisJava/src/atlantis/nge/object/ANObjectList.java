package atlantis.nge.object;

import atlantis.nge.ANRenderHints;
import atlantis.nge.ANRenderer;
import java.util.ArrayList;

/**
 * A composite object, of course these can be nested to make trees of objects
 * 
 * @author Adam Davison
 */
public class ANObjectList extends ANObject {

    private ArrayList<ANObject> m_list;
    
    public ANObjectList() {
        m_list = new ArrayList<ANObject>();
    }
    
    public void addObject(ANObject o) {
        m_list.add(o);
    }
    
    public void render(ANRenderer r, ANRenderHints hint) {
        for (int i = 0; i < m_list.size(); i++) {
            m_list.get(i).render(r, hint);
        }
    }

}
