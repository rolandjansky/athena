package atlantis.projection;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.geom.Point2D;
import java.util.List;
import java.util.ArrayList;

import javax.swing.JMenuItem;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.ACalorimeterData;
import atlantis.data.ADHelix;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.geometry.AAtlasDetector;
import atlantis.graphics.ACoord;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.AParameter;
import atlantis.utils.APolygon;
import atlantis.utils.AVector;

/**
 * Base class of Standard 2D projections handles the painting of a projection
 * Handles non-linear transforms (linear transforms in AWindow)
 */
public abstract class AProjection2D extends AProjection
{
    private boolean debug = false;
    protected ArrayList<JMenuItem> popupItems;
    protected final static String ASPECT_RATIO_1 = "Aspect Ratio 1";
    
    public AProjection2D()
    {
        popupItems = new ArrayList<JMenuItem>();
    }
    
    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();
        
        if (action.equals(ASPECT_RATIO_1))
            setAspectRatio1(ACanvas.getCanvas().getCurrentWindow());
    }
    
    public void setAspectRatio1(AWindow window)
    {
        //Get the extend of the shown projection (i.e. the corners in user coordinates)
        Point2D.Double[] corners = window.getUserCorners();
        AVector v01 = new AVector(corners[0], corners[1]);
        AVector v12 = new AVector(corners[1], corners[2]);
        //Get the actual size of this window on the screen
        Rectangle wSize = window.getBounds();
        
        //Calculate new corner positions
        //This seems a bit longish to me... CLEANUP - S.B.

        //Calculate stretch factor f such that we keep the larger one of the two extends in user coordinates 
        //"modulus" just means length of vector!
        double f = Math.min(wSize.width / v01.modulus(), wSize.height / v12.modulus());
    
        //Rescale new width and height
        double w = wSize.width / f;
        double h = wSize.height / f;
        
        //All this just to calculate the center and the new scale
        //Maybe easier to split with an if-statement rather than using "Math.min" above
        AVector down = v12.getUnitary().scale(h / 2);
        AVector up = v12.getUnitary().invert().scale(h / 2);
        
        AVector v0 = v01.getUnitary().invert().scale(w / 2).add(up);
        AVector v1 = v01.getUnitary().scale(w / 2).add(up);
        AVector v2 = v01.getUnitary().scale(w / 2).add(down);
        
        AVector v02 = new AVector(corners[0], corners[2]).scale(0.5);
        Point2D.Double center = new Point2D.Double(corners[0].x + v02.dx, corners[0].y + v02.dy);
        
        //Set the new corner positions
        corners[0].setLocation(center.x + v0.dx, center.y + v0.dy);
        corners[1].setLocation(center.x + v1.dx, center.y + v1.dy);
        corners[2].setLocation(center.x + v2.dx, center.y + v2.dy);
        
        //Apply new user coordinates
        window.setUserCorners(corners);
    }
    
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
    
    // min rho for tracks
    public double getMinRho()
    {
        return 0.;
    }
    
    public void paint(AWindow window, Graphics g)
    {
        
        AEvent event = null;
        List hitsAndTracks = null;
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
        
        // draw detector geometry
        AAtlasDetector.getDetector().draw(window, ag, this);
        if (debug)
        {
            System.out.println("det " + (System.currentTimeMillis() - time));
            time = System.currentTimeMillis();
        }
        
        // is true when F key is pressed during ZMR operations (detector is
        // zoomed for instance, but data are not drawn during the operation)
        boolean fastZooming = parameterStore.get("Projection", "SkipData").getStatus();
        if (fastZooming)
        {
            return;
        }

        // draw the current event 
        event = eventManager.getCurrentEvent();
        
        //If there is no current event do nothing
        if (event == null) return;
        
        ACalorimeterData.drawCalorimeters(window, ag, this, event);
        
        hitsAndTracks = event.getHitsAndTracks(this);
        if (debug)
        {
            System.out.println("calo " + (System.currentTimeMillis() - time));
            time = System.currentTimeMillis();
        }
        for (int h = 0; h < hitsAndTracks.size(); h++)
        {
            ((AData) hitsAndTracks.get(h)).draw(window, ag, this);
            if (debug)
            {
                long delta = (System.currentTimeMillis() - time);
                if (delta > 0)
                {
                    System.out.println(((AData) hitsAndTracks.get(h)).getName() + " " + delta);
                }
                time = System.currentTimeMillis();
            }
        } // for
     
    } // paint()
    
    protected void fillBackground(AWindow window, AGraphics ag)
    {
        ag.setColor(getBackgroundFillColor(AColorMap.getColors()));
        ag.fillRect(0, 0, window.getWidth(), window.getHeight());
    }
    
    // this is here to make drawing of tracks fast enough
    public abstract ACoord getUserPoint(ADHelix dH, double s);
    
    public ACoord nonLinearTransform(ACoord user)
    {
        return user;
    }
    
    public ACoord[] nonLinearTransform(ACoord[] user)
    {
        if (user != null)
            for (int i = 0; i < user.length; i++)
                user[i] = nonLinearTransform(user[i]);
        return user;
    }
    
    public Point2D.Double nonLinearTransform(double x, double y)
    {
        ACoord c = nonLinearTransform(new ACoord(x, y));
        return new Point2D.Double(c.hv[0][0][0], c.hv[1][0][0]);
    }
    
    public Point2D.Double nonLinearTransform(Point2D.Double user)
    {
        return nonLinearTransform(user.x, user.y);
    }
    
    public ACoord inverseNonLinearTransform(ACoord user)
    {
        return user;
    }
    
    public Point2D.Double inverseNonLinearTransform(double x, double y)
    {
        ACoord c = inverseNonLinearTransform(new ACoord(x, y));
        
        return new Point2D.Double(c.hv[0][0][0], c.hv[1][0][0]);
    }
    
    public Point2D.Double inverseNonLinearTransform(Point2D.Double user)
    {
        return inverseNonLinearTransform(user.x, user.y);
    }
    
    protected static ACoord nonLinearTransform2D(ACoord user, String projectionName)
    {
        AParameter fishEyePar = parameterStore.get(projectionName, "FishEye");
        
        if (fishEyePar.getStatus())
        {
            double[] z, r;
            double fishEye = 0.001 * fishEyePar.getD();
            double rTo = parameterStore.get(projectionName, "rTo").getD();
            double zTo = parameterStore.get(projectionName, "zTo").getD();
            
            for (int j = 0; j < user.hv[0].length; ++j)
            {
                z = user.hv[0][j];
                r = user.hv[1][j];
                for (int i = 0; i < z.length; ++i)
                {
                    z[i] *= (1 + fishEye * zTo) / (1 + fishEye * Math.abs(z[i]));
                    r[i] *= (1 + fishEye * rTo) / (1 + fishEye * Math.abs(r[i]));
                }
            }
        }
        return user;
    }
    
    protected static ACoord inverseNonLinearTransform2D(ACoord user, String projectionName)
    {
        AParameter fishEyePar = parameterStore.get(projectionName, "FishEye");
        
        if (fishEyePar.getStatus())
        {
            double[] z, r;
            double fishEye = 0.001 * fishEyePar.getD();
            double rTo = parameterStore.get(projectionName, "rTo").getD();
            double zTo = parameterStore.get(projectionName, "zTo").getD();
            
            for (int j = 0; j < user.hv[0].length; ++j)
            {
                z = user.hv[0][j];
                r = user.hv[1][j];
                for (int i = 0; i < z.length; ++i)
                {
                    z[i] *= 1 / (1 + fishEye * (zTo - Math.abs(z[i])));
                    r[i] *= 1 / (1 + fishEye * (rTo - Math.abs(r[i])));
                }
            }
        }
        return user;
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
    
    public void setScales()
    {}
    
    public abstract Point2D.Double[] calculateNoZoomCorners(Dimension wSize);
    
    public abstract Point2D.Double getCenter();
    
    public Point2D.Double[] validateCorners(Point2D.Double[] corners)
    {
        APolygon p = new APolygon();
        
        for (int i = 0; i < corners.length; i++)
            p.addPoint(corners[i].x, corners[i].y);
        
        if (p.getArea() == 0)
            return null;
        else
            return corners;
    }
    
    /**
     * This function is called when the ACanvas configuration was written on
     * a screen with one aspect ratio but the current screen has a different
     * aspect ratio. Subclasses may re-implement this to behave however they
     * like. At present (when this was written) only YX does anything.
     * 
     *  -- Adam Davison
     * 
     * @param w AWindow object containing this
     * @param oldaspect Aspect ratio of screen where current window corners were chosen
     * @param newaspect Aspect ratio of screen now
     */
    
    public void aspectRatioChange(AWindow w, double oldaspect, double newaspect) {
        // Do nothing by default
    }
    
    public JMenuItem[] getPopupItems()
    {
        // Can't cast arrays very easily...
        JMenuItem[] ret = new JMenuItem[popupItems.size()];
        for (int i = 0; i < popupItems.size(); i++) {
            ret[i] = (JMenuItem)popupItems.get(i);
        }
        return ret;
    }
    
    protected void addPopupItem(String str) {
        JMenuItem it = new JMenuItem(str);
        popupItems.add(it);
        it.addActionListener(this);
    }
    
}
