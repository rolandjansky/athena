package atlantis.gui;

import javax.swing.JMenu;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * The Help menu in the GUI.
 */
public class AHelpControl extends JMenu
{
    public AHelpControl()
    {
        super("Help");
        add("Online Help System").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                AHelpSystem.getInstance().showPage("HelpHowTo");
            }
        });
        add("Modifier Keys").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                AMouseHelpDialog.getInstance().setVisible(true);
            }
        });
        add("Current color and collection summary").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                AColorHelpDialog.getInstance().setVisible(true);
            }
        });
        add("About...").addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                AAboutDialog.getInstance().setVisible(true);
            }
        });

    } // AHelpControl() -----------------------------------------------------
}
