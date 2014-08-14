package atlantis.gui;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Font;
import java.awt.Color;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.JPanel;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JTextPane;
import javax.swing.BorderFactory;
import javax.swing.JFrame;

import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.utils.AUtilities;


/**
 *
 * About information dialog (Help -> About)
 *
 * @author Zdenek Maxa, Mark Stockton
 */
public class AAboutDialog extends JFrame
{

    private static JPanel a_panel1 = null;
    private static JPanel a_panel2 = null;
    private static JPanel a_panel3 = null;
    private static JPanel a_panel4 = null;
    private static JPanel a_textPanel = null;
    private static JLabel a_label1 = null;
    private static JTextPane a_label2 = null;
    private static JLabel a_label3 = null;
    private static JTextPane a_label4 = null;
    
    private static JPanel m_panel1 = null;
    private static JPanel m_panel2 = null;
    private static JPanel m_panel3 = null;
    private static JPanel m_panel4 = null;
    private static JPanel m_textPanel = null;
    private static JLabel m_label1 = null;
    private static JTextPane m_label2 = null;
    private static JTextPane m_label3 = null;
    private static JTextPane m_label4 = null;
    
    private static JPanel c_panel1 = null;
    
    private static JPanel mainPanel = null;


    private static JButton closeButton = null;

    private static AAboutDialog instance = null;

    private static final AGlobals globals = AGlobals.instance();


    private AAboutDialog(String title)
    {
        super(title);
        try
        {
            createGUI();
            pack();
        }
        catch(Exception ex)
        {
            ex.printStackTrace();
        }

        AIcon.setIconImage(this);

    } // AAboutDialog() -----------------------------------------------------



    private void createGUI()
    {
        // top panel - atlantis
        a_panel1 = new JPanel();
        a_panel1.setLayout(new GridLayout(1,2));

        // top left text panel (4 lines of text)
        a_panel2 = new JPanel();
        a_label2 = new JTextPane();
        Font font = new Font(a_panel2.getFont().getName(),
                             Font.BOLD,
                             a_panel2.getFont().getSize() + 6);
        a_label2.setFont(font);
        a_label2.setEditable(false);
        a_label2.setBackground(a_panel2.getBackground());
        a_label2.setText(AGlobals.instance().getVersion());
        a_panel2.add(a_label2);

        a_panel3 = new JPanel();
        a_label3 = new JLabel("Event display for ATLAS");
        a_panel3.add(a_label3);

        a_panel4 = new JPanel();
        a_label4 = new JTextPane();
        a_label4.setBackground(a_panel4.getBackground());
        a_label4.setForeground(Color.blue);
        a_label4.setText("http://www.cern.ch/atlantis" + "\n" +
                       "hn-atlas-AtlantisDisplay@cern.ch");
        a_label4.setEditable(false);
        a_panel4.add(a_label4);

        a_textPanel = new JPanel();
        a_textPanel.setLayout(new GridLayout(3,1));
        a_textPanel.add(a_panel2);
        a_textPanel.add(a_panel3);
        a_textPanel.add(a_panel4);

        a_panel1.add(a_textPanel);

        String FILE_SEPAR = System.getProperty("file.separator");
        String homeDirectory = AGlobals.instance().getHomeDirectory();
        
        // top right atlantis image
        String path = homeDirectory + "img" + FILE_SEPAR +
                      "atlantis_log.gif";
        a_label1 = new JLabel(AUtilities.getFileAsImageIcon(path));
        a_panel1.add(a_label1);

        // middle panel - minerva
        m_panel1 = new JPanel();
        m_panel1.setLayout(new GridLayout(1,2));

        // middle left minerva image
        path = homeDirectory + "img" + FILE_SEPAR +
                      "minerva_log.gif";
        m_label1 = new JLabel(AUtilities.getFileAsImageIcon(path));
        m_panel1.add(m_label1);        

        // middle right text panel 
        m_panel2 = new JPanel();
        m_label2 = new JTextPane();
        m_label2.setFont(font);
        m_label2.setEditable(false);
        m_label2.setBackground(m_panel2.getBackground());
        m_label2.setText("MINERVA");
        m_panel2.add(m_label2);

        m_panel3 = new JPanel();
        m_label3 = new JTextPane();
        m_label3.setBackground(m_panel3.getBackground());
        m_label3.setEditable(false);
        m_label3.setText("     Masterclass INvolving Event \n Recognition Visualised with Atlantis");
        m_panel3.add(m_label3);

        m_panel4 = new JPanel();
        m_label4 = new JTextPane();
        m_label4.setBackground(m_panel4.getBackground());
        m_label4.setForeground(Color.blue);
        m_label4.setText("http://www.cern.ch/atlas-minerva");
        m_label4.setEditable(false);
        m_panel4.add(m_label4);

        m_textPanel = new JPanel();
        m_textPanel.setLayout(new GridLayout(3,1));
        m_textPanel.add(m_panel2);
        m_textPanel.add(m_panel3);
        m_textPanel.add(m_panel4);

        m_panel1.add(m_textPanel);

        // bottom panel - close button
        c_panel1 = new JPanel(new BorderLayout());
        closeButton = new JButton("Close");
        closeButton.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                AAboutDialog.getInstance().dispose();
            }
        });
        c_panel1.add(closeButton, BorderLayout.EAST);
        c_panel1.setBorder(BorderFactory.createEmptyBorder(15, 0, 15, 15));


        // main panel
        mainPanel = new JPanel();
        mainPanel.setLayout(new BorderLayout());
        mainPanel.add(a_panel1, BorderLayout.NORTH);
        mainPanel.add(m_panel1, BorderLayout.CENTER);
        mainPanel.add(c_panel1, BorderLayout.SOUTH);

        this.getContentPane().add(mainPanel, BorderLayout.CENTER);

        this.setResizable(false);

        this.setLocation(globals.getGuiFrame().getLocation());

    } // createGUI() --------------------------------------------------------


    public static AAboutDialog getInstance()
    {
        if(instance == null)
        {
            instance = new AAboutDialog("About Atlantis");
        }
        return instance;

    } // getInstance() ------------------------------------------------------



} // class AAboutDialog =====================================================
