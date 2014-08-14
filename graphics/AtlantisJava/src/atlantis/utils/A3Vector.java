package atlantis.utils;


/**
 * Provides basic 3D vector operations.
 */

public class A3Vector {

  public double x, y, z;

  /**
   * Construct zero 3-vector.
   */
  public A3Vector() {}

  /**
   * Construct 3-vector from Cartesian coordinates.
   * @param dx
   * @param dy
   * @param dz
   */
  public A3Vector(double x, double y, double z) {
	    this.x=x;
	    this.y=y;
	    this.z=z;
  }
  
  /**
   * Create 3-vector from eta, phi and magnitude.
   * @param eta pseudorapidity along z direction
   * @param phi azimuthal angle [radians]
   * @param r   magnitude of vector
   * @return new A3Vector
   */
  public static A3Vector fromEtaPhiR(double eta, double phi, double r) {
      double theta = 2.0 * Math.atan(Math.exp(-eta));
      return new A3Vector(r * Math.sin(theta) * Math.cos(phi),
              r * Math.sin(theta) * Math.sin(phi),
              r * Math.cos(theta));
  }

  /**
   * Create 3-vector from cylindrical coordinates rho, phi, z.
   * @param rho component perpendicular to z axis
   * @param phi azimuthal angle [radians]
   * @param z
   * @return
   */
  public static A3Vector fromRhoPhiZ(double rho, double phi, double z) {
	  double x=rho*Math.cos(phi);
	  double y=rho*Math.sin(phi);
	  return new A3Vector(x,y,z);
  }
  
  public A3Vector getNormalized() {
    double r=Math.sqrt(x*x+y*y+z*z);
    return new A3Vector(x/r, y/r, z/r);
  }

  public A3Vector normalize() {
    double r=magnitude();
    x/=r;
    y/=r;
    z/=r;
    return this;
  }

  public double magnitude() {
    return Math.sqrt(x*x+y*y+z*z);
  }

  public A3Vector scale(double a) {
    x*=a;
    y*=a;
    z*=a;
    return this;
  }

  public A3Vector add(A3Vector v) {
    x+=v.x;
    y+=v.y;
    z+=v.z;
    return this;
  }

  public A3Vector subtract(A3Vector v) {
    x-=v.x;
    y-=v.y;
    z-=v.z;
    return this;
  }

  public String toString() {
    return "A3Vector[x="+x+", y="+y+", z="+z+"]";
  }

}
