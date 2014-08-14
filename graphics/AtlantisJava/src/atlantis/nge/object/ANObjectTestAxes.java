package atlantis.nge.object;

import atlantis.nge.*;
import javax.media.opengl.GL;

/**
 * A simple test object
 * 
 * @author Adam Davison, Mark Stockton
 */
public class ANObjectTestAxes extends ANObject {

    public ANObjectTestAxes() {
        
    }
    
    @Override
    public void render(ANRenderer r, ANRenderHints hint) {
        
        r.lineWidth(2.0f);
        
        // x
        r.color3f(1.0f, 0.0f, 0.0f);
        r.begin(GL.GL_LINES);
        r.vertex3f(0.0f, 0.0f, 0.0f);
        r.vertex3f(1500.0f, 0.0f, 0.0f);
        r.end();
        
        // y
        r.color3f(0.0f, 1.0f, 0.0f);
        r.begin(GL.GL_LINES);
        r.vertex3f(0.0f, 0.0f, 0.0f);
        r.vertex3f(0.0f, 1500.0f, 0.0f);
        r.end();
        
        // z
        r.color3f(0.0f, 0.0f, 1.0f);
        r.begin(GL.GL_LINES);
        r.vertex3f(0.0f, 0.0f, 0.0f);
        r.vertex3f(0.0f, 0.0f, 1500.0f);
        r.end();
    }

}
