package atlantis.interactions;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.geom.Point2D;

import atlantis.graphics.ACursorFactory;

public class AWindowInfoInteraction extends AInteraction implements AMouseDragListener, AEnterExitListener
{
    // private int width=64;
    private Point pickedPoint;
    private Color c;

    public AWindowInfoInteraction()
    {
        super(1, AUTOMATIC_REPAINT, SCREEN_COORDINATES, true);

        this.c = Color.white;

        // hr[0] = new Rectangle2D.Double(x, y, width, width);
        pickedPoint = new Point(0, 0);
    }

    public int getButton()
    {
        return AInteractionsManager.LEFT;
    }

    public void paint(Graphics2D g)
    {
        g.setColor(c);
        g.fill(hr[0]);

        /*
         * g.fillRect(hr[0].getX(), hr[0].getY(), hr[0].getWidth(),
         * hr[0].getHeight());
         */
    }

    public void start(Point2D.Double p, int region, int key)
    {
        pickedPoint.setLocation(p.x - hr[0].getX(), p.y - hr[0].getY());
    }

    public void drag(Point2D.Double p, int region, int key)
    {
        hr[0].setFrame(p.x - pickedPoint.x, p.y - pickedPoint.y, hr[0].getWidth(), hr[0].getHeight());
    }

    public void stop()
    {}

    public void cancel()
    {}

    public void entered()
    {
        window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }

    public void exited()
    {
        window.setCursor(ACursorFactory.getInstance().getDefaultCursor());
    }

    public int getPopupType()
    {
        return APopupListener.REGION_POPUP;
    }

}
