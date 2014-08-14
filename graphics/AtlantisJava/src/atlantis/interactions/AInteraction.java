package atlantis.interactions;

import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.Point2D;
import java.awt.geom.RectangularShape;
import java.awt.Point;
import javax.swing.JPopupMenu;
import javax.swing.JMenuItem;

import atlantis.canvas.AWindow;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;
import atlantis.gui.APopupHelper;
import atlantis.parameters.APar;
import atlantis.utils.APolygon;

/**
 * The abstract supperclass of all the Atlantis interactions.
 */
public abstract class AInteraction
{
	protected static APar parameterStore = APar.instance();
	
    /** When the inteaction does'nt need repainting */
    public final static int NO_REPAINT = 0;

    /**
     * When the inteaction needs an automatic repainting (acoording to its hot
     * regions)
     */
    public final static int AUTOMATIC_REPAINT = 1;

    /** When the inteaction needs an repaint of all windows */
    public final static int ALL_WINDOWS_REPAINT = 3;

    /** When the inteaction needs to reapint from scratch its window */
    public final static int WINDOW_TOTAL_REPAINT = 4;

    /**
     * The constant used to specify that the interaction wants SCREEN
     * COORDINATES
     */
    public final static int SCREEN_COORDINATES = 0;

    /** The constant used to specify that the interaction wants WORLD COORDINATES */
    public final static int WORLD_COORDINATES = 1;

    protected final static ADrawParameters frameDrawParameters = new ADrawParameters(true, AColorMap.BK, 4, 1, 1, AGraphics.SYMBOL_FILLED_BOX);

    protected final static ADrawParameters drawParameters = new ADrawParameters(true, AColorMap.WH, 4, 1, 0, AGraphics.SYMBOL_FILLED_BOX);

    protected AWindow window;
    protected static final int ctrlPtSize = 1;
    protected RectangularShape[] hr;
    protected Point2D.Double[] pt;
    protected Rectangle oldAffectedRegion;
    protected int hotRegionsCount;
    protected int repaintMode;
    protected int coordinatesType;
    protected boolean isPrintable;

    /**
     * Creates a new Interaction.
     * 
     * @param hotRegionsCount The number of hot regions used be the interaction
     * @param repaintMode The type of repaint wanted. Could be: NO_REPAINT,
     *        AUTOMATIC_REPAINT, WINDOW_REPAINT, ALL_WINDOWS_REPAINT,
     *        WINDOW_TOTAL_REPAINT, ALL_WINDOWS_TOTAL_REPAINT.
     * @param coordinatesType Could be: SCREEN_COORDINATES, WORLD_COORDINATES.
     * @param isPrintable A boolean which indicates whether or not this
     *        interaction should be printable.
     */
    public AInteraction(int hotRegionsCount, int repaintMode, int coordinatesType, boolean isPrintable)
    {

        this.hotRegionsCount = hotRegionsCount;
        this.repaintMode = repaintMode;
        this.coordinatesType = coordinatesType;
        this.isPrintable = isPrintable;

        hr = new RectangularShape[hotRegionsCount];

        /*
         * pt = new Point2D.Double[getPointsToIncludeCount()]; for (int i = 0; i <
         * pt.length; i++) pt[i] = new Point2D.Double(0, 0);
         */
    }

    /**
     * Called by the InteractionsManager when the interaction gets connected to
     * it.
     */
    public void connect(AInteractionsManager manager)
    {
        this.window = manager.getWindow();
    }

    /**
     * Called by the InteractionsManager when the interaction gets disconnected
     * from it.
     */
    public void disconnect()
    {
        this.window = null;
    }

    /**
     * Called by the window on which this interaction works in order to allow
     * the interaction to draw on it.
     * 
     * @param g The graphical context on which to draw.
     */
    public abstract void paint(Graphics2D g);

    /**
     * This method should convert any hot region used by interaction from user
     * coordinates to screen coordinates.
     * 
     * @param region The number of the hot region which needs to be converted.
     * @return The new RectangularShape object which contains the converted
     *         region.
     */
    public RectangularShape getScreenHotRegion(int region)
    {
        switch (coordinatesType)
        {
            case AInteraction.SCREEN_COORDINATES:
                return hr[region];

            case AInteraction.WORLD_COORDINATES:
                RectangularShape screenShape = (RectangularShape) hr[region].clone();
                Point2D.Double center = window.calculateDisplay(screenShape.getCenterX(), screenShape.getCenterY());
                double w = screenShape.getWidth();
                double h = screenShape.getHeight();

                screenShape.setFrame(center.x - w / 2, center.y - w / 2, w, h);
                return screenShape;

            default:
                return null;
        }
    }

