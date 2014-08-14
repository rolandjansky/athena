package atlantis.interactions;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;

import javax.swing.JMenuItem;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.event.AEventManager;
import atlantis.graphics.ACursorFactory;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.parameters.APar;
import atlantis.parameters.AParameter;
import atlantis.projection.AProjection;
import atlantis.projection.AProjection2D;
import atlantis.projection.AProjection3D;
import atlantis.projection.AProjectionFR;
import atlantis.projection.AProjectionFZ;
import atlantis.projection.AProjectionLegoPlot;
import atlantis.projection.AProjectionRZ;
import atlantis.projection.AProjectionVP;
import atlantis.projection.AProjectionXZ;
import atlantis.projection.AProjectionYX;
import atlantis.projection.AProjectionYZ;
import atlantis.utils.AMath;
import atlantis.utils.AVector;

/**
 * The Zoom, Move and Rotate interaction.
 */
public class AZMRInteraction extends AInteraction implements
        ASleepMouseDragListener, AMousePressListener, AEnterExitListener, ActionListener
{
    // when these values are reached on any of the coordinates, zooming takes
    // no longer any effect
    private static final double MINIMAL_ZOOM_LIMIT = 0.001; // [cm] = 10 um
    private static final double MAXIMAL_ZOOM_LIMIT = 100000; // [cm] = 1000 m

    private static final int ZOOM_MODE = 0;
    private static final int HORIZONTAL_ZOOM_MODE = 1;
    private static final int VERTICAL_ZOOM_MODE = 2;
    private static final int ROTATE_MODE = 3;
    private static final int MOVE_MODE = 5;

    private int centerMarkRadius = 6;
    private Point2D.Double p0;
    private int previousKey = KeyEvent.VK_UNDEFINED;
    private int mode;
    private JMenuItem[] popupItems;
    private final static String TO_CENTER_OF_DETECTOR = "To Center Of Detector";
    private final static String CENTER_PICTURE = "Center The Picture";
    private final static String ASPECT_RATIO_1 = "Aspect Ratio 1";
    private final static String UNZOOM_FULL = "Unzoom Full";

    // if false, none of the ZMR interactions will have the red central dot
    // painted
    private static boolean paintCenterDot = true;

    public AZMRInteraction()
    {
        super(1, AUTOMATIC_REPAINT, WORLD_COORDINATES, false);

        popupItems = new JMenuItem[] {
            new JMenuItem(TO_CENTER_OF_DETECTOR),
            new JMenuItem(CENTER_PICTURE),
            new JMenuItem(UNZOOM_FULL)};
        
        for (int i = 0; i < popupItems.length; i++) {
            popupItems[i].addActionListener(this);
        }

        // The center of the ellipse is in (0, 0) in User Space
        hr[0] = new Ellipse2D.Double(-centerMarkRadius / 2,
                -centerMarkRadius / 2, centerMarkRadius, centerMarkRadius);
        
        //By default we are in zoom mode
        mode = ZOOM_MODE;
    }

    public static void setPaintCenterDot(boolean state)
    {
        paintCenterDot = state;
    }

    public void connect(AInteractionsManager manager)
    {
        super.connect(manager);
        Point2D.Double c = ((AProjection2D) window.getProjection()).getCenter();

        setCenter(hr[0], c.x, c.y);
    }

    public int getPressButton()
    {
        return AInteractionsManager.LEFT;
    }

    public int init(Point2D.Double p, int key)
    {
        keyChange(p, key);
        return -1;
    }

    public void pressed(Point2D.Double p, int button, int key)
    {
        if (key == KeyEvent.VK_C)
            setCenter(hr[0], p.x, p.y);
        if (key == KeyEvent.VK_0){
            double[] primaryVertex = AEventManager.instance().getCurrentEvent().getPrimaryVertex();
            if(window.getProjection().getName().equals("YX")) setCenter(hr[0], primaryVertex[0], primaryVertex[1]);
            if(window.getProjection().getName().equals("RZ")) {
                double phiMid2 = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
                double phiDiff = Math.abs(Math.atan2(primaryVertex[1], primaryVertex[0]) - phiMid2);
                int s=-1;
                if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.) s = 1;
                setCenter(hr[0], primaryVertex[2], s*Math.sqrt(primaryVertex[0]*primaryVertex[0]+primaryVertex[1]*primaryVertex[1]));
            }
        }
    }

    public int getButton()
    {
        return AInteractionsManager.LEFT;
    }

    public void start(Point2D.Double p, int region, int key)
    {}

    private double getHd0(Point2D.Double p)
    {
        Point2D.Double[] c = window.getUserCorners();
        AVector v21 = new AVector(c[2].x, c[2].y, c[1].x, c[1].y);
        AVector v10 = new AVector(c[1].x, c[1].y, c[0].x, c[0].y);
        Point2D.Double center = getCenter(hr[0]);

        return p.distance(AMath.intersectionPoint(center, v21, p, v10));
    }

    private double getVd0(Point2D.Double p)
    {
        Point2D.Double[] c = window.getUserCorners();
        AVector v01 = new AVector(c[0].x, c[0].y, c[1].x, c[1].y);
        AVector v12 = new AVector(c[1].x, c[1].y, c[2].x, c[2].y);
        Point2D.Double center = getCenter(hr[0]);

        return p.distance(AMath.intersectionPoint(center, v01, p, v12));
    }

    private void keyChange(Point2D.Double p, int key)
    {
        p0 = p;

        switch (key)
        {
            case KeyEvent.VK_UNDEFINED:
                mode = ZOOM_MODE;
                break;

            case KeyEvent.VK_H:
                mode = HORIZONTAL_ZOOM_MODE;
                break;

            case KeyEvent.VK_V:
                mode = VERTICAL_ZOOM_MODE;
                break;

            case KeyEvent.VK_R:
                mode = ROTATE_MODE;
                break;

            case KeyEvent.VK_M:
                mode = MOVE_MODE;
                break;

            case KeyEvent.VK_Z:
                mode = ZOOM_MODE;
                break;
        }
        
        //Update cursor mode when key changes
        setCursorMode();
    }

    public void drag(Point2D.Double p, int region, int key)
    {

        if (key != previousKey)
        {
            keyChange(p, key);
            previousKey = key;
        }

        double zf;
        Point2D.Double center = getCenter(hr[0]);
        AProjection proj = window.getProjection();

        switch (mode)
        {
            case ZOOM_MODE:
                zf = p0.distance(center.x, center.y)
                        / p.distance(center.x, center.y);
                performZoom(getCenter(hr[0]), zf, window);
                break;

            case HORIZONTAL_ZOOM_MODE:
                zf = getHd0(p0) / getHd0(p);
                performHorizontalZoom(center, zf, window);
                break;

            case VERTICAL_ZOOM_MODE:
                zf = getVd0(p0) / getVd0(p);
                performVerticalZoom(center, zf, window);
                break;

            case ROTATE_MODE:
                if ((proj instanceof AProjectionFR)
                        || (proj instanceof AProjectionFZ)
                        || (proj instanceof AProjectionVP))
                    performPhiRotation(p.y - p0.y, window);
                else if (proj instanceof AProjectionLegoPlot)
                    performLegoRotation(center, p.x - p0.x, p.y - p0.y, window);
                else
                {
                    if ((proj instanceof AProjectionRZ)
                            || (proj instanceof AProjectionXZ)
                            || (proj instanceof AProjectionYZ)
                            || (proj instanceof AProjection3D))
                    {
                        AParameter phiPar = parameterStore.get(proj.getName(), "Phi");
                        if (proj instanceof AProjection3D)
                        {
                            Point2D.Double pDisp = window.calculateDisplay(p);
                            Point2D.Double p0Disp = window.calculateDisplay(p0);
                            double height = window.getCurrDisp().getHeight();
                            double deltaV = pDisp.getY() - p0Disp.getY();
                            phiPar.setD(adjustPhi(phiPar.getD() + 360. * deltaV
                                    / height));
                        }
                        else
                            phiPar.setD(adjustPhi(phiPar.getD() + (p.y - p0.y)));
                        p0 = p;
                        ACanvas.getCanvas().repaintAllFromScratch();
                    }
                    else
                    {
                        double alpha = AMath.getAngle(p0, center)
                                - AMath.getAngle(p, center);

                        performRotation(alpha, center, window);
                        if (proj instanceof AProjectionYX)
                        {
                            AParameter phiPar = parameterStore.get(proj.getName(), "Phi");

                            phiPar.setD(adjustPhi(phiPar.getD() + Math.toDegrees(alpha)));
                        }
                    }
                }
                break;

            case MOVE_MODE:
                if ((proj instanceof AProjectionFR)
                        || (proj instanceof AProjectionFZ)
                        || (proj instanceof AProjectionVP))
                {
                    performMove(p.x - p0.x, 0, window);
                    performPhiRotation(p.y - p0.y, window);
                }
                else
                    performMove(p.x - p0.x, p.y - p0.y, window);
                break;
        }
    } // drag()
        // ---------------------------------------------------------------

    private double adjustPhi(double phi)
    {
        while (phi < 0)
            phi += 360;
        while (phi > 360)
            phi -= 360;
        return phi;
    }

    public void stop()
    {}

    public void cancel()
    {}

    public void paint(Graphics2D g)
    {
        Point2D.Double p = window.calculateDisplay(getCenter(hr[0]));

        //don't draw red dot if not drawing the lego plot 
        //(i.e. when just drawing the legend)
        boolean paintingJustLegoLegend = false;
        if(window.getProjection().getName().equals("LegoPlot"))
        {
            parameterStore.selectWindowParameters(window.getName());
            if(!parameterStore.get("LegoPlot","DrawPlot").getStatus())
                paintingJustLegoLegend=true;
            parameterStore.restoreWindowParameters();
        }
        
        if (paintCenterDot && !paintingJustLegoLegend)
        {
            g.setColor(Color.red);
            g.fillOval((int) (p.x - centerMarkRadius / 2),
                    (int) (p.y - centerMarkRadius / 2), centerMarkRadius,
                    centerMarkRadius);
        }
    }

    /**
     * Check the coordinates of the corners. If any of the limits was already
     * reached, zooming / unzooming takes no longer any effect.
     * 
     * @param corners Double
     * @return boolean
     */
    private static boolean checkCornerLimits(Point2D.Double[] corners)
    {
        
        for (int i = 0; i < corners.length; i++)
        {
            //Get the difference in x and y off this corner and the next
            //There is at most 3 corners - this will give us all of the combinations
            double dX=Math.abs(corners[i].x-corners[(i+1)%(corners.length-1)].x);
            double dY=Math.abs(corners[i].y-corners[(i+1)%(corners.length-1)].y);
            //Coordinates will differ in only x OR y, get the one that does
            double dMax = Math.max(dX,dY);
            //Check that corner coordinate difference is within limits
            if ((dMax < MINIMAL_ZOOM_LIMIT) || (dMax > MAXIMAL_ZOOM_LIMIT))
            {
              AOutput.alwaysAppend("zoom / unzoom limit reached\n",
              ALogInterface.NORMAL);
              return false;
            }
        }
          return true;
    } // checkTheCornerLimits()

    
    public static void performZoom(Point2D.Double center, double zf,
            AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        for (int i = 0; i < corners.length; i++)
        {
            corners[i].x = center.x + (corners[i].x - center.x) * zf;
            corners[i].y = center.y + (corners[i].y - center.y) * zf;
        }

        if (!checkCornerLimits(corners))
        {
            return; // zooming has already reached its limit
        }

        window.setUserCorners(corners);
    }

    public static void performRotation(double angle, Point2D.Double center,
            AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        double cos = Math.cos(angle);
        double sin = Math.sin(angle);

        for (int i = 0; i < corners.length; i++)
        {
            double dx = corners[i].x - center.x;
            double dy = corners[i].y - center.y;

            corners[i].x = center.x + dx * cos - dy * sin;
            corners[i].y = center.y + dx * sin + dy * cos;
        }

        window.setUserCorners(corners);
    }

    public static void performMinus90Rotation(AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        AVector v12 = new AVector(corners[1], corners[2]);
        double x3 = corners[0].x + v12.dx;
        double y3 = corners[0].y + v12.dy;

        corners[0].setLocation(corners[1]);
        corners[1].setLocation(corners[2]);
        corners[2].setLocation(x3, y3);

        window.setUserCorners(corners);
    }

    public static void performPlus90Rotation(AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        AVector v12 = new AVector(corners[1], corners[2]);
        double x3 = corners[0].x + v12.dx;
        double y3 = corners[0].y + v12.dy;

        corners[2].setLocation(corners[1].x, corners[1].y);
        corners[1].setLocation(corners[0].x, corners[0].y);
        corners[0].setLocation(x3, y3);

        window.setUserCorners(corners);
    }

    public static void performPhiRotation(double dPhi, AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        for (int i = 0; i < corners.length; i++)
            corners[i].y -= dPhi;

        window.setUserCorners(corners);
    }

    public void performLegoRotation(Point2D.Double center, double dPhi,
            double dEta, AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        if (dPhi != 0.0 || dEta != 0.0)
        {
            //get original phi coord in 0-360 range
            double centerX = -AProjectionLegoPlot.adjustPhi(window, -center.x, center.y);
                        
            corners[0].x -= (dPhi);
            // x offset=xz*360 so xz=change/360
            AProjectionLegoPlot.setxz(window.getIndex(), AProjectionLegoPlot
                    .getxz(window.getIndex())
                    + (dPhi) / 360);

            if (AProjectionLegoPlot.getyz(window.getIndex()) < 1.0
                    || (dEta) > 0.0)
            {
                corners[0].y -= (dEta);
                corners[1].y -= (dEta);
                // y offset=yz*50 so yz=change/50
                AProjectionLegoPlot.setyz(window.getIndex(),
                        AProjectionLegoPlot.getyz(window.getIndex()) - (dEta)
                                / 50);
            }
            
            //now have changed xz recalcuate phi coordinte
            center.x = AProjectionLegoPlot.adjustPhi(window, centerX, center.y);
            setCenter(hr[0], center.x, center.y);
            
            window.setUserCorners(corners);
        }
    }

    public static void performHorizontalZoom(Point2D.Double center, double f,
            AWindow window)
    {
        Point2D.Double[] c = window.getUserCorners();
        AVector v21 = new AVector(c[2].x, c[2].y, c[1].x, c[1].y);
        Point2D.Double center1 = new Point2D.Double(center.x + v21.dx, center.y
                + v21.dy);
        Point2D.Double p0 = AMath
                .intersectionPoint(c[0], c[1], center, center1);

        AVector v0 = new AVector(p0, c[0]).scale(f);
        AVector v1 = new AVector(p0, c[1]).scale(f);

        c[0].x = p0.x + v0.dx;
        c[0].y = p0.y + v0.dy;

        c[1].x = p0.x + v1.dx;
        c[1].y = p0.y + v1.dy;

        v21.invert();
        p0.setLocation(p0.x + v21.dx, p0.y + v21.dy);
        c[2].x = p0.x + v1.dx;
        c[2].y = p0.y + v1.dy;

        if (!checkCornerLimits(c))
        {
            return; // zooming has already reached its limit
        }

        window.setUserCorners(c);

    }

    public static void performVerticalZoom(Point2D.Double center, double f,
            AWindow window)
    {
        Point2D.Double[] c = window.getUserCorners();
        AVector v01 = new AVector(c[0].x, c[0].y, c[1].x, c[1].y);
        Point2D.Double center1 = new Point2D.Double(center.x + v01.dx, center.y
                + v01.dy);
        Point2D.Double p0 = AMath
                .intersectionPoint(c[1], c[2], center, center1);

        AVector v1 = new AVector(p0, c[1]).scale(f);
        AVector v2 = new AVector(p0, c[2]).scale(f);

        c[1].x = p0.x + v1.dx;
        c[1].y = p0.y + v1.dy;

        c[2].x = p0.x + v2.dx;
        c[2].y = p0.y + v2.dy;

        v01.invert();
        p0.setLocation(p0.x + v01.dx, p0.y + v01.dy);
        c[0].x = p0.x + v1.dx;
        c[0].y = p0.y + v1.dy;

        if (!checkCornerLimits(c))
        {
            return; // zooming has already reached its limit
        }

        window.setUserCorners(c);

    }

    public static void performMove(double dx, double dy, AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();

        for (int i = 0; i < corners.length; i++)
        {
            corners[i].x -= dx;
            corners[i].y -= dy;
        }

        window.setUserCorners(corners);
    }

    public static Line2D.Double getMiddleHorizontalLine(AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();
        AVector v12 = new AVector(corners[1], corners[2]).scale(0.5);

        return new Line2D.Double(corners[0].x + v12.dx, corners[0].y + v12.dy,
                corners[1].x + v12.dx, corners[1].y + v12.dy);
    }

    public static Line2D.Double getMiddleVerticalLine(AWindow window)
    {
        Point2D.Double[] corners = window.getUserCorners();
        AVector v10 = new AVector(corners[1], corners[0]).scale(0.5);

        return new Line2D.Double(corners[1].x + v10.dx, corners[1].y + v10.dy,
                corners[2].x + v10.dx, corners[2].y + v10.dy);
    }

    public static void performFlip(Line2D.Double line, AWindow window)
    {
        if (line == null)
            return;

        Point2D.Double[] corners = window.getUserCorners();
        double dSquare = Point2D.distanceSq(line.x1, line.y1, line.x2, line.y2);

        for (int i = 0; i < corners.length; i++)
        {
            double u = (1 / dSquare)
                    * ((corners[i].x - line.x1) * (line.x2 - line.x1) + (corners[i].y - line.y1)
                            * (line.y2 - line.y1));

            double x0 = line.x1 + u * (line.x2 - line.x1);
            double y0 = line.y1 + u * (line.y2 - line.y1);

            corners[i].x = 2 * x0 - corners[i].x;
            corners[i].y = 2 * y0 - corners[i].y;
        }

        window.setUserCorners(corners);
    }

    /**
     * Set cursor acording to current mode
     */
    public void setCursorMode(){
        //check which mode we have
        switch (mode)
        {
            case ZOOM_MODE:
                window.setCursor(ACursorFactory.getInstance().getZoomCursor());
                break;

            case HORIZONTAL_ZOOM_MODE:
                window.setCursor(ACursorFactory.getInstance().getZoomCursor());
                break;

            case VERTICAL_ZOOM_MODE:
                window.setCursor(ACursorFactory.getInstance().getZoomCursor());
                break;

            case ROTATE_MODE:
                window.setCursor(ACursorFactory.getInstance().getRotateCursor());
                break;

            case MOVE_MODE:
                window.setCursor(ACursorFactory.getInstance().getMoveCursor());
                break;
        }        
    }
    
    /**
     * Gets called each time we enter a window in which this interaction is active
     */
    public void entered()
    {
        //Set mouse cursor according to current mode
        setCursorMode();
    }

    /**
     * Gets called each time we leave a window in which this interaction is active
     */
    public void exited()
    {
       //Set mouse cursor
       window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }

    public int getPopupType()
    {
        return APopupListener.WINDOW_POPUP;
    }

    public JMenuItem[] getPopupItems() {
        return popupItems;
    }
    
    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();

        if (action.equals(TO_CENTER_OF_DETECTOR))
        {
            Point2D.Double c = ((AProjection2D) window.getProjection())
                    .getCenter();

            setCenter(hr[0], c.x, c.y);
            window.repaint();
        }
        else if (action.equals(CENTER_PICTURE))
        {
            Point2D.Double[] c = window.getUserCorners();
            AVector v02 = new AVector(c[0].x, c[0].y, c[2].x, c[2].y)
                    .scale(0.5);
            double xc = c[0].x + v02.dx;
            double yc = c[0].y + v02.dy;
            AVector vC0 = new AVector(xc, yc, 0, 0);
            AProjection proj = window.getProjection();
            if (proj instanceof AProjectionLegoPlot)
                vC0 = new AVector(xc, yc, 180 * (1 - AProjectionLegoPlot
                        .getxz(window.getIndex())), -5);
            // 180*(1-AProjectionLegoPlot.xz), -5 is different center as center
            // of plot is not at 0,0

            for (int i = 0; i < c.length; i++)
            {
                c[i].x += vC0.dx;
                c[i].y += vC0.dy;
            }
            window.setUserCorners(c);
        } else if (action.equals(ASPECT_RATIO_1)) {
            ((AProjection2D) window.getProjection()).setAspectRatio1(window);
        }
        else if (action.equals(UNZOOM_FULL))
        {
            window.setUserCorners(((AProjection2D) window.getProjection()).
                    calculateNoZoomCorners(window.getSize()));
        }
    }

    public AModifier[] getModifiers()
    {
        return new AModifier[] {
            new AModifier(KeyEvent.VK_UNDEFINED, false, "Zoom"),
            new AModifier(KeyEvent.VK_Z, false, "Zoom"),
            new AModifier(KeyEvent.VK_H, false, "Horizonatal Zoom"),
            new AModifier(KeyEvent.VK_V, false, "Vertical Zoom"),
            new AModifier(KeyEvent.VK_R, false, "Rotate"),
            new AModifier(KeyEvent.VK_F, false, "Fast Zoom"),
            new AModifier(KeyEvent.VK_M, false, "Move (pan)"),
            new AModifier(KeyEvent.VK_C, false, "Modify Central Point")
        };
    }

}
