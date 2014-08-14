package atlantis.graphics;

import java.awt.Graphics;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.PathIterator;
import java.util.ArrayList;

import javax.swing.JPopupMenu;

import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.event.AData;
import atlantis.data.ATrackData;
import atlantis.data.ATrackResidualData;
import atlantis.list.AList;
import atlantis.output.ALogInterface;
import atlantis.output.AOutput;
import atlantis.projection.AProjectionTrackResidual;
import atlantis.projection.AProjectionsManager;
import atlantis.utils.ALogger;

/**
 * A very limited PostScript graphics context for output of Atlantis functions
 * Any call to a none implemented will throw an exception
 */

public class APickingGraphics2D extends ATemplateGraphics2D
{
    private static ALogger logger = ALogger.getLogger(APickingGraphics2D.class);
    
    public final static int PICK_HITS_AND_TRACKS = 0;
    public final static int PICK_DETECTORS = 1;
    public final static int MOVE = 2;
    public final static int PICK_RESIDUAL = 3;

    public static int mode;

    static AData pickedData = null;
    static ATrackResidualData pickedResidual = null;
    static ArrayList pickedDetectors;
    static int pickedIndex;

    public static Point pickedPoint;
    static double minDist;
    public static double pickedH;
    public static double pickedV;

    private static JPopupMenu residualPopupMenu;
    private static final String SHOW_RESIDUALS_X = "Show Residual X";
    private static final String SHOW_PULLS_X = "Show Pull X";

    static
    {
        residualPopupMenu = new JPopupMenu();
        ActionListener residualPopupActionListener =
            new APickingGraphics2D.ResidualPopupMenuActionListener();
        residualPopupMenu.add(SHOW_RESIDUALS_X).addActionListener(residualPopupActionListener);
        residualPopupMenu.add(SHOW_PULLS_X).addActionListener(residualPopupActionListener);
    }

    static class ResidualPopupMenuActionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            String action = e.getActionCommand();

