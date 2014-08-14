package atlantis.interactions;

import java.awt.Graphics2D;
import java.awt.geom.Point2D;

import atlantis.graphics.AGraphics;
import atlantis.utils.AMath;

public class ASquareSelection extends ASelection
{
    public ASquareSelection()
    {
        super(5);
    }

    public int init(Point2D.Double p, int key)
    {
        isValid = false;

        for (int i = 0; i < 5; i++)
            setCenter(hr[i], p.x, p.y);

        this.region = 2;
        return region;
    }

    public void drag(Point2D.Double p, int region, int key)
    {
        isValid = true;

        if (region == 4)
        {
            double dx = p.x - hr[4].getCenterX();
            double dy = p.y - hr[4].getCenterY();

            for (int i = 0; i < 5; i++)
                setCenter(hr[i], hr[i].getCenterX() + dx, hr[i].getCenterY() + dy);
        }
        else
        {
            double ratio = ((double) window.getSize().width) / ((double) window.getSize().height);

            double sx1 = AMath.getSign(p.x - hr[4].getCenterX());
            double sy1 = AMath.getSign(p.y - hr[4].getCenterY());
            double sx2 = sy1;
            double sy2 = sx1;
            double dy = getRadius(p.x, p.y, 4) / Math.sqrt(1 + ratio * ratio);
            double dx = dy * ratio;

            setCenter(hr[region], hr[4].getCenterX() + dx * sx1, hr[4].getCenterY() + dy * sy1);
            setCenter(hr[(region + 2) % 4], hr[4].getCenterX() - dx * sx1, hr[4].getCenterY() - dy * sy1);

            setCenter(hr[(region + 1) % 4], hr[4].getCenterX() - dx * sx2, hr[4].getCenterY() + dy * sy2);
            setCenter(hr[(region + 3) % 4], hr[4].getCenterX() + dx * sx2, hr[4].getCenterY() - dy * sy2);
        }
    }

    public void paint(Graphics2D g)
    {
        if (isValid)
        {
            AGraphics ag = AGraphics.makeAGraphics(g);

            ag.updateDrawParameters(frameDrawParameters);
            ag.drawPolygon(getAPolygon());
            drawLine(0, 2, ag);
            drawLine(1, 3, ag);
            ag.updateDrawParameters(drawParameters);
            ag.drawPolygon(getAPolygon());
            drawLine(0, 2, ag);
            drawLine(1, 3, ag);
            drawActivePoint(region, ag);
        }
    }

    public Point2D.Double[] getCorners()
    {
        int first = getUpperLeftRegion();

        // Calculate the adjacent partner and the opposite corner.
        int second = (first + 1) % 4;
        int third = (first + 2) % 4;

        return convert(first, second, third);
    }

}
