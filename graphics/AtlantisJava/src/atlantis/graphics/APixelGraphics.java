package atlantis.graphics;

import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.geom.GeneralPath;

/**
 * This graphics is used for drawing on the screen and making pixel based
 * images. In the end all drawing is done in pixels(integers). Picking is
 * implemented here. Provide high quality pixel based drawing of thin frames
 * around lines and areas, in the method draw.
 */
// When drawing borders could speed things up by only drawing what is needed
public class APixelGraphics extends AGraphics
{
    APixelGraphics(Graphics g)
    {
        super(g);
    }

    public void setLineWidth(int lineWidth)
    {}

    public void drawLine(double h0, double v0, double h1, double v1)
    {
        if (clipper.isLineWithin(h0, v0, h1, v1))
        {
            double[][] hvClipped = clipper.getHV();
            double h0Clipped = hvClipped[0][0];
            double v0Clipped = hvClipped[1][0];
            double h1Clipped = hvClipped[0][1];
            double v1Clipped = hvClipped[1][1];
            if (lineWidth == 1)
            {
                updateColor();
                g.drawLine((int) h0Clipped, (int) v0Clipped, (int) h1Clipped, (int) v1Clipped);
            }
            else
            {
                drawThickLine(h0Clipped, v0Clipped, h1Clipped, v1Clipped);
            }
        }

    } // drawLine()
    
    private void drawThickLine(double h0, double v0, double h1, double v1)
    {
        // Drawing thick line this way is quicker than using setLine width
        // correct for Postscipt
        // pretty good for pixel device (assume java line drawing is Bresenhams
        // or similar)
        double h = Math.abs(h1 - h0);
        double v = Math.abs(v1 - v0);

        for (int i = 0; i < lineWidth; ++i)
        {
            // 101/50 to avoid rounding toward 0 problem
            int d = i + (101 - lineWidth) / 2 - 50;

            int h0new = (int) h0;
            int v0new = (int) v0;
            int h1new = (int) h1;
            int v1new = (int) v1;

            if (h > v)
            {
                v0new += d;
                v1new += d;
            }
            else
            {
                h0new += d;
                h1new += d;
            }

            // Caution!!!
            // When point (h0, v0) or (h1, v1) is close to the border of the
            // drawing area, after +/- d, the (h0new, v0new) or (h1new, v1new) 
            // might beyond the valid value range, which will cause an 
            // IndexArrayOutOfBound error thrown from g.drawLine method.
            if(clipper.isLineWithin(h0new, v0new, h1new, v1new))
            {
                updateColor();
                h0new = (int) clipper.getH0();
                v0new = (int) clipper.getV0();
                h1new = (int) clipper.getH1();
                v1new = (int) clipper.getV1();
                g.drawLine(h0new, v0new, h1new, v1new);
            }
        }
    }

    public void drawSmoothLine(double h0, double v0, double ch0, double cv0,
            double ch1, double cv1, double h1, double v1)
    {        
        GeneralPath curve = new GeneralPath();
        for (int i=0; i<lineWidth; ++i) {
            // 101/50 to avoid rounding toward 0 problem
            int d = i + (101 - lineWidth) / 2 - 50;
            float dh, dv;
            
            if (Math.abs(h1 - h0) > Math.abs(v1 - v0)) {
                dh = 0;
                dv = d;
            } else {
                dh = d;
                dv = 0;
            }
            
            curve.moveTo((float)h0+dh, (float)v0+dv);
            curve.curveTo((float)ch0+dh, (float)cv0+dv, (float)ch1+dh, (float)cv1+dv, (float)h1+dh, (float)v1+dv);
        }
        updateColor();
        g.draw(curve);
    }

    protected void fillRect(double h, double v, int width, int height)
    {
        updateColor();
        g.fillRect((int) (h - width / 2.), (int) (v - height / 2.), width, height);
    }

    @Override
    protected void fillOval(double h, double v, int width, int height) {
        
        g.fillOval((int) (h - width / 2.), (int) (v - height / 2.), width, height);
        g.drawOval((int) (h - width / 2.), (int) (v - height / 2.), width, height);
    }
    
    public void drawPolygon(double[] h, double[] v, int numPoints)
    {
        int type = getContainmentType(h, v, numPoints, POLYGON);

        if (type == FULLY_INSIDE || type == INTERSECTS)
        {
            for (int i = 0; i < numPoints - 1; ++i)
            {
                drawLine(h[i], v[i], h[i + 1], v[i + 1]);
            }
            if (numPoints > 1)
            {
                drawLine(h[numPoints - 1], v[numPoints - 1], h[0], v[0]);
            }
        }
    } // drawPolygon()

    public void drawPolyline(double[] h, double[] v, int numPoints)
    {
        int type = getContainmentType(h, v, numPoints, POLYLINE);
        if (type == FULLY_INSIDE || type == INTERSECTS)
        {
            // Some code here to simplify polylines by only drawing segments
            // at least 1 pixel long, speeds up the ellipses for recvertices
            // quite a lot since they're often very small but have thousands
            // of line segments - AD
            double lasth = 0.0;
            double lastv = 0.0;
            
            for (int i = 0; i < numPoints - 1; ++i) {
                if (i == 0) {
                    // Always draw something
                    lasth = h[1];
                    lastv = v[1];
                    drawLine(h[0], v[0], h[1], v[1]);
                }
               
                double diffh = h[i+1] - lasth;
                double diffv = v[i+1] - lastv;
                
                if ((diffh*diffh + diffv*diffv) > 1.0) {
                    drawLine(lasth, lastv, h[i + 1], v[i + 1]);
                    lasth = h[i+1];
                    lastv = v[i+1];
                }
            }
        }
    } // drawPolyline()

