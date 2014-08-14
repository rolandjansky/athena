package atlantis.canvas;

import atlantis.globals.AGlobals;

import java.awt.Graphics;
import javax.swing.JPanel;

/**
 * 
 * This class provides an Java2D/Swing context. When the parent AWindow repaints
 * this is propagated down to this view component, which then calls back to
 * the parent AWindow requesting everything be copied in.
 * 
 * @author Adam Davison
 */
public class AWindowSwingView extends JPanel {
    
    private AWindow m_parent;

    public AWindowSwingView(AWindow parent) {
        setLayout(null);
        m_parent = parent;
    }

    @Override
    protected void paintComponent(Graphics g) {
        //System.out.println(getX() + ":" + getY() + ":" + getWidth() + ":" + getHeight());
        g.setClip(0, 0, getWidth(), getHeight());
        //g.setColor(Color.RED);
        //g.fillRect(10, 10, getWidth() - 10, getHeight() - 10);
        if (AGlobals.isAtlantisHeadless()) {
            // Don't need to paint to a backing buffer ever in headless
            // since we're already painting offscreen
            m_parent.paintWindow(g, false);
        } else {
            m_parent.paintWindow(g, true);
        }
    }
    
    @Override
    protected void printComponent(Graphics g) {
        //System.out.println(getX() + ":" + getY() + ":" + getWidth() + ":" + getHeight());
        g.setClip(0, 0, getWidth(), getHeight());
        //g.setColor(Color.RED);
        //g.fillRect(10, 10, getWidth() - 10, getHeight() - 10);
        m_parent.printComponent(g);
    }
    
    @Override
    public void setBounds(int x, int y, int width, int height) {
        super.setBounds(x, y, width, height);
        //System.out.println("RESIZE " + x + ":" + y + ":" + width + ":" + height);
    }
    
    
    
}
