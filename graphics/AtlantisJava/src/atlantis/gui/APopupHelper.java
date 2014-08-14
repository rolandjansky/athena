/*
 * APopupHelper.java
 *
 * Created on 24 October 2007, 09:12
 *
 * @author Adam Davison
 */

package atlantis.gui;

import javax.swing.JPopupMenu;
import javax.swing.JMenuItem;
import java.awt.Point;
import java.awt.Component;

/**
 *
 * @author Adam Davison
 */

public class APopupHelper {
        
    public static void showPopupMenu(Point p, Component window, JMenuItem[] items) {
        JPopupMenu pop = getPopupMenu(items);
        
        if (pop == null) {
            return;
        }
        
        pop.show(window, (int)p.getX(), (int)p.getY());
    }
    
    public static JPopupMenu getPopupMenu(JMenuItem[] items) {
        
        if (items == null) {
            return null;
        }
        
        JPopupMenu jp = new JPopupMenu();
        
        for (int i = 0; i < items.length; i++) {
            jp.add(items[i]);
        }
        
        return jp;
    }
    
}
