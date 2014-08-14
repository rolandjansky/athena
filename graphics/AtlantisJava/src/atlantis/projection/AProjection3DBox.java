package atlantis.projection;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.geom.Point2D;
import java.util.LinkedList;

import javax.swing.JMenuItem;

import Jama.Matrix;
import atlantis.canvas.ACanvas;
import atlantis.canvas.AWindow;
import atlantis.data.A3DPointData;
import atlantis.data.ADHelix;
import atlantis.event.AEvent;
import atlantis.data.ATrackData;
import atlantis.graphics.ACoord;
import atlantis.graphics.ADrawParameters;
import atlantis.graphics.ADrawable;
import atlantis.graphics.AGraphics;
import atlantis.graphics.colormap.AColorMap;

public class AProjection3DBox extends AProjection
{
    // angle of the rotation about the axis
    static private double phi;
    static private double sinPhi;
    static private double cosPhi;

    static private double scale;
    static private double zP;
    static private double xz;
    static private double yz;
    
    static private AVec3D centerDisplay;
    static private AVec3D sizeDisplay;
    
    static private AVec3D sizeUser = new AVec3D(1.1, .5, .5);
    
    static final String RESTORE_DEFAULTS = "Default Box Volume";
    
    static private LinkedList<AVec3D[]> planes = new LinkedList<AVec3D[]>();
    
    static private JMenuItem[] popupItems;
    
    public AProjection3DBox()
    {
        popupItems = new JMenuItem[1];
        popupItems[0] = new JMenuItem(RESTORE_DEFAULTS);
        popupItems[0].addActionListener(this);
    }
    
    public String getName()
    {
        return "3DBox";
    }
    
    public String getScreenName()
    {
        return "3DBox";
    }
    
    public String getXLabel()
    {
        return "V";
    }
    
    public String getYLabel()
    {
        return "H";
    }
    
    public String getXUnits()
    {
        return "(cm)";
    }
    
    public String getYUnits()
    {
        return "(cm)";
    }
    
    public void setScales()
    {}
    
    public boolean processLocalCommand(String name)
    {
        return false;
    }
    
    public Point2D.Double getCenter(AWindow window)
    {
        updateParameters(window);
        return new Point2D.Double(centerDisplay.x, centerDisplay.y);
    }
    
    public void zoom(double factor)
    {
        if (factor != 0.)
        {
            sizeUser = sizeUser.scale(factor);
            updateScale();
        }
    }
    
    public static void setSizeUser(double size)
    {
        if (size != 0.)
        {
            sizeUser.x = size;
            // updateScale();
            // ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
        }
    }
    
    public void zoomHorizontal(double factor)
    {
        if (factor != 0.)
        {
            sizeUser.x *= factor;
            updateScale();
        }
    }
    
    public void zoomVertical(double factor)
    {
        if (factor != 0. && !Double.isInfinite(factor))
        {
            sizeUser.y *= factor;
            sizeUser.z *= factor;
            updateScale();
        }
    }
    
    public static void updateScale()
    {
        scale = parameterStore.get("3DBox", "Scale").getD();
        ACanvas.getCanvas().getCurrentWindow().setUserCorners(0, 2. * sizeUser.x / scale, -sizeUser.y / scale, sizeUser.y / scale);
    }
    
    public static void updateScale(AWindow window)
    {
        scale = parameterStore.get("3DBox", "Scale").getD();
        window.setUserCorners(0, 2. * sizeUser.x / scale, -sizeUser.y / scale, sizeUser.y / scale);
    }
    
    public static double getPhi()
    {
        return parameterStore.get("3DBox", "Phi").getD();
    }
    
    public static void updateParameters(AWindow window)
    {
        phi = Math.toRadians(getPhi());
        sinPhi = Math.sin(phi);
        cosPhi = Math.cos(phi);
        scale = parameterStore.get("3DBox", "Scale").getD();
        zP = parameterStore.get("3DBox", "ZP").getD();
        xz = parameterStore.get("3DBox", "xz").getD();
        yz = parameterStore.get("3DBox", "yz").getD();
        
        Rectangle bounds = window.getCurrDisp();
        centerDisplay = new AVec3D(bounds.x + bounds.width / 2, bounds.y + bounds.height / 2, 0.);
        sizeDisplay = new AVec3D(0.5 * bounds.width * scale, 0.5 * bounds.height * scale, 0.5 * bounds.height * scale);
        centerDisplay.x -= sizeDisplay.x;
        updateScale(window);
    }
    
