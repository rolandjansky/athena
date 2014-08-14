package atlantis.interactions;


import atlantis.utils.AVector;
import java.awt.geom.*;


/**
 * A implementation of the java.awt.RectangularShape which
 * represents a segment of a line.
 */
public class ALineSegment extends RectangularShape implements PathIterator {

  Point2D.Double p1;
  Point2D.Double p2;
  double thickness;

  public ALineSegment(Point2D.Double p1, Point2D.Double p2, double thickness) {
    this.p1=p1;
    this.p2=p2;
    this.thickness=thickness;
  }

  public ALineSegment(double x1, double y1, double x2, double y2, double thickness) {
    this.p1=new Point2D.Double(x1, y1);
    this.p2=new Point2D.Double(x2, y2);
    this.thickness=thickness;
  }

  private Point2D.Double[] calculateNodes() {
    AVector v1=new AVector(p1, p2).makeUnitary().scale(thickness/2).rotate(Math.PI/2);
    AVector v2=new AVector(p1, p2).makeUnitary().scale(thickness/2).rotate(-Math.PI/2);

    Point2D.Double[] p=new Point2D.Double[4];

    p[0]=new Point2D.Double(p1.x+v1.dx, p1.y+v1.dy);
    p[1]=new Point2D.Double(p2.x+v1.dx, p2.y+v1.dy);
    p[2]=new Point2D.Double(p2.x+v2.dx, p2.y+v2.dy);
    p[3]=new Point2D.Double(p1.x+v2.dx, p1.y+v2.dy);

    return p;
  }

  public Rectangle2D getBounds2D() {
    Point2D.Double[] p=calculateNodes();

    double minX, minY, maxX, maxY;

    minX=maxX=p[0].x;
    minY=maxY=p[0].y;

    for(int i=1; i<p.length; i++) {
      if(p[i].x<minX) minX=p[i].x;
      if(p[i].x>maxX) maxX=p[i].x;
      if(p[i].y<minY) minY=p[i].y;
      if(p[i].y>maxY) maxY=p[i].y;
    }

    return new Rectangle2D.Double(minX, minY, maxX-minX, maxY-minY);
  }

  public boolean contains(double x, double y) {
    boolean c=false;
    Point2D.Double[] p=calculateNodes();

    for(int i=0, j=p.length-1; i<p.length; j=i++)
      if((((p[i].y<=y)&&(y<p[j].y))||((p[j].y<=y)&&(y<p[i].y)))
          &&(x<(p[j].x-p[i].x)*(y-p[i].y)/(p[j].y-p[i].y)+p[i].x)
          )  c=!c;

    return c;
  }

  public boolean intersects(double x, double y, double w, double h) {
    return false;
  }

  public boolean contains(double x, double y, double w, double h) {
    return false;
  }

  public PathIterator getPathIterator(AffineTransform at) {
    lineNumber=-1;
    return this;
  }

  public void setFrame(double x, double y, double w, double h) {}

  public boolean isEmpty() {
    return false;
  }

  public double getWidth() {
    return getBounds2D().getWidth();
  }

  public double getHeight() {
    return getBounds2D().getHeight();
  }

  public double getX() {
    return getBounds2D().getX();
  }

  public double getY() {
    return getBounds2D().getY();
  }

  // PathIterator implementation
  int lineNumber;

  // Returns the coordinates and type of the current path segment in the iteration.
  public int currentSegment(double[] coords) {
    Point2D.Double[] p=calculateNodes();

    if(lineNumber==-1) {
      coords[0]=p[0].x;
      coords[1]=p[0].y;
      return PathIterator.SEG_MOVETO;
    }

    if(lineNumber==4) return PathIterator.SEG_CLOSE;

    coords[0]=p[lineNumber].x;
    coords[1]=p[lineNumber].y;
    return PathIterator.SEG_LINETO;
  }

  public int currentSegment(float[] coords) {
    Point2D.Double[] p=calculateNodes();

    if(lineNumber==-1) {
      coords[0]=(float)p[0].x;
      coords[1]=(float)p[0].y;
      return PathIterator.SEG_MOVETO;
    }

    if(lineNumber==4) return PathIterator.SEG_CLOSE;

    coords[0]=(float)p[lineNumber].x;
    coords[1]=(float)p[lineNumber].y;
    return PathIterator.SEG_LINETO;
  }

  // Returns the winding rule for determining the interior of the path.
  public int getWindingRule() {
    return PathIterator.WIND_NON_ZERO;
  }

  public boolean isDone() {
    if(lineNumber>4)
      return true;
    else
      return false;
  }

  public void next() {
    lineNumber++;
  }

  public String toString() {
    return "ALineSegment["+p1.x+", "+p1.y+", "+p2.x+", "+p2.y+"]";
  }

}
