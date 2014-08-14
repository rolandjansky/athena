package atlantis.gui;

import atlantis.graphics.AIcon;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.utils.ALogger;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.net.URL;

import javax.help.BadIDException;
import javax.help.HelpSet;
import javax.help.JHelp;
import javax.swing.JFrame;



public class AHelpSystem extends JFrame
{
    private static ALogger logger = ALogger.getLogger(AHelpSystem.class);
    
    private static AHelpSystem instance = null;
    private static String hsPath = "jhelpset.hs";
    private static JHelp helpViewer = null;


    static
    {
        HelpSet hs = getHelpSet(hsPath);
        // HelpBroker hb = hs.createHelpBroker();
        helpViewer = new JHelp(hs);
        helpViewer.setCurrentID("HelpHowTo");
    }


    private AHelpSystem()
    {
        super("Atlantis Online Help");
        addWindowListener(new WindowAdapter()
        {
            public void windowClosing(WindowEvent e)
            {
                dispose();
                instance = null;
            }
        });
        this.getContentPane().add(helpViewer);
        this.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        AIcon.setIconImage(this);
        this.setSize(900, 600);

    } // AHelpSystem() ------------------------------------------------------



    public static AHelpSystem getInstance()
    {
        if(instance == null)
        {
            instance = new AHelpSystem();
        }
        return instance;

    } // getInstance() ------------------------------------------------------



    public void showPage(String pageName)
    {
        try
        {
            helpViewer.setCurrentID(pageName);
            // clear the iconified bit (no matter whether or not it is
            // current iconified)
            Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
            this.setLocation((int)(Math.max(0, (screenSize.getWidth()-this.getWidth())/2)), (int)((screenSize.getHeight()-this.getHeight())/3));
            this.setExtendedState(this.getExtendedState() & ~Frame.ICONIFIED);
            this.setVisible(true);
        }
        catch(BadIDException ex)
        {
            AOutput.alwaysAppend("\nNo help for \"" + pageName + "\", sorry.\n",
                           ALogInterface.BAD_COMMAND);
        }

    } // showPage() ---------------------------------------------------------



    private static HelpSet getHelpSet(String helpSetFile)
    {
        HelpSet hs = null;
        // ClassLoader cl = this.getClass().getClassLoader(); // if not static
        ClassLoader cl = AHelpSystem.class.getClassLoader();
        try
        {
            // helpSetFile must contain just the name of HelpSet file (.hs),
            // .jar file containing the whole help must be in the classpath
            // then the file is successfully found. returns null if helpSetFile
            // contains the whole absolute / relative path
            URL hsURL = HelpSet.findHelpSet(cl, helpSetFile);
            // hs = new HelpSet(null, hsURL);
            hs = new HelpSet(cl, hsURL);
        }
        catch(Exception ee)
        {
            logger.error("AHelpSystem: " + ee.getMessage(), ee);
            logger.error("AHelpSystem: " + helpSetFile + " not found");
        }
        return hs;

    } // getHelpSet() -------------------------------------------------------


} // class AHelpSystem ======================================================