    /**
     * Returns an APolygon made up of the first 4 Hot Regions.
     * 
     * @return The APolygon.
     */
    public APolygon getAPolygon()
    {
        APolygon p = new APolygon();

        for (int i = 0; i < 4; i++)
            p.addPoint(hr[i].getCenterX(), hr[i].getCenterY());

        return p;
    }

    /**
     * Sets the x-coordinate of the center of a RectangularShape.
     * 
     * @param shape The shape to set the center to.
     * @param x The new x-coordinate of the center.
     */
    protected void setCenterX(RectangularShape shape, double x)
    {
        double w = shape.getWidth();
        double h = shape.getHeight();

        shape.setFrame(x - w / 2, shape.getY(), w, h);
    }

    /**
     * Sets the y-coordinate of the center of a RectangularShape.
     * 
     * @param shape The shape to set the center to.
     * @param y The new y-coordinate of the center.
     */
    protected void setCenterY(RectangularShape shape, double y)
    {
        double w = shape.getWidth();
        double h = shape.getHeight();

        shape.setFrame(shape.getX(), y - h / 2, w, h);
    }

    /**
     * Sets the center of a RectangularShape to (x, y).
     * 
     * @param shape The shape to set the center to.
     * @param x The new x-coordinate of the center.
     * @param y The new y-coordinate of the center.
     */
    protected void setCenter(RectangularShape shape, double x, double y)
    {
        double w = shape.getWidth();
        double h = shape.getHeight();

        shape.setFrame(x - w / 2, y - h / 2, w, h);
    }

    /**
     * Returns the center of an arbitrary hot-region.
     * 
     * @param s The RectangularShape object the center of which is required
     * @return The center in double format
     */
    public Point2D.Double getCenter(RectangularShape s)
    {
        return new Point2D.Double(s.getCenterX(), s.getCenterY());
    }

    /**
     * Returns the distance between two hot-regions. The result depends on the
     * coordinates type.
     * 
     * @param region1 The first region
     * @param region2 The second region
     * @return The distance
     */
    protected double getRadius(int region1, int region2)
    {
        double dx = hr[region1].getCenterX() - hr[region2].getCenterX();
        double dy = hr[region1].getCenterX() - hr[region2].getCenterY();

        return Math.sqrt(dx * dx + dy * dy);
    }

    /**
     * Returns the distance between a point and a hot-regions. The result
     * depends on the coordinates type.
     * 
     * @param x The x coordinate of the point
     * @param y The y coordinate of the point
     * @param region The region index to be used to get the region itself
     * @return The distance
     */
    protected double getRadius(double x, double y, int region)
    {
        double dx = x - hr[region].getCenterX();
        double dy = y - hr[region].getCenterY();

        return Math.sqrt(dx * dx + dy * dy);
    }

    /**
     * Returns the distance between the center (0, 0) and a hot-regions. The
     * result depends on the coordinates type.
     * 
     * @param s The hot-region
     * @return The distance
     */
    protected double getRadius(RectangularShape s)
    {
        return s.getCenterX() * s.getCenterX() + s.getCenterY() * s.getCenterY();
    }

    /**
     * Retirns the angle made by the line connecting the two hot-regions and the
     * horizontal axis.
     * 
     * @param region1 The first region
     * @param region2 The second region
     * @return the angle in radians.
     */
    protected double getAngle(int region1, int region2)
    {
        double dx = hr[region1].getCenterX() - hr[region2].getCenterX();
        double dy = hr[region1].getCenterY() - hr[region2].getCenterY();

        if (dx != 0 || dy != 0)
            // is this correct
            return Math.atan2(dx, dy);
        else
            return 0.;
    }

    protected void drawActivePoint(double x, double y, AGraphics g)
    {
        g.updateDrawParameters(frameDrawParameters);
        g.drawSymbol(x, y);
        g.updateDrawParameters(drawParameters);
        g.drawSymbol(x, y);
    }

    protected void drawActivePoint(int region, AGraphics g)
    {
        g.updateDrawParameters(frameDrawParameters);
        g.drawSymbol(hr[region].getCenterX(), hr[region].getCenterY());
        g.updateDrawParameters(drawParameters);
        g.drawSymbol(hr[region].getCenterX(), hr[region].getCenterY());
    }

    protected void drawLine(int region1, int region2, AGraphics g)
    {
        g.drawLine(hr[region1].getCenterX(), hr[region1].getCenterY(), hr[region2].getCenterX(), hr[region2].getCenterY());
    }

    public boolean isPrintable()
    {
        return isPrintable;
    }

    public AModifier[] getModifiers()
    {
        return new AModifier[0];
    }
    
    public void showPopupMenu(Point p, int region) {
        APopupHelper.showPopupMenu(p, window, getPopupItems());
    }
    
    public JPopupMenu getPopupMenu() {
        return APopupHelper.getPopupMenu(getPopupItems());
    }
    
    public JMenuItem[] getPopupItems() {
        return null;
    }
    
}
