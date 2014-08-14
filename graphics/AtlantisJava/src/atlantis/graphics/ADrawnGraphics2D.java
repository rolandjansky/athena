package atlantis.graphics;

import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.geom.PathIterator;
import java.util.ArrayList;
import java.util.HashMap;

import atlantis.data.ACalorimeterData;
import atlantis.event.AData;
import atlantis.event.AEvent;
import atlantis.event.AEventManager;
import atlantis.data.AHistogram;
import atlantis.list.AList;
import atlantis.list.AListManager;
import atlantis.utils.ALogger;

/**
 * Simple graphics context simply decides whether something was drawn or not
 */

public class ADrawnGraphics2D extends ATemplateGraphics2D
{
    private static ALogger logger = ALogger.getLogger(ADrawnGraphics2D.class);
    private static AEventManager eventManager = AEventManager.instance();
    
    private static HashMap map;
    private static ArrayList histogramList;

    public ADrawnGraphics2D(Rectangle bounds)
    {
        super(bounds);
        map = new HashMap();
        histogramList = new ArrayList();
        AEvent event = eventManager.getCurrentEvent();
        if (event != null)
        {
            AData[] data = event.getData();
            for (int i = 0; i < data.length; ++i)
            {
                map.put(data[i], new boolean[data[i].getNumData()]);
            }
        }
    } // ADrawnGraphics2D() ----------------------------------------------------

    ADrawnGraphics2D(ADrawnGraphics2D a)
    {
        super(a);
    } // ADrawnGraphics2D() ----------------------------------------------------

    public static void updateLastDrawn()
    {
        AListManager.getInstance().clearLastDrawn();
        AEvent event = eventManager.getCurrentEvent();
        if (event != null)
        {
            AData[] data = event.getData();
            for (int i = 0; i < data.length; ++i)
            {
                boolean[] drawn = (boolean[]) map.get(data[i]);
                if (data[i] instanceof ACalorimeterData)
                {
                    drawn = ((ACalorimeterData) data[i]).expandDrawn(drawn);
                }
                AListManager.getInstance().addLastDrawn(new AList(data[i], drawn));
            }
            for (int i = 0; i < histogramList.size(); ++i)
            {
                boolean[] drawn = (boolean[]) map.get(histogramList.get(i));
                AListManager.getInstance().addLastDrawn(new AList((AData)histogramList.get(i), drawn));
            }
        }
    } // updateLastDrawn() -----------------------------------------------------

    public Graphics create()
    {
        return new ADrawnGraphics2D(this);
    }

    public void drawLine(double h0, double v0, double h1, double v1)
    {
        if (AGraphics.currentData != null)
        {
                ((boolean[]) map.get(AGraphics.currentData))[AGraphics.currentIndex] = true;
        }
    }

    public void fillRect(int h, int v, int width, int height)
    {
        double dh = width / 2.;
        double dv = height / 2.;
        drawLine(h - dh, v - dv, h + dh, v - dv);
        drawLine(h + dh, v - dv, h + dh, v + dv);
        drawLine(h + dh, v + dv, h - dh, v + dv);
        drawLine(h - dh, v + dv, h - dh, v - dv);
    }

    public void draw(Shape s)
    {
        PathIterator path = s.getPathIterator(null);
        double[] coords = new double[6];
        double h0 = 0.;
        double v0 = 0.;
        double h1 = 0.;
        double v1 = 0.;
        while (!path.isDone())
        {
            int segType = path.currentSegment(coords);

            switch (segType)
            {
                case PathIterator.SEG_MOVETO:
                    h0 = coords[0];
                    v0 = coords[1];
                    break;

                case PathIterator.SEG_LINETO:
                case PathIterator.SEG_CUBICTO:
                    h1 = coords[0];
                    v1 = coords[1];
                    drawLine(h0, v0, h1, v1);
                    h0 = h1;
                    v0 = v1;
                    break;

                case PathIterator.SEG_CLOSE:
                    break;

                default:
                    logger.error(" Error unknown segment type");
                    break;
            }
            path.next();
        }
    }

    public void fill(Shape s)
    {
        draw(s);
    }

    public void addHistogramData(AHistogram data)
    {
        histogramList.add(data);
        map.put(data, new boolean[data.getNumData()]);
    }

}
