package atlantis.gui;

import java.awt.Frame;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JOptionPane;

import atlantis.config.ADefaultValues;
import atlantis.utils.ALogger;

/**
 *
 * Atlantis application closing confirmation dialog.
 * All Atlantis closing actions can be added here and this
 * class shall be used for closing the application.
 *
 */
public class AClosingConfirmationDialog extends WindowAdapter
{
	private static ALogger logger = ALogger.getLogger(AClosingConfirmationDialog.class);
	
    private Frame adaptee;

    
    public AClosingConfirmationDialog(Frame adaptee)
    {
        this.adaptee = adaptee;

    } // AClosingConfirmationDialog() ---------------------------------------



    public void windowClosing(WindowEvent e)
    {
        exitApp();

    } // windowClosing() ----------------------------------------------------



    public void exitApp()
    {
        
        String msg = "Do you really want to quit Atlantis?";
        int i = JOptionPane.showConfirmDialog(adaptee, msg, "Exit Atlantis",
                JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null);
        if(i == JOptionPane.YES_OPTION)
        {
        	logger.debug("Shutting down, going to save runtime values...");        	
            ADefaultValues.saveRuntimeValues();
            logger.warn("Shutting down, calling System.exit(0)");
            System.exit(0);
        }
        else
        {
            return;
        }

    } // exitApp() ----------------------------------------------------------



} // class AClosingConfirmationDialog =======================================
