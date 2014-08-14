/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package atlantis.gui;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.canvas.AWindowGLView;
import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.ButtonGroup;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JSeparator;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;

/**
 *
 * @author Adam
 */
public class AOpenGLControl extends JMenu implements MenuListener, ActionListener {

    private ButtonGroup m_bg = new ButtonGroup();
    private JRadioButtonMenuItem m_fsaa0 = new JRadioButtonMenuItem("No AA");
    private JRadioButtonMenuItem m_fsaa2 = new JRadioButtonMenuItem("2x FSAA");
    private JRadioButtonMenuItem m_fsaa4 = new JRadioButtonMenuItem("4x FSAA");
    private JRadioButtonMenuItem m_fsaa6 = new JRadioButtonMenuItem("6x FSAA");
    private JRadioButtonMenuItem m_fsaa8 = new JRadioButtonMenuItem("8x FSAA");
    private JCheckBoxMenuItem m_blendborder = new JCheckBoxMenuItem("Blend Border");
    private boolean m_updating = false;

    public AOpenGLControl() {
        super("OpenGL");

        m_bg.add(m_fsaa0);
        m_bg.add(m_fsaa2);
        m_bg.add(m_fsaa4);
        m_bg.add(m_fsaa6);
        m_bg.add(m_fsaa8);

        add(m_fsaa0);
        add(m_fsaa2);
        add(m_fsaa4);
        add(m_fsaa6);
        add(m_fsaa8);
        add(new JSeparator());
        add(m_blendborder);

        m_fsaa0.addActionListener(this);
        m_fsaa2.addActionListener(this);
        m_fsaa4.addActionListener(this);
        m_fsaa6.addActionListener(this);
        m_fsaa8.addActionListener(this);

        m_blendborder.addActionListener(this);

        addMenuListener(this);
    }

    public void menuSelected(MenuEvent e) {
        m_updating = true;

        int maxFSAA = AWindowGLView.getMaxFSAA();

        m_fsaa2.setEnabled(false);
        m_fsaa4.setEnabled(false);
        m_fsaa6.setEnabled(false);
        m_fsaa8.setEnabled(false);

        if (maxFSAA >= 2) {
            m_fsaa2.setEnabled(true);

            if (maxFSAA >= 4) {
                m_fsaa4.setEnabled(true);

                if (maxFSAA >= 6) {
                    m_fsaa6.setEnabled(true);

                    if (maxFSAA >= 8) {
                        m_fsaa8.setEnabled(true);
                    }
                }
            }
        }

        int currentFSAA = AWindowGLView.getCurrentFSAA();

        switch (currentFSAA) {
            case 0:
                m_fsaa0.setSelected(true);
                break;
            case 2:
                m_fsaa2.setSelected(true);
                break;
            case 4:
                m_fsaa4.setSelected(true);
                break;
            case 6:
                m_fsaa6.setSelected(true);
                break;
            case 8:
                m_fsaa8.setSelected(true);
                break;
            default:
                break;
        }

        m_updating = false;
    }

    public void menuDeselected(MenuEvent e) {
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void menuCanceled(MenuEvent e) {
    //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void actionPerformed(ActionEvent e) {
        if (m_updating) {
            return;
        }

        JMenuItem src = (JMenuItem) (e.getSource());

        if (src == m_fsaa0) {
            AWindowGLView.setCurrentFSAA(0);
        } else if (src == m_fsaa2) {
            AWindowGLView.setCurrentFSAA(2);
        } else if (src == m_fsaa4) {
            AWindowGLView.setCurrentFSAA(4);
        } else if (src == m_fsaa6) {
            AWindowGLView.setCurrentFSAA(6);
        } else if (src == m_fsaa8) {
            AWindowGLView.setCurrentFSAA(8);
        } else if (src == m_blendborder) {
            int oldr = AWindow.BORDER_BACKGROUND_COLOR.getRed();
            int oldg = AWindow.BORDER_BACKGROUND_COLOR.getGreen();
            int oldb = AWindow.BORDER_BACKGROUND_COLOR.getBlue();
            int oldselr = AWindow.BORDER_SELECTED_BACKGROUND_COLOR.getRed();
            int oldselg = AWindow.BORDER_SELECTED_BACKGROUND_COLOR.getGreen();
            int oldselb = AWindow.BORDER_SELECTED_BACKGROUND_COLOR.getBlue();
            if (m_blendborder.getState()) {
                AWindow.BORDER_BACKGROUND_COLOR = new Color(oldr, oldg, oldb, 127);
                AWindow.BORDER_SELECTED_BACKGROUND_COLOR = new Color(
                        oldselr, oldselg, oldselb, 127);
            } else {
                AWindow.BORDER_BACKGROUND_COLOR = new Color(oldr, oldg, oldb, 255);
                AWindow.BORDER_SELECTED_BACKGROUND_COLOR = new Color(
                        oldselr, oldselg, oldselb, 255);
            }
            ACanvas.getCanvas().repaintAllFromScratch();
        }
    }
}
