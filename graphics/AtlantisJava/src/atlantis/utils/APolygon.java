package atlantis.utils;


import java.util.Vector;
import java.awt.geom.*;


/**
 * Used in the process of clipping.
 */

public class APolygon {
  protected Vector nodes=new Vector(5, 10);

  public APolygon() {}

  public APolygon(double[] x, double[] y) {
    for(int i=0; i<x.length; i++)
      nodes.addElement(new APoint(x[i], y[i]));

    if(getSignedArea()<0) {
      Vector newNodes=new Vector(5, 10);

      for(int i=nodes.size()-1; i>=0; i--)
        newNodes.addElement(nodes.elementAt(i));
      nodes=newNodes;
    }
  }

  public void addPoint(double x, double y) {
    nodes.addElement(new APoint(x, y));
  }

  public double getSignedArea() {
    APoint p;
    double[] x=new double[nodes.size()];
    double[] y=new double[nodes.size()];

    for(int i=0; i<nodes.size(); i++) {
      p=(APoint)nodes.elementAt(i);
      x[i]=p.x;
      y[i]=p.y;
    }

    return getSignedArea(x, y);
  }

  // this function returns the ABSOLUT VALUE of the area
  public double getArea() {
    return Math.abs(getSignedArea());
  }

  public static double getSignedArea(double[] x, double[] y) {
    if(x.length!=y.length)
      throw new Error("x.length != y.length");
    if(x.length==0) return 0;

    double s=0;

    for(int i=0; i<x.length-1; i++)
      s+=x[i]*y[i+1]-x[i+1]*y[i];

    s+=x[x.length-1]*y[0]-x[0]*y[x.length-1];
    return s/2;
  }

  public static double getArea(double[] x, double[] y) {
    return Math.abs(getSignedArea(x, y));
  }

  public static Point2D.Double getCenter(double[] x, double[] y, Point2D.Double p) {
    if(x.length==0 || y.length==0) return null;
    if(p==null) p=new Point2D.Double(0, 0);

    double s=0, v;

    for(int i=0; i<x.length-1; i++) {
      v=x[i]*y[i+1]-x[i+1]*y[i];
      s+=v;
      p.x+=(x[i]+x[i+1])*v;
      p.y+=(y[i]+y[i+1])*v;
    }

    v=x[x.length-1]*y[0]-x[0]*y[x.length-1];
    s+=v;
    p.x+=(x[x.length-1]+x[0])*v;
    p.y+=(y[x.length-1]+y[0])*v;

    double f=1/(6*s/2);

    p.setLocation(p.x*f, p.y*f);
    return p;
  }

  public static void scale(double[] x, double[] y, double factor) {
    Point2D.Double center=APolygon.getCenter(x, y, null);

    for(int i=0; i<x.length; i++) {
      x[i]=(x[i]-center.x)*factor+center.x;
      y[i]=(y[i]-center.y)*factor+center.y;
    }
  }

  /*
   public boolean isClockwise() {
   if (getArea() < 0)
   return true;
   else
   return false;
   }
   */

  public APoint getNode(int index) {
    if(index>=0&&index<nodes.size())
      return(APoint)(nodes.elementAt(index));
    else
      return null;
  }

  public int nrNodes() {
    return nodes.size();
  }

  public boolean clip(AClipPolygon clipPolygon) {
    int i=0;
    int limit=clipPolygon.nrNodes();

    APoint node=null;
    double[] normalized=new double[3];
    double distance=0.0;

    for(i=0; i<limit; i++) {
      normalized=clipPolygon.get_normalized(i);
      node=clipPolygon.getNode(i);
      distance=clipPolygon.get_distance(i);
      if(normalized!=null&&node!=null&& !Double.isNaN(distance))
        s_h_clip_poly(normalized, node, distance);
    }

    return(nodes.size()>0);
  }

  public void s_h_clip_poly(double[] normalized, APoint node, double distance) {
    int degree=nodes.size();
    APoint s=null, p=null, q=null;
    boolean s_inside, p_inside;
    double tls=0.0;
    Vector keptNodes=new Vector();

    if(degree>0&&nodes.elementAt(degree-1) instanceof APoint)
      s=(APoint)nodes.elementAt(degree-1);

    for(int i=0; i<nodes.size(); i++) {
      if(nodes.elementAt(i) instanceof APoint) {
        p=(APoint)nodes.elementAt(i);
        s_inside=s.inside(normalized, distance);
        p_inside=p.inside(normalized, distance);
        if(s_inside)
          if(!p_inside) {
            tls=s.calculate_tls(p, node, normalized);
            q=s.point_of_intersection(p, tls);
            keptNodes.addElement(q);
          } else
            keptNodes.addElement(p);
        else if(p_inside) {
          tls=s.calculate_tls(p, node, normalized);
          q=s.point_of_intersection(p, tls);
          keptNodes.addElement(q);
          keptNodes.addElement(p);
        }
      }
      s=p;
    }
    nodes=keptNodes;
  }

  public double[][] getPolygon() {
    int nrPoints=nodes.size();
    double[][] coord=new double[nrPoints][nrPoints];

    if(nrPoints>0)
      for(int i=0; i<nrPoints; i++) {
        APoint aPoint=getNode(i);

        if(aPoint!=null) {
          coord[0][i]=aPoint.x;
          coord[1][i]=aPoint.y;
        }
      }
    return coord;
  }

  public double[] getX() {
    int nrPoints=nodes.size();
    double[] coord=new double[nrPoints];

    if(nrPoints>0)
      for(int i=0; i<nrPoints; i++) {
        APoint aPoint=getNode(i);

        if(aPoint!=null)
          coord[i]=aPoint.x;
      }
    return coord;
  }

  public double[] getY() {
    int nrPoints=nodes.size();
    double[] coord=new double[nrPoints];

    if(nrPoints>0)
      for(int i=0; i<nrPoints; i++) {
        APoint aPoint=getNode(i);

        if(aPoint!=null)
          coord[i]=aPoint.y;
      }
    return coord;
  }

  public String toString() {
    StringBuilder s = new StringBuilder();
    int limit=nodes.size();

    s.append("x:{ ");
    for(int i=0; i<limit; i++) {
      s.append(((APoint)nodes.elementAt(i)).x);
      s.append(" :: ");
    }
    s.append(" }\ny:{ ");
    for(int i=0; i<limit; i++) {
      s.append(((APoint)nodes.elementAt(i)).y);
      s.append(" :: ");
    }
    return s.toString();
  }

}
