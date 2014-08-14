package atlantis.gui;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import atlantis.canvas.ACanvas;
import atlantis.globals.AGlobals;

/**
 * A dialog which lets the user to select between the different layouts of the
 * canvas.
 */
public class ALayoutDialog extends JDialog implements ActionListener
{
    private JPanel choosePanel;
    private JPanel buttonsPanel;
    private JButton okButton, cancelButton;
    private JRadioButton selected;
    
    private static final AGlobals globals = AGlobals.instance();

    public ALayoutDialog()
    {
        super(globals.getGuiFrame(), "Layout Properties Dialog", true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setResizable(false);
        
        choosePanel = new JPanel();
        choosePanel.setBorder(BorderFactory.createTitledBorder(" Select Layout "));
        
        String[] layoutNames = ACanvas.getCanvas().getLayoutNames();
        ButtonGroup group = new ButtonGroup();
        
        choosePanel.setLayout(new GridLayout(layoutNames.length, 1));
        for (int i = 0; i < layoutNames.length; i++)
        {
            JRadioButton r = new JRadioButton(layoutNames[i]);
            
            r.addActionListener(this);
            if (ACanvas.getCanvas().getCurrentLayout().getName().equals(layoutNames[i]))
            {
                selected = r;
                r.setSelected(true);
            }
            else
                r.setSelected(false);
            group.add(r);
            choosePanel.add(r);
        }
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(choosePanel, BorderLayout.CENTER);
        
        okButton = new JButton("OK");
        okButton.addActionListener(new ActionListener()
                {
            public void actionPerformed(ActionEvent e)
            {
                ACanvas.getCanvas().setCurrentLayout(selected.getText());
                dispose();
                ACanvas.getCanvas().bringToFront();
            }
                });
        
        cancelButton = new JButton("Cancel");
        cancelButton.addActionListener(new ActionListener()
                {
            public void actionPerformed(ActionEvent e)
            {
                dispose();
            }
                });
        
        buttonsPanel = new JPanel();
        buttonsPanel.add(okButton);
        buttonsPanel.add(cancelButton);
        getContentPane().add(buttonsPanel, BorderLayout.SOUTH);
        pack();
        
        // set the initial location
        AGUI gui = AGUI.getGUI();
        int guiWidth = gui.getWidth();
        int guiHeight = gui.getHeight();
        int guiX = gui.getX();
        int dialogWidth = (int) this.getPreferredSize().getWidth();
        int dialogHeight = (int) this.getPreferredSize().getHeight();
        int screenWidth = Math.round((float) Toolkit.getDefaultToolkit().getScreenSize().getWidth());
        if(guiX+guiWidth+(dialogWidth-guiWidth)/2>screenWidth)
            this.setLocation(Math.max(0, screenWidth - dialogWidth), Math.max(0, (guiHeight - dialogHeight) / 3));
        else
            this.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2), Math.max(0, (guiHeight - dialogHeight) / 3));
        setVisible(true);
    }
    
    public void actionPerformed(ActionEvent e)
    {
        selected = (JRadioButton) e.getSource();
    }
    
}
