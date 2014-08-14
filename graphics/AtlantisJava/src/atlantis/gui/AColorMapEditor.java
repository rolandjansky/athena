package atlantis.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import atlantis.canvas.ACanvas;
import atlantis.globals.AGlobals;
import atlantis.graphics.colormap.AColorMap;
import atlantis.graphics.colormap.MappedColor;
import atlantis.graphics.layout.AFlowLayout;
import atlantis.graphics.dnd.ADnDLabel;
import atlantis.graphics.dnd.ADragListener;
import atlantis.utils.AUtilities;

/**
 * The dialog allows the user to edit the colors maps (at runtime).
 */
public class AColorMapEditor extends JDialog implements ADragListener
{
    private static AColorMapEditor instance;
    private ADnDLabel[][] labels = new ADnDLabel[AColorMap.getNumMaps()][AColorMap.getNumColors()];
    private int colorMap;
    private int index;
    private MappedColor previousColor;
    private ADnDLabel source;
    
    private JPanel colorPanel, buttonsPanel;
    private JButton okButton, applyButton, cancelButton;
    
    private static final AGlobals globals = AGlobals.instance();

    private AColorMapEditor()
    {
        super(globals.getGuiFrame(), "Color Map Editor", false);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        setResizable(false);
        colorPanel = new JPanel();
        colorPanel.setLayout(new GridLayout(AColorMap.getNumColors() + 1, AColorMap.getNumMaps() + 1, 0, 0));
        
        // creating the names row
        JLabel padding = new JLabel("");
        colorPanel.add(padding);
        for (int i = 0; i < AColorMap.getNumMaps(); i++)
        {
            JLabel label = new JLabel("  " + AColorMap.getMapNames()[i] + "  ", JLabel.CENTER);
            label.setFont(new Font("Dialog", Font.BOLD, 14));
            colorPanel.add(label);
        }
        
        // creating the color table
        Color[] col;
        MouseListener labelMouseListener = new LabelMouseListener();
        
        for (int j = 0; j < AColorMap.getNumColors(); j++)
        {
            JLabel label = new JLabel("" + j, JLabel.CENTER);
            label.setFont(new Font("Dialog", Font.BOLD, 14));
            colorPanel.add(label);
            for (int i = 0; i < AColorMap.getNumMaps(); i++)
            {
                col = AColorMap.getColors(i);
                labels[i][j] = new ADnDLabel("text", false);
                labels[i][j].addDragListener(this);
                labels[i][j].setOpaque(true);
                labels[i][j].setBackground(col[j]);
                labels[i][j].setForeground(col[j]);
                labels[i][j].setToolTipText(col[j].getRed() + ", " + col[j].getGreen() + ", " + col[j].getBlue());
                setLineBorder(labels[i][j]);
                labels[i][j].addMouseListener(labelMouseListener);
                colorPanel.add(labels[i][j]);
            }
        }
        
        buttonsPanel = new JPanel();
        buttonsPanel.setLayout(new AFlowLayout(10, 10));
        
        okButton = new JButton("OK");
        okButton.addActionListener(new ActionListener()
                {
            public void actionPerformed(ActionEvent e)
            {
                applyColors();
                dispose();
                instance = null;
            }
                });
        buttonsPanel.add(okButton);
        
        applyButton = new JButton("Apply");
        applyButton.addActionListener(new ActionListener()
                {
            public void actionPerformed(ActionEvent e)
            {
                applyColors();
            }
                });
        buttonsPanel.add(applyButton);
        
        cancelButton = new JButton("Cancel");
        cancelButton.addActionListener(new ActionListener()
                {
            public void actionPerformed(ActionEvent e)
            {
                dispose();
                instance = null;
            }
                });
        buttonsPanel.add(cancelButton);
        
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(colorPanel, BorderLayout.CENTER);
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
    
    public static AColorMapEditor getInstance()
    {
        if (instance == null)
        {
            instance = new AColorMapEditor();
        }
        return instance;
    }
    
    public void updateColors()
    {
        if (instance != null)
        {
        	MappedColor[][] maps = AColorMap.getMaps();
            for (int i = 0; i < maps.length; i++)
                for (int j = 0; j < maps[i].length; j++)
                {
                    Color c = maps[i][j];
                    
                    labels[i][j].setBackground(c);
                    labels[i][j].setForeground(c);
                    labels[i][j].setToolTipText(c.getRed() + ", " + c.getGreen() + ", " + c.getBlue());
                }
            repaint();
        }
    }
    
    public void dragPerformed(Object from, Object to, int action)
    {
        ADnDLabel fromLabel = (ADnDLabel) from;
        ADnDLabel toLabel = (ADnDLabel) to;
        
        toLabel.setBackground(fromLabel.getBackground());
    }
    
    class LabelMouseListener extends MouseAdapter
    {
        public void mouseClicked(MouseEvent e)
        {
            if (SwingUtilities.isLeftMouseButton(e))
            {
                source = (ADnDLabel) e.getSource();
                Color color = JColorChooser.showDialog(instance, "Color Map Editor - Chooser", source.getBackground());
                if (color != null)
                {
                    source.setBackground(color);
                    source.setForeground(color);
                    setLineBorder(source);
                    source.setToolTipText(color.getRed() + ", " + color.getGreen() + ", " + color.getBlue());
                }
            }
        }
        
        public void mousePressed(MouseEvent e)
        {
            source = (ADnDLabel) e.getSource();
            getPosition();
            
            if (AUtilities.isRightMouseButton(e))
            {
                if (index < 8)
                    for (int j = 8; j < AColorMap.getNumColors(); j++)
                        labels[colorMap][j].setBackground(labels[colorMap][index].getBackground());
                else
                    for (int j = 0; j < 8; j++)
                        labels[colorMap][j].setForeground(labels[colorMap][index].getForeground());
                
            }
            else if (SwingUtilities.isMiddleMouseButton(e))
            {
            	MappedColor[][] maps = AColorMap.getMaps();
                previousColor = maps[colorMap][index];
                MappedColor c = maps[colorMap][index];
                
                if (c.getGreen() > 128)
                    maps[colorMap][index] = maps[colorMap][AColorMap.BK];
                else
                    maps[colorMap][index] = maps[colorMap][AColorMap.WH];
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
        
        public void mouseReleased(MouseEvent e)
        {
            if (AUtilities.isRightMouseButton(e))
            {
                if (index < 8)
                    for (int j = 8; j < AColorMap.getNumColors(); j++)
                        labels[colorMap][j].setBackground(labels[colorMap][j].getForeground());
                else
                    for (int j = 0; j < 8; j++)
                        labels[colorMap][j].setForeground(labels[colorMap][j].getBackground());
            }
            else if (SwingUtilities.isMiddleMouseButton(e))
            {
            	MappedColor[][] maps = AColorMap.getMaps();
                maps[colorMap][index] = previousColor;
                ACanvas.getCanvas().repaintAllFromScratch();
            }
        }
    }
    
    private void setLineBorder(JComponent comp)
    {
        comp.setBorder(BorderFactory.createLineBorder(new Color(207, 207, 207), 3));
    }
    
    private void getPosition()
    {
        for (int i = 0; i < AColorMap.getNumMaps(); i++)
            for (int j = 0; j < AColorMap.getNumColors(); j++)
                if (labels[i][j] == source)
                {
                    colorMap = i;
                    index = j;
                    break;
                }
    }
    
    private void applyColors()
    {
    	MappedColor[][] maps = AColorMap.getMaps();
        for (int i = 0; i < maps.length; i++)
            for (int j = 0; j < maps[i].length; j++)
            {
                Color c = labels[i][j].getBackground();
                
                maps[i][j] = new MappedColor(c.getRed(), c.getGreen(), c.getBlue(), j);
            }
        AGUI.getGUI().repaintTable();
        ACanvas.getCanvas().repaintAllFromScratch();
    }
    
}
