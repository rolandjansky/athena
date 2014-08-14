package atlantis.gui;

import java.awt.BorderLayout;
import java.awt.Frame;
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
import atlantis.graphics.colormap.AColorMap;

/**
 * This dialog is displayed when the user wants to change the current color set.
 */
public class AColorMapDialog extends JDialog implements ActionListener
{
    private JPanel choosePanel;
    private JPanel buttonsPanel;
    private JButton okButton, cancelButton;
    private JRadioButton selected;
    
    private static final AGlobals globals = AGlobals.instance();
    
    public AColorMapDialog()
    {
        super(globals.getGuiFrame(), "Select Map Dialog", true);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setResizable(false);
        
        choosePanel = new JPanel();
        choosePanel.setBorder(BorderFactory.createTitledBorder(" Select Color Map "));
        
        ButtonGroup group = new ButtonGroup();
        
        choosePanel.setLayout(new GridLayout(5, 1));
        for (int i = 0; i < AColorMap.getMapNames().length - 2; i++)
        { // quick fix to not select hitcolor CT
            JRadioButton r = new JRadioButton(AColorMap.getMapNames()[i]);
            
            r.addActionListener(this);
            if (AColorMap.getColorMap() == i)
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
                AColorMap.setColorMap(selected.getText());
                ACanvas.getCanvas().repaintAllFromScratch();
                dispose();
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
        Frame owner = globals.getGuiFrame();
        int guiWidth = owner.getWidth();
        int guiHeight = owner.getHeight();
        int guiX = owner.getX();
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
