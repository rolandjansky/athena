package atlantis.utils;


import java.awt.geom.Point2D;


/**
 * Provides basic 2D vector operations.
 */

public class AVector {

  public double dx, dy;

  public AVector(double dx, double dy) {
    set(dx, dy);
  }

  public AVector(double x1, double y1, double x2, double y2) {
    set(x2-x1, y2-y1);
  }

  public AVector(Point2D p1, Point2D p2) {
    this(p1.getX(), p1.getY(), p2.getX(), p2.getY());
  }

  public AVector set(double dx, double dy) {
    this.dx=dx;
    this.dy=dy;
    return this;
  }

  public AVector getUnitary() {
    double r=Math.sqrt(dx*dx+dy*dy);

    return new AVector(dx/r, dy/r);
  }

  public AVector makeUnitary() {
    double r=Math.sqrt(dx*dx+dy*dy);

    dx/=r;
    dy/=r;
    return this;
  }

  public AVector rotate(double alpha) {
    double sin=Math.sin(alpha);
    double cos=Math.cos(alpha);
    double dx_prime=dx*cos-dy*sin;
    double dy_prime=dx*sin+dy*cos;

    dx=dx_prime;
    dy=dy_prime;
    return this;
  }

  public AVector scale(double factor) {
    dx*=factor;
    dy*=factor;
    return this;
  }

  public AVector invert() {
    dx*=-1;
    dy*=-1;
    return this;
  }

  public double modulus() {
    return Math.sqrt(dx*dx+dy*dy);
  }

  public AVector add(AVector v) {
    dx+=v.dx;
    dy+=v.dy;
    return this;
  }

  public String toString() {
    return "AVector[dx="+dx+", dy="+dy+"]";
  }

}
