package atlantis.interactions;

import java.awt.Graphics2D;
import java.awt.geom.Point2D;

import atlantis.graphics.ACursorFactory;
import atlantis.graphics.AGraphics;

public class ARectangleSelection extends ASelection implements AEnterExitListener
{
    public ARectangleSelection()
    {
        super(4);
    }

    public int init(Point2D.Double p, int key)
    {
        isValid = false;

        for (int i = 0; i < 4; i++)
            setCenter(hr[i], p.x, p.y);

        this.region = 2;
        return region;
    }

    public void drag(Point2D.Double p, int region, int key)
    {
        isValid = true;

        // Update the active control point.
        setCenter(hr[region], p.x, p.y);

        // Get the opposite control point.
        int oppCtrlPt = (region + 2) % 4;
        double xOpp = hr[oppCtrlPt].getCenterX();
        double yOpp = hr[oppCtrlPt].getCenterY();

        // Now choose the next control point so that we maintain a
        // clockwise order to the points.
        int otherCtrlPt = ((p.x < xOpp && p.y > yOpp) || (p.x > xOpp && p.y < yOpp)) ? 1 : 3;

        otherCtrlPt = (region + otherCtrlPt) % 4;

        // Set the other two control points.
        setCenter(hr[otherCtrlPt], p.x, yOpp);
        otherCtrlPt = (otherCtrlPt + 2) % 4;
        setCenter(hr[otherCtrlPt], xOpp, p.y);
    }

    public void paint(Graphics2D g)
    {
        if (!isValid)
            return;

        AGraphics ag = AGraphics.makeAGraphics(g);

        ag.updateDrawParameters(frameDrawParameters);
        ag.drawPolygon(getAPolygon());
        ag.updateDrawParameters(drawParameters);
        ag.drawPolygon(getAPolygon());
        drawActivePoint(region, ag);
    }

    public Point2D.Double[] getCorners()
    {
        int first = getUpperLeftRegion();

        // Calculate the adjacent partner and the opposite corner.
        int second = (first + 1) % 4;
        int third = (first + 2) % 4;

        return convert(first, second, third);
    }

    /**
     * Get called each time we enter a window in which this interaction is
     * active
     */
    public void entered()
    {
        // Set mouse cursor
        window.setCursor(ACursorFactory.getInstance().getRectSelectCursor());
    }

    /**
     * Get called each time we leave a window in which this interaction is active
     */
    public void exited()
    {
        //Set mouse cursor
        window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }

}