    public void drawDottedPolyline(double[] h, double[] v, int numPoints)
    {
        int type = getContainmentType(h, v, numPoints, POLYLINE);
        if (type == FULLY_INSIDE || type == INTERSECTS)
        {
            for (int i = 0; i < numPoints - 1; i += 2)
                drawLine(h[i], v[i], h[i + 1], v[i + 1]);
        }
    } // drawDottedPolyline()

    public void drawSmoothPolyline(double[] h0, double[] v0, int numPoints0) {
        
        // For curves having less than 3 points and for picking use the normal polyline.
        if (numPoints0 < 3 || g instanceof APickingGraphics2D) {
            drawPolyline(h0, v0, numPoints0);
            return;
        }

        int type = getContainmentType(h0, v0, numPoints0, POLYLINE);
        if (type == FULLY_INSIDE || type == INTERSECTS) {

            int numPoints = 3 * numPoints0 - 2;

            // Add the control points to the array, at 1/3 and 2/3
            // between the already existing points.
            float[] h = new float[numPoints];
            float[] v = new float[numPoints];
            for (int i=0; i<numPoints; i++) {

                switch (i%3) {
                    case 0:
                        // One of the fixed points.
                        h[i] = (float)h0[i/3];
                        v[i] = (float)v0[i/3];
                        break;
                    case 1:
                        // First control point.
                        h[i] = (float)(2./3. * h0[i/3] + 1./3. * h0[i/3+1]);
                        v[i] = (float)(2./3. * v0[i/3] + 1./3. * v0[i/3+1]);
                        break;
                    case 2:
                        // Second control point.
                        h[i] = (float)(1./3. * h0[i/3] + 2./3. * h0[i/3+1]);
                        v[i] = (float)(1./3. * v0[i/3] + 2./3. * v0[i/3+1]);
                        break;
                }
            }

            // Now we have a normal polyline (straight line segments between fixed points),
            // but as a cubic Bezier curve. All we need to do to make it smooth is to move
            // the control points away from the curve, in such a way that [control point 2] -
            // [fixed point] - [control point 1] is a straight line for every fixed point.
            // We do this by averaging the angles of the line segments on either side of a
            // fixed point.
            for (int i=3; i<numPoints-2; i+=3) {
                double lenLeft = Math.sqrt(Math.pow(h[i]-h[i-1], 2) + Math.pow(v[i]-v[i-1], 2));
                double lenRight = Math.sqrt(Math.pow(h[i+1]-h[i], 2) + Math.pow(v[i+1]-v[i], 2));

                // Without length we cannot define an angle, so skip the point.
                if (lenLeft < 1e-6 || lenRight < 1e-6) continue;

                double phiLeft = Math.atan2(v[i]-v[i-1], h[i]-h[i-1]);
                double phiRight = Math.atan2(v[i+1]-v[i], h[i+1]-h[i]);

                if (phiLeft-phiRight > Math.PI) {
                    phiRight += 2.*Math.PI;
                } else if (phiRight-phiLeft > Math.PI) {
                    phiLeft += 2.*Math.PI;
                }

                if (Math.abs(phiLeft-phiRight) > Math.PI/2.) {
                    // The line turns more than 90 degrees, this is wrong. Move the control points
                    // back in so they are the same as the point on the line. This disables the smooth
                    // curve locally, reverting to a normal connect-the-dots polyline.
                    h[i-1] = h[i+1] = h[i];
                    v[i-1] = v[i+1] = v[i];
                } else {
                    // Calculate the desired angle for the tangent by weighting the angle on each side,
                    // the weight is inverse proportional to the distance to the next point.
                    double phi = (lenRight*phiLeft + lenLeft*phiRight) / (lenLeft+lenRight);

                    h[i-1] = h[i] - (float)(lenLeft*Math.cos(phi));
                    v[i-1] = v[i] - (float)(lenLeft*Math.sin(phi));
                    h[i+1] = h[i] + (float)(lenRight*Math.cos(phi));
                    v[i+1] = v[i] + (float)(lenRight*Math.sin(phi));
                }
            }

            for (int i=0; i<numPoints-1; i+=3) {
                drawSmoothLine(h[i], v[i], h[i+1], v[i+1], h[i+2], v[i+2], h[i+3], v[i+3]);
            }
        }
    }
    
    public void fillPolygon(double[] h, double[] v, int numPoints)
    {
        int type = getContainmentType(h, v, numPoints, POLYGON);
        if (type == FULLY_INSIDE || type == INTERSECTS)
        {
            if (type == INTERSECTS)
            {
                ACoord clipped = clipper.clipPolygon(h, v, numPoints);
                h = clipped.hv[0][0];
                v = clipped.hv[1][0];
                numPoints = h.length;
            }
            int[] hInt = new int[numPoints];
            int[] vInt = new int[numPoints];
            for (int i = 0; i < numPoints; ++i)
            {
                hInt[i] = (int) h[i];
                vInt[i] = (int) v[i];
            }
            updateColor();
            g.fillPolygon(hInt, vInt, numPoints);
            drawPolygon(h, v, numPoints);
        }
        else if (type == CONTAINS)
        {
            Rectangle bounds = g.getClipBounds();
            updateColor();
            g.fillRect((int) Math.rint(bounds.getX()), (int) Math.rint(bounds.getY()), (int) Math.rint(bounds.getWidth()), (int) Math.rint(bounds.getHeight()));
        }
    }

}
