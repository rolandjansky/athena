package atlantis.projection;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;

import javax.swing.ImageIcon;

import atlantis.canvas.AWindow;
import atlantis.event.AEvent;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;

/**
 * Stripped down clone of the 2D Projection class
 * Used simply to display event data (as text) in a window
 * with the ATLAS experiment logo, for publications and presentation purposes
 * 
 * Also contains rudimentary image scaling code used to scale the ATLAS logo depending
 * on window size
 * 
 * @author Tom McLaughlan
 */
public abstract class AProjectionInfo extends AProjection
{
    private boolean debug = false;

    protected final static String ASPECT_RATIO_1 = "Aspect Ratio 1";
    
    public AProjectionInfo()
    {}
    
    public boolean processLocalCommand(String name)
    {
        return false;
    }
    
    public abstract String getName();
    
    public String getScreenName()
    {
        return getName();
    }
    
    protected Color getBackgroundFillColor(Color[] colorMap)
    {
        return colorMap[parameterStore.get("Color", "BkgFill").getI()]; 
    }
    
    public void paint(AWindow window, Graphics g)
    {
        
        AEvent event = null;
        AGraphics ag = AGraphics.makeAGraphics(g);
        long time = 0;
        
        if (debug)
        {
            time = System.currentTimeMillis();
        }
        fillBackground(window, ag);
        if (debug)
        {
            System.out.println("fill " + (System.currentTimeMillis() - time));
            time = System.currentTimeMillis();
        }
        

        // draw the current event 
        event = eventManager.getCurrentEvent();
        
        //If there is no current event do nothing
        if (event == null) return;
     
    } // paint()
    
    protected void fillBackground(AWindow window, AGraphics ag)
    {
        ag.setColor(getBackgroundFillColor(AColorMap.getColors()));
        ag.fillRect(0, 0, window.getWidth(), window.getHeight());
    }
    
    protected static Point2D.Double[] aspectRatioLayout(double width, double height, Dimension wSize)
    {
        double f = Math.min(wSize.width / width, wSize.height / height);
        
        width = wSize.width / f;
        height = wSize.height / f;
        
        Point2D.Double[] corners = new Point2D.Double[3];
        
        corners[0] = new Point2D.Double(-width, +height);
        corners[1] = new Point2D.Double(+width, +height);
        corners[2] = new Point2D.Double(+width, -height);
        return corners;
    }
}
