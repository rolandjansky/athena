package atlantis.nge;

/**
 * This class provides an interface for drawing into a context of some kind
 * essentially it's a drawing API, much like java.awt.Graphics but rather
 * tuned towards drawing 3-d objects via OpenGL
 * 
 * @author Adam Davison
 */
public abstract class ANRenderer {
    
    public abstract void begin(int type);
    public abstract void end();
    
    public abstract void vertex3f(float x, float y, float z);
    public abstract void color3f(float r, float g, float b);
    public abstract void color4f(float r, float g, float b, float a);
    public abstract void lineWidth(float w);
    
    public abstract void drawVertices(float[] vert);
    
}