    protected void drawBox(AGraphics ag)
    {
        double x = sizeDisplay.x;
        double y = sizeDisplay.y;
        double z = sizeDisplay.z;
        
        AVec3D[] firstPlane = new AVec3D[] { new AVec3D(0, y, z), new AVec3D(0, y, -z), new AVec3D(0, -y, -z), new AVec3D(0, -y, z) };
        AVec3D[] middlePlane = new AVec3D[] { new AVec3D(x, y, z), new AVec3D(x, y, -z), new AVec3D(x, -y, -z), new AVec3D(x, -y, z) };
        AVec3D[] lastPlane = new AVec3D[] { new AVec3D(2 * x, y, z), new AVec3D(2 * x, y, -z), new AVec3D(2 * x, -y, -z), new AVec3D(2 * x, -y, z) };
        
        planes = new LinkedList<AVec3D[]>();
        
        planes.add(firstPlane);
        planes.add(middlePlane);
        planes.add(lastPlane);
        
        for (int i = 0; i < planes.size(); i++)
        {
            AVec3D[] plane = (AVec3D[]) (planes.get(i));
            for (int j = 0; j < plane.length; ++j)
                plane[j] = applyViewPoint(plane[j]);
        }
        
        double[] h = new double[4];
        double[] v = new double[4];
        
        // back
        AVec3D[] back = new AVec3D[] { firstPlane[3], firstPlane[1], lastPlane[1], lastPlane[3] };
        for (int j = 0; j < back.length; ++j)
        {
            h[j] = back[j].x;
            v[j] = back[j].y;
        }
        
        ag.updateDrawParameters(new ADrawParameters(true, 0, 0, 1, 0, 0));
        ag.setColor(Color.gray);
        ag.fillPolygon(h, v, 4);
        ag.setColor(Color.white);
        ag.drawPolygon(h, v, 4);
        // base
        AVec3D[] base = new AVec3D[] { firstPlane[0], firstPlane[1], lastPlane[1], lastPlane[0] };
        for (int j = 0; j < base.length; ++j)
        {
            h[j] = base[j].x;
            v[j] = base[j].y;
        }
        ag.setColor(Color.black);
        ag.fillPolygon(h, v, 4);
        ag.setColor(Color.white);
        ag.drawPolygon(h, v, 4);
        
        for (int i = 0; i < planes.size(); i++)
        {
            AVec3D[] plane = (AVec3D[]) (planes.get(i));
            for (int j = 0; j < plane.length; ++j)
            {
                h[j] = plane[j].x;
                v[j] = plane[j].y;
            }
            ag.setColor(Color.black);
            ag.fillPolygon(h, v, 4);
            ag.setColor(Color.white);
            ag.drawPolygon(h, v, 4);
        }
        
        // top
        AVec3D[] top = new AVec3D[] { firstPlane[3], firstPlane[2], lastPlane[2], lastPlane[3] };
        for (int j = 0; j < top.length; ++j)
        {
            h[j] = top[j].x;
            v[j] = top[j].y;
        }
        ag.setColor(Color.white);
        ag.drawPolygon(h, v, 4);
    }
    
    protected void touchupBox(AGraphics ag)
    {
        ag.updateDrawParameters(new ADrawParameters(true, 0, 0, 1, 0, 0));
        ag.setColor(Color.white);
        for (int i = 0; i < planes.size(); i++)
        {
            AVec3D[] plane = (AVec3D[]) (planes.get(i));
            ag.drawLine(plane[1].x, plane[1].y, plane[2].x, plane[2].y);
        }
    }
    
    AVec3D scale(AVec3D v)
    {
        return v.mult(sizeDisplay).divide(sizeUser);
    }
    
    AVec3D rotate(AVec3D v)
    {
        return new AVec3D(v.x, v.y * cosPhi - v.z * sinPhi, v.y * sinPhi + v.z * cosPhi);
    }
    
    static AVec3D applyViewPoint(AVec3D v)
    {
        return new AVec3D((v.x + xz * v.z) * (1 - v.z * zP) + centerDisplay.x, (v.y + yz * v.z) * (1 - v.z * zP) + centerDisplay.y, v.z);
    }
    
    public void paint(AWindow window, Graphics g)
    {
        updateParameters(window);
        AGraphics ag = AGraphics.makeAGraphics(g);
        fillBackground(window, ag);
        drawBox(ag);
        AEvent event = eventManager.getCurrentEvent();
        if (event == null)
            return;
        
        java.util.List hitsAndTracks = event.getHitsAndTracks(this);
        
        for (int h = 0; h < hitsAndTracks.size(); h++)
        {
            Object o = hitsAndTracks.get(h);
            if (o instanceof ATrackData)
                drawTracks((ATrackData) o, window, ag);
            else if (o instanceof A3DPointData)
                drawPoints((A3DPointData) o, window, ag);
        }
        touchupBox(ag);
    }
    
