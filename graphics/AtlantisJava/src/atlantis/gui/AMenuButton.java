package atlantis.gui;

import java.awt.Dimension;
import java.awt.FontMetrics;
import javax.swing.JMenuItem;

/**
 * AMenuButton is an object that acts like a JMenuItem but looks like a JMenu.
 * This is for the buttons that we add directly to the menu bar. It does not 
 * allow the layout manager to change it size.
 *
 * @author Eric Jansen
 */
public class AMenuButton extends JMenuItem {

    /** Extra space needed to make JMenuItem and JMenu look the same */
    private static int EXTRA_SPACE = 6;
    
    /** Initial preferred size */
    private Dimension size;

    public AMenuButton(String title) {
        super(title);
        setOpaque(false);
        
        FontMetrics fm = getFontMetrics(getFont());        
        size = super.getPreferredSize();
        size.width = getMargin().left + fm.stringWidth(title) 
                + getMargin().right + EXTRA_SPACE;
    }
    
    @Override
    public Dimension getMinimumSize() {
        return size;
    }

    @Override
    public Dimension getMaximumSize() {
        return size;
    }
    
    @Override
    public Dimension getPreferredSize() {
        return size;
    }
}
