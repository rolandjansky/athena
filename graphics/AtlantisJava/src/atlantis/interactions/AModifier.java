/*
 * AModifier.java
 *
 * Created on 16 July 2007, 12:17 by Adam Davison <adamd at hep.ucl.ac.uk>
 */

package atlantis.interactions;

import java.awt.event.KeyEvent;

/**
 * A structure for holding details of an interaction modifier key
 *
 * @author Adam Davison
 */
public class AModifier {
    
    public static AModifier nothing = new AModifier(KeyEvent.VK_UNDEFINED, false, "Default interaction");
    
    private int m_keycode;
    private char m_keychar;
    
    // Won't use mouse button id's from MouseEvent because i have some memory
    // of this not being very platform independant, can fiddle this if we need
    // middle click or something later.
    private boolean m_right;
    
    private String m_desc;
    
    public AModifier(int keycode, boolean right, String desc) {
        m_keycode = keycode;
        m_keychar = (char)keycode;
        m_right = right;
        m_desc = desc;
    }
    
    public int getKeyCode() {
        return m_keycode;
    }
    
    public char getKeyChar() {
        return m_keychar;
    }
    
    public boolean getRightClick() {
        return m_right;
    }
    
    public String toKeyString() {
        String ret = "";
        if (m_keycode != KeyEvent.VK_UNDEFINED) {
            ret = String.valueOf(m_keychar);
            if (m_right) {
                ret = ret.concat(" + ");
            }
        }
        
        if (m_right) {
            ret = ret.concat("Right");
        }

        return ret;
    }
    
    public String toDescString() {
        return m_desc;
    }
    
    // Two interactions are pretty much the same even if they have different
    // descriptions, hence I won't override equals here and cause problems
    // putting these things in containers or anything.
    public boolean sameAs(AModifier acmp) {
        if (acmp == null) { return false; }
        
        if (acmp.getKeyCode() != getKeyCode()) {
            return false;
        }
        
        if (acmp.getRightClick() != getRightClick()) {
            return false;
        }
        
        return true;
    }
}
