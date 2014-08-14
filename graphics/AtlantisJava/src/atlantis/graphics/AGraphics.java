package atlantis.graphics;

import atlantis.canvas.AGLGraphics;
import java.awt.Color;
import java.awt.AlphaComposite;
import java.awt.Composite;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.Image;
import java.awt.Stroke;

import atlantis.event.AData;
import atlantis.geometry.ADetectors;
import atlantis.graphics.colormap.AColorMap;
import atlantis.parameters.APar;
import atlantis.utils.APolygon;

import org.sourceforge.jlibeps.epsgraphics.*;
import org.apache.batik.svggen.SVGGraphics2D;

/**
 * Normally all drawing should be done on an AGraphics. For special uses, e.g.
 * Braintest projections, standard Java Graphics may be used. To allow this,
 * projections are always given a graphics which they must convert to an
 * AGraphics with AGraphics.makeAGraphics. AGraphics provides picking of drawn
 * objects.
 */

public abstract class AGraphics
{
    protected Graphics2D g;
    AClipper clipper = null;
    Color currentColor;
    Color lastDrawnColor = null;
    Rectangle bounds;
    int lineWidth = 1;
    int size;
    int symbol;
    ADrawParameters lastDrawParameters = null;

    static AData currentData = null;
    static ADetectors currentDetector = null;
    static int currentIndex;

    public static final int SYMBOL_FILLED_BOX = 0;
    public static final int SYMBOL_HORIZONTAL_LINE = 1;
    public static final int SYMBOL_VERTICAL_LINE = 2;
    public static final int SYMBOL_PLUS = 3;
    public static final int SYMBOL_FILLED_CIRCLE = 4;

    public static final int DRAW = 0;
    public static final int FILL = 1;

    protected static final int NUM_LAYERS = 2;

    protected static final int FULLY_INSIDE = 0;
    protected static final int FULLY_OUTSIDE = 1;
    protected static final int CONTAINS = 2;
    protected static final int INTERSECTS = 3;

    protected static final int POLYGON = 0;
    protected static final int POLYLINE = 1;

    AGraphics(Graphics g)
    {
        this.g = (Graphics2D) g;
        if (g.getClipBounds() != null)
            clipper = new AClipper(g.getClipBounds());
        currentColor = g.getColor();
    }

    public Graphics2D getGraphics2D()
    {
        return g;
    }

    public static AGraphics makeAGraphics(Graphics g)
    {
        if (g instanceof EpsGraphics2D || g instanceof APickingGraphics2D ||
            g instanceof ADrawnGraphics2D || g instanceof SVGGraphics2D)
        {
            return new AVectorGraphics(g);
        }
        else if (g instanceof AGLGraphics)
        {
            return new AGLPixelGraphics((AGLGraphics)g);
        }
        else
        {
            return new APixelGraphics(g);
        }
    }

    public static void setCurrentDataAndIndex(AData current, int index)
    {
        currentData = current;
        currentIndex = index;
    }

    public static void setCurrentDetectorAndIndex(ADetectors current, int index)
    {
        currentDetector = current;
        currentIndex = index;
    }

    public static void clearCurrentDetectorAndIndex()
    {
        currentDetector = null;
        currentIndex = -1;
    }

    public static void clearCurrentDataAndIndex()
    {
        currentData = null;
        currentIndex = -1;
    }

    public void setFont(Font f)
    {
        g.setFont(f);
    }

    public void setStroke(Stroke s)
    {
        g.setStroke(s);
    }

    public void setColor(Color c)
    {
        currentColor = c;
    }

    public void updateColor()
    {
        if (currentColor != lastDrawnColor)
        {
            g.setColor(currentColor);
            lastDrawnColor = currentColor;
        }
    }

    public abstract void setLineWidth(int lineWidth);

    public void updateDrawParameters(ADrawParameters dp)
    {
        if (dp != lastDrawParameters)
        {
            setColor(dp.color);
            lineWidth = Math.max(dp.lineWidth, 1) + 2 * dp.frameWidth;
            setLineWidth(lineWidth);
            size = dp.size + 2 * dp.frameWidth;
            symbol = dp.symbol;
            lastDrawParameters = dp;
        }
    }

    public void drawString(String str, double x, double y)
    {
        g.drawString(str, (int) x, (int) y);
    }

