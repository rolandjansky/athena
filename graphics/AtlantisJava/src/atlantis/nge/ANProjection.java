package atlantis.nge;

import javax.media.opengl.GLAutoDrawable;

/**
 * So this class represents a projection, which in this model is the class
 * that decides what objects to draw and how to draw them
 * 
 * It is controlled by the ANFrameManager which tells it what to do. 
 * 
 * It shouldn't touch the Atlantis core at all. It gets ANObjects from an
 * ANObjectGenerator which is can then render in any order it chooses into a
 * renderer it can configure in any way it likes.
 * 
 * @author Adam Davison
 */
public abstract class ANProjection {

    public abstract void display(GLAutoDrawable drawable, int w, int h, ANRenderHints hint);
}
