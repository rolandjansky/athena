package atlantis;

import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLCapabilities;

/**
 * Provides utility method to check if OpenGL is available.
 * @author Adam
 */
public class AOpenGLTools {

    public static boolean isOpenGLAvailable() {
        try {
            GLCapabilities cap = new GLCapabilities();
            GLCanvas canv = new GLCanvas(cap);
        } catch (Error e) {
            // Expect some kind of hard exception if the class can't be loaded
            return false;
        }
        return true;
    }
}