    public void draw(Shape s)
    {
        g.draw(s);
    }

    public void drawPoint(AData detector, int index, double h, double v, int pixelsH, int pixelsV)
    {
        setCurrentDataAndIndex(detector, index);
        drawPoint(h, v, pixelsH, pixelsV);
        clearCurrentDataAndIndex();
    }

    public void drawPoint(double h, double v, int pixelsH, int pixelsV)
    {
        if (clipper.isPointWithin(h, v))
            fillRect(h, v, pixelsH, pixelsV);
    }

    public void drawSymbol(double h, double v)
    {
        if (clipper.isPointWithin(h, v))
        {
            if (symbol == SYMBOL_FILLED_BOX)
            {
                fillRect(h, v, size, size);
            }
            else if (symbol == SYMBOL_HORIZONTAL_LINE)
            {
                fillRect(h, v, size, lineWidth);
            }
            else if (symbol == SYMBOL_VERTICAL_LINE)
            {
                fillRect(h, v, lineWidth, size);
            }
            else if (symbol == SYMBOL_PLUS)
            {
                fillRect(h, v, size, lineWidth);
                fillRect(h, v, lineWidth, size);
            }
            else if (symbol == SYMBOL_FILLED_CIRCLE)
            {
                fillOval(h, v, size, size);
            }
        }
    }

    //Draw an image at postion x,y of this graphics context
    //Introduced for FishEye symbol showing FishEye status
    public void drawImage(Image img, int x, int y){
        //Draw the image on screen 
        g.drawImage(img,x,y,null);    
    }
 
    
    public void fillPolygon(AData detector, int index, double[] h, double[] v, int numPoints)
    {
        setCurrentDataAndIndex(detector, index);
        fillPolygon(h, v, numPoints);
        clearCurrentDataAndIndex();
    }

    public void fillRect(int h, int v, int width, int height)
    {
        updateColor();
        g.fillRect(h, v, width, height);
    }
    
    public void fillOval(int h, int v, int width, int height)
    {
        g.fillOval(h, v, width, height);
    }

    public abstract void drawPolyline(double[] h, double[] v, int numPoints);

    public abstract void drawDottedPolyline(double[] h, double[] v, int numPoints);

    public abstract void drawSmoothPolyline(double[] h0, double[] v0, int numPoints0);

    public abstract void drawLine(double h0, double v0, double h1, double v1);
    
    public abstract void drawPolygon(double[] h, double[] v, int numPoints);

    protected abstract void fillRect(double h, double v, int width, int height);
    
    protected abstract void fillOval(double h, double v, int width, int height);

    public void fillPolygon(ADetectors det, int index, double[] h, double[] v, int numPoints)
    {
        setCurrentDetectorAndIndex(det, index);
        fillPolygon(h, v, numPoints);
        clearCurrentDetectorAndIndex();
    }
    
    public void fillPolygon(double[] h, double[] v, int numPoints, float alpha)
    {
    	Composite originalComposite = g.getComposite();
    	int ACtype = AlphaComposite.SRC_OVER;
        g.setComposite(AlphaComposite.getInstance(ACtype, alpha));
        fillPolygon(h, v, numPoints);
    	g.setComposite(originalComposite);
    }

    protected int getContainmentType(double[] h, double[] v, int numPoints, int shape)
    {
        int within = clipper.fastWithin(h, v, numPoints);
        if (within == AClipper.NONE_WITHIN)
            return FULLY_OUTSIDE;
        if (within == AClipper.ALL_WITHIN)
            return FULLY_INSIDE;
        for (int i = 0; i < numPoints - 1; ++i)
            if (clipper.isLineWithin(h[i], v[i], h[i + 1], v[i + 1]))
                return INTERSECTS;
        if (shape == POLYGON && numPoints > 1)
            if (clipper.isLineWithin(h[numPoints - 1], v[numPoints - 1], h[0], v[0]))
                return INTERSECTS;
        Rectangle bounds = g.getClipBounds();
        double xCentreClipped = bounds.getX() + bounds.getWidth() / 2.;
        double yCentreClipped = bounds.getY() + bounds.getHeight() / 2.;
        if (isPointInside(h, v, numPoints, xCentreClipped, yCentreClipped))
            return CONTAINS;
        return FULLY_OUTSIDE;
    }

