package atlantis.graphics;

import java.awt.*;
import java.awt.geom.Ellipse2D;
import java.awt.geom.GeneralPath;

/**
 * Used when drawing to postscript
 */
public class AVectorGraphics extends AGraphics {
  
  private GeneralPath path=new GeneralPath();
  
  AVectorGraphics(Graphics g) {
    super(g);
  }
  
  public void setLineWidth(int lineWidth) {
    setStroke(new BasicStroke(lineWidth));
  }
  
  public void drawLine(int h0, int v0, int h1, int v1) {
    drawLine((double)h0, (double)v0, (double)h1, (double)v1);
  }

  public void drawLine(double h0, double v0, double h1, double v1) {
    if(clipper.isLineWithin(h0, v0, h1, v1)) {
      double[][] hvClipped=clipper.getHV();
      double h0Clipped=hvClipped[0][0];
      double v0Clipped=hvClipped[1][0];
      double h1Clipped=hvClipped[0][1];
      double v1Clipped=hvClipped[1][1];
      
      path.reset();
      path.moveTo((float)h0Clipped, (float)v0Clipped);
      path.lineTo((float)h1Clipped, (float)v1Clipped);
      updateColor();
      g.draw(path);
    }
  }
  
  protected void fillRect(double h, double v, int width, int height) {
    path.reset();
    float h0=(float)h;
    float v0=(float)v;
    float dh=(float)(width/2.);
    float dv=(float)(height/2.);
    
    path.moveTo(h0-dh, v0-dv);
    path.lineTo(h0+dh, v0-dv);
    path.lineTo(h0+dh, v0+dv);
    path.lineTo(h0-dh, v0+dv);
    path.closePath();
    updateColor();
    g.fill(path);
  }
  
  protected void fillOval(double h, double v, int width, int height) {
      
    Ellipse2D oval = new Ellipse2D.Float((float)(h-width/2.), (float)(v-height/2.), width, height);
    updateColor();
    g.fill(oval);
  }
  
  public void drawPolygon(double[] h, double[] v, int numPoints) {
    
    int type=getContainmentType(h, v, numPoints, POLYGON);
    if(type==FULLY_INSIDE||type==INTERSECTS) {
      if(type==INTERSECTS) {
        ACoord clipped=clipper.clipPolygon(h, v, numPoints);
        h=clipped.hv[0][0];
        v=clipped.hv[1][0];
        numPoints=h.length;
      }
      
      path.reset();
      for(int i=0; i<numPoints; ++i) {
        if(i==0)
          path.moveTo((float)h[i], (float)v[i]);
        else
          path.lineTo((float)h[i], (float)v[i]);
      }
      if(numPoints>0) {
        path.closePath();
        updateColor();
        g.draw(path);
      }
    }
  }
  
  public void drawPolyline(double[] h, double[] v, int numPoints) {
    int type=getContainmentType(h, v, numPoints, POLYLINE);
    if(type==FULLY_INSIDE||type==INTERSECTS) {
      int start=0;
      int end=numPoints-1;
      
      if(type==INTERSECTS) {
        // clip polylines if there is only one interval
        for(int i=0; i<numPoints-1; i++) {
          if(clipper.isLineWithin(h[i], v[i], h[i+1], v[i+1])) {
            start=i;
            break;
          }
        }
        for(int i=numPoints-1; i>start; i--) {
          if(clipper.isLineWithin(h[i], v[i], h[i-1], v[i-1])) {
            end=i;
            break;
          }
        }
        for(int i=start; i<end; i++) {
          if(!clipper.isLineWithin(h[i], v[i], h[i+1], v[i+1])) {
            start=0;
            end=numPoints-1;
            break;
          }
        }
      }
      
      /*
       if (type == INTERSECTS) {
       ACoord clipped = clipper.clipPolygon(h, v, numPoints);
       h = clipped.hv[0][0];
       v = clipped.hv[1][0];
       numPoints = h.length;
       }
       */
      
      path.reset();
      for(int i=start; i<=end; ++i) {
        if(i==start)
          path.moveTo((float)h[i], (float)v[i]);
        else
          path.lineTo((float)h[i], (float)v[i]);
      }
      if(end-start>0) {
        updateColor();
        g.draw(path);
      }
    }
  }
  
  // very similar with drawPolyline, but draw a dotted instead
  public void drawDottedPolyline(double[] h, double[] v, int numPoints) {
    int type=getContainmentType(h, v, numPoints, POLYLINE);
    if(type==FULLY_INSIDE||type==INTERSECTS) {
      int start=0;
      int end=numPoints-1;
      
      if(type==INTERSECTS) {
        // clip polylines if there is only one interval
        for(int i=0; i<numPoints-1; i++) {
          if(clipper.isLineWithin(h[i], v[i], h[i+1], v[i+1])) {
            start=i;
            break;
          }
        }
        for(int i=numPoints-1; i>start; i--) {
          if(clipper.isLineWithin(h[i], v[i], h[i-1], v[i-1])) {
            end=i;
            break;
          }
        }
        for(int i=start; i<end; i++) {
          if(!clipper.isLineWithin(h[i], v[i], h[i+1], v[i+1])) {
            start=0;
            end=numPoints-1;
            break;
          }
        }
      }
      
      path.reset();
      boolean drawFlag = false;
      for(int i=start; i<=end; ++i) {
        if(drawFlag)
          path.lineTo((float)h[i], (float)v[i]);
        else
          path.moveTo((float)h[i], (float)v[i]);
        drawFlag = !drawFlag;
      }
      if(end-start>0) {
        updateColor();
        g.draw(path);
      }
    }
    
  }
  
  public void drawSmoothPolyline(double[] h0, double[] v0, int numPoints0) {

    // For curves having less than 3 points and for picking use the normal polyline.
    if (numPoints0 < 3 || g instanceof APickingGraphics2D) {
      drawPolyline(h0, v0, numPoints0);
      return;
    }
    
    int type = getContainmentType(h0, v0, numPoints0, POLYLINE);
    if (type == FULLY_INSIDE || type == INTERSECTS) {

        GeneralPath curve = new GeneralPath();
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

        curve.moveTo(h[0], v[0]);
        for (int i=3; i<numPoints; i+=3) {
            curve.curveTo(h[i-2], v[i-2], h[i-1], v[i-1], h[i], v[i]);
        }

        updateColor();
        g.draw(curve);
    }
  }
  
  public void fillPolygon(double[] h, double[] v, int numPoints) {
    int type=getContainmentType(h, v, numPoints, POLYGON);
    if(type==FULLY_INSIDE||type==INTERSECTS) {
      if(type==INTERSECTS) {
        ACoord clipped=clipper.clipPolygon(h, v, numPoints);
        h=clipped.hv[0][0];
        v=clipped.hv[1][0];
        numPoints=h.length;
      }
      path.reset();
      
      for(int i=0; i<numPoints; ++i)
        if(i==0)
          path.moveTo((float)h[i], (float)v[i]);
        else
          path.lineTo((float)h[i], (float)v[i]);
      if(numPoints>0) {
        path.closePath();
        updateColor();
        g.fill(path);
        g.draw(path);
      }
    } else if(type==CONTAINS) {
      Rectangle bounds=g.getClipBounds();
      updateColor();
      g.fillRect((int)Math.rint(bounds.getX()), (int)Math.rint(bounds.getY()),
              (int)Math.rint(bounds.getWidth()), (int)Math.rint(bounds.getHeight()));
    }
  }
  
}
