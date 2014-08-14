package atlantis.canvas;

import java.awt.Color;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.Toolkit;

import javax.swing.JFrame;
import javax.swing.JTextField;

import atlantis.globals.AGlobals;
import atlantis.graphics.AIcon;
import atlantis.graphics.colormap.AColorMap;

/**
 * The legend window appears when selected in the preferences menu 
 * It displays the energy scale for the summed endcaps as these do not
 * have the pick interaction.
 * 
 * Moved from package atlantis.gui to atlantis.canvas because of closer
 * coupling with classes in atlantis.canvas and atlantis.data. Could move
 * back later if can be decoupled. (Ben Waugh, 2011-10-01)
 * 
 * @author Mark Stockton
 */
public class ALegendWindow extends JFrame
{
    private static JTextField[] titleField = new JTextField[2];
    private static JTextField[] textField;
    private static ALegendWindow instance = null;

    public static ALegendWindow getInstance()
    {
        if (instance == null)
            instance = new ALegendWindow();
        return instance;
    }
    
    /**
     * Check if ALegendWindow instance exists, without creating it if not.
     * @return true if ALegendWindow instance exists
     */
    public static boolean exists() {
    	return (instance != null);
    }
    
    public void addEnergyText(AWindow window, double[] Energies, double Energy, Color[] colorMap)
    {
        if(window.equals(ACanvas.getCanvas().getCurrentWindow()))
        {
            getContentPane().removeAll();
            validate();
            getContentPane().setLayout(new GridLayout(Energies.length+titleField.length,1));
            
            titleField[0].setText("Color Energy thresholds:");
            getContentPane().add(titleField[0]);
            if(titleField[1]==null)
                titleField[1]=new JTextField();
            titleField[1].setEditable(false);
            titleField[1].setBackground(Color.white);
            titleField[1].setForeground(Color.black);
            titleField[1].setText("Maximum Cell Energy: " + (Math.rint(1000. * Energy)/1000) + " GeV ");
            getContentPane().add(titleField[1]);
            
            String s="";
            for(int i=Energies.length-1;i>=0;i--)
            {
                if(textField[i]==null)
                    textField[i]=new JTextField();
                textField[i].setEditable(false);
                //to even out the line spacing
                if(i<10)
                    s="0" + i;
                else
                    s="" + i;
                s ="Color " + s + " Energy from: " + (Math.rint(1000. * Energies[i])/1000) + " GeV";
                textField[i].setText(s);
                textField[i].setBackground(colorMap[i]);
                //find out if too dark for black text
                if( colorMap[i].getRed() + colorMap[i].getGreen() +
                        colorMap[i].getBlue()<=128)
                    textField[i].setForeground(Color.white);
                else
                    textField[i].setForeground(Color.black);
                getContentPane().add(textField[i]);
            }
            invalidate();
            pack();       
            setVisible(true);
        }
    }
    
    public void addEnergyText(AWindow window, double minEnergy, double maxEnergy)
    {
        if(window.equals(ACanvas.getCanvas().getCurrentWindow()))
        {
            getContentPane().removeAll();
            validate();
            getContentPane().setLayout(new GridLayout(3,1));
            titleField[0].setText("Color Energy thresholds:");
            getContentPane().add(titleField[0]);
            if(titleField[1]==null)
                titleField[1]=new JTextField();
            titleField[1].setEditable(false);
            titleField[1].setBackground(Color.white);
            titleField[1].setForeground(Color.black);
            titleField[1].setText("Maximum Cell Energy: " + (Math.rint(1000. * maxEnergy)/1000) + " GeV ");
            getContentPane().add(titleField[1]);
            
            if(textField[0]==null)
                textField[0]=new JTextField();
            textField[0].setEditable(false);
            textField[0].setText("Minimum Cell Energy: " + (Math.rint(1000. * minEnergy)/1000) + " GeV ");
            textField[0].setBackground(Color.white);
            textField[0].setForeground(Color.black);
            getContentPane().add(textField[0]);
            invalidate();
            pack();
            setVisible(true);
        }
    }
    
    public void clearText(AWindow window)
    {
        if(!titleField[0].getText().equals("Select colour by Energy to bring up scale")
                && window.equals(ACanvas.getCanvas().getCurrentWindow()))
        {
            getContentPane().removeAll();
            validate();
            getContentPane().setLayout(new GridLayout(1,1));
            titleField[0].setText("Select colour by Energy to bring up scale");
            getContentPane().add(titleField[0]);
            invalidate();
            pack();
            setVisible(true);    
        }
    }
    
    public void nothingToDisplay(AWindow window)
    {
        if(!titleField[0].getText().equals("Select LAr/HEC Summed and colour by Energy to bring up scale")
                && window.equals(ACanvas.getCanvas().getCurrentWindow()))
        {
            getContentPane().removeAll();
            validate();
            getContentPane().setLayout(new GridLayout(1,1));
            titleField[0].setText("Select LAr/HEC Summed and colour by Energy to bring up scale");
            getContentPane().add(titleField[0]);
            invalidate();
            pack();
            setVisible(true);            
        }

    }
    
    private ALegendWindow()
    {
        this.setTitle("Legend");
        AIcon.setIconImage(this);
        this.setResizable(false);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        int numColors = AColorMap.getNumColors();
        getContentPane().setLayout(new GridLayout(1,1));
        textField = new JTextField[numColors];
        titleField[0] =new JTextField();
        titleField[0].setEditable(false);
        titleField[0].setBackground(Color.white);
        titleField[0].setForeground(Color.black);
        titleField[0].setText("Select LAr/HEC Summed and colour by Energy to bring up scale");
        getContentPane().add(titleField[0]);
        
        // set the initial location
        Component gui = AGlobals.instance().getGuiFrame();
        int guiWidth = gui.getWidth();
        int guiHeight = gui.getHeight();
        int guiX = gui.getX();
        int dialogWidth = (int) this.getPreferredSize().getWidth();
        int dialogHeight = (int) this.getPreferredSize().getHeight();
        int screenWidth = Math.round((float) Toolkit.getDefaultToolkit()
                .getScreenSize().getWidth());
        if (guiX + guiWidth + (dialogWidth - guiWidth) / 2 > screenWidth)
            this.setLocation(Math.max(0, screenWidth - dialogWidth), Math.max(
                    0, (guiHeight - dialogHeight) / 3));
        else
            this.setLocation(Math.max(0, guiX + (guiWidth - dialogWidth) / 2),
                    Math.max(0, (guiHeight - dialogHeight) / 3));
        
        pack();
        setVisible(true);
    }

    public void dispose()
    {
        instance = null;
        super.dispose();
    }
}