    public abstract void fillPolygon(double[] h, double[] v, int numPoints);

    public void drawPolygon(APolygon p)
    {
        drawPolygon(p.getX(), p.getY(), p.nrNodes());
    }
    
    public static boolean isPointInside(double[] h, double[] v, int numPoints, double hP, double vP)
    {
        double[] a = new double[numPoints];

        for (int i = 0; i < numPoints; ++i)
        {
            a[i] = Math.atan2(v[i] - vP, h[i] - hP);
        }
        double[] d = new double[numPoints];

        for (int i = 0; i < numPoints; ++i)
        {
            if (i < numPoints - 1)
                d[i] = a[i + 1] - a[i];
            else
                d[i] = a[0] - a[i];
            if (d[i] > Math.PI)
                d[i] -= 2. * Math.PI;
            else if (d[i] < -Math.PI)
                d[i] += 2. * Math.PI;
        }
        double sum = 0.;

        for (int i = 0; i < numPoints; ++i)
        {
            sum += d[i];
        }
        if (Math.abs(sum) > Math.PI)
            return true;
        else
            return false;
    }

    public void draw(ACoord display)
    {
        ADrawable source = display.source;

        if (source == null)
            return;
        ADrawParameters adp = source.getDrawParameters(0, 0);

        if (adp.getForceSymbols() == true || adp.getMinSize() > 0)
            display.toSymbols(adp.getForceSymbols(), adp.getMinSize());
        Color[] colorMap = AColorMap.getColors();
        int[] index = display.index;
        int[] c = source.getColor(index);
        int[] t = source.getType(index);
        double[][] h = display.hv[0];
        double[][] v = display.hv[1];

        for (int layer = 0; layer < NUM_LAYERS; layer++)
        {
            for (int type = 0; type < source.getNumTypes(); type++)
            {
                ADrawParameters dp = source.getDrawParameters(layer, type);
                int drawType = dp.getDrawOrFill();
                updateDrawParameters(dp);
                if (dp.draw)
                {
                    for (int j = 0; j < t.length; j++)
                    {
                        if (t[j] == type)
                        {
                            if (layer == NUM_LAYERS - 1)
                            {
                                setColor(colorMap[c[j]]);
                                if (source instanceof AData)
                                    setCurrentDataAndIndex((AData) source, index[j]);
                            }
                            if (display.type == ACoord.SYMBOLS)
                                drawSymbol(h[0][j], v[0][j]);
                            else if (display.type == ACoord.POLYGONS && h[j].length == 1)
                                drawSymbol(h[j][0], v[j][0]);
                            else if (display.type == ACoord.LINES)
                                drawLine(h[0][j], v[0][j], h[1][j], v[1][j]);
                            else if (display.type == ACoord.POLYLINES)
                                drawPolyline(h[j], v[j], h[j].length);
                            else if (display.type == ACoord.SMOOTH_POLYLINES)
                                drawSmoothPolyline(h[j], v[j], h[j].length);
                            else if (display.type == ACoord.DOTTED_POLYGONS)
                                drawDottedPolyline(h[j], v[j], h[j].length);
                            else if (display.type == ACoord.POLYGONS)
                            {
                                if (drawType == DRAW)
                                    drawPolygon(h[j], v[j], h[j].length);
                                else if (drawType == FILL)
                                    fillPolygon(h[j], v[j], h[j].length);
                            }
                            else if (display.type == ACoord.TRANSPARENT_POLYGONS)
                            {
                            	if(g instanceof EpsGraphics2D){ 
                            		// setComposite is currently unsupported for EpsGraphics2D
                            		// for now just draw as an outline, so we can still see
                            		// jets and the underlying objects
                            		drawPolygon(h[j], v[j], h[j].length);
                            	}
                            	else {
                            		Composite originalComposite = g.getComposite();
                            		int ACtype = AlphaComposite.SRC_OVER;
                            		g.setComposite(AlphaComposite.getInstance(ACtype, (float)APar.instance().get("Jet","Alpha").getD()));
                            		fillPolygon(h[j], v[j], h[j].length);
                            		g.setComposite(originalComposite);
                            	}
                            }
                            clearCurrentDataAndIndex();
                        }
                    }
                }
            }
        }
    }

}
