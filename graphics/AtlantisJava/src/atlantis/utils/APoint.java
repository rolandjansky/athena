package atlantis.utils;


/**
 * Used by APolygon and AClipPolygon in order to perform clipping.
 */
public class APoint {

  public double x, y;

  public APoint(APoint p) {
    this.x=p.x;
    this.y=p.y;
  }

  public APoint(double x, double y) {
    this.x=x;
    this.y=y;
  }

  public double dot_product(double[] normPoint) {
    if(normPoint==null||normPoint.length!=3)
      return 0.0;
    else
      return x*normPoint[0]+y*normPoint[1];
  }

  public APoint minus(APoint cgp) {
    if(cgp==null)
      return new APoint(this);
    else
      return new APoint(x-cgp.x, y-cgp.y);
  }

  public APoint plus(APoint cgp) {
    if(cgp==null)
      return new APoint(this);
    else
      return new APoint(x+cgp.x, y+cgp.y);
  }

  public void scalar_multiply(double d) {
    x*=d;
    y*=d;
  }

  public boolean inside(double[] normalizedVector, double distance) {
    if(normalizedVector==null||normalizedVector.length!=3) return false;
    double k=dot_product(normalizedVector)+distance;

    return(k<=0);
  }

  public double calculate_tls(APoint secondPoint, APoint clipNode, double[] normalized) {
    APoint dv1, dv2;

    dv1=minus(clipNode);
    dv2=secondPoint.minus(this);
    return -(dv1.dot_product(normalized)/dv2.dot_product(normalized));
  }

  public APoint point_of_intersection(APoint second, double tls) {
    if(second!=null) {
      APoint v=second.minus(this);

      if(v!=null) v.scalar_multiply(tls);
      return plus(v);
    } else
      return null;
  }

}