            // Use window "B" to show track residual
            String residualWindowName = "B";
            if(ACanvas.getCanvas().isValidWindowName(residualWindowName))
            {
                AWindow residualWindow = ACanvas.getCanvas().getWindow(residualWindowName);
                AProjectionTrackResidual residualProj =
                    (AProjectionTrackResidual) AProjectionsManager.getProjection("TrackResidual");
                if (action.equals(SHOW_RESIDUALS_X))
                    residualProj.setResidualType(ATrackResidualData.RESIDUAL_X);
                else if (action.equals(SHOW_PULLS_X))
                    residualProj.setResidualType(ATrackResidualData.PULL_X);

                // when this action is activated, the pickedData must be a track
                ATrackData trackData = (ATrackData) pickedData;
                residualProj.setResidual(trackData.getTrackResidual(pickedIndex));
                ACanvas.getCanvas().moveToFrontWindow(residualWindowName);
                residualWindow.setProjection(residualProj);
            }
            else
                AOutput.append("\nWindow " + residualWindowName + " is not available in current layout"
                        + "\n", ALogInterface.WARNING);
        }
    }

    public APickingGraphics2D(Rectangle bounds, int m, Point p)
    {
        super(bounds);
        mode = m;
        pickedPoint = p;
        minDist = 10000000.;
        if (mode != MOVE)
        {
            pickedData = null;
            pickedIndex = -1;
        }
        if (pickedDetectors == null)
        {
            pickedDetectors = new ArrayList();
        }
        else
        {
            pickedDetectors.clear();
        }
    }

    APickingGraphics2D(APickingGraphics2D a)
    {
        super(a);
    }

    public Graphics create()
    {
        return new APickingGraphics2D(this);
    }

    public void drawLine(double h0, double v0, double h1, double v1)
    {
        if (mode == PICK_DETECTORS)
            return;
        if (AGraphics.currentData == null)
            return;
        if (mode == MOVE &&
                !(AGraphics.currentData == pickedData && AGraphics.currentIndex == pickedIndex))
            return;
        double hPicked = pickedPoint.getX();
        double vPicked = pickedPoint.getY();
        // treat first two endpoints
        double delH = h0 - hPicked;
        double delV = v0 - vPicked;
        double dist = delH * delH + delV * delV;

        if (dist < minDist)
        {
            minDist = dist;
            setPicked(h0, v0);
        }
        delH = h1 - hPicked;
        delV = v1 - vPicked;
        dist = delH * delH + delV * delV;
        if (dist < minDist)
        {
            minDist = dist;
            setPicked(h1, v1);
        }
        // now treat point within line being closest
        double h10 = h1 - h0;
        double v10 = v1 - v0;
        double hp0 = hPicked - h0;
        double vp0 = vPicked - v0;
        double size = h10 * h10 + v10 * v10;

        if (size > 0.0)
        {
            double u = (h10 * hp0 + v10 * vp0) / size;

            if (u > 0. && u < 1.)
            {
                double hClosest = h0 + u * h10;
                double vClosest = v0 + u * v10;

                delH = hClosest - hPicked;
                delV = vClosest - vPicked;
                dist = delH * delH + delV * delV;
                if (dist < minDist)
                {
                    minDist = dist;
                    setPicked(hClosest, vClosest);
                }
            }
        }
    }

    public void fillRect(int h, int v, int width, int height)
    {
        if (mode == PICK_DETECTORS && AGraphics.currentDetector != null)
        {
            // Collect the different detector names.
            String info = AGraphics.currentDetector.getInfo(AGraphics.currentIndex);
            if (!pickedDetectors.contains(info))
            {
                pickedDetectors.add(info);
            }
        }
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
        double hInit = 0.;
        double vInit = 0.;
        while (!path.isDone())
        {
            int segType = path.currentSegment(coords);

            switch (segType)
            {
                case PathIterator.SEG_MOVETO:
                    h0 = coords[0];
                    v0 = coords[1];
                    hInit = h0;
                    vInit = v0;
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
                    drawLine(h0, v0, hInit, vInit);
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
        if (mode == PICK_DETECTORS && AGraphics.currentDetector != null)
        {
            double[][] hv = getPolygon(s);
            if (AGraphics.isPointInside(hv[0], hv[1], hv[0].length, pickedPoint.getX(), pickedPoint.getY()))
            {
                // Collect the different detector names.
                String info = AGraphics.currentDetector.getInfo(AGraphics.currentIndex);
                if (!pickedDetectors.contains(info))
                {
                    pickedDetectors.add(info);
                }
            }
        }
        else
        {
            draw(s);
        }
    }

    public double[][] getPolygon(Shape s)
    {
        double[] coords = new double[6];

        int numPoints = 0;
        PathIterator path = s.getPathIterator(null);
        while (!path.isDone())
        {
            int segType = path.currentSegment(coords);
            switch (segType)
            {
                case PathIterator.SEG_MOVETO:
                    numPoints++;
                    break;

                case PathIterator.SEG_LINETO:
                case PathIterator.SEG_CUBICTO:
                    numPoints++;
                    break;

                case PathIterator.SEG_CLOSE:
                    // numPoints++;
                    break;

                default:
                    break;
            }
            path.next();
        }
        double[][] hv = new double[2][numPoints];
        numPoints = 0;
        path = s.getPathIterator(null);
        while (!path.isDone())
        {
            int segType = path.currentSegment(coords);

            switch (segType)
            {
                case PathIterator.SEG_MOVETO:
                    hv[0][numPoints] = coords[0];
                    hv[1][numPoints] = coords[1];
                    numPoints++;
                    break;

                case PathIterator.SEG_LINETO:
                case PathIterator.SEG_CUBICTO:
                    hv[0][numPoints] = coords[0];
                    hv[1][numPoints] = coords[1];
                    numPoints++;
                    break;

                case PathIterator.SEG_CLOSE:
                    // drawLine(h0,v0,hInit,vInit);
                    break;

                default:
                    logger.error(" Error unknown segment type");
                    break;
            }
            path.next();
        }
        return hv;
    }

    private static void setPicked(double h, double v)
    {
        pickedData = AGraphics.currentData;
        pickedIndex = AGraphics.currentIndex;
        pickedH = h;
        pickedV = v;
    }

    public static AData getPickedData()
    {
        return pickedData;

    } // getPickedData()

    public static ATrackResidualData getPickedResidual()
    {
        return pickedResidual;
    }

    public static int getPickedH()
    {
        return (int) Math.rint(pickedH);
    }

    public static int getPickedV()
    {
        return (int) Math.rint(pickedV);
    }

    public static AList getPickedHit()
    {
        if (pickedData != null)
            return new AList(pickedData, pickedIndex);
        else
            return null;
    }

    public static Integer getPickedHitIndex()
    {
        if (pickedData != null)
        {
            return new Integer(pickedIndex);
        }
        else
        {
            return null;
        }
    }

    public static void setPicked(AData data, int index)
    {
        pickedData = data;
        pickedIndex = index;
    }

    public static void setPicked(ATrackResidualData data)
    {
        pickedResidual = data;
    }

    public static void printPicked()
    {
        if(mode == APickingGraphics2D.PICK_RESIDUAL)
        {
            AOutput.alwaysAppend("\n\n" + pickedResidual.getHitInfo(), ALogInterface.PICK);
            mode = APickingGraphics2D.PICK_HITS_AND_TRACKS;
        }
        else
        {
            if (pickedData != null)
            {
                AOutput.alwaysAppend("\n\n" + pickedData.getHitInfo(pickedIndex), ALogInterface.PICK);
            }
            else if (pickedDetectors != null && pickedDetectors.size() > 0)
            {
                AOutput.alwaysAppend("\n\nYou are inside:\n", ALogInterface.PICK);
                for (int i = 0; i < pickedDetectors.size(); i++)
                {
                    AOutput.alwaysAppend(" " + pickedDetectors.get(i) + "\n", ALogInterface.PICK);
                }
            }
        }
    }

    /**
     * Method is called after pick+n (navigate = show data associations)
     */
    public static void navigatePicked()
    {
        if(pickedData != null)
        {
            String navigateInfo = pickedData.navigate(pickedIndex);
            AOutput.alwaysAppend("\n" + navigateInfo + "\n", ALogInterface.PICK);
        }
    }

    public static void showTrackResiduals(AWindow window, int x, int y)
    {
        APickingGraphics2D.residualPopupMenu.show(window, x, y);
    }

}