    protected void drawPoints(A3DPointData pointData, AWindow window, AGraphics ag)
    {
        double[][] points = pointData.get3DPoints();
        int[] index = pointData.get3DPointsIndex();
        
        double[][][] hv = new double[2][1][index.length];
        
        for (int i = 0; i < points.length; ++i)
        {
            AVec3D point = rotate(new AVec3D(AProjection3D.getRotated(points[i])));
            
            if (point.x > 0 && point.x < 2 * sizeUser.x && point.y > -sizeUser.y && point.y < sizeUser.y && point.z > -sizeUser.z && point.z < sizeUser.z)
            {
                point = applyViewPoint(scale(point));
                hv[0][0][i] = point.x;
                hv[1][0][i] = point.y;
            }
            else
            {
                // aaargh allow this to be clipped from picture for now
                hv[0][0][i] = -1000.;
                hv[1][0][i] = -1000.;
            }
        }
        
        ag.draw(new ACoord(hv, index, pointData, ACoord.SYMBOLS));
    }
    
    protected void drawTracks(ATrackData tracks, AWindow window, AGraphics ag)
    {
        ADHelix[] dhelix = tracks.getHelices();

        int numPlanes = planes.size();
        int numSegments = numPlanes + 4;
        AVec3D[][] intersection = new AVec3D[dhelix.length][numPlanes];
        
        double[][][] hv = new double[2][dhelix.length * numSegments][0];
        int[] index = new int[dhelix.length * numSegments];
        int[] indexTemp = tracks.getDrawList();
        for (int i = 0; i < indexTemp.length; ++i)
            for (int j = 0; j < numSegments; ++j)
                index[i * numSegments + j] = indexTemp[i];
        
        int numPointsOnEllipse = 24;
        AVec3D[] ellipse = new AVec3D[numPointsOnEllipse + 1];
        double radius = 0.005;
        for (int i = 0; i < ellipse.length; i++)
        {
            double phi = 2. * Math.PI * i / numPointsOnEllipse;
            ellipse[i] = new AVec3D(0., radius * Math.sin(phi), radius * Math.cos(phi));
        }
        
        for (int j = 0; j < dhelix.length; ++j)
            if (dhelix[j] != null)
            {
                // s always 0? I don't know why that is...
                double s = 0.;
                AVec3D start = new AVec3D(dhelix[j].get3DPointAsArray(s));
                AVec3D end = new AVec3D(dhelix[j].get3DPointAsArray(s + 0.01));
                
                boolean backwards = false;
                
                // calculate intersections
                for (int p = 0; p < planes.size(); ++p)
                {
                    // position of plane
                    double xInt = p * sizeUser.x;
                    double m = (xInt - start.x) / (end.x - start.x);
                    
                    // While we're in here, can check if we're pointing in the right direction
                    if (p == 1 && m < 0.0) { backwards = true; }
                    
                    double yInt = start.y + m * (end.y - start.y);
                    double zInt = start.z + m * (end.z - start.z);
                    intersection[j][p] = new AVec3D(xInt, yInt, zInt);
                }
                
                // Throw out anything that isn't pointing in the right direction...
                if (backwards) { continue; }
                
                AVec3D first = rotate(intersection[j][0]);
                AVec3D middle = rotate(intersection[j][1]);
                AVec3D last = rotate(intersection[j][numPlanes - 1]);
                
                if (first.y > -sizeUser.y && first.y < sizeUser.y && first.z > -sizeUser.z && first.z < sizeUser.z && last.y > -sizeUser.y && last.y < sizeUser.y && last.z > -sizeUser.z && last.z < sizeUser.z)
                {
                    
                    double[][] cov = dhelix[j].helix.getCovariance();
                    if (cov != null)
                    {
                        double[][] c = new double[2][2];
                        c[0][0] = cov[0][0];
                        c[0][1] = cov[0][1];
                        c[1][0] = cov[1][0];
                        c[1][1] = cov[1][1];
                        Matrix covM = new Matrix(c);
                        Matrix errM = covM.inverse();
                        
                        double b1 = errM.get(0, 0);
                        double b2 = errM.get(1, 1);
                        
                        double b3 = 2. * errM.get(0, 1);
                        if (parameterStore.get("3DBox", "ellipses").getStatus())
                        {
                            ellipse = new AVec3D[180];
                            
                            for (int i = 0; i < 360; i += 2)
                            {
                                double thetat = Math.toRadians(i);
                                double tt = Math.tan(thetat);
                                double xx = Math.sqrt(1. / (b1 + b2 * tt * tt + b3 * tt));
                                if (i > 90 && i <= 270)
                                    xx *= -1.;
                                ellipse[i / 2] = new AVec3D(0., xx, xx * tt);
                            }
                        }
                        // add intersections to draw list
                        for (int p = 0; p < planes.size(); ++p)
                        {
                            double[] h = new double[ellipse.length];
                            double[] v = new double[ellipse.length];
                            
                            for (int i = 0; i < ellipse.length; i++)
                            {
                                AVec3D point = applyViewPoint(scale(rotate(intersection[j][p].add(ellipse[i]))));
                                h[i] = point.x;
                                v[i] = point.y;
                            }
                            
                            hv[0][numSegments * j + p] = h;
                            hv[1][numSegments * j + p] = v;
                        }
                    }
                    first = applyViewPoint(scale(first));
                    middle = applyViewPoint(scale(middle));
                    last = applyViewPoint(scale(last));
                    
                    double[][] edge = new double[planes.size()][2];
                    for (int i = 0; i < planes.size(); i++)
                    {
                        AVec3D[] plane = (AVec3D[]) (planes.get(i));
                        double xInt = plane[1].x;
                        double m = (xInt - first.x) / (last.x - first.x);
                        double yInt = first.y + m * (last.y - first.y);
                        edge[i] = new double[] { xInt, yInt };
                    }
                    
                    hv[0][numSegments * j + 3] = new double[] { first.x, edge[1][0] };
                    hv[1][numSegments * j + 3] = new double[] { first.y, edge[1][1] };
                    hv[0][numSegments * j + 4] = new double[] { middle.x, edge[2][0] };
                    hv[1][numSegments * j + 4] = new double[] { middle.y, edge[2][1] };
                    
                    drawDashed(ag, tracks, indexTemp[j], edge[1][0], edge[1][1], middle.x, middle.y);
                    drawDashed(ag, tracks, indexTemp[j], edge[2][0], edge[2][1], last.x, last.y);
                }
            }
        ag.draw(new ACoord(hv, index, tracks, ACoord.POLYLINES));
    }
    
    protected void drawDashed(AGraphics ag, ADrawable source, int ind, double h0, double v0, double h1, double v1)
    {
        double dH = h1 - h0;
        double dV = v1 - v0;
        double dist = Math.sqrt(dH * dH + dV * dV);
        double dashSize = 4;
        int num = (int) (dist / dashSize);
        int[] index = new int[num / 2 + 1];
        double[][][] hv = new double[2][num / 2 + 1][2];
        
        for (int i = 0; i < num; ++i)
        {
            if (i % 2 == 1)
            {
                int n = i / 2;
                index[n] = ind;
                hv[0][n][0] = h0 + dH * i / num;
                hv[0][n][1] = h0 + dH * (i + 1) / num;
                hv[1][n][0] = v0 + dV * i / num;
                hv[1][n][1] = v0 + dV * (i + 1) / num;
                if (i >= num - 2)
                {
                    hv[0][n][1] = h1;
                    hv[1][n][1] = v1;
                }
            }
        }
        ag.draw(new ACoord(hv, index, source, ACoord.POLYLINES));
    }
    
protected void fillBackground(AWindow window, AGraphics ag)
    {
        Color[] colorMap = AColorMap.getColors();
        ag.setColor(colorMap[getBackgroundFillColor()]);
        ag.fillRect(0, 0, window.getWidth(), window.getHeight());
    }
    
    protected int getBackgroundFillColor()
    {
        return parameterStore.get("Color", "BkgFill").getI();
    }
    
    public JMenuItem[] getPopupItems()
    {
        return popupItems;
    }
    
    public void actionPerformed(ActionEvent e)
    {
        String action = e.getActionCommand();
        if (action.equals(RESTORE_DEFAULTS))
        {
            sizeUser.x = 2.;
            sizeUser.y = .1;
            sizeUser.z = .1;
            updateScale();
            ACanvas.getCanvas().getCurrentWindow().repaintFromScratch();
        }
    }
    
}

class AVec3D
{
    double x, y, z;
    
    AVec3D(double x, double y, double z)
    {
        this.x = x;
        this.y = y;
        this.z = z;
    }
    
    AVec3D(double[] v)
    {
        this.x = v[0];
        this.y = v[1];
        this.z = v[2];
    }
    
    AVec3D cross(AVec3D d)
    {
        return new AVec3D(+this.y * d.z - this.z * d.y, -this.x * d.z + this.z * d.x, +this.x * d.y - this.y * d.x);
    }
    
    AVec3D add(AVec3D d)
    {
        return new AVec3D(x + d.x, y + d.y, z + d.z);
    }
    
    AVec3D mult(AVec3D d)
    {
        return new AVec3D(x * d.x, y * d.y, z * d.z);
    }
    
    AVec3D divide(AVec3D d)
    {
        return new AVec3D(x / d.x, y / d.y, z / d.z);
    }
    
    AVec3D scale(double s)
    {
        return new AVec3D(s * x, s * y, s * z);
    }
    
    public String toString()
    {
        return x + " " + y + " " + z;
    }
    
    public double mag2() {
        return x*x + y*y + z*z;
    }
    
}
